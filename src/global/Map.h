#ifndef _MAP_H_
#define _MAP_H_

#include "LinkedList.h"


#define DECL_MAP(MAP,KEY,TYPE,MALLOC,FREE,DEL_TYPE,HASH) \
typedef struct \
{ \
	KEY key; \
	TYPE* value; \
}  MAP##_Entry; \
\
INLINE void MAP##_free_entry( \
	MAP##_Entry* entry, \
	int size \
) \
{ \
	DEL_TYPE( entry->value, sizeof( TYPE ) ); \
	FREE( entry, sizeof( MAP##_Entry ) );\
} \
\
DECL_LIST(MAP##_Bucket, MAP##_BucketEl, MAP##_Entry, getbytes,freebytes,MAP##_free_entry) \
DEF_LIST(MAP##_Bucket, MAP##_BucketEl, MAP##_Entry, getbytes,freebytes,freebytes) \
 \
DECL_BUFFER(MAP##_Buckets,MAP##_Bucket) \
DEF_BUFFER(MAP##_Buckets,MAP##_Bucket) \
 \
struct S##MAP\
{ \
	MAP##_Buckets buckets; \
} ; \
 \
typedef struct S##MAP MAP;


#define DEF_MAP(MAP,KEY,TYPE,MALLOC,FREE,DEL_TYPE,HASH) \
INLINE void MAP##_init( MAP* map, int size) \
{ \
	MAP##_Buckets_init( & map->buckets, size ); \
	for( int i=0; i<size; i++ ) \
	{ \
		MAP##_BucketInit( \
			& MAP##_Buckets_get_array( & map->buckets )[i]  \
		); \
	} \
} \
 \
INLINE void MAP##_exit( MAP* map ) \
{ \
	for( int i=0; i< MAP##_Buckets_get_size( & map->buckets ); i++ ) \
	{ \
		MAP##_BucketExit( \
			& MAP##_Buckets_get_array( & map->buckets )[i]  \
		); \
	} \
	MAP##_Buckets_exit( & map->buckets ); \
} \
 \
INLINE MAP##_Bucket* MAP##_priv_find_bucket( \
	MAP* map, \
	KEY key \
) \
{ \
	int size = MAP##_Buckets_get_size( & map->buckets ); \
	int hash = HASH( key ) % size ; \
	return & MAP##_Buckets_get_array( & map -> buckets )[ hash ]; \
} \
 \
INLINE MAP##_BucketEl* MAP##_priv_find_bucket_el( \
	MAP##_Bucket* bucket, \
	KEY key \
) \
{ \
	LIST_FORALL_BEGIN(MAP##_Bucket,MAP##_BucketEl,Entry,bucket,index,pEl) \
		if( pEl->pData->key == key ) \
			return pEl; \
	LIST_FORALL_END(MAP##_Bucket,MAP##_BucketEl,Entry,bucket,index,pEl) \
	return NULL; \
} \
 \
INLINE void MAP##_delete( \
	MAP* map, \
	KEY key \
) \
{ \
	MAP##_Bucket* bucket = MAP##_priv_find_bucket( map, key ); \
	MAP##_BucketEl* bucketEl = MAP##_priv_find_bucket_el( bucket, key ); \
	if( bucketEl != NULL ) \
	{ \
		MAP##_BucketDel( bucket, bucketEl ); \
	} \
} \
 \
INLINE void MAP##_insert( \
	MAP* map, \
	KEY key, \
	TYPE* x \
) \
{ \
	MAP##_Bucket* bucket = MAP##_priv_find_bucket( map, key ); \
	MAP##_BucketEl* bucketEl = MAP##_priv_find_bucket_el( bucket, key ); \
	if( bucketEl != NULL ) \
		MAP##_BucketDel( bucket, bucketEl ); \
 \
	MAP##_Entry* entry = getbytes( sizeof( MAP##_Entry ) ); \
	entry->key = key; \
	entry->value = x; \
	MAP##_BucketAdd( bucket, entry ); \
} \
 \
INLINE TYPE* MAP##_get( \
	MAP* map, \
	KEY key \
) \
{ \
	MAP##_Bucket* bucket = MAP##_priv_find_bucket( map, key ); \
	MAP##_BucketEl* bucketEl = MAP##_priv_find_bucket_el( bucket, key ); \
	if( bucketEl == NULL ) \
		return NULL; \
	return bucketEl->pData->value; \
}

#endif
