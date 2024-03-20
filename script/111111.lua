#!/usr/bin/lua

function x11111_callfailedtest(x,y,i,j,world)
    local addNum = LambdaAdd(x,y);
    local subNum = Sub(i,j);
    print("addNum = "..addNum)
    print("subNum = "..subNum)
    print("world = "..world)
    print("x11111_test1 done")
    return 1
end


function x11111_test(x,y)
    local addNum = LuaFnAdd(x,y);
    local subNum = Sub(x,y);
    print("addNum = "..addNum)
    print("subNum = "..subNum)
    local testclass = OuterClass (10)
    --local testclass1 = OuterClass1 (10)
    testclass:Say("Hello cpp");
    --testclass1:Say("Hello cpp");
    local spaceclass = space.OuterClass (10)
    spaceclass:Say("Space Hello cpp");
    local spaceadd = space.Add(100,200)
    print("x11111_test spaceadd = "..spaceadd)
    print("=======================lua===========================")
    local outerclass = LuaFnGetOurterClass(100)
    outerclass:Say("Lua Hello outerclass1");
    print("=======================lua===========================")
    return 1
end