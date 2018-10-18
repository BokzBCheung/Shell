/*
 *********************************
 *  BJResLib 公共库
 *
 *  创建时间: 2016-04-18
 *********************************
 */

#include "BJResLib.h"

#include <stdio.h>

/* BJResLib 导入库 */
#ifndef _CR_NO_PRAGMA_LIB_
    #pragma comment (lib, "COREs.lib")
#endif

/*
=======================================
    加载密钥文件
=======================================
*/
CR_API bool_t
load_rsa_key (
  __CR_OT__ sRSA*           key,
  __CR_IN__ const ansi_t*   name
    )
{
    leng_t      cnt;
    ansi_t*     str;
    ansi_t**    lst;

    str = file_load_as_strA(name);
    if (str == NULL)
        return (FALSE);
    lst = str_splitA(str, ',', &cnt);
    if (lst == NULL)
        goto _failure1;
    if (cnt < 4)
        goto _failure2;
    key->block = str2intxA(lst[0], NULL);
    if (key->block == 0 || key->block % 4 != 0)
        goto _failure2;
    key->split = str2intxA(lst[1], NULL);
    if (key->split == 0 || key->split > key->block)
        goto _failure2;
    bigint_from(&key->N, lst[2], 0, 16);
    bigint_from(&key->E, lst[3], 0, 16);
    mem_free(lst);
    mem_free(str);
    return (TRUE);

_failure2:
    mem_free(lst);
_failure1:
    mem_free(str);
    return (FALSE);
}

/*
=======================================
    保存密钥文件
=======================================
*/
CR_API bool_t
save_rsa_key (
  __CR_IN__ const sRSA*     key,
  __CR_IN__ const ansi_t*   name
    )
{
    FILE*   fp;
    leng_t  sz;
    ansi_t* nn;
    ansi_t* ee;

    sz = key->block * 2 + 1;
    nn = str_allocA(sz);
    if (nn == NULL)
        return (FALSE);
    ee = str_allocA(sz);
    if (ee == NULL)
        goto _failure1;
    if (bigint_to(&key->N, nn, sz, 16) == 0)
        goto _failure2;
    if (bigint_to(&key->E, ee, sz, 16) == 0)
        goto _failure2;
    fp = fopen(name, "w");
    if (fp == NULL)
        goto _failure2;
    fprintf(fp, "%u,%u,%s,%s", key->block, key->split, nn, ee);
    fclose(fp);
    mem_free(ee);
    mem_free(nn);
    return (TRUE);

_failure2:
    mem_free(ee);
_failure1:
    mem_free(nn);
    return (FALSE);
}

/*
=======================================
    计算数据哈希
=======================================
*/
CR_API uint_t
hash_doit (
  __CR_OT__ byte_t*         hash,
  __CR_IN__ const void_t*   data,
  __CR_IN__ leng_t          size,
  __CR_IN__ uint_t          type
    )
{
    int32u  crc32;
    int64u  crc64;

    switch (type)
    {
        default:
        case 0:     /* MD5 */
            hash_md5_total(hash, data, size);
            type = 16;
            break;

        case 1:     /* SHA-1 */
            hash_sha1_total(hash, data, size);
            type = 20;
            break;

        case 2:     /* SHA-224 */
            hash_sha224_total(hash, data, size);
            type = 28;
            break;

        case 3:     /* SHA-256 */
            hash_sha256_total(hash, data, size);
            type = 32;
            break;

        case 4:     /* SHA-384 */
            hash_sha384_total(hash, data, size);
            type = 48;
            break;

        case 5:     /* SHA-512 */
            hash_sha512_total(hash, data, size);
            type = 64;
            break;

        case 6:     /* CRC-32 */
            crc32 = hash_crc32i_total(data, size);
            crc32 = DWORD_BE(crc32);
            mem_cpy(hash, &crc32, sizeof(crc32));
            type = 4;
            break;

        case 7:     /* CRC-64 */
            crc64 = hash_crc64e_total(data, size);
            crc64 = QWORD_BE(crc64);
            mem_cpy(hash, &crc64, sizeof(crc64));
            type = 8;
            break;

        case 8:     /* MD2 */
            hash_md2_total(hash, data, size);
            type = 16;
            break;

        case 9:     /* MD4 */
            hash_md4_total(hash, data, size);
            type = 16;
            break;

        case 10:    /* RMD-128 */
            hash_rmd128_total(hash, data, size);
            type = 16;
            break;

        case 11:    /* RMD-160 */
            hash_rmd160_total(hash, data, size);
            type = 20;
            break;

        case 12:    /* RMD-256 */
            hash_rmd256_total(hash, data, size);
            type = 32;
            break;

        case 13:    /* RMD-320 */
            hash_rmd320_total(hash, data, size);
            type = 40;
            break;
    }
    return (type);
}

/*
=======================================
    文件数据加密
=======================================
*/
CR_API void_t
crypt_encode (
  __CR_IO__ byte_t*         data,
  __CR_IN__ leng_t          size,
  __CR_IN__ const ansi_t*   skey,
  __CR_IN__ uint_t          type
    )
{
    union {
        sAES    aes;
        sANUBIS anubis;
        sBF     bf;
        sCAST5  cast5;
        sCAST6  cast6;
        s3DES   des3;
        sRC2    rc2;
        sRC4    rc4;
        sTF     tf;
    } ctx;

    leng_t  idx;
    byte_t  key[16];

    /* 密码学加密 */
    hash_md5_total(key, skey, str_lenA(skey));
    switch (type)
    {
        default:
        case 0:     /* BLOWFISH */
            crypto_bfish_key(&ctx.bf, key, sizeof(key));
            for (idx = 0; idx < size / 8; idx++) {
                crypto_bfish_enc(&ctx.bf, (int32u*)(&data[idx * 8]),
                                          (int32u*)(&data[idx * 8]));
            }
            break;

        case 1:     /* AES */
            crypto_aes_key(&ctx.aes, key, sizeof(key));
            for (idx = 0; idx < size / 16; idx++) {
                crypto_aes_enc(&ctx.aes, (int32u*)(&data[idx * 16]),
                                         (int32u*)(&data[idx * 16]));
            }
            break;

        case 2:     /* ANUBIS */
            crypto_anubis_key(&ctx.anubis, key, sizeof(key));
            for (idx = 0; idx < size / 16; idx++) {
                crypto_anubis_enc(&ctx.anubis, (int32u*)(&data[idx * 16]),
                                               (int32u*)(&data[idx * 16]));
            }
            break;

        case 3:     /* CAST5 */
            crypto_cast5_key(&ctx.cast5, key, sizeof(key));
            for (idx = 0; idx < size / 8; idx++) {
                crypto_cast5_enc(&ctx.cast5, (int32u*)(&data[idx * 8]),
                                             (int32u*)(&data[idx * 8]));
            }
            break;

        case 4:     /* CAST6 */
            crypto_cast6_key(&ctx.cast6, key, sizeof(key));
            for (idx = 0; idx < size / 16; idx++) {
                crypto_cast6_enc(&ctx.cast6, (int32u*)(&data[idx * 16]),
                                             (int32u*)(&data[idx * 16]));
            }
            break;

        case 5:     /* DES */
            crypto_des_key(&ctx.des3.ek1, key, FALSE);
            for (idx = 0; idx < size / 8; idx++) {
                crypto_des_ops(&ctx.des3.ek1, (byte_t*)(&data[idx * 8]),
                                              (byte_t*)(&data[idx * 8]));
            }
            break;

        case 6:     /* 3DES */
            crypto_3des_key2(&ctx.des3, key);
            for (idx = 0; idx < size / 8; idx++) {
                crypto_3des_enc1(&ctx.des3, (byte_t*)(&data[idx * 8]),
                                            (byte_t*)(&data[idx * 8]));
            }
            break;

        case 7:     /* IDEA */
            for (idx = 0; idx < size / 8; idx++) {
                crypto_idea_enc((int16u*)(&data[idx * 8]),
                                (int16u*)(&data[idx * 8]), (int16u*)key);
            }
            break;

        case 8:     /* RC2 */
            crypto_rc2_key(&ctx.rc2, key, sizeof(key), 0);
            for (idx = 0; idx < size / 8; idx++) {
                crypto_rc2_enc(&ctx.rc2, (byte_t*)(&data[idx * 8]),
                                         (byte_t*)(&data[idx * 8]));
            }
            break;

        case 9:     /* RC4 */
            crypto_rc4_key(&ctx.rc4, key, sizeof(key));
            crypto_rc4_ops(&ctx.rc4, data, size);
            crypto_rc4_ops(&ctx.rc4, data, size);
            break;

        case 10:    /* TEA */
            for (idx = 0; idx < size / 8; idx++)
                crypto_tea_enc((int32u*)(&data[idx * 8]), (int32u*)key, 16);
            break;

        case 11:    /* TWOFISH */
            crypto_tfish_key(&ctx.tf, key, sizeof(key));
            for (idx = 0; idx < size / 16; idx++) {
                crypto_tfish_enc(&ctx.tf, (int32u*)(&data[idx * 16]),
                                          (int32u*)(&data[idx * 16]));
            }
            break;

        case 12:    /* XTEA */
            for (idx = 0; idx < size / 8; idx++)
                crypto_xtea_enc((int32u*)(&data[idx * 8]), (int32u*)key, 16);
            break;

        case 13:    /* XXTEA */
            for (idx = 0; idx < size / 16; idx++)
                crypto_xxtea_enc((int32u*)(&data[idx * 16]), 4, (int32u*)key);
            break;
    }

    /* 混淆加密 */
    for (idx = 0; idx < size; idx++) {
        data[idx] = rotl_byte_t(data[idx], 5);
        data[idx] = data[idx] ^ 0xAA;
    }
}

/*
=======================================
    文件数据解密
=======================================
*/
CR_API void_t
crypt_decode (
  __CR_IO__ byte_t*         data,
  __CR_IN__ leng_t          size,
  __CR_IN__ const ansi_t*   skey,
  __CR_IN__ uint_t          type
    )
{
    union {
        sAES    aes;
        sANUBIS anubis;
        sBF     bf;
        sCAST5  cast5;
        sCAST6  cast6;
        s3DES   des3;
        sRC2    rc2;
        sRC4    rc4;
        sTF     tf;
    } ctx;

    leng_t  idx;
    byte_t  key[16];

    /* 混淆解密 */
    for (idx = 0; idx < size; idx++) {
        data[idx] = data[idx] ^ 0xAA;
        data[idx] = rotr_byte_t(data[idx], 5);
    }

    /* 密码学解密 */
    hash_md5_total(key, skey, str_lenA(skey));
    switch (type)
    {
        default:
        case 0:     /* BLOWFISH */
            crypto_bfish_key(&ctx.bf, key, sizeof(key));
            for (idx = 0; idx < size / 8; idx++) {
                crypto_bfish_dec(&ctx.bf, (int32u*)(&data[idx * 8]),
                                          (int32u*)(&data[idx * 8]));
            }
            break;

        case 1:     /* AES */
            crypto_aes_key(&ctx.aes, key, sizeof(key));
            for (idx = 0; idx < size / 16; idx++) {
                crypto_aes_dec(&ctx.aes, (int32u*)(&data[idx * 16]),
                                         (int32u*)(&data[idx * 16]));
            }
            break;

        case 2:     /* ANUBIS */
            crypto_anubis_key(&ctx.anubis, key, sizeof(key));
            for (idx = 0; idx < size / 16; idx++) {
                crypto_anubis_dec(&ctx.anubis, (int32u*)(&data[idx * 16]),
                                               (int32u*)(&data[idx * 16]));
            }
            break;

        case 3:     /* CAST5 */
            crypto_cast5_key(&ctx.cast5, key, sizeof(key));
            for (idx = 0; idx < size / 8; idx++) {
                crypto_cast5_dec(&ctx.cast5, (int32u*)(&data[idx * 8]),
                                             (int32u*)(&data[idx * 8]));
            }
            break;

        case 4:     /* CAST6 */
            crypto_cast6_key(&ctx.cast6, key, sizeof(key));
            for (idx = 0; idx < size / 16; idx++) {
                crypto_cast6_dec(&ctx.cast6, (int32u*)(&data[idx * 16]),
                                             (int32u*)(&data[idx * 16]));
            }
            break;

        case 5:     /* DES */
            crypto_des_key(&ctx.des3.ek1, key, TRUE);
            for (idx = 0; idx < size / 8; idx++) {
                crypto_des_ops(&ctx.des3.ek1, (byte_t*)(&data[idx * 8]),
                                              (byte_t*)(&data[idx * 8]));
            }
            break;

        case 6:     /* 3DES */
            crypto_3des_key2(&ctx.des3, key);
            for (idx = 0; idx < size / 8; idx++) {
                crypto_3des_dec1(&ctx.des3, (byte_t*)(&data[idx * 8]),
                                            (byte_t*)(&data[idx * 8]));
            }
            break;

        case 7:     /* IDEA */
            for (idx = 0; idx < size / 8; idx++) {
                crypto_idea_dec((int16u*)(&data[idx * 8]),
                                (int16u*)(&data[idx * 8]), (int16u*)key);
            }
            break;

        case 8:     /* RC2 */
            crypto_rc2_key(&ctx.rc2, key, sizeof(key), 0);
            for (idx = 0; idx < size / 8; idx++) {
                crypto_rc2_dec(&ctx.rc2, (byte_t*)(&data[idx * 8]),
                                         (byte_t*)(&data[idx * 8]));
            }
            break;

        case 9:     /* RC4 */
            crypto_rc4_key(&ctx.rc4, key, sizeof(key));
            crypto_rc4_ops(&ctx.rc4, data, size);
            crypto_rc4_ops(&ctx.rc4, data, size);
            break;

        case 10:    /* TEA */
            for (idx = 0; idx < size / 8; idx++)
                crypto_tea_dec((int32u*)(&data[idx * 8]), (int32u*)key, 16);
            break;

        case 11:    /* TWOFISH */
            crypto_tfish_key(&ctx.tf, key, sizeof(key));
            for (idx = 0; idx < size / 16; idx++) {
                crypto_tfish_dec(&ctx.tf, (int32u*)(&data[idx * 16]),
                                          (int32u*)(&data[idx * 16]));
            }
            break;

        case 12:    /* XTEA */
            for (idx = 0; idx < size / 8; idx++)
                crypto_xtea_dec((int32u*)(&data[idx * 8]), (int32u*)key, 16);
            break;

        case 13:    /* XXTEA */
            for (idx = 0; idx < size / 16; idx++)
                crypto_xxtea_dec((int32u*)(&data[idx * 16]), 4, (int32u*)key);
            break;
    }
}
