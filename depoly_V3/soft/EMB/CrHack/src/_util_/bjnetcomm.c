/*****************************************************************************/
/*                                                  ###                      */
/*       #####          ###    ###                  ###  CREATE: 2016-04-05  */
/*     #######          ###    ###      [UTIL]      ###  ~~~~~~~~~~~~~~~~~~  */
/*    ########          ###    ###                  ###  MODIFY: XXXX-XX-XX  */
/*    ####  ##          ###    ###                  ###  ~~~~~~~~~~~~~~~~~~  */
/*   ###       ### ###  ###    ###    ####    ####  ###   ##  +-----------+  */
/*  ####       ######## ##########  #######  ###### ###  ###  |  A NEW C  |  */
/*  ###        ######## ########## ########  ###### ### ###   | FRAMEWORK |  */
/*  ###     ## #### ### ########## ###  ### ###     ######    |  FOR ALL  |  */
/*  ####   ### ###  ### ###    ### ###  ### ###     ######    | PLATFORMS |  */
/*  ########## ###      ###    ### ######## ####### #######   |  AND ALL  |  */
/*   #######   ###      ###    ### ########  ###### ###  ###  | COMPILERS |  */
/*    #####    ###      ###    ###  #### ##   ####  ###   ##  +-----------+  */
/*  =======================================================================  */
/*  >>>>>>>>>>>>>>>>>>>>>>> BlowJobs 通讯协议函数库 <<<<<<<<<<<<<<<<<<<<<<<  */
/*  =======================================================================  */
/*****************************************************************************/

#include "hash.h"
#include "applib.h"
#include "crypto.h"
#include "enclib.h"
#include "memlib.h"
#include "morder.h"
#include "strlib.h"
#include "util/bjnetcomm.h"

/* 默认的 TOKEN 参数 */
#define BJNETCOMM_TOKEN_LEN 32
#define BJNETCOMM_TOKEN_DEF "&Key="
#define BJNETCOMM_CSIZE_MAX CR_M2B(16)
#define BJNETCOMM_CSIZE_DEF CR_K2B(128)

/* BJNetComm 内部结构 */
typedef struct
{
        byte_t  seckey[40];
        int32u  fuzzy, size_max;
        ansi_t  token[BJNETCOMM_TOKEN_LEN + 2];

} sBJNetComm;

/*
=======================================
    BJNetComm 初始化
=======================================
*/
CR_API bjnetcomm_t
bjnetcomm_init (void_t)
{
    sBJNetComm* bjnet;

    bjnet = struct_new(sBJNetComm);
    if (bjnet == NULL)
        return (NULL);
    bjnetcomm_reset(bjnet);
    bjnet->size_max = BJNETCOMM_CSIZE_DEF;
    return ((bjnetcomm_t)bjnet);
}

/*
=======================================
    BJNetComm 释放
=======================================
*/
CR_API void_t
bjnetcomm_kill (
  __CR_IN__ bjnetcomm_t bjnet
    )
{
    struct_zero(bjnet, sBJNetComm);
    mem_free(bjnet);
}

/*
=======================================
    BJNetComm 重置
=======================================
*/
CR_API void_t
bjnetcomm_reset (
  __CR_IN__ bjnetcomm_t bjnet
    )
{
    bjnetcomm_update_token(bjnet, BJNETCOMM_TOKEN_DEF);
}

/*
=======================================
    BJNetComm 最大数据包
=======================================
*/
CR_API void_t
bjnetcomm_size_max (
  __CR_IN__ bjnetcomm_t bjnet,
  __CR_IN__ int32u      size_kb
    )
{
    sBJNetComm* real = (sBJNetComm*)bjnet;

    real->size_max = (int32u)CR_K2B(size_kb);
    if (real->size_max > BJNETCOMM_CSIZE_MAX)
        real->size_max = BJNETCOMM_CSIZE_MAX;
}

/*
=======================================
    BJNetComm 更新令牌
=======================================
*/
CR_API void_t
bjnetcomm_update_token (
  __CR_IO__ bjnetcomm_t     bjnet,
  __CR_IN__ const ansi_t*   token
    )
{
    uint_t      idx;
    int32u      crc32;
    sBJNetComm* real = (sBJNetComm*)bjnet;

    hash_sha1_total(real->seckey, token, str_lenA(token));
    crc32 = hash_crc32i_total(real->seckey, 20);
    crc32 = DWORD_BE(crc32) ^ 0x55AA55AA;
    mem_cpy(&real->seckey[20], &crc32, sizeof(crc32));
    hash_md5_total(&real->seckey[24], real->seckey, 24);
    hex2strA(real->token, &real->seckey[12], 16);
    for (idx = 0; idx < sizeof(real->seckey); idx++)
        real->seckey[idx] ^= 0xAA;
    real->fuzzy = timer_get32() ^ 0xAA55AA55;
}

/*
---------------------------------------
    BJNetComm 计算校验
---------------------------------------
*/
static void_t
bjnetcomm_hash_data (
  __CR_OT__ byte_t              hash[20],
  __CR_IN__ const sBJNetComm*   bjnet,
  __CR_IN__ const void_t*       data,
  __CR_IN__ leng_t              size,
  __CR_IN__ uint_t              type,
  __CR_IN__ bool_t              nofuzzy
    )
{
    uint_t  len;
    int32u  t1, t2;

    union {
        sMD5    md5;
        sSHA1   sha;
    } ctx;

    len = (uint_t)str_lenA(BJNETCOMM_TOKEN_DEF);
    if (type & 1) {
        hash_sha1_init(&ctx.sha);
        hash_sha1_update(&ctx.sha, data, size);
        hash_sha1_update(&ctx.sha, BJNETCOMM_TOKEN_DEF, len);
        hash_sha1_update(&ctx.sha, bjnet->token, BJNETCOMM_TOKEN_LEN);
        hash_sha1_finish(hash, &ctx.sha);
    }
    else {
        hash_md5_init(&ctx.md5);
        hash_md5_update(&ctx.md5, data, size);
        hash_md5_update(&ctx.md5, BJNETCOMM_TOKEN_DEF, len);
        hash_md5_update(&ctx.md5, bjnet->token, BJNETCOMM_TOKEN_LEN);
        hash_md5_finish(&hash[4], &ctx.md5);
        if (!nofuzzy) {
            t1 = hash_crc32i_total(&bjnet->fuzzy, sizeof(bjnet->fuzzy));
            mem_cpy(hash, &t1, sizeof(int32u));
        }
    }
    if ((type >> 1) & 1) {
        t1 = 0x7C53F961;
        for (len = 0; len < 20; len++) {
            t1 *= 0x3D09;
            t2  = t1 >> 0x10;
            hash[len] ^= (byte_t)t2;
        }
    }
    else {
        for (len = 0; len < 20; len++)
            hash[len] ^= 0x55;
    }
}

/*
---------------------------------------
    BJNetComm 校验检查
---------------------------------------
*/
static bool_t
bjnetcomm_hash_check (
  __CR_IN__ const sBJNetComm*   bjnet,
  __CR_IN__ const void_t*       data,
  __CR_IN__ leng_t              size,
  __CR_IN__ uint_t              type,
  __CR_IN__ const byte_t        hash[20]
    )
{
    byte_t  dhash[20];

    bjnetcomm_hash_data(dhash, bjnet, data, size, type, TRUE);
    if (type & 1) {
        if (mem_cmp(dhash, hash, 20) == 0)
            return (TRUE);
    }
    else {
        if (mem_cmp(&dhash[4], &hash[4], 16) == 0)
            return (TRUE);
    }
    return (FALSE);
}

/*
---------------------------------------
    BJNetComm 数据加密
---------------------------------------
*/
static ansi_t*
bjnetcomm_data_encode (
  __CR_IN__ const sBJNetComm*   bjnet,
  __CR_IN__ const void_t*       data,
  __CR_IN__ leng_t              size,
  __CR_IN__ uint_t              type,
  __CR_OT__ leng_t*             osize,
  __CR_IN__ int16u              fuzzy
    )
{
    leng_t  dsize;
    int32u  t1, t2;
    byte_t* buffer;
    ansi_t* finals;
    leng_t  idx, blk;
#if !defined(_BJNETCOMM_NO_RC4_) && \
    !defined(_BJNETCOMM_NO_AES_)
    union {
        sAES    aes;
        sRC4    rc4;
    } ctx;
#endif
    /* MAX = 31 */
    switch ((type & 0x7F) >> 2)
    {
        default:
            return (NULL);

        case 0:     /* NO CRYPTO */
            dsize = size;
            buffer = (byte_t*)mem_dup(data, size);
            if (buffer == NULL)
                return (NULL);
            break;

        case 1:     /* SIMPLE CHANGE */
            dsize = size;
            buffer = (byte_t*)mem_dup(data, size);
            if (buffer == NULL)
                return (NULL);
            t1 = 0x7C53F961;
            for (idx = 0; idx < dsize; idx++) {
                t1 *= 0x3D09;
                t2  = t1 >> 0x10;
                buffer[idx] = buffer[idx] + (byte_t)t2;
            }
            break;
#if !defined(_BJNETCOMM_NO_TEA_)
        case 2:     /* TEA(16) */
            dsize = size;
            buffer = (byte_t*)mem_dup(data, size);
            if (buffer == NULL)
                return (NULL);
            blk = dsize / 8;
            for (idx = 0; blk != 0; blk--, idx += 8) {
                crypto_tea_enc((int32u*)(&buffer[idx]),
                               (int32u*)bjnet->seckey, 16);
            }
            break;
#endif
#if !defined(_BJNETCOMM_NO_RC4_)
        case 3:     /* RC4(2) */
            dsize = size;
            buffer = (byte_t*)mem_dup(data, size);
            if (buffer == NULL)
                return (NULL);
            crypto_rc4_key(&ctx.rc4, bjnet->seckey, sizeof(bjnet->seckey));
            crypto_rc4_ops(&ctx.rc4, buffer, dsize);
            crypto_rc4_ops(&ctx.rc4, buffer, dsize);
            break;
#endif
#if !defined(_BJNETCOMM_NO_AES_)
        case 4:     /* AES(128)/EBC/PADDING7 */
            dsize = ((size / 16) + 1) * 16;
            buffer = (byte_t*)mem_malloc(dsize);
            if (buffer == NULL)
                return (NULL);
            mem_cpy(buffer, data, size);
            t1 = (int32u)(dsize - size);
            for (idx = size; idx < dsize; idx++)
                buffer[idx] = (byte_t)t1;
            blk = dsize / 16;
            crypto_aes_key(&ctx.aes, bjnet->seckey, 16);
            for (idx = 0; blk != 0; blk--, idx += 16) {
                crypto_aes_enc(&ctx.aes, (int32u*)(&buffer[idx]),
                                         (int32u*)(&buffer[idx]));
            }
            break;
#endif
#if !defined(_BJNETCOMM_NO_BZ2_)
        case 5:     /* BZIP2 */
            if (size >= bjnet->size_max)
                return (NULL);
            dsize = compr_bzip2(NULL, 0, data, size, 9);
            if (dsize == 0)
                return (NULL);
            buffer = (byte_t*)mem_malloc(dsize + sizeof(t1));
            if (buffer == NULL)
                return (NULL);
            t1 = DWORD_BE((int32u)size);
            mem_cpy(buffer, &t1, sizeof(t1));
            dsize = compr_bzip2(buffer + sizeof(t1), dsize, data, size, 9);
            if (dsize == 0)
                goto _failure;
            dsize += sizeof(t1);
            break;
#endif
    }

    /* FUZZY */
    t1 = (int32u)(fuzzy >> 8);
    t2 = (int32u)(fuzzy & 255);
    for (blk = 0; blk < dsize; blk++) {
        buffer[blk] = buffer[blk] ^ (byte_t)t1;
        buffer[blk] = buffer[blk] + (byte_t)t2;
    }

    /* BASE64 */
    size = encode_base64(NULL, 0, buffer, dsize, 0);
    if (size == 0)
        goto _failure;
    finals = str_allocA(size + 1);
    if (finals == NULL)
        goto _failure;
    dsize = encode_base64(finals, size, buffer, dsize, 0);
    if (type & 0x80)
        encode_base64url(finals, dsize);
    finals[dsize] = 0x00;
    mem_free(buffer);
    if (dsize == 0) {
        mem_free(finals);
        return (NULL);
    }

    /* RETURN */
    if (osize != NULL)
        *osize = dsize;
    return (finals);

_failure:
    mem_free(buffer);
    return (NULL);
}

/*
---------------------------------------
    BJNetComm 数据解密
---------------------------------------
*/
static void_t*
bjnetcomm_data_decode (
  __CR_IN__ const sBJNetComm*   bjnet,
  __CR_IN__ const void_t*       data,
  __CR_IN__ leng_t              size,
  __CR_IN__ uint_t              type,
  __CR_OT__ leng_t*             osize,
  __CR_IN__ int16u              fuzzy
    )
{
    leng_t  dsize;
    int32u  t1, t2;
    byte_t* buffer;
#if !defined(_BJNETCOMM_NO_BZ2_)
    byte_t* uncomp;
#endif
    leng_t  idx, blk;
#if !defined(_BJNETCOMM_NO_RC4_) && \
    !defined(_BJNETCOMM_NO_AES_)
    union {
        sAES    aes;
        sRC4    rc4;
    } ctx;
#endif
    /* BASE64 */
    buffer = (byte_t*)mem_dup(data, size);
    if (buffer == NULL)
        return (NULL);
    if (type & 0x80)
        decode_base64url(buffer, size);
    size = decode_base64(buffer, size, buffer, size);
    if (size == 0)
        goto _failure;

    /* FUZZY */
    t1 = (int32u)(fuzzy >> 8);
    t2 = (int32u)(fuzzy & 255);
    for (blk = 0; blk < size; blk++) {
        buffer[blk] = buffer[blk] - (byte_t)t2;
        buffer[blk] = buffer[blk] ^ (byte_t)t1;
    }

    /* MAX = 31 */
    switch ((type & 0x7F) >> 2)
    {
        default:
            goto _failure;

        case 0:     /* NO CRYPTO */
            dsize = size;
            break;

        case 1:     /* SIMPLE CHANGE */
            dsize = size;
            t1 = 0x7C53F961;
            for (idx = 0; idx < dsize; idx++) {
                t1 *= 0x3D09;
                t2  = t1 >> 0x10;
                buffer[idx] = buffer[idx] - (byte_t)t2;
            }
            break;
#if !defined(_BJNETCOMM_NO_TEA_)
        case 2:     /* TEA(16) */
            dsize = size;
            blk = dsize / 8;
            for (idx = 0; blk != 0; blk--, idx += 8) {
                crypto_tea_dec((int32u*)(&buffer[idx]),
                               (int32u*)bjnet->seckey, 16);
            }
            break;
#endif
#if !defined(_BJNETCOMM_NO_RC4_)
        case 3:     /* RC4(2) */
            dsize = size;
            crypto_rc4_key(&ctx.rc4, bjnet->seckey, sizeof(bjnet->seckey));
            crypto_rc4_ops(&ctx.rc4, buffer, dsize);
            crypto_rc4_ops(&ctx.rc4, buffer, dsize);
            break;
#endif
#if !defined(_BJNETCOMM_NO_AES_)
        case 4:     /* AES(128)/EBC/PADDING7 */
            if (size % 16 != 0)
                goto _failure;
            blk = size / 16;
            crypto_aes_key(&ctx.aes, bjnet->seckey, 16);
            for (idx = 0; blk != 0; blk--, idx += 16) {
                crypto_aes_dec(&ctx.aes, (int32u*)(&buffer[idx]),
                                         (int32u*)(&buffer[idx]));
            }
            idx -= 1;
            if (buffer[idx] == 0 ||
                buffer[idx] > 16 || (leng_t)buffer[idx] >= size)
                goto _failure;
            dsize = size - buffer[idx];
            break;
#endif
#if !defined(_BJNETCOMM_NO_BZ2_)
        case 5:     /* BZIP2 */
            if (size <= sizeof(t1))
                goto _failure;
            mem_cpy(&t1, buffer, sizeof(t1));
            dsize = (leng_t)DWORD_BE(t1);
            if (dsize >= bjnet->size_max)
                goto _failure;
            uncomp = (byte_t*)mem_malloc(dsize);
            if (uncomp == NULL)
                goto _failure;
            size -= sizeof(t1);
            dsize = uncompr_bzip2(uncomp, dsize, buffer + sizeof(t1), size);
            if (dsize == 0) {
                mem_free(uncomp);
                goto _failure;
            }
            mem_free(buffer);
            buffer = uncomp;
            break;
#endif
    }

    /* RETURN */
    if (osize != NULL)
        *osize = dsize;
    return (buffer);

_failure:
    mem_free(buffer);
    return (NULL);
}

/*
=======================================
    BJNetComm 编码字节组
=======================================
*/
CR_API ansi_t*
bjnetcomm_enc_bytes (
  __CR_IN__ bjnetcomm_t     bjnet,
  __CR_IN__ uint_t          type,
  __CR_IN__ const void_t*   data,
  __CR_IN__ uint_t          size,
  __CR_IN__ const ansi_t*   user
    )
{
    int32u  fuzzy;
    leng_t  dsize;
    ansi_t* finals;
    ansi_t* buffer;
    byte_t  hash[20];
    ansi_t  shash[41];
    /* ------------ */
    sBJNetComm* real = (sBJNetComm*)bjnet;

    fuzzy = (int16u)real->fuzzy;
    if (fuzzy == 0) fuzzy = 0x7E81;
    real->fuzzy = real->fuzzy * 214013 + 2531011;
    type = ((type & 0x3F) << 2) | (real->fuzzy & 3);
    bjnetcomm_hash_data(hash, real, data, size, type, FALSE);
    buffer = bjnetcomm_data_encode(real, data, size, type,
                        &dsize, (int16u)fuzzy);
    if (buffer == NULL)
        return (NULL);
    fuzzy = (fuzzy << 8) | (fuzzy & 255);
    type = ((type & 0xFF) << 8) ^ fuzzy;
    hex2strA(shash, hash, 20);
    fuzzy = timer_get32();
    if (user != NULL) {
        finals = str_fmtA("{\"type\":%u,\"data\":\"%s\",\"sign\":\"%s\","
                           "\"tick\":%u,%s}", type, buffer, shash, fuzzy,
                                              user);
    }
    else {
        finals = str_fmtA("{\"type\":%u,\"data\":\"%s\",\"sign\":\"%s\","
                           "\"tick\":%u}", type, buffer, shash, fuzzy);
    }
    mem_free(buffer);
    return (finals);
}

/*
=======================================
    BJNetComm 编码字符串
=======================================
*/
CR_API ansi_t*
bjnetcomm_enc_string (
  __CR_IN__ bjnetcomm_t     bjnet,
  __CR_IN__ uint_t          type,
  __CR_IN__ const ansi_t*   string,
  __CR_IN__ const ansi_t*   user
    )
{
    return (bjnetcomm_enc_bytes(bjnet, type, string,
                (uint_t)str_lenA(string), user));
}

/*
=======================================
    BJNetComm 解码字节组 (高级)
=======================================
*/
CR_API void_t*
bjnetcomm_dec_bytes2 (
  __CR_IN__ bjnetcomm_t     bjnet,
  __CR_IN__ uint_t          tag_type,
  __CR_IN__ const ansi_t*   tag_data,
  __CR_IN__ const ansi_t*   tag_sign,
  __CR_OT__ uint_t*         size
    )
{
    leng_t  dsize;
    int16u  fuzzy;
    leng_t  hsize;
    void_t* buffer;
    byte_t  hash[20];
    /* ----------- */
    sBJNetComm* real = (sBJNetComm*)bjnet;

    fuzzy = (int16u)(((tag_type >> 8) & 0xFF00) | (tag_type & 0xFF));
    tag_type = ((tag_type >> 8) & 255) ^ (byte_t)fuzzy;
    buffer = bjnetcomm_data_decode(real, tag_data, (uint_t)str_lenA(tag_data),
                                   tag_type, &dsize, fuzzy);
    if (buffer == NULL)
        return (NULL);
    hsize = sizeof(hash);
    str2datA(hash, &hsize, tag_sign);
    if (hsize != sizeof(hash))
        goto _failure;
    if (!bjnetcomm_hash_check(real, buffer, dsize, tag_type, hash))
        goto _failure;
    if (size != NULL)
        *size = (uint_t)dsize;
    return (buffer);

_failure:
    mem_free(buffer);
    return (NULL);
}

/*
=======================================
    BJNetComm 解码字节组
=======================================
*/
CR_API void_t*
bjnetcomm_dec_bytes (
  __CR_IN__ bjnetcomm_t     bjnet,
  __CR_IN__ const ansi_t*   json,
  __CR_OT__ uint_t*         size
    )
{
    leng_t  skip;
    uint_t  type;
    ansi_t* temp;
    ansi_t* data;
    ansi_t* sign;
    void_t* final;

    /* type */
    temp = str_strA(json, "\"type\"");
    if (temp == NULL)
        return (NULL);
    temp = skip_spaceA(temp + 6);
    if (*temp != CR_AC(':'))
        return (NULL);
    temp = skip_spaceA(temp + 1);
    type = str2intA(temp, &skip);
    if ((sint_t)type <= 0 || skip == 0)
        return (NULL);

    /* data */
    temp = str_strA(json, "\"data\"");
    if (temp == NULL)
        return (NULL);
    temp = skip_spaceA(temp + 6);
    if (*temp != CR_AC(':'))
        return (NULL);
    temp = skip_spaceA(temp + 1);
    if (*temp != CR_AC('\"'))
        return (NULL);
    data = str_esc_dupU(temp, NULL, NULL);
    if (data == NULL)
        return (NULL);

    /* sign */
    temp = str_strA(json, "\"sign\"");
    if (temp == NULL)
        goto _failure;
    temp = skip_spaceA(temp + 6);
    if (*temp != CR_AC(':'))
        goto _failure;
    temp = skip_spaceA(temp + 1);
    if (*temp != CR_AC('\"'))
        goto _failure;
    sign = str_esc_dupU(temp, NULL, NULL);
    if (sign == NULL)
        goto _failure;
    if (str_lenA(sign) != 40) {
        mem_free(sign);
        goto _failure;
    }

    /* DECODE */
    final = bjnetcomm_dec_bytes2(bjnet, type, data, sign, size);
    mem_free(sign);
    mem_free(data);
    return (final);

_failure:
    mem_free(data);
    return (NULL);
}

/*
=======================================
    BJNetComm 解码字符串
=======================================
*/
CR_API ansi_t*
bjnetcomm_dec_string (
  __CR_IN__ bjnetcomm_t     bjnet,
  __CR_IN__ const ansi_t*   json
    )
{
    uint_t  size;
    void_t* buffer;
    ansi_t* finals;

    buffer = bjnetcomm_dec_bytes(bjnet, json, &size);
    if (buffer == NULL)
        return (NULL);
    if (size == 0)
        goto _failure;
    finals = str_allocA(size + 2);
    if (finals == NULL)
        goto _failure;
    mem_cpy(finals, buffer, size);
    finals[size + 0] = 0x00;
    finals[size + 1] = 0x00;
    mem_free(buffer);
    return (finals);

_failure:
    mem_free(buffer);
    return (NULL);
}

/*****************************************************************************/
/* _________________________________________________________________________ */
/* uBMAzRBoAKAHaACQD6FoAIAPqbgA/7rIA+5CM9uKw8D4Au7u7mSIJ0t18mYz0mYz9rAQZCgHc */
/* wRIZIgHZovGBXUAZg+v0GbB+gRmgcJ7BAAAisIlAwB1Av7LSHUC/s9IdQL+w0h1Av7HZkZmgf */
/* 4JLgIAdb262gPsqAh0+zP/uQB9ZYsFZYktq+L3sMi/AAK7gAKJAUtLdfq5IANXvT8BiQzfBIv */
/* FLaAAweAEmff53wb+Adjx3kQE2xwy5Io8ithkigcFgACJBN8E3sneNvwB2xyLHDkdfA2JHSyA */
/* adtAAQPdZYgHR0dNdbZfSYP5UHWr/kQEtAHNFg+Eef/DWAKgDw== |~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ >>> END OF FILE <<< */
/*****************************************************************************/
