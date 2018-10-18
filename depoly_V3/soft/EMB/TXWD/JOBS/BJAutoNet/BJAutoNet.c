/*
 *********************************
 *  BJAutoNet 服务器
 *
 *  创建时间: 2016-04-25
 *********************************
 */

#include "crhack.h"
#include "util/cjson/cJSON.h"

#include <stdio.h>

/* BJAutoNet 导入库 */
#ifndef _CR_NO_PRAGMA_LIB_
    #pragma comment (lib, "COREs.lib")
    #pragma comment (lib, "UTILs.lib")
#endif

/* 全局的参数 */
static sCAST6   s_crypto;
static byte_t   s_key[32];
static int16u   s_port = 6666;
static ansi_t*  s_addr = NULL;
static ansi_t*  s_mdns = NULL;
static bool_t   s_quit = FALSE;
static sCURBEAD s_device_list;

/* 预设的命令 */
static const byte_t s_getall[] = {
    'B', 'J', 'A', 'N', 0x00, 0x00, 0x1B,
};

/* 配置的参数 */
#define BJAUTONET_OKAY          0
#define BJAUTONET_FAIL      __LINE__
#define BJAUTONET_TIME          10
#define BJAUTONET_MAX_DATA     512
/* ---------------------------- */
#define BJAUTONET_KLEN          20
#define BJAUTONET_SKEY  "BlowJobs::BJAutoNet"
#define BJAUTONET_MDNS    "BJAutoNet.mdns"
#define BJAUTONET_CFGS    "BJAutoNet.json"

/* 节点单元结构 */
typedef struct
{
        ansi_t  sn[34];     /* 序号 */
        ansi_t  ip[16];     /* 地址 */
        ansi_t  tp[30];     /* 类型 */

} sBJNODE;

typedef void_t  (*bjnode_do_t) (void_t*, sBJNODE*);

/*
---------------------------------------
    节点查找
---------------------------------------
*/
static uint_t
bjnode_find (
  __CR_IN__ const void_t*   key
    )
{
    return (hash_str_djb2(key, 32));
}

/*
---------------------------------------
    节点比较
---------------------------------------
*/
static bool_t
bjnode_comp (
  __CR_IN__ const void_t*   key,
  __CR_IN__ const void_t*   obj
    )
{
    sBJNODE*    unit;

    unit = (sBJNODE*)obj;
    if (mem_cmp(key, unit->sn, 32) == 0)
        return (TRUE);
    return (FALSE);
}

/*
---------------------------------------
    读取设备列表
---------------------------------------
*/
static bool_t
device_reload (
  __CR_IN__ const ansi_t*   cfgs
    )
{
    cJSON*  root;
    cJSON*  objs;
    cJSON*  objt;
    uint_t  idxs;
    uint_t  cnts;
    ansi_t* temp;
    sBJNODE node;

    /* 加载配置文件 */
    temp = file_load_as_strA(cfgs);
    if (temp == NULL)
        return (FALSE);
    root = cJSON_Parse(temp);
    mem_free(temp);
    if (root == NULL)
        return (FALSE);
    cnts = cJSON_GetArraySize(root);
    if (cnts <= 1)
        goto _failure;

    /* 创建设备列表 */
    curbead_freeT(&s_device_list, sBJNODE);
    if (!curbead_initT(&s_device_list, sBJNODE, (cnts * 1000) / 618))
        goto _failure;
    s_device_list.find = bjnode_find;
    s_device_list.comp = bjnode_comp;
    for (idxs = 0; idxs < cnts; idxs++)
    {
        /* 解析节点参数 */
        objs = cJSON_GetArrayItem(root, idxs);
        if (objs == NULL || objs->type != cJSON_Object) {
            printf("[BJAutoNet] invalid device node!\n");
            continue;
        }
        objt = cJSON_GetObjectItem(objs, "SN");
        if (objt == NULL || objt->type != cJSON_String ||
            str_lenA(objt->valuestring) != 32) {
            printf("[BJAutoNet] invalid node attr SN!\n");
            continue;
        }
        struct_zero(&node, sBJNODE);
        mem_cpy(node.sn, objt->valuestring, 32);
        objt = cJSON_GetObjectItem(objs, "TYPE");
        if (objt == NULL || objt->type != cJSON_String ||
            str_lenA(objt->valuestring) >= 30) {
            printf("[BJAutoNet] invalid node attr TYPE!\n");
            continue;
        }
        str_cpyA(node.tp, objt->valuestring);

        /* 插入节点到列表 */
        if (curbead_insertT(&s_device_list, sBJNODE, node.sn, &node) == NULL)
            printf("[BJAutoNet] curbead_insertT() failure!\n");
    }
    cJSON_Delete(root);
    return (TRUE);

_failure:
    cJSON_Delete(root);
    return (FALSE);
}

/*
---------------------------------------
    设备遍历
---------------------------------------
*/
static void_t
device_traversal (
  __CR_IN__ void_t*     parm,
  __CR_IN__ bjnode_do_t func
    )
{
    uint_t      idx;
    sCURBEAD*   tbl = &s_device_list;

    if (tbl->__list__ == NULL)
        return;
    for (idx = 0; idx < tbl->__size__; idx++)
    {
        sLIST*      lst;
        sLST_UNIT*  node;

        lst = &tbl->__list__[idx];
        if (lst->__size__ == 0)
            continue;
        node = lst->__head__;
        do
        {
            sBJNODE*    info;

            info = slist_get_dataT(node, sBJNODE);
            func(parm, info);
            node = node->next;
        } while (node != NULL);
    }
}

/*
---------------------------------------
    打印节点
---------------------------------------
*/
static void_t
device_list (
  __CR_IN__ void_t*     param,
  __CR_IO__ sBJNODE*    bjnode
    )
{
    if (bjnode->ip[0] == 0x00)
        printf("SN=%s\tTYPE=%s\t[OFFLINE]\n", bjnode->sn, bjnode->tp);
    else
        printf("SN=%s\tTYPE=%s\tIP=%s\n", bjnode->sn, bjnode->tp, bjnode->ip);
}

/*
---------------------------------------
    DNS 映射
---------------------------------------
*/
static void_t
device_flush (
  __CR_IN__ void_t*     param,
  __CR_IO__ sBJNODE*    bjnode
    )
{
    if (bjnode->ip[0] != 0x00)
        fprintf((FILE*)param, "%s\tbjan.%s\n", bjnode->ip, bjnode->sn);
}

/*
---------------------------------------
    注册设备
---------------------------------------
*/
static bool_t
device_online (
  __CR_OT__ ansi_t*         sn,
  __CR_IN__ const byte_t*   dat,
  __CR_IN__ const ansi_t*   ip
    )
{
    int32u      chk[4];
    sBJNODE*    bjnode;

    /* 判断是不是在设备列表里 */
    hex2strA(sn, dat, sizeof(chk));
    bjnode = curbead_findT(&s_device_list, sBJNODE, sn);
    if (bjnode == NULL)
        return (FALSE);

    /* 校验是不是有效设备 */
    mem_cpy(chk, dat, sizeof(chk));
    crypto_cast6_enc(&s_crypto, chk, chk);
    if (mem_cmp(chk, &dat[16], 16) != 0)
        return (FALSE);

    /* 必要时更新 IP 地址 */
    if (str_cmpA(ip, bjnode->ip) != 0)
    {
        FILE*   fp;

        str_cpyA(bjnode->ip, ip);
#if defined(_CR_OS_MSWIN_)
        fp = fopen("C:\\Windows\\System32\\drivers\\etc\\hosts", "w");
#else
        fp = fopen("/etc/hosts", "w");
#endif
        if (fp != NULL) {
            fprintf(fp, "%s", s_mdns);
            device_traversal(fp, device_flush);
            fclose(fp);
        }
        return (TRUE);
    }
    return (FALSE);
}

/*
---------------------------------------
    查询设备
---------------------------------------
*/
static bool_t
device_query (
  __CR_OT__ ansi_t*         ip,
  __CR_IN__ const byte_t*   dat,
  __CR_IN__ socket_t        netw
    )
{
    uint_t      idx;
    uint_t      sum;
    sBJNODE*    bjnode;
    /* ------------- */
    byte_t  st[BJAUTONET_MAX_DATA];

    /* 判断是不是在设备列表里 */
    bjnode = curbead_findT(&s_device_list, sBJNODE, dat);
    if (bjnode == NULL)
        return (FALSE);
    str_cpyA(ip, bjnode->ip);

    /* 广播这条信息 */
    mem_cpy(&st[0], "BJAN\x04\x30", 6);
    mem_cpy(&st[6], dat, 32);
    mem_cpy(&st[38], ip, 16);
    for (sum = 0, idx = 0; idx < 6 + 32 + 16; idx++)
        sum = sum + (uint_t)st[idx];
    st[idx++] = (byte_t)sum;
    socket_udp_send(netw, NULL, 0, st, idx);
    return (TRUE);
}

/*
---------------------------------------
    组播线程
---------------------------------------
*/
static uint_t STDCALL
bjautonet_main (
  __CR_IN__ void_t* arg
    )
{
    int16u  port;
    ansi_t  ip[16], sn[34];
    uint_t  back, idx, sum;
    byte_t  dat[BJAUTONET_MAX_DATA];
    /* -------------------------- */
    xtime_t     xtim = timer_new();
    socket_t    netw = (socket_t)arg;

    /* 启动时查一遍 */
    socket_udp_send(netw, s_addr, s_port, s_getall,
                            sizeof(s_getall));
    timer_set_base(xtim);
    while (!s_quit)
    {
        /* 每一段时间更新一下 SN 表 */
        if (timer_get_delta(xtim) > BJAUTONET_TIME * 1000) {
            socket_udp_send(netw, s_addr, s_port, s_getall,
                                    sizeof(s_getall));
            timer_set_base(xtim);
        }

        /* 接收响应包 */
        back = socket_udp_recv(netw, dat, BJAUTONET_MAX_DATA);
        if (back < 7 || back > BJAUTONET_MAX_DATA)
            continue;
        if (mem_cmp(dat, "BJAN", 4) != 0)
            continue;
        if (back - 7 != (uint_t)dat[4 + 1])
            continue;
        sum = (uint_t)('B' + 'J' + 'A' + 'N');
        for (idx = 4; idx < back - 1; idx++)
            sum = sum + (uint_t)dat[idx];
        if ((sum & 0xFF) != (uint_t)dat[back - 1])
            continue;
        port = socket_remote_ipA(netw, ip);

        /* 处理命令 */
        switch (dat[4 + 0])
        {
            default:    /* 非法指令 */
                if (dat[4 + 0] & 0xC0) {
                    printf("[BJAutoNet] unsupported command %02X!\n",
                            dat[4 + 0]);
                }
                break;

            case 0x80:
            case 0x83:  /* 注册设备 */
                if (dat[4 + 1] == 0x20) {
                    if (device_online(sn, &dat[4 + 2], ip)) {
                        printf("[BJAutoNet] online IP=%s:%u SN=%s ",
                                            ip, port, sn);
                        if (dat[4 + 0] == 0x83)
                            printf("RESET\n");
                        else
                            printf("QUERY\n");
                    }
                }
                break;

            case 0x84:  /* 查询设备 */
                if (dat[4 + 1] == 0x20) {
                    if (device_query(ip, &dat[4 + 2], netw))
                        printf("[BJAutoNet] query dest IP=%s\n", ip);
                }
                break;

            case 0x85:  /* 打印节点 */
                if (dat[4 + 1] == 0x00) {
                    printf("[BJAutoNet] device node list\n");
                    printf("============================\n");
                    device_traversal(NULL, device_list);
                }
                break;

            case 0x86:  /* 刷新节点 */
                if (dat[4 + 1] == 0x00) {
                    if (device_reload(BJAUTONET_CFGS))
                        printf("[BJAutoNet] config reload okay!\n");
                }
                break;
        }
    }
    timer_del(xtim);
    socket_close(netw);
    return (BJAUTONET_OKAY);
}

/*
=======================================
    主函数
=======================================
*/
int main (int argc, char *argv[])
{
    sRC4        ctx;
    leng_t      skip;
    uint_t      port;
    ansi_t*     addr;
    socket_t    netw;

    if (argc < 3) {
        printf("Usage: BJAutoNet <IP> <PORT> [ETH_IP=0.0.0.0]\n");
        return (BJAUTONET_FAIL);
    }
    crhack_core_init();
    port = str2intxA(argv[2], &skip);
    if (skip <= 3 || port <= 1024 || port >= 65535) {
        printf("[BJAutoNet] invalid port!\n");
        return (BJAUTONET_FAIL);
    }
    if (argc >= 4)
        addr = argv[3];
    else
        addr = "0.0.0.0";

    /* 初始化加载配置 */
    crypto_rc4_key(&ctx, BJAUTONET_SKEY, BJAUTONET_KLEN);
    struct_zero(&s_device_list, sCURBEAD);
    if (!device_reload(BJAUTONET_CFGS)) {
        printf("[BJAutoNet] device_reload() failure!\n");
        return (BJAUTONET_FAIL);
    }
    hash_sha256_total(s_key, BJAUTONET_SKEY, BJAUTONET_KLEN);
    if (!socket_init()) {
        printf("[BJAutoNet] socket_init() failure!\n");
        return (BJAUTONET_FAIL);
    }

    /* 初始化组播部分 */
#if defined(_CR_OS_MSWIN_)
    netw = server_udp_open(addr, (int16u)port);
#else
    netw = server_udp_open(argv[1], (int16u)port);
#endif
    if (netw == NULL) {
        printf("[BJAutoNet] server_udp_open() failure!\n");
        return (BJAUTONET_FAIL);
    }
    crypto_rc4_ops(&ctx, s_key, sizeof(s_key));
#if defined(_CR_OS_MSWIN_)
    if (!socket_mcast_enter(netw, argv[1], NULL, 64, TRUE)) {
#else
    if (!socket_mcast_enter(netw, NULL, addr, 64, TRUE)) {
#endif
        printf("[BJAutoNet] socket_mcast_enter() failure!\n");
        socket_close(netw);
        return (BJAUTONET_FAIL);
    }

    /* 默认域名映射 */
    s_mdns = file_load_as_strA(BJAUTONET_MDNS);
    if (s_mdns == NULL)
        s_mdns = "127.0.0.1\tlocalhost\n";

    /* 启动组播任务 */
    s_port = (int16u)port;
    s_addr = str_dupA(argv[1]);
    crypto_rc4_ops(&ctx, s_key, sizeof(s_key));
    socket_set_timeout(netw, 1000, 1000);
    printf("[BJAutoNet] Welcome!\n");
    crypto_cast6_key(&s_crypto, s_key, sizeof(s_key));
    mem_zero(s_key, sizeof(s_key));
    bjautonet_main(netw);
    printf("[BJAutoNet] Bye Bye!\n");
    return (BJAUTONET_OKAY);
}
