#include "dbcommon.h"
#include "str.h"
#include <stdlib.h>

void
ph_common_ecache_free(struct ph_common_ecache *ecache, bool only_ebuildvars)
{
	if (!ecache->alloc_bit)
		return;
	
	if (!only_ebuildvars)
	{
		str_free(ecache->cat);
		str_free(ecache->pkg);
		str_free(ecache->ver);
		str_free(ecache->repo);
	}
	
	str_free(ecache->DESCRIPTION);
	str_free(ecache->BDEPEND);
	str_free(ecache->HOMEPAGE);
	str_free(ecache->IDEPEND);
	str_free(ecache->INHERIT);
	str_free(ecache->IUSE);
	str_free(ecache->KEYWORDS);
	str_free(ecache->LICENSE);
	str_free(ecache->RDEPEND);
	str_free(ecache->REQUIRED_USE);
	str_free(ecache->RESTRICT);
	str_free(ecache->SLOT);
	str_free(ecache->SRC_URI);
}
