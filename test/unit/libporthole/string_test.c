#include <assert.h>
#include <stdio.h>
#include <str.h>

#define PRINT_STRING(_string) printf("%.*s\n", (int)str_length(_string), _string)

int
main()
{
	char *string = str_new();
	assert(str_length(string) == 0);

	// str_cinsert	
	str_cinsert(&string, "test", 999);
	PRINT_STRING(string);
	str_cinsert(&string, "begin", 0);
	PRINT_STRING(string);
	str_cinsert(&string, " the ", 5);
	PRINT_STRING(string);
	str_cinsert(&string, "... over!", -1);
	PRINT_STRING(string);
	str_cinsert(&string, " I repeat, over!", -1);
	PRINT_STRING(string);
	
	// str_subs
	char *subs = NULL;
	str_subs(&string, &subs, 2, 3);
	PRINT_STRING(subs);
	str_free(subs);
	subs = NULL;
	
	str_subs(&string, &subs, 2, 9999);
	PRINT_STRING(subs);
	str_free(subs);
	subs = NULL;
	
	// str_remove_at, str_cremove
	str_remove_at(&string, 17, 6);
	PRINT_STRING(string);
	str_cremove(&string, "I repeat, ");
	PRINT_STRING(string);
	
	// str_new_from_cstr
	char *string2 = str_new_from_cstr("Hello world!");
	str_cremove(&string2, "Hello");
	str_cinsert(&string2, "Goodbye", 0);
	PRINT_STRING(string2);
	
	str_free(string2);
	str_free(string);
	return 0;
}
