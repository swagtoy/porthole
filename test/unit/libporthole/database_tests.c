#include <stddef.h>
#include "database.h"

int
main()
{
	ph_database_t db;
	ph_database_open(&db, NULL);
	
	ph_database_close(&db);
	
	return 0;
}
