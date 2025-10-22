// Licensed under the BSD 3-Clause License

#ifndef PORTHOLE_USE_H
#define PORTHOLE_USE_H
#include "common.h"
#include <stddef.h>
#include <stdbool.h>

struct ph_atom_use_dep
{
	char *use;
	char pre;
	char post;
	char use_default;
};

struct ph_atom_use_deps
{
	struct ph_atom_use_dep *deps;
	size_t deps_len;
};
typedef struct ph_atom_use_deps ph_atom_use_deps_t;

_LIBPH_PUBLIC int ph_use_deps_parse(char *atomstr, ph_atom_use_deps_t *deps);
_LIBPH_PUBLIC char *ph_use_deps_to_string(ph_atom_use_deps_t *atom);
_LIBPH_PUBLIC bool ph_use_is_valid(char *use);
_LIBPH_PUBLIC void ph_use_free(ph_atom_use_deps_t *deps);

#endif
