#include "sdScript.h"

#include "Function.h"

#include <stdlib.h>
#include <string.h>


#define PROGRAMS_HASH_SIZE 1024

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

void script_obj_on_set_main(
		t_script_obj* pThis,
		t_symbol* s,
		int argc,
		t_atom* argv
);
void script_obj_on_set_program(
		t_script_obj* pThis,
		t_symbol* s,
		int argc,
		t_atom* argv
);
void script_obj_on_del_program(
		t_script_obj* pThis,
		t_symbol* s
);
void script_obj_on_exec(
	t_script_obj* pThis,
	t_symbol* program
);
void script_obj_on_set_var(
	t_script_obj* pThis,
	t_symbol* s,
	int argc,
	t_atom* argv
);

// executes the current code:
void sgScriptObj_exec(
		RuntimeData* rt
);

// lexer methods:
typedef int POS;
#define END_OF_FILE -1
#define ERROR -2
typedef BOOL TokenFits;

t_atom* lexer_getNextToken(
		RuntimeData* rt
);
TokenFits lexer_consumeNextToken(
		RuntimeData* rt,
		t_atom* pExpectedSym
);


FunctionInfo* getFunctionInfo(t_atom* pToken);
BOOL isValue(t_atom* pToken);
/*
  checks if the next command is one that can be executed before all parameters have been read. If this is the case, and if there are enough parameters on stack for it, executed it.
*/
void tryToExecuteImmediately(
		RuntimeData* rt
);

//*********************************************

// calls the function that is on the cmdStack
void callFunction(
		RuntimeData* rt,
		FunctionInfo* pFunctionInfo,
		t_int countParam
);


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

	class_addmethod(
		class,
		(t_method )script_obj_on_set_program,
		gensym("prog_set"),
		A_GIMME
	);
	class_addmethod(
		class,
		(t_method )script_obj_on_del_program,
		gensym("prog_del"),
		A_SYMBOL,
		0
	);
	class_addmethod(
		class,
		(t_method )script_obj_on_exec,
		gensym("exec"),
		A_SYMBOL,
		0
	);
	class_addmethod(
		class,
		(t_method )script_obj_on_set_var,
		gensym( "set" ),
		A_GIMME
	);
	return class;
}

void* t_script_obj_init(
	t_symbol* name,
	int argc,
	t_atom* argv
)
{
	DB_PRINT("creating sgScript object...");
	t_script_obj* x = (t_script_obj* )pd_new( script_class );

	Programs_init( & x->programs, PROGRAMS_HASH_SIZE );
	x -> pSymbolTable = SymbolTable_New();

	x -> pOutlet = outlet_new( & x -> obj, &s_list);
	/*
	inlet_new(
		& x -> obj,
		& x -> obj . ob_pd,
		gensym("list"),
		gensym("set_prog")
	);
	*/
	if( argc > 0 )
	{
		/*
		inlet_new(
			& x -> obj,
			& x -> obj . ob_pd,
			gensym("set"),
			gensym("onSetVar")
		);
		*/
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
	return (void* )x;
}

void t_script_obj_exit(
	t_script_obj* pThis
)
{
	DB_PRINT("removing sgScript object...");
	Programs_exit( & pThis->programs );
	SymbolTable_Free( pThis -> pSymbolTable );
}

void sdScript_output(
	t_script_obj* pThis,
	int argc,
	t_atom* argv
)
{
	outlet_list(
		pThis -> pOutlet,
		&s_list,
		argc,
		argv
	);
}

/*
void script_obj_on_set_main(
		t_script_obj* pThis,
		t_symbol* s,
		int argc,
		t_atom* argv
)
{
}
*/

void script_obj_on_set_program(
	t_script_obj* pThis,
	t_symbol* s,
	int argc,
	t_atom* argv
)
{
	DB_PRINT("script_obj_on_set_program");
	if( argc < 1 )
	{
		pd_error(
				pThis,
				"sdScript: wrong syntax!"
		);
	}

	t_symbol* prog_name = atom_getsymbol(
		& argv[0]
	);
	int prog_size = (argc-1);

	TokenBuf* new_program = getbytes( sizeof( TokenBuf ) );
	TokenBuf_init( new_program, prog_size);
	
	memcpy(
		TokenBuf_get_array( new_program ),
		& argv[1],
		sizeof( t_atom ) * prog_size
	);

	Programs_insert(
		& pThis -> programs,
		prog_name,
		new_program
	);
}

void script_obj_on_del_program(
	t_script_obj* pThis,
	t_symbol* prog_name
)
{
	Programs_delete(
		& pThis -> programs,
		prog_name
	);
}

void script_obj_on_exec(
	t_script_obj* pThis,
	t_symbol* prog_name
)
{
	TokenBuf* prog = Programs_get(
			& pThis -> programs,
			prog_name
	);
	if(
			prog == NULL
	)
	{
		t_atom a_prog_name;
		SETSYMBOL( & a_prog_name, prog_name );
		char buffer[256];
		atom_string( & a_prog_name, buffer, 255 );
		pd_error( pThis, "sdScript: no such program: '%s'", buffer );
		return;
	}
	ListAtom stack;
	ListCommand cmd_stack;
	OutputBuf output_buffer;

	ListAtomInit( &stack );
	ListCommandInit( &cmd_stack );
	OutputBuf_init( &output_buffer );

	RuntimeData rt = {
		.current_prog_name = prog_name,
		.current_prog = prog,
		.pSymbolTable = pThis-> pSymbolTable,
		.skipMode = FALSE,
		.stack = &stack,
		.cmdStack = &cmd_stack,
		.peek = 0,
		.outputBuffer = &output_buffer,
		.script_obj = pThis
	};
	sgScriptObj_exec(
			&rt
	);

	OutputBuf_exit( &output_buffer );
	ListAtomExit( &stack );
	ListCommandExit( &cmd_stack );
}

void script_obj_on_set_var(
	t_script_obj* pThis,
	t_symbol* s,
	int argc,
	t_atom* argv
)
{
	DB_PRINT("script_obj_on_set_var");

	// __INTERN_SET Set ( <var> ... )
	t_int prog_count = argc+4;
	t_atom* prog = getbytes( sizeof(t_atom)* prog_count );
	SETSYMBOL( &prog[0], gensym("__INTERN_SET"));
	SETSYMBOL( &prog[1], gensym("Set"));
	SETSYMBOL( &prog[2], atom_getsymbol(&leftParenthesis));
	prog[3] = argv[0];
	memcpy( &prog[4], &argv[1], sizeof(t_atom)*(argc-1));
	SETSYMBOL( &prog[4+argc-1], atom_getsymbol(&rightParenthesis));

	script_obj_on_set_program(
			pThis,
			&s_list,
			prog_count,
			prog
	);
	script_obj_on_exec( pThis, gensym("__INTERN_SET") );
	freebytes( prog, sizeof(t_atom) * prog_count );
}

ElementCommand* ifFunc(
	RuntimeData* rt,
	FunctionInfo* pFunctionInfo
)
{
	ElementCommand* pElCurrentFunction = NULL;
	// <func>
	DB_PRINT("Is a function");
	// add the function to the command stack:
	CommandInfo* pCurrentCommandInfo = getbytes(sizeof(CommandInfo));
	pCurrentCommandInfo -> stackHeight0 =
		ListAtomGetSize ( rt -> stack );
	pCurrentCommandInfo -> pFunctionInfo = pFunctionInfo;
	pElCurrentFunction = ListCommandAdd (
			rt->cmdStack,
			pCurrentCommandInfo
	);

	return pElCurrentFunction;
}

ElementCommand* ifRightParenthesis(
	RuntimeData* rt,
	ElementCommand* pElCurrentFunction
)
{
	FunctionInfo* pFunctionInfo = pElCurrentFunction -> pData -> pFunctionInfo;
	t_int paramCount =
		ListAtomGetSize ( rt -> stack )
		- pElCurrentFunction -> pData -> stackHeight0
	;
	if(
		( pFunctionInfo -> paramCount == -1 )
		|| ( pFunctionInfo -> paramCount == paramCount )
	)
	{
		// execute the function:
		callFunction(
				rt,
				pFunctionInfo,
				paramCount
		);
		pElCurrentFunction = ListCommandGetLast ( rt -> cmdStack );
	}
	else
	{
		char buf[256];
		atom_string( &pFunctionInfo->name, buf, 255 );
		post("ERROR: wrong number of parameters for function %s", buf);
	}

	//delete the function from Stack, becaus it has been executed:
	ListCommandDel( rt -> cmdStack, pElCurrentFunction);
	pElCurrentFunction = ListCommandGetLast ( rt -> cmdStack );
	tryToExecuteImmediately(
			rt
	);
	pElCurrentFunction = ListCommandGetLast ( rt -> cmdStack );
	return pElCurrentFunction;
}

void ifValue(
		RuntimeData* rt,
	t_atom* pCurrentToken
)
{
	// put value on stack:
	t_atom* pValue = getbytes(sizeof(t_atom));
	*pValue = *pCurrentToken;
	ListAtomAdd( rt -> stack, pValue);
}

// executes the current code:
void sgScriptObj_exec(
	RuntimeData* rt
)
{
	int countParenthesisRightIgnore = 1;
	BOOL escape = FALSE;
	SymbolTable_Clear( rt -> pSymbolTable );

	ElementCommand* pElCurrentFunction = NULL; // topmost function on the command stack
	t_atom* pCurrentToken=NULL;
	while( (pCurrentToken = lexer_getNextToken(rt)) )
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
			if( ! rt -> skipMode )
				pElCurrentFunction = ifFunc(rt,pFunctionInfo);	
			// eat the "(" that follows:
			if( ! lexer_consumeNextToken(rt, & leftParenthesis) )
			{
				// report syntax error:
				post("ERROR: '(' expected, after ...");
			}
			else if( rt->skipMode )
				(countParenthesisRightIgnore) ++;
		}
		else if ((!escape) && (compareAtoms(pCurrentToken,&rightParenthesis)))
		{
			if( rt->skipMode )
			{
				countParenthesisRightIgnore --;
				if( countParenthesisRightIgnore == 0 )
				{
					rt -> skipMode = FALSE;
					countParenthesisRightIgnore = 1;
				}
				continue;
			}
			// ")"
			if (pElCurrentFunction)
			{
				pElCurrentFunction =
					ifRightParenthesis(rt,pElCurrentFunction);
			}
			else
				post("ERROR: ')' found, but no corresponding 'func ('");
		}
		else
		{
			DB_PRINT("is a value");
			if( rt->skipMode )
				continue;

			// if in escape mode, everything is considered a value:
			if (escape || isValue(pCurrentToken))
			{
				ifValue(rt,pCurrentToken);
			}
			else
			{
				post("ERROR: token is neither function, var, or procedure: '%s'",buf);
			}
			tryToExecuteImmediately(rt);
			pElCurrentFunction = ListCommandGetLast ( rt -> cmdStack );
		}
	}
}

// lexer methods:
t_atom* lexer_getNextToken(
		RuntimeData* rt
)
{
	t_atom* pRet = 0;
	if( rt -> peek < TokenBuf_get_size( rt -> current_prog ) )
	//if( rt -> peek < rt -> currentProgCount )
	{
		pRet = & TokenBuf_get_array( rt -> current_prog )[ rt -> peek ];
		rt -> peek += 1;
	}
	return pRet;
}

TokenFits lexer_consumeNextToken(
	RuntimeData* rt,
	t_atom* pExpectedSym
)
{
	t_atom* pNextToken = lexer_getNextToken( rt );
	return compareAtoms(pNextToken, pExpectedSym );
}

void callFunction(
	RuntimeData* rt,
	FunctionInfo* pFunctionInfo,
	t_int countParam
)
{
	// first copy params:
	t_atom* pArgs = getbytes( sizeof(t_atom )* countParam);
	{
		ElementAtom* pElOpNext = ListAtomGetLast( rt -> stack);
		//pArrayParam[countParam-1] = pElOpNext -> pData;
		for ( int i=countParam-1; i>=0; i--)
		{
			pArgs[i] = *(pElOpNext-> pData);
			pElOpNext = ListAtomGetPrev( rt -> stack, pElOpNext);
		}
	}
	// ... delete them from stack
	for ( int i=0; i<countParam; i++)
	{
		ElementAtom* pElParam = ListAtomGetLast ( rt -> stack );
		ListAtomDel( rt -> stack, pElParam );
	}
	DB_PRINT("callFunction called");
	// call command:
	(pFunctionInfo -> pFunc) (
			rt,
			countParam,
			pArgs
	);
	// free array of parameters:
	freebytes( pArgs, sizeof(t_atom ) * countParam );
}

BOOL isValue(t_atom* pToken)
{
	if( atomEqualsString( pToken, "(" ) )
		return FALSE;
	return TRUE;
}

void tryToExecuteImmediately(
		RuntimeData* rt
)
{
	ElementCommand* pElCurrentFunction = ListCommandGetLast ( rt -> cmdStack );
	if( !pElCurrentFunction )
		return ;
	// if the topmost command is a dont-read-all-parameters-command:
	if( pElCurrentFunction->pData -> pFunctionInfo -> executeAfter != -1)
	{
		DB_PRINT("trying to executed immediately...:");
		FunctionInfo* pFunctionInfo = pElCurrentFunction->pData -> pFunctionInfo;
		// check if there are enough values on stack now to call the next function
		t_int paramCount =
			ListAtomGetSize ( rt -> stack )
			- pElCurrentFunction -> pData -> stackHeight0
		;
		if( paramCount== pFunctionInfo -> executeAfter )
		{
			callFunction(rt, pFunctionInfo, paramCount);
		}
		else if( paramCount > pFunctionInfo -> executeAfter)
		{
			char buf[256];
			atom_string(& pFunctionInfo->name, buf, 256);
			post("ERROR: wrong number of parameters for %s", buf);
		}
		ListCommandDel( rt -> cmdStack, pElCurrentFunction);
	}
}

