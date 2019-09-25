-- Main Lua Function
-- Test Lua Binding

function TestBindingClass()
	print("Test class binding")
    print(Gun);
    print("hello world!");

    local gun = Gun:new("Hello");
    gun:Shoot();
    Gun.PickUp("AAABBCC");

	local name = gun:GetName();
	print("name", name);

	print("Test extend class binding")
	local mini_gun = MiniGun:new("Uzi");
	print(mini_gun:GetName());
end 

function TestBindingModule()
	print("Test module binding");
	print(Weather);
	print(Weather.Sun);
	print("ENUM", TestEnumType_B, Weather.TestEnumType_B)
	Weather.TestModuleFunction(10);
	print("");
	Weather.TestLambda(20);
end 

function Main()
	TestBindingClass();
	TestBindingModule();
end 

Main();