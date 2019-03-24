#include "luaBinder.h"

namespace Cjing3D {

bool LuaBindClassBase::RegisterFunction(const std::string & name, LuaRef func)
{
	LuaRef metaClass = mCurrentMeta.RawGet("__CLASS");
	metaClass.RawSet(name, func);

	return true;
}

}