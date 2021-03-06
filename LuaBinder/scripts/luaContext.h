#pragma once

#include "scripts\luaRef.h"
#include "scripts\luaTools.h"
#include "scripts\luaBinder.h"

#include <string>

namespace Cjing3D
{
class LuaContext
{
public:
	LuaContext();
	~LuaContext();

	void Initialize();
	void Update();
	void Uninitialize();

	static FunctionExportToLua
		api_panic;

	static bool DoFileIfExists(lua_State*l, const std::string& name);
	static bool LoadFile(lua_State*l, const std::string& name);

	lua_State* GetLuaState() { return mLuaState; }

private:

	lua_State* mLuaState = nullptr;
};
}