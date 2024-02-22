#include <string.h>
#include <chrono>
#include "base.h"
#include <stdio.h>

using namespace std;

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

Tokens StrSplit(const std::string &src, const std::string &sep)
{
	Tokens r;
	std::string s;
	for (std::string::const_iterator i = src.begin(); i != src.end(); i++) {
		if (sep.find(*i) != std::string::npos) {
			if (s.length()) r.push_back(s);
			s = "";
		}
		else {
			s += *i;
		}
	}
	if (s.length()) r.push_back(s);
	return r;
}

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