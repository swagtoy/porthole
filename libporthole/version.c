// Licensed under the BSD 3-Clause License
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "version.h"
#include "str.h"
#include "common.h"

static char const *_suffixes[] = {
	"alpha", "beta", "pre", "rc", "p"
};
#define _SUFFIXES_LEN 5

char *
_parse_version(char *ver, char const *delims)
{
	char *c = NULL;
	char last = 0;
	if (!ver) return NULL;
	if (!isdigit(*ver)) return NULL;
	for (c = ver; *c; (last = *c, ++c))
	{
		// check if we ended at 1.123.4a (and not 1.234.a)
		if (last != 0 && isdigit(last) && islower(*c))
			break;
		// check delims
		for (char const *delim = delims; *delim; ++delim)
			if (*c == *delim)
			{
				// make sure we dont end on 1.-
				if (last != 0 && isdigit(last))
					goto out;
				else
					return NULL;
			}
		if (!(isdigit(*c) || (*c == '.' && last != '.')))
			return NULL;
	}
out:	
	return c;
}

int
ph_version_parse_string(char *verstr, ph_version_t *vers)
{
	struct ph_suffix *suffixes = NULL;
	char *tmp;
	char *end;
	bool is_suffix = false;
	
	if ((tmp = _parse_version(verstr, "-_")) == NULL)
		goto err;
	
	DEBUGF("stopped at: %c\n", *tmp);
	if (islower(*tmp))
	{
		// prevent cases like 3.0aaaaaa
		switch (tmp[1])
		{
			case '\0': case '-': case ':': case '_': break;
			default: goto err;
		}
		vers->version_suffix = *tmp;
		*tmp = '\0';
		++tmp;
		is_suffix = (*tmp == '_');
	}
	else {
		vers->version_suffix = 0;
		is_suffix = (*tmp == '_');
		*tmp = '\0';
	}
	DEBUGF("at suffix? %d\n", is_suffix);
	
	vers->version = verstr;
	verstr = tmp + 1;
	DEBUGF("verstr with revision or parse: %s\n", verstr);
	// verstr == alpha or r(4)
	
	// parse suffix (like _alpha4_beta2_p20230101
	if (is_suffix)
	{
		size_t i = 0;
		bool found, finished = false;
		for (i = 0; !finished; ++i)
		{
			found = false;
			suffixes = realloc(suffixes, sizeof(struct ph_suffix) * (i+1));

			for (size_t j = 0; j < _SUFFIXES_LEN; ++j)
			{
#define _SUFFIX_STRLEN ((_SUFFIXES_LEN) - j)				
				// Yes, (_SUFFIXES_LEN - j) works to get the string length... 
				if (strncmp(verstr, _suffixes[j], _SUFFIX_STRLEN) == 0)
				{
					// TODO this could be cleaned up
					found = true;
					DEBUGF("<<%s\n", verstr);
					verstr += _SUFFIX_STRLEN;
					DEBUGF(">>%s\n", verstr);
					long suffixnum = strtol(verstr, &end, 10);
					if (suffixnum < 0)
						goto errsuffix;
					// -1 == unset
					if (end == verstr) suffixnum = -1;
					if (end != verstr && *end != '\0')
					{
						verstr = end + 1;
					}
					else if (*end == '-') // -r4 -> r4
						verstr += 1;
					if (*verstr == '_')
						verstr = end + 1;
					// Break out and finish
					if (*end != '_')
						finished = true;
					suffixes[i].number = suffixnum;
					suffixes[i].suffix = j;
					break;
				}
			}
			
			if (!found)
				goto errsuffix;
		}
		vers->suffixes = suffixes;
		vers->suffixes_len = i;
	}
	else {
		vers->suffixes = 0;
		vers->suffixes_len = 0;
	}
	
	// parse revision
	if (*verstr == 'r')
	{
		long rev = strtol(verstr + 1, &end, 10);
		if (rev < 0 || !end || end == (verstr + 1) || *end != '\0')
			goto err;
		vers->rev = rev;
	}
	else
		vers->rev = 0;
	
	return 0;
errsuffix:
	free(suffixes);
err:
	vers->version = NULL;
	vers->rev = 0;
	vers->suffixes = NULL;
	return -1;
}

void
ph_version_free(ph_version_t *vers)
{
	free(vers->suffixes);
}
