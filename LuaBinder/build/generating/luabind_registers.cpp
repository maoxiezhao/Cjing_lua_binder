#include "test.h"
#include "scripts\luaContext.h"
#include "scripts\luaBinder.h"

using namespace Cjing3D;

void luabind_registers_AutoBindFunction(lua_State* l)
{
LuaBinder(l)
.BeginClass<Gun>("Gun")
.AddConstructor(_LUA_ARGS_(std::string&))
.AddMethod("Shoot", &Gun::Shoot)
.AddMethod("Show", &Gun::Show)
.AddMethod("GetName", &Gun::GetName)
.AddFunction("PickUp", &Gun::PickUp)
.EndClass();

	
}
LUA_FUNCTION_AUTO_BINDER(luabind_registers_auto_binder,luabind_registers_AutoBindFunction)