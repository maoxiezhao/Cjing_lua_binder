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

				int resultCount = ClassMethodCaller<T, F, R, LuaArgHolder<Args>...>::Call(l, obj, func, args);
				return resultCount;
			});
		}
	};

	template<typename T, typename F, typename Args = F, typename Enable = void>
	struct BindClassMethodFunc{};

	template<typename T, typename F>
	struct BindClassMethodFunc<T, F, F, typename std::enable_if<std::is_function<F>::value>::type> {};

	template<typename T, typename TF, typename R, typename... Args>
	struct BindClassMethodFunc<T, R(TF::*)(Args...)> :			// ==> <typename T, typename F, typename R, typename... Args>
		BindClassMethodFuncCaller<T, R(T::*)(Args...), R, Args...>
	{
		static_assert(std::is_base_of<TF, T>::value);
	};


	// Method function 
	template<typename F, typename R, typename... Args>
	struct BindClassStaicFuncCaller
	{
		// the called lua function
		static int Caller(lua_State* l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				F& func = *static_cast<F*>(lua_touserdata(l, lua_upvalueindex(1)));
				LuaArgValueTuple<Args...> args;
				LuaInputArgs<Args...>::Get(l, 1, args);

				int resultCount = StaticFunctionCaller<F, R, typename LuaArgs<Args>::LuaArgHolderType...>::Call(l, func, args);
				return resultCount;
			});
		}
	};

	template<typename F, typename Args = F, typename Enable = void>
	struct BindClassStaticFunc{};

	template<typename R, typename... Args>
	struct BindClassStaticFunc<R(*)(Args...), R(*)(Args...)> :
		BindClassStaicFuncCaller<R(*)(Args...), R, Args...> {};


	// Bind Module meta method
	struct BindModuleMetaMethod
	{
		static int Index(lua_State* l);
		static int NewIndex(lua_State* l);
	};

	// bind variable setter/getter, pass-by-value
	template<typename T>
	struct BindVariableMethod
	{
		static int Getter(lua_State*l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				LuaTools::CheckAssert(lua_islightuserdata(l, lua_upvalueindex(1)), 
					"BindVariableMethod::Getter upvalue must is lightuserdata.");

				const T* p = static_cast<const T*>(lua_touserdata(l, lua_upvalueindex(1)));
				LuaTools::CheckAssert(p != nullptr, "BindVariableMethod::Getter userdata is nullptr.");

				LuaTools::Push<T>(l, *p);
				return 1;
			});
		}

		static int Setter(lua_State*l)
		{
			return LuaTools::ExceptionBoundary(l, [&] {
				LuaTools::CheckAssert(lua_islightuserdata(l, lua_upvalueindex(1)), 
					"BindVariableMethod::Setter upvalue must is lightuserdata.");

				T* p = static_cast<T*>(lua_touserdata(l, lua_upvalueindex(1)));
				LuaTools::CheckAssert(p != nullptr, "BindVariableMethod::Setter userdata is nullptr.");

				*p = LuaTools::Get<T>(l, 1);

				return 0;
			});
		}
	};
}