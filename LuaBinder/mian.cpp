
#include "helper\debug.h"
#include "helper\logger.h"
#include "helper\fileSystem.h"

#include "scripts\luaContext.h"
#include "scripts\luaBinder.h"

#include <Windows.h>

using namespace Cjing3D;

// Testing Class
class Gun
{
public:
	Gun() {}
	~Gun() {}

	void Shoot() { std::cout << "Shoot" << std::endl; }
	void Show() { std::cout << mName << std::endl; }

	static void PickUp(const std::string& name)
	{
		std::cout << name << std::endl;
	}

private:
	std::string mName;
};

void TestLuaBinding(LuaContext& context)
{
	lua_State* l = context.GetLuaState();

	// bind class
	LuaBinder(l)
		.BeginClass<Gun>("Gun")
		.AddConstructor()
		.AddFunction()
		.AddMethod()
		.AddMember()
		.EndClass();

	context.DoFileIfExists(l, "main");
}

int main()
{
	Debug::SetDebugConsoleEnable(true);
	Debug::InitializeDebugConsole();

	std::string dataPath = "./../Assets";
	if (!FileData::OpenData("", dataPath))
		Debug::Die("No data file was found int the direcion:" + dataPath);

	LuaContext context;
	context.Initialize();

	TestLuaBinding(context);

	system("pause");
	
	context.Uninitialize();
	FileData::CloseData();
	return 0;
}