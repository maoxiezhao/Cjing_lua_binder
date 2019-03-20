#pragma once

#include "scripts\luaBinder_impl.h"

#include <string>

namespace Cjing3D
{
	// 1.��bindClassʱ������3����Ϊ��ʵ�ֲ�ͬ�ķ��ʿ��ƣ��ֱ𴴽�class, const_class, static_class
	//  class��const_class�������õ�static_class, ��static_class�������class��const_class


	// Binding Class
	// used for add func, add member

	template<typename T, typename ParentT>
	class LuaBindClass
	{
	public:
		static LuaBindClass<T, ParentT> BindClass(lua_State* l, LuaRef& parentMeta, const std::string& name)
		{
			Logger::Info("BindClass");
			LuaRef currentMeta;
			LuaBinderImpl::RegisterClass(currentMeta, parentMeta, name);
			
			return LuaBindClass<T, ParentT>(l, currentMeta);
		}

		LuaBindClass<T, ParentT>& AddConstructor()
		{
			Logger::Info("AddConstructor");
			return *this;
		}

		LuaBindClass<T, ParentT>& AddMetaFunction()
		{
			Logger::Info("AddMetaFunction");
			return *this;
		}

		LuaBindClass<T, ParentT>& AddFunction()
		{
			Logger::Info("AddFunction");
			return *this;
		}

		template<typename T, typename FUNC>
		LuaBindClass<T, ParentT>& AddMethod(const std::string& name, const FUNC& func)
		{
			Logger::Info("AddMethod");
			using MethodCaller = LuaBinderImpl::BindClassMethodFunc<T, FUNC>;
			LuaRef funcRef = LuaTools::CreateFuncWithUserdata(l, MethodCaller::Caller, func);
			RegisterFunction(name, funcRef);

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
			mLuaState(l),
			mCurrentMeta(meta)
		{}

		bool RegisterFunction(const std::string& name, LuaRef func)
		{
			return true;
		}

		lua_State * mLuaState = nullptr;
		LuaRef mCurrentMeta = LuaRef::NULL_REF;
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