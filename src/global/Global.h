#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "Basic.h"
#include "LinkedList.h"


DECL_LIST(ListAtom,ElementAtom,t_atom,getbytes,freebytes,freebytes)
DEF_LIST(ListAtom,ElementAtom,t_atom,getbytes,freebytes,freebytes);

// pointers to atoms are considered references, therefore not deleted automatically:
#pragma GCC diagnostic ignored "-Wunused-value"
DECL_LIST(ListAtomPointer,ElementAtomPointer,t_atom,getbytes,freebytes,)
DEF_LIST(ListAtomPointer,ElementAtomPointer,t_atom,getbytes,freebytes,);
#pragma GCC diagnostic pop

#endif
