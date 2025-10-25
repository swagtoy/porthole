// Fur d_type
#define _DEFAULT_SOURCE
#include <assert.h>
#include <stdio.h>
#include <limits.h>
#ifndef PATH_MAX
#	warning "No PATH_MAX detected, using a hardcoded guestimate of 2048"
#	define PATH_MAX 2048
#endif
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include "atom.h"
#include "str.h"
#include "repo.h"

enum _ph_repo_ctx_type
{
	CTX_CATEGORY,
	CTX_CATPKG,
	CTX_EBUILDS,
};

struct _ph_repo_ctx_impl
{
	enum _ph_repo_ctx_type type;
	
	struct dirent *ent;
	DIR *dir;
};

static void
_create_context(struct ph_repo_ctx *ctx)
{
	free(ctx->_impl);
	ctx->_impl = malloc(sizeof(struct _ph_repo_ctx_impl));
}

int
ph_repo_context_category_open(struct ph_repo_ctx *ctx)
{
	_create_context(ctx);
	struct ph_repo *repo = ctx->repo;
	struct _ph_repo_ctx_impl *impl = ctx->_impl;
	impl->type = CTX_CATEGORY;
	
	impl->dir = opendir(repo->directory);
	return impl->dir ? 0 : -1;
}

int
ph_repo_context_catpkg_open(struct ph_repo_ctx *ctx, char const *cat)
{
	_create_context(ctx);
	struct ph_repo *repo = ctx->repo;
	struct _ph_repo_ctx_impl *impl = ctx->_impl;
	impl->type = CTX_CATPKG;
	
	char path[PATH_MAX] = { 0 };
	snprintf(path, PATH_MAX-1, "%s/%s", repo->directory, cat);
	
	impl->dir = opendir(path);
	return impl->dir ? 0 : -1;
}

int
ph_repo_context_pkg_open(struct ph_repo_ctx *ctx, char const *cat, char const *pkg)
{
	_create_context(ctx);
	struct ph_repo *repo = ctx->repo;
	struct _ph_repo_ctx_impl *impl = ctx->_impl;
	impl->type = CTX_EBUILDS;
	
	char path[PATH_MAX] = { 0 };
	snprintf(path, PATH_MAX-1, "%s/%s/%s", repo->directory, cat, pkg);
	
	impl->dir = opendir(path);
	return impl->dir ? 0 : -1;
}

char *
ph_repo_context_next(struct ph_repo_ctx *ctx)
{
	struct ph_repo *repo = ctx->repo;
	struct _ph_repo_ctx_impl *impl = ctx->_impl;

	char *str = NULL;
	struct dirent *ent;
	while ((ent = readdir(impl->dir)))
	{
		switch (impl->type)
		{
		case CTX_CATEGORY:
			// TODO thorough checking
			// TODO: Use stat?
			if (ent->d_type == DT_DIR &&
			    strcmp(ent->d_name, "profiles") != 0 &&
			    strcmp(ent->d_name, "scripts")  != 0 &&
			    strcmp(ent->d_name, "eclass")   != 0 &&
			    strcmp(ent->d_name, "metadata") != 0 &&
			    strcmp(ent->d_name, "licenses") != 0 &&
			    ph_atom_category_is_valid(ent->d_name))
			{
				impl->ent = ent;
				return ent->d_name;
			}
			break;
		case CTX_CATPKG:
			// TODO: thorough checking
			if (ent->d_type == DT_DIR &&
			    ph_atom_pkgname_is_valid(ent->d_name))
			{
				impl->ent = ent;
				return ent->d_name;
			}
			break;
		case CTX_EBUILDS:
			if (ent->d_type != DT_DIR &&
			    strstr(ent->d_name, ".ebuild"))
			{
				impl->ent = ent;
				return ent->d_name;
			}
			break;
		default:
			assert(!"Shouldn't be here!");
			break;
		}
	}
	
	return NULL;
}

void
ph_repo_context_close(struct ph_repo_ctx *ctx)
{
	struct ph_repo *repo = ctx->repo;
	struct _ph_repo_ctx_impl *impl = ctx->_impl;
	
	closedir(impl->dir);
	free(impl);
}

void
ph_repo_free(struct ph_repo *repo)
{
}
