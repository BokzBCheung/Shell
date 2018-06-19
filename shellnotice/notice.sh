#/bin/bash

size1=`du -m /home/nginx/nginx01/logs/error.log | awk '{print $1}'`
size2=`du -m /home/nginx/nginx01/logs/access.log | awk '{print $1}'`
if [ $size1 -gt 1024 -o $size2 -gt 2048 ] ;then
echo "您的代理服务器错误日志已经超过1GB或者日志文件超过2GB，请注意检查机器运行状况，确认无误请忽略！"|mail -s "上海世贸nginx代理服务器错误报警" zb@reformer.com.cn
else
exit 0
fi

#step=1
#for((i=0;i<60;i=(i+step)));do
#  echo '定时任务发送邮件------------------by bokzbcheung'|mail -s '定时任务' -a hellolinux.txt zb@reformer.com.cn
#  sleep $step
#done
#exit 0
