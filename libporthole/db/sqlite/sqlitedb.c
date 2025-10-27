#include <assert.h>
#include <sqlite3.h>
#include <stdint.h>
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
#include "data/add_pkg.sql.h"

#define _DB_FILENAME "phcache.db"

enum _stmt_id
{
	STMT_BEGIN_TRANSACTION,
	STMT_COMMIT,
	STMT_CREATE_TABLES,
	STMT_INSERT_PKG,
	STMT_LEN,
};

struct _ph_database_impl
{
	sqlite3 *sdb;
	sqlite3_stmt *cached_stmts[STMT_LEN];
};

struct _bind_span
{
	enum _bind_span_type {
		_BIND_UNSET,
		_BIND_NULL,
		_BIND_INT,
		_BIND_INT64,
		_BIND_DOUBLE,
		_BIND_TEXT,
	} t;
	union {
		char     *t;
		int       i;
		uint64_t  i64;
		double    d;
	} d;
};

int
_sqlite_span_bind(sqlite3_stmt *stmt, struct _bind_span *spans, int len)
{
	for (; len > 0; --len)
	{
		struct _bind_span *span = spans + len - 1;
		switch (span->t)
		{
		case _BIND_UNSET:
			break;
		case _BIND_NULL:
			sqlite3_bind_null  (stmt, len); break;
		case _BIND_INT:
			sqlite3_bind_int   (stmt, len, span->d.i); break;
		case _BIND_INT64:
			sqlite3_bind_int64 (stmt, len, span->d.i64); break;
		case _BIND_DOUBLE:
			sqlite3_bind_double(stmt, len, span->d.d); break;
		case _BIND_TEXT:
			sqlite3_bind_text  (stmt, len, span->d.t, -1, NULL); break;
		default:
			assert(!"Unhandled bind!");
		}
	}
	return SQLITE_OK;
}

int
_sqlite_prepare_cached(ph_database_t *db,
                       enum _stmt_id id,
					   char const *sql,
					   int nbytes,
					   sqlite3_stmt **stmt,
					   char const **pzTail)
{
	struct _ph_database_impl *impl = db->_impl;
	sqlite3_stmt *cached_stmt;
	if ((cached_stmt = impl->cached_stmts[id]) != NULL)
	{
		sqlite3_reset(cached_stmt);
		*stmt = cached_stmt;
		return SQLITE_OK;
	}
	
	int ret = sqlite3_prepare_v2(impl->sdb, sql, nbytes, impl->cached_stmts + id, pzTail);
	*stmt = impl->cached_stmts[id];
	return ret;
}

int
_sqlite_simple_step_through(ph_database_t *db, sqlite3_stmt *stmt)
{
	int ret;
	struct _ph_database_impl *impl = db->_impl;
	while ((ret = sqlite3_step(stmt)) != SQLITE_DONE)
		if (ret == SQLITE_ERROR)
		{
			db->error = strdup(sqlite3_errmsg(impl->sdb));
			DEBUGF("sqlite error while stepping through: %s\n", db->error);
			return ret;
		}
	
	db->error = NULL;
	return ret;
}

int
_sqlite_simple_exec(ph_database_t *db,
                    enum _stmt_id id,
                    char const *sql,
                    int nbytes)
{
	struct _ph_database_impl *impl = db->_impl;
	sqlite3_stmt* stmt = NULL;
	int ret;
	if ((ret = _sqlite_prepare_cached(
	        db, id, sql, nbytes, &stmt, NULL)) != SQLITE_OK)
	{
		db->error = strdup(sqlite3_errmsg(impl->sdb));
		DEBUGF("error when preparing: %s\n", db->error);
		return ret;
	}
	
	if ((ret = _sqlite_simple_step_through(db, stmt)) != SQLITE_DONE)
		return ret;
	
	return ret;
}

bool
_setup_database(ph_database_t *db)
{
	if (_sqlite_simple_exec(db, STMT_CREATE_TABLES, _PH_IMPORTED_FILE(setup_sql)) != SQLITE_DONE)
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
	
	db->_impl = calloc(1, sizeof(struct _ph_database_impl));
	
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

void
ph_database_begin_transaction(ph_database_t *db)
{
#define SQL "BEGIN TRANSACTION;"
	if (_sqlite_simple_exec(db, STMT_BEGIN_TRANSACTION, SQL, sizeof(SQL)-1) != SQLITE_DONE)
	{
		DEBUGF("BEGIN TRANSACTION failed! %s\n", sqlite3_errmsg(db->_impl->sdb));
	}
#undef SQL
}

void
ph_database_commit(ph_database_t *db)
{
#define SQL "COMMIT;"
	if (_sqlite_simple_exec(db, STMT_COMMIT, SQL, sizeof(SQL)-1) != SQLITE_DONE)
	{
		DEBUG("Commit failed!\n");
	}
#undef SQL
}

bool
ph_database_add_pkg(ph_database_t *db, struct ph_db_ecache *data)
{
	struct _ph_database_impl *impl = db->_impl;
	sqlite3_stmt* stmt = NULL;
	int ret;
	if ((ret = _sqlite_prepare_cached(
	        db, STMT_INSERT_PKG, _PH_IMPORTED_FILE(add_pkg_sql), &stmt, NULL)) != SQLITE_OK)
	{
		db->error = strdup(sqlite3_errmsg(impl->sdb));
		DEBUGF("error when preparing: %s\n", db->error);
		return false;
	}
	
	struct _bind_span binds[] = {
		{ _BIND_TEXT, {.t = data->cat} }, // cat
		{ _BIND_TEXT, {.t = data->pkg} }, // pkg
		{ _BIND_TEXT, {.t = data->ver} }, // ver
		{ _BIND_TEXT, {.t = data->repo} }, // repo
		{ _BIND_INT,  {.i = data->EAPI} }, // EAPI
		{ _BIND_TEXT, {.t = data->DESCRIPTION} }, // DESCRIPTION
		{ _BIND_TEXT, {.t = data->BDEPEND} }, // BDEPEND
		{ _BIND_TEXT, {.t = data->HOMEPAGE} }, // HOMEPAGE
		{ _BIND_TEXT, {.t = data->IDEPEND} }, // IDEPEND
		{ _BIND_TEXT, {.t = data->INHERIT} }, // INHERIT
		{ _BIND_TEXT, {.t = data->IUSE} }, // IUSE
		{ _BIND_TEXT, {.t = data->KEYWORDS} }, // KEYWORDS
		{ _BIND_TEXT, {.t = data->LICENSE} }, // LICENSE
		{ _BIND_TEXT, {.t = data->RDEPEND} }, // RDEPEND
		{ _BIND_TEXT, {.t = data->REQUIRED_USE} }, // REQUIRED_USE
		{ _BIND_TEXT, {.t = data->RESTRICT} }, // RESTRICT
		{ _BIND_TEXT, {.t = data->SLOT} }, // SLOT
		{ _BIND_TEXT, {.t = data->SRC_URI} }, // SRC_URI
	};
	_sqlite_span_bind(stmt, binds, (sizeof(binds)/sizeof(binds[0])));
	
	_sqlite_simple_step_through(db, stmt);
	
	return true;
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
	{
		for (unsigned i = 0; i < STMT_LEN; ++i)
			sqlite3_finalize(db->_impl->cached_stmts[i]);
		sqlite3_close(db->_impl->sdb);
	}
	free(db->_impl);
	free(db->error);
	db->_impl      = NULL;
	db->path       = NULL;
}
