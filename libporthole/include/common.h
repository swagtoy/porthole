#ifndef LIBPORTHOLE_COMMON_H
#define LIBPORTHOLE_COMMON_H

#include <stdbool.h>
#include "libporthole_config.h"

#ifdef EXTRA_DEBUG
/* Note: It's probably good practice to remove _noisy_ DEBUGF's when
 *   you're done. Minor ones are fine to keep around. This at least
 *   ensures that <stdio.h> won't be accidentally included.
 */
#	include <stdio.h>
#	define DEBUGF(msg, ...) printf( "DBG:" __FILE__ ":%d: " msg, __LINE__, __VA_ARGS__)
#else
#	define DEBUGF(msg, ...) ;
#endif

_LIBPH_PRIVATE char _str_oneof(char source, char const *match);
_LIBPH_PRIVATE char *_next_nb(char *point, int min_travel, bool *err);

#endif
