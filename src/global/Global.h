#ifndef _GLOBAL_H_
#define _GLOBAL_H_

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

#define DECL_BUFFER(BUF_TYPE,EL_TYPE) \
 \
typedef struct S##BUF_TYPE{ \
	int size; \
	EL_TYPE* array; \
} BUF_TYPE;

#define DEF_BUFFER(BUF_TYPE,EL_TYPE) \
 \
INLINE void BUF_TYPE##_init( \
	BUF_TYPE* buf, \
	int size \
) \
{ \
	buf -> size = size; \
	buf->array = getbytes( sizeof( EL_TYPE ) * size ); \
} \
 \
INLINE void BUF_TYPE##_exit( \
	BUF_TYPE* buf \
) \
{ \
	freebytes( buf->array, sizeof( EL_TYPE ) * buf->size ); \
	buf -> array = NULL; \
	buf -> size = 0; \
} \
INLINE int BUF_TYPE##_get_size( \
	BUF_TYPE* buf \
) \
{ \
	return buf->size; \
} \
 \
INLINE EL_TYPE* BUF_TYPE##_get_array( \
	BUF_TYPE* buf \
) \
{ \
	return buf->array; \
} \
 \
INLINE void BUF_TYPE##_resize( \
	BUF_TYPE* buf, \
	int size \
) \
{ \
	EL_TYPE* newA; \
	newA = getbytes( sizeof( EL_TYPE ) * size ); \
	memcpy( newA, buf->array, sizeof( EL_TYPE ) * min( buf->size, size ) ); \
	freebytes( buf->array, sizeof( EL_TYPE ) * buf->size ); \
	buf -> size = size; \
	buf->array = newA; \
} \
INLINE void BUF_TYPE##_double_size( \
	BUF_TYPE* buf \
) \
{ \
	BUF_TYPE##_resize( buf, buf->size * 2 ); \
}


#define DECL_DYN_ARRAY(ARRAY_TYPE,EL_TYPE) \
	DECL_BUFFER(ARRAY_TYPE##Buf,EL_TYPE) \
	DECL_DYN_ARRAY_IMPL(ARRAY_TYPE,EL_TYPE,ARRAY_TYPE##Buf)

#define DEF_DYN_ARRAY(ARRAY_TYPE,EL_TYPE) \
	DEF_BUFFER(ARRAY_TYPE##Buf,EL_TYPE) \
	DEF_DYN_ARRAY_IMPL(ARRAY_TYPE,EL_TYPE,ARRAY_TYPE##Buf)


#define DECL_DYN_ARRAY_IMPL(ARRAY_TYPE,EL_TYPE,BUF_TYPE) \
 \
typedef struct S##ARRAY_TYPE { \
	int size; \
	BUF_TYPE buffer; \
} ARRAY_TYPE;


#define DEF_DYN_ARRAY_IMPL(ARRAY_TYPE, EL_TYPE, BUF_TYPE) \
\
INLINE void ARRAY_TYPE##_init( \
	ARRAY_TYPE* array \
) \
{ \
	BUF_TYPE##_init( &array->buffer, 2); \
	array -> size = 0; \
} \
\
INLINE void ARRAY_TYPE##_exit( \
	ARRAY_TYPE* array \
) \
{ \
	BUF_TYPE##_exit( &array->buffer); \
} \
\
INLINE int ARRAY_TYPE##_get_size( \
	ARRAY_TYPE* array \
) \
{ \
	return array->size; \
} \
 \
INLINE EL_TYPE* ARRAY_TYPE##_get_array( \
	ARRAY_TYPE* array \
) \
{ \
	return BUF_TYPE##_get_array( & array->buffer ); \
} \
\
INLINE int ARRAY_TYPE##_get_alloc_size( \
	ARRAY_TYPE* array \
) \
{ \
	return BUF_TYPE##_get_size( & array->buffer ); \
} \
 \
INLINE void ARRAY_TYPE##_append( \
	ARRAY_TYPE* array, \
	EL_TYPE el \
) \
{ \
	if( ARRAY_TYPE##_get_size( array ) + 1 > ARRAY_TYPE##_get_alloc_size( array ) ) \
	{ \
		BUF_TYPE##_double_size( & array->buffer ); \
	} \
	BUF_TYPE##_get_array( & array->buffer )[array->size] = el; \
	array->size ++; \
} \
\
INLINE void ARRAY_TYPE##_clear( \
	ARRAY_TYPE* array \
) \
{ \
	array->size = 0; \
}

#endif
