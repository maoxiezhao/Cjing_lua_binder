#include "generator.h"
#include "helper\fileSystem.h"
#include "helper\logger.h"

#include <atlbase.h>

namespace Cjing3D {

// TODO use config file
std::map<std::string, bool>	LuaBindingsGenerator::ignoreFiles = {
	{ "lua",     true },
	{ "build",   true },
	{ "scripts", true },
};

namespace {

const std::string generateStringPrefix = R"(
#include "scripts\luaContext.h"
#include "scripts\luaBinder.h"

using namespace Cjing3D;

)";

std::string WrapAutoBindFunction(const std::string& name, const std::string& stringBuffer)
{
	std::string funcName = name + "_AutoBindFunction";
	std::string result = "void " + funcName + "(lua_State* l)\n{\n";
	result = result + stringBuffer;
	result = result + "	\n}\nLUA_FUNCTION_AUTO_BINDER(" + name + "_auto_binder," + funcName + ")";

	return result;
}

}

void LuaBindingsGenerator::ParseAllHeader(const std::string & path)
{
	auto files = FileData::EnumerateFiles(path);
	for (const std::string file : files)
	{
		if (ignoreFiles.find(file) != ignoreFiles.end()) {
			continue;
		}

		if (file.find(".h") != std::string::npos) {
			std::string fullPath = file;
			if (path != "") {
				fullPath = fullPath + "/" + file;
			}
			ParseHeader(fullPath, file);
		}
		else if (FileData::IsDirectory(file))
		{
			ParseAllHeader(path + "/" + file);
		}
	}
}

bool LuaBindingsGenerator::ParseHeader(const std::string & path, const std::string& name)
{
	OutputDebugString(("Generating lua binding file:" + path).c_str());
	Logger::Info(("Generating lua binding file:" + path).c_str());


	if (!GenerateSource(name)) {
		Logger::Warning("Failed to generate lua binding file:" + path);
		return false;
	}

	return true;
}

std::string LuaBindingsGenerator::GenerateRegisterCode()
{
	std::string registerCode = "";

	return registerCode;
}

bool LuaBindingsGenerator::GenerateSource(const std::string & path)
{
	std::string generateName = path;
	auto lastIndex = path.find_last_of('.');
	if (lastIndex != std::string::npos)
	{
		generateName = path.substr(0, lastIndex);
	}

	std::string stringBuffer = generateStringPrefix;
	stringBuffer = stringBuffer + WrapAutoBindFunction(generateName, GenerateRegisterCode());

	// generate final cpp
	FileData::SaveFile(generateName + "_bindings" + ".cpp", stringBuffer);
	return true;
}



}