/*
 *********************************
 *  BJResDec ��Դ���ܹ���
 *
 *  ����ʱ��: 2016-04-15
 *********************************
 */

#include "BJResLib.h"

#include <stdio.h>

/*
=======================================
    ������
=======================================
*/
int main (int argc, char *argv[])
{
    uint_t  type;
    leng_t  size;
    byte_t* data;
    ansi_t  key[MAX_PATHA];

    /* ����˵�� */
    crhack_core_init();
    if (argc <= 1) {
        printf("Usage: BJResDec <file2dec.ext> [type=0]\n");
        return (0);
    }

    /* �����ļ� */
    data = (byte_t*)file_load_as_binA(argv[1], &size);
    if (data == NULL) {
        printf("read %s failure!\n", argv[1]);
        return (-1);
    }

    /* ʹ���ļ���������Կ */
    if (argc > 2)
        type = str2intxA(argv[2], NULL);
    else
        type = 0;
    str_lwrA(flname_extractA(key, argv[1]));
    crypt_decode(data, size, key, type);

    /* �����ļ� */
    if (!file_saveA(argv[1], data, size)) {
        printf("write %s failure!\n", argv[1]);
        mem_free(data);
        return (-1);
    }
    mem_free(data);
    return (1);
}
