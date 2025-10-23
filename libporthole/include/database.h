// Licensed under the BSD 3-Clause License

#ifndef PORTHOLE_DATABASE_H
#define PORTHOLE_DATABASE_H

#include <stdbool.h>
#include "libporthole_config.h"

struct _ph_database_impl;

struct ph_database
{
	char *path;
	struct _ph_database_impl *_impl;
};
typedef struct ph_database ph_database_t;

/// Creates or opens the database if it doesn't exist
_LIBPH_PUBLIC bool ph_database_open(ph_database_t *db, char const *location);
_LIBPH_PUBLIC void ph_database_close(ph_database_t *db);

#endif
