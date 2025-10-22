// Licensed under the BSD 3-Clause License

#ifndef PORTHOLE_TYPES_H
#define PORTHOLE_TYPES_H

#include "common.h"
enum ph_cmp {
	PH_CMP_BAD = -1,
	PH_CMP_UNDEF = 0,
	PH_CMP_EQ,
	PH_CMP_GE,
	PH_CMP_LE,
	PH_CMP_GT,
	PH_CMP_LT,
};

_LIBPH_PUBLIC char const *ph_cmp_to_string(enum ph_cmp cmp);

#endif
