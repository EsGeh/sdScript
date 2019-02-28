#ifndef _BASIC_H
#define _BASIC_H

#include "m_pd.h"

#include <string.h>


#define BOOL int
#define TRUE 1
#define FALSE 0

#define INLINE static inline

#ifndef NULL
    #define NULL 0
#endif

#ifndef DEBUG
	#define DB_PRINT(message, ...)
#else
	#define DB_PRINT(message, ...)\
	POST(message, ## __VA_ARGS__)
#endif

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _b : _a; })


INLINE int compareAtoms(t_atom* atoml, t_atom* atomr)
{
	if(atoml -> a_type == A_SYMBOL)
	{
		if(atomr -> a_type == A_SYMBOL)
		{
			return atom_getsymbol( atoml ) == atom_getsymbol( atomr );
		}
	}
	else
	{
		if(atomr -> a_type == A_FLOAT)
		{
			return atom_getfloat(atoml)==atom_getfloat(atomr);
		}
	}
	return 0;
}

INLINE BOOL atomEqualsString(t_atom* pAtom, char* string)
{
	char buf[256];
	atom_string(pAtom,buf,256);
	if(!strncmp(buf,string,256))
		return TRUE;
	return FALSE;
}

#endif