#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#ifndef PATH_MAX
#	warning "No PATH_MAX detected, using a hardcoded guestimate of 2048"
#	define PATH_MAX 2048
#endif
#include "database.h"
#include "common.h"
#include "data/setup.sql.h"

#define _DB_FILENAME "phcache.db"

struct _ph_database_impl
{
	sqlite3 *sdb;
};

int
_sqlite_simple_exec(ph_database_t *db,
                    char const *sql,
					int nbytes)
{
	struct _ph_database_impl *impl = db->_impl;
	sqlite3 *sdb = impl->sdb;
	sqlite3_stmt* stmt = NULL;
	int ret;
	if ((ret = sqlite3_prepare_v2(sdb, sql, nbytes, &stmt, NULL)) != SQLITE_OK)
	{
		db->error = strdup(sqlite3_errmsg(impl->sdb));
		DEBUGF("error when preparing: %s\n", db->error);
		return ret;
	}
	
	while ((ret = sqlite3_step(stmt)) != SQLITE_DONE)
		if (ret == SQLITE_ERROR)
		{
			db->error = strdup(sqlite3_errmsg(impl->sdb));
			DEBUGF("sqlite error: %s\n", db->error);
			goto err;
		}
	
	db->error = NULL;
err:
	sqlite3_finalize(stmt);
	return ret;
}

bool
_setup_database(ph_database_t *db)
{
	struct _ph_database_impl *impl = db->_impl;
	sqlite3 *sdb = impl->sdb;
	
	if (_sqlite_simple_exec(db, _PH_IMPORTED_FILE(setup_sql)) != SQLITE_DONE)
	{
		DEBUG("Setting up database failed!\n");
		return false;
	}
	
	return true;
}

bool
ph_database_open(ph_database_t *db, char const *location)
{
	char path[PATH_MAX] = { 0 };
	if (!location || strcmp(location, PH_CACHE_DIR) != 0)
	{
		location = PH_CACHE_DIR;
		db->path = NULL;
	}
	else
		db->path = strdup(location);
	snprintf(path, PATH_MAX-1, "%s/" _DB_FILENAME, location);
	
	db->_impl = malloc(sizeof(struct _ph_database_impl));
	
	DEBUGF("db path: %s\n", path);
	if (sqlite3_open(path, &db->_impl->sdb) != SQLITE_OK)
	{
		DEBUGF("error when creating db: %s\n", sqlite3_errmsg(db->_impl->sdb));
		goto err;
	}
	
	_setup_database(db);
	
	return true;
err:
	//db->_impl->sdb = NULL;
	ph_database_close(db);
	return false;
}

char *
ph_database_get_error(ph_database_t *db)
{
	if (!db) return NULL;
	return db->error;
}

void
ph_database_close(ph_database_t *db)
{
	free(db->path);
	if (db->_impl)
		sqlite3_close(db->_impl->sdb);
	free(db->_impl);
	free(db->error);
	db->_impl      = NULL;
	db->path       = NULL;
}
