#include <assert.h>
#include <stdio.h>
#include <repo.h>
#include "atom.h"
#include "database.h"
#include "ebuild.h"

int
main()
{
	char buf[512] = {0};
	struct ph_repo repo = {
		.directory = "/var/db/repos/gentoo"
	};
	struct ph_repo_ctx ctx = { &repo };
	struct ph_ebuild_proc proc = { 0 };
	assert(ph_ebuild_proc_spawn(&proc, "/var/db/repos/gentoo") == true);
	
	ph_database_t db;
	ph_database_open(&db, NULL);
	ph_database_begin_transaction(&db);
	
	ph_repo_context_category_open(&ctx);
	char *cat;
	while ((cat = ph_repo_context_next(&ctx)))
	{
		struct ph_repo_ctx ctx2 = { &repo };
		char *pkg;
		ph_repo_context_catpkg_open(&ctx2, cat);
		while ((pkg = ph_repo_context_next(&ctx2)))
		{
			struct ph_repo_ctx ctx3 = { &repo, 0 };
			char *ebuild;
			ph_repo_context_pkg_open(&ctx3, cat, pkg);
			while ((ebuild = ph_repo_context_next(&ctx3)))
			{
				snprintf(buf, 511, "%s/%s::gentoo", cat, ebuild);
				// parse ebuild filename now!
				ph_atom_t atom;
				char *atomstr;
				if (ph_atom_parse_string(buf, &atom, PH_ATOM_PARSE_STRIP_EBUILD) != 0)
					fprintf(stderr, "Couldn't parse ebuild: %s\n", buf);
				
				struct ph_common_ecache record = {
					.cat = atom.category,
					.pkg = atom.pkgname,
					.ver = atom.version.version,
					.repo = atom.repository,
				};
				
				ph_ebuild_proc_push_ebuild(&proc, cat, pkg, ebuild);
				ph_ebuild_proc_read_ecache_vars(&proc, &record);
				
				ph_database_add_pkg(&db, &record);
				ph_common_ecache_free(&record, 1);
			}
			ph_repo_context_close(&ctx3);
		}
		ph_repo_context_close(&ctx2);
	}
	ph_repo_context_close(&ctx);
	
	ph_database_commit(&db);
	ph_database_close(&db);
	ph_ebuild_proc_stop(&proc);
	ph_ebuild_proc_wait(&proc);
	
	return 0;
}
