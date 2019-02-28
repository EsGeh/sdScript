#ifndef _SDSCRIPT_H
#define _SDSCRIPT_H

#include "Global.h"
#include "LinkedList.h"
#include "SymbolTable.h"
#include "Function.h"

#include "m_pd.h"

#define OUTPUTBUFFER_LENGTH 1024

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

struct _script_obj {
	//internal obj information:
	t_object obj;
	t_outlet* pOutlet;
	// skip mode (default FALSE):
	BOOL skipMode;
	//symbol table
	SymbolTable* pSymbolTable;
	// return stack
	ListAtom stack;
	// CommandStack:
	ListCommand cmdStack;
	// current program:
	t_int currentProgCount;
	Command* currentCode;
	// code
	t_int cmdCount;
	Command* code;
	// code
	t_int metaCodeCount;
	Command* metaCode;
	// instruction pointer
	t_int peek;
	// output buffer:
	OutputBuf outputBuffer;
	//
};

#endif 
