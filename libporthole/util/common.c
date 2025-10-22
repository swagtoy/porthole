#include "common.h"

char
_str_oneof(char src, char const *match)
{
	for (; *match; ++match)
		if (*match == src)
			return *match;
	return 0;
}
