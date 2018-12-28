#!/bin/bash
cd ~
source $HOME/.bash_profile
a=`jps|grep QuorumPeerMain|wc -l`
if [ $a -ne 1 ];then
    sleep 10
    a=`jps|grep QuorumPeerMain|wc -l`
    if [ $a -ne 1 ];then
         cd /data/zookeeper/zookeeper/bin
         ./zkServer.sh start
    fi
fi
