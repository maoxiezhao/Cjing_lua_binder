#pragma once

#include <string>
#include <iostream>

// Testing Class
class Gun
{
public:
	Gun(const std::string& name) :mName(name) {}
	~Gun() {}

	void Shoot() { std::cout << "Shoot" << std::endl; }
	void Show() { std::cout << mName << std::endl; }
	std::string GetName()const { return mName; }

	static void PickUp(const std::string& name)
	{
		std::cout << name << std::endl;
	}

private:
	std::string mName;
};