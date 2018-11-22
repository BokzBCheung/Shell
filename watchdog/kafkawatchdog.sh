#!/bin/bash
cd ~
source $HOME/.bash_profile
a=`jps|grep Kafka|wc -l`
if [ $a -ne 1 ];then
    sleep 10
    a=`jps|grep Kafka|wc -l`
    if [ $a -ne 1 ];then
        cd /home/kafka/kafka_2.11-1.0.0/bin
        nohup ./kafka-server-start.sh  ../config/server.properties & 
    fi
fi
