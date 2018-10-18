/*
 *********************************
 *  BJNetComm 库包装
 *
 *  创建时间: 2016-04-06
 *********************************
 */

#include "crhack.h"
#include "util/bjnetcomm.h"

/* BJNetComm 导入库 */
#ifndef _CR_NO_PRAGMA_LIB_
    #pragma comment (lib, "COREs.lib")
    #pragma comment (lib, "UTILs.lib")
#endif

#if defined(_CR_OS_MSWIN_)
/*
=======================================
    BJNetComm DLL 入口点
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
void_t  so_bjnet_init (void_t) __attribute__((constructor));
void_t  so_bjnet_init (void_t)
{
    crhack_core_init();
}
void_t  so_bjnet_fini (void_t) __attribute__((destructor));
void_t  so_bjnet_fini (void_t)
{
    crhack_core_free();
}
#endif  /* _CR_OS_MSWIN_ */

/*****************************************************************************/
/*                                   LIB                                     */
/*****************************************************************************/

/*
=======================================
    BJNetComm 初始化
=======================================
*/
CR_DLL bjnetcomm_t STDCALL
BJNetComm_Init (void_t)
{
    return (bjnetcomm_init());
}

/*
=======================================
    BJNetComm 释放
=======================================
*/
CR_DLL void_t STDCALL
BJNetComm_Kill (
  __CR_IN__ bjnetcomm_t bjnet
    )
{
    bjnetcomm_kill(bjnet);
}

/*
=======================================
    BJNetComm 重置
=======================================
*/
CR_DLL void_t STDCALL
BJNetComm_Reset (
  __CR_IN__ bjnetcomm_t bjnet
    )
{
    bjnetcomm_reset(bjnet);
}

/*
=======================================
    BJNetComm 最大数据包
=======================================
*/
CR_DLL void_t STDCALL
BJNetComm_SizeMax (
  __CR_IN__ bjnetcomm_t bjnet,
  __CR_IN__ int32u      size_kb
    )
{
    bjnetcomm_size_max(bjnet, size_kb);
}

/*
=======================================
    BJNetComm 更新令牌
=======================================
*/
CR_DLL void_t STDCALL
BJNetComm_UpdateToken (
  __CR_IO__ bjnetcomm_t     bjnet,
  __CR_IN__ const ansi_t*   token
    )
{
    bjnetcomm_update_token(bjnet, token);
}

/*
=======================================
    BJNetComm 编码字节组
=======================================
*/
CR_DLL ansi_t* STDCALL
BJNetComm_EncBytes (
  __CR_IN__ bjnetcomm_t     bjnet,
  __CR_IN__ uint_t          type,
  __CR_IN__ const void_t*   data,
  __CR_IN__ uint_t          size,
  __CR_IN__ const ansi_t*   user
    )
{
    return (bjnetcomm_enc_bytes(bjnet, type, data, size, user));
}

/*
=======================================
    BJNetComm 编码字符串
=======================================
*/
CR_DLL ansi_t* STDCALL
BJNetComm_EncString (
  __CR_IN__ bjnetcomm_t     bjnet,
  __CR_IN__ uint_t          type,
  __CR_IN__ const ansi_t*   string,
  __CR_IN__ const ansi_t*   user
    )
{
    return (bjnetcomm_enc_string(bjnet, type, string, user));
}

/*
=======================================
    BJNetComm 解码字节组 (高级)
=======================================
*/
CR_DLL void_t* STDCALL
BJNetComm_DecBytesEx (
  __CR_IN__ bjnetcomm_t     bjnet,
  __CR_IN__ uint_t          tag_type,
  __CR_IN__ const ansi_t*   tag_data,
  __CR_IN__ const ansi_t*   tag_sign,
  __CR_OT__ uint_t*         size
    )
{
    return (bjnetcomm_dec_bytes2(bjnet, tag_type, tag_data, tag_sign, size));
}

/*
=======================================
    BJNetComm 解码字节组
=======================================
*/
CR_DLL void_t* STDCALL
BJNetComm_DecBytes (
  __CR_IN__ bjnetcomm_t     bjnet,
  __CR_IN__ const ansi_t*   json,
  __CR_OT__ uint_t*         size
    )
{
    return (bjnetcomm_dec_bytes(bjnet, json, size));
}

/*
=======================================
    BJNetComm 解码字符串
=======================================
*/
CR_DLL ansi_t* STDCALL
BJNetComm_DecString (
  __CR_IN__ bjnetcomm_t     bjnet,
  __CR_IN__ const ansi_t*   json
    )
{
    return (bjnetcomm_dec_string(bjnet, json));
}

/*
=======================================
    BJNetComm 释放缓存
=======================================
*/
CR_DLL void_t STDCALL
BJNetComm_PtrFree (
  __CR_IN__ const void_t*   ptr
    )
{
    mem_free(ptr);
}

/*
=======================================
    BJAutoNet 序列号生成
=======================================
*/
CR_DLL void_t* STDCALL
BJAutoNet_GenSN32 (
  __CR_IN__ const ansi_t*   sn
    )
{
    sRC4    rc4;
    sCAST6  ct6;
    leng_t  size;
    byte_t  key[32];

    if (str_lenA(sn) != 32)
        return (NULL);
    hash_sha256_total(key, "BlowJobs::BJAutoNet", 20);
    crypto_rc4_key(&rc4, "BlowJobs::BJAutoNet", 20);
    crypto_rc4_ops(&rc4, key, sizeof(key));
    crypto_rc4_ops(&rc4, key, sizeof(key));
    crypto_cast6_key(&ct6, key, sizeof(key));
    size = 16;
    str2datA(key, &size, sn);
    if (size != 16)
        return (NULL);
    mem_cpy(key + 16, key, 16);
    crypto_cast6_enc(&ct6, (int32u*)(&key[16]), (int32u*)(&key[16]));
    return (mem_dup(key, sizeof(key)));
}

/*****************************************************************************/
/*                                   JNI                                     */
/*****************************************************************************/

#if defined(_CR_OS_ANDROID_)
    #include <jni.h>
#else
    #include "../jni/jni.h"
#endif
#define JNI_PTR2JLONG(p)    ((jlong)((size_t)(p)))
#define JNI_JLONG2PTR(l)    ((bjnetcomm_t)((size_t)(l)))

/*
=======================================
    BJNetComm 初始化
=======================================
*/
JNIEXPORT jlong JNICALL
Java_com_BlowJobs_BJNetComm_init (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz
    )
{
    return (JNI_PTR2JLONG(bjnetcomm_init()));
}

/*
=======================================
    BJNetComm 释放
=======================================
*/
JNIEXPORT void JNICALL
Java_com_BlowJobs_BJNetComm_kill (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   bjnet
    )
{
    bjnetcomm_kill(JNI_JLONG2PTR(bjnet));
}

/*
=======================================
    BJNetComm 重置
=======================================
*/
JNIEXPORT void JNICALL
Java_com_BlowJobs_BJNetComm_reset (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   bjnet
    )
{
    bjnetcomm_reset(JNI_JLONG2PTR(bjnet));
}

/*
=======================================
    BJNetComm 最大数据包
=======================================
*/
JNIEXPORT void JNICALL
Java_com_BlowJobs_BJNetComm_sizeMax (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   bjnet,
  __CR_IN__ jint    size_kb
    )
{
    bjnetcomm_size_max(JNI_JLONG2PTR(bjnet), size_kb);
}

/*
=======================================
    BJNetComm 更新令牌
=======================================
*/
JNIEXPORT void JNICALL
Java_com_BlowJobs_BJNetComm_updateToken (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   bjnet,
  __CR_IN__ jstring token
    )
{
    const char *s_token = (*env)->GetStringUTFChars(env, token, NULL);

    bjnetcomm_update_token(JNI_JLONG2PTR(bjnet), s_token);

    (*env)->ReleaseStringUTFChars(env, token, s_token);
}

/*
=======================================
    BJNetComm 编码字节组
=======================================
*/
JNIEXPORT jstring JNICALL
Java_com_BlowJobs_BJNetComm_encBytes (
  __CR_IN__ JNIEnv*     env,
  __CR_IN__ jclass      thiz,
  __CR_IN__ jlong       bjnet,
  __CR_IN__ jint        type,
  __CR_IN__ jbyteArray  data,
  __CR_IN__ jstring     user
    )
{
    char *back;
    jstring rett;
    const char *s_user;
    jsize len = (*env)->GetArrayLength(env, data);
    jbyte *buf = (jbyte*)mem_malloc(len * sizeof(jbyte));

    if (buf == NULL)
        return (NULL);
    (*env)->GetByteArrayRegion(env, data, 0, len, buf);
    if (user != NULL)
        s_user = (*env)->GetStringUTFChars(env, user, NULL);
    else
        s_user = NULL;

    back = bjnetcomm_enc_bytes(JNI_JLONG2PTR(bjnet), type, buf, len, s_user);

    if (user != NULL)
        (*env)->ReleaseStringUTFChars(env, user, s_user);
    mem_free(buf);

    if (back == NULL)
        return (NULL);
    rett = (*env)->NewStringUTF(env, back);
    mem_free(back);
    return (rett);
}

/*
=======================================
    BJNetComm 编码字符串
=======================================
*/
JNIEXPORT jstring JNICALL
Java_com_BlowJobs_BJNetComm_encString (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   bjnet,
  __CR_IN__ jint    type,
  __CR_IN__ jstring string,
  __CR_IN__ jstring user
    )
{
    char *back;
    jstring rett;
    const char *s_user;
    const char *s_string = (*env)->GetStringUTFChars(env, string, NULL);

    if (user != NULL)
        s_user = (*env)->GetStringUTFChars(env, user, NULL);
    else
        s_user = NULL;

    back = bjnetcomm_enc_string(JNI_JLONG2PTR(bjnet), type, s_string, s_user);

    if (user != NULL)
        (*env)->ReleaseStringUTFChars(env, user, s_user);
    (*env)->ReleaseStringUTFChars(env, string, s_string);

    if (back == NULL)
        return (NULL);
    rett = (*env)->NewStringUTF(env, back);
    mem_free(back);
    return (rett);
}

/*
=======================================
    BJNetComm 解码字节组 (高级)
=======================================
*/
JNIEXPORT jbyteArray JNICALL
Java_com_BlowJobs_BJNetComm_decBytesEx (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   bjnet,
  __CR_IN__ jint    tag_type,
  __CR_IN__ jstring tag_data,
  __CR_IN__ jstring tag_sign
    )
{
    jbyte *back;
    uint_t size;
    jbyteArray rett;
    const char *s_tag_data = (*env)->GetStringUTFChars(env, tag_data, NULL);
    const char *s_tag_sign = (*env)->GetStringUTFChars(env, tag_sign, NULL);

    back = (jbyte*)bjnetcomm_dec_bytes2(JNI_JLONG2PTR(bjnet), tag_type,
                                s_tag_data, s_tag_sign, &size);

    (*env)->ReleaseStringUTFChars(env, tag_sign, s_tag_sign);
    (*env)->ReleaseStringUTFChars(env, tag_data, s_tag_data);

    if (back == NULL)
        return (NULL);
    rett = (*env)->NewByteArray(env, size);
    (*env)->SetByteArrayRegion(env, rett, 0, size, back);
    mem_free(back);
    return (rett);
}

/*
=======================================
    BJNetComm 解码字节组
=======================================
*/
JNIEXPORT jbyteArray JNICALL
Java_com_BlowJobs_BJNetComm_decBytes (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   bjnet,
  __CR_IN__ jstring json
    )
{
    jbyte *back;
    uint_t size;
    jbyteArray rett;
    const char *s_json = (*env)->GetStringUTFChars(env, json, NULL);

    back = (jbyte*)bjnetcomm_dec_bytes(JNI_JLONG2PTR(bjnet), s_json, &size);

    (*env)->ReleaseStringUTFChars(env, json, s_json);

    if (back == NULL)
        return (NULL);
    rett = (*env)->NewByteArray(env, size);
    (*env)->SetByteArrayRegion(env, rett, 0, size, back);
    mem_free(back);
    return (rett);
}

/*
=======================================
    BJNetComm 解码字符串
=======================================
*/
JNIEXPORT jstring JNICALL
Java_com_BlowJobs_BJNetComm_decString (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   bjnet,
  __CR_IN__ jstring json
    )
{
    char *back;
    jstring rett;
    const char *s_json = (*env)->GetStringUTFChars(env, json, NULL);

    back = bjnetcomm_dec_string(JNI_JLONG2PTR(bjnet), s_json);

    (*env)->ReleaseStringUTFChars(env, json, s_json);

    if (back == NULL)
        return (NULL);
    rett = (*env)->NewStringUTF(env, back);
    mem_free(back);
    return (rett);
}

/*
=======================================
    BJAutoNet 序列号生成
=======================================
*/
JNIEXPORT jbyteArray JNICALL
Java_com_BlowJobs_BJNetComm_genSN32 (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jstring sn
    )
{
    jbyte *back;
    jbyteArray rett;
    const char *s_sn = (*env)->GetStringUTFChars(env, sn, NULL);

    back = (jbyte*)BJAutoNet_GenSN32(s_sn);

    (*env)->ReleaseStringUTFChars(env, sn, s_sn);

    if (back == NULL)
        return (NULL);
    rett = (*env)->NewByteArray(env, 32);
    (*env)->SetByteArrayRegion(env, rett, 0, 32, back);
    mem_free(back);
    return (rett);
}
