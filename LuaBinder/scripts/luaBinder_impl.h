#pragma once

#include "scripts\luaTools.h"
#include "scripts\luaRef.h"
#include "scripts\luaObject.h"
#include "scripts\luaBinder_Arg.h"

namespace Cjing3D
{
namespace LuaBinderImpl
{
	// use size_t... INDEX to get args
	template<typename T, typename FUNC, typename R, typename TUPLE, size_t... INDEX>
	struct ClassMethodDispatchCaller<T, FUNC, R, TUPLE, 0, INDEX...>
	{
		static R Call(T& t, FUNC& func, TUPLE& args)
		{
			return (t.(*func))(std::get<INDEX>(args).Get()...);
		}
	};
	template<typename T, typename FUNC, typename R, typename TUPLE, size_t N, size_t... INDEX>
	struct ClassMethodDispatchCaller : ClassMethodDispatchCaller<T, FUNC, R, TUPLE, N - 1, N - 1, INDEX...> {};

	// Class Method Caller
	template<typename T, typename FUNC, typename R, typename... Args>
	struct ClassMethodCaller
	{
		static int Call(lua_State*l, T& t, const FUNC& func, std::tuple<Args...>& args)
		{
			R result = ClassMethodDispatchCaller<T, FUNC, R, std::tuple<Args...>, sizeof...(args)>::Call(t, func, args);
			LuaTools::Push<R>(l, result);
			return 1;
		}
	};
	template<typename T, typename FUNC, typename... Args>
	struct ClassMethodCaller<T, FUNC, void, Args...>
	{
		static int Call(lua_State*l, T& t, const FUNC& func, std::tuple<Args...>& args)
		{
			ClassMethodDispatchCaller<T, FUNC, R, std::tuple<Args...>, sizeof...(args)>::Call(t, func, args);
			return 0;
		}
	};

	// Method function 
	template<typename T, typename F, typename R, typename... Args>
	struct BindClassMethodFuncCaller
	{
		// the called lua function
		static int Caller(lua_State* l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				F& func = *static_cast<F*>(lua_touserdata(l, lua_upvalueindex(1)));
				T* obj = LuaObject::GetObject<T>(l, 1);
				LuaArgValueTuple<Args...> args;
				LuaInputArgs<Args...>::Get(l, 2, args);

				int resultCount = ClassMethodCaller<T, F, R, Args...>::Call(l, *obj, func, args);

				return resultCount;
			}
		}
	};

	template<typename T, typename F, typename Args = F, typename Enable = void>
	struct BindClassMethodFunc
	{
		static constexpr int value = 1;
	};

	template<typename T, typename F>
	struct BindClassMethodFunc<T, F, F, typename std::enable_if<std::is_function<F>::value>::type> {
		static constexpr int value = 10;
	};

	template<typename T, typename TF, typename R, typename... Args>
	struct BindClassMethodFunc<T, R(TF::*)(Args...)> :			// ==> <typename T, typename F, typename R, typename... Args>
		BindClassMethodFuncCaller<T, R(T::*)(Args...), R, Args...>
	{
		// ���ӵĳ�Ա�������Ե�ǰ���ͬ����߸�����Ч
		static_assert(std::is_base_of<TF, T>::value);
		static constexpr int value = 2;
	};

	/////////////////////////////////////////////////////////////////////////////////////////

	struct BindClassMetaMethod
	{
		static int index(lua_State* l);
		static int newIndex(lua_State* l);
		static int gc(lua_State* l);
	};

	inline bool RegisterClass(LuaRef& currentMeta, LuaRef& parentMeta, const std::string& name)
	{
		LuaRef ref = parentMeta.RawGet(name);
		if (ref != LuaRef::NULL_REF)
		{
			currentMeta = ref;
			return false;
		}

		lua_State* l = parentMeta.GetLuaState();

		LuaRef classTable = LuaRef::CreateTable(l);
		classTable.SetMetatable(classTable);

		// no use now.
		LuaRef constClassTable = LuaRef::CreateTable(l);
		constClassTable.SetMetatable(constClassTable);

		LuaRef staticClassTable = LuaRef::CreateTable(l);
		staticClassTable.RawSet("__CLASS", constClassTable);
		staticClassTable.RawSet("__CONST", constClassTable);

		parentMeta.RawSet(name, staticClassTable);

		currentMeta = staticClassTable;
		return true;
	}
}
}