//
// Created by dguco on 18-9-9.
//

#ifndef SERVER_TEST_H
#define SERVER_TEST_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <zconf.h>
#include <dlfcn.h>
#include <sys/time.h>
#include <unordered_map>
#include <base.h>
//
// Created by dguco on 18-6-18.
//

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

void listenFileChange()
{
    printf("Begin listen file changed\n");
    while (true) {
        int length, i = 0;
        int fd;
        int wd;
        char buffer[EVENT_BUF_LEN];

        //创建inotify实例
        fd = inotify_init( );
        //检测是否成功
        if (fd < 0) {
            perror("inotify_init");
        }
        //监听文件
        wd = inotify_add_watch(fd, "/data/test", IN_CREATE | IN_DELETE | IN_MODIFY);

        length = read(fd, buffer, EVENT_BUF_LEN);

        if (length < 0) {
            perror("read");
        }

        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->len) {
                if (event->mask & IN_CREATE) {
                    if (event->mask & IN_ISDIR) {
                        printf("New directory %s created.\n", event->name);
                    }
                    else {
                        printf("New file %s created.\n", event->name);
                    }
                }
                else if (event->mask & IN_DELETE) {
                    if (event->mask & IN_ISDIR) {
                        printf("Directory %s deleted.\n", event->name);
                    }
                    else {
                        printf("File %s deleted.\n", event->name);
                    }
                }
                else if (event->mask & IN_MODIFY) {
                    if (event->mask & IN_ISDIR) {
                        printf("Directory %s modify.\n", event->name);
                    }
                    else {
                        printf("File %s modify.\n", event->name);
                    }
                }
            }
            i += EVENT_SIZE + event->len;
        }
        sleep(1);
        inotify_rm_watch(fd, wd);
        close(fd);
    }
}

void testDlopen()
{
    //表示要将库装载到内存，准备使用
    long time1 = GetUSTime( );
    void *lib_handle = dlopen("/usr/local/lib/libprotoc.so.15.0.1", RTLD_NOW);
    if (!lib_handle) {
        fprintf(stderr, "%s\n", dlerror( ));
        return;
    }
    long time2 = GetUSTime( );
    printf("Load Time: %ld us\n", time2 - time1);
    time1 = GetUSTime( );
    dlclose(lib_handle);
    time2 = GetUSTime( );
    printf("Close Time: %ld us\n", time2 - time1);
}

int testSoHotLoad()
{
//////////////////////////////////////////////////////////////////////////////////////////
    testDlopen( );
    void *lib_handle;
    int (*sum)(int, int);
    int (*sub)(int, int);
    char *error;
    int a = 5, b = 8;
    //表示要将库装载到内存，准备使用
    printf("**************libtest1so***************\n");
    long time1 = GetUSTime( );
    lib_handle = dlopen("libtest1so.so", RTLD_LAZY);
    if (!lib_handle) {
        fprintf(stderr, "%s\n", dlerror( ));
        return 1;
    }
    long time2 = GetUSTime( );
    printf("Load Time: %ld us\n", time2 - time1);
    ///////////////////////////////////////////////////
    //获得指定函数(symbol)在内存中的位置(指针)
    time1 = GetUSTime( );
    sum = (int (*)(int, int)) (dlsym(lib_handle, "sum"));
    if ((error = dlerror( )) != NULL) {
        fprintf(stderr, "%sn", error);
        return 1;
    }

    sub = (int (*)(int, int)) (dlsym(lib_handle, "sub"));
    if ((error = dlerror( )) != NULL) {
        fprintf(stderr, "%s\n", error);
        return 1;
    }
    time2 = GetUSTime( );
    printf("Dlsym Time: %ld us\n", time2 - time1);
    ////////////////////////////////////////////////////

    printf("sum:0x%x\n", sum);
    printf("sub:0x%x\n", sub);
    printf("a - b = %d\n", sum(a, b));
    printf("a + b = %d\n", sub(a, b));
    void *oldHandle = lib_handle;
    //表示要将库装载到内存，准备使用
    int (*sum1)(int, int);
    int (*sub1)(int, int);
    printf("**************libtest2so***************\n");
    time1 = GetUSTime( );
    lib_handle = dlopen("libtest2so.so", RTLD_LAZY);
    if (!lib_handle) {
        fprintf(stderr, "%s\n", dlerror( ));
        return 1;
    }
    time2 = GetUSTime( );
    printf("Load Time: %ld us\n", time2 - time1);
    /////////////////////////////////////////////////////////
    //获得指定函数(symbol)在内存中的位置(指针)
    time1 = GetUSTime( );
    sum1 = (int (*)(int, int)) (dlsym(lib_handle, "sum"));
    if ((error = dlerror( )) != NULL) {
        fprintf(stderr, "%sn", error);
        return 1;
    }

    sub1 = (int (*)(int, int)) (dlsym(lib_handle, "sub"));
    if ((error = dlerror( )) != NULL) {
        fprintf(stderr, "%s\n", error);
        return 1;
    }
    time2 = GetUSTime( );
    printf("Dlsym Time: %ld us\n", time2 - time1);
    //////////////////////////////////////////////////
    dlclose(oldHandle);
    time1 = GetUSTime( );
    sum = sum1;
    sub = sub1;
    time2 = GetUSTime( );
    printf("Change Time: %ld us\n", time2 - time1);
    printf("sum:0x%x\n", sum);
    printf("sub:0x%x\n", sub);
    printf("a - b = %d\n", sum(a, b));
    printf("a + b = %d\n", sub(a, b));
    dlclose(lib_handle);
    return 0;
}

struct Demo
{
    int a;
    int b;
};

class Test
{
public:
    Test()
    {
        map = std::move(std::unordered_map<long, Demo>(10000));
    }
public:
    std::unordered_map<long, Demo> map;
};
#endif //SERVER_TEST_H
