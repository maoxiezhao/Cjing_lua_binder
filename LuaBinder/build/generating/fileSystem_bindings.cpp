
#include "scripts\luaContext.h"
#include "scripts\luaBinder.h"

using namespace Cjing3D;

void fileSystem_AutoBindFunction(lua_State* l)
{
	
}
LUA_FUNCTION_AUTO_BINDER(fileSystem_auto_binder,fileSystem_AutoBindFunction)