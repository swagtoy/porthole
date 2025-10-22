// Licensed under the BSD 3-Clause License
	
#ifndef PORTHOLE_VERSION_H
#define PORTHOLE_VERSION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "types.h"

enum ph_version_suffix
{
	PH_S_UNSET = 0,
	PH_S_ALPHA,
	PH_S_BETA,
	PH_S_PRE,
	PH_S_RC,
	PH_S_P,
};

struct ph_suffix
{
	enum ph_version_suffix suffix;
	long number;
};

struct ph_version
{
	/// Package version
	char *version;
	char version_suffix;
	struct ph_suffix *suffixes;
	size_t suffixes_len;
	unsigned long rev;
};
typedef struct ph_version ph_version_t;

// Note: atomstr will be tarnished, but it's mainly used by the atom 
_LIBPH_PUBLIC int ph_version_parse_string(char *verstr, ph_version_t *vers);

_LIBPH_PUBLIC void ph_version_free(ph_version_t *vers);

#endif
