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
		(TokenBuf_exit( prog ))

#define PROGS_HASH(sym) ((unsigned int ) sym)
DECL_MAP(Programs,t_symbol*,TokenBuf,getbytes,freebytes,DEL_PROG,PROGS_HASH)
DEF_MAP(Programs,t_symbol*,TokenBuf,getbytes,freebytes,DEL_PROG,PROGS_HASH)

typedef CommandBuf Program;

typedef struct SRuntimeData {
	t_symbol* current_prog_name;
	TokenBuf* current_prog;

	SymbolTable* pSymbolTable;


	// return stack
	ListAtom* stack;
	// CommandStack:
	ListCommand* cmdStack;
	// instruction pointer
	t_int peek;
	// output buffer:
	OutputBuf* outputBuffer;

	int countParenthesisRightIgnore;
	BOOL escape;
	// skip mode (default FALSE):
	BOOL skipMode;
	// if != 0: wait before next cmd:
	double delay;

	t_script_obj* script_obj;
} RuntimeData;


struct _script_obj {
	//internal obj information:
	t_object obj;
	t_outlet* pOutlet;

	//symbol table
	SymbolTable* pSymbolTable;

	// programs
	Programs programs;

	// while running: stores the execution state
	RuntimeData* rt;

	// clock needed for the "Delay" command:
	t_clock *clock;
};

void sdScript_output(
	t_script_obj* pThis,
	int argc,
	t_atom* argv
);

#endif 
