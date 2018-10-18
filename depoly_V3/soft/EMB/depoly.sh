#!/bin/sh
#tomcat version. end of tar dir name
TOMCAT_NAME="apache-tomcat-8.5.11"

#host to user to app mapping config path
H2U2A_PATH="user.csv"

#default user password
DEFAULT_PASSWD="test123"

#remote root password 
ROOT_PASSWD="1234"

# user home dir
HOME_DIR=/data

#package path 
#store .war config 
PACKAGE_PATH=./

#default tomcat port
#no need to modify
DEFAULT_TOMCAT_PORT=("8080" "8005" "8009" )

## run model (init ,or other ,todo more)
model=$1

# $1 host line
# init server's host name
# bulid hosts file to /tmp/hosts
function init_host_name()
{
	host=($1)
	hosts=(${host//:/ })
	if [ ${#hosts[@]} -gt 1 ] ; then
		echo "echo \"${hosts[0]}	${hosts[1]}\">>/tmp/_tmp_hosts.csv" | sh
	fi
}

# $1 host line
# scp hosts file to remote server
function scp_hosts_file()
{
	echo "echo \"127.0.0.1	localhost\" >>/tmp/_tmp_hosts.csv" | sh
	echo "sort /tmp/_tmp_hosts.csv | uniq > /tmp/tmp_hosts.csv" | sh
	cat $H2U2A_PATH | while read LINE
	do
		if [ -n "$LINE" ] ; then 
			if [ "$model" = "init" ];then
				host=($LINE)
				hosts=(${host//:/ })
				if [ ${#hosts[@]} -gt 1 ] ; then
					scp -r /tmp/tmp_hosts.csv root@${hosts[0]}:/etc/hosts
					echo ssh root@${hosts[0]} "echo \"${hosts[1]}\" > /etc/sysconfig/network" | sh
					echo ssh root@${hosts[0]} "hostname ${hosts[1]}" | sh
				fi
			fi
		fi
    done
	# delete local hosts file 
	echo "rm -rf /tmp/_tmp_hosts.csv /tmp/tmp_hosts.csv" | sh
}



## $1 host line
# init remote server 
# adduser change password 
function init_remote_user()
{
    host=($1)
	hosts=(${host//:/ })
    for apps in ${host[@]:1}
    do    
		_app=(${apps//:/ })
		echo ssh root@${hosts[0]} "userdel -f ${_app[0]}" | sh
		# warning! delete user old dir. be sure that dir has no important data. or commented out this line
		echo ssh root@${hosts[0]} "rm -rf $HOME_DIR/${_app[0]}" | sh
		echo ssh root@${hosts[0]} "useradd -d $HOME_DIR/${_app[0]} -m ${_app[0]}" | sh
		## build tmp user:passwd file for create user
		echo ssh root@${hosts[0]} "echo \"${_app[0]}:$DEFAULT_PASSWD\">>/tmp/_users.csv" | sh
    done
	
    echo ssh root@${hosts[0]} "sort /tmp/_users.csv | uniq > /tmp/users_end.csv" | sh
    echo ssh root@${hosts[0]} "chpasswd < /tmp/users_end.csv" | sh
    echo ssh root@${hosts[0]} "rm -rf /tmp/_users.csv /tmp/users_end.csv" | sh
}

# $1 ip 
# $2 user
# $3 password
##remote server secret login
function secret_login()
{
    ip=$1
    user=$2
	password=$3
	
    expect -c "set timeout -1;
        spawn ssh-copy-id $user@$ip;
        expect {
            *(yes/no)* {send -- yes\r;exp_continue;}
            *assword:* {send -- $password\r;exp_continue;}
            eof        {exit 0;}
        }";
}

## $1 host line
#init remote server root secret 
function init_root_secret_login()
{
	host=($1)
	hosts=(${host//:/ })
	secret_login ${hosts[0]} "root" $ROOT_PASSWD
}

## $1 host line
#init remote server user secret 
function init_user_secret_login()
{
	host=($1)
	hosts=(${host//:/ })
    for apps in ${host[@]:1}
    do    
		_app=(${apps//:/ })
		echo "host :" ${hosts[0]} ${_app[0]} $DEFAULT_PASSWD
		secret_login ${hosts[0]} ${_app[0]} $DEFAULT_PASSWD
    done
}


## $1 host line
## install soft common dependency
function install_common()
{
    user="root"
	host=($1)
	hosts=(${host//:/ })
	
    echo ssh $user@${hosts[0]} "yum install -y java" | sh
    echo ssh $user@${hosts[0]} "yum install -y epel-release" | sh
    echo ssh $user@${hosts[0]} "yum install -y redis" | sh
	
	#scp saas config to remote server
	echo ssh $user@${hosts[0]} "mkdir -p /etc/rf.saas/" | sh
	scp -r $PACKAGE_PATH/conf/rf.saas/* root@${hosts[0]}:/etc/rf.saas/.
	
	#scp redis config to remote server
	scp -r $PACKAGE_PATH/conf/redis/* root@${hosts[0]}:/etc/.
	# chmod config to 777 
	echo ssh $user@${hosts[0]} "chmod 777 -R /etc/redis.conf /etc/rf.saas" | sh
}

## $1 host line
## install soft under user  like tomcat
function install_user()
{
	host=($1)
	hosts=(${host//:/ })
    for apps in ${host[@]:1}
    do    
		_app=(${apps//:/ })
		# remove old tomcat.  avoid deleting temporary cancellation
		#ssh ${users[0]}@${_app[0]} "rf -rf ./$TOMCAT_NAME"
		
		#scp tomcat to remote server
		scp -r $PACKAGE_PATH/soft/$TOMCAT_NAME ${_app[0]}@${hosts[0]}:./.
    done
}
# $1 host line
# modify tomcat lisenter port 
function modify_tomcat_port()
{
	host=($1)
	hosts=(${host//:/ })
    for apps in ${host[@]:1}
    do    
		_app=(${apps//:/ })
		if [ ${#_app} > 2 ]; then
			for((i=2;i<${#_app[@]}&&i<${#DEFAULT_TOMCAT_PORT[@]}+2;i++));do
				echo ssh ${_app[0]}@${hosts[0]} "sed -i \"s/${DEFAULT_TOMCAT_PORT[i-2]}/${_app[$i]}/g\" ./$TOMCAT_NAME/conf/server.xml" | sh
			done
		fi
    done
}

# $1 host line
# install user defind soft 
# like gcc EMB 
function install_user_def(){
	host=($1)
	hosts=(${host//:/ })
    for apps in ${host[@]:1}
    do    
		_app=(${apps//:/ })
		#scp tomcat to remote server
		if [ ${_app[1]} = "BPParkCompute.war" ] ;then
			scp -r $PACKAGE_PATH/package/javaScript ${_app[0]}@${hosts[0]}:./$TOMCAT_NAME/.
		elif [ ${_app[1]} = "BPParkManager.war" ]; then
			echo ssh root@${hosts[0]} "yum install -y gcc" | sh
			scp -r $PACKAGE_PATH/soft/EMB ${_app[0]}@${hosts[0]}:./.
			echo ssh root@${hosts[0]} "sh ~/EMB/install.sh $ROOT_PASSWD" | sh
		elif [ ${_app[1]} = "PortalParking.war" ]; then
			echo ssh root@${hosts[0]} "yum install -y gcc" | sh
			scp -r $PACKAGE_PATH/soft/EMB ${_app[0]}@${hosts[0]}:./.
			echo ssh root@${hosts[0]} "sh ~/EMB/install.sh $ROOT_PASSWD" | sh
		fi 
		scp -r $PACKAGE_PATH/package/${_app[1]} ${_app[0]}@${hosts[0]}:./$TOMCAT_NAME/webapps/.
    done
}

## $1 host line 
## publish application to server
function publish_apps()
{
	host=($1)
	hosts=(${host//:/ })
    for apps in ${host[@]:1}
    do    
		_app=(${apps//:/ })
		scp -r $PACKAGE_PATH/package/${_app[1]} ${_app[0]}@${hosts[0]}:./$TOMCAT_NAME/webapps/.
    done
}

## read host2user2app mapping config
## then check execute model (init or update)
function readConf ()
{
    cat $H2U2A_PATH | while read LINE
	do
		if [ -n "$LINE" ] ; then 
			if [ "$model" = "init" ];then
					init_root_secret_login "$LINE"
					init_remote_user "$LINE"
					init_user_secret_login "$LINE"
					install_common "$LINE"
					install_user "$LINE"
					modify_tomcat_port "$LINE"
					init_host_name "$LINE"
			fi
			publish_apps "$LINE"
		fi
    done
	
	scp_hosts_file
	
}

# init 
# install and config dependency
function main()
{
	if [ "$model" = "init" ];then
		yum install -y expect
		expect -c "set timeout -1;
			spawn ssh-keygen -t rsa;
			expect {
				*(/* {send -- \r;exp_continue;}
				*(y/n)* {send -- y\r;exp_continue;}
				*(* {send -- \r;exp_continue;}
				*a* {send -- \r;exp_continue;}
				eof	{exit 0;}
			}";
	fi
	
	readConf
}

#         +-----------------------------------------------------------+
#         |depoly                                     			      |
#         +-----------------------------------------------------------+
# 
#         +-----------------------+        +--------------------+
#         |init local denpendence |        |install_user        |
#         +-----------------------+        |-- install user soft|
#                                          |include tomcat      |
#         +----------------------------+   +--------------------+
#         |init_root_secret_login      |
#         |--remote server secret_login|  +-----------------------+
#         +----------------------------+  |modify_tomcat_port     |
#                                         |-- modify tomcat port  |
#         +----------------------------+  |8080 -> use def por    |
#         |init_remote_user            |  +-----------------------+
#         |-- crete remote server users|
#         +----------------------------+
#         +---------------------------+    +----------------------+
#         |init_user_secret_login     |    |publish_apps          |
#         |-- init remote server      |    |-- publish app to user|
#         | users secret_login        |    +----------------------+
#         +---------------------------+
#         +-----------------------+
#         |install_common         |
#         |-- install comm soft   |
#         | include redis java    |
#         | common config         |
#         +-----------------------+

main
