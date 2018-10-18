#-*- coding: utf-8 -*-
##################################
#   TDF 文件相关实现
#
#   创建时间: 2016-04-11
##################################

import hashlib, os

#######################################
#   加载一个文件
#######################################
def file_load(name):
    try:
        fp = open(name, "rb")
    except:
        return None
    try:
        data = fp.read()
    except:
        return None
    finally:
        fp.close()
    return data

#######################################
#   保存一个文件
#######################################
def file_save(name, data):
    try:
        fp = open(name, "wb")
    except:
        return False
    try:
        fp.write(data)
    except:
        fp.close()
        os.remove(name)
        return False
    fp.close()
    return True

#######################################
#   加载一个文件 (带 MD5 校验)
#######################################
def file_load_md5(name):
    data = file_load(name)
    if data == None:
        return None
    size = len(data)
    if size <= 16:
        return None
    size -= 16
    hash = data[-16:]
    mm = hashlib.md5()
    result = data[0:size]
    mm.update(result)
    if hash != mm.digest():
        return None
    return result

#######################################
#   保存一个文件 (带 MD5 校验)
#######################################
def file_save_md5(name, data):
    try:
        fp = open(name, "wb")
    except:
        return False
    mm = hashlib.md5()
    mm.update(data)
    hash = mm.digest()
    try:
        fp.write(data)
        fp.write(hash)
    except:
        fp.close()
        os.remove(name)
        return False
    fp.close()
    return True

#######################################
#   生成以太网配置脚本
#######################################
def file_eth_conf(name, config):
    if ("IP" not in config.keys()) or ("MASK" not in config.keys()):
        return False
    try:
        fp = open(name + ".sh", "w")
    except:
        return False
    try:
        fp.write("#!/bin/sh\n")
        fp.write("ifconfig " + name + " down\n")
        fp.write("ifconfig " + name + " " + config["IP"] + " netmask " + config["MASK"] + "\n")
        if "GATEWAY" in config.keys():
            fp.write("route add default gw " + config["GATEWAY"] + "\n")
        fp.write("ifconfig " + name + " up\n")
    except:
        fp.close()
        os.remove(name)
        return False
    fp.close()
    return True
