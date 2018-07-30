#!/bin/sh
# author BokzBCheung
# date 2018-6-15 14:23:02

# remotelogin the 18 machine
################################################################################################################################
ssh root@10.10.10.18 << remotessh

# run the script of the remote machine
"/root/FileWatchdog.sh"
exit
remotessh
###############################################################################################################################

# remotelogin the 19 machine
###############################################################################################################################
ssh root@10.10.10.19 << remotessh

#run the script of the remote machine
"/root/FileWatchdog.sh"
exit
remotessh
###############################################################################################################################

# scp the remote file to local machine
scp root@10.10.10.18:/root/\{capacity18.txt,capacityofmachine18.txt\} /root/txtOfSSH
scp root@10.10.10.19:/root/\{capacity19.txt,capacityofmachine19.txt\} /root/txtOfSSH

# construct a summary file
cat /root/txtOfSSH/capacity18.txt > capacity.txt
cat /root/txtOfSSH/capacity19.txt >> capacity.txt
echo "10.10.10.18 Machine disk running status:" > capacityofmachine.txt
cat /root/txtOfSSH/capacityofmachine18.txt >> capacityofmachine.txt
echo "10.10.10.19 Machine disk running status:" >> capacityofmachine.txt
cat /root/txtOfSSH/capacityofmachine19.txt >> capacityofmachine.txt

# mail to admin show the machine status
cat /root/capacity.txt|mail -s "世茂服务器--18,19机器(/dev/vda1)磁盘信息" -a /root/capacityofmachine.txt zb@reformer.com.cn
