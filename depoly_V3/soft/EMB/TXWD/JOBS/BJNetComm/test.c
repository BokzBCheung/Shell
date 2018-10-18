/*
 *********************************
 *  BJNetComm 测试用例
 *
 *  创建时间: 2016-04-06
 *********************************
 */

#include "BJNetComm.h"

#include <stdio.h>
#include <string.h>

#ifndef _CR_NO_PRAGMA_LIB_
    #pragma comment (lib, "BJNetComm.lib")
#endif

#define TEST_STR \
    "1234567890+abcdefghijklmnopqrstuvwxyz-ABCDEFGHIJKLMNOPQRSTUVWXYZ"\
    "1234567890+abcdefghijklmnopqrstuvwxyz-ABCDEFGHIJKLMNOPQRSTUVWXYZ"\
    "1234567890+abcdefghijklmnopqrstuvwxyz-ABCDEFGHIJKLMNOPQRSTUVWXYZ"\
    "1234567890+abcdefghijklmnopqrstuvwxyz-ABCDEFGHIJKLMNOPQRSTUVWXYZ"
/*
=======================================
    主函数
=======================================
*/
int main (int argc, char *argv[])
{
    FILE *fp;
    char name[260];
    unsigned int idx;
    char *str1, *str2;
    bjnetcomm_t bjnet;

    bjnet = BJNetComm_Init();
    if (bjnet == NULL) {
        printf("BJNetComm_Init() failure!\n");
        return (-1);
    }

    for (idx = 0x00; idx <= 0x05; idx++) {
        str1 = BJNetComm_EncString(bjnet, idx, TEST_STR, NULL);
        if (str1 == NULL) {
            printf("BJNetComm_EncString() failure!\n");
            continue;
        }
        sprintf(name, "%02x.txt", idx);
        fp = fopen(name, "w");
        if (fp != NULL) {
            fprintf(fp, "%s", str1);
            fclose(fp);
        }
        str2 = BJNetComm_DecString(bjnet, str1);
        BJNetComm_PtrFree(str1);
        if (str2 == NULL) {
            printf("BJNetComm_DecString() failure!\n");
            continue;
        }
        if (strcmp(str2, TEST_STR) == 0)
            printf("%02x: OKAY\n", idx);
        else
            printf("%02x: FAIL\n", idx);
        BJNetComm_PtrFree(str2);
    }

    BJNetComm_UpdateToken(bjnet, TEST_STR);

    for (idx = 0x20; idx <= 0x25; idx++) {
        str1 = BJNetComm_EncString(bjnet, idx, TEST_STR, NULL);
        if (str1 == NULL) {
            printf("BJNetComm_EncString() failure!\n");
            continue;
        }
        sprintf(name, "%02x.txt", idx);
        fp = fopen(name, "w");
        if (fp != NULL) {
            fprintf(fp, "%s", str1);
            fclose(fp);
        }
        str2 = BJNetComm_DecString(bjnet, str1);
        BJNetComm_PtrFree(str1);
        if (str2 == NULL) {
            printf("BJNetComm_DecString() failure!\n");
            continue;
        }
        if (strcmp(str2, TEST_STR) == 0)
            printf("%02x: OKAY\n", idx);
        else
            printf("%02x: FAIL\n", idx);
        BJNetComm_PtrFree(str2);
    }

    BJNetComm_Kill(bjnet);
    return (0);
}
