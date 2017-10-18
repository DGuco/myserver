#!/bin/sh

user=$USER
curpwd=`pwd`
curpwd=${curpwd%/*}

target='proxysvrd'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	svrdpwd=`ls -l /proc/$pid/exe|awk '{print $11}'`
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	if [ $curpwd = $svrdpwd ] ; then
 		kill -9 $pid
	fi
done

target='gamesvrd'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	svrdpwd=`ls -l /proc/$pid/exe|awk '{print $11}'`
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	if [ $curpwd = $svrdpwd ] ; then
 		kill -9 $pid
	fi
done

target='offlinesvrd'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	svrdpwd=`ls -l /proc/$pid/exe|awk '{print $11}'`
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	if [ $curpwd = $svrdpwd ] ; then
 		kill -9 $pid
	fi
done

target='dbsvrd'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	svrdpwd=`ls -l /proc/$pid/exe|awk '{print $11}'`
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	echo $svrdpwd
	echo $curpwd
	if [ $curpwd = $svrdpwd ] ; then
 		kill -9 $pid
	fi
done

target='worldsvrd'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	svrdpwd=`ls -l /proc/$pid/exe|awk '{print $11}'`
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	if [ $curpwd = $svrdpwd ] ; then
 		kill -9 $pid
	fi
done

target='tcpsvrd'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	svrdpwd=`ls -l /proc/$pid/exe|awk '{print $11}'`
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	if [ $curpwd = $svrdpwd ] ; then
 		kill -9 $pid
	fi
done

target='websvrd'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	svrdpwd=`ls -l /proc/$pid/exe|awk '{print $11}'`
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	if [ $curpwd = $svrdpwd ] ; then
 		kill -9 $pid
	fi
done

target='offlinesvrd'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	svrdpwd=`ls -l /proc/$pid/exe|awk '{print $11}'`
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	if [ $curpwd = $svrdpwd ] ; then
 		kill -9 $pid
	fi
done

target='logsvrd'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	svrdpwd=`ls -l /proc/$pid/exe|awk '{print $11}'`
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	if [ $curpwd = $svrdpwd ] ; then
 		kill -9 $pid
	fi
done

sleep 1
echo "Delete share memory"
shmid=`ipcs -m | grep -w $user | awk '$6==0{printf " -m  " $2  " "}'`
echo "$shmid"
ipcrm $shmid

