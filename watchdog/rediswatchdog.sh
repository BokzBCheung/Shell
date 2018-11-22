source ~/.bash_profile
source ~/.bashrc 
#specify redis home dir
cd ~
t=~/redis-4.0.1
if `! echo |telnet 127.0.0.1 6379|grep -q 'Escape character'`;then
    sleep 10
    if `! echo |telnet 127.0.0.1 6379|grep -q 'Escape character'`;then
        echo "redis-server is not alive!!!"
  killall redis-server;sleep 10;killall redis-server;sleep 10;killall redis-server
  cd $t/src/
        nohup ./redis-server&
    fi
fi
