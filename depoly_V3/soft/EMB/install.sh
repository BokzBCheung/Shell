#!/bin/sh

ROOT_PASSWD=$1

# make emb so
# bjcomm.so holidays.so
function make_emb()
{
	cd ~/EMB/CrHack/mak
	echo "sh ./linux.sh"|sh

	cd ~/EMB/TXWD/JOBS/BJNetComm/
	echo "make"|sh

	cd ~/EMB/TXWD/JOBS/Holidays/
	echo "make"|sh
}

# export path to user
function export_env()
{
	cd ~
	path=$(pwd)

	sed -i '/LD_LIBRARY_PATH/d' ~/.bashrc
	echo "export LD_LIBRARY_PATH=$path/EMB/TXWD/JOBS/BJNetComm/" >> ~/.bashrc
}

# mv holidays.so to /var/lib
function mv_so_2_lib()
{
	expect -c "set timeout -1;
			spawn ssh root@localhost \"cp -r $path/EMB/TXWD/JOBS/Holidays/libHolidays.so /var/lib/.\";
			expect {
				*(yes/no)* {send -- yes\r;exp_continue;}
				*password* {send -- $ROOT_PASSWD\r;exp_continue;}
				eof	{exit 0;}
			}";

			
	expect -c "set timeout -1;
			spawn ssh root@localhost \"chmod 777 /var/lib/libHolidays.so\";
			expect {
				*(yes/no)* {send -- yes\r;exp_continue;}
				*password* {send -- $ROOT_PASSWD\r;exp_continue;}
				eof	{exit 0;}
			}";
}

# start script 
function start()
{
	make_emb
	export_env
	mv_so_2_lib
}

start