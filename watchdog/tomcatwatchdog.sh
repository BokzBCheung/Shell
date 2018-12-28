#!/bin/bash
#遍历catalina.sh文件查找tomcat项目
for i in `find /data* -name catalina.sh`;do
    #h为tomcat的home目录
    h=${i%bin/catalina.sh}
    #当h!=i时认为是tomcat项目
    if [ "$h" != "$i" ];then
        #获取用户
        u=`ls -l $i|awk '{print $3}'`
        g=`ls -l $i|awk '{print $4}'`
        #获取进程
        pid=`su - $u -c 'jps|grep Bootstrap|awk "{print $1}"'`
        #获取用户名目录
        home=`su - $u -c 'echo $HOME'`
        #如果进程存在
        if [ a"$pid" != a ];then
             mkdir $home/shells
             echo 'source ~/.bash_profile' > $home/shells/watchdog.sh
             echo 'source ~/.bashrc' >> $home/shells/watchdog.sh
             echo 'date +"%F %T"' >> $home/shells/watchdog.sh
             echo 'which jps;if [ $? -ne 0 ];then exit;fi' >> $home/shells/watchdog.sh
             echo 'user=`whoami`' >> $home/shells/watchdog.sh
             echo 'flag=0' >> $home/shells/watchdog.sh
             echo 'c=$(find ~ -name  localhost_access_log.`date +%F`.txt)' >> $home/shells/watchdog.sh
             echo 'h=${c%logs/localhost_access_log.`date +%F`.txt}' >> $home/shells/watchdog.sh
             echo 'n=$(find ~ -name localhost_access_log.`date +%F`.txt -mmin -30|wc -l)' >> $home/shells/watchdog.sh
             echo 'if [ $n -eq 1 ];then' >> $home/shells/watchdog.sh
             echo '    s=0' >> $home/shells/watchdog.sh
             echo '    for i in `jps|awk '"'"'{print $1}'"'"'`;do' >> $home/shells/watchdog.sh
             echo '       s=1' >> $home/shells/watchdog.sh
             echo '       m=`top -bn 1 -p $i|grep java|grep -v grep |wc -l`' >> $home/shells/watchdog.sh
             echo '       if [ "$m" -gt 0 ];then' >> $home/shells/watchdog.sh
             echo '          flag=1' >> $home/shells/watchdog.sh
             echo '       fi' >> $home/shells/watchdog.sh
             echo '    done' >> $home/shells/watchdog.sh
             echo '    if [ "$s" -eq 0 ];then exit;fi' >> $home/shells/watchdog.sh
             echo '    if [ $flag -eq 0 ];then' >> $home/shells/watchdog.sh
             echo '       killall -9 java' >> $home/shells/watchdog.sh
             echo '       sleep 0.1' >> $home/shells/watchdog.sh
             echo '       cd $h/bin' >> $home/shells/watchdog.sh
             echo '       ./catalina.sh start ' >> $home/shells/watchdog.sh
             echo '    fi' >> $home/shells/watchdog.sh
             echo 'fi' >> $home/shells/watchdog.sh
             chown -R $u.$g $home/shells/
             touch /var/spool/cron/$u
             if [ `grep -c watchdog.sh /var/spool/cron/$u` -eq 0 ];then echo "*/5 * * * * cd;sh shells/watchdog.sh >> shells/watchdog.log 2>&1" >>/var/spool/cron/$u;fi 
        fi
    fi
done
