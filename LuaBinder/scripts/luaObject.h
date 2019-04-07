#pragma once

#include "scripts\luaRef.h"
#include "scripts\luaTools.h"
#include "scripts\luaInvoke.h"
#include "scripts\luaArg.h"

namespace Cjing3D
{
	// Ϊ��֧���ڱ����ڣ�ͨ��AddConstructor�д�������Ĳ�ͬ����
	// ʵ��LuaObjectConstructor::Callʱͨ��typenam...Args����������������
	// AddConstructor ������һ������ָ�룬����ΪAddConstructor����Ĳ�����
	// ����Ϊ__args(*)(__VA_ARGS__)
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
		// ʹ��T�����µ�LuaObjectIDGenerator�ṹ���static������ַ��ΪID
		// ��Ϊĳ�������λ�Ʊ�ʶ��
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

		// ����userdata��ͬʱ����metatable
		template<typename T>
		static void* Allocate(lua_State*l, void* classID)
		{
			void* mem = lua_newuserdata(l, sizeof(T));
			lua_rawgetp(l, LUA_REGISTRYINDEX, classID);
			lua_setmetatable(l, -2);
			return mem;
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
			void* classID = LuaObjectIDGenerator<T>::GetID();
			void *mem = Allocate<T>(l, classID);
			LuaHandleObject<T>* obj = new(mem) LuaHandleObject<T>();
			ClassStructorCaller<T>::Call(obj->GetObjectPtr(), args);
		}
	};
}