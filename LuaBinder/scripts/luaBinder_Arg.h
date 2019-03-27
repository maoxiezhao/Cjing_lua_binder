#pragma once

#include "scripts\luaTools.h"
#include "scripts\luaRef.h"
#include "scripts\luaObject.h"

#include <tuple>

namespace Cjing3D
{
namespace LuaBinderImpl
{

template<typename T>
struct LuaArgHolder
{
    T& Get() { return mValue; }
    const T& Get()const { return mValue; }
    void Set(const T& value) { mValue = value;}
    T mValue;
};
template<typename T>
struct LuaArgHolder<T&>
{
    T& Get() { return *mValue; }
    void Set(T& value) { mValue = &value;}
    T* mValue;
};

// 从lua stack 中取出数据，并将数据赋给LuaArgHover
template<typename T>
struct LuaArgGetter
{
    static int Get(lua_State*l, int index, LuaArgHolder<T>& holder)
    {
        auto& value = LuaTools::Get<T>(l, index);
        holder.Set(value);
        return 1;
    }
};

template<typename T>
struct LuaArgSetter;

template<typename T>
struct LuaArgs
{
    static int Get(lua_State*l, int index, LuaArgHolder& holder)
    {
        return LuaArgGetter<T>::Get(l, index, holder);        
    }
};

// 依次遍历获取各个参数
template<typename... Args>
struct LuaInputArgs;

template<>
struct LuaInputArgs<> 
{
    template<typename... T>
    static void Get(lua_State*l, int index, std::tuple<T...>& valueTuple) {}   
};

template<typename P, typename... Args>
struct LuaInputArgs<P, Args... >
{
    template<typename... T>
    static void Get(lua_State*l, int index, std::tuple<T...>& valueTuple)
    {
        // handle current arg
        auto& value = std::get<sizeof...(T) - sizeof...(Args) - 1>(valueTuple);
        index += LuaArgs<P>::Get(l, index, value);
        return LuaInputArgs<Args...>::Get(l, index, valueTuple);
    }
};

template<typename... Args>
using LuaArgValueTuple = std::tuple<typename LuaArgHolder<Args>...>;

}
}