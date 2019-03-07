#include "Function.h"

#include "SymbolTable.h"
#include "LinkedList.h"
#include "sdScript.h"

#include <stdlib.h>


DECL_DYN_ARRAY(FunctionInfos, FunctionInfo)
DEF_DYN_ARRAY(FunctionInfos, FunctionInfo)


FunctionInfo* pNOP;
FunctionInfo* pRETURN_ALL;

#define PFUNCTION_HEADER(name) \
void name(t_rt* rt, t_int countArgs, t_atom* pArgs)

PFUNCTION_HEADER(add);
PFUNCTION_HEADER(sub);
PFUNCTION_HEADER(mul);
PFUNCTION_HEADER(div_);
PFUNCTION_HEADER(mod);
// output:
PFUNCTION_HEADER(print);
PFUNCTION_HEADER(pack);
PFUNCTION_HEADER(out);
// variables:
PFUNCTION_HEADER(addVar);
PFUNCTION_HEADER(getVar);
PFUNCTION_HEADER(getVarA);
PFUNCTION_HEADER(setVar);
PFUNCTION_HEADER(setVarA);
// main variables:
PFUNCTION_HEADER(addMainVar);
PFUNCTION_HEADER(clearMain);
// conditionality:
PFUNCTION_HEADER(if_);
// sgScales:
PFUNCTION_HEADER(sgFunc);
PFUNCTION_HEADER(sgScale);
// boolean operators:
PFUNCTION_HEADER(and_);
PFUNCTION_HEADER(or_);
PFUNCTION_HEADER(not_);
// comparison operators:
PFUNCTION_HEADER(isEqual);
PFUNCTION_HEADER(isNotEqual);
PFUNCTION_HEADER(isLessThan);
PFUNCTION_HEADER(isGreaterThan);
PFUNCTION_HEADER(isLessOrEqual);
PFUNCTION_HEADER(isGreaterOrEqual);
// Set operations:
PFUNCTION_HEADER(setify);
PFUNCTION_HEADER(card);
PFUNCTION_HEADER(setOp);
PFUNCTION_HEADER(contains);
PFUNCTION_HEADER(calcTransp);

PFUNCTION_HEADER(addA);
PFUNCTION_HEADER(subA);
PFUNCTION_HEADER(mulA);
PFUNCTION_HEADER(divA);
PFUNCTION_HEADER(modA);
// random
PFUNCTION_HEADER(sgMinMax);
PFUNCTION_HEADER(random_);

// just throw away all parameters:
PFUNCTION_HEADER(nop);
PFUNCTION_HEADER(returnAll);

PFUNCTION_HEADER(rndInt);
PFUNCTION_HEADER(inc);
PFUNCTION_HEADER(dec);
PFUNCTION_HEADER(rndIntUnequal);

//sgPack:
PFUNCTION_HEADER( sgpackType );
PFUNCTION_HEADER( sgpackCount );
PFUNCTION_HEADER( sgpackParams );

// like the abstraction it converts one OR MANY sgPacks
// from the "humane" syntax using "(" and ")" to the 
// inhumane one, example:
// pack ( bli bla blubb ) becomes: pack 3 bli bla blubb
PFUNCTION_HEADER( sgPackFromHuman );

// sgData (lists of sgPacks):
PFUNCTION_HEADER( sgDataGetPackFromType );
PFUNCTION_HEADER( sgDataGetPackFromTypeRest );
PFUNCTION_HEADER( sgDataGetPackFromIndex );
// returns the first sgPack
PFUNCTION_HEADER( sgDataGetFirst );
// returns all sgPacks but the first sgPack
PFUNCTION_HEADER( sgDataGetRest );
// deletes first pack from a list of sgPacks and returns it

PFUNCTION_HEADER( delay );
// deletes first pack from a list of sgPacks and returns it

// call an other function as sub routine:
PFUNCTION_HEADER( callFunction );

#define ADD_FUNCTION( NAME,PFUNC,PARAMCOUNT,EXECAFTER) \
{ \
	t_atom atomName; \
	SETSYMBOL( &atomName, gensym( NAME ) ); \
 \
	FunctionInfo func = { \
		.name = atomName, \
		.paramCount = PARAMCOUNT, \
		.executeAfter = EXECAFTER, \
		.pFunc = PFUNC \
	}; \
	FunctionInfos_append( &function_infos, \
			func \
	); \
}


FunctionInfos function_infos;

void functions_init()
{
	FunctionInfos_init( & function_infos );

	ADD_FUNCTION("NOP",&nop,-1,-1);
	ADD_FUNCTION("Add",&add,2,-1);
	ADD_FUNCTION("Sub",&sub,2,-1);
	ADD_FUNCTION("Mul",&mul,2,-1);
	ADD_FUNCTION("Div",&div_,2,-1);
	ADD_FUNCTION("Mod",&mod,2,-1);
	ADD_FUNCTION("Print",&print,-1,-1);
	ADD_FUNCTION("Pack",&pack,-1,-1);
	ADD_FUNCTION("Out",&out,0,-1);
	ADD_FUNCTION("Var",&addVar,-1,-1);
	ADD_FUNCTION("Get",&getVar,1,-1);
	ADD_FUNCTION("GetA",&getVarA,2,-1);
	ADD_FUNCTION("Set",&setVar,-1,-1);
	ADD_FUNCTION("SetA",&setVarA,-1,-1);
	ADD_FUNCTION("If",&if_,-1,1);
	ADD_FUNCTION("VarMain",&addMainVar,-1,-1);
	ADD_FUNCTION("ClearMain",&clearMain,0,-1);
	// sgScales:
	// a | b , c , x
	ADD_FUNCTION("sgFunc",&sgFunc,4,-1);
	// # , a | b , c 
	ADD_FUNCTION("sgScale",&sgScale,4,-1);
	// boolean operators:

	ADD_FUNCTION("&&",&and_,2,-1);
	ADD_FUNCTION("||",&or_,2,-1);
	ADD_FUNCTION("!",&not_,1,-1);
	// comparison operators:
	ADD_FUNCTION("==",&isEqual,2,-1);
	ADD_FUNCTION("!=",&isNotEqual,2,-1);
	ADD_FUNCTION("<",&isLessThan,2,-1);
	ADD_FUNCTION(">",&isGreaterThan,2,-1);
	ADD_FUNCTION("<=",&isLessOrEqual,2,-1);
	ADD_FUNCTION(">=",&isGreaterOrEqual,2,-1);
	// Set operations:
	ADD_FUNCTION("Setify",&setify,-1,-1);
	ADD_FUNCTION("Card",&card,-1,-1);
	ADD_FUNCTION("SetOp",&setOp,-1,-1);
	ADD_FUNCTION("CalcTransp",&calcTransp,-1,-1);
	ADD_FUNCTION("Contains",&contains,-1,-1);
	ADD_FUNCTION("AddA",&addA,-1,-1);
	ADD_FUNCTION("SubA",&subA,-1,-1);
	ADD_FUNCTION("MulA",&mulA,-1,-1);
	ADD_FUNCTION("DivA",&divA,-1,-1);
	ADD_FUNCTION("ModA",&modA,-1,-1);
	ADD_FUNCTION("Rnd",&random_,2,-1);
	ADD_FUNCTION("MinMax",&sgMinMax,3,-1);
	ADD_FUNCTION("RETURN_ALL",&returnAll,-1,-1);
	ADD_FUNCTION("RndI",&rndInt,2,-1);
	ADD_FUNCTION("Inc",&inc,1,-1);
	ADD_FUNCTION("Dec",&dec,1,-1);
	ADD_FUNCTION("RndIUnequal",&rndIntUnequal,-1,-1);
	ADD_FUNCTION("sgPackGetType",&sgpackType,-1,-1);
	ADD_FUNCTION("sgPackGetCount",&sgpackCount,-1,-1);
	ADD_FUNCTION("sgPackGetParams",&sgpackParams,-1,-1);
	ADD_FUNCTION("sgPackFromHuman",&sgPackFromHuman,-1,-1);
	ADD_FUNCTION("sgDataGetPackFromType",&sgDataGetPackFromType,-1,-1);
	ADD_FUNCTION("sgDataGetPackFromTypeRest",&sgDataGetPackFromTypeRest,-1,-1);
	ADD_FUNCTION("sgDataGetPackFromIndex",&sgDataGetPackFromIndex,-1,-1);
	ADD_FUNCTION("sgDataGetFirst",&sgDataGetFirst,-1,-1);
	ADD_FUNCTION("sgDataGetRest",&sgDataGetRest,-1,-1);
	ADD_FUNCTION("Delay", &delay, 1, -1 );
	ADD_FUNCTION("Call", &callFunction, 1, -1);

	t_atom atom_temp;
	SETSYMBOL( &atom_temp, gensym( "NOP" ) );
	pNOP = getFunctionInfo( &atom_temp );
	SETSYMBOL( &atom_temp, gensym( "RETURN_ALL" ) );
	pRETURN_ALL = getFunctionInfo( &atom_temp );
}

void functions_exit()
{
	FunctionInfos_exit( &function_infos );
}

FunctionInfo* getFunctionInfo(t_atom* pName)
{
	for(int i=0; i<FunctionInfos_get_size( &function_infos ); i++)
	{
		FunctionInfo* current = & FunctionInfos_get_array( &function_infos )[i];
		if( compareAtoms( & current->name, pName))
			return current;
	}
	return NULL;
}

FunctionInfo* get_NOP() { return pNOP; }
FunctionInfo* get_RETURN_ALL() { return pRETURN_ALL; }

/*****************************************
 * function implementations:
 *****************************************/

PFUNCTION_HEADER( add )
{
	DB_PRINT("add");
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, atom_getfloat(& pArgs[0]) + atom_getfloat(& pArgs[1]));
	//push result on stack:
	ListAtomAdd( & rt -> stack, pResult);
}


PFUNCTION_HEADER( sub )
{
	DB_PRINT("sub");
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, atom_getfloat(&pArgs[0]) - atom_getfloat(&pArgs[1]));
	//push result on stack:
	ListAtomAdd( & rt -> stack, pResult);
}
PFUNCTION_HEADER( mul )
{
	DB_PRINT("mul");
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, atom_getfloat(&pArgs[0]) * atom_getfloat(& pArgs[1]));
	//push result on stack:
	ListAtomAdd( & rt -> stack, pResult);
}
PFUNCTION_HEADER( div_ )
{
	DB_PRINT("div");
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, atom_getfloat(&pArgs[0]) / atom_getfloat(& pArgs[1]));
	//push result on stack:
	ListAtomAdd( & rt -> stack, pResult);
}
PFUNCTION_HEADER( mod )
{
	DB_PRINT("div");
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, (t_int )atom_getfloat(&pArgs[0]) % (t_int )atom_getfloat(& pArgs[1]));
	//push result on stack:
	ListAtomAdd( & rt -> stack, pResult);
}

PFUNCTION_HEADER( print )
{
	for ( int i=0; i<countArgs; i++)
	{
		OutputBuf_append( & rt->script_obj -> output_buffer, pArgs[i] );
	}
}

PFUNCTION_HEADER( pack )
{
	t_atom** pResult = getbytes(sizeof(t_atom* )* (countArgs + 2));
	pResult[0] = getbytes( sizeof(t_atom) );
	*pResult[0] = pArgs[0];
	t_atom atomSizePack;
	SETFLOAT( &atomSizePack, countArgs-1 );
	pResult[1] = getbytes( sizeof(t_atom) );
	*pResult[1] = atomSizePack;
	for(int i=0; i< countArgs-1; i++)
	{
		pResult[2+i] = getbytes( sizeof(t_atom) );
		* pResult[2+i] = pArgs[i+1];
	}
	ListAtomAdd( & rt -> stack, pResult[0]);
	ListAtomAdd( & rt -> stack, pResult[1]);
	for(int i=0; i< countArgs-1; i++)
	{
		ListAtomAdd( & rt -> stack, pResult[2+i]);
	}
	freebytes( pResult, sizeof(t_atom* ) * (countArgs+2));
}

PFUNCTION_HEADER( out )
{
	sdScript_output(
		rt -> script_obj,
		OutputBuf_get_size( & rt -> script_obj -> output_buffer ),
		OutputBuf_get_array( & rt -> script_obj -> output_buffer )
		);
	OutputBuf_clear( & rt -> script_obj -> output_buffer );
}

PFUNCTION_HEADER( addVar )
{
	if( countArgs == 0 )
	{
		post("WARNING: addVar with zero parameters called!");
		return;
	}

	Atoms* new_var = getbytes( sizeof( Atoms ) );
	Atoms_init( new_var );
	Atoms_set_size( new_var, countArgs-1 );
	memcpy(
			Atoms_get_array( new_var ),
			& pArgs[1],
			sizeof( t_atom ) * (countArgs-1)
	);
	Scope_insert(
			rt -> scope,
			atom_getsymbol( &pArgs[0] ),
			new_var
	);
}


PFUNCTION_HEADER( getVar )
{
	Atoms* value =
		Scope_get(
				rt -> scope,
				atom_getsymbol( & pArgs[0] )
		);
	if( ! value )
	{
		value =
			Scope_get(
					rt -> global_scope,
					atom_getsymbol( & pArgs[0] )
			);
	}
	if( ! value )
	{
		char buf[256];
		atom_string( & pArgs[0], buf, 256 );
		post("ERROR: getVar: couldn't find variable \"%s\"", buf);
		return;
	}

	for( int i=0; i< Atoms_get_size( value ); i++ )
	{
		t_atom* pResult = getbytes(sizeof(t_atom));
		(*pResult) = Atoms_get_array( value )[i];
		ListAtomAdd( & rt -> stack, pResult);
	}
}

PFUNCTION_HEADER( getVarA )
{
	Atoms* value =
		Scope_get(
				rt -> scope,
				atom_getsymbol( & pArgs[0] )
		);
	if( ! value )
	{
		value =
			Scope_get(
					rt -> global_scope,
					atom_getsymbol( & pArgs[0] )
			);
	}
	if( ! value )
	{
		char buf[256];
		atom_string( & pArgs[0], buf, 256 );
		post("ERROR: getVar: couldn't find variable \"%s\"", buf);
		return;
	}
	t_int index = atom_getint( & pArgs[1] );
	if( index < 0 || index >= Atoms_get_size( value ) )
	{
		post("ERROR: getVarA: index out of bounds: \"%i\"", (int )index);
		return;
	}

	t_atom* pResult = getbytes(sizeof(t_atom));
	(*pResult) = Atoms_get_array( value )[index];
	ListAtomAdd( & rt -> stack, pResult);
}

PFUNCTION_HEADER( setVar )
{
	if( countArgs == 0 )
	{
		post("WARNING: setVar called with zero parameters");
		return;
	}
	DB_PRINT("setVar called with %i args", countArgs);

	Atoms* value =
		Scope_get(
				rt -> scope,
				atom_getsymbol( & pArgs[0] )
		);
	if( ! value )
	{
		value =
			Scope_get(
					rt -> global_scope,
					atom_getsymbol( & pArgs[0] )
			);
	}
	if( ! value )
	{
		char buf[256];
		atom_string( & pArgs[0], buf, 256 );
		post("ERROR: variable \"%s\" not found!", buf);
		return;
	}
	Atoms_set_size(
			value,
			countArgs-1
	);
	memcpy(
			Atoms_get_array( value ),
			& pArgs[1],
			sizeof( t_atom ) * (countArgs-1)
	);
}

PFUNCTION_HEADER( setVarA )
{
	Atoms* value =
		Scope_get(
				rt -> scope,
				atom_getsymbol( & pArgs[0] )
		);
	if( ! value )
	{
		value =
			Scope_get(
					rt -> global_scope,
					atom_getsymbol( & pArgs[0] )
			);
	}
	if( ! value )
	{
		char buf[256];
		atom_string( & pArgs[0], buf, 256 );
		post("ERROR: variable \"%s\" not found!", buf);
		return;
	}
	t_int index = atom_getfloat( &pArgs[1] );
	t_atom* new_value = & pArgs[2];
	if( index < 0 || index >= Atoms_get_size( value ) )
	{
		post("ERROR: setVarA: index out of bounds: \"%i\"", (int )index);
		return;
	}
	Atoms_get_array( value )[index] = *new_value;
}

PFUNCTION_HEADER( addMainVar )
{
	if( countArgs == 0 )
	{
		post("WARNING: addVar with zero parameters called!");
		return;
	}

	Atoms* new_var = getbytes( sizeof( Atoms ) );
	Atoms_init( new_var );
	Atoms_set_size( new_var, countArgs-1 );
	memcpy(
			Atoms_get_array( new_var ),
			& pArgs[1],
			sizeof( t_atom ) * (countArgs-1)
	);
	Scope_insert(
			rt -> global_scope,
			atom_getsymbol( &pArgs[0] ),
			new_var
	);
}

PFUNCTION_HEADER( clearMain )
{
	Scope_clear(
			rt -> global_scope
	);
}

PFUNCTION_HEADER( if_ )
{
	if( atom_getfloat(& pArgs[0]) )
	{
		CommandInfo* pCurrentCommandInfo = getbytes(sizeof(CommandInfo));
		pCurrentCommandInfo -> stackHeight0 = ListAtomGetSize ( & rt -> stack );
		pCurrentCommandInfo -> pFunctionInfo = get_RETURN_ALL();
		ListCommandAdd( & rt -> command_stack, pCurrentCommandInfo);
	}
	else
	{
		rt->skipMode = TRUE;
	}
}

// sgScales:
PFUNCTION_HEADER( sgFunc )
{
	t_float x = atom_getfloat( & pArgs[0] );
	t_float stepAdd = atom_getfloat( & pArgs[1] );
	t_float step0 = atom_getfloat( & pArgs[2] );
	t_float c = atom_getfloat( & pArgs[3] );
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, c + stepAdd/2 * x*x + ( step0 - stepAdd/2 ) * x );
	ListAtomAdd( & rt -> stack, pResult);
}
PFUNCTION_HEADER( sgScale )
{
	t_float n = atom_getfloat( & pArgs[0] );
	t_float stepAdd = atom_getfloat( & pArgs[1] );
	t_float step0 = atom_getfloat( & pArgs[2] );
	t_float c = atom_getfloat( & pArgs[3] );

	for( int i=0; i<n; i++ )
	{
		t_atom* pResult = getbytes(sizeof(t_atom));
		SETFLOAT( pResult, c + stepAdd/2 * i*i + ( step0 - stepAdd/2 ) * i );
		ListAtomAdd( & rt -> stack, pResult);
	}
}

// boolean operators:
PFUNCTION_HEADER( and_ )
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a && b );
	ListAtomAdd( & rt -> stack, pResult);
}
PFUNCTION_HEADER( or_ )
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a || b );
	ListAtomAdd( & rt -> stack, pResult);
}
PFUNCTION_HEADER( not_ )
{
	t_float a = atom_getfloat( & pArgs[0] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, ! a );
	ListAtomAdd( & rt -> stack, pResult);
}

// comparison operators:
PFUNCTION_HEADER( isEqual )
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a == b );
	ListAtomAdd( & rt -> stack, pResult);
}
PFUNCTION_HEADER( isNotEqual )
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a != b );
	ListAtomAdd( & rt -> stack, pResult);

}
PFUNCTION_HEADER( isLessThan )
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a < b );
	ListAtomAdd( & rt -> stack, pResult);
}
PFUNCTION_HEADER( isGreaterThan )
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a > b );
	ListAtomAdd( & rt -> stack, pResult);
}
PFUNCTION_HEADER( isLessOrEqual )
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a <= b );
	ListAtomAdd( & rt -> stack, pResult);
}
PFUNCTION_HEADER( isGreaterOrEqual )
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a >= b );
	ListAtomAdd( & rt -> stack, pResult);
}

typedef enum ESetOp { UNION, MINUS } SetOp;
// Set operations:
BOOL AtomListCompareAtoms(t_atom* pInList, t_atom* p)
{
	return compareAtoms(pInList, p);
}
BOOL setContains(t_int count, t_atom* set, t_atom* element);
BOOL listContains(ListAtom* pList, t_atom* pElement);

PFUNCTION_HEADER( setify )
{
	for( int i=0; i<countArgs; i++ )
	{
		t_atom* pCurrent = & pArgs[i];
		if( ! setContains( i-1, pArgs, pCurrent ) )
		{
			t_atom* pResult = getbytes(sizeof(t_atom));
			*pResult = *pCurrent;
			ListAtomAdd( & rt -> stack, pResult);
		}
	}
}
PFUNCTION_HEADER( card )
{
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, countArgs );
	ListAtomAdd( & rt -> stack, pResult);
}

PFUNCTION_HEADER( setOp )
{
	ListAtom listReturn;
	ListAtomInit( & listReturn );
	// brute force.
	// a better solution would be to sort both lists, and then to merge them:
	t_int countFirst = 0;
	BOOL afterOp = FALSE;
	SetOp op;
	for( int i=0; i<countArgs; i++ )
	{
		t_atom* pCurrent = & pArgs[i];
		if( afterOp == FALSE )
		{
			if( atomEqualsString( pCurrent, "union" ) )
			{
				op = UNION;
				afterOp = TRUE;
			}
			else if( atomEqualsString( pCurrent, "minus" ) )
			{
				op = MINUS;
				afterOp = TRUE;
			}
			else
			{
				countFirst ++ ;
				t_atom* pAdd = getbytes(sizeof(t_atom));
				(* pAdd) = * pCurrent;
				ListAtomAdd( & listReturn, pAdd );
			}
		}
		else
		{
			ElementAtom* pElToDelete = NULL;
			switch( op )
			{
				case UNION:
					if( ! listContains( &listReturn, pCurrent ) )
					{
						t_atom* pAdd = getbytes(sizeof(t_atom));
						(* pAdd) = * pCurrent;
						ListAtomAdd( & listReturn, pAdd );
					}
				break;
				case MINUS:
					pElToDelete = ListAtomGetElement( &listReturn, pCurrent, AtomListCompareAtoms);
					if( pElToDelete )
					{
						ListAtomDel( & listReturn, pElToDelete );
					}
				break;
			}
		}
	}
	ElementAtom* pCurrent = ListAtomGetFirst( & listReturn );
	while ( pCurrent )
	{
		t_atom* pResult = getbytes(sizeof(t_atom));
		(*pResult) = (* pCurrent -> pData);
		ListAtomAdd( & rt -> stack, pResult);
		
		pCurrent = ListAtomGetNext( & listReturn, pCurrent );
	}
	ListAtomExit( & listReturn );
}

BOOL listContains(ListAtom* pList, t_atom* pElement)
{
	ElementAtom* pElCurrent = ListAtomGetFirst( pList );
	while( pElCurrent )
	{
		if( atom_getfloat( pElCurrent->pData) == atom_getfloat( pElement ) )
			return TRUE;
		pElCurrent = ListAtomGetNext( pList, pElCurrent );
	}
	return FALSE;
}

PFUNCTION_HEADER( contains )
{
	t_atom element = ( pArgs[0] );
	BOOL bRet = setContains( countArgs-1, & pArgs[1], &element);

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, bRet );
	ListAtomAdd( & rt -> stack, pResult);
}

BOOL setContains(t_int count, t_atom* set, t_atom* element)
{
	for( int i=0; i<count; i++ )
	{
		if( atom_getfloat(& set[i]) == atom_getfloat( element ))
			return TRUE;
	}
	return FALSE;
}

PFUNCTION_HEADER( calcTransp )
{
	// the sets should be separated by |
	ListAtom listReturn;
	ListAtomInit( & listReturn );
	// brute force.
	// a better solution would be to sort both lists, and then to merge them:
	t_int countFirst = 0;
	for( int i=0; i<countArgs; i++ )
	{
		t_atom* pCurrent = & pArgs[i];
		if( atomEqualsString( pCurrent, "|" ) )
		{
			break;
		}
		else
		{
			countFirst ++ ;
		}
	}
	if( countFirst == countArgs )
	{
		post("ERROR: separator \"|\" not found!");
		return;
	}
	for( int transp=0; transp<12; transp++ )
	{
		BOOL passed = TRUE;
		for( int i=countFirst+1; i<countArgs; i++ )
		{
			t_atom current;
			SETFLOAT( & current, (t_int )(atom_getfloat( & pArgs[i]) + transp) % 12 );
			if( setContains( countArgs-countFirst-1, pArgs, & current ) )
			{
				passed = FALSE;
			}
		}
		if( passed )
		{
			t_atom* pResult = getbytes(sizeof(t_atom));
			SETFLOAT( pResult, transp );
			ListAtomAdd( & listReturn, pResult );
		}
	}
	ElementAtom* pCurrent = ListAtomGetFirst( & listReturn );
	while ( pCurrent )
	{
		t_atom* pResult = getbytes(sizeof(t_atom));
		(*pResult) = (* pCurrent -> pData);
		ListAtomAdd( & rt -> stack, pResult);
		
		pCurrent = ListAtomGetNext( & listReturn, pCurrent );
	}
	ListAtomExit( & listReturn );
}

PFUNCTION_HEADER( addA )
{
	if( countArgs == 0 )
	{
		post("ERROR: addA called with no parameters!");
		return;
	}
	t_atom a = pArgs[0] ;
	for( int i=1; i<countArgs; i++)
	{
		t_atom x = pArgs[i] ;
		t_atom* pResult = getbytes(sizeof(t_atom));
		SETFLOAT( pResult, atom_getfloat(&x) + atom_getfloat(&a) );
		ListAtomAdd( & rt -> stack, pResult);
	}
}
PFUNCTION_HEADER( subA )
{
	if( countArgs == 0 )
	{
		post("ERROR: subA called with no parameters!");
		return;
	}
	t_atom a = pArgs[0] ;
	for( int i=1; i<countArgs; i++)
	{
		t_atom x = pArgs[i] ;
		t_atom* pResult = getbytes(sizeof(t_atom));
		SETFLOAT( pResult, atom_getfloat(&x) - atom_getfloat(&a) );
		ListAtomAdd( & rt -> stack, pResult);
	}
}
PFUNCTION_HEADER( mulA )
{
	if( countArgs == 0 )
	{
		post("ERROR: mulA called with no parameters!");
		return;
	}
	t_atom a = pArgs[0] ;
	for( int i=1; i<countArgs; i++)
	{
		t_atom x = pArgs[i] ;
		t_atom* pResult = getbytes(sizeof(t_atom));
		SETFLOAT( pResult, atom_getfloat(&x) * atom_getfloat(&a) );
		ListAtomAdd( & rt -> stack, pResult);
	}
}
PFUNCTION_HEADER( divA )
{
	if( countArgs == 0 )
	{
		post("ERROR: divA called with no parameters!");
		return;
	}
	t_atom a = pArgs[0] ;
	for( int i=1; i<countArgs; i++)
	{
		t_atom x = pArgs[i] ;
		t_atom* pResult = getbytes(sizeof(t_atom));
		SETFLOAT( pResult, atom_getfloat(&x) / atom_getfloat(&a) );
		ListAtomAdd( & rt -> stack, pResult);
	}
}

PFUNCTION_HEADER( modA )
{
	if( countArgs == 0 )
	{
		post("ERROR: modA called with no parameters!");
		return;
	}
	t_atom m = pArgs[0] ;
	for( int i=1; i<countArgs; i++)
	{
		t_atom x = pArgs[i] ;
		t_atom* pResult = getbytes(sizeof(t_atom));
		SETFLOAT( pResult, (t_int )atom_getfloat(&x) % (t_int )atom_getfloat(&m) );
		ListAtomAdd( & rt -> stack, pResult);
	}
}

PFUNCTION_HEADER( sgMinMax )
{
	t_float pMin = atom_getfloat( & pArgs[0] );
	t_float pMax = atom_getfloat( & pArgs[1] );
	t_float pInput = atom_getfloat( & pArgs[2] );
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT(
		pResult,
		((pMax) - (pMin)) * (pInput) + (pMin)
	);
	ListAtomAdd( & rt -> stack, pResult);
}

// random
// Rand ( )
PFUNCTION_HEADER( random_ )
{
	t_float min = atom_getfloat( & pArgs[0] );
	t_float max = atom_getfloat( & pArgs[1] );
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, min + (rand() % 1000000)/1000000.0 * (max-min) );
	ListAtomAdd( & rt -> stack, pResult);
}

PFUNCTION_HEADER( nop )
{
}

PFUNCTION_HEADER( returnAll )
{
	for( int i=0; i<countArgs; i++ )
	{
		t_atom* pResult = getbytes(sizeof(t_atom));
		*pResult = pArgs[i] ;
		ListAtomAdd( & rt -> stack, pResult);
	}
}


PFUNCTION_HEADER( rndInt )
{
	t_int min = atom_getfloat( & pArgs[0] );
	t_int max = atom_getfloat( & pArgs[1] );
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, min + (rand() % (max - min + 1) ) );
	ListAtomAdd( & rt -> stack, pResult);
}
PFUNCTION_HEADER( inc )
{
	if( countArgs == 0 )
	{
		post("WARNING: inc called with zero parameters");
		return;
	}
	DB_PRINT("inc called with %i args", countArgs);

	Atoms* value =
		Scope_get(
				rt -> scope,
				atom_getsymbol( & pArgs[0] )
		);
	char buf[256];
	atom_string( & pArgs[0], buf, 256 );
	if( ! value )
	{
		post("ERROR: inc: variable \"%s\" not found!", buf);
		return;
	}
	if( ! Atoms_get_size( value ) )
	{
		post("ERROR: inc: variable \"%s\" is empty!", buf);
		return;
	}
	t_int old_val =
		atom_getint(
				& Atoms_get_array( value )[0]
		);
	SETFLOAT(
			& Atoms_get_array( value )[0],
			old_val+1
	);
}
PFUNCTION_HEADER( dec )
{
	if( countArgs == 0 )
	{
		post("WARNING: inc called with zero parameters");
		return;
	}
	DB_PRINT("inc called with %i args", countArgs);

	Atoms* value =
		Scope_get(
				rt -> scope,
				atom_getsymbol( & pArgs[0] )
		);
	char buf[256];
	atom_string( & pArgs[0], buf, 256 );
	if( ! value )
	{
		post("ERROR: inc: variable \"%s\" not found!", buf);
		return;
	}
	if( ! Atoms_get_size( value ) )
	{
		post("ERROR: inc: variable \"%s\" is empty!", buf);
		return;
	}
	t_int old_val =
		atom_getint(
				& Atoms_get_array( value )[0]
		);
	SETFLOAT(
			& Atoms_get_array( value )[0],
			old_val-1
	);
}

PFUNCTION_HEADER( rndIntUnequal )
{
	t_int count = countArgs;
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, -1 ); // (set a default value)
	t_float rand_ = (rand() % 1000000) / 1000000.0; 
	t_float current = 0;
	for( int i=0; i<count; i++ )
	{
		current += atom_getfloat( & pArgs[i] );
		if( rand_ < current )
		{
			SETFLOAT( pResult,  i );
			break;
		}
	}
	ListAtomAdd( & rt -> stack, pResult);
}


PFUNCTION_HEADER(sgpackType)
{
	t_atom* pResult = getbytes(sizeof(t_atom));
	(*pResult) = pArgs[0];
	ListAtomAdd( & rt->stack, pResult);
}
PFUNCTION_HEADER(sgpackCount)
{
	t_atom* pResult = getbytes(sizeof(t_atom));
	(*pResult) = pArgs[1];
	ListAtomAdd( & rt->stack, pResult);
}
PFUNCTION_HEADER(sgpackParams)
{
	t_int count = atom_getfloat( & pArgs[1]);
	for( int i=0; i<count; i++)
	{
		t_atom* pResult = getbytes(sizeof(t_atom));
		(*pResult) = pArgs[2+i];
		ListAtomAdd( & rt->stack, pResult);
	}
}

PFUNCTION_HEADER( sgPackFromHuman )
{
	ListAtomPointer stackSizeInfo; // a stack of the atoms with the current packs size
	t_int indexNew = 0; // index in the return values
	ListAtomPointerInit( &stackSizeInfo );
	for( int index=0; index < countArgs; index++ )
	{
		t_atom* pCurrent = & pArgs[index];
		if( atom_getsymbol(pCurrent) == gensym("(") )
		{
			t_atom* pAtom = getbytes( sizeof(t_atom));
			SETFLOAT(pAtom, indexNew);
			ListAtomAdd( & rt->stack, pAtom);

			ListAtomPointerAdd( &stackSizeInfo,pAtom);
			indexNew ++;
		}
		else if( atom_getsymbol(pCurrent) == gensym(")") )
		{
			if( ListAtomPointerGetSize( &stackSizeInfo ) > 0 )
			{
				ElementAtomPointer* pEl = ListAtomPointerGetLast(&stackSizeInfo);
				SETFLOAT( pEl->pData, indexNew - atom_getfloat(pEl->pData) -1 );
				ListAtomPointerDel( &stackSizeInfo, pEl );
			}
			else
			{
				post("ERROR: sgPackFromHuman: \")\" found without corresponding \"(\"!");
			}
		}
		else
		{
			t_atom* pAtom = getbytes( sizeof(t_atom));
			(*pAtom) = pArgs[index] ;
			ListAtomAdd( & rt->stack, pAtom);
			indexNew ++;
		}
	}
	if( ListAtomPointerGetSize( &stackSizeInfo ) > 0 )
	{
		post("WARNING: sgPackFromHuman: more \"(\" than \")\"!");
	}
	ListAtomPointerExit( &stackSizeInfo );
}

// sgData (lists of sgPacks):
PFUNCTION_HEADER( sgDataGetPackFromType )
{
	t_atom* pType = &pArgs[0];
	t_int pos = 1;
	while( pos < countArgs )
	{
		t_atom* pCurrentType = & pArgs[pos];
		t_int count = atom_getfloat(& pArgs[pos+1]);
		if( atom_getsymbol(pCurrentType) == atom_getsymbol(pType))
		{
			for( int i=pos; i<pos+2+count; i++ )
			{
				t_atom* pAtom = getbytes( sizeof(t_atom));
				(*pAtom) = pArgs[i] ;
				ListAtomAdd( & rt->stack, pAtom);
			}
		}
		pos += (2 + count);
	}
}
PFUNCTION_HEADER( sgDataGetPackFromTypeRest )
{
	t_atom* pType = &pArgs[0];
	t_int pos = 1;
	while( pos < countArgs )
	{
		t_atom* pCurrentType = & pArgs[pos];
		t_int count = atom_getfloat(& pArgs[pos+1]);
		if( atom_getsymbol(pCurrentType) != atom_getsymbol(pType))
		{
			for( int i=pos; i<pos+2+count; i++ )
			{
				t_atom* pAtom = getbytes( sizeof(t_atom));
				(*pAtom) = pArgs[i] ;
				ListAtomAdd( & rt->stack, pAtom);
			}
		}
		pos += (2 + count);
	}
}
PFUNCTION_HEADER( sgDataGetPackFromIndex )
{
	t_int index = atom_getint(&pArgs[0]);
	t_int indexCurrent = 0;
	t_int pos = 1;
	while( pos < countArgs )
	{
		t_int count = atom_getfloat(& pArgs[pos+1]);
		if( indexCurrent == index )
		{
			for( int i=pos; i<pos+2+count; i++ )
			{
				t_atom* pAtom = getbytes( sizeof(t_atom));
				(*pAtom) = pArgs[i] ;
				ListAtomAdd( & rt->stack, pAtom);
			}
		}
		pos += (2 + count);
		indexCurrent ++;
	}
}
// returns the first sgPack
PFUNCTION_HEADER( sgDataGetFirst )
{
	t_int pos = 0;
	
	t_int count = atom_getfloat(& pArgs[pos+1]);
	for( int i=pos; i<pos+2+count; i++ )
	{
		t_atom* pAtom = getbytes( sizeof(t_atom));
		(*pAtom) = pArgs[i] ;
		ListAtomAdd( & rt->stack, pAtom);
	}
}


PFUNCTION_HEADER( sgDataGetRest )
{
	t_int pos = 0;
	
	t_int count = atom_getfloat(& pArgs[pos+1]);
	for( int i=pos; i<pos+2+count; i++ )
	{
		t_atom* pAtom = getbytes( sizeof(t_atom));
		(*pAtom) = pArgs[i] ;
	}
	pos += (2 + count);
	for( ; pos<countArgs; pos++ )
	{
		t_atom* pAtom = getbytes( sizeof(t_atom));
		(*pAtom) = pArgs[pos] ;
		ListAtomAdd( & rt->stack, pAtom);
	}
}

PFUNCTION_HEADER( delay )
{
	rt -> script_obj -> delay =
		atom_getint( & pArgs[0] );
}

PFUNCTION_HEADER( callFunction )
{
	rt -> script_obj -> jump_to_program =
			atom_getsymbol( & pArgs[0] );
}
