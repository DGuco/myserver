#!/bin/sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
filename=$1
if [ "$filename" = "" ];then
	echo "you have to input a proto file name"
	exit 1
else
	protoc -I=. --java_out=../Server/empire-parent/protocol/generated/main/java $filename
	echo "已生成$filename.java"
fi

