// Licensed under the BSD 3-Clause License
	
#ifndef PORTHOLE_ATOM_H
#define PORTHOLE_ATOM_H

#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "version.h"
#include "types.h"
#include "use.h"

struct ph_atom
{
	/// Comparison, set when parsing the atom string, i.e. >=, >, =
	enum ph_cmp cmp;
	/// Package category, i.e. 'app-text'
	char *category;
	/// Package name, i.e. 'gentoo-kernel-bin' or 'gnome-shell'
	char *pkgname;
	/// Version info; this is so interesting that it's split up
	ph_version_t version;
	/// Slot, i.e. '6.2' or 'whatever' (per the spec)
	char *slot;
	/// Package repository, i.e. 'gentoo' or 'some-wacky-overlay'
	char *repository;
	/// Use dependencies.
	ph_atom_use_deps_t use_deps;
	
	/// Private internal storage. Do not touch this.
	char *_storage;
};
typedef struct ph_atom ph_atom_t;
typedef uint8_t ph_atom_parse_opts_t;

/// If set, ph_atom_parse_string will also parse use dependencies
#define PH_ATOM_PARSE_DEPENDENCY 1
/// If set, ph_atom_parse_string will strip .ebuild from the end
#define PH_ATOM_PARSE_STRIP_EBUILD (1<<1)
/// If set, ph_atom_parse_string will not bother with ::repo (likely error)
#define PH_ATOM_PARSE_NO_REPO (1<<2)
/// If set, ph_atom_parse_string will only parse ::repo after usedeps (i.e. [one?]::repo)
#define PH_ATOM_PARSE_REPO_AFTER_USEDEP (1<<3)
/// If set, ph_atom_parse_string will align with the PMS
#define PH_ATOM_PARSE_PMS_CORRECT \
	(PH_ATOM_PARSE_DEPENDENCY | PH_ATOM_PARSE_NO_REPO | PH_ATOM_PARSE_DEPENDENCY)

_LIBPH_PUBLIC int ph_atom_parse_string(char const *atomstr, ph_atom_t *atom, ph_atom_parse_opts_t opts);
_LIBPH_PUBLIC bool ph_atom_str_is_valid(char *atomstr);
_LIBPH_PUBLIC bool ph_atom_is_valid(ph_atom_t *atom);
_LIBPH_PUBLIC bool ph_atom_category_is_valid(char *category);
_LIBPH_PUBLIC bool ph_atom_pkgname_is_valid(char *pkgname);
_LIBPH_PUBLIC char * ph_atom_to_string(ph_atom_t *atom);

_LIBPH_PUBLIC void ph_atom_free(ph_atom_t *atom);

#endif
