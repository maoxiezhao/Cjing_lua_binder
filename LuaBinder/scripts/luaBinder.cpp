#include "luaBinder.h"

namespace Cjing3D {

bool LuaBindClassBase::BindClassMeta(LuaRef & currentMeta, LuaRef & parentMeta, const std::string & name, void * currentClassID)
{
	LuaRef ref = parentMeta.RawGet(name);
	if (!ref.IsEmpty())
	{
		currentMeta = ref;
		return false;
	}

	lua_State* l = parentMeta.GetLuaState();

	LuaRef classTable = LuaRef::CreateTable(l);
	classTable.SetMetatable(classTable);

	LuaRef staticClassTable = LuaRef::CreateTable(l);
	staticClassTable.RawSet("__CLASS", classTable);

	// rawset metatable into registryIndex
	LuaRef classIDRef = LuaRef::CreateRefFromPtr(l, currentClassID);
	LuaRef registry = LuaRef::CreateRef(l, LUA_REGISTRYINDEX);
	registry.RawSet(classIDRef, classTable);

	// no use now.
	LuaRef constClassTable = LuaRef::CreateTable(l);
	constClassTable.SetMetatable(constClassTable);
	staticClassTable.RawSet("__CONST", constClassTable);

	parentMeta.RawSet(name, staticClassTable);
	currentMeta = staticClassTable;

	return true;
}

bool LuaBindClassBase::RegisterFunction(const std::string & name, LuaRef func)
{
	LuaRef metaClass = mCurrentMeta.RawGet("__CLASS");
	metaClass.RawSet(name, func);

	return true;
}

}