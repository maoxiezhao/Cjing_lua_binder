
#include "helper\debug.h"
#include "helper\logger.h"
#include "helper\fileSystem.h"

#include "scripts\luaContext.h"
#include "scripts\luaBinder.h"

#include "test.h"

#include <Windows.h>

using namespace Cjing3D;

template<typename T, typename F>
void TestBindingFunction(lua_State*l, T& object, const F& func)
{
	//using FuncType = BindClassMethodFunc<T, F>;
	//std::cout << FuncType::value << std::endl;

	//LuaTools::BindingUserData::PushUserdata<F>(l, func);
}

enum TestEnumType
{
	TestEnumType_A,
	TestEnumType_B
};

int TestModuleFunction(int value)
{
	std::cout << "Test module function, value:" + std::to_string(value);
	return 0;
}

void TestLuaBinding(LuaContext& context)
{
	lua_State* l = context.GetLuaState();

	// bind class
	LuaBinder(l)
		.BeginClass<Gun>("Gun")
		.AddConstructor(_LUA_ARGS_(std::string))
		.AddMethod("Shoot", &Gun::Shoot)
		.AddFunction("PickUp", &Gun::PickUp)
		.AddMethod("GetName", &Gun::GetName)
		.EndClass();

	//Gun gun;
	//TestBindingFunction(l, gun, &Gun::Shoot);

	// bind module
	LuaBinder(l)
		.BeginModule("Weather")
		.AddConstant("Sun", "Sun")
		.AddEnum("TestEnumType_B", TestEnumType_B)
		.AddFunction("TestModuleFunction", TestModuleFunction)
		.AddFunction("TestLambda", [](int value) { std::cout << value << std::endl; })
		.EndModule();

	// do test lua file
	context.DoFileIfExists(l, "main");
}

int main()
{
	Debug::SetDebugConsoleEnable(true);
	Debug::InitializeDebugConsole();

	std::string dataPath = "./../Assets";
	if (!FileData::OpenData("", dataPath))
		Debug::Die("No data file was found int the direction:" + dataPath);

	LuaContext context;
	context.Initialize();

	TestLuaBinding(context);

	system("pause");
	
	context.Uninitialize();
	FileData::CloseData();
	return 0;
}