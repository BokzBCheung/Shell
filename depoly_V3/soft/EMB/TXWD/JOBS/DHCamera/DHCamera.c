/*
 *********************************
 *  大华相机通讯库
 *
 *  创建时间: 2016-05-05
 *********************************
 */

#include "crhack.h"
#include "util/codepage.h"
#include "util/cjson/cJSON.h"
#ifndef _CR_NO_PRAGMA_LIB_
    #pragma comment (lib, "COREs.lib")
    #pragma comment (lib, "UTILs.lib")
#endif

#include <stdio.h>

/* 大华相机协议头 */
#ifndef _CR_NO_PRAGMA_PACK_
    #pragma pack (push, 1)
#endif
typedef struct
{
        byte_t  magic[2];       /* "DH" */
        byte_t  version[2];     /* 版本 1.0 */
        int32u  extlen;         /* 扩展数据长度 */
        byte_t  exttype;        /* 扩展数据类型 0:JSON/1:BINARY */
        byte_t  rec[3];         /* 保留字节、不使用填0 */
        int32u  reserved[5];    /* 保留字节、不使用填0 */

} CR_PACKED sITCHead;

#ifndef _CR_NO_PRAGMA_PACK_
    #pragma pack (pop)
#endif

/* 调试用宏 */
#if defined(DHTEST)
    #if defined(_CR_OS_MSWIN_)
        #include <conio.h>
    #endif
    #define DH_DEBUG \
        printf("%u\n", __LINE__);
#else
    #define DH_DEBUG
#endif

/* 外部类型 */
typedef void_t* dhcamera_t;
typedef void_t* dhnotify_t;

#define DHCAMERA_MAGIC  0x55AA

/* 内部结构 */
typedef struct
{
        cJSON*  infos;
        uint_t  count;
        uint_t  isize;
        byte_t* image;
        uint_t  psize[64];
        byte_t* plate[64];

} sDHCAM_SNAP;

typedef struct
{
        bool_t      quit;
        lock_t      lock;
        ansi_t*     addr;
        int16u      port;
        int16u      flag;
        uint_t      cnts;
        uint_t      tout;
        socket_t    netw;

        void_t  (*onErrors) (dhcamera_t, sint_t);
        void_t  (*onSetKey) (dhcamera_t, sint_t);
        void_t  (*onAttach) (dhcamera_t, sint_t);
        void_t  (*onDetach) (dhcamera_t, sint_t);
        void_t  (*onStrobe) (dhcamera_t, sint_t);
        void_t  (*onKAlive) (dhcamera_t, sint_t);
        void_t  (*onNotify) (dhcamera_t, sDHCAM_SNAP*);

} sDHCAM;

/* 两种回调函数类型 */
typedef void_t  (*dh_callback1_t) (dhcamera_t, sint_t);
typedef void_t  (*dh_callback2_t) (dhcamera_t, sDHCAM_SNAP*);

/* 命令号固定的编号 */
#define DHCAMERA_GETKEY     31
#define DHCAMERA_SETKEY     32
#define DHCAMERA_ATTACH     33
#define DHCAMERA_DETACH     34
#define DHCAMERA_STROBE     35
#define DHCAMERA_KALIVE     36

#if !defined(DHTEST)
#if defined(_CR_OS_MSWIN_)
/*
=======================================
    DHCamera DLL 入口点
=======================================
*/
BOOL WINAPI
DllMain (
  __CR_IN__ HANDLE  hinst,
  __CR_IN__ DWORD   reason,
  __CR_UU__ LPVOID  reserved
    )
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            crhack_core_init();
            break;

        case DLL_PROCESS_DETACH:
            crhack_core_free();
            break;
    }
    CR_NOUSE(hinst);
    CR_NOUSE(reserved);
    return (TRUE);
}
#elif   defined(_CR_CC_GCC_)
void_t  so_dhcam_init (void_t) __attribute__((constructor));
void_t  so_dhcam_init (void_t)
{
    crhack_core_init();
}
void_t  so_dhcam_fini (void_t) __attribute__((destructor));
void_t  so_dhcam_fini (void_t)
{
    crhack_core_free();
}
#endif  /* _CR_OS_MSWIN_ */
#endif  /* !DHTEST */

/*
---------------------------------------
    UTF-8 to GBK
---------------------------------------
*/
static ansi_t*
utf8_to_gbk (
  __CR_IN__ const ansi_t*   utf8
    )
{
    bool_t          cvt = FALSE;
    const ansi_t*   str = utf8;

    /* 需要时才转换 */
    while (*str != 0) {
        if (*str++ & 0x80) {
            cvt = TRUE;
            break;
        }
    }
    if (!cvt)
        return ((ansi_t*)utf8);
#if defined(_CR_AR_X86_) || \
    defined(_CR_AR_X64_)
    str = utf8_to_local(CR_GBK, utf8);
#else
    str = utf8_to_cp936(utf8);
#endif
    if (str == NULL)
        return ((ansi_t*)utf8);
    mem_free(utf8);
    return ((ansi_t*)str);
}

/*
---------------------------------------
    发送数据包
---------------------------------------
*/
static void_t
dhcamera_send (
  __CR_IN__ socket_t        netw,
  __CR_IN__ const ansi_t*   json
    )
{
    uint_t      leng;
    sITCHead    head;

    head.magic[0] = 'D';
    head.magic[1] = 'H';
    head.version[0] = 0;
    head.version[1] = 1;
    leng = (uint_t)str_lenA(json);
    head.extlen = (int32u)leng;
    head.extlen = DWORD_LE(head.extlen);
    head.exttype = 0;
    mem_zero(head.rec, sizeof(head.rec));
    mem_zero(head.reserved, sizeof(head.reserved));
    socket_tcp_send(netw, &head, sizeof(head));
    socket_tcp_send(netw, json, leng);
#if defined(DHTEST)
    printf("+++++++++++++++++++++\n");
    printf("%s\n", json);
    printf("+++++++++++++++++++++\n");
#endif
}

/*
---------------------------------------
    接收数据包
---------------------------------------
*/
static sint_t
dhcamera_recv (
  __CR_IN__ socket_t    netw,
  __CR_IN__ ansi_t**    json
    )
{
    uint_t      back;
    ansi_t*     text;
    sITCHead    head;

    /* 查找协议头 */
    for (;;) {
        back = socket_tcp_peek(netw, &head, 2);
        if (back == CR_SOCKET_TIMEOUT)
            return (-3);
        if (back != 2) {
            DH_DEBUG
            return (-1);
        }
        if (head.magic[0] == 'D' && head.magic[1] == 'H')
            break;
        if (head.magic[1] == 'D')
            socket_tcp_recv(netw, &head, 1);
        else
            socket_tcp_recv(netw, &head, 2);
    }
    back = socket_tcp_recv(netw, &head, sizeof(head));
    if (back == CR_SOCKET_TIMEOUT)
        return (-3);
    if (back != sizeof(head)) {
        DH_DEBUG
        return (-1);
    }
    head.extlen = DWORD_LE(head.extlen);
    if (head.extlen > CR_K2B(32)) {
        DH_DEBUG
        return (-1);
    }
    text = str_allocA(head.extlen + 1);
    if (text == NULL)
        return (-2);
    back = socket_tcp_recv(netw, text, head.extlen);
    if (back != head.extlen) {
        DH_DEBUG
        mem_free(text);
        return (-1);
    }
    text[back] = 0x00;
#if defined(DHTEST)
    printf("=====================\n");
    printf("%s\n", text);
    printf("=====================\n");
#endif
    *json = utf8_to_gbk(text);
    return (0);
}

/*
---------------------------------------
    接收二进制图片
---------------------------------------
*/
static byte_t*
dhcamera_recv_bin (
  __CR_IN__ socket_t    netw,
  __CR_IN__ uint_t      size
    )
{
    uint_t      ptr;
    uint_t      back;
    byte_t*     data;
    sITCHead    head;

    /* 先分配内存 */
    data = (byte_t*)mem_malloc(size);
    if (data == NULL)
        return (NULL);
    ptr = 0;

    /* 直到数据收满为止 */
    while (ptr < size)
    {
        /* 查找协议头 */
        for (;;) {
            back = socket_tcp_peek(netw, &head, 2);
            if (back != 2) {
                DH_DEBUG
                goto _failure;
            }
            if (head.magic[0] == 'D' && head.magic[1] == 'H')
                break;
            if (head.magic[1] == 'D')
                socket_tcp_recv(netw, &head, 1);
            else
                socket_tcp_recv(netw, &head, 2);
        }
        back = socket_tcp_recv(netw, &head, sizeof(head));
        if (back != sizeof(head)) {
            DH_DEBUG
            goto _failure;
        }
        head.extlen = DWORD_LE(head.extlen);
        if (head.extlen > CR_K2B(32)) {
            DH_DEBUG
            goto _failure;
        }
        if (ptr + head.extlen > size)
            goto _failure;
        back = socket_tcp_recv(netw, &data[ptr], head.extlen);
        if (back != head.extlen) {
            DH_DEBUG
            goto _failure;
        }
        ptr += head.extlen;
    }
    return (data);

_failure:
    mem_free(data);
    return (NULL);
}

/*
---------------------------------------
    相机鉴权操作
---------------------------------------
*/
static bool_t
dhcamera_check (
  __CR_IN__ socket_t        netw,
  __CR_IN__ const ansi_t*   password
    )
{
    cJSON*  root;
    cJSON*  objs;
    cJSON*  objt;
    sint_t  retc;
    leng_t  size;
    ansi_t  str[34];
    ansi_t  rnd1[18];
    byte_t  shash[16];
    byte_t  dhash[16];
    ansi_t  *json, *rnd2;

    /* 验证相机密码 */
    rand_seed((uint_t)timer_get32());
    sprintf(rnd1, "%08X%08X", rand_get() * rand_get(),
                              rand_get() * rand_get());
    json = str_fmtA("{\"method\":\"getEncryptKey\","
                    "\"params\":{\"random\":\"%s\"},\"id\":%u}",
                            rnd1, DHCAMERA_GETKEY);
    if (json == NULL)
        return (FALSE);
    dhcamera_send(netw, json);
    mem_free(json);
    retc = dhcamera_recv(netw, &json);
    if (retc != 0) {
        DH_DEBUG
        return (FALSE);
    }
    root = cJSON_Parse(json);
    mem_free(json);
    if (root == NULL)
        return (FALSE);
    objt = cJSON_GetObjectItem(root, "params");
    if (objt == NULL || objt->type != cJSON_Object)
        goto _failure;
    objs = cJSON_GetObjectItem(objt, "encryptKey");
    if (objs == NULL || objs->type != cJSON_String)
        goto _failure;
    size = sizeof(shash);
    str2datA(shash, &size, objs->valuestring);
    if (size != sizeof(shash))
        goto _failure;
    objs = cJSON_GetObjectItem(objt, "random");
    if (objs == NULL || objs->type != cJSON_String)
        goto _failure;
    rnd2 = objs->valuestring;
    json = str_fmtA("%s:%s:%s", rnd1, password, rnd2);
    if (json == NULL)
        goto _failure;
    hash_md5_total(dhash, json, str_lenA(json));
    mem_free(json);
    if (mem_cmp(dhash, shash, sizeof(shash)) != 0)
        goto _failure;
    cJSON_Delete(root);

    /* 验证平台密码 */
    retc = dhcamera_recv(netw, &json);
    if (retc != 0) {
        DH_DEBUG
        return (FALSE);
    }
    root = cJSON_Parse(json);
    mem_free(json);
    if (root == NULL)
        return (FALSE);
    objs = cJSON_GetObjectItem(root, "method");
    if (objs == NULL || objs->type != cJSON_String)
        goto _failure;
    if (str_cmpA(objs->valuestring, "getEncryptKey") != 0)
        goto _failure;
    objs = cJSON_GetObjectItem(root, "id");
    if (objs == NULL || objs->type != cJSON_Number)
        goto _failure;
    retc = objs->valueint;
    objt = cJSON_GetObjectItem(root, "params");
    if (objt == NULL || objt->type != cJSON_Object)
        goto _failure;
    objs = cJSON_GetObjectItem(objt, "random");
    if (objs == NULL || objs->type != cJSON_String)
        goto _failure;
    rnd2 = objs->valuestring;
    sprintf(rnd1, "%08X%08X", rand_get() * rand_get(),
                              rand_get() * rand_get());
    json = str_fmtA("%s:%s:%s", rnd2, password, rnd1);
    if (json == NULL)
        goto _failure;
    hash_md5_total(dhash, json, str_lenA(json));
    mem_free(json);
    hex2strA(str, dhash, sizeof(dhash));
    str_lwrA(str);
    json = str_fmtA("{\"id\":%u,\"params\":{\"encryptKey\":\"%s\","
            "\"random\":\"%s\"},\"result\":true}", retc, str, rnd1);
    if (json == NULL)
        goto _failure;
    dhcamera_send(netw, json);
    mem_free(json);
    cJSON_Delete(root);
    return (TRUE);

_failure:
    cJSON_Delete(root);
    return (FALSE);
}

/*
---------------------------------------
    处理相机抓拍
---------------------------------------
*/
static void_t
dhcamera_snap (
  __CR_IN__ sDHCAM* dhcam,
  __CR_IN__ cJSON*  root
    )
{
    uint_t          idx;
    uint_t          offs;
    uint_t          size;
    cJSON*          objt;
    cJSON*          objs;
    sDHCAM_SNAP     snap;
    dh_callback2_t  func;

    /* 有效性校验 */
    struct_zero(&snap, sDHCAM_SNAP);
    snap.infos = cJSON_GetObjectItem(root, "params");
    if (snap.infos == NULL || snap.infos->type != cJSON_Object)
        return;
    snap.infos = cJSON_GetObjectItem(snap.infos, "snapInfos");
    if (snap.infos == NULL || snap.infos->type != cJSON_Array)
        return;
    snap.count = cJSON_GetArraySize(snap.infos);
    if (snap.count == 0 || snap.count > cntsof(snap.plate))
        return;

    /* 读取图片数据 */
    objt = cJSON_GetArrayItem(snap.infos, 0);
    if (objt != NULL && objt->type == cJSON_Object) {
        objt = cJSON_GetObjectItem(objt, "BinSize");
        if (objt != NULL && objt->type == cJSON_Number) {
            size = objt->valueint;
            snap.image = dhcamera_recv_bin(dhcam->netw, size);
            if (snap.image != NULL)
            {
#if defined(DHTEST)
                FILE* fp;
                fp = fopen("test.jpg", "wb");
                if (fp != NULL) {
                    fwrite(snap.image, size, 1, fp);
                    fclose(fp);
                }
#endif
                /* 有图有真相 */
                for (idx = 0; idx < snap.count; idx++) {
                    objt = cJSON_GetArrayItem(snap.infos, idx);
                    if (objt == NULL || objt->type != cJSON_Object)
                        break;
                    objt = cJSON_GetObjectItem(objt, "Image");
                    if (objt == NULL || objt->type != cJSON_Object) {
                        if (idx == 0 && size > 3 * 2)
                        {
                            /* 查找 JPEG 的结束 */
                            for (offs = 0; offs < size - 2; offs++) {
                                if (snap.image[offs + 0] == 0xFF &&
                                    snap.image[offs + 1] == 0xD9)
                                    break;
                            }
                            snap.isize = offs + 2;
                        }
                        continue;
                    }
                    objs = cJSON_GetObjectItem(objt, "Offset");
                    if (objs == NULL || objs->type != cJSON_Number)
                        continue;
                    offs = objs->valueint;
                    objs = cJSON_GetObjectItem(objt, "Length");
                    if (objs == NULL || objs->type != cJSON_Number)
                        continue;
                    if (size < offs ||
                        size - offs < (uint_t)objs->valueint)
                        continue;
                    snap.psize[idx] = objs->valueint;
                    snap.plate[idx] = &snap.image[offs];
                    if (idx == 0)
                        snap.isize = offs;
                }
            }
        }
    }

    /* 调用用户回调 */
    mtlock_acquire(&dhcam->lock);
    func = dhcam->onNotify;
    mtlock_release(&dhcam->lock);
    if (func != NULL)
        func(dhcam, &snap);

    /* 释放图片内存 */
    TRY_FREE(snap.image);
}

/*
---------------------------------------
    相机接收线程
---------------------------------------
*/
static uint_t STDCALL
dhcamera_main (
  __CR_IN__ void_t* arg
    )
{
    cJSON*  root;
    cJSON*  objs;
    sint_t  retc;
    uint_t  tout;
    ansi_t* json;
    sDHCAM* real;

    tout = 0;
    real = (sDHCAM*)arg;
    while (!real->quit)
    {
        dh_callback1_t  func;

        /* 等着接收数据包 */
        retc = dhcamera_recv(real->netw, &json);
        if (retc == -3) {
#if defined(DHTEST)
            printf("timeout count = %u\n", tout);
#endif
            mtlock_acquire(&real->lock);
            if (real->cnts == 0 || ++tout < real->tout) {
                mtlock_release(&real->lock);
                continue;
            }
            mtlock_release(&real->lock);
        }
        tout = 0;
        if (retc != 0) {
            mtlock_acquire(&real->lock);
            func = real->onErrors;
            mtlock_release(&real->lock);
            if (func != NULL)
                func(real, 0);
            DH_DEBUG
            break;
        }

        /* 处理接收到的 JSON 数据 */
        root = cJSON_Parse(json);
        mem_free(json);
        if (root == NULL)
            continue;
        objs = cJSON_GetObjectItem(root, "method");
        if (objs != NULL && objs->type == cJSON_String &&
            str_cmpA(objs->valuestring, "notifySnapInfo") == 0)
        {
            /* 相机抓拍通知 */
            dhcamera_snap(real, root);
        }
        else
        {
            /* 普通命令返回 */
            objs = cJSON_GetObjectItem(root, "id");
            if ((objs == NULL) ||
                (objs->type != cJSON_Number))
                goto _next_loop;
            retc = objs->valueint;
            objs = cJSON_GetObjectItem(root, "result");
            if ((objs == NULL) ||
                (objs->type != cJSON_True && objs->type != cJSON_False))
                goto _next_loop;

            /* 根据命令 ID 执行回调 */
            switch (retc)
            {
                default:
                    break;

                case DHCAMERA_SETKEY:
                    mtlock_acquire(&real->lock);
                    if (real->cnts != 0)
                        real->cnts -= 1;
                    func = real->onSetKey;
                    mtlock_release(&real->lock);
                    if (func != NULL)
                        func(real, objs->valueint);
                    break;

                case DHCAMERA_ATTACH:
                    mtlock_acquire(&real->lock);
                    if (real->cnts != 0)
                        real->cnts -= 1;
                    func = real->onAttach;
                    mtlock_release(&real->lock);
                    if (func != NULL)
                        func(real, objs->valueint);
                    break;

                case DHCAMERA_DETACH:
                    mtlock_acquire(&real->lock);
                    if (real->cnts != 0)
                        real->cnts -= 1;
                    func = real->onDetach;
                    mtlock_release(&real->lock);
                    if (func != NULL)
                        func(real, objs->valueint);
                    break;

                case DHCAMERA_STROBE:
                    mtlock_acquire(&real->lock);
                    if (real->cnts != 0)
                        real->cnts -= 1;
                    func = real->onStrobe;
                    mtlock_release(&real->lock);
                    if (func != NULL)
                        func(real, objs->valueint);
                    break;

                case DHCAMERA_KALIVE:
                    mtlock_acquire(&real->lock);
                    if (real->cnts != 0)
                        real->cnts -= 1;
                    func = real->onKAlive;
                    mtlock_release(&real->lock);
                    if (func != NULL)
                        func(real, objs->valueint);
                    break;
            }
        }
_next_loop:
        cJSON_Delete(root);
    }
    real->quit = TRUE;
    real->flag = DHCAMERA_MAGIC;
    return (TRUE);
}

/*
=======================================
    相机 setKey 操作
=======================================
*/
CR_API sint_t
dhcamera_setkey (
  __CR_IN__ dhcamera_t      dhcam,
  __CR_IN__ const ansi_t*   password
    )
{
    ansi_t* json;
    sDHCAM* real;

    json = str_fmtA("{\"method\":\"setKey\",\"params\":{\"key\":\"%s\"},"
                        "\"id\":%u}", password, DHCAMERA_SETKEY);
    if (json == NULL)
        return (-2);
    real = (sDHCAM*)dhcam;
    dhcamera_send(real->netw, json);
    mem_free(json);
    mtlock_acquire(&real->lock);
    real->cnts += 1;
    mtlock_release(&real->lock);
    return (0);
}

/*
=======================================
    相机 attachSnapInfo 操作
=======================================
*/
CR_API sint_t
dhcamera_attach (
  __CR_IN__ dhcamera_t  dhcam
    )
{
    ansi_t* json;
    sDHCAM* real;

    json = str_fmtA("{\"method\":\"attachSnapInfo\",\"params\":null,"
                        "\"id\":%u}", DHCAMERA_ATTACH);
    if (json == NULL)
        return (-2);
    real = (sDHCAM*)dhcam;
    dhcamera_send(real->netw, json);
    mem_free(json);
    mtlock_acquire(&real->lock);
    real->cnts += 1;
    mtlock_release(&real->lock);
    return (0);
}

/*
=======================================
    相机 detachSnapInfo 操作
=======================================
*/
CR_API sint_t
dhcamera_detach (
  __CR_IN__ dhcamera_t  dhcam
    )
{
    ansi_t* json;
    sDHCAM* real;

    json = str_fmtA("{\"method\":\"detachSnapInfo\",\"params\":null,"
                        "\"id\":%u}", DHCAMERA_DETACH);
    if (json == NULL)
        return (-2);
    real = (sDHCAM*)dhcam;
    dhcamera_send(real->netw, json);
    mem_free(json);
    mtlock_acquire(&real->lock);
    real->cnts += 1;
    mtlock_release(&real->lock);
    return (0);
}

/*
=======================================
    相机 operStrobe 操作
=======================================
*/
CR_API sint_t
dhcamera_strobe (
  __CR_IN__ dhcamera_t  dhcam,
  __CR_IN__ uint_t      is_open
    )
{
    ansi_t* json;
    ansi_t* gate;
    sDHCAM* real;

    if (is_open)
        gate = "\"open\"";
    else
        gate = "\"close\"";
    json = str_fmtA("{\"method\":\"operStrobe\",\"params\":{\"action\":%s},"
                        "\"id\":%u}", gate, DHCAMERA_STROBE);
    if (json == NULL)
        return (-2);
    real = (sDHCAM*)dhcam;
    dhcamera_send(real->netw, json);
    mem_free(json);
    mtlock_acquire(&real->lock);
    real->cnts += 1;
    mtlock_release(&real->lock);
    return (0);
}

/*
=======================================
    相机 keepAlive 操作
=======================================
*/
CR_API sint_t
dhcamera_kalive (
  __CR_IN__ dhcamera_t  dhcam,
  __CR_IN__ uint_t      seconds
    )
{
    ansi_t* json;
    sDHCAM* real;

    json = str_fmtA("{\"method\":\"keepAlive\",\"params\":{\"timeout\":%u},"
                        "\"id\":%u}", seconds, DHCAMERA_KALIVE);
    if (json == NULL)
        return (-2);
    real = (sDHCAM*)dhcam;
    dhcamera_send(real->netw, json);
    mem_free(json);
    mtlock_acquire(&real->lock);
    real->cnts += 1;
    real->tout = seconds;
    mtlock_release(&real->lock);
    return (0);
}

/*
=======================================
    相机断开
=======================================
*/
CR_API void_t
dhcamera_close (
  __CR_IN__ dhcamera_t  dhcam
    )
{
    sDHCAM* real;

    real = (sDHCAM*)dhcam;
    if (real->netw != NULL)
        socket_close(real->netw);
    mtlock_free(&real->lock);
    mem_free(real->addr);
    mem_free(dhcam);
}

/*
=======================================
    相机连接
=======================================
*/
CR_API dhcamera_t
dhcamera_open (
  __CR_IN__ const ansi_t*   addr,
  __CR_IN__ int16u          port,
  __CR_IN__ const ansi_t*   password
    )
{
    thrd_t  thrd;
    sDHCAM* rett;

    rett = struct_new(sDHCAM);
    if (rett == NULL)
        return (NULL);
    struct_zero(rett, sDHCAM);
    rett->addr = str_dupA(addr);
    if (rett->addr == NULL) {
        mem_free(rett);
        return (NULL);
    }
    rett->tout = 60;
    rett->port = port;
    rett->netw = client_tcp_open(addr, port, 100);
    if (rett->netw == NULL) {
        mem_free(rett->addr);
        mem_free(rett);
        return (NULL);
    }
    socket_set_timeout(rett->netw, 1000, 1000);
    if (!socket_tcp_set_alive(rett->netw, 5000, 1000, 5))
        goto _failure;
    if (!dhcamera_check(rett->netw, password))
        goto _failure;
    mtlock_init(&rett->lock);
    thrd = thread_new(0, dhcamera_main, rett, FALSE, CR_PRRT_NRM, NULL);
    if (thrd == NULL)
        goto _failure;
    thread_del(thrd);
    return ((dhcamera_t)rett);

_failure:
    dhcamera_close(rett);
    return (NULL);
}

/*
=======================================
    相机重连
=======================================
*/
CR_API dhcamera_t
dhcamera_reopen (
  __CR_IN__ dhcamera_t      dhcam,
  __CR_IN__ const ansi_t*   password
    )
{
    thrd_t  thrd;
    sDHCAM* real;

    /* 结束当前线程 */
    real = (sDHCAM*)dhcam;
    if (!real->quit) {
        real->quit = TRUE;
        while (real->flag != DHCAMERA_MAGIC)
            thread_sleep(1);
    }

    /* 重走流程 */
    if (real->netw != NULL)
        socket_close(real->netw);
    real->netw = client_tcp_open(real->addr, real->port, 100);
    if (real->netw == NULL)
        goto _failure;
    socket_set_timeout(real->netw, 1000, 1000);
    if (!socket_tcp_set_alive(real->netw, 5000, 1000, 5))
        goto _failure;
    if (!dhcamera_check(real->netw, password))
        goto _failure;
    real->flag = 0;
    real->cnts = 0;
    real->quit = FALSE;
    thrd = thread_new(0, dhcamera_main, real, FALSE, CR_PRRT_NRM, NULL);
    if (thrd == NULL)
        goto _failure;
    thread_del(thrd);
    return ((dhcamera_t)real);

_failure:
    real->quit = TRUE;
    if (real->netw != NULL) {
        socket_close(real->netw);
        real->netw = NULL;
    }
    return (NULL);
}

/*
=======================================
    相机设置回调
=======================================
*/
CR_API void_t
dhcamera_callback (
  __CR_IN__ dhcamera_t      dhcam,
  __CR_IN__ const ansi_t*   type,
  __CR_IN__ void_t*         func
    )
{
    sDHCAM* real = (sDHCAM*)dhcam;

    if (str_cmpA(type, "onErrors") == 0) {
        mtlock_acquire(&real->lock);
        real->onErrors = (dh_callback1_t)func;
        mtlock_release(&real->lock);
    }
    else
    if (str_cmpA(type, "onSetKey") == 0) {
        mtlock_acquire(&real->lock);
        real->onSetKey = (dh_callback1_t)func;
        mtlock_release(&real->lock);
    }
    else
    if (str_cmpA(type, "onAttach") == 0) {
        mtlock_acquire(&real->lock);
        real->onAttach = (dh_callback1_t)func;
        mtlock_release(&real->lock);
    }
    else
    if (str_cmpA(type, "onDetach") == 0) {
        mtlock_acquire(&real->lock);
        real->onDetach = (dh_callback1_t)func;
        mtlock_release(&real->lock);
    }
    else
    if (str_cmpA(type, "onStrobe") == 0) {
        mtlock_acquire(&real->lock);
        real->onStrobe = (dh_callback1_t)func;
        mtlock_release(&real->lock);
    }
    else
    if (str_cmpA(type, "onKAlive") == 0) {
        mtlock_acquire(&real->lock);
        real->onKAlive = (dh_callback1_t)func;
        mtlock_release(&real->lock);
    }
    else
    if (str_cmpA(type, "onNotify") == 0) {
        mtlock_acquire(&real->lock);
        real->onNotify = (dh_callback2_t)func;
        mtlock_release(&real->lock);
    }
}

/*
=======================================
    获取相机地址
=======================================
*/
CR_API ansi_t*
dhcamera_addr (
  __CR_IN__ dhcamera_t  dhcam
    )
{
    return (((sDHCAM*)dhcam)->addr);
}

/*
=======================================
    获取抓拍结果个数
=======================================
*/
CR_API uint_t
dhnotify_count (
  __CR_IN__ dhnotify_t  dhsnap
    )
{
    return (((sDHCAM_SNAP*)dhsnap)->count);
}

/*
=======================================
    获取抓拍大图片
=======================================
*/
CR_API void_t*
dhnotify_image (
  __CR_IN__ dhnotify_t  dhsnap,
  __CR_OT__ uint_t*     size
    )
{
    sDHCAM_SNAP*    real;

    real = (sDHCAM_SNAP*)dhsnap;
    if (size != NULL)
        *size = real->isize;
    return (real->image);
}

/*
=======================================
    获取抓拍小图片
=======================================
*/
CR_API void_t*
dhnotify_plate (
  __CR_IN__ dhnotify_t  dhsnap,
  __CR_IN__ uint_t      index,
  __CR_OT__ uint_t*     size
    )
{
    sDHCAM_SNAP*    real;

    real = (sDHCAM_SNAP*)dhsnap;
    if (index >= real->count)
        return (NULL);
    if (size != NULL)
        *size = real->psize[index];
    return (real->plate[index]);
}

/*
=======================================
    获取抓拍结果 (整数型)
=======================================
*/
CR_API sint_t
dhnotify_number (
  __CR_IN__ dhnotify_t      dhsnap,
  __CR_IN__ uint_t          index,
  __CR_IN__ const ansi_t*   name
    )
{
    cJSON*          objs;
    sDHCAM_SNAP*    real;

    real = (sDHCAM_SNAP*)dhsnap;
    if (index >= real->count)
        return (-1);
    objs = cJSON_GetArrayItem(real->infos, index);
    if (objs == NULL || objs->type != cJSON_Object)
        return (-1);
    objs = cJSON_GetObjectItem(objs, name);
    if (objs == NULL || objs->type != cJSON_Number)
        return (-1);
    return (objs->valueint);
}

/*
=======================================
    获取抓拍结果 (字符串)
=======================================
*/
CR_API ansi_t*
dhnotify_string (
  __CR_IN__ dhnotify_t      dhsnap,
  __CR_IN__ uint_t          index,
  __CR_IN__ const ansi_t*   name
    )
{
    cJSON*          objs;
    sDHCAM_SNAP*    real;

    real = (sDHCAM_SNAP*)dhsnap;
    if (index >= real->count)
        return (NULL);
    objs = cJSON_GetArrayItem(real->infos, index);
    if (objs == NULL || objs->type != cJSON_Object)
        return (NULL);
    objs = cJSON_GetObjectItem(objs, name);
    if (objs == NULL || objs->type != cJSON_String)
        return (NULL);
    return (objs->valuestring);
}

/*
=======================================
    获取抓拍结果 (颜色型)
=======================================
*/
CR_API sint_t
dhnotify_colour (
  __CR_IN__ dhnotify_t      dhsnap,
  __CR_IN__ uint_t          index,
  __CR_IN__ const ansi_t*   name
    )
{
    cJSON*          objs;
    cJSON*          clrs;
    int32u          vals;
    sint_t          item;
    sDHCAM_SNAP*    real;

    real = (sDHCAM_SNAP*)dhsnap;
    if (index >= real->count)
        return (-1);
    objs = cJSON_GetArrayItem(real->infos, index);
    if (objs == NULL || objs->type != cJSON_Object)
        return (-1);
    objs = cJSON_GetObjectItem(objs, name);
    if (objs == NULL || objs->type != cJSON_Array ||
        cJSON_GetArraySize(objs) != 4)
        return (-1);
    for (vals = 0, item = 0; item < 3; item++) {
        clrs = cJSON_GetArrayItem(objs, item);
        if (clrs == NULL || clrs->type != cJSON_Number)
            return (-1);
        vals <<= 8;
        vals |= (clrs->valueint & 0xFF);
    }
    return ((sint_t)vals);
}

#if defined(DHTEST)

static bool_t s_reopen = FALSE;

/*
---------------------------------------
    出错回调
---------------------------------------
*/
static void_t
onErrors (
  __CR_IN__ dhcamera_t  dhcam,
  __CR_IN__ sint_t      okay
    )
{
    printf("onErrors() = %u\n", okay);

    /* 不能在回调里重连 */
    s_reopen = TRUE;
}

/*
---------------------------------------
    注册回调
---------------------------------------
*/
static void_t
onAttach (
  __CR_IN__ dhcamera_t  dhcam,
  __CR_IN__ sint_t      okay
    )
{
    printf("onAttach() = %u\n", okay);
}

/*
---------------------------------------
    注销回调
---------------------------------------
*/
static void_t
onDetach (
  __CR_IN__ dhcamera_t  dhcam,
  __CR_IN__ sint_t      okay
    )
{
    printf("onDetach() = %u\n", okay);
}

/*
---------------------------------------
    开闸回调
---------------------------------------
*/
static void_t
onStrobe (
  __CR_IN__ dhcamera_t  dhcam,
  __CR_IN__ sint_t      okay
    )
{
    printf("onStrobe() = %u\n", okay);
}

/*
---------------------------------------
    心跳回调
---------------------------------------
*/
static void_t
onKAlive (
  __CR_IN__ dhcamera_t  dhcam,
  __CR_IN__ sint_t      okay
    )
{
    printf("onKAlive() = %u\n", okay);
}

/*
---------------------------------------
    抓拍回调
---------------------------------------
*/
static void_t
onNotify (
  __CR_IN__ dhcamera_t  dhcam,
  __CR_IN__ dhnotify_t  dhsnap
    )
{
    FILE*   fp;
    uint_t  size;
    void_t* data;

    printf("onNotify() = %s\n",dhcamera_addr(dhcam));
    printf("\tConfidence = %d\n",dhnotify_number(dhsnap,0,"Confidence"));
    printf("\tDeviceID = %s\n",dhnotify_string(dhsnap,0,"DeviceID"));
    printf("\tPlateNumber = %s\n",dhnotify_string(dhsnap,0,"PlateNumber"));
    printf("\tPlateType = %s\n",dhnotify_string(dhsnap,0,"PlateType"));
    printf("\tSnapSource = %s\n",dhnotify_string(dhsnap,0,"SnapSource"));
    printf("\tSnapTime = %s\n",dhnotify_string(dhsnap,0,"SnapTime"));
    printf("\tStrobeState = %s\n",dhnotify_string(dhsnap,0,"StrobeState"));
    printf("\tVehicleType = %s\n",dhnotify_string(dhsnap,0,"VehicleType"));
    printf("\tPlateColor = %08X\n",dhnotify_colour(dhsnap,0,"PlateColor"));
    printf("\tVehicleColor = %08X\n",dhnotify_colour(dhsnap,0,"VehicleColor"));
    data = dhnotify_image(dhsnap, &size);
    if (data != NULL) {
        fp = fopen("big.jpg", "wb");
        if (fp != NULL) {
            fwrite(data, size, 1, fp);
            fclose(fp);
        }
    }
    data = dhnotify_plate(dhsnap, 0, &size);
    if (data != NULL) {
        fp = fopen("small.jpg", "wb");
        if (fp != NULL) {
            fwrite(data, size, 1, fp);
            fclose(fp);
        }
    }
}

/*
=======================================
    测试使用
=======================================
*/
int main (int argc, char *argv[])
{
    ansi_t*     key;
    dhcamera_t  dhcam;

    /* 准备参数 */
    if (argc < 2) {
        printf("Usage: DHCamera <IP> [KEY=admin]\n");
        return (-1);
    }
    if (argc >= 3)
        key = argv[2];
    else
        key = "admin";

    /* 开始测试流程 */
    crhack_core_init();
    if (!socket_init()) {
        printf("socket_init() failure!\n");
        return (-1);
    }
    dhcam = dhcamera_open(argv[1], 34567, key);
    if (dhcam == NULL) {
        printf("dhcamera_open() failure!\n");
        return (-1);
    }
    dhcamera_callback(dhcam, "onErrors", (void_t*)onErrors);
    dhcamera_callback(dhcam, "onAttach", (void_t*)onAttach);
    dhcamera_callback(dhcam, "onDetach", (void_t*)onDetach);
    dhcamera_callback(dhcam, "onStrobe", (void_t*)onStrobe);
    dhcamera_callback(dhcam, "onKAlive", (void_t*)onKAlive);
    dhcamera_callback(dhcam, "onNotify", (void_t*)onNotify);
    dhcamera_attach(dhcam);
    for (;;) {
#if defined(_CR_OS_MSWIN_)
        sint_t  retc;

        if (_kbhit()) {
            retc = getch();
            if (retc == 0x1B)
                break;
            if (retc == 'O' || retc == 'o')
                dhcamera_strobe(dhcam, TRUE);
            else
            if (retc == 'C' || retc == 'c')
                dhcamera_strobe(dhcam, FALSE);
            else
            if (retc == 'A' || retc == 'a')
                dhcamera_kalive(dhcam, 60);
        }
#endif
        if (s_reopen) {
            s_reopen = FALSE;
            for (;;) {
                if (dhcamera_reopen(dhcam, "admin") != NULL) {
                    printf("dhcamera_reopen() success!\n");
                    dhcamera_attach(dhcam);
                    break;
                }
                printf("dhcamera_reopen() failure!\n");
                thread_sleep(3000);
            }
        }
        thread_sleep(1);
    }
    dhcamera_detach(dhcam);
    dhcamera_close(dhcam);
    return (0);
}

#endif  /* DHTEST */
