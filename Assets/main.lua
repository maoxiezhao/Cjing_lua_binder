-- Main Lua Function
-- Test Lua Binding

function Main()
    print(Gun);
    print("hello world!");

    local gun = Gun:new();
    gun:Shoot();
end 

Main();