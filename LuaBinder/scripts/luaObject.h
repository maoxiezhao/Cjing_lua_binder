#pragma once

#include "scripts\luaRef.h"
#include "scripts\luaTools.h"
#include "scripts\luaInvoke.h"
#include "scripts\luaArg.h"

namespace Cjing3D
{
	class LuaObject;

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

		// ����userdata��ͬʱ����metatable
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
		// ����Ҫ���ǵ��������ڴ��ж���ģʽ
		// ������alignof��void*)ʱ���ض���ָ�룬��С��ʱ�����ǻ������ͣ��򲿷ְ����������͵Ľṹ�壩
		/*using ALIGN_TPYP = std::conditional<alignof(T) <= alignof(void*), T, void*>::type;
		static constexpr int MEM_PADDING = alignof(T) <= alignof(ALIGN_TPYP) ? 0 : alignof(T) - alignof(ALIGN_TPYP) + 1;*/
		unsigned char mData[sizeof(T)];
	};
}