#include "luaObject.h"

namespace Cjing3D
{
	LuaObject * LuaObject::GetLuaObject(lua_State * l, int index, void * classID)
	{
		index = LuaTools::GetPositiveIndex(l, index);
		if (!lua_isuserdata(l, index))
		{
			LuaException::Error(l, "Except userdata, got ." + 
				std::string(lua_typename(l, lua_type(l, index))));
			return;
		}

		lua_rawgetp(l, LUA_REGISTRYINDEX, classID);
		if (!lua_istable(l, -1))
		{
			LuaException::Error(l, "Invalid class without metatable.");
			return;
		}

		// check class
		// stack: obj args... except_meta
		lua_getmetatable(l, index);
		if (!lua_istable(l, -1))
		{
			LuaException::Error(l, "Invalid obj without metatable.");
			return;
		}

		// except_meta crrent_meta
		bool is_valid = false;
		while(lua_isnil(l, -1))
		{
			if (lua_rawequal(l, -1, -2))
			{
				is_valid = true;
				break;
			}

			// get super class
			lua_pushstring(l, "__SUPER");
			lua_rawget(l, -2);

			// stack: except_meta current_meta nil/super_meta
			lua_remove(l, -2);
		}

		lua_pop(l, 2);

		// stack: obj args... 
		if (is_valid == true)
		{
			return static_cast<LuaObject*>(lua_touserdata(l, index));
		}
		else
		{
			return nullptr;			
		}
	}
}