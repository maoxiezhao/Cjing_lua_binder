#pragma once

#include "scripts\luaRef.h"
#include "scripts\luaTools.h"
#include "scripts\luaInvoke.h"
#include "scripts\luaArg.h"

namespace Cjing3D
{
	class LuaObject;

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
			return LuaTools::ExceptionBoundary(l, [&] {
				LuaObject* obj = LuaObject::GetLuaObject<T>(l, 1);
				if (obj != nullptr) {
					obj->~LuaObject();
				}

				return 0;
			});
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
		LuaObject() {}
		virtual ~LuaObject() {}

		virtual void* GetObjectPtr()
		{
			return nullptr;
		}

		template<typename T>
		static LuaObject* GetLuaObject(lua_State*l, int index)
		{
			void* classID = LuaObjectIDGenerator<T>::GetID();
			return GetLuaObject(l, index, classID);
		}

		template<typename T>
		static T* GetObject(lua_State*l, int index)
		{
			void* classID = LuaObjectIDGenerator<T>::GetID();
			LuaObject* obj = GetLuaObject(l, index, classID);
			return obj ? static_cast<T*>(obj->GetObjectPtr()) : nullptr;
		}

		// 分配userdata，同时设置metatable
		template<typename T>
		static void* Allocate(lua_State*l, void* classID)
		{
			void* mem = lua_newuserdata(l, sizeof(T));
			lua_rawgetp(l, LUA_REGISTRYINDEX, classID);
			lua_setmetatable(l, -2);
			return mem;
		}

		// __index and __newindex
		static bool CheckLuaObjectMetatableValid(lua_State* l, int index);
		static int MetaFuncIndex(lua_State* l);
		static int MetaFuncNewIndex(lua_State* l);

	private:
		static LuaObject* GetLuaObject(lua_State*l, int index, void* classID);
	};

	// lua manage, create by lua, destory by lua gc
	template<typename T>
	class LuaHandleObject : public LuaObject
	{
	public:
		LuaHandleObject()
		{
		}

		~LuaHandleObject()
		{
			T* obj = static_cast<T*>(GetObjectPtr());
			obj->~T();
		}

		template<typename... Args>
		static void Push(lua_State* l, std::tuple<Args...>& args)
		{
			void* classID = LuaObjectIDGenerator<T>::GetID();
			void *mem = Allocate<T>(l, classID);
			LuaHandleObject<T>* obj = new(mem) LuaHandleObject<T>();
			ClassStructorCaller<T>::Call(obj->GetObjectPtr(), args);
		}

		virtual void* GetObjectPtr()
		{
			return &mData[0];
		}

	private:
		// 还需要考虑到对象在内存中对齐模式
		// 当大于alignof（void*)时，必定是指针，当小于时可能是基础类型（或部分包含基础类型的结构体）
		/*using ALIGN_TPYP = std::conditional<alignof(T) <= alignof(void*), T, void*>::type;
		static constexpr int MEM_PADDING = alignof(T) <= alignof(ALIGN_TPYP) ? 0 : alignof(T) - alignof(ALIGN_TPYP) + 1;*/
		unsigned char mData[sizeof(T)];
	};
}