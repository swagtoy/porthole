#include <stddef.h>
#include "database.h"

int
main()
{
	ph_database_t db;
	ph_database_open(&db, NULL);
	
	struct ph_db_ecache record = {
		.cat = "hello",
		.pkg = "hi",
		.ver = "1.0",
		.repo = "gentoo",
	};
	ph_database_add_pkg(&db, &record);
	
	ph_database_close(&db);
	
	return 0;
}
