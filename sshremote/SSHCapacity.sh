#!/bin/sh
# author BokzBCheung
# date 2018-6-15 14:23:02

# remotelogin the 18 machine
ssh root@10.10.10.18 << remotessh

# run the script of the remote machine
"/root/FileWatchdog.sh"
exit
remotessh

# scp the remote file to local machine
scp root@10.10.10.18:/root/\{capacity.txt,capacityofmachine.txt\} /root/

# mail to admin show the machine status
cat capacity.txt|mail -s "世茂服务器--机器(/dev/vda1)磁盘信息" -a /root/capacityofmachine.txt zb@reformer.com.cn
