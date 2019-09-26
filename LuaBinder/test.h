#pragma once

#include <string>
#include <iostream>

// Testing Class

LUA_BINDER_REGISTER_CLASS
class Gun
{
public:
	Gun(const std::string& name) :mName(name) {}
	~Gun() {}

	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	void Shoot() { std::cout << "Shoot" << std::endl; }

	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	void Show() { std::cout << mName << std::endl; }

	LUA_BINDER_REGISTER_CLASS_METHOD_FUNCTION
	std::string GetName()const { return mName; }

	LUA_BINDER_REGISTER_CLASS_STATIC_FUNCTION
	static void PickUp(const std::string& name)
	{
		std::cout << name << std::endl;
	}

	std::string mValue = "Boom!!!!";

protected:
	std::string mName;
};

class MiniGun : public Gun
{
public:
	MiniGun(const std::string& name) : Gun(name) {}

	void Shoot() { std::cout << "Shoot_MINI:" << mName << std::endl; }
	void Drop() { std::cout << "Drop!!!" << std::endl; }
};