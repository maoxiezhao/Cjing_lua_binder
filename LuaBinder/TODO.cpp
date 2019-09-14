
// 目前LuaBinder已经支持
// 1.绑定Class对象
// 2.绑定非const成员函数和静态函数，通过模板技术实现对函数参数的解析（详见luaInvoke.h)
// 3. LuaTypeMapping模板支持对对象的压栈和出栈操作
// 4. LuaObject派生的LuaHandleObject和LuaObjectPtr，分别支持由Lua端创建对象和将C++对象指针传入Lua
// 5. 支持绑定Module下的const变量和普通变量以及非成员函数

// TODO
// 1. 支持const成员函数
// 1. 支持派生对象绑定（metatable要存储super信息)
// 2. 支持绑定成员变量和全局Const值
// 3. 通过宏来实现自动化绑定

