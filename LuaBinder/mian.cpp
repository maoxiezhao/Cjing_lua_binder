
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

template<typename T, typename F>
void TestBindingFunction(lua_State*l, T& object, const F& func)
{
	//using FuncType = BindClassMethodFunc<T, F>;
	//std::cout << FuncType::value << std::endl;

	//LuaTools::BindingUserData::PushUserdata<F>(l, func);
}

void TestLuaBinding(LuaContext& context)
{
	lua_State* l = context.GetLuaState();

	// bind class
	LuaBinder(l)
		.BeginClass<Gun>("Gun")
		.AddConstructor(_LUA_ARGS_())
		.AddMethod("Shoot", &Gun::Shoot)
		.AddFunction("PickUp", &Gun::PickUp)
		.EndClass();

	context.DoFileIfExists(l, "main");

	Gun gun;
	TestBindingFunction(l, gun, &Gun::Shoot);
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