/*
 *********************************
 *  BJNetComm 服务器
 *
 *  创建时间: 2016-04-08
 *********************************
 */

#include "crhack.h"
#include "util/bjnetcomm.h"

#include <stdio.h>

/* BJNetServ 导入库 */
#ifndef _CR_NO_PRAGMA_LIB_
    #pragma comment (lib, "COREs.lib")
    #pragma comment (lib, "UTILs.lib")
#endif

/* 全局的参数 */
static bool_t   s_quit = FALSE;

/* 配置的参数 */
#define BJNETSERV_OKAY          0
#define BJNETSERV_FAIL      __LINE__
#define BJNETSERV_TIMEOUT     5000
#define BJNETSERV_MAX_DATA  CR_M2B(2)

/*
---------------------------------------
    返回结果
---------------------------------------
*/
static void_t
return_result (
  __CR_IN__ socket_t        netw,
  __CR_IN__ const void_t*   data,
  __CR_IN__ uint_t          size
    )
{
    int32u  temp = DWORD_BE(size);

    socket_tcp_send(netw, &temp, sizeof(temp));
    socket_tcp_send(netw, data, size);
}

/*
---------------------------------------
    清除缓冲
---------------------------------------
*/
static void_t
clean_buffer (
  __CR_IN__ socket_t    netw,
  __CR_IN__ uint_t      size
    )
{
    byte_t  tmp;

    socket_set_timeout(netw, 1000, 100);
    for (; size != 0; size--) {
        if (socket_tcp_recv(netw, &tmp, 1) != 1)
            break;
    }
    socket_set_timeout(netw, 1000, BJNETSERV_TIMEOUT);
}

/*
---------------------------------------
    工作线程
---------------------------------------
*/
static uint_t STDCALL
bjnetcomm_main (
  __CR_IN__ void_t* arg
    )
{
    byte_t  cmd;
    uint_t  len;
    byte_t* buf;
    ansi_t* json;
    ansi_t* user;
    uint_t  back;
    uint_t  size;
    int32u  temp;
    uint_t  type = 1;
    ansi_t  token[256];
    /* ------------- */
    bjnetcomm_t bjnet;
    socket_t netw = (socket_t)arg;

    bjnet = bjnetcomm_init();
    if (bjnet == NULL) {
        printf("[BJNetServ] bjnetcomm_init() failure!\n");
        socket_close(netw);
        return (BJNETSERV_FAIL);
    }
    user = NULL;
    mem_zero(token, sizeof(token));

    for (;;) {
        cmd = 0xFF;
        back = socket_tcp_recv(netw, &cmd, sizeof(cmd));
        if (back == CR_SOCKET_TIMEOUT)
            continue;
        if (back != sizeof(cmd))
            break;

        /* 退出 */
        if (cmd == 0x00) {
            s_quit = TRUE;
            break;
        }

        /* 复位 */
        if (cmd == 0x01) {
            bjnetcomm_reset(bjnet);
            continue;
        }

        /* 设置最大包大小 */
        if (cmd == 0x02) {
            back = socket_tcp_recv(netw, &temp, sizeof(temp));
            if (back == CR_SOCKET_TIMEOUT)
                continue;
            if (back != sizeof(temp))
                break;
            bjnetcomm_size_max(bjnet, DWORD_BE(temp));
            continue;
        }

        /* 更新令牌 */
        if (cmd == 0x03) {
            back = socket_tcp_recv(netw, &cmd, sizeof(cmd));
            if (back == CR_SOCKET_TIMEOUT)
                continue;
            if (back != sizeof(cmd))
                break;
            back = socket_tcp_recv(netw, token, (uint_t)cmd);
            if (back == CR_SOCKET_TIMEOUT)
                continue;
            if (back != (uint_t)cmd)
                break;
            token[back] = 0x00;
            bjnetcomm_update_token(bjnet, token);
            mem_zero(token, sizeof(token));
            continue;
        }

        /* 更新加密 */
        if (cmd == 0x04) {
            back = socket_tcp_recv(netw, &cmd, sizeof(cmd));
            if (back == CR_SOCKET_TIMEOUT)
                continue;
            if (back != sizeof(cmd))
                break;
            type = (uint_t)cmd;
            continue;
        }

        /* 加密数据 & 解密数据 & 用户字符串 */
        if (cmd == 0x05 || cmd == 0x06 || cmd == 0x07) {
            back = socket_tcp_recv(netw, &temp, sizeof(temp));
            if (back == CR_SOCKET_TIMEOUT)
                continue;
            if (back != sizeof(temp))
                break;
            len = (uint_t)DWORD_BE(temp);
            if (len == 0) {
                return_result(netw, "error", 5);
                printf("[BJNetServ] empty packet!\n");
                continue;
            }
            if (len >= BJNETSERV_MAX_DATA) {
                return_result(netw, "error", 5);
                clean_buffer(netw, len);
                printf("[BJNetServ] packet too big! %u\n", len);
                continue;
            }
            buf = (byte_t*)mem_malloc(len + 1);
            if (buf == NULL) {
                return_result(netw, "error", 5);
                clean_buffer(netw, len);
                printf("[BJNetServ] out of memory!\n");
                continue;
            }
            back = socket_tcp_recv(netw, buf, len);
            if (back == CR_SOCKET_TIMEOUT) {
                mem_free(buf);
                continue;
            }
            if (back != len) {
                mem_free(buf);
                break;
            }
            if (cmd == 0x05) {
                json = bjnetcomm_enc_bytes(bjnet, type, buf, len, user);
                if (json == NULL) {
                    printf("[BJNetServ] bjnetcomm_enc_bytes() failure!\n");
                    return_result(netw, "error", 5);
                }
                else {
                    return_result(netw, json, str_lenA(json));
                    mem_free(json);
                }
                mem_free(buf);
            }
            else
            if (cmd == 0x06) {
                buf[len] = 0x00;
                arg = bjnetcomm_dec_bytes(bjnet, (ansi_t*)buf, &size);
                if (arg == NULL) {
                    printf("[BJNetServ] bjnetcomm_dec_bytes() failure!\n");
                    return_result(netw, "error", 5);
                }
                else {
                    return_result(netw, arg, size);
                    mem_free(arg);
                }
                mem_free(buf);
            }
            else {
                buf[len] = 0x00;
                if (buf[0] == 0x00) {
                    mem_free(buf);
                    buf = NULL;
                }
                if (user != NULL)
                    mem_free(user);
                user = (ansi_t*)buf;
            }
            continue;
        }
    }
    if (user != NULL)
        mem_free(user);
    bjnetcomm_kill(bjnet);
    socket_close(netw);
    return (BJNETSERV_OKAY);
}

/*
=======================================
    主函数
=======================================
*/
int main (int argc, char *argv[])
{
    leng_t      skip;
    uint_t      port;
    thrd_t      thrd;
    socket_t    srv, cli;

    if (argc < 2) {
        printf("Usage: BJNetServ <PORT>\n");
        return (BJNETSERV_FAIL);
    }
    crhack_core_init();
    port = str2intxA(argv[1], &skip);
    if (skip <= 3 || port <= 1024 || port >= 65535) {
        printf("[BJNetServ] invalid port!\n");
        return (BJNETSERV_FAIL);
    }

    if (!socket_init()) {
        printf("[BJNetServ] socket_init() failure!\n");
        return (BJNETSERV_FAIL);
    }
    srv = server_tcp_open("127.0.0.1", (int16u)port);
    if (srv == NULL) {
        printf("[BJNetServ] server_tcp_open() failure!\n");
        return (BJNETSERV_FAIL);
    }
    socket_set_timeout(srv, 1000, BJNETSERV_TIMEOUT);
    printf("[BJNetServ] Welcome!\n");

    while (!s_quit) {
        cli = server_tcp_accept(srv);
        if (cli == NULL)
            continue;
        socket_set_timeout(cli, 1000, BJNETSERV_TIMEOUT);
        thrd = thread_new(0, bjnetcomm_main, cli, FALSE, CR_PRRT_NRM, NULL);
        if (thrd == NULL) {
            printf("[BJNetServ] thread_new() failure!\n");
            socket_close(cli);
        }
        else {
            thread_del(thrd);
        }
    }
    socket_close(srv);
    printf("[BJNetServ] Bye Bye!\n");
    return (BJNETSERV_OKAY);
}
