
#include "scripts\luaContext.h"
#include "scripts\luaBinder.h"

using namespace Cjing3D;

void test_AutoBindFunction(lua_State* l)
{
	
}
LUA_FUNCTION_AUTO_BINDER(test_auto_binder,test_AutoBindFunction)