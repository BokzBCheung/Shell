/*
 *********************************
 *  BJNodeSet 节点设置
 *
 *  创建时间: 2016-05-26
 *********************************
 */

#include "crhack.h"

#include <stdio.h>

/* BJNodeSet 导入库 */
#ifndef _CR_NO_PRAGMA_LIB_
    #pragma comment (lib, "COREs.lib")
#endif

/* 配置的参数 */
#define BJNODESET_OKAY           0
#define BJNODESET_FAIL          -1
/* ---------------------------- */
#define BJNODESET_KLEN          20
#define BJNODESET_SKEY  "BlowJobs::BJAutoNet"

/*
=======================================
    主函数
=======================================
*/
int main (int argc, char *argv[])
{
    byte_t  st[512];
    byte_t  key[32];
    /* ---------- */
    sRC4        rc4;
    sCAST6      ct6;
    uint_t      port;
    leng_t      skip;
    socket_t    netw;

    if (argc < 3) {
        printf("Usage: BJNodeSet <SN> <IP> [PORT=8888]\n");
        return (BJNODESET_FAIL);
    }

    /* 参数校验 */
    crhack_core_init();
    if (str_lenA(argv[1]) != 32) {
        printf("[BJNodeSet] invalid node sn!\n");
        return (BJNODESET_FAIL);
    }
    if (argc >= 4) {
        port = str2intxA(argv[3], &skip);
        if (skip <= 3 || port <= 1024 || port >= 65535) {
            printf("[BJNodeSet] invalid port!\n");
            return (BJNODESET_FAIL);
        }
    }
    else {
        port = 8888;
    }

    /* 准备参数 */
    hash_sha256_total(key, BJNODESET_SKEY, BJNODESET_KLEN);
    crypto_rc4_key(&rc4, BJNODESET_SKEY, BJNODESET_KLEN);
    crypto_rc4_ops(&rc4, key, sizeof(key));
    crypto_rc4_ops(&rc4, key, sizeof(key));
    crypto_cast6_key(&ct6, key, sizeof(key));
    skip = 16;
    str2datA(key, &skip, argv[1]);
    if (skip != 16) {
        printf("[BJNodeSet] invalid node sn!\n");
        return (BJNODESET_FAIL);
    }
    mem_cpy(key + 16, key, 16);
    crypto_cast6_enc(&ct6, (int32u*)(&key[16]), (int32u*)(&key[16]));

    /* 发送命令 */
    if (!socket_init()) {
        printf("[BJNodeSet] socket_init() failure!\n");
        return (BJNODESET_FAIL);
    }
    netw = client_tcp_open(argv[2], (int16u)port, 3000);
    if (netw == NULL) {
        printf("[BJNodeSet] client_tcp_open() failure!\n");
        return (BJNODESET_FAIL);
    }
    socket_set_timeout(netw, 1000, 5000);
    mem_cpy(&st[0], "BJAN\x01\x20", 6);
    mem_cpy(&st[6], key, 32);
    for (port = 0, skip = 0; skip < 6 + 32; skip++)
        port = port + (uint_t)st[skip];
    st[skip++] = (byte_t)port;
    socket_tcp_send(netw, st, skip);
    port = socket_tcp_recv(netw, st, 7);
    if (port != 7 || mem_cmp(st, "BJAN\x81\x00\x9C", 7) != 0) {
        printf("[BJNodeSet] socket_tcp_recv() failure!\n");
        return (BJNODESET_FAIL);
    }
    socket_close(netw);
    return (BJNODESET_OKAY);
}
