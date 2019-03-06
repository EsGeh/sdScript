#ifndef _SDSCRIPT_H
#define _SDSCRIPT_H

#include "Global.h"
#include "LinkedList.h"
#include "SymbolTable.h"
#include "Function.h"
#include "Map.h"

#include "m_pd.h"


void sdScript_setup();

// information kept during runtime for each function:
typedef struct SCommandInfo {
	t_int stackHeight0;
	FunctionInfo* pFunctionInfo;
} CommandInfo;

// predeclarations
typedef t_atom Command;

DECL_LIST(ListCommand,ElementCommand,CommandInfo,getbytes,freebytes,freebytes)
DEF_LIST(ListCommand,ElementCommand,CommandInfo,getbytes,freebytes,freebytes);

DECL_BUFFER(CommandBuf,CommandInfo)
DEF_BUFFER(CommandBuf,CommandInfo)

DECL_BUFFER(TokenBuf, t_atom)
DEF_BUFFER(TokenBuf, t_atom)


#define DEL_PROG( prog, size ) \
		TokenBuf_exit( prog ); \
		freebytes( prog, size )

#define PROGS_HASH(sym) ((unsigned int ) sym)
DECL_MAP(Programs,t_symbol*,TokenBuf,getbytes,freebytes,DEL_PROG,PROGS_HASH)
DEF_MAP(Programs,t_symbol*,TokenBuf,getbytes,freebytes,DEL_PROG,PROGS_HASH)

typedef CommandBuf Program;

typedef struct SRuntimeData {
	t_symbol* current_prog_name;
	TokenBuf* current_prog;

	Scope* scope;
	Scope* global_scope;

	// return stack
	ListAtom stack;
	// CommandStack:
	ListCommand command_stack;
	// instruction pointer
	t_int peek;

	int countParenthesisRightIgnore;
	BOOL escape;
	// skip mode (default FALSE):
	BOOL skipMode;
	// if != 0: wait before next cmd:
	double delay;

	t_script_obj* script_obj;
} RuntimeData;

DECL_LIST( ProgStack, ProgStackEl, RuntimeData, getbytes, freebytes, DEL_RT );


struct _script_obj {
	//internal obj information:
	t_object obj;
	t_outlet* pOutlet;

	//symbol table
	SymbolTable* symbol_table;

	Scope global_scope;

	// programs
	Programs programs;

	// while running: stores the execution state
	ProgStack program_stack;
	//RuntimeData* rt;

	// output buffer:
	OutputBuf output_buffer;

	// clock needed for the "Delay" command:
	t_clock *clock;
};


INLINE void DEL_RT(RuntimeData* rt, int size)
{
	ListAtomExit(  & rt -> stack );
	ListCommandExit( & rt -> command_stack );
	symtab_del_scope(
			rt -> script_obj -> symbol_table,
			rt -> current_prog_name
	);
	freebytes( rt, size );
}

DEF_LIST( ProgStack, ProgStackEl, RuntimeData, getbytes, freebytes, DEL_RT );

int script_obj_exec_sub_program(
	t_script_obj* this,
	t_symbol* prog_name
);

void sdScript_output(
	t_script_obj* pThis,
	int argc,
	t_atom* argv
);

#endif 
