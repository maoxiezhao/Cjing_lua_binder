#pragma once

#include "scripts\luaRef.h"
#include "scripts\luaTools.h"
#include "scripts\luaInvoke.h"
#include "scripts\luaArg.h"

namespace Cjing3D
{
	class LuaObject;

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

	///////////////////////////////////////////////////////////////////////////
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
			void* classID = ObjectIDGenerator<T>::GetID();
			return GetLuaObject(l, index, classID);
		}

		template<typename T>
		static T* GetObject(lua_State*l, int index)
		{
			void* classID = ObjectIDGenerator<T>::GetID();
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

	// ������Lua��������������Ҳ��Lua����������GCʱ����ö������������
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

		// ����Args����һ���µĶ�����Lua�����__call�����е���
		template<typename... Args>
		static void Push(lua_State* l, std::tuple<Args...>& args)
		{
			void* classID = ObjectIDGenerator<T>::GetID();
			void *mem = Allocate<LuaHandleObject<T>>(l, classID);
			LuaHandleObject<T>* obj = new(mem) LuaHandleObject<T>();
			ClassStructorCaller<T>::Call(obj->GetObjectPtr(), args);
		}

		// ��CPP�˵��ã����ݿ������캯����������ѹջ
		// �������BindClass
		static void Push(lua_State* l, const T& obj)
		{
			void* classID = ObjectIDGenerator<T>::GetID();
			void *mem = Allocate<LuaHandleObject<T>>(l, classID);
			LuaHandleObject<T>* lua_obj = new(mem) LuaHandleObject<T>();
			new(lua_obj->GetObjectPtr()) T(obj);
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

	// ָ����������C++����������������C++����������GCʱ����ö������������
	template<typename T>
	class LuaObjectPtr : public LuaObject
	{
	public:
		LuaObjectPtr(T& obj) :
			mObjectPtr(&ojb)
		{
		}

		// ָ����󲻻ᴴ���µ�ʵ������ֻ�Ǵ���һ���µ�LuaObjectPtrָ�����
		static void Push(lua_State* l, const T& obj)
		{
			void* classID = ObjectIDGenerator<T>::GetID();
			void *mem = Allocate<LuaObjectPtr<T>>(l, classID);
			new(mem) LuaObjectPtr<T>(obj);
		}

		virtual void* GetObjectPtr()
		{
			return mObjectPtr;
		}

	private:
		void* mObjectPtr = nullptr;
	};

	///////////////////////////////////////////////////////////////////////////

	// ��ѹջ�Ķ�����Ҫ�ж϶����Ƿ���ref
	template<typename T, bool IsRef>
	struct LuaObjectHandler {};

	// ��ref������ֱ�ӿ�������һ���µ�ʵ��
	template<typename T>
	struct LuaObjectHandler<T, false>
	{
		static void Push(lua_State*l, const T&value)
		{
			LuaHandleObject<T>::Push(l, value);
		}

		static T& Get(lua_State*l, int index)
		{
			LuaObject* obj = LuaObject::GetLuaObject<T>(l, index);
			return *static_cast<T*>(obj->GetObjectPtr());
		}
	};

	// ref�����򴴽�һ��LuaPtrObject����ѹջ
	template<typename T>
	struct LuaObjectHandler<T, true>
	{
		static void Push(lua_State*l, const T&value)
		{
			LuaObjectPtr<T>::Push(l, value);
		}

		static T& Get(lua_State*l, int index)
		{
			LuaObject* ojb = LuaObject::GetLuaObject<T>(l, index);
			return *static_cast<T*>(obj->GetObjectPtr());
		}
	};

	template<typename T, bool IsRef>
	struct LuaTypeClassMapping
	{
		static void Push(lua_State*l, const T&value)
		{
			LuaObjectHandler<T, IsRef>::Push(l, value);
		}

		static T& Get(lua_State*l, int index)
		{
			return LuaObjectHandler<T, IsRef>::Get(l, index);
		}
	};
}