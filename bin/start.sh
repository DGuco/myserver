#!/bin/sh
user=$USER
sh stop.sh

export TRITON_HOME=/home/yaoqiang/marble
#svn up $TRITON_HOME/dbserver
#chmod +x $TRITON_HOME/dbserver/bin/dbsvrd
#svn up $TRITON_HOME/proxyserver
#chmod +x $TRITON_HOME/proxyserver/bin/proxysvrd
#svn up $TRITON_HOME/gameserver
#chmod +x $TRITON_HOME/gameserver/bin/gamesvrd
#svn up $TRITON_HOME/webserver
#chmod +x $TRITON_HOME/webserver/bin/websvrd

cd $TRITON_HOME/proxyserver/bin
target='proxysvrd'
rm ../log/* -f
./$target
sleep 1


cd $TRITON_HOME/dbserver/bin
target='dbsvrd'
rm ../log/* -f
./$target
sleep 1

#cd $TRITON_HOME/checkserver/bin
#target='checksvrd'
#rm ../log/* -f
#./$target
#sleep 1

#export PIPE_ID=4
#export SC_PIPE_ID=5
#export CS_PIPE_ID=6
#target='worldsvrd'
#echo $target
#cd $TRITON_HOME/worldserver/bin
#rm ../log/* -f
#./$target
#sleep 1


export PIPE_ID=1
export SC_PIPE_ID=2
export CS_PIPE_ID=3
export SL_PIPE_ID=4
target='gamesvrd'
echo $target
cd $TRITON_HOME/gameserver/bin
rm ../log/* -f
./tcpsvrd
sleep 1
./logsvrd
sleep 1
./$target
sleep 1


cd $TRITON_HOME/webserver/bin
target='websvrd'
rm ../log/*
nohup ./$target &
sleep 1

ipcs -m | grep $user
ps aux | grep $user | grep svrd | grep -v grep

