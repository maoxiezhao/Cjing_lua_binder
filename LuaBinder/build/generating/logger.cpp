
#include "scripts\luaContext.h"
#include "scripts\luaBinder.h"

using namespace Cjing3D;

void logger_AutoBindFunction(lua_State* l)
{
	
}
LUA_FUNCTION_AUTO_BINDER(logger_auto_binder,logger_AutoBindFunction)