#!/bin/sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

rm *.pb.h *.pb.cc 1>/dev/null 2>&1
cd ./proto/
for atomic_int1 in *.proto
do
	protoc -I=. --cpp_out=. $atomic_int1
	if [ "$?" -eq 0 ]
	then
		echo "已生成$atomic_int1.pb.h $atomic_int1.pb.cc"
	else
		exit 1
	fi
done

mv ./*.pb.* ../
