// Licensed under the BSD 3-Clause License
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include "use.h"
#include "str.h"

static char *
_use_strchr(char *use, char *stop, bool *err)
{
	if (!isalnum(*use))
	{
		*err = true;
		return use;
	}
	
	for (; *use; ++use)
	{
		if (stop)
			if (_str_oneof(*use, stop))
				goto out;
		if (!(isalnum(*use) || *use == '+' || *use == '_' || *use == '@' || *use == '-'))
			*err = true;
	}
out:
	return use;
}

bool
ph_use_is_valid(char *use)
{
	bool err = false;
	if (!_use_strchr(use, NULL, &err) || err)
		return false;
	
	return true;
}

int
ph_use_deps_parse(char *atomstr, ph_atom_use_deps_t *dep)
{
#define USE_END_SANITY_CHECK(_var) if (!*_var && *_var != ',' && *_var != ']' && *_var != '(') goto err;
	char *tmp;
	assert(*atomstr == '[');
	struct ph_atom_use_dep *depl = NULL;
	bool err;
	
	*atomstr = '\0';
	++atomstr;
	
	size_t i;
	for (i = 0; *atomstr && *atomstr != ']'; ++i)
	{
		depl = realloc(depl, sizeof(struct ph_atom_use_dep) * (i+1));
		depl[i].use_default = depl[i].pre = depl[i].post = 0;
		depl[i].use = NULL;
		
		switch (*atomstr)
		{
		case '!':
			depl[i].pre = '!';
			++atomstr;
			break;
		case '-':
			depl[i].pre = '-';
			++atomstr;
			break;
		default:
			/* hopefully [A-Za-z0-9], but _use_strchr will check if
			 * the use flag is valid */
			break;
		}
		
		/* Loop through each use flag until we hit a special
		 * character, in which we process it. */
		if ((tmp = _use_strchr(atomstr, ",](?=", &err)))
		{
			if (err)
				goto err;

			switch (*tmp)
			{
			case '?':
				*tmp = '\0';
				++tmp;
				depl[i].post = '?';
				USE_END_SANITY_CHECK(tmp);
				if (*tmp != ']') ++tmp;
				break;
			case '(':
				*tmp = '\0';
				++tmp;
				switch (*tmp)
				{
				case '-':
					depl[i].use_default = '-';
					break;
				case '+':
					depl[i].use_default = '+';
					break;
				default:
					goto err;
				}
				if (*(++tmp) != ')')
					goto err;
				
				++tmp;
				break;
			case ',':
				*tmp = '\0';
				++tmp;
				/* cases like [use,] would cause this loop to end and
				 * no error to occur */
				if (*tmp == ']')
					goto err;
				break;
			case ']':
				*tmp = '\0';
				break;
			default:
				goto err;
			}
			
			depl[i].use = atomstr;

			// use(+)
			if (*tmp == '?')
			{
				if (tmp[-1] != ')')
					goto err;
				++tmp;
				depl[i].post = '?';
				USE_END_SANITY_CHECK(tmp);
				if (*tmp != ']') ++tmp;
			}
			
			atomstr = tmp;
		}
		else
			goto err; // ?
	}
	dep->deps_len = i;
	
	
	dep->deps = depl;
	return 0;
err:
	free(depl);
	dep->deps = 0;
	dep->deps_len = 0;
	return -1;
}

char *
ph_use_deps_to_string(ph_atom_use_deps_t *deps)
{
	if (!(deps->deps && deps->deps_len))
		return NULL;
	char *res = str_new();
	str_chappend(&res, '[');
	
	for (size_t i = 0; i < deps->deps_len; ++i)
	{
		struct ph_atom_use_dep *dep = deps->deps + i;
		if (dep->pre)
			str_chappend(&res, dep->pre);
		
		if (dep->use)
			str_cappend(&res, dep->use);
		
		if (dep->use_default)
		{
			str_chappend(&res, '(');
			str_chappend(&res, dep->use_default);
			str_chappend(&res, ')');
		}
		
		if (dep->post)
			str_chappend(&res, dep->post);
		
		if (i != deps->deps_len - 1)
			str_chappend(&res, ',');
	}
	
	str_chappend(&res, ']');
	return str_release(res);
}

void
ph_use_free(ph_atom_use_deps_t *deps)
{
	free(deps->deps);
}
