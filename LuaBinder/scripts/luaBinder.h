#pragma once

#include "scripts\luaBinder_impl.h"

#include <string>

namespace Cjing3D
{
	// LuaBindClass在bindClass时会创建一个StaticMeta和一个ClassMeta,后者在创建userdata时会setmetatable
	// 同时RegisterStaticFunction会将函数设置在StaticMeta中，而RegisterMethod会将成员函数设置在classmeta

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
			if (BindClassMeta(currentMeta, parentMeta, name, ObjectIDGenerator<T>::GetID()))
			{
				currentMeta.RawGet("__CLASS").RawSet("__gc", &LuaObjectDestructor<T>::Call);
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

		template<typename FUNC>
		LuaBindClass<T, ParentT>& AddFunction(const std::string& name, const FUNC& func)
		{
			Logger::Info("AddFunction");
			using FunctionCaller = BindClassStaticFunc<FUNC>;
			LuaRef funcRef = LuaRef::CreateFuncWithUserdata(mLuaState, &FunctionCaller::Caller, func);
			RegisterStaticFunction(name, funcRef);

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

	// Binding Module
	class LuaBindModuleBase
	{
	public:
		LuaBindModuleBase(LuaRef& meta) :
			mCurrentMeta(meta)
		{}

		LuaBindModuleBase(LuaRef& meta, const std::string& name);

		static int ReadOnlyError(lua_State* l);
		lua_State* GetLuaState() { return mCurrentMeta.GetLuaState(); }

	protected:
		void SetGetter(const std::string& name, const LuaRef& getter);
		void SetSetter(const std::string& name, const LuaRef& setter);
		void SetReadOnly(const std::string& name);

		LuaRef mCurrentMeta = LuaRef::NULL_REF;
		std::string mName;
	};
	
	template<typename ParentT>
	class LuaBindModule : public LuaBindModuleBase
	{
	public:
		LuaBindModule(LuaRef& meta, const std::string& name) :
			LuaBindModuleBase(meta, name),
			mName(name) 
		{}

		LuaBindModule<ParentT>& AddEnum(const std::string& name, int value)
		{
			return AddConstant(name, value);
		}

		template<typename V>
		LuaBindModule<ParentT>& AddConstant(const std::string& name, const V& value)
		{
			lua_State* l = GetLuaState();
			LuaRef ref = LuaRef::CreateRefFromValue(l, value);
			if (ref.IsFunction()) {
				// TODO
			}

			SetGetter(name, ref);
			SetReadOnly(name);
			return *this;
		}

		template<typename V>
		LuaBindModule<ParentT>& AddVariable(const std::string& name, const V& value)
		{
			lua_State* l = GetLuaState();
			SetSetter(name, LuaRef::CreateFuncWithPtr(l, &BindVariableMethod<V>::Setter, &value));
			SetGetter(name, LuaRef::CreateFuncWithPtr(l, &BindVariableMethod<V>::Getter, &value));
			return *this;
		}

		template<typename FUNC>
		LuaBindModule<ParentT>& AddFunction(const std::string& name, const FUNC& func)
		{
			using FunctionCaller = BindClassStaticFunc<FUNC>;
			LuaRef funcRef = LuaRef::CreateFunc(GetLuaState(), &FunctionCaller::Caller, FunctionCaller::ConvertToFunction(func));
			mCurrentMeta.RawSet(name, funcRef);

			return *this;
		}

		template<typename T>
		LuaBindClass<T, LuaBindModule<ParentT>> BeginClass(const std::string& name)
		{
			return LuaBindClass<T, LuaBindModule<ParentT>>::BindClass(GetLuaState(), mCurrentMeta, name);
		}

		LuaBindModule<LuaBindModule<ParentT>> BeginModule(const std::string& name)
		{
			return LuaBindModule<LuaBindModule<ParentT>>(GetLuaState(), mCurrentMeta, name);
		}

		ParentT EndModule()
		{
			Logger::Info("EndModule");
			return ParentT(mCurrentMeta.GetLuaState());
		}

	protected:
		std::string mName;
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

		LuaBindModule<LuaBinder> BeginModule(const std::string& name)
		{
			return LuaBindModule<LuaBinder>(mCurrentMeta, name);
		}

	private:
		lua_State * mLuaState = nullptr;
		LuaRef mCurrentMeta = LuaRef::NULL_REF;
	};

	class AutoLuaBindFunctions
	{
	public:
		static AutoLuaBindFunctions& GetInstance()
		{
			static AutoLuaBindFunctions funcs;
			return funcs;
		}

		void PushAutoBindFunction(std::function<void(lua_State* l)> func);
		void DoAutoBindFunctions(lua_State* l);
		
		std::vector<std::function<void(lua_State* l)>> mfuncs;
	};

#define LUA_FUNCTION_AUTO_BINDER(LuaName, function) \
	class AutoBinder##LuaName{				\
		public:AutoBinder##LuaName(){		\
			AutoLuaBindFunctions::GetInstance().PushAutoBindFunction(function); }	\
		static AutoBinder##LuaName instance;};	\
	 AutoBinder##LuaName  AutoBinder##LuaName::instance;
}