#!/bin/sh
# author BokzBCheung
# date 2018-6-14 17:06:36

# show the capacity of the /dev/vda1 
df -h|awk '/vda1/ {print $5}' > capacity18.txt

capacity=$(cat capacity18.txt)

# write the info to file
echo "your machine 18's capacity is $capacity !" >capacity18.txt

# write the machine to file
df -h > capacityofmachine18.txt

# make a judgement
if [ ${capacity%?} -ge 80 ];then
echo "your machine 18's available capacity is more than  80%,System will clear the logs file,Please attention check！" >> capacity18.txt

# clear the logs file
echo "=============================clear logs file start=============================" >> capacity18.txt
echo "clear mecury_b's file..." >> capacity18.txt
rm -rf /home/mecury_b/apache-tomcat-8.0.47/logs/*.txt
rm -rf /home/mecury_b/apache-tomcat-8.0.47/logs/catalina.out
rm -rf /home/mecury_b/apache-tomcat-8.0.47/logs/bussiness
echo "complete mecury_b's file" >> capacity18.txt

echo "clear mecury_c's file..." >> capacity18.txt
rm -rf /home/mecury_c/apache-tomcat-8.0.47/logs/*.txt
rm -rf /home/mecury_c/apache-tomcat-8.0.47/logs/catalina.out
rm -rf /home/mecury_c/apache-tomcat-8.0.47/logs/billingCounter
echo "complete mecury_c's file" >> capacity18.txt

echo "clear mecury_p's file..." >> capacity18.txt
rm -rf /home/mecury_p/apache-tomcat-8.0.47/logs/*.txt
rm -rf /home/mecury_p/apache-tomcat-8.0.47/logs/catalina.out
rm -rf /home/mecury_p/apache-tomcat-8.0.47/logs/ioCounter
echo "complete mecury_p's file" >> capacity18.txt

echo "clear mecury_r's file..." >> capacity18.txt
rm -rf /home/mecury_r/apache-tomcat-8.0.47/logs/*.txt
rm -rf /home/mecury_r/apache-tomcat-8.0.47/logs/catalina.out
rm -rf /home/mecury_r/apache-tomcat-8.0.47/logs/saasmercury
echo "complete mecury_r' file" >> capacity18.txt
echo "==============================clear logs file end==============================" >> capacity18.txt

else
echo "your machine 18's available capacity is enough!" >> capacity18.txt
fi
