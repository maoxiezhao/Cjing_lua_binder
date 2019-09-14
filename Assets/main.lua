-- Main Lua Function
-- Test Lua Binding

function Main()
	print("Test class binding")
    print(Gun);
    print("hello world!");

    local gun = Gun:new();
    gun:Shoot();

    Gun.PickUp("AAABBCC");

	print("Test module binding");
	print(Weather);
	print(Weather.Sun);
	print("ENUM", TestEnumType_B, Weather.TestEnumType_B)
	Weather.TestModuleFunction(10);
end 

Main();