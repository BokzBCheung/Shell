/*
 *********************************
 *  BJResSig 资源签名工具
 *
 *  创建时间: 2016-04-15
 *********************************
 */

#include "BJResLib.h"

#include <stdio.h>

/*
---------------------------------------
    使用说明
---------------------------------------
*/
static void_t usage (void_t)
{
    printf("Usage: BJResSig <g> <file2gen.ext> [type=0]\n");
    printf("       BJResSig <c> <file2chk.ext> [type=0]\n");
    printf("       BJResSig <k> <bits> [split=0] [e=65537]\n");
}

/*
=======================================
    主函数
=======================================
*/
int main (int argc, char *argv[])
{
    FILE*   fp;
    sRSA    pub;
    sRSA    prv;
    ansi_t* eee;
    byte_t* data;
    byte_t  dest[64];
    byte_t  hash[64];
    leng_t  size, idx;
    uint_t  bits, split;
    ansi_t  name[MAX_PATHA];

    /* 帮助说明 */
    crhack_core_init();
    if (argc <= 2) {
        usage();
        return (0);
    }

    /* 公用参数 */
    if (argc > 3)
        split = str2intxA(argv[3], NULL);
    else
        split = 0;
    if ((argv[1])[0] == 'k')
    {
        /* 创建公私密钥 */
        bits = str2intxA(argv[2], NULL);
        eee = "65537";
        if (argc > 4)
            eee = argv[4];
        rand_seed((uint_t)timer_get32());
        crypto_rsa_key(&pub, &prv, eee, bits, split);
        if (!save_rsa_key(&pub, PUB_KEY_FILE))
            printf("write " PUB_KEY_FILE " failure!\n");
        if (!save_rsa_key(&prv, PRV_KEY_FILE))
            printf("write " PRV_KEY_FILE " failure!\n");
    }
    else
    if ((argv[1])[0] == 'g')
    {
        /* 生成文件签名 */
        if (!load_rsa_key(&prv, PRV_KEY_FILE)) {
            printf("read " PRV_KEY_FILE " failure!\n");
            return (-1);
        }
        data = (byte_t*)file_load_as_binA(argv[2], &size);
        if (data == NULL) {
            printf("read %s failure!\n", argv[2]);
            return (-1);
        }
        bits = hash_doit(hash, data, size, split);
        mem_free(data);
        size = crypto_rsa_enc(&prv, NULL, hash, bits);
        data = (byte_t*)mem_malloc(size);
        if (data == NULL) {
            printf("out of memory!\n");
            return (-1);
        }
        sprintf(name, "%s.sig", argv[2]);
        fp = fopen(name, "w");
        if (fp == NULL) {
            printf("write %s failure!\n", name);
            mem_free(data);
            return (-1);
        }
        size = crypto_rsa_enc(&prv, data, hash, bits);
        for (idx = 0; idx < size; idx++)
            fprintf(fp, "%02X", data[idx]);
        fclose(fp);
        mem_free(data);
    }
    else
    if ((argv[1])[0] == 'c')
    {
        /* 生成文件签名 */
        if (!load_rsa_key(&pub, PUB_KEY_FILE)) {
            printf("read " PUB_KEY_FILE " failure!\n");
            return (-1);
        }
        data = (byte_t*)file_load_as_binA(argv[2], &size);
        if (data == NULL) {
            printf("read %s failure!\n", argv[2]);
            return (-1);
        }
        bits = hash_doit(hash, data, size, split);
        mem_free(data);
        sprintf(name, "%s.sig", argv[2]);
        eee = file_load_as_strA(name);
        if (eee == NULL) {
            printf("read %s failure!\n", name);
            return (-1);
        }
        size = str_lenA(eee);
        if (size == 0 || size % 2 != 0) {
            printf("invalid sign file %s!\n", name);
            mem_free(eee);
            return (-1);
        }
        size /= 2;
        data = (byte_t*)mem_malloc(size);
        if (data == NULL) {
            printf("out of memory!\n");
            mem_free(eee);
            return (-1);
        }
        idx = size;
        str2datA(data, &idx, eee);
        mem_free(eee);
        if (idx != size) {
            printf("invalid sign file %s!\n", name);
            mem_free(data);
            return (-1);
        }
        idx = crypto_rsa_dec(&pub, NULL, data, size);
        if (idx != bits) {
            printf("sign check failure!\n");
            mem_free(data);
            return (-1);
        }
        crypto_rsa_dec(&pub, dest, data, size);
        mem_free(data);
        if (mem_cmp(dest, hash, bits) != 0) {
            printf("sign check failure!\n");
            return (-1);
        }
        printf("sign check success!\n");
    }
    else
    {
        /* 帮助说明 */
        usage();
        return (0);
    }
    return (1);
}
