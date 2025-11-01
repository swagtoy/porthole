// Licensed under the BSD 3-Clause License
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cache_spawner.h"
#include "atom.h"
#include "database.h"
#include "ebuild.h"
#include "repo.h"
#include "str.h"


/* We have to cap this, otherwise the piece of shit known as 'bash'
 *   will shit itself when there is too much data on stdin. I don't
 *   know what bash is doing but it will slow down to a crawl or even
 *   appear to genuinely freeze if too much data is on stdin. The
 *   initial design I wanted for this was to just fill up bashes stdin
 *   until write starts to block, then bash would do its job and we'd
 *   move on with our lives, but sadly that won't be the case. See
 *   _work_loop to get an idea of what we do.
 */
#define _MAX_EBUILDS (300)

struct _phcache_item
{
	ph_atom_t atom;
	struct _phcache_item *next;
};

struct _phcache_proc_data
{
	struct _phcache_item *list;
	struct _phcache_item *last;
	size_t len;
};

static unsigned long count = 0;
static size_t curr_proc_idx = 0;

static struct _phcache_item *
_phcache_list_push(struct _phcache_proc_data *data)
{
	struct _phcache_item *res;
	++data->len;
	if (data->list == NULL)
		data->list = res = calloc(1, sizeof(struct _phcache_item));
	else
	{
		data->last->next = res = calloc(1, sizeof(struct _phcache_item));
	}
	
	data->last = res;
	return res;
}

static void
_phcache_list_remove(struct _phcache_proc_data *data)
{
	// We specifically assert this case because it should never happen
	assert(data->list != NULL);
	
	// free it
	struct _phcache_item *it = data->list;
	ph_atom_free(&it->atom);
	data->list = it->next;
	--data->len;
	free(it);
}

static void
_next_job(long jobs)
{
	curr_proc_idx = (curr_proc_idx + 1 >= jobs) ? (0) : (curr_proc_idx + 1);
}

/* We want to avoid a situation where we sit on one process all day
 *   and forget to fill the others, so we essentially push an ebuild,
 *   but we don't actually read its results until we get N amount of
 *   queue'd ebuilds in the proc_data. If all processes are full, we
 *   instead just spinlock until a hole opens up, however, the higher
 *   N is, the less likely this is to happen. We'd prefer for
 *   spinlocking to not happen at all, and to instead just keep
 *   ourselves busy.
 */
void
_work_loop(ph_database_t *db,
           struct ph_ebuild_proc *procs,
           long jobs,
           char const *repo,
           char *pending_ebuild,
           char *pkg,
           char *ebuild)
{
	long left = 0;
	/* If pending_ebuild isn't set, assume we are done iterating
	 * through everything and just cleanup
	 */
	// XXX: this is awful and it seems that it doesnt always end cleanly. Ouch
	if (!pending_ebuild)
	{
		for (long i = 0; i < jobs; ++i)
		{
			struct ph_ebuild_proc *proc = procs + curr_proc_idx;
			struct _phcache_proc_data *pdata = proc->data;
			left += pdata->len;
		}
	}
	
	while (true)
	{
		struct ph_ebuild_proc *proc = procs + curr_proc_idx;
		struct _phcache_proc_data *pdata = proc->data;
		
		if (pdata->len < _MAX_EBUILDS && pending_ebuild)
		{
			struct _phcache_item *item = _phcache_list_push(pdata);
			if (ph_atom_parse_string(pending_ebuild, &item->atom, PH_ATOM_PARSE_STRIP_EBUILD) != 0)
			{
				fprintf(stderr, "Couldn't parse ebuild: %s\n", pending_ebuild);
				//continue;
			}
			ph_ebuild_proc_push_ebuild(proc, item->atom.category, /* item->atom.pkgname */ pkg, ebuild);
			
			_next_job(jobs);
			break;
		}
		else if (ph_ebuild_proc_data_is_ready(proc) && pdata->len) {
			struct _phcache_item *item = pdata->list;

			struct ph_common_ecache record = {
				.cat = item->atom.category,
				.pkg = item->atom.pkgname,
				.ver = item->atom.version.version,
				.repo = repo,
			};

			ph_ebuild_proc_read_ecache_vars(proc, &record);
			ph_database_add_pkg(db, &record);
			_phcache_list_remove(pdata);
			++count;
			if (!pending_ebuild && (--left <= 0))
				return;
			
			if ((count % 500) == 0)
			{
				fputs(".", stdout);
				fflush(stdout);
			}
			
			// We aren't gonna break out yet, still have a pending ebuild to deal with
		}
		
		_next_job(jobs);
	}
}

bool
phcache_gen_cache(char const *repo_arg, long jobs)
{
	if (strlen(repo_arg) == 0)
		return false;
	char buf[1024] = {0};
	char *repopath = str_new_from_cstr("/var/db/repos/");
	str_cappend(&repopath, repo_arg);
	struct ph_repo repo = { .directory = repopath };
	
	struct ph_repo_ctx ctx = { &repo }, ctx2 = { &repo }, ctx3 = { &repo };
	char *cat, *pkg, *ebuild;
	
	// create processes
	struct ph_ebuild_proc *procs = calloc(1, sizeof(struct ph_ebuild_proc) * jobs);
	for (long i = 0; i < jobs; ++i)
	{
		procs[i].data = calloc(1, sizeof(struct _phcache_proc_data));
		ph_ebuild_proc_spawn(procs + i, repopath);
	}
	
	ph_database_t db;
	ph_database_open(&db, NULL);
	ph_database_begin_transaction(&db);
	
	fputs("Updating database...", stdout);
	fflush(stdout);
	
	// categories
	ph_repo_context_category_open(&ctx);
	while ((cat = ph_repo_context_next(&ctx)))
	{
		// packages
		ph_repo_context_catpkg_open(&ctx2, cat);
		while ((pkg = ph_repo_context_next(&ctx2)))
		{
			// ebuilds
			ph_repo_context_pkg_open(&ctx3, cat, pkg);
			while ((ebuild = ph_repo_context_next(&ctx3)))
			{
				// HACK to not strdup category a billion times
				//  well, it also validates the category for us, so...
				snprintf(buf, 1023, "%s/%s", cat, ebuild); 
				
				// NOTE: is pkg needed here? i could've sworn that the
				//   PMS said that it didnt need to match the folder,
				//   but too lazy to check.
				_work_loop(&db, procs, jobs, repo_arg, buf, pkg, ebuild);
				
			}
			ph_repo_context_close(&ctx3);
		}
		ph_repo_context_close(&ctx2);
	}
	ph_repo_context_close(&ctx);
outer:

	/* for (int i = 0; i < jobs; ++i) */
	/* { */
	/* 	struct _phcache_proc_data *data = procs[i].data; */
	/* 	_flush_pending_results(&db, procs + i, repo_arg); */
	/* 	DEBUGF("Packages left: %ld\n", data->len); */
	/* } */
	
	_work_loop(&db, procs, jobs, repo_arg, NULL, NULL, NULL);
	
	puts("");
	printf("Cached %ld packages!\n", count);
	
	ph_database_commit(&db);
	
out:
	for (long i = 0; i < jobs; ++i)
	{
		free(procs[i].data);
		ph_ebuild_proc_stop(procs + i);
	}
	str_free(repopath);
	return true;
}
