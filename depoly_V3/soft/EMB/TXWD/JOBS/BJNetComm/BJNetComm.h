/*
 *********************************
 *  BJNetComm 头文件
 *
 *  创建时间: 2016-04-06
 *********************************
 */

#ifndef __BJNETCOMM_H__
#define __BJNETCOMM_H__

typedef void* bjnetcomm_t;

#ifndef _WIN32
    #define BJSTDCALL
#else
    #define BJSTDCALL   __stdcall
#endif

#ifdef __cplusplus
extern "C"
{
#endif

bjnetcomm_t BJSTDCALL BJNetComm_Init (void);
void BJSTDCALL BJNetComm_Kill (bjnetcomm_t bjnet);
void BJSTDCALL BJNetComm_Reset (bjnetcomm_t bjnet);
void BJSTDCALL BJNetComm_SizeMax (bjnetcomm_t bjnet,
                                  unsigned int size_kb);
void BJSTDCALL BJNetComm_UpdateToken (bjnetcomm_t bjnet,
                                      const char *token);
char* BJSTDCALL BJNetComm_EncBytes (bjnetcomm_t bjnet,
                                    unsigned int type,
                                    const void *data,
                                    unsigned int size,
                                    const char *user);
char* BJSTDCALL BJNetComm_EncString (bjnetcomm_t bjnet,
                                     unsigned int type,
                                     const char *string,
                                     const char *user);
void* BJSTDCALL BJNetComm_DecBytesEx (bjnetcomm_t bjnet,
                                      unsigned int tag_type,
                                      const char *tag_data,
                                      const char *tag_sign,
                                      unsigned int *size);
void* BJSTDCALL BJNetComm_DecBytes (bjnetcomm_t bjnet,
                                    const char *json,
                                    unsigned int *size);
char* BJSTDCALL BJNetComm_DecString (bjnetcomm_t bjnet,
                                     const char *json);
void BJSTDCALL BJNetComm_PtrFree (const void *ptr);

void* BJSTDCALL BJAutoNet_GenSN32 (const char *sn);

#ifdef __cplusplus
}
#endif

#endif  /* !__BJNETCOMM_H__ */
