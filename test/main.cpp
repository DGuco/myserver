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
public:
	int a;
	int b;
};

int test(std::shared_ptr<Test> shared_ptr1)
{
	auto ptr2 = std::move(shared_ptr1);
	printf("shared_ptr<Test> use count  = %d\n", ptr2.use_count());
	return 0;
}

int main()
{
//	std::map<int, std::shared_ptr<Test>> testMap;
//	testMap.insert(std::make_pair(1, std::make_shared<Test>()));
//	testMap.insert(std::make_pair(2, std::make_shared<Test>()));
//	printf("Before Clear\n");
//	testMap.clear();
	std::shared_ptr<Test> shared_ptr1 = std::make_shared<Test>();
	test(shared_ptr1);
	printf("shared_ptr<Test> use count  = %d\n", shared_ptr1.use_count());
	std::shared_ptr<Test> shared_ptr2 = NULL;
	if (shared_ptr2) {
		printf("Size of shared_ptr<Test> = %d\n", sizeof(shared_ptr1));
	}
	return 0;
}