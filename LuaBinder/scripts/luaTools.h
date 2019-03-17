#pragma once

#include "scripts\luaTypeMapping.h"

#include<string>

namespace Cjing3D
{
class LuaRef;

namespace LuaTools
{
	LuaRef CreateRef(lua_State*l);
	LuaRef CreateRef(lua_State*l, int index);
	LuaRef CreateTable(lua_State* l, int narray = 0, int nrec = 0);
	LuaRef CreateGlobalRef(lua_State* l);

	void CheckType(lua_State*l, int index, int exceptedType);
	int  GetPositiveIndex(lua_State*l, int index);
	void PrintLuaStack(lua_State*l);
	void PrintInIndex(lua_State*l, int index);
	int GetCallDepth(lua_State*l);

	template<typename T>
	inline void Push(lua_State*l, const T& v)
	{
		LuaType<T>::Push(l, v);
	}

	template<typename T = LuaRef>
	inline T Get(lua_State*l, int index)
	{
		return LuaType<T>::Get(l, index);
	}
}
}