#include <stddef.h>
#include "common.h"

static bool
_is_blank(char bl)
{
	return bl == ' ' || bl == '\t';
}

char
_str_oneof(char src, char const *match)
{
	for (; *match; ++match)
		if (*match == src)
			return *match;
	return 0;
}

char *
_next_nb(char *point, int min_travel, bool *err)
{
	for (int i = 0; *point; (++i, ++point))
	{
		if (!_is_blank(*point))
		{
			if (!min_travel || i >= min_travel)
				return point;
			else
			{
				*err = true;
				return point;
			}
		}
	}
	if (*point == '\0')
		return NULL;
	return point;
}
