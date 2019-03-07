#ifndef _SYMBOLTABLE_H_
#define _SYMBOLTABLE_H_ 

#include "Global.h"
#include "Map.h"

#define VARS_HASH_SIZE 1024
#define SCOPES_HASH_SIZE 1024


DECL_DYN_ARRAY(Atoms,t_atom)
DEF_DYN_ARRAY(Atoms,t_atom)

#define DEL_VAR(var,size) \
	Atoms_exit( var ); \
	freebytes( var, size )

#define HASH_SYMBOL(sym) \
	((unsigned int )sym)

DECL_MAP(Scope,t_symbol*,Atoms,getbytes,freebytes,DEL_VAR,HASH_SYMBOL)
DEF_MAP(Scope,t_symbol*,Atoms,getbytes,freebytes,DEL_VAR,HASH_SYMBOL)

#define DEL_SCOPE(scope,size) \
	Scope_exit( scope ); \
	freebytes( scope, size )

/*
DECL_MAP(ScopeVars,t_symbol*,Atoms,getbytes,freebytes,DEL_VAR,HASH_SYMBOL)
DEF_MAP(ScopeVars,t_symbol*,Atoms,getbytes,freebytes,DEL_VAR,HASH_SYMBOL)

typedef struct SScope {
	ScopeVars vars;
	// other info...
	
} Scope;

#define DEL_SCOPE(scope,size) \
	ScopeVars_exit( & scope->vars ); \
	freebytes( scope, size )
*/

DECL_MAP(SymbolTable,t_symbol*,Scope,getbytes,freebytes,DEL_SCOPE,HASH_SYMBOL)
DEF_MAP(SymbolTable,t_symbol*,Scope,getbytes,freebytes,DEL_SCOPE,HASH_SYMBOL)

SymbolTable* symtab_init();
void symtab_exit(
	SymbolTable* x
);
Scope* symtab_add_scope(
	SymbolTable* x,
	t_symbol* name
);
Scope* symtab_get_scope(
		SymbolTable* x,
		t_symbol* name
);
void symtab_del_scope(
	SymbolTable* x,
	t_symbol* name
);
Atoms* symtab_get_value(
		Scope* scope,
		t_symbol* name
);

#endif 
