#include <iostream>
#include <list>
#include <unordered_map>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <mutex>

#include "rbthash_map.h"
#include "rb_tree.h"

/**
 * https://zhuanlan.zhihu.com/p/31805309
 * https://blog.csdn.net/m0_49374492/article/details/119045823
 * https://zhuanlan.zhihu.com/p/31758048
 * https://www.cnblogs.com/skywang12345/p/3624291.html
 * https://www.cs.usfca.edu/~galles/visualization/RedBlack.html
 * https://www.cnblogs.com/skywang12345/p/3624202.html
 * @return
 */

using namespace rbt_hash;
using namespace std;
#define TEST_COUNT 1000000
#define REMOVE_COUNT 50000
#define RB_COUNT 5000
#define HASH_CONFLICT_COUNT 5
#define HASH_CONFLICT_COUNT1 10000
#define COMSUME_KEEP_TIME 60 * 1000 * 1000
void testRBTree()
{
    printf("==========================test rbtree start===================================\n");
    // 设置种子
    srand( (unsigned)time( NULL ) );
    int *a= new int[RB_COUNT];
    RBTNode<int,unsigned int>* node = new RBTNode<int,unsigned int>[RB_COUNT];
    RBTree<int,int,unsigned int,RB_COUNT>tree(node,0);
    cout << "== 原始数据: ";
    int i = 0;
    std::unordered_map<int,int> stdmap;
    for(i=0; i < RB_COUNT; i++)
    {
        int key = rand();
        while (1)
        {
            if(stdmap.find(key) == stdmap.end())
            {
                break;
            }else
            {
                key = rand();
            }
        }
        a[i] = key;
        stdmap.insert(std::make_pair(key,1));
        cout << a[i] <<" ";
        node[i].init_rb();
        node[i].set_key(a[i]);
    }
    cout << endl;

    for(i=0; i<RB_COUNT; i++)
    {
        tree.insert(&node[i]);
        if(!tree.isRBTree())
        {
            tree.print();
            printf("test rbtree insert failed\n");
            exit(0);
        }
    }

    cout << "== 前序遍历: ";
    std::list<RBTNode<int,unsigned int>*> resList;
    resList.clear();
    tree.preOrder(resList);
    std::list<RBTNode<int,unsigned int>*>::iterator it = resList.begin();
    for(;it != resList.end();it++)
    {
        cout << (*it)->get_key() << " ";
    }
    cout << "\n== 中序遍历: ";
    resList.clear();
    tree.inOrder(resList);
    it = resList.begin();
    for(;it != resList.end();it++)
    {
        cout << (*it)->get_key() << " ";
    }
    cout << "\n== 后序遍历: ";
    resList.clear();
    tree.postOrder(resList);
    it = resList.begin();
    for(;it != resList.end();it++)
    {
        cout << (*it)->get_key() << " ";
    }
    cout << endl;
    cout << "== 最小值: " << tree.minimum()->get_key() << endl;
    cout << "== 最大值: " << tree.maximum()->get_key() << endl;
//    cout << "== 树的详细信息: " << endl;
//    tree.print();
    printf("== test rbtree insert done\n");

    for(i=0; i<RB_COUNT; i++)
    {
        tree.remove(a[i]);
//        cout << "== 删除节点: " << a[i] << endl;
//        cout << "== 树的详细信息: " << endl;
//        tree.print();
        if(!tree.isRBTree())
        {
            tree.print();
            printf("test rbtree remove failed\n");
            exit(0);
        }
    }
    printf("== test rbtree remove done\n");
    delete[] node;
    delete [] a;
    printf("==========================test rbtree done===================================\n");
}

class ValueType
{
public:
    ValueType(int value) : a(value)
    {

    }

    ~ValueType()
    {

    }

    int Key()
    {
        return  a;
    }

    bool IsValid()
    {
        return  a >= 0;
    }

    int a;
};

void testInsert()
{
    // 设置种子
    srand( (unsigned)time( NULL ) );
    printf("==========================test insert start===================================\n");
    RbtHashMap<int,ValueType,TEST_COUNT>* testMap = new RbtHashMap<int,ValueType,TEST_COUNT>();
    std::unordered_map<int,ValueType> stdmap;
    while(true)
    {
        int key = rand();
        if(stdmap.find(key) == stdmap.end())
        {
            stdmap.insert(std::make_pair(key,ValueType(key)));
            testMap->insert(key,key);
        }

        if(stdmap.size() >= TEST_COUNT)
        {
            break;
        }
    }

    if(stdmap.size() != testMap->size())
    {
        printf("test insert failed stdsize = %d,rbtsize = %d\n",stdmap.size(),testMap->size());
        exit(0);
    }

    RbtHashMap<int,ValueType,TEST_COUNT>::iterator it = testMap->begin();
    for(;it != testMap->end();it++)
    {
        std::unordered_map<int,ValueType>::iterator  stdit = stdmap.find(it->first);
        if(stdit == stdmap.end())
        {
            printf("test insert failed never insert key = %d\n",it->first);
            exit(0);
        }
        if(it->second->a != stdit->second.a)
        {
            printf("test insert failed stdvalue= %d,rbtvalue = %d\n",stdit->second.a,it->second->a);
            exit(0);
        }
        //printf("it->first = %d,it->second = %d\n",it->first,it->second->a);
        stdmap.erase(it->first);
    }
    if(stdmap.size() > 0)
    {
        printf("test insert failed look map not all,last =  %d\n",stdmap.size());
        exit(0);
    }

    RbtHashMap<int,int,5> testMap1;
    testMap1[1] = 1;
    testMap1[2] = 2;
    testMap1[3] = 3;
    testMap1[4] = 4;
    testMap1[5] = 5;
    if(!(testMap1[6] = 6))
    {
        printf("[] failed no space\n");
    }
    if(testMap1[1] = 100)
    {
        printf("[]  ok newvalue = %d\n",*(testMap1.find(1)->second));
    }
    RbtHashMap<int,int,5>::const_iterator const_it = testMap1.find(1);
    //*(const_it->second) = 10;
    printf("==========================test insert done===================================\n");
    delete testMap;
    testMap = NULL;
}

void testremove()
{
    // 设置种子
    srand( (unsigned)time( NULL ) );
    printf("==========================test remove begin===================================\n");
    RbtHashMap<int,ValueType,TEST_COUNT>* testMap = new RbtHashMap<int,ValueType,TEST_COUNT>();
    std::unordered_map<int,ValueType> stdmap;
    while(true)
    {
        int key = rand();
        if(stdmap.find(key) == stdmap.end())
        {
            stdmap.insert(std::make_pair(key,ValueType(key)));
            testMap->insert(key,key);
        }

        if(stdmap.size() >= TEST_COUNT)
        {
            break;
        }
    }

    std::unordered_map<int,ValueType>::iterator stdit = stdmap.begin();
    int count = 0;
    for(;stdit != stdmap.end();)
    {
        if(!testMap->try_erase(stdit->first))
        {
            printf("test remove failed never remove key = %d\n",stdit->first);
            exit(0);
        }
        stdit = stdmap.erase(stdit);
        count++;
        if(count >= REMOVE_COUNT)
        {
            break;
        }
    }

    if(stdmap.size() != testMap->size())
    {
        printf("test remove failed stdsize = %d,rbtsize = %d\n",stdmap.size(),testMap->size());
        exit(0);
    }

    RbtHashMap<int,ValueType,TEST_COUNT>::iterator it = testMap->begin();
    count = 0;
    for(;it != testMap->end();it++)
    {
        count++;
        std::unordered_map<int,ValueType>::iterator  stdit = stdmap.find(it->first);
        if(stdit == stdmap.end())
        {
            printf("test remove failed never lost key = %d\n",it->first);
            exit(0);
        }
        if(it->second->a != stdit->second.a)
        {
            printf("test remove failed stdvalue= %d,rbtvalue = %d\n",stdit->second.a,it->second->a);
            exit(0);
        }
        stdmap.erase(it->first);
    }
    if(stdmap.size() > 0)
    {
        printf("test remove failed look map not all,last =  %d\n",stdmap.size());
        exit(0);
    }

    RbtHashMap<int,ValueType,TEST_COUNT>::iterator  beginit = testMap->begin();
    int erasecount = 0;
    int size = testMap->size();
    for(;beginit != testMap->end();)
    {
        beginit = testMap->erase(beginit);
        erasecount++;
    }
    if(erasecount != size || testMap->size() != 0)
    {
        printf("test remove failed size = %d,erasecount = %d\n",size,erasecount);
        exit(0);
    }
    printf("==========================test remove done===================================\n");
    delete testMap;
    testMap = NULL;
}

void testmempool()
{
    // 设置种子
    srand( (unsigned)time( NULL ) );
    printf("==========================test mempool begin===================================\n");
    RbtHashMap<int,ValueType,TEST_COUNT>* testMap = new RbtHashMap<int,ValueType,TEST_COUNT>();
    std::unordered_map<int,ValueType> stdmap;
    while(true)
    {
        int key = rand();
        if(stdmap.find(key) == stdmap.end())
        {
            stdmap.insert(std::make_pair(key,ValueType(key)));
            testMap->insert(key,key);
        }

        if(stdmap.size() >= TEST_COUNT)
        {
            break;
        }
    }

    std::unordered_map<int,ValueType>::iterator stdit = stdmap.begin();
    int count = 0;
    for(;stdit != stdmap.end();)
    {
        if(!testMap->try_erase(stdit->first))
        {
            printf("test mempool failed never remove key = %d\n",stdit->first);
            exit(0);
        }
        stdit = stdmap.erase(stdit);
        count++;
        if(count >= REMOVE_COUNT)
        {
            break;
        }
    }

    if(stdmap.size() != testMap->size())
    {
        printf("test mempool failed stdsize = %d,rbtsize = %d\n",stdmap.size(),testMap->size());
        exit(0);
    }

    while(true)
    {
        int key = rand();
        if(stdmap.find(key) == stdmap.end())
        {
            stdmap.insert(std::make_pair(key,ValueType(key)));
            if(!testMap->insert(key,key))
            {
                printf("test mempool failed some mem not reuse\n",stdmap.size(),testMap->size());
            }
        }

        if(stdmap.size() >= TEST_COUNT)
        {
            break;
        }
    }
    if(stdmap.size() != testMap->size())
    {
        printf("test mempool failed stdsize = %d,rbtsize = %d\n",stdmap.size(),testMap->size());
        exit(0);
    }

    printf("==========================test mempool done===================================\n");
    delete testMap;
    testMap = NULL;
}

// 获取当前微秒
time_t GetUSTime()
{
    struct timeval tmval = {0};
    int nRetCode = gettimeofday(&tmval, NULL);
    if (nRetCode != 0)
    {
        return 0;
    }
    return ((tmval.tv_sec * 1000 * 1000) + tmval.tv_usec);
}

void testformance()
{
    // 设置种子
    srand( (unsigned)time( NULL ) );
    time_t start = 0;
    time_t end = 0;
    unsigned long long res = 0;
    printf("---------------------------test performance begin---------------------------------------------\n");
    RbtHashMap<unsigned long long,ValueType,TEST_COUNT>* testMap = new RbtHashMap<unsigned long long,ValueType,TEST_COUNT>();
    start = GetUSTime();
    for(unsigned long long i = 0;i < TEST_COUNT;i++)
    {
        if(!testMap->insert(i * HASH_CONFLICT_COUNT, ValueType(i)))
        {
            printf("testformance failed key = %ld\n",i * HASH_CONFLICT_COUNT);
            exit(0);
        }
    }
    end = GetUSTime();
    printf("RbtHashMap<int,ValueType,%d>[conflict count = %d], insert use  %ld ms\n",TEST_COUNT,HASH_CONFLICT_COUNT,(end - start) / 1000);
    start = GetUSTime();
    res = 0;
    for(unsigned long long i = 0;i < TEST_COUNT;i++)
    {
        res += testMap->find(i * HASH_CONFLICT_COUNT)->second->a;
    }
    end = GetUSTime();
    printf("RbtHashMap<int,ValueType,%d>[conflict count = %d] find use  %ld ms,res = %llu\n",TEST_COUNT,HASH_CONFLICT_COUNT,(end - start) / 1000,res);
    testMap->clear();
    start = GetUSTime();
    for(unsigned long long i = 0;i < TEST_COUNT;i++)
    {
        if(!testMap->insert(i * HASH_CONFLICT_COUNT1, ValueType(i)))
        {
            printf("testformance failed11 key = %ld\n",i * HASH_CONFLICT_COUNT1);
            exit(0);
        }
    }
    end = GetUSTime();
    printf("RbtHashMap<int,ValueType,%d>[conflict count = %d], insert use  %ld ms\n",TEST_COUNT,HASH_CONFLICT_COUNT1,(end - start) / 1000);
    start = GetUSTime();
    res = 0;
    for(unsigned long long i = 0;i < TEST_COUNT;i++)
    {
        res += testMap->find(i * HASH_CONFLICT_COUNT1)->second->a;
    }
    end = GetUSTime();
    printf("RbtHashMap<int,ValueType,%d>[conflict count = %d] find use  %ld ms,res = %llu\n",TEST_COUNT,HASH_CONFLICT_COUNT1,(end - start) / 1000,res);
    delete testMap;
    testMap = NULL;
    printf("------------------------------------------------------------------------\n");
    {
        std::map<int,ValueType> stdtMap;
        start = GetUSTime();
        for(int i = 0;i < TEST_COUNT;i++)
        {
            stdtMap.insert(std::make_pair(i * HASH_CONFLICT_COUNT, ValueType (i)));
        }
        end = GetUSTime();
        printf("std::map<int,ValueType> insert use  %ld ms\n",(end - start) / 1000);
        start = GetUSTime();
        res = 0;
        for(int i = 0;i < TEST_COUNT;i++)
        {
            res += stdtMap.find(i * HASH_CONFLICT_COUNT)->second.a;
        }
        end = GetUSTime();
        printf("std::map<int,ValueType> find use  %ld ms,res = %llu\n",(end - start) / 1000,res);
    }
    printf("------------------------------------------------------------------------\n");
    {
        std::unordered_map<int,ValueType> testUnorderMap;
        start = GetUSTime();
        for(int i = 0;i < TEST_COUNT;i++)
        {
            testUnorderMap.insert(std::make_pair(i * HASH_CONFLICT_COUNT, ValueType(i)));
        }
        end = GetUSTime();
        printf("std::unordered_map<int,ValueType> insert use  %ld ms\n",(end - start) / 1000);
        res = 0;
        start = GetUSTime();
        for(int i = 0;i < TEST_COUNT;i++)
        {
            res += testUnorderMap.find(i * HASH_CONFLICT_COUNT)->second.a;
        }
        end = GetUSTime();
        printf("std::unordered_map<int,ValueType> find use  %ld ms,res = %llu\n",(end - start) / 1000,res);
    }
    printf("-----------------------------test performance done-------------------------------------------\n");
    return;
}

RbtHashMap<int,ValueType,TEST_COUNT>* g_testMap = new RbtHashMap<int,ValueType,TEST_COUNT>();
std::unordered_map<int,ValueType> g_testUnorderMap;
std::mutex g_mtx;
bool g_exit = 0;
bool g_print1 = 0;
bool g_print2 = 0;
void* consume_insert(void* data)
{
    // 设置种子
    srand( (unsigned)time( NULL ) );
    unsigned int count = 0;
    while(true && !g_exit)
    {
        if(g_print1)
        {
            printf("consume inserting .....\n");
            g_print1 = 0;
        }
        {
            std::lock_guard<std::mutex> lck(g_mtx);
            if(g_testUnorderMap.size() >= TEST_COUNT)
            {
                continue;
            }else
            {
                int key = rand();
                if(g_testUnorderMap.find(key) == g_testUnorderMap.end())
                {
                    g_testUnorderMap.insert(std::make_pair(key,ValueType(key)));
                    if(!g_testMap->insert(key,key))
                    {
                        g_exit = 1;
                        printf("consume_insert insert failed testmap key = %d\n", key);
                        return 0;
                    }
                    count++;
                }
            }
        }
    }
    printf("consume_insert insert count = %d\n", count);
}

void* consume_remove(void* data)
{
    unsigned int count = 0;
    while (true && !g_exit)
    {
        if(g_print2)
        {
            printf("consume removing .....\n");
            g_print2 = 0;
        }
        {
            std::lock_guard<std::mutex> lck(g_mtx);
            std::unordered_map<int, ValueType>::iterator it = g_testUnorderMap.begin();
            if (it != g_testUnorderMap.end())
            {
                if (g_testMap->find(it->first) == g_testMap->end()) {
                    g_exit = 1;
                    printf("consume_remove find failed testmap lost key = %d\n", it->first);
                    return 0;
                }
                int key = it->first;
                g_testUnorderMap.erase(key);
                if (!g_testMap->try_erase(key))
                {
                    g_exit = 1;
                    printf("consume_remove erase failed testmap lost key = %d\n", key);
                    return 0;
                }
                count++;
            }
        }
    }
    printf("consume_insert remove count = %d\n", count);
}


void testconsume()
{
    printf("-----------------------------test consume begin-------------------------------------------\n");
    printf("test consume need time %d S,please waiting\n",COMSUME_KEEP_TIME / 1000 / 1000);
    pthread_t t1,t2;
    time_t start = GetUSTime();
    pthread_create(&t1,0,consume_insert,NULL);
    pthread_create(&t2,0,consume_remove,NULL);
    time_t lasttime = COMSUME_KEEP_TIME;
    time_t printstart = start;
    while (true)
    {
        time_t now = GetUSTime();
        if(now - printstart >= 20 * 1000 * 1000)
        {
            lasttime = lasttime - (now - printstart);
            printf("test consume last time %ld S,please waiting\n",lasttime / 1000 / 1000);
            g_print1 = 1;
            g_print2 = 1;
            printstart = now;
        }
        time_t keep = now - start;
        if(keep >= COMSUME_KEEP_TIME)
        {
            g_exit = 1;
            break;
        }
        usleep(10);
    }
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    printf("test consume keep time %d S\n",COMSUME_KEEP_TIME / 1000 / 1000);
    printf("-----------------------------test consume done-------------------------------------------\n");
}

int main()
{
    testRBTree();
    testInsert();
    testremove();
    testmempool();
    testformance();
    testconsume();
//    RbtHashMap<int,int,100>* testMap = new RbtHashMap<int,int,100>();
//    testMap->insert(1,1);
//    int res = *testMap->find(1)->second;
//    testMap->insert(1,10);
//    res = *testMap->find(1)->second;
//    delete testMap;
//    testMap = NULL;

    std::cout << "Test Done,Hello, World!" << std::endl;
}