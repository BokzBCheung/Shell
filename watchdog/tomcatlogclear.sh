#!/bin/bash
date +'%F %T'
#磁盘使用率阀值75%
u=75
#日志内容head部分含有关键字
k="com.reformer. cn.rf. c.r."
#遍历超过阀值的分区
for i in `df|awk -F'[ %]+' '/\//&&$1!="tmpfs"&&$5>'$u'&&$NF!="/"{print $NF}'`;do
    echo $i
    #查找logs目录
    d=`find $i -name logs`
    if [ d"$d" != d ];then 
        #查找log文件
        f=`find $d -type f -mtime +1 -name "*log*"|xargs ls -tr`
        for j in $f;do
            #判断文件中是否含有关键字，如果有的话可以放心删除
            for m in `echo $k`;do
                t="$t"`head  $j|awk "/$m/{print;exit}"`
            done
            if [ t"$t" != t ];then
                echo "rm -f $j"
                rm -f $j
            fi
            #判断磁盘使用率已经降到阀值以内停止清理
            if [ `df $i|awk -F'[ %]+' '/\//&&$5>'$u'{print $NF}'|wc -l` -eq 0 ];then
                break
            fi
            sleep 0.1
        done
    fi
done
