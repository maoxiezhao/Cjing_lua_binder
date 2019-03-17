#pragma once

#include "lua\lua.hpp"

#include <exception>
#include <string>

namespace Cjing3D {

	class LuaException : public std::exception
	{
	public:
		LuaException(lua_State*l, const std::string& errmsg) : l(l), mErrMsg(errmsg) {};
		lua_State* GetLuaState()const { return l; }
		virtual const char* what()const throw() override { return mErrMsg.c_str(); }
	private:
		std::string mErrMsg;
		lua_State* l;
	};

	void Error(lua_State * l, const std::string & message);
	void ArgError(lua_State * l, int index, const std::string & message);
}