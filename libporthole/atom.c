// Licensed under the BSD 3-Clause License
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "atom.h"
#include "str.h"
#include "use.h"
#include "version.h"

// Shared between category name and package name
#define COMMON_CHAR_CHECK(ch) (isalnum(ch) || ch == '+' || ch == '_' || ch == '-')

static bool
_numeric_until_end(char const *str)
{
	for (char const *s = str; *s; ++s)
		if (!isdigit(*s))
			return false;
	
	return true;
}

static enum ph_cmp
_parse_cmp(char const *cmp)
{
	if (cmp == NULL) return PH_CMP_BAD;
	switch (cmp[0])
	{
	case '\0': return PH_CMP_UNDEF;
	case '>':
		switch (cmp[1])
		{	
		case '\0': return PH_CMP_BAD;
		case '=':  return PH_CMP_GE;
		default:   return PH_CMP_GT;
		}
	case '<':
		switch (cmp[1])
		{	
		case '\0': return PH_CMP_BAD;
		case '=':  return PH_CMP_LE;
		default:   return PH_CMP_LT;
		}
	case '=': return PH_CMP_EQ;
	default:  return PH_CMP_UNDEF;
	}
}

// TODO: Move this?
char const *
ph_cmp_to_string(enum ph_cmp cmp)
{
	switch (cmp)
	{
	case PH_CMP_UNDEF: return "";
	case PH_CMP_EQ: return "=";
	case PH_CMP_GE: return ">=";
	case PH_CMP_LE: return "<=";
	case PH_CMP_GT: return ">";
	case PH_CMP_LT: return "<";
	default:
	case PH_CMP_BAD: return NULL;
	}
}

static char const *
_category_strchr(char const *cat, char stop, bool *err)
{
	switch (*cat)
	{
	case '\0': case '-': case '+': case '.': *err = true; return cat;
	}
	
	for (; *cat && *cat != stop; ++cat)
		if (!(COMMON_CHAR_CHECK(*cat) || *cat == '.'))
			*err = true;
	
	if (*cat != stop)
		return NULL;
	return cat;
}

bool ph_atom_category_is_valid(char *category)
{
	bool err = false;
	if (!_category_strchr(category, '\0', &err) || err)
		return false;
	
	return true;
}

bool ph_atom_pkgname_is_valid(char *pkgname)
{
	if (pkgname == NULL || strlen(pkgname) == 0) return false;
	
	switch (pkgname[0]) {
	case '-': case '+':
		return false;
	}
	
	for (char *c = pkgname; *c != '\0'; ++c)
		if (!COMMON_CHAR_CHECK(*c))
			return false;
	
	// TODO ???
	// First check out any revision info
	char *last_hyp;
	if ((last_hyp = strrchr(pkgname, '-')) == NULL)
		return true;
	if (last_hyp[1] == 'r')
	{
		/* If it fails (i.e. r34isfun) then that means it's valid. So
		 * we'll flip the return */
		return !_numeric_until_end(last_hyp + 2);
	}
	if (isdigit(last_hyp[1]))
			return false;
	
	return true;
}

/* pretty big TODO's
 * -> validate more stuff
 * -> validate category names (need a strchr alternative)
 * -> validate package names (without using ph_atom_pkgname_is_valid)
 * -> test out versioning better
 */
int
ph_atom_parse_string(char const *atomstr, ph_atom_t *atom, ph_atom_parse_opts_t opts)
{
	char *tmp,
	     *deptmp,
		 *work = NULL, /* we need to free/set storage later at a whim,
		                * so this is our working var */
		 *slotchr; // Only used to check if slotchr < tmp
	char *storage = NULL;
	char *category = NULL;
	bool err = false;
	enum ph_cmp cmp = _parse_cmp(atomstr);
	
	switch (cmp)
	{
	case PH_CMP_EQ: case PH_CMP_GT: case PH_CMP_LT:
		atomstr += 1;
		break;
	case PH_CMP_GE: case PH_CMP_LE:
		atomstr += 2;
		break;
	case PH_CMP_UNDEF: break;
	case PH_CMP_BAD:
	default:
		goto err;
	}
	
	// dont want to clobber the old string, so we'll work on this one
	storage = work = str_new_from_cstr(atomstr);
	slotchr = strchr(work, ':');
	if ((tmp = _category_strchr(work, '/', &err)))
	{
		// Don't count "package-name:0/2"
		if (slotchr && tmp > slotchr)
		{
			tmp = work;
			atom->category = NULL;
		}
		else {
			if (err)
				goto err;
			// prevent cases like "/emacs-2000.0"
			if (tmp == work)
				goto err;
			*tmp = '\0';
			atom->category = work;
			work = tmp + 1;
		}
	}
	else
		atom->category = NULL; // not strictly required to work with
	
	// Before we go further, strip the ebuild now...
	if ((opts & PH_ATOM_PARSE_STRIP_EBUILD) == PH_ATOM_PARSE_STRIP_EBUILD &&
	    (tmp = strstr(work, ".ebuild")))
	{
		*tmp = '\0';
	}
	
	// Handle some weird edge cases, probably not needed when we do proper name validation later
	switch (*work)
	{
	case '-': case '+': goto err;
	}
	// later, we'll null terminate it
	atom->pkgname = work;
	
	/* Before our string gets clobbered with null terminators, let's
	 *   go ahead and look at '['. */
	if ((opts & PH_ATOM_PARSE_DEPENDENCY) == PH_ATOM_PARSE_DEPENDENCY)
	{
		if ((deptmp = strchr(work, '[')))
		{
			if (ph_use_deps_parse(deptmp, &atom->use_deps) != 0)
				goto err;
		}
		else
			memset(&atom->use_deps, 0, sizeof(struct ph_atom_use_deps));
	}
	else
		memset(&atom->use_deps, 0, sizeof(struct ph_atom_use_deps));
	
	// check for repo name
	bool is_slot = false;
	if ((tmp = strrchr(work, ':')) && tmp[-1] == ':' && tmp[1])
	{
		// TODO: validate repository name
		// repository is the last thing (i think), so no need to null terminate
		atom->repository = tmp + 1;
		
		// next, let's look for a slot!
		// tmp is at a repo divider, so let's go back
		tmp[-1] = '\0'; // since we know the slot is (probably) behind us, we can set this
		tmp = strrchr(work, ':');
		
		if (tmp && tmp[-1] != ':')
			is_slot = true;
	}
	else {
		atom->repository = NULL;
		is_slot = tmp;
		
		// Note: we may have already searched for tmp, so we can still assume it's a slot
		// But we do need to find the end 
		if (tmp)
		{
			DEBUGF("repo check failed, searching for slot at %s\n", tmp);
			for (char *c = tmp; *c; ++c)
			{
				if (!(COMMON_CHAR_CHECK(*c) || *c == '.' || *c == '/'	))
				{
					if (*c == ':')
					{
						*c = '\0';
						break;
					}
					else
						is_slot = false;
				}
			}
		}
	}
	
	if (is_slot)
	{
		atom->slot = tmp + 1;
		*tmp = '\0';
	}
	else
		atom->slot = NULL;
	
	//              -v- work is here
	//   util-garbage/neovim-1.0-r3:1::genfive
	// at this point, we are here -^-
	// or rather: util-garbage\0neovim-1.0-r3\01\0:genfive
	
	// now let's try and find where the version begins
	bool do_version = true;
	DEBUGF("atomstr: %s\n", work);
	if ((tmp = strrchr(work, '-')))
	{
		if (tmp[1] == 'r')
		{ // step back a little furhter
			char *revpt = tmp; // hack
			*revpt = '\0';
			if ((tmp = strrchr(work, '-')))
				if (!isdigit(tmp[1]))
					do_version = false;
			*revpt = '-'; // undo hack
		}
		else if (!isdigit(tmp[1]))
			do_version = false;
	}
	
	if (tmp && do_version)
	{
		*tmp = '\0'; // so atom->pkgname doesn't include the version
		char *version_ptr = tmp + 1;
		DEBUGF("version: %s\n", version_ptr);
		if (ph_version_parse_string(version_ptr, &atom->version) != 0)
		{
			goto err;
		}
	}
	
	DEBUGF("Resultant atomstr (should be the pkgname): %s\n", work);
	if (!(*work && ph_atom_pkgname_is_valid(work)))
		goto err;
	
	atom->_storage = storage; // For freeing
	atom->cmp = cmp;
	return 0;
err:
	atom->cmp      = PH_CMP_BAD;
	atom->category = NULL;
	atom->pkgname  = NULL;
	atom->repository  = NULL;
	atom->slot  = NULL;
	memset(&atom->version, 0, sizeof(struct ph_version));
	memset(&atom->use_deps, 0, sizeof(struct ph_atom_use_deps));
	// Prevent incorrect frees
	atom->_storage = NULL;
	str_free(storage);
	return -1;
}

// TODO
static char const *_suffixes[] = {
	"alpha", "beta", "pre", "rc", "p"
};

/* we typically try not to return NULL from this function even if
 *   the atom's data is rubbish. */
char *
ph_atom_to_string(ph_atom_t *atom)
{
	char *res = str_new();
	
	char const *cmp = ph_cmp_to_string(atom->cmp);
	if (cmp)
		str_cappend(&res, cmp);
	if (atom->category)
	{
		str_cappend(&res, atom->category);
		str_chappend(&res, '/');
	}
	str_cappend(&res, atom->pkgname ? atom->pkgname : "?");
	
	if (atom->version.version)
	{
		str_chappend(&res, '-');
		str_cappend(&res, atom->version.version);
		if (atom->version.version_suffix)
			str_chappend(&res, atom->version.version_suffix);
		
		for (size_t i = 0; i < atom->version.suffixes_len; ++i)
		{
			char buf[16] = { 0 };
			str_chappend(&res, '_');
			struct ph_suffix *suffix = atom->version.suffixes + i;
			str_cappend(&res, _suffixes[suffix->suffix]);
			if (suffix->number != -1)
			{
				snprintf(buf, 15, "%u", suffix->number);
				str_cappend(&res, buf);
			}
		}
		
		if (atom->version.rev)
		{
			char buf[16] = { 0 };
			snprintf(buf, 15, "-r%u", atom->version.rev);
			str_cappend(&res, buf);
		}
	}
	
	if (atom->slot)
	{
		str_chappend(&res, ':');
		str_cappend(&res, atom->slot);
	}
	
	if (atom->repository)
	{
		str_cappend(&res, "::");
		str_cappend(&res, atom->repository);
	}
	
	if (atom->use_deps.deps)
	{
		char *use_deps = ph_use_deps_to_string(&atom->use_deps);
		if (use_deps)
			str_cappend(&res, use_deps);
		free(use_deps);
	}
	
	return str_release(res);
}

bool ph_atomstr_is_valid(char *atomstr)
{
}

bool ph_atom_is_valid(ph_atom_t *atom)
{
	
}

void
ph_atom_free(ph_atom_t *atom)
{
	ph_version_free(&atom->version);
	str_free(atom->_storage);
}
