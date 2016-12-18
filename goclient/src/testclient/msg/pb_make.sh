#	protoc --go_out=. 
protoc -I. --go_out=. message.proto proxymessage.proto webmessage.proto
#!/bin/sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

rm *.pb.go -f
for i in *.proto
do
	protoc -I=. --go_out=. $i
	if [ "$?" -eq 0 ]
	then
		echo "已生成$i.pb.go"
	else
		exit 1
	fi
done
