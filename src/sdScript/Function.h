#ifndef _FUNCTION_
#define _FUNCTION_

#include "Global.h"
//#include "DynArray.h"

typedef struct SProgramRTInfo t_rt;

typedef void (*POperatorFunction) (t_rt* pThis, t_int countParam, t_atom* pArgs);


// describes the restrictions for a function:
/*
  Please notice:
  if executeAfter != -1, the function may be executed BEFORE its closing ")".
  it is therefore important, that such a function either jumps after after its corresponding ")" via "jump", or explicitely puts a new command on the command stack!
*/
typedef struct SFunctionInfo {
	t_atom name;
	t_int paramCount; // -1 means variable

	t_int executeAfter;
	/* if != -1 this means the function is called after "executeAfter" parameters,
		just ignoring the parameters that might follow
	  -1 is the usual behaviour. the function is called when all parameters have
	  	been parsed, that means when the ')' is found.
	*/

	POperatorFunction pFunc;
} FunctionInfo;

DECL_DYN_ARRAY(OutputBuf,t_atom,getbytes,freebytes)
DEF_DYN_ARRAY(OutputBuf,t_atom,getbytes,freebytes)

DECL_DYN_ARRAY(FunctionInfos, FunctionInfo,getbytes,freebytes)
DEF_DYN_ARRAY(FunctionInfos, FunctionInfo,getbytes,freebytes)

FunctionInfos* functions_init();
void functions_exit(
		FunctionInfos* x
);

FunctionInfo* getFunctionInfo(
		FunctionInfos* function_infos,
		t_atom* pName
);

FunctionInfo* get_NOP(
		FunctionInfos* function_infos
);
FunctionInfo* get_RETURN_ALL(
		FunctionInfos* function_infos
);

#endif 
