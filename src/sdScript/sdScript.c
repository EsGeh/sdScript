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
void script_obj_on_set_bang(
		t_script_obj* pThis,
		t_symbol* s,
		int argc,
		t_atom* argv
);
void script_obj_on_del_program(
		t_script_obj* pThis,
		t_symbol* s
);
void script_obj_on_set_var(
	t_script_obj* pThis,
	t_symbol* s,
	int argc,
	t_atom* argv
);
void script_obj_on_exec(
	t_script_obj* pThis,
	t_symbol* prog_name,
	int argc,
	t_atom* argv
);

// ****************************
// low level execution control:
// ****************************

// exec as subroutine:
int script_obj_exec_sub_program(
	t_script_obj* this,
	t_symbol* prog_name
);

// executes the current code:
void script_obj_exec_current(
	t_script_obj* this
);

//*********************************************
// utils:

RuntimeData* script_obj_init_prog(
	t_script_obj* this,
	t_symbol* prog_name
);

void utils_push_cmd(
	RuntimeData* rt,
	FunctionInfo* pFunctionInfo
);

void utils_pop_cmd_and_exec(
	RuntimeData* rt
);

void utils_push_value(
	RuntimeData* rt,
	t_atom* pCurrentToken
);

BOOL utils_is_value(t_atom* pToken);

// calls the function that is on the command_stack
void utils_call_function(
		RuntimeData* rt,
		FunctionInfo* pFunctionInfo,
		t_int countParam
);

/*
  checks if the next command is one that can be executed before all parameters have been read. If this is the case, and if there are enough parameters on stack for it, executed it.
*/
void utils_try_to_exec_immediately(
		RuntimeData* rt
);

//*********************************************
// lexer methods:

typedef BOOL TokenFits;

t_atom* lexer_getNextToken(
		RuntimeData* rt
);
TokenFits lexer_consumeNextToken(
		RuntimeData* rt,
		t_atom* pExpectedSym
);

//*********************************************
// implementation
//*********************************************


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
		(t_method )script_obj_on_set_bang,
		gensym("prog_set_bang"),
		A_GIMME
	);
	class_addmethod(
		class,
		(t_method )script_obj_on_del_program,
		gensym("prog_del"),
		A_SYMBOL,
		0
	);
	class_addanything(
		class,
		(t_method )script_obj_on_exec
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
	x->symbol_table = symtab_init();
	ProgStackInit( & x-> program_stack );
	OutputBuf_init( & x->output_buffer );

	x->clock = clock_new( 
			x,
			(t_method )script_obj_exec_current
	);

	x -> pOutlet = outlet_new( & x -> obj, &s_list);
	inlet_new(
		& x -> obj,
		& x -> obj . ob_pd,
		gensym("list"),
		gensym("prog_set_bang")
	);
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
	Scope_init(
			& x -> global_scope,
			VARS_HASH_SIZE
	);
	for( int i=0; i<argc; i++)
	{

		Atoms* new_var = getbytes( sizeof( Atoms ) );
		Atoms_init( new_var );
		t_atom new_atom;
		SETFLOAT( & new_atom, 0 );
		Atoms_append( new_var, new_atom );
		Scope_insert(
				&  x -> global_scope,
				atom_getsymbol( & argv[i] ),
				new_var
		);
	}
	/*
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
	*/
	return (void* )x;
}

void t_script_obj_exit(
	t_script_obj* pThis
)
{

	Scope_exit( & pThis -> global_scope );

	if( pThis->clock)
		clock_free( pThis -> clock );

	DB_PRINT("removing sgScript object...");
	ProgStackExit( & pThis-> program_stack );
	Programs_exit( & pThis->programs );
	symtab_exit( pThis -> symbol_table );

	OutputBuf_exit( & pThis -> output_buffer );
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

void script_obj_on_set_bang(
		t_script_obj* pThis,
		t_symbol* s,
		int argc,
		t_atom* argv
)
{
	DB_PRINT("script_obj_on_set_bang");

	t_symbol* prog_name = gensym( "bang" );
	int prog_size = argc;

	TokenBuf* new_program = getbytes( sizeof( TokenBuf ) );
	TokenBuf_init( new_program, prog_size);
	
	memcpy(
		TokenBuf_get_array( new_program ),
		& argv[0],
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

void script_obj_on_set_var(
	t_script_obj* pThis,
	t_symbol* s,
	int argc,
	t_atom* argv
)
{
	DB_PRINT("script_obj_on_set_var");
	if( argc < 1 )
	{
		pd_error( pThis, "wrong syntax: expected 'set <global_var> val1 ...'" );
		return;
	}

	t_symbol* var_name = atom_getsymbol( & argv[0] );
	Atoms* value = Scope_get(
			& pThis -> global_scope,
			var_name
	);
	Atoms_set_size(
			value,
			argc-1
	);
	memcpy(
			Atoms_get_array( value ),
			& argv[1],
			sizeof( t_atom ) * (argc-1)
	);
}

void script_obj_on_exec(
	t_script_obj* pThis,
	t_symbol* prog_name,
	int argc,
	t_atom* argv
)
{
	if(
			argc != 0
	)
	{
		pd_error( pThis, "sdScript: wrong syntax!"  );
		return;
	}
	// if some program is still loaded, clean it up:
	if(
			ProgStackGetSize( & pThis -> program_stack )
	)
	{
		ProgStackClear( & pThis -> program_stack );
		// TODO: stop all clocks!
	}

	int sub_prog_ret = script_obj_exec_sub_program(
			pThis,
			prog_name
	);
	if( sub_prog_ret != 0)
	{
		t_atom a_prog_name;
		SETSYMBOL( & a_prog_name, prog_name );
		char buffer[256];
		atom_string( & a_prog_name, buffer, 255 );
		pd_error( pThis, "sdScript: no such comand or program: '%s'", buffer );
	}
}


// ****************************
// low level execution control:
// ****************************

// exec as subroutine:
int script_obj_exec_sub_program(
	t_script_obj* this,
	t_symbol* prog_name
)
{

	RuntimeData* rt = script_obj_init_prog(
			this,
			prog_name
	);
	if(
			!rt
	)
	{
		return -1; 
	}
	script_obj_exec_current(
			this
	);
	// if the the sub program set itself asleep:
	if(
		rt->delay
	)
	{
		// start a timer to continue execution:
		clock_delay( this->clock, rt->delay );
	}
	return 0;
}

RuntimeData* script_obj_init_prog(
	t_script_obj* this,
	t_symbol* prog_name
)
{
	TokenBuf* prog = Programs_get(
			& this -> programs,
			prog_name
	);
	if( !prog )
	{
		return NULL;
	}

	RuntimeData* rt = getbytes( sizeof( RuntimeData ) );

	ListAtomInit( & rt -> stack );
	ListCommandInit( & rt -> command_stack  );
	Scope* scope =
		symtab_add_scope(
				this -> symbol_table,
				prog_name
		);

	(*rt) = (RuntimeData) {
		.current_prog_name = prog_name,
		.current_prog = prog,
		.scope = scope,
		.global_scope = & this->global_scope,
		.peek = 0,

		.countParenthesisRightIgnore = 1,
		.escape = FALSE,
		.skipMode = FALSE,
		.delay = 0,

		.script_obj = this
	};
	ProgStackAdd(
			& this->program_stack,
			rt
	);
	return rt;

	/*
	TokenBuf* prog = Programs_get(
			& this -> programs,
			prog_name
	);
	if( !prog )
	{
		return;
	}

	RuntimeData* rt = getbytes( sizeof( RuntimeData ) );
	ListAtom* stack = getbytes( sizeof( ListAtom ) );
	ListCommand* cmd_stack = getbytes( sizeof( ListCommand ) );
	OutputBuf* output_buffer = getbytes( sizeof( OutputBuf ) );

	ListAtomInit( stack );
	ListCommandInit( cmd_stack );
	OutputBuf_init( output_buffer );

	(*rt) = (RuntimeData) {
		.current_prog_name = prog_name,
		.current_prog = prog,
		.pSymbolTable = this-> pSymbolTable,
		.stack = stack,
		.command_stack = cmd_stack,
		.peek = 0,
		.outputBuffer = output_buffer,
		.script_obj = this,

		.countParenthesisRightIgnore = 1,
		.escape = FALSE,
		.skipMode = FALSE,
		.delay = 0
	};
	SymbolTable_Clear( rt -> pSymbolTable );
	this->rt = rt;
	*/
}

// executes the current code:
void script_obj_exec_current(
	t_script_obj* this
)
{
	ProgStackEl* last_prog_el = ProgStackGetLast(
			& this-> program_stack
	);
	RuntimeData* rt = last_prog_el -> pData;

	rt -> delay = 0;

	t_atom* pCurrentToken=NULL;
	while(
		( rt->delay == 0 )
		&&
		(pCurrentToken = lexer_getNextToken(rt))
	)
	{
		char buf[256];
		atom_string(pCurrentToken, buf, 256);
		DB_PRINT("current Token: '%s'", buf);

		// 1. switch escape mode on/off:
		if( compareAtoms(pCurrentToken, &escapeBegin) )
		{
			rt->escape = TRUE;
		}
		else if( compareAtoms(pCurrentToken, &escapeEnd) )
		{
			if (rt->escape == FALSE)
				post("ERROR: #] found, without corresponding #[!");
			rt->escape = FALSE;
		}

		else if( ! rt->escape)
		{
			FunctionInfo* pFunctionInfo = getFunctionInfo( pCurrentToken );
			// <Command>
			if( pFunctionInfo )
			{
				if( ! rt -> skipMode )
					utils_push_cmd(rt,pFunctionInfo);	
				// eat the "(" that follows:
				if( ! lexer_consumeNextToken(rt, & leftParenthesis) )
				{
					// report syntax error:
					post("ERROR: '(' expected, after ...");
				}
				else if( rt->skipMode )
					(rt->countParenthesisRightIgnore) ++;
			}
			// ')'
			else if( compareAtoms(pCurrentToken,&rightParenthesis) )
			{
				if( rt->skipMode )
				{
					rt->countParenthesisRightIgnore --;
					if( rt->countParenthesisRightIgnore == 0 )
					{
						rt -> skipMode = FALSE;
						rt->countParenthesisRightIgnore = 1;
					}
				}
				else if( ListCommandGetSize( & rt->command_stack ) )
				{
					utils_pop_cmd_and_exec( rt );
				}
				else
					post("ERROR: ')' found, but no corresponding 'func ('");
			}
			else if( rt->skipMode )
			{
				// just skip
			}
			// <value>
			else if (utils_is_value(pCurrentToken))
			{
				utils_push_value(rt,pCurrentToken);
			}
			else
			{
				post("ERROR: token is neither function, var, or procedure: '%s'",buf);
			}
			utils_try_to_exec_immediately(rt);
		}
		else // we are in escape mode:
		{
			DB_PRINT("is a value");
			if( ! rt->skipMode )
			{
				// if in escape mode, everything is considered a value:
				utils_push_value(rt,pCurrentToken);
				utils_try_to_exec_immediately(rt);
			}
		}
	}
	if( ! rt -> delay )
	{
		ProgStackDel(
				& this -> program_stack,
				last_prog_el
		);
	}
}

//*********************************************
// utils:

void utils_push_cmd(
	RuntimeData* rt,
	FunctionInfo* pFunctionInfo
)
{
	// <func>
	DB_PRINT("Is a function");
	// add the function to the command stack:
	CommandInfo* pCurrentCommandInfo = getbytes(sizeof(CommandInfo));
	pCurrentCommandInfo -> stackHeight0 =
		ListAtomGetSize ( & rt -> stack );
	pCurrentCommandInfo -> pFunctionInfo = pFunctionInfo;
	ListCommandAdd (
			& rt->command_stack,
			pCurrentCommandInfo
	);
}

void utils_pop_cmd_and_exec(
	RuntimeData* rt
	//ElementCommand* pElCurrentFunction
)
{
	ElementCommand* pElCurrentFunction =
		ListCommandGetLast( & rt->command_stack );
	FunctionInfo* pFunctionInfo = pElCurrentFunction -> pData -> pFunctionInfo;
	t_int paramCount =
		ListAtomGetSize ( & rt -> stack )
		- pElCurrentFunction -> pData -> stackHeight0
	;
	if(
		( pFunctionInfo -> paramCount == -1 )
		|| ( pFunctionInfo -> paramCount == paramCount )
	)
	{
		// execute the function:
		utils_call_function(
				rt,
				pFunctionInfo,
				paramCount
		);
		pElCurrentFunction = ListCommandGetLast ( & rt -> command_stack );
	}
	else
	{
		char buf[256];
		atom_string( &pFunctionInfo->name, buf, 255 );
		post("ERROR: wrong number of parameters for function %s", buf);
	}

	//delete the function from Stack, because it has been executed:
	ListCommandDel( & rt -> command_stack, pElCurrentFunction);
	utils_try_to_exec_immediately(
			rt
	);
}

void utils_push_value(
	RuntimeData* rt,
	t_atom* pCurrentToken
)
{
	// put value on stack:
	t_atom* pValue = getbytes(sizeof(t_atom));
	*pValue = *pCurrentToken;
	ListAtomAdd( & rt -> stack, pValue);
}

BOOL utils_is_value(t_atom* pToken)
{
	if( atomEqualsString( pToken, "(" ) )
		return FALSE;
	return TRUE;
}

// calls the function that is on the command_stack
void utils_call_function(
	RuntimeData* rt,
	FunctionInfo* pFunctionInfo,
	t_int countParam
)
{
	// first copy params:
	t_atom* pArgs = getbytes( sizeof(t_atom )* countParam);
	{
		ElementAtom* pElOpNext = ListAtomGetLast( & rt -> stack);
		for ( int i=countParam-1; i>=0; i--)
		{
			pArgs[i] = *(pElOpNext-> pData);
			pElOpNext = ListAtomGetPrev( & rt -> stack, pElOpNext);
		}
	}
	// ... delete them from stack
	for ( int i=0; i<countParam; i++)
	{
		ElementAtom* pElParam = ListAtomGetLast ( & rt -> stack );
		ListAtomDel( & rt -> stack, pElParam );
	}
	DB_PRINT("utils_call_function called");
	// call command:
	(pFunctionInfo -> pFunc) (
			rt,
			countParam,
			pArgs
	);
	// free array of parameters:
	freebytes( pArgs, sizeof(t_atom ) * countParam );
}

void utils_try_to_exec_immediately(
		RuntimeData* rt
)
{
	ElementCommand* pElCurrentFunction =
		ListCommandGetLast ( & rt -> command_stack );
	if( !pElCurrentFunction )
		return ;
	// if the topmost command is a dont-read-all-parameters-command:
	if( pElCurrentFunction->pData -> pFunctionInfo -> executeAfter != -1)
	{
		DB_PRINT("trying to executed immediately...:");
		FunctionInfo* pFunctionInfo = pElCurrentFunction->pData -> pFunctionInfo;
		// check if there are enough values on stack now to call the next function
		t_int paramCount =
			ListAtomGetSize ( & rt -> stack )
			- pElCurrentFunction -> pData -> stackHeight0
		;
		if( paramCount== pFunctionInfo -> executeAfter )
		{
			utils_call_function(rt, pFunctionInfo, paramCount);
		}
		else if( paramCount > pFunctionInfo -> executeAfter)
		{
			char buf[256];
			atom_string(& pFunctionInfo->name, buf, 256);
			post("ERROR: wrong number of parameters for %s", buf);
		}
		ListCommandDel( & rt -> command_stack, pElCurrentFunction);
	}
}

//*********************************************
// lexer methods:

t_atom* lexer_getNextToken(
		RuntimeData* rt
)
{
	t_atom* pRet = 0;
	if( rt -> peek < TokenBuf_get_size( rt -> current_prog ) )
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
