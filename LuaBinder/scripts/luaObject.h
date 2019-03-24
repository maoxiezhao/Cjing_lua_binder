#pragma once

#include "scripts\luaRef.h"
#include "scripts\luaTools.h"

namespace Cjing3D
{
	template<typename T>
	struct LuaObjectIDGenerator
	{
	public:
		// 使用T类型下的LuaObjectIDGenerator结构体的static变量地址作为ID
		static void* GetID()
		{
			static bool id = false;
			return &id;
		}
	};

	class LuaObject
	{
	public:

		void* GetObjectPtr()
		{
			return nullptr;
		}

		template<typename T>
		static T* GetObject(lua_State*l, int index)
		{
			void* classID = LuaObjectIDGenerator<T>::GetID();
			return static_cast<T*>(GetObject(l, index, classID)->GetObjectPtr());
		}

	private:
		static LuaObject* GetObject(lua_State*l, int index, void* classID);
	};
}