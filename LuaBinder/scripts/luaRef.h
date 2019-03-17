#pragma once

#include "lua\lua.hpp"
#include "scripts\luaTools.h"

#include <string>

namespace Cjing3D 
{
	class LuaRef
	{
	public:
		LuaRef();
		LuaRef(lua_State*l, int ref);
		LuaRef(const LuaRef& other);
		LuaRef(LuaRef&& other);
		LuaRef& operator=(const LuaRef&other);
		LuaRef& operator=(LuaRef& other);
		~LuaRef();

		bool operator ==(const LuaRef& ref)const;
		bool operator !=(const LuaRef& ref)const;

		bool IsEmpty()const;
		int  GetRef()const;
		void Push()const;
		void Clear();
		lua_State* GetLuaState()const;

		template<typename K, typename V>
		void RawSet(const K& key, const V& value)
		{
			Push();
			LuaTools::Push(l, key);
			LuaTools::Push(l, value);
			lua_rawset(l, -3);
			lua_pop(l, 1);
		}

		template<typename K, typename V = LuaRef>
		V RawGet(const K& key)
		{
			Push();
			LuaTools::Push(l, key);
			lua_rawget(l, -2);
			V v = LuaTools::Get<V>(l, -1);
			lua_pop(l, 2);
			return v;
		}

		static LuaRef NULL_REF;
	private:
		lua_State * l;
		int mRef;

	};

	template<>
	struct LuaTypeNormalMapping<LuaRef>
	{
		static void Push(lua_State*l, const LuaRef& value)
		{
			if (value.IsEmpty())
			{
				lua_pushnil(l);
			}
			else
			{
				value.Push();
			}
		}

		static LuaRef Get(lua_State*l, int index)
		{
	
			if (!lua_isnil(l, index))
			{
				return LuaRef::NULL_REF;
			}
			else
			{
				return LuaTools::CreateRef(l, index);
			}
		}
	};
}