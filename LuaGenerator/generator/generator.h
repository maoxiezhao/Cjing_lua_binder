#pragma once

#include <string>
#include <map>

namespace Cjing3D {

class LuaBindingsGenerator
{
public:
	static std::map<std::string, bool> ignoreFiles;		// ºöÂÔµÄÎÄ¼ş

	void ParseAllHeader(const std::string& path);

private:
	bool ParseHeader(const std::string& path, const std::string& name);
	std::string GenerateRegisterCode();
	bool GenerateSource(const std::string& path);

public:
	LuaBindingsGenerator() = default;
};
}