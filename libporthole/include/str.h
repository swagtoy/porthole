// Licensed under the BSD 3-Clause License
	
#ifndef PORTHOLE_UTIL_STR_H
#define PORTHOLE_UTIL_STR_H

#include "common.h"
#ifndef NDEBUG
#	include <stdint.h>
#endif
#include <string.h>
#include <stdbool.h>

struct string_guts
{
#ifndef NDEBUG
	uint32_t magic;
#endif
	size_t length;
};

#define STR_CONTAINS(str1, str2) (strstr(str1, str2) != NULL)

/**
 * Creates a new empty string.
 *
 * Must be free'd with str_free, not free. There is internal
 *   bookkeeping done by this string utility, which must be
 *   appropriately released.
 *
 * \return New string. NULL on error.
 */
_LIBPH_PUBLIC char *str_new();

_LIBPH_PUBLIC char *str_new_from_cstr(char const *cstr);

/**
 * Returns the length of the string.
 *
 * \remark This is not O(strlen)! Internally, this string keeps track
 *   of its own length, so calling this is cheap and easy.
 *
 * \return The pre-calculated string length.
 */
_LIBPH_PUBLIC size_t str_length(char *str);

_LIBPH_PUBLIC int str_subs(char **str, char **result, int offset, int len);
_LIBPH_PUBLIC void str_clinsert(char **str, char const *other, int len, int index);
_LIBPH_PUBLIC void str_cinsert(char **str, char const *other, int index);

_LIBPH_PUBLIC void str_remove_at(char **str, int offset, int len);

_LIBPH_PUBLIC bool str_cremove(char **str, char const *other);

_LIBPH_PUBLIC void str_cappend(char **str, char const *data);
_LIBPH_PUBLIC void str_clappend(char **str, char const *data, size_t len);
_LIBPH_PUBLIC void str_chappend(char **str, char data);

_LIBPH_PUBLIC char *str_release(char *str);

/**
 * Frees the string and it's internals.
 */
_LIBPH_PUBLIC void str_free(char *str);

#endif

