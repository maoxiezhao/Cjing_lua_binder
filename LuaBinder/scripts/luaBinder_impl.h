#pragma once

#include "scripts\luaTools.h"
#include "scripts\luaRef.h"

namespace Cjing3D
{
namespace LuaBinderImpl
{
	struct BindClassMetaMethod
	{
		static int index(lua_State* l);
		static int newIndex(lua_State* l);
		static int gc(lua_State* l);
	};

	bool RegisterClass(LuaRef& currentMeta, LuaRef& parentMeta, const std::string& name)
	{
		LuaRef ref = parentMeta.RawGet(name);
		if (ref != LuaRef::NULL_REF)
		{
			currentMeta = ref;
			return false;
		}

		lua_State* l = parentMeta.GetLuaState();

		LuaRef classTable = LuaTools::CreateTable(l);
		LuaRef constClassTable = LuaTools::CreateTable(l);
		LuaRef staticClassTable = LuaTools::CreateTable(l);

		parentMeta.RawSet(name, staticClassTable);

		currentMeta = staticClassTable;
		return true;
	}
}
}