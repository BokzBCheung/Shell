/*
 *********************************
 *  BJResLib 头文件
 *
 *  创建时间: 2016-04-18
 *********************************
 */

#ifndef __BJRESLIB_H__
#define __BJRESLIB_H__

#include "crhack.h"

#define PUB_KEY_FILE    "sign.pub"
#define PRV_KEY_FILE    "sign.prv"

CR_API bool_t   load_rsa_key (sRSA *key, const ansi_t *name);
CR_API bool_t   save_rsa_key (const sRSA *key, const ansi_t *name);
CR_API uint_t   hash_doit (byte_t *hash, const void_t *data,
                           leng_t size, uint_t type);
CR_API void_t   crypt_encode (byte_t *data, leng_t size,
                              const ansi_t *skey, uint_t type);
CR_API void_t   crypt_decode (byte_t *data, leng_t size,
                              const ansi_t *skey, uint_t type);

#endif  /* !__BJRESLIB_H__ */
