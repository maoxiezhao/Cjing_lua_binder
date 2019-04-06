#pragma once

#include "scripts\luaRef.h"
#include "scripts\luaTools.h"
#include "scripts\luaInvoke.h"
#include "scripts\luaArg.h"

namespace Cjing3D
{
	// 为了支持在编译期，通过AddConstructor中传入参数的不同，来
	// 实现LuaObjectConstructor::Call时通过typenam...Args来解析各个参数，
	// AddConstructor 将传入一个函数指针，参数为AddConstructor传入的参数，
	// 定义为__args(*)(__VA_ARGS__)

	struct _lua_args {};

	#define _LUA_ARGS_TYPE(...) _lua_args(*)(__VA_ARGS__) 
	#define  _LUA_ARGS_(...) static_cast<_LUA_ARGS_TYPE(__VA_ARGS__)>(nullptr)


	template<typename T, typename ARGS>
	struct LuaObjectConstructor;

	template<typename T, typename...Args>
	struct LuaObjectConstructor<T, _lua_args(*)(Args...)>
	{
		static int Call(lua_State*l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				LuaArgValueTuple<Args...> args;
				LuaInputArgs<Args...>::Get(l, 2, args);
				LuaHandleObject<T>::Push(l, args);

				return 1;
			});
		}
	};

	template<typename T>
	struct LuaObjectDestructor
	{
		static int Call(lua_State*l)
		{
			return 0;
		}
	};

	template<typename T>
	struct LuaObjectIDGenerator
	{
	public:
		// 使用T类型下的LuaObjectIDGenerator结构体的static变量地址作为ID
		// 作为某个对象的位移标识符
		static void* GetID()
		{
			static bool id = false;
			return &id;
		}
	};

	// lua base object
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
		static LuaObject* GetObject(lua_State*l, int index, void* classID)
		{
			return nullptr;
		}
	};

	// lua manage
	template<typename T>
	class LuaHandleObject : public LuaObject
	{
	public:
		template<typename... Args>
		static void Push(lua_State* l, std::tuple<Args...>& args)
		{
			void *mem = nullptr;
			LuaHandleObject<T>* obj = new(mem) LuaHandleObject<T>();
			ClassStructorCaller<T>::Call(obj->GetObjectPtr(), args);
		}
	};
}