#include <string.h>
#include <chrono>
#include "base.h"
#include <stdio.h>

using namespace std;

// 通过unix时间戳获取是当年的第几天
int GetDayOfYear(time_t tTime)
{
	struct tm tm;
	tm = *localtime(&tTime);
	return tm.tm_yday;
}

// 判断两个时间戳是否是同一天
bool IsSameDay(time_t tFirstTime, time_t tSecondTime)
{
	struct tm tmFirst;
	struct tm tmSecond;
	tmFirst = *localtime(&tFirstTime);
	tmSecond = *localtime(&tSecondTime);

	if (tmFirst.tm_year == tmSecond.tm_year
		&& tmFirst.tm_mon == tmSecond.tm_mon
		&& tmFirst.tm_mday == tmSecond.tm_mday)
		return true;

	return false;
}

// 获取当前秒数
int GetSecondTime()
{
	std::chrono::time_point<std::chrono::system_clock> tmTime = std::chrono::system_clock::now();
	return tmTime.time_since_epoch().count() / 1000 / 1000;
}

// 获取当前毫秒数
time_t GetMSTime()
{
	std::chrono::time_point<std::chrono::system_clock> tmTime = std::chrono::system_clock::now();
	return tmTime.time_since_epoch().count() / 1000;
}

// 获取当前微秒
time_t GetUSTime()
{
	std::chrono::time_point<std::chrono::system_clock> tmTime = std::chrono::system_clock::now();
	return tmTime.time_since_epoch().count();
}

// 分割字符串，获取单词
// 拷贝字符串里面的单词（以空格回车换行等隔开）到pb里面
void TrimStr(char *strInput)
{
	char *pb;
	char *pe;
	int iTempLength;
	if (strInput == NULL) {
		return;
	}

	iTempLength = strlen(strInput);

	if (iTempLength == 0) {
		return;
	}

	pb = strInput;

	while (((*pb == ' ') || (*pb == '\t') || (*pb == '\n') || (*pb == '\r')) && (*pb != 0)) {
		pb++;  // 当遇到控制字符的时候字符串指针++
	}

	pe = &strInput[iTempLength - 1];  // pe指针指向strInput的最后一个字符

	while ((pe >= pb) && ((*pe == ' ') || (*pe == '\t') || (*pe == '\n') || (*pe == '\r'))) {
		pe--;
	}
	*(pe + 1) = '\0';

	strcpy(strInput, pb);

	return;

}

// int SockAddrToString(sockaddr_in *pstSockAddr, char *szResult)
// {
// 	char *pcTempIP = NULL;
// 	unsigned short nTempPort = 0;
// 
// 	if (!pstSockAddr || !szResult) {
// 		return -1;
// 	}
// 
// 	pcTempIP = inet_ntoa(pstSockAddr->sin_addr);
// 
// 	if (!pcTempIP) {
// 		return -1;
// 	}
// 
// 	nTempPort = ntohs(pstSockAddr->sin_port);
// 	//unsigned short nTempPort_hton = htons(pstSockAddr->sin_port);
// 
// 	sprintf(szResult, "%s:%d", pcTempIP, nTempPort);
// 	//sprintf(szResult, "%s", pstSockAddr->sa_data);
// 
// 	return 0;
// }
// 
// int SockAddrToString(unsigned int ip, unsigned short port, char *szResult)
// {
// 	if (!szResult) {
// 		return -1;
// 	}
// 
// 	struct in_addr addr;
// 	addr.s_addr = (in_addr_t) ip;
// 	char *pcTempIP = inet_ntoa(addr);
// 
// 	if (!pcTempIP) {
// 		return -1;
// 	}
// 
// 	sprintf(szResult, "%s:%d", pcTempIP, port);
// 
// 	return 0;
// }
// 
// // 获取两段时间的间隔
// int TimeValMinus(timeval &tvA, timeval &tvB, timeval &tvResult)
// {
// 	timeval tvTmp;
// 
// 	if (tvA.tv_usec < tvB.tv_usec) {
// 		tvTmp.tv_usec = (1000000 + tvA.tv_usec) - tvB.tv_usec;
// 		tvTmp.tv_sec = tvA.tv_sec - tvB.tv_sec - 1;
// 	}
// 	else {
// 		tvTmp.tv_usec = tvA.tv_usec - tvB.tv_usec;
// 		tvTmp.tv_sec = tvA.tv_sec - tvB.tv_sec;
// 	}
// 
// 	tvResult.tv_sec = tvTmp.tv_sec;
// 	tvResult.tv_usec = tvTmp.tv_usec;
// 
// 	return 0;
// }


void ignore_pipe()
{
// 	struct sigaction sig;
// 
// 	sig.sa_handler = SIG_IGN;
// 	sig.sa_flags = 0;
// 	sigemptyset(&sig.sa_mask);
// 	sigaddset(&sig.sa_mask, SIGPIPE);
// 	if (pthread_sigmask(SIG_BLOCK, &sig.sa_mask, NULL) == -1) {
// 		perror("SIG_PIPE");
// 	}
}