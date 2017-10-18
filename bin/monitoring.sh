#!/bin/bash
user=$USER
export TRITON_HOME=/home/kay/kaige/trunk

#服务正常,监控退出
(( `ps -ef| grep svrd |grep -w $USER| grep -v grep |wc -l ` == 6 )) && exit

#停服状态,不进行监控,退出
(( `ps -ef | grep svrd | grep -v grep |wc -l` == 0 )) && exit

#如果有其他monitoring.sh脚本在执行,不进行监控,退出
#((`ps -ef | grep  "monitoring.sh" | grep -v grep |wc -l` > 2 )) && exit
name=`basename $0`
pidFile=/tmp/${name/.sh/.pid}
if [ -f $pidFile ]
then
        pid=`cat $pidFile`
        pidPs=`ps -ef | grep $name |grep -v $$ | grep -v grep | awk '{print $2}'`
        if [[ $pid == $pidPs ]]
        then
                #echo running....
                exit
        else
                rm -f $pidFile
        fi
fi
echo $$>$pidFile

#如果有shutdown.sh|stop.sh|restart.sh脚本在执行,正在启服或停服,不进行监控,退出
ps -ef | grep -E "shutdown.sh|stop.sh|restart.sh|start.sh" | grep -vq grep && exit


#检查proxysvrd
#重启过程:
#1) 启动proxysvrd 
#2）等待1分钟
#3）shutdown.sh
#4）关闭所有进程
#5）正常启动所有服务器start.sh
checkProxysvrd()
{
        svrd="proxysvrd"
        if ! ps -ef | grep $svrd | grep -w $user | grep -vq grep
        then
                target='proxysvrd'
                cd $TRITON_HOME/proxyserver/bin
                ##1) 启动proxysvrd -D
                ./$target
echo "`date +"%F %T"` ...$target..."
                ##2）等待1分钟
                sleep 60
                cd $TRITON_HOME/bin
                ##3）shutdown.sh,4）关闭所有进程
                ./shutdown.sh
                ##5）正常启动所有服务器start.sh
                ./start.sh
                exit
        fi
}

#检查dbsvrd
#重启过程:
#1) 启动dbsvrd
#2）等待1分钟
#3）shutdown.sh
#4）关闭所有进程
#5）正常启动所有服务器start.sh
checkDBsvrd()
{
        svrd="dbsvrd"
        if ! ps -ef | grep $svrd | grep -w $user | grep -vq grep
        then
                target='dbsvrd'
                cd $TRITON_HOME/dbserver/bin
                ##1) 启动dbsvrd -D
                ./$target
echo "`date +"%F %T"` ...$target..."
                ##2）等待1分钟
                sleep 60
                cd $TRITON_HOME/bin
                ##3）shutdown.sh,4）关闭所有进程
                ./shutdown.sh
                ##5）正常启动所有服务器start.sh
                ./start.sh
                exit
        fi
}

#监控gamesvrd
#gamesvrd进程
#重启过程:
#1）gamesvrd -Resume
#2）等待gamesvrd自动退出
#3）启动gamesvrd 
checkGamesvrd()
{
	svrds="gameserver"
	if ! ps -ef | grep $svrd | grep -w $user | grep -vq grep
	then
		export PIPE_ID=1
		export SC_PIPE_ID=2
		export CS_PIPE_ID=3
		export SL_PIPE_ID=4
		target='gamesvrd -Resume'
		cd $TRITON_HOME/gameserver/bin
		##1）scenesvrd -Resume
		./$target
		echo "`date +"%F %T"` ...$target..."
		##2）等待gamesvrd自动退出
		wait
		##3）启动gamesvrd
		target='gamesvrd'
		./$target
		exit
	fi
}

#监控logsvrd,websvrd
#挂掉,直接重启  -D
checkSvrds()
{
	svrds="logsvrd websvrd"
	for svrd in $svrds
	do
		if ! ps -ef | grep $svrd | grep -w $user | grep -vq grep 
		then
			case $svrd in 
				logsvrd)
					export PIPE_ID=1
					export SC_PIPE_ID=2
					export CS_PIPE_ID=3
					export SL_PIPE_ID=4
					target='logsvrd'
					cd $TRITON_HOME/logserver/bin
					./$target
					echo "`date +"%F %T"` ...$target..."
					sleep 3
					;; 
				websvrd)
					target='websvrd'
					cd $TRITON_HOME/webserver/bin
					nohup ./$target &
					echo "`date +"%F %T"` ...$target..."
					sleep 3
					;; 
			esac
		fi
	done
}

checkProxysvrd
checkDBsvrd
checkSvrds
checkGamesvrd
rm -f $pidFile
