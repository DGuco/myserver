//
//  main.cpp
//  tcpserver 
//  Created by 杜国超 on 16/12/8.
//  Copyright © 2016年 杜国超. All rights reserved.
//

#include <iostream>
#include <signal.h>
#include <stdio.h>
#include "inc/tcpctrl.h"

using namespace std;

int main(int argc,char **argv)
{
    int iTmpRet;
    int iInitMode;

    //版本查询
    if (1 < argc && !strcasecmp(argv[1]),"-v")
    {
        char caTmp1[100] = {0};
        char caTmp2[100] = {0};      
        char caTmp3[100] = {0};
        snprintf(caTmp1,sizeof(caTmp1) - 1,"Epoll Model");

        #ifdef _DEBUG_
              snprintf(caTmp3,sizeof(caTmp1) - 1,"Debug");
        #else
              snprintf(caTmp3,sizeof(caTmp1) - 1,"Release");
        #endif
        printf("Tcpserver %s (support %d fd connect) %s %s build in %s %s\n",
                caTmp1, MAX_SOCKET_NUM, caTmp3, caTmp2, __DATE__, __TIME__);
        exit(0);
    }
}
