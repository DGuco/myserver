//
// Created by dguco on 18-6-18.
//

#include <iostream>
#include <map>
#include <memory>
using namespace std;

class Test
{
public:
    ~Test()
    {
        printf("~Test\n");
    }
};

int main()
{
    std::map<int, std::shared_ptr<Test>> testMap;
    testMap.insert(std::make_pair(1, std::make_shared<Test>()));
    testMap.insert(std::make_pair(2, std::make_shared<Test>()));
    printf("Before Clear\n");
    testMap.clear();
    printf("After Clear\n");
    return 0;
}