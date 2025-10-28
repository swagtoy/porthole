// Licensed under the BSD 3-Clause License
	
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#define _XOPEN_SOURCE 500
#include <string.h>
#include "str.h"

#define STRING_GUTS_MAGIC 0xFEB14

static struct string_guts *
_get_str_guts(char *presumed_string)
{
	// TODO consider alignment and whatever
	struct string_guts *guts =
		(struct string_guts *)(presumed_string - sizeof(struct string_guts));
	assert(guts->magic == STRING_GUTS_MAGIC);
	return guts;
}

static struct string_guts *
_realloc_str(char *str, size_t size)
{
	return realloc(_get_str_guts(str), sizeof(struct string_guts) + size);
}

static void
_str_resize(char **str, int size)
{
	struct string_guts *guts = _realloc_str(*str, size + 1);
	guts->length = size;
	(*str)[guts->length] = '\0';
	*str = (char *)(guts + 1);
}

static void
_string_guts_init(struct string_guts *guts)
{
#ifndef NDEBUG
	guts->magic = STRING_GUTS_MAGIC;
#endif
}

char *
str_new()
{
	struct string_guts *str = calloc(1, sizeof(struct string_guts) + 1);
	_string_guts_init(str);
	return (char *)(str + 1);
}

char *
str_new_from_cstr(char const *cstr)
{
	if (cstr == NULL)
		return str_new();
	size_t len = strlen(cstr);
	struct string_guts *str = calloc(1, sizeof(struct string_guts) + len + 1);
	_string_guts_init(str);
	str->length = len;
	
	char *res = (char *)(str + 1);
	memcpy(res, cstr, len);
	return res;
}

size_t
str_length(char *str)
{
	return _get_str_guts(str)->length;
}

static void
str_clinsert(char **str, char const *other, int len, int index)
{
	assert(other != NULL);
	int olen = str_length(*str);
	if (index > olen || index < 0)
		index = olen;
	struct string_guts *guts = _realloc_str(*str, olen + len + 1);
	*str = (char*)(guts + 1);
	
	// Shift over
	memmove(*str + index + len, *str + index, olen - index);
	// ... and copy in its place
	memcpy(*str + index, other, len);
	
	guts->length = olen + len;
	// Top off with a null terminator for usage with C functions
	(*str)[guts->length] = '\0';
}

int
str_subs(char **str, char **result, int offset, int len)
{
	int olen = str_length(*str);
	if (offset < 0 && (offset = olen - (offset)) < 0)
		goto err;
	*result = str_new();
	// Not really reasonable, but we have to create a new string anyway
	if (offset >= olen || len == 0)
	{
		return offset;
	}
	// Truncate length
	if (offset + len >= olen)
		len = olen - offset;
	
	_str_resize(result, len);
	memcpy(*result, *str + offset, len);
	return offset;
err:
	*result = NULL;
	return -1;
}

void
str_cinsert(char **str, char const *other, int index)
{
	str_clinsert(str, other, strlen(other), index);
}

void
str_cappend(char **str, char const *data)
{
	str_cinsert(str, data, -1);
}

void
str_chappend(char **str, char ch)
{
	str_clinsert(str, &ch, 1, -1);
}

char *
str_release(char *str)
{
	// TODO: Could probably do some fancy resize realloc here, but
	//   it's 5am so i'm gonna do a good old fashioned dupe and noop
	char *released = strdup(str);
	str_free(str);
	return released;
}

void
str_remove_at(char **str, int offset, int len)
{
	int olen = str_length(*str);
	// TODO checks
	
	memmove(*str + offset, *str + offset + len, olen - (offset + len));
	_get_str_guts(*str)->length = olen - len;
	(*str)[_get_str_guts(*str)->length] = '\0';
}

bool
str_cremove(char **str, char const *other)
{
	char *find = strstr(*str, other);
	if (!find)
		return false;
	str_remove_at(str, find - *str, strlen(other));
	return true;
}

void
str_free(char *str)
{
	if (str == NULL)
		return;
	struct string_guts *guts = _get_str_guts(str);
	free(guts);
}
