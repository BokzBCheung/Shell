#-*- coding: utf-8 -*-
##################################
#   TDF UNIX 相关实现
#
#   创建时间: 2016-04-11
##################################

import fcntl, time

#######################################
#   进程锁 (非阻塞)
#######################################
def app_lock_nb(name):
    pidfile = open(name, "w")
    try:
        fcntl.lockf(pidfile, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except IOError:
        return False
    return True

#######################################
#   进程锁 (阻塞)
#######################################
def app_lock(name, ts):
    while True:
        if app_lock_nb(name):
            break
        time.sleep(ts)
