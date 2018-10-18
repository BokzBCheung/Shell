#-*- coding: utf-8 -*-
##################################
#   BJAutoNet 客户端
#
#   创建时间: 2016-05-30
##################################

import socket
import TDFfile

#######################################
#   BJAN 组包
#######################################
def bjan_packet(cmd, dat):
    if dat != None:
        size = len(dat)
    else:
        size = 0
    buff = bytearray(7 + size)
    buff[0] = 0x42  # 'B'
    buff[1] = 0x4A  # 'J'
    buff[2] = 0x41  # 'A'
    buff[3] = 0x4E  # 'N'
    buff[4] = cmd
    buff[5] = size & 0xFF
    sum = 0x1B + buff[4] + buff[5]
    idx = 6
    size += 6
    if dat != None:
        while idx < size:
            buff[idx] = dat[idx - 6]
            sum += buff[idx]
            idx += 1
    buff[idx] = sum & 0xFF
    return buff

#######################################
#   发送 BJAN 包
#######################################
def bjan_send(conn, cmd, dat):
    try:
        buff = bjan_packet(cmd, dat)
        conn.sendall(buff)
    except:
        return False
    return True

#######################################
#   TCP 包处理
#######################################
def tcp_main(conn, bjsn):
    while True:
        while True:
            head = conn.recv(1)
            if len(head) != 1:
                return False
            if ord(head) == 0x42:
                break
        head = conn.recv(5)
        if len(head) != 5:
            return False
        if ord(head[0]) != 0x4A or \
           ord(head[1]) != 0x41 or \
           ord(head[2]) != 0x4E or \
           ord(head[3]) != 0x01 or \
           ord(head[4]) != 0x20:
            return False
        body = conn.recv(33)
        if len(body) != 33:
            return False
        sum = 0x1B + 0x21
        idx = 0
        while idx < 32:
            sum += ord(body[idx])
            idx += 1
        sum &= 0xFF
        if sum != ord(body[idx]):
            return False
        idx = 0
        while idx < 32:
            bjsn[idx] = body[idx]
            idx += 1
        if TDFfile.file_save_md5("BJSN.bin", bjsn):
            bjan_send(conn, 0x81, None)
        else:
            bjan_send(conn, 0x41, None)
    return True

#######################################
#   程序入口点
#######################################
if __name__ == "__main__":
    bjsn = TDFfile.file_load_md5("BJSN.bin")
    if bjsn == None or len(data) != 32:
        print "[BJAutoNet] Missing or bad BJSN.bin!"
        bjsn = bytearray(32)
    serv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serv.bind(("0.0.0.0", 8888))
    serv.listen(1)
    serv.settimeout(0.1)
    srv_quit = False
    while not srv_quit:
        try:
            conn, addr = serv.accept()
            print "[BJAutoNet] BJNodeSet in:", addr
        except:
            continue
        conn.settimeout(0.1)
        cli_quit = False
        while not cli_quit:
            try:
                if not tcp_main(conn, bjsn):
                    cli_quit = True
            except socket.timeout:
                pass
            except:
                cli_quit = True
        conn.close()
    serv.close()
