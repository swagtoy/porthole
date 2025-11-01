// Licensed under the BSD 3-Clause License

#ifndef PORTHOLE_DBCOMMON_H
#define PORTHOLE_DBCOMMON_H

#include <stdbool.h>
#include "common.h"

struct ph_common_ecache
{
	unsigned long id;
	char const *cat, *pkg, *ver, *repo;
	
	unsigned long EAPI;
	char *DESCRIPTION, *BDEPEND, *HOMEPAGE,
	     *IDEPEND, *INHERIT, *IUSE, *KEYWORDS,
		 *LICENSE, *RDEPEND, *REQUIRED_USE, *RESTRICT,
		 *SLOT, *SRC_URI;
	unsigned char alloc_bit : 1;
};

_LIBPH_PUBLIC void ph_common_ecache_free(struct ph_common_ecache *ecache, bool only_ebuildvars);

#endif
