#include "luaTools.h"
#include "scripts\luaRef.h"
#include "helper\logger.h"
#include "helper\debug.h"

#include<iostream>
#include<sstream>

namespace Cjing3D
{
namespace LuaTools
{
	LuaRef CreateRef(lua_State*l)
	{
		return LuaRef(l, luaL_ref(l, LUA_REGISTRYINDEX));
	}

	LuaRef CreateRef(lua_State*l, int index)
	{
		lua_pushvalue(l, index);
		return LuaRef(l, luaL_ref(l, LUA_REGISTRYINDEX));
	}

	LuaRef CreateTable(lua_State* l, int narray, int nrec)
	{
		lua_createtable(l, narray, nrec);
		return CreateRef(l);
	}

	LuaRef CreateGlobalRef(lua_State* l)
	{
		lua_pushglobaltable(l);
		return CreateRef(l);
	}

	void CheckType(lua_State * l, int index, int exceptedType)
	{
		if (lua_type(l, index) != exceptedType)
		{
			ArgError(l, index, std::string("excepted") + std::string(luaL_typename(l, exceptedType)) +
				" but get" + luaL_typename(l, index));
		}
	}

	int GetPositiveIndex(lua_State * l, int index)
	{
		int positiveIndex = index;
		if (index < 0 && index >= -lua_gettop(l))
			positiveIndex = lua_gettop(l) + index + 1;

		return positiveIndex;
	}

	void Error(lua_State * l, const std::string & message)
	{
		std::string msg(message);
		luaL_traceback(l, l, NULL, 1);
		msg += lua_tostring(l, -1);

		throw LuaException(l, msg);
	}

	void ArgError(lua_State * l, int index, const std::string & message)
	{
		std::ostringstream oss;
		lua_Debug info;
		if (!lua_getstack(l, 0, &info))		// 获取当前运行函数的活动记录
		{
			// ??????
			oss << "Bad argument #" << index << "(" << message << ")";
			Error(l, oss.str());
		}

		lua_getinfo(l, "n", &info);			// 填充name域，即当前函数的名字域
		if (std::string(info.namewhat) == "method")
		{
			// ?????
			oss << "Calling:" << info.name << " failed. (" << message << ")";
			Error(l, oss.str());
		}

		if (info.name == nullptr)
			info.name = "?";

		oss << "Bad argument #" << index << " to " << info.name << "(" << message << ")";
		Error(l, oss.str());
	}

	void PrintLuaStack(lua_State * l)
	{
		int stackSize = lua_gettop(l);
		std::ostringstream oss;
		oss << std::endl;
		for (int i = 1; i <= stackSize; i++)
		{
			switch (lua_type(l, i))
			{
			case LUA_TSTRING:
				oss << "\"" << lua_tostring(l, i) << "\"";
				break;
			case LUA_TBOOLEAN:
				oss << (lua_toboolean(l, i) ? "True" : "False");
				break;
			case LUA_TNUMBER:
				oss << (lua_tonumber(l, i));
				break;
			default:
				oss << lua_typename(l, lua_type(l, i));
				break;
			}
			oss << std::endl;
		}
		Logger::Debug(oss.str());
	}

	void PrintInIndex(lua_State * l, int index)
	{
		index = LuaTools::GetPositiveIndex(l, index);
		std::ostringstream oss;
		switch (lua_type(l, index))
		{
		case LUA_TSTRING:
			oss << "\"" << lua_tostring(l, index) << "\"";
			break;
		case LUA_TBOOLEAN:
			oss << (lua_toboolean(l, index) ? "True" : "False");
			break;
		case LUA_TNUMBER:
			oss << (lua_tonumber(l, index));
			break;
		default:
			oss << lua_typename(l, lua_type(l, index));
			break;
		}
		Logger::Debug(oss.str());
	}

	int GetCallDepth(lua_State * l)
	{
		int depth = 0;
		lua_Debug info;
		for (; lua_getstack(l, depth + 1, &info) != 0; depth++);
		return depth;
	}
}
}