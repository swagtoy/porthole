#include <assert.h>
#include <stdio.h>
#include "common.h"

static bool
_is_blanker(char bl)
{
	return bl == ' ' || bl == '\t' || bl == '\0';
}

int
main()
{
	bool err;
	char *test_string = "hey   there!       Uh wow";
	
	char *point = test_string;
	while ((point = _next_nb(point, 0, &err)))
	{
		printf("%s\n", point);
		while (!_is_blanker(*point))
			 ++point;
	}
	puts("");
	return 0;
}
