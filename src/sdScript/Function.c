#include "Function.h"

#include "SymbolTable.h"
#include "LinkedList.h"
#include "sdScript.h"

#include <stdlib.h>


/*
#define FUNCTION_COUNT 54
FunctionInfo listFunctionInfo[FUNCTION_COUNT];
*/

DECL_DYN_ARRAY(FunctionInfos, FunctionInfo)
DEF_DYN_ARRAY(FunctionInfos, FunctionInfo)


FunctionInfo* pNOP;
FunctionInfo* pRETURN_ALL;

#define PFUNCTION_HEADER(name) \
void name(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)

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


#define ADD_FUNCTION( NAME,PARAMCOUNT,EXECAFTER,PFUNC ) \
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

	ADD_FUNCTION("NOP",-1,-1,&nop);
	ADD_FUNCTION("Add",2,-1,&add);
	ADD_FUNCTION("Sub",2,-1,&sub);
	ADD_FUNCTION("Mul",2,-1,&mul);
	ADD_FUNCTION("Div",2,-1,&div_);
	ADD_FUNCTION("Mod",2,-1,&mod);
	ADD_FUNCTION("Print",-1,-1,&print);
	ADD_FUNCTION("Pack",-1,-1,&pack);
	ADD_FUNCTION("Out",0,-1,&out);
	ADD_FUNCTION("Var",-1,-1,&addVar);
	ADD_FUNCTION("Get",1,-1,&getVar);
	ADD_FUNCTION("GetA",2,-1,&getVarA);
	ADD_FUNCTION("Set",-1,-1,&setVar);
	ADD_FUNCTION("SetA",-1,-1,&setVarA);
	ADD_FUNCTION("If",-1,1,&if_);
	ADD_FUNCTION("VarMain",-1,-1,&addMainVar);
	ADD_FUNCTION("ClearMain",0,-1,&clearMain);
	// sgScales:
	// a | b , c , x
	ADD_FUNCTION("sgFunc",4,-1,&sgFunc);
	// # , a | b , c 
	ADD_FUNCTION("sgScale",4,-1,&sgScale);
	// boolean operators:

	ADD_FUNCTION("&&",2,-1,&and_);
	ADD_FUNCTION("||",2,-1,&or_);
	ADD_FUNCTION("!",1,-1,&not_);
	// comparison operators:
	ADD_FUNCTION("==",2,-1,&isEqual);
	ADD_FUNCTION("!=",2,-1,&isNotEqual);
	ADD_FUNCTION("<",2,-1,&isLessThan);
	ADD_FUNCTION(">",2,-1,&isGreaterThan);
	ADD_FUNCTION("<=",2,-1,&isLessOrEqual);
	ADD_FUNCTION(">=",2,-1,&isGreaterOrEqual);
	// Set operations:
	ADD_FUNCTION("Setify",-1,-1,&setify);
	ADD_FUNCTION("Card",-1,-1,&card);
	ADD_FUNCTION("SetOp",-1,-1,&setOp);
	ADD_FUNCTION("CalcTransp",-1,-1,&calcTransp);
	ADD_FUNCTION("Contains",-1,-1,&contains);
	ADD_FUNCTION("AddA",-1,-1,&addA);
	ADD_FUNCTION("SubA",-1,-1,&subA);
	ADD_FUNCTION("MulA",-1,-1,&mulA);
	ADD_FUNCTION("DivA",-1,-1,&divA);
	ADD_FUNCTION("ModA",-1,-1,&modA);
	ADD_FUNCTION("Rnd",2,-1,&random_);
	ADD_FUNCTION("MinMax",3,-1,&sgMinMax);
	ADD_FUNCTION("RETURN_ALL",-1,-1,&returnAll);
	ADD_FUNCTION("RndI",2,-1,&rndInt);
	ADD_FUNCTION("Inc",1,-1,&inc);
	ADD_FUNCTION("Dec",1,-1,&dec);
	ADD_FUNCTION("RndIUnequal",-1,-1,&rndIntUnequal);
	ADD_FUNCTION("sgPackGetType",-1,-1,&sgpackType);
	ADD_FUNCTION("sgPackGetCount",-1,-1,&sgpackCount);
	ADD_FUNCTION("sgPackGetParams",-1,-1,&sgpackParams);
	ADD_FUNCTION("sgPackFromHuman",-1,-1,&sgPackFromHuman);
	ADD_FUNCTION("sgDataGetPackFromType",-1,-1,&sgDataGetPackFromType);
	ADD_FUNCTION("sgDataGetPackFromTypeRest",-1,-1,&sgDataGetPackFromTypeRest);
	ADD_FUNCTION("sgDataGetPackFromIndex",-1,-1,&sgDataGetPackFromIndex);
	ADD_FUNCTION("sgDataGetFirst",-1,-1,&sgDataGetFirst);
	ADD_FUNCTION("sgDataGetRest",-1,-1,&sgDataGetRest);

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

void add(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	DB_PRINT("add");
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, atom_getfloat(& pArgs[0]) + atom_getfloat(& pArgs[1]));
	//push result on stack:
	ListAtomAdd( &pThis -> stack, pResult);
}


void sub(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	DB_PRINT("sub");
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, atom_getfloat(&pArgs[0]) - atom_getfloat(&pArgs[1]));
	//push result on stack:
	ListAtomAdd( &pThis -> stack, pResult);
}
void mul(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	DB_PRINT("mul");
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, atom_getfloat(&pArgs[0]) * atom_getfloat(& pArgs[1]));
	//push result on stack:
	ListAtomAdd( &pThis -> stack, pResult);
}
void div_(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	DB_PRINT("div");
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, atom_getfloat(&pArgs[0]) / atom_getfloat(& pArgs[1]));
	//push result on stack:
	ListAtomAdd( &pThis -> stack, pResult);
}
void mod(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	DB_PRINT("div");
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, (t_int )atom_getfloat(&pArgs[0]) % (t_int )atom_getfloat(& pArgs[1]));
	//push result on stack:
	ListAtomAdd( &pThis -> stack, pResult);
}

void print(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	for ( int i=0; i<countArgs; i++)
	{
		/*
		if( pThis->outputBufferCount + 1 > OUTPUTBUFFER_LENGTH )
		{
			pd_error( pThis, "output buffer overflow! (maximum: %i). You can try to recompile with bigger OUTPUTBUFFER_LENGTH", OUTPUTBUFFER_LENGTH );
			return;
		}
		*/
		OutputBuf_append( & pThis->outputBuffer, pArgs[i] );
		/*
		pThis -> outputBuffer [ pThis -> outputBufferCount ] = (pArgs[i]);
		pThis -> outputBufferCount ++ ;
		*/
	}
}

void pack(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
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
	ListAtomAdd( &pThis -> stack, pResult[0]);
	ListAtomAdd( &pThis -> stack, pResult[1]);
	for(int i=0; i< countArgs-1; i++)
	{
		ListAtomAdd( & pThis -> stack, pResult[2+i]);
	}
	freebytes( pResult, sizeof(t_atom* ) * (countArgs+2));
}

void out(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	outlet_list(
		pThis -> pOutlet,
		&s_list,
		OutputBuf_get_size( & pThis -> outputBuffer ),
		OutputBuf_get_array( & pThis -> outputBuffer )
		);
	OutputBuf_clear( & pThis -> outputBuffer );
	//pThis -> outputBufferCount = 0;
}

void addVar(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	if( countArgs == 0 )
	{
		post("WARNING: addVar with zero parameters called!");
		return;
	}
	/*else if( countArgs == 1 )
	{
		post("WARNING: addVar with one parameters called!");
		return;
	}*/
	Variable variable;
	variable . count = countArgs - 1;
	variable . values = getbytes( sizeof(t_atom)* (countArgs-1) );
	for( int i=0; i<(countArgs-1); i++ )
	{
		variable . values[i] = pArgs[i+1];
	}
	STValue value;
	value . type = VALUE;
	value . variable = variable;
	STEntry entry;
	entry . symbol = pArgs[0];
	entry . value = value;
	SymbolTable_Add(pThis -> pSymbolTable, entry);
	//addToSymbolTable( pThis, countArgs-1 , & pArgs[1]);
}


void getVar(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	STValue* pSTValue = SymbolTable_Lookup( pThis -> pSymbolTable, & pArgs[0] );
	if( ! pSTValue )
	{
		char buf[256];
		atom_string( & pArgs[0], buf, 256 );
		post("ERROR: getVar: couldn't find variable \"%s\"", buf);
		return;
	}
	if( pSTValue -> type == VALUE)
	{
		Variable* pVar = & pSTValue -> variable;
		for( int i=0; i< pVar->count; i++ )
		{
			t_atom* pResult = getbytes(sizeof(t_atom));
			(*pResult) =  pSTValue -> variable . values[i];
			ListAtomAdd( &pThis -> stack, pResult);
		}
		//return pSTValue -> value;
	}
	else
		post("ERROR: symbol table entry is not a value!");
	
}

void getVarA(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	STValue* pSTValue = SymbolTable_Lookup( pThis -> pSymbolTable, & pArgs[0] );
	t_int index = atom_getfloat( & pArgs[1] );
	if( ! pSTValue )
	{
		char buf[256];
		atom_string( & pArgs[0], buf, 256 );
		post("ERROR: getVar: couldn't find variable \"%s\"", buf);
		return;
	}
	if( pSTValue -> type == VALUE)
	{
		Variable* pVar = & pSTValue -> variable;

		if( index < 0 || index >= pVar->count )
		{
			post("ERROR: getVarA: index out of bounds: \"%i\"", (int )index);
			return;
		}
		t_atom* pResult = getbytes(sizeof(t_atom));
		(*pResult) =  pSTValue -> variable . values[index];
		ListAtomAdd( &pThis -> stack, pResult);
		//return pSTValue -> value;
	}
	else
		post("ERROR: symbol table entry is not a value!");
	
}

//void setVar(t_script_obj* pThis, t_atom* pName, t_atom* pValue)
void setVar(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	if( countArgs == 0 )
	{
		post("WARNING: setVar called with zero parameters");
		return;
	}
	DB_PRINT("setVar called with %i args", countArgs);

	STValue* pSTValue = SymbolTable_Lookup( pThis -> pSymbolTable, & pArgs[0] );
	if( !pSTValue )
	{
		char buf[256];
		atom_string( & pArgs[0],buf,256);
		post("ERROR: variable \"%s\" not found!", buf);
		return;
	}
	Variable* pVar = & pSTValue -> variable;
	freebytes( pVar->values, sizeof(t_atom)* pVar->count);
	pVar->values = getbytes( sizeof(t_atom)* (countArgs-1));
	pVar->count = countArgs-1;
	for( int i=0; i<(countArgs-1); i++ )
	{
		pVar->values[i] = pArgs[i+1];
	}
}

void setVarA(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_int index = atom_getfloat( &pArgs[1] );
	t_atom* pNewValue = & pArgs[2];

	STValue* pSTValue = SymbolTable_Lookup( pThis -> pSymbolTable, & pArgs[0] );
	if( !pSTValue )
	{
		char buf[256];
		atom_string(& pArgs[0],buf,256);
		post("ERROR: variable \"%s\" not found!", buf);
		return;
	}
	Variable* pVar = & pSTValue -> variable;
	if( index < 0 || index >= pVar->count )
	{
		post("ERROR: setVarA: index out of bounds: \"%i\"", (int )index);
		return;
	}
	pVar->values[index] = *pNewValue;

}

void addMainVar(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	//post("addMainVar");
	if( countArgs == 0 )
	{
		post("WARNING: addMainVar with zero parameters called!");
		return;
	}
	else if( countArgs == 1 )
	{
		post("WARNING: addMainVar with one parameters called!");
		return;
	}
	Variable variable;
	variable . count = countArgs - 1;
	variable . values = getbytes( sizeof(t_atom)* (countArgs-1) );
	for( int i=0; i<(countArgs-1); i++ )
	{
		variable . values[i] = pArgs[i+1];
	}
	STValue value;
	value . type = VALUE;
	value . variable = variable;
	STEntry entry;
	entry . symbol = pArgs[0];
	entry . value = value;
	SymbolTable_AddMainVar(pThis -> pSymbolTable, entry);
}
void clearMain(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	//post("clearMain");
	SymbolTable_Exit( pThis -> pSymbolTable );
	SymbolTable_Init( pThis -> pSymbolTable );
}

void if_(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	//post("if_ called!!");
	if( atom_getfloat(& pArgs[0]) )
	{
		CommandInfo* pCurrentCommandInfo = getbytes(sizeof(CommandInfo));
		pCurrentCommandInfo -> stackHeight0 = ListAtomGetSize ( & pThis -> stack );
		pCurrentCommandInfo -> pFunctionInfo = get_RETURN_ALL();
		ListCommandAdd ( & pThis -> cmdStack, pCurrentCommandInfo);
	}
	else
	{
		pThis->skipMode = TRUE;
	}
}

// sgScales:
void sgFunc(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float x = atom_getfloat( & pArgs[0] );
	t_float stepAdd = atom_getfloat( & pArgs[1] );
	t_float step0 = atom_getfloat( & pArgs[2] );
	t_float c = atom_getfloat( & pArgs[3] );
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, c + stepAdd/2 * x*x + ( step0 - stepAdd/2 ) * x );
	ListAtomAdd( &pThis -> stack, pResult);
}
void sgScale(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float n = atom_getfloat( & pArgs[0] );
	t_float stepAdd = atom_getfloat( & pArgs[1] );
	t_float step0 = atom_getfloat( & pArgs[2] );
	t_float c = atom_getfloat( & pArgs[3] );

	for( int i=0; i<n; i++ )
	{
		t_atom* pResult = getbytes(sizeof(t_atom));
		SETFLOAT( pResult, c + stepAdd/2 * i*i + ( step0 - stepAdd/2 ) * i );
		ListAtomAdd( &pThis -> stack, pResult);
	}
}

// boolean operators:
void and_(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a && b );
	ListAtomAdd( &pThis -> stack, pResult);
}
void or_(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a || b );
	ListAtomAdd( &pThis -> stack, pResult);
}
void not_(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, ! a );
	ListAtomAdd( &pThis -> stack, pResult);
}

// comparison operators:
void isEqual(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a == b );
	ListAtomAdd( &pThis -> stack, pResult);
}
void isNotEqual(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a != b );
	ListAtomAdd( &pThis -> stack, pResult);

}
void isLessThan(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a < b );
	ListAtomAdd( &pThis -> stack, pResult);
}
void isGreaterThan(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a > b );
	ListAtomAdd( &pThis -> stack, pResult);
}
void isLessOrEqual(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a <= b );
	ListAtomAdd( &pThis -> stack, pResult);
}
void isGreaterOrEqual(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a >= b );
	ListAtomAdd( &pThis -> stack, pResult);
}

typedef enum ESetOp { UNION, MINUS } SetOp;
// Set operations:
BOOL AtomListCompareAtoms(t_atom* pInList, t_atom* p)
{
	return compareAtoms(pInList, p);
}
BOOL setContains(t_int count, t_atom* set, t_atom* element);
BOOL listContains(ListAtom* pList, t_atom* pElement);

void setify(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	for( int i=0; i<countArgs; i++ )
	{
		t_atom* pCurrent = & pArgs[i];
		if( ! setContains( i-1, pArgs, pCurrent ) )
		{
			t_atom* pResult = getbytes(sizeof(t_atom));
			*pResult = *pCurrent;
			ListAtomAdd( &pThis -> stack, pResult);
		}
	}
}
void card(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, countArgs );
	ListAtomAdd( &pThis -> stack, pResult);
}

void setOp(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
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
			//if( atom_getsymbol( pCurrent ) == gensym("union") )
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
						/*t_atom* pAdd = getbytes(sizeof(t_atom));
						(* pAdd) = * pCurrent;
						ListAtomAdd( & listReturn, pAdd );*/
						ListAtomDel( & listReturn, pElToDelete );
					}
				break;
			}
		}
	}
	ElementAtom* pCurrent = ListAtomGetFirst( & listReturn );
	while ( pCurrent )
	{
		/*char buf[256];
		atom_string( pCurrent->pData, buf, 256);
		post("current: %s", buf);*/
		t_atom* pResult = getbytes(sizeof(t_atom));
		(*pResult) = (* pCurrent -> pData);
		ListAtomAdd( &pThis -> stack, pResult);
		
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

void contains(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_atom element = ( pArgs[0] );
	BOOL bRet = setContains( countArgs-1, & pArgs[1], &element);

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, bRet );
	ListAtomAdd( &pThis -> stack, pResult);
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

void calcTransp(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
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
		/*char buf[256];
		atom_string( pCurrent->pData, buf, 256);
		post("current: %s", buf);*/
		t_atom* pResult = getbytes(sizeof(t_atom));
		(*pResult) = (* pCurrent -> pData);
		ListAtomAdd( &pThis -> stack, pResult);
		
		pCurrent = ListAtomGetNext( & listReturn, pCurrent );
	}
	ListAtomExit( & listReturn );
}

void addA(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
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
		ListAtomAdd( &pThis -> stack, pResult);
	}
}
void subA(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
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
		ListAtomAdd( &pThis -> stack, pResult);
	}
}
void mulA(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
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
		ListAtomAdd( &pThis -> stack, pResult);
	}
}
void divA(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
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
		ListAtomAdd( &pThis -> stack, pResult);
	}
}

void modA(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	if( countArgs == 0 )
	{
		post("ERROR: modA called with no parameters!");
		return;
	}
	/*t_float m = atom_getfloat( & pArgs[0] );
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a  b );
	ListAtomAdd( &pThis -> stack, pResult)*/
	t_atom m = pArgs[0] ;
	for( int i=1; i<countArgs; i++)
	{
		t_atom x = pArgs[i] ;
		t_atom* pResult = getbytes(sizeof(t_atom));
		SETFLOAT( pResult, (t_int )atom_getfloat(&x) % (t_int )atom_getfloat(&m) );
		ListAtomAdd( &pThis -> stack, pResult);
	}
}

void sgMinMax(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float pMin = atom_getfloat( & pArgs[0] );
	t_float pMax = atom_getfloat( & pArgs[1] );
	t_float pInput = atom_getfloat( & pArgs[2] );
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT(
		pResult,
		((pMax) - (pMin)) * (pInput) + (pMin)
	);
	ListAtomAdd( &pThis -> stack, pResult);
}

// random
// Rand ( )
void random_(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float min = atom_getfloat( & pArgs[0] );
	t_float max = atom_getfloat( & pArgs[1] );
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, min + (rand() % 1000000)/1000000.0 * (max-min) );
	ListAtomAdd( &pThis -> stack, pResult);
}

void nop(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
}

void returnAll(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	for( int i=0; i<countArgs; i++ )
	{
		t_atom* pResult = getbytes(sizeof(t_atom));
		*pResult = pArgs[i] ;
		ListAtomAdd( &pThis -> stack, pResult);
	}
}


void rndInt(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	t_int min = atom_getfloat( & pArgs[0] );
	t_int max = atom_getfloat( & pArgs[1] );
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, min + (rand() % (max - min + 1) ) );
	ListAtomAdd( &pThis -> stack, pResult);
}
void inc(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	if( countArgs == 0 )
	{
		post("WARNING: inc called with zero parameters");
		return;
	}
	DB_PRINT("inc called with %i args", countArgs);

	STValue* pSTValue = SymbolTable_Lookup( pThis -> pSymbolTable, & pArgs[0] );
	if( !pSTValue )
	{
		char buf[256];
		atom_string( & pArgs[0],buf,256);
		post("ERROR: inc: variable \"%s\" not found!", buf);
		return;
	}
	Variable* pVar = & pSTValue -> variable;
	t_float oldVal = atom_getfloat( & pVar -> values [0] );
	freebytes( pVar->values, sizeof(t_atom)* pVar->count);
	pVar->values = getbytes( sizeof(t_atom) );
	pVar->count = 1;
	SETFLOAT( & pVar->values [0], oldVal+1 );
	/*for( int i=0; i<(countArgs-1); i++ )
	{
		pVar->values[i] = pArgs[i+1];
	}*/
}
void dec(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	if( countArgs == 0 )
	{
		post("WARNING: dec called with zero parameters");
		return;
	}
	DB_PRINT("dec called with %i args", countArgs);

	STValue* pSTValue = SymbolTable_Lookup( pThis -> pSymbolTable, & pArgs[0] );
	if( !pSTValue )
	{
		char buf[256];
		atom_string( & pArgs[0],buf,256);
		post("ERROR: dec: variable \"%s\" not found!", buf);
		return;
	}
	Variable* pVar = & pSTValue -> variable;
	t_float oldVal = atom_getfloat( & pVar -> values [0] );
	freebytes( pVar->values, sizeof(t_atom)* pVar->count);
	pVar->values = getbytes( sizeof(t_atom) );
	pVar->count = 1;
	SETFLOAT( & pVar->values [0], oldVal-1 );
	/*for( int i=0; i<(countArgs-1); i++ )
	{
		pVar->values[i] = pArgs[i+1];
	}*/
}

void rndIntUnequal(t_script_obj* pThis, t_int countArgs, t_atom* pArgs)
{
	/*t_int min = atom_getfloat( & pArgs[0] );
	t_int max = atom_getfloat( & pArgs[1] );*/
	//t_int countRange = max - min + 1;
	t_int count = countArgs;
	/*if( (countArgs - 2) != (countRange) )
		post( "WARNING: rndIntUnequal range contains an amount of values unequal than the cardinality of the list following!");*/
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
	//SETFLOAT( pResult, min + (rand() % (max - min + 1) ) );
	ListAtomAdd( &pThis -> stack, pResult);
}


PFUNCTION_HEADER(sgpackType)
{
	t_atom* pResult = getbytes(sizeof(t_atom));
	//SETSYMBOL(pResult, atom_getsymbol(& pArgs[0]));
	(*pResult) = pArgs[0];
	ListAtomAdd( & pThis->stack, pResult);
}
PFUNCTION_HEADER(sgpackCount)
{
	t_atom* pResult = getbytes(sizeof(t_atom));
	//SETFLOAT(pResult, atom_getfloat(& pArgs[1]));
	(*pResult) = pArgs[1];
	ListAtomAdd( & pThis->stack, pResult);
}
PFUNCTION_HEADER(sgpackParams)
{
	t_int count = atom_getfloat( & pArgs[1]);
	// SETSYMBOL(pResult, atom_getsymbol(pArgs[0]));
	for( int i=0; i<count; i++)
	{
		t_atom* pResult = getbytes(sizeof(t_atom));
		(*pResult) = pArgs[2+i];
		ListAtomAdd( & pThis->stack, pResult);
	}
}

PFUNCTION_HEADER( sgPackFromHuman )
{
	//t_int index = 0;
	ListAtomPointer stackSizeInfo; // a stack of the atoms with the current packs size
	//t_int index = 0; // index in the list being parsed
	t_int indexNew = 0; // index in the return values
	ListAtomPointerInit( &stackSizeInfo );
	for( int index=0; index < countArgs; index++ )
	{
		t_atom* pCurrent = & pArgs[index];
		if( atom_getsymbol(pCurrent) == gensym("(") )
		{
			//post("[");
			t_atom* pAtom = getbytes( sizeof(t_atom));
			SETFLOAT(pAtom, indexNew);
			ListAtomAdd( & pThis->stack, pAtom);

			ListAtomPointerAdd( &stackSizeInfo,pAtom);
			indexNew ++;
		}
		else if( atom_getsymbol(pCurrent) == gensym(")") )
		{
			//post("]");
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
			ListAtomAdd( & pThis->stack, pAtom);
			indexNew ++;
			//t_int count = pArgs[index];
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
				ListAtomAdd( & pThis->stack, pAtom);
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
				ListAtomAdd( & pThis->stack, pAtom);
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
		//t_atom* pCurrentType = & pArgs[pos];
		t_int count = atom_getfloat(& pArgs[pos+1]);
		if( indexCurrent == index )
		{
			for( int i=pos; i<pos+2+count; i++ )
			{
				t_atom* pAtom = getbytes( sizeof(t_atom));
				(*pAtom) = pArgs[i] ;
				ListAtomAdd( & pThis->stack, pAtom);
			}
		}
		pos += (2 + count);
		indexCurrent ++;
	}
}
// returns the first sgPack
PFUNCTION_HEADER( sgDataGetFirst )
{
	//t_int index = atom_getint(&pArgs[0]);
	//t_int indexCurrent = 0;
	t_int pos = 0;
	
	//t_atom* pCurrentType = & pArgs[pos];
	t_int count = atom_getfloat(& pArgs[pos+1]);
	for( int i=pos; i<pos+2+count; i++ )
	{
		t_atom* pAtom = getbytes( sizeof(t_atom));
		(*pAtom) = pArgs[i] ;
		ListAtomAdd( & pThis->stack, pAtom);
	}
}


PFUNCTION_HEADER( sgDataGetRest )
{
	//t_int index = atom_getint(&pArgs[0]);
	//t_int indexCurrent = 0;
	t_int pos = 0;
	
	//t_atom* pCurrentType = & pArgs[pos];
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
		ListAtomAdd( & pThis->stack, pAtom);
	}
	/*while( pos < countArgs )
	{
		//t_atom* pCurrentType = & pArgs[pos];
		t_int count = atom_getfloat(& pArgs[pos+1]);
		for( int i=pos; i<pos+2+count; i++ )
		{
			t_atom* pAtom = getbytes( sizeof(t_atom));
			(*pAtom) = pArgs[i] ;
			ListAtomAdd( & pThis->stack, pAtom);
		}
		pos += (2 + count);
		//indexCurrent ++;
	}*/
}
