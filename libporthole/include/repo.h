// Licensed under the BSD 3-Clause License
	
#ifndef PORTHOLE_REPO_H
#define PORTHOLE_REPO_H

#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "atom.h"

struct ph_repo_ctx_impl;

struct ph_repo
{
	char *directory;
	char *name;
};

struct ph_repo_ctx
{
	struct ph_repo *repo;
	struct _ph_repo_ctx_impl *_impl;
};

_LIBPH_PUBLIC int ph_repo_context_category_open(struct ph_repo_ctx *ctx);
_LIBPH_PUBLIC int ph_repo_context_catpkg_open(struct ph_repo_ctx *ctx, char const *cat);
_LIBPH_PUBLIC int ph_repo_context_pkg_open(struct ph_repo_ctx *ctx, char const *cat, char const *pkg);
_LIBPH_PUBLIC char *ph_repo_context_next(struct ph_repo_ctx *ctx);
_LIBPH_PUBLIC void ph_repo_context_close(struct ph_repo_ctx *ctx);

_LIBPH_PUBLIC void ph_repo_free(struct ph_repo *repo);

#endif
