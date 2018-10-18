#-*- coding: utf-8 -*-
##################################
#   BJNetServ 客户端
#
#   创建时间: 2016-04-08
##################################

import httplib, json, socket

#######################################
#   连接到 BJNetServ
#######################################
def bjnet_open(port):
    conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    conn.settimeout(5)
    try:
        conn.connect(("127.0.0.1", port))
    except:
        return None
    return conn

#######################################
#   关闭 BJNetServ 连接
#######################################
def bjnet_close(conn):
    try:
        conn.close()
    except:
        return False
    return True

#######################################
#   关闭 BJNetServ 服务器
#######################################
def bjnet_exit(conn):
    try:
        cmd = bytearray(1)
        cmd[0] = 0x00
        conn.sendall(cmd)
    except:
        return False
    return True

#######################################
#   BJNetComm 重置
#######################################
def bjnet_reset(conn):
    try:
        cmd = bytearray(1)
        cmd[0] = 0x01
        conn.sendall(cmd)
    except:
        return False
    return True

#######################################
#   BJNetComm 最大数据包
#######################################
def bjnet_sizeMax(conn, size_kb):
    try:
        cmd = bytearray(5)
        cmd[0] = 0x02
        cmd[1] = (size_kb >> 24) & 0xFF
        cmd[2] = (size_kb >> 16) & 0xFF
        cmd[3] = (size_kb >> 8) & 0xFF
        cmd[4] = (size_kb & 0xFF)
        conn.sendall(cmd)
    except:
        return False
    return True

#######################################
#   BJNetComm 更新令牌
#######################################
def bjnet_updateToken(conn, token):
    if token == None or token == "":
        return bjnet_reset(conn)
    try:
        size = len(token)
        if size > 255:
            size = 255
            token = token[0:255]
        cmd = bytearray(2)
        cmd[0] = 0x03
        cmd[1] = size & 0xFF
        conn.sendall(cmd)
        conn.sendall(token)
    except:
        return False
    return True

#######################################
#   BJNetComm 更新加密
#######################################
def bjnet_updateType(conn, type):
    try:
        cmd = bytearray(2)
        cmd[0] = 0x04
        cmd[1] = type & 0xFF
        conn.sendall(cmd)
    except:
        return False
    return True

#######################################
#   BJNetComm 读取数据
#######################################
def bjnet_socket_recv(conn, size):
    idx = 0
    ret = ""
    while idx < size:
        data = conn.recv(size)
        back = len(data)
        if back == 0:
            break
        idx += back
        ret += data
    return (ret, idx)

#######################################
#   BJNetComm 加密数据 & 解密数据
#######################################
def bjnet_do_crypt(conn, text, is_enc):
    try:
        size = len(text)
        if size == 0 or size >= 1024 * 1024:
            return None
        cmd = bytearray(5)
        if is_enc:
            cmd[0] = 0x05
        else:
            cmd[0] = 0x06
        cmd[1] = (size >> 24) & 0xFF
        cmd[2] = (size >> 16) & 0xFF
        cmd[3] = (size >> 8) & 0xFF
        cmd[4] = (size & 0xFF)
        conn.sendall(cmd)
        conn.sendall(text)
        leng = conn.recv(4)
        if len(leng) != 4:
            return None
        size  = ord(leng[0])
        size *= 256
        size += ord(leng[1])
        size *= 256
        size += ord(leng[2])
        size *= 256
        size += ord(leng[3])
        back = bjnet_socket_recv(conn, size)
        if back[1] != size:
            return None
        if back[0] == "error":
            return None
    except:
        return None
    return back[0]

#######################################
#   BJNetComm 设置用户字符串
#######################################
def bjnet_set_user(conn, ustr):
    try:
        if ustr == None:
            size = 1
        else:
            size = len(ustr)
        cmd = bytearray(5)
        cmd[0] = 0x07
        cmd[1] = (size >> 24) & 0xFF
        cmd[2] = (size >> 16) & 0xFF
        cmd[3] = (size >> 8) & 0xFF
        cmd[4] = (size & 0xFF)
        conn.sendall(cmd)
        if ustr == None:
            conn.sendall("\0")
        else:
            conn.sendall(ustr)
    except:
        return False
    return True

#######################################
#   BJNetComm 加密数据
#######################################
def bjnet_enc(conn, text):
    return bjnet_do_crypt(conn, text, True)

#######################################
#   BJNetComm 解密数据
#######################################
def bjnet_dec(conn, text):
    return bjnet_do_crypt(conn, text, False)

#######################################
#   BJNetComm 处理返回的 JSON
#######################################
def bjnet_do_json(conn, txts):
    try:
        objs = json.loads(txts)
    except:
        return (None, txts)
    if (conn == None) or ("type" not in objs.keys()) or \
                         ("data" not in objs.keys()) or \
                         ("sign" not in objs.keys()) or \
                         ("tick" not in objs.keys()):
        return (objs, txts)
    txtd = bjnet_dec(conn, txts)
    if txtd != None:
        try:
            objs = json.loads(txtd)
        except:
            return (None, txtd)
        return (objs, txtd)
    return (objs, txts)

#######################################
#   BJNetComm HTTP GET
#######################################
def bjnet_http_get(conn, host, url, timeout):
    http = None
    headers = { "Content-type":"charset=utf-8", "Accept":"application/json" }
    try:
        http = httplib.HTTPConnection(host, timeout=timeout)
        http.request("GET", url, headers=headers)
        result = http.getresponse()
        status = result.status
        reason = result.reason
        back = bjnet_do_json(conn, result.read())
        http.close()
        http = None
    except:
        if http != None:
            http.close()
        return None
    return (back[0], back[1], status, reason)

#######################################
#   BJNetComm HTTP POST
#######################################
def bjnet_http_post(conn, host, url, body, is_enc, timeout):
    http = None
    headers = { "Content-type":"application/json;charset=utf-8", "Accept":"application/json" }
    try:
        if type(body) != type("str"):
            body = json.dumps(body)
        if is_enc or (conn == None):
            body = bjnet_enc(conn, body)
            if body == None:
                return None
        http = httplib.HTTPConnection(host, timeout=timeout)
        http.request("POST", url, body, headers)
        result = http.getresponse()
        status = result.status
        reason = result.reason
        back = bjnet_do_json(conn, result.read())
        http.close()
        http = None
    except:
        if http != None:
            http.close()
        return None
    return (back[0], back[1], status, reason)
