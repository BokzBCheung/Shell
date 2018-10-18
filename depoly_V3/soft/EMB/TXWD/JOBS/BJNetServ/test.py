#-*- coding: utf-8 -*-
##################################
#   BJNetServ 测试用例
#
#   创建时间: 2016-04-08
##################################

import sys
import BJNetServ

conn = BJNetServ.bjnet_open(5050)
if conn == None:
    print "bjnet_open() failure!"
    sys.exit(-1)
test_str = "1234567890+abcdefghijklmnopqrstuvwxyz-ABCDEFGHIJKLMNOPQRSTUVWXYZ"\
           "1234567890+abcdefghijklmnopqrstuvwxyz-ABCDEFGHIJKLMNOPQRSTUVWXYZ"\
           "1234567890+abcdefghijklmnopqrstuvwxyz-ABCDEFGHIJKLMNOPQRSTUVWXYZ"\
           "1234567890+abcdefghijklmnopqrstuvwxyz-ABCDEFGHIJKLMNOPQRSTUVWXYZ"
BJNetServ.bjnet_sizeMax(conn, 1024)
BJNetServ.bjnet_set_user(conn, '"id":"123"')
idx = 0x00
while idx <= 0x05:
    if not BJNetServ.bjnet_updateType(conn, idx):
        idx += 1
        print "bjnet_updateType() failure!"
        continue
    str1 = BJNetServ.bjnet_enc(conn, test_str)
    if str1 == None:
        idx += 1
        print "bjnet_enc() failure!"
        continue
    str2 = BJNetServ.bjnet_dec(conn, str1)
    if str2 == None:
        idx += 1
        print "bjnet_dec() failure!"
        continue
    if str2 == test_str:
        print "%02x: OKAY" % idx
    else:
        print "%02x: FAIL" % idx
    idx += 1
if not BJNetServ.bjnet_updateToken(conn, test_str):
    print "bjnet_updateToken() failure!"
    sys.exit(-1)
BJNetServ.bjnet_set_user(conn, None)
idx = 0x20
while idx <= 0x25:
    if not BJNetServ.bjnet_updateType(conn, idx):
        idx += 1
        print "bjnet_updateType() failure!"
        continue
    str1 = BJNetServ.bjnet_enc(conn, test_str)
    if str1 == None:
        idx += 1
        print "bjnet_enc() failure!"
        continue
    str2 = BJNetServ.bjnet_dec(conn, str1)
    if str2 == None:
        idx += 1
        print "bjnet_dec() failure!"
        continue
    if str2 == test_str:
        print "%02x: OKAY" % idx
    else:
        print "%02x: FAIL" % idx
    idx += 1
BJNetServ.bjnet_exit(conn)
