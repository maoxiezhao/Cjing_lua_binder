#pragma once

#include "scripts\luaInvoke.h"
#include "scripts\luaTools.h"
#include "scripts\luaRef.h"
#include "scripts\luaObject.h"

namespace Cjing3D
{
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

				int resultCount = ClassMethodCaller<T, F, R, Args...>::Call(l, obj, func, args);

				return resultCount;
			});
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

	inline bool BindClassMeta(LuaRef& currentMeta, LuaRef& parentMeta, const std::string& name)
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

		LuaRef staticClassTable = LuaRef::CreateTable(l);
		staticClassTable.RawSet("__CLASS", classTable);

		parentMeta.RawSet(name, staticClassTable);

		// no use now.
		LuaRef constClassTable = LuaRef::CreateTable(l);
		constClassTable.SetMetatable(constClassTable);
		staticClassTable.RawSet("__CONST", constClassTable);

		currentMeta = staticClassTable;
		return true;
	}
}