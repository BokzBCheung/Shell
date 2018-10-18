/*
 *********************************
 *  BJResDec 资源解密工具
 *
 *  创建时间: 2016-04-15
 *********************************
 */

#include "BJResLib.h"

#include <stdio.h>

/*
=======================================
    主函数
=======================================
*/
int main (int argc, char *argv[])
{
    uint_t  type;
    leng_t  size;
    byte_t* data;
    ansi_t  key[MAX_PATHA];

    /* 帮助说明 */
    crhack_core_init();
    if (argc <= 1) {
        printf("Usage: BJResDec <file2dec.ext> [type=0]\n");
        return (0);
    }

    /* 读入文件 */
    data = (byte_t*)file_load_as_binA(argv[1], &size);
    if (data == NULL) {
        printf("read %s failure!\n", argv[1]);
        return (-1);
    }

    /* 使用文件名来做密钥 */
    if (argc > 2)
        type = str2intxA(argv[2], NULL);
    else
        type = 0;
    str_lwrA(flname_extractA(key, argv[1]));
    crypt_decode(data, size, key, type);

    /* 保存文件 */
    if (!file_saveA(argv[1], data, size)) {
        printf("write %s failure!\n", argv[1]);
        mem_free(data);
        return (-1);
    }
    mem_free(data);
    return (1);
}
