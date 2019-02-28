#include "sdScript.h"

#include "Function.h"

#include <stdlib.h>
#include <string.h>


static t_class* script_class;
t_class* register_script_class( t_symbol* class_name );

void sdScript_setup()
{
	script_class = register_script_class( gensym("sdScript") );
	functions_init();
}

/*
	Stmts 	-> 	Stmt Stmts | eps
	Stmt 	->	func ( Stmts )
		|	proc
		|	value
		|	var
*/



// constructor:
void* t_script_obj_init(
	t_symbol* name,
	int argc,
	t_atom* argv
);
// destructor:
void t_script_obj_exit(
	t_script_obj* pThis
);

void sgScriptObj_OnExecute(t_script_obj* pThis);
void sgScriptObj_OnExecMeta(t_script_obj* pThis, t_symbol* s, int argc, t_atom* argv);
void sgScriptObj_OnSetProgram(t_script_obj* pThis, t_symbol* s, int argc, t_atom* argv);
void sgScriptObj_OnSetVar(t_script_obj* pThis, t_symbol* s, int argc, t_atom* argv);

// executes the current code:
void sgScriptObj_exec(t_script_obj* pThis);

// lexer methods:
typedef int POS;
#define END_OF_FILE -1
#define ERROR -2
typedef BOOL TokenFits;
t_atom* lexer_getNextToken(t_script_obj* pThis);
TokenFits lexer_consumeNextToken(t_script_obj* pThis, t_atom* pExpectedSym);
void lexer_setIP(t_script_obj* pThis, t_int peek);


void freeProgram(t_script_obj* pThis);
void freeMetaProgram(t_script_obj* pThis);

//BOOL isFunction(t_atom* pToken);
FunctionInfo* getFunctionInfo(t_atom* pToken);
BOOL isValue(t_atom* pToken);
/*
  checks if the next command is one that can be executed before all parameters have been read. If this is the case, and if there are enough parameters on stack for it, executed it.
*/
void tryToExecuteImmediately(t_script_obj* pThis);
//BOOL isVar(t_atom* pToken);
//BOOL isProc(t_atom* pToken);

//*********************************************

// calls the function that is on the cmdStack
void callFunction(t_script_obj* pThis, FunctionInfo* pFunctionInfo, t_int countParam);


t_atom leftParenthesis, rightParenthesis, escapeBegin, escapeEnd;

t_class* register_script_class( t_symbol* class_name )
{

	SETSYMBOL( &leftParenthesis, gensym("("));
	SETSYMBOL(& rightParenthesis, gensym(")"));
	SETSYMBOL(& escapeBegin, gensym("#["));
	SETSYMBOL(& escapeEnd, gensym("#]"));

	t_class* class =
		class_new(
			class_name, 		// name
			(t_newmethod )t_script_obj_init, 	// newmethod
			(t_method )t_script_obj_exit, 	// freemethod
			sizeof(t_script_obj), 		// size
			CLASS_DEFAULT, 			// object type
			A_GIMME,			// list of atoms following the selector...
			0
		);
	class_addbang(
		class,
		(t_method )sgScriptObj_OnExecute
	);
	class_addlist(
		class,
		(t_method )sgScriptObj_OnExecMeta
	);
	/*class_addlist(
		script_class,
		(t_method )sgScriptObj_OnSetProgram
	);*/
	class_addmethod(
		class,
		(t_method )sgScriptObj_OnSetProgram,
		gensym("setProgram"),
		A_GIMME
	);
	class_addanything(
		class,
		(t_method )sgScriptObj_OnSetVar
	);
	//class_addfloat(sgScript_class, sgScriptOnInput);
	return class;
}

// the constructor
/*void* sgScriptNew()
{
	t_script_obj* x = (t_script_obj* )pd_new(script_class);
	//x -> controlMessage = 0;
	
	//post("opening usb device...");
	//TRY(!usbOpen(),MSG_COULD_NOT_OPEN);
	return (void* )x;
}*/

void* t_script_obj_init(
	t_symbol* name,
	int argc,
	t_atom* argv
)
{
	DB_PRINT("creating sgScript object...");
	t_script_obj* x = (t_script_obj* )pd_new( script_class );

	x -> skipMode = FALSE;
	x -> currentCode = NULL;
	x -> currentProgCount = 0;
	x -> code = NULL;
	x -> cmdCount = 0;
	x -> metaCode = NULL;
	x -> metaCodeCount = 0;
	
	x -> peek = 0;
	//x -> outputBufferCount = 0;
	OutputBuf_init( & x->outputBuffer );
	x -> pSymbolTable = SymbolTable_New();
	//ListSTEntryInit( & x->symbolTable );
	ListAtomInit( & x->stack );
	ListCommandInit( & x->cmdStack );
	//ListAtomInit( & x -> currentCode );
	//x -> currentCode = getbytes(sizeof(t_atom)*MAX_PROGRAM_LENGTH);

	x -> pOutlet = outlet_new( & x -> obj, &s_list);
	inlet_new(
		& x -> obj,
		& x -> obj . ob_pd,
		gensym("list"),
		gensym("setProgram")
	);
	if( argc > 0 )
	{
		inlet_new(
			& x -> obj,
			& x -> obj . ob_pd,
			gensym("set"),
			gensym("onSetVar")
		);
	}
	for( int i=0; i<argc; i++)
	{
		Variable variable;
		variable . count = 1;
		variable . values = getbytes( sizeof(t_atom) );
		SETFLOAT( & variable . values[0], 0 );
		STValue value;
		value . type = VALUE;
		value . variable = variable;
		STEntry entry;
		entry . symbol = argv[i];
		entry . value = value;
		SymbolTable_AddMainVar(x -> pSymbolTable, entry);

	}
	/*x -> bufferLength = -1;
	x -> outTriggers = outlet_new(& x->obj, &s_list);
	x -> outSwitches = outlet_new(& x->obj, &s_list);
	x -> outAnalog = outlet_new(& x->obj, &s_list);
	x -> outMeta = outlet_new(&x->obj, &s_list);
	x -> outMidi = outlet_new(& x->obj, &s_list);*/
	return (void* )x;
}

void t_script_obj_exit(
	t_script_obj* pThis
)
{
	DB_PRINT("removing sgScript object...");
	OutputBuf_exit( & pThis->outputBuffer );
	freeProgram(pThis);
	freeMetaProgram(pThis);
	SymbolTable_Free( pThis -> pSymbolTable );
	ListCommandExit( & pThis->cmdStack );
	ListAtomExit( & pThis->stack );
	//ListSTEntryExit( & pThis->symbolTable );
	//freebytes(x -> currentCode);
	//ListAtomExit( & pThis -> currentCode );
}

//DECL_LIST(ListInt,ElementInt,t_int,copybytes,getbytes,freebytes);
//DEF_LIST(ListInt,ElementInt,t_int,copybytes,getbytes,freebytes);

void sgScriptObj_OnExecute(t_script_obj* pThis)
{
	pThis -> currentProgCount = pThis-> cmdCount;
	pThis -> currentCode = pThis -> code;
	sgScriptObj_exec( pThis );
}

void sgScriptObj_OnExecMeta(t_script_obj* pThis, t_symbol* s, int argc, t_atom* argv)
{
	DB_PRINT("sgScriptObj_OnExecMeta");
	freeMetaProgram(pThis);
	/*if( pThis -> currentCode != pThis -> code )
		freebytes(pThis -> currentCode, pThis->currentProgCount * sizeof(Command));*/
	pThis -> metaCode = getbytes(sizeof(t_atom) * argc);
	memcpy( pThis->metaCode, argv, sizeof(t_atom) * argc);
	pThis -> metaCodeCount = argc;

	pThis -> currentCode = pThis -> metaCode;
	pThis -> currentProgCount = pThis -> metaCodeCount;

	sgScriptObj_exec( pThis );
}

void sgScriptObj_OnSetProgram(t_script_obj* pThis, t_symbol* s, int argc, t_atom* argv)
{
	DB_PRINT("sgScriptObj_OnSetProgram");
	freeProgram(pThis);
	pThis -> code = copybytes(argv, sizeof(t_atom)*argc);
	pThis -> cmdCount = argc;
}
void sgScriptObj_OnSetVar(t_script_obj* pThis, t_symbol* s, int argc, t_atom* argv)
{
	DB_PRINT("sgScriptObj_OnSetVar");
	t_atom* prog = getbytes( sizeof(t_atom)* (argc+3) );
	SETSYMBOL( &prog[0], gensym("Set"));
	SETSYMBOL( &prog[1], atom_getsymbol(&leftParenthesis));
	prog[2] = argv[0];
	memcpy( &prog[3], &argv[1], sizeof(t_atom)*(argc-1));
	SETSYMBOL( &prog[3+argc-1], atom_getsymbol(&rightParenthesis));

	pThis -> currentCode = prog;
	pThis -> currentProgCount = 3+argc;
	sgScriptObj_exec( pThis );
	freebytes( prog, sizeof(t_atom) * (argc+3) );
}

ElementCommand* ifFunc(t_script_obj* pThis, FunctionInfo* pFunctionInfo)
{
	ElementCommand* pElCurrentFunction = NULL;
	// <func>
	DB_PRINT("Is a function");
	// add the function to the command stack:
	CommandInfo* pCurrentCommandInfo = getbytes(sizeof(CommandInfo));
	//pCurrentCommandInfo -> name = *pCurrentToken;
	//pCurrentCommandInfo -> countParam = -1;
	pCurrentCommandInfo -> stackHeight0 = ListAtomGetSize ( & pThis -> stack );
	pCurrentCommandInfo -> pFunctionInfo = pFunctionInfo;
	pElCurrentFunction = ListCommandAdd ( & pThis -> cmdStack, pCurrentCommandInfo);

	return pElCurrentFunction;
}

ElementCommand* ifRightParenthesis(t_script_obj* pThis,ElementCommand* pElCurrentFunction)
{
	FunctionInfo* pFunctionInfo = pElCurrentFunction -> pData -> pFunctionInfo;
	t_int paramCount =
		ListAtomGetSize ( & pThis -> stack )
		- pElCurrentFunction -> pData -> stackHeight0
	;
	if(
		( pFunctionInfo -> paramCount == -1 )
		|| ( pFunctionInfo -> paramCount == paramCount )
	)
	{
		// execute the function:
		callFunction(pThis, pFunctionInfo, paramCount);
		pElCurrentFunction = ListCommandGetLast ( & pThis -> cmdStack );
	}
	else
	{
		char buf[256];
		atom_string( &pFunctionInfo->name, buf, 255 );
		post("ERROR: wrong number of parameters for function %s", buf);
	}

	//delete the function from Stack, becaus it has been executed:
	ListCommandDel( &pThis -> cmdStack, pElCurrentFunction);
	pElCurrentFunction = ListCommandGetLast ( & pThis -> cmdStack );
	tryToExecuteImmediately(pThis);
	pElCurrentFunction = ListCommandGetLast ( & pThis -> cmdStack );
	return pElCurrentFunction;
}

void ifValue(t_script_obj* pThis, t_atom* pCurrentToken)
{
	// put value on stack:
	t_atom* pValue = getbytes(sizeof(t_atom));
	*pValue = *pCurrentToken;
	ListAtomAdd( & pThis -> stack, pValue);
}

// executes the current code:
void sgScriptObj_exec(t_script_obj* pThis)
{
	int countParenthesisRightIgnore = 1;
	BOOL escape = FALSE;
	pThis -> skipMode = FALSE;
	pThis -> peek = 0;
	SymbolTable_Clear( pThis -> pSymbolTable );

	ElementCommand* pElCurrentFunction = NULL; // topmost function on the command stack
	t_atom* pCurrentToken=NULL;
	while( (pCurrentToken = lexer_getNextToken(pThis)) )
	{
		char buf[256];
		atom_string(pCurrentToken, buf, 256);
		DB_PRINT("current Token: '%s'", buf);
		if( compareAtoms(pCurrentToken, &escapeBegin) )
		{
			escape = TRUE;
			continue;
		}
		if( compareAtoms(pCurrentToken, &escapeEnd) )
		{
			if (escape == FALSE)
				post("ERROR: #] found, without corresponding #[!");
			escape = FALSE;
			continue;
		}
		FunctionInfo* pFunctionInfo = NULL;
		if( !escape)
			pFunctionInfo = getFunctionInfo( pCurrentToken );
		if( pFunctionInfo )
		{
			if( ! pThis -> skipMode )
				pElCurrentFunction = ifFunc(pThis,pFunctionInfo);	
			// eat the "(" that follows:
			if( ! lexer_consumeNextToken(pThis, & leftParenthesis) )
			{
				// report syntax error:
				post("ERROR: '(' expected, after ...");
			}
			else if( pThis->skipMode )
				(countParenthesisRightIgnore) ++;
		}
		else if ((!escape) && (compareAtoms(pCurrentToken,&rightParenthesis)))
		{
			if( pThis->skipMode )
			{
				countParenthesisRightIgnore --;
				if( countParenthesisRightIgnore == 0 )
				{
					pThis -> skipMode = FALSE;
					countParenthesisRightIgnore = 1;
				}
				continue;
			}
			// ")"
			if (pElCurrentFunction)
			{
				pElCurrentFunction =
					ifRightParenthesis(pThis,pElCurrentFunction);
			}
			else
				post("ERROR: ')' found, but no corresponding 'func ('");
		}
		else
		{
			DB_PRINT("is a value");
			if( pThis->skipMode )
				continue;

			// if in escape mode, everything is considered a value:
			if (escape || isValue(pCurrentToken))
			{
				ifValue(pThis,pCurrentToken);
			}
			else
			{
				post("ERROR: token is neither function, var, or procedure: '%s'",buf);
			}
			tryToExecuteImmediately(pThis);
			pElCurrentFunction = ListCommandGetLast ( & pThis -> cmdStack );
		}
	}
}

// lexer methods:
t_atom* lexer_getNextToken(t_script_obj* pThis)
{
	t_atom* pRet = 0;
	if( pThis -> peek < pThis -> currentProgCount )
	{
		pRet = & pThis -> currentCode[ pThis -> peek ];
		pThis -> peek += 1;
	}
	return pRet;
}

TokenFits lexer_consumeNextToken(t_script_obj* pThis, t_atom* pExpectedSym)
{
	t_atom* pNextToken = lexer_getNextToken(pThis);
	return compareAtoms(pNextToken, pExpectedSym );
}

void lexer_setIP(t_script_obj* pThis, t_int peek)
{
}

void callFunction(t_script_obj* pThis, FunctionInfo* pFunctionInfo, t_int countParam)
{
	// create an array of pointers to the parameters:
	//t_atom** pArrayParam = getbytes( sizeof(t_atom* )* countParam);
	// first copy params:
	t_atom* pArgs = getbytes( sizeof(t_atom )* countParam);
	{
		ElementAtom* pElOpNext = ListAtomGetLast( &pThis -> stack);
		//pArrayParam[countParam-1] = pElOpNext -> pData;
		for ( int i=countParam-1; i>=0; i--)
		{
			pArgs[i] = *(pElOpNext-> pData);
			pElOpNext = ListAtomGetPrev( &pThis -> stack, pElOpNext);
		}
	}
	// ... delete them from stack
	for ( int i=0; i<countParam; i++)
	{
		ElementAtom* pElParam = ListAtomGetLast ( &pThis -> stack );
		ListAtomDel( &pThis -> stack, pElParam );
	}
	DB_PRINT("callFunction called");
	// call command:
	(pFunctionInfo -> pFunc) (pThis, countParam, pArgs);
	// free array of parameters:
	freebytes( pArgs, sizeof(t_atom ) * countParam );
}

void freeProgram(t_script_obj* pThis)
{
	if( pThis -> code )
		freebytes(pThis -> code, pThis->cmdCount * sizeof(Command));
}
void freeMetaProgram(t_script_obj* pThis)
{
	if( pThis -> metaCode )
		freebytes(pThis -> metaCode, pThis->metaCodeCount * sizeof(Command));
}

BOOL isValue(t_atom* pToken)
{
	if( atomEqualsString( pToken, "(" ) )
		return FALSE;
	return TRUE;
}

void tryToExecuteImmediately(t_script_obj* pThis)
{
	ElementCommand* pElCurrentFunction = ListCommandGetLast ( & pThis -> cmdStack );
	if( !pElCurrentFunction )
		return ;
	// if the topmost command is a dont-read-all-parameters-command:
	if( pElCurrentFunction->pData -> pFunctionInfo -> executeAfter != -1)
	{
		DB_PRINT("trying to executed immediately...:");
		FunctionInfo* pFunctionInfo = pElCurrentFunction->pData -> pFunctionInfo;
		// check if there are enough values on stack now to call the next function
		t_int paramCount =
			ListAtomGetSize ( & pThis -> stack )
			- pElCurrentFunction -> pData -> stackHeight0
		;
		if( paramCount== pFunctionInfo -> executeAfter )
		{
			callFunction(pThis, pFunctionInfo, paramCount);
		}
		else if( paramCount > pFunctionInfo -> executeAfter)
		{
			char buf[256];
			atom_string(& pFunctionInfo->name, buf, 256);
			post("ERROR: wrong number of parameters for %s", buf);
		}
		ListCommandDel( &pThis -> cmdStack, pElCurrentFunction);
	}
}

