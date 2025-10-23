#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
#	include <linux/limits.h>
#else
#	warning "No PATH_MAX detected, using a hardcoded guestimate of 2048"
#	define PATH_MAX 2048
#endif
#include "database.h"
#include "common.h"

#define _DB_FILENAME "phcache.db"

struct _ph_database_impl
{
	sqlite3* sdb;
};

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
	
	return true;
err:
	//db->_impl->sdb = NULL;
	ph_database_close(db);
	return false;
}

void
ph_database_close(ph_database_t *db)
{
	free(db->path);
	if (db->_impl)
		sqlite3_close(db->_impl->sdb);
	free(db->_impl);
	db->_impl      = NULL;
	db->path       = NULL;
}
