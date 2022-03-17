# Cjing_lua_binder
一个自动化的C++/Lua的绑定库
1. 预处理工具会预先扫描对应的代码文件，识别绑定宏，然后自动生成对应的绑定代码
2. LuaBinder绑定模块采用纯模板实现
TestCode
```cpp
LUA_BINDER_REGISTER_CLASS
class Gun
{
public:
	LUA_BINDER_REGISTER_CLASS_CONSTRUCTOR
	Gun(const std::string& name) :mName(name) {}
	~Gun() {}

	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	void Shoot() { std::cout << "Shoot" << std::endl; }

	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	void Show() { std::cout << mName << std::endl; }

	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	std::string GetName()const { return mName; }

	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	static void PickUp(const std::string& name)
	{
		std::cout << name << std::endl;
	}

	std::string mValue = "Boom!!!!";

protected:
	std::string mName;
};
```
Generated
```cpp
void luabind_registers_AutoBindFunction(lua_State* l)
{
LuaBinder(l)
.BeginClass<Gun>("Gun")
.AddConstructor(_LUA_ARGS_(std::string&))
.AddMethod("Shoot", &Gun::Shoot)
.AddMethod("Show", &Gun::Show)
.AddMethod("GetName", &Gun::GetName)
.AddFunction("PickUp", &Gun::PickUp)
.EndClass();
}
```
