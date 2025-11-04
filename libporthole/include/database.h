// Licensed under the BSD 3-Clause License

#ifndef PORTHOLE_DATABASE_H
#define PORTHOLE_DATABASE_H

#include <stdbool.h>
#include "atom.h"
#include "libporthole_config.h"
#include "dbcommon.h"

struct _ph_database_impl;

struct ph_database
{
	char *path;
	char *error;
	struct _ph_database_impl *_impl;
};
typedef struct ph_database ph_database_t;

/// Creates or opens the database if it doesn't exist
_LIBPH_PUBLIC bool ph_database_open(ph_database_t *db, char const *location);
_LIBPH_PUBLIC char *ph_database_get_error(ph_database_t *db);
_LIBPH_PUBLIC void ph_database_close(ph_database_t *db);

// Common queries/operations
_LIBPH_PUBLIC void ph_database_begin_transaction(ph_database_t *db);
_LIBPH_PUBLIC void ph_database_commit(ph_database_t *db);
_LIBPH_PUBLIC bool ph_database_add_pkg(ph_database_t *db, struct ph_common_ecache *data);
_LIBPH_PUBLIC bool ph_database_pkg_exists(ph_database_t *db, ph_atom_t *atom);



#endif
