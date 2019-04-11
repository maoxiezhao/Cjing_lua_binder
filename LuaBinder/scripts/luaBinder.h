#pragma once

#include "scripts\luaBinder_impl.h"

#include <string>

namespace Cjing3D
{
	// 1.��bindClassʱ������3������Ϊ��ʵ�ֲ�ͬ�ķ��ʿ��ƣ��ֱ𴴽�class, const_class, static_class
	//  class��const_class�������õ�static_class, ��static_class�������class��const_class

	class LuaBindClassBase
	{
	public:
		LuaBindClassBase(lua_State* l, LuaRef& meta) :
			mLuaState(l),
			mCurrentMeta(meta)
		{}

	protected:
		static bool BindClassMeta(LuaRef& currentMeta, LuaRef& parentMeta, const std::string& name, void* currentClassID);

		void RegisterStaticFunction(const std::string& name, LuaRef func);
		void RegisterMethod(const std::string& name, LuaRef func);

		lua_State * mLuaState = nullptr;
		LuaRef mCurrentMeta = LuaRef::NULL_REF;
	};

	// Binding Class
	// used for add func, add member
	template<typename T, typename ParentT>
	class LuaBindClass : public LuaBindClassBase
	{
	public:
		static LuaBindClass<T, ParentT> BindClass(lua_State* l, LuaRef& parentMeta, const std::string& name)
		{
			Logger::Info("BindClass");
			LuaRef currentMeta;
			if (BindClassMeta(currentMeta, parentMeta, name, LuaObjectIDGenerator<T>::GetID()))
			{
				currentMeta.RawGet("__CLASS").RawSet("__gc", LuaObjectDestructor<T>::Call);
			}
			
			return LuaBindClass<T, ParentT>(l, currentMeta);
		}

		template<typename ARGS>
		LuaBindClass<T, ParentT>& AddConstructor(ARGS)
		{
			// 1. local obj = CLAZZ(...);
			// 2. local obj = CLAZZ:new(...);

			mCurrentMeta.RawSet("__call", LuaObjectConstructor<T, ARGS>::Call);
			mCurrentMeta.RawSet("new",	  LuaObjectConstructor<T, ARGS>::Call);

			Logger::Info("AddConstructor");
			return *this;
		}

		LuaBindClass<T, ParentT>& AddMetaFunction()
		{
			Logger::Info("AddMetaFunction");
			return *this;
		}

		LuaBindClass<T, ParentT>& AddFunction(const std::string& name, const FUNC& func)
		{
			Logger::Info("AddFunction");
			using FunctionCaller = BindClassStaticFunc<FUNC>;
			LuaRef funcRef = LuaRef::CreateFuncWithUserdata(mLuaState, &FunctionCaller::Caller, func);
			RegisterStaticFunction(l, funcRef);

			return *this;
		}

		template<typename FUNC>
		LuaBindClass<T, ParentT>& AddMethod(const std::string& name, const FUNC& func)
		{
			Logger::Info("AddMethod");
			using MethodCaller = BindClassMethodFunc<T, FUNC>;
			LuaRef funcRef = LuaRef::CreateFuncWithUserdata(mLuaState, &MethodCaller::Caller, func);
			RegisterMethod(name, funcRef);

			return *this;
		}

		LuaBindClass<T, ParentT>& AddMember()
		{
			Logger::Info("AddMember");
			return *this;
		}

		ParentT EndClass()
		{
			Logger::Info("EndClass");
			return ParentT(mLuaState);
		}

	private:
		LuaBindClass(lua_State* l, LuaRef& meta) : 
			LuaBindClassBase(l, meta)
		{}
	};

	class LuaBinder
	{
	public:
		LuaBinder(lua_State* l) :
			mLuaState(l)
		{
			mCurrentMeta = LuaRef::CreateGlobalRef(l);
		}

		template<typename T>
		LuaBindClass<T, LuaBinder> BeginClass(const std::string& name)
		{
			return LuaBindClass<T, LuaBinder>::BindClass(mLuaState, mCurrentMeta, name);
		}

	private:
		lua_State * mLuaState = nullptr;
		LuaRef mCurrentMeta = LuaRef::NULL_REF;
	};

}