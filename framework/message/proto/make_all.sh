#!/bin/sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
for i in *.proto
do
	protoc -I=. --java_out=../Server/empire-parent/protocol/generated/main/java $i
	if [ "$?" -eq 0 ]
	then
		echo "已生成$i.java"
	else
		exit 1
	fi
done


