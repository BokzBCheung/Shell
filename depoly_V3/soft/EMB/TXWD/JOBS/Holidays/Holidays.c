/*
 *********************************
 *  Holidays 库包装
 *
 *  创建时间: 2016-04-13
 *********************************
 */

#include "crhack.h"
#include "util/holidays.h"

/* Holidays 导入库 */
#ifndef _CR_NO_PRAGMA_LIB_
    #pragma comment (lib, "COREs.lib")
    #pragma comment (lib, "UTILs.lib")
#endif

#if defined(_CR_OS_MSWIN_)
/*
=======================================
    Holidays DLL 入口点
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
void_t  so_hdays_init (void_t) __attribute__((constructor));
void_t  so_hdays_init (void_t)
{
    crhack_core_init();
}
void_t  so_hdays_fini (void_t) __attribute__((destructor));
void_t  so_hdays_fini (void_t)
{
    crhack_core_free();
}
#endif  /* _CR_OS_MSWIN_ */

/*****************************************************************************/
/*                                   LIB                                     */
/*****************************************************************************/

/*
=======================================
    Holidays 初始化
=======================================
*/
CR_DLL holidays_t STDCALL
Holidays_Init (void_t)
{
    return (holidays_init());
}

/*
=======================================
    Holidays 释放
=======================================
*/
CR_DLL void_t STDCALL
Holidays_Kill (
  __CR_IN__ holidays_t  hdays
    )
{
    holidays_kill(hdays);
}

/*
=======================================
    Holidays 设置规则
=======================================
*/
CR_DLL sint_t STDCALL
Holidays_RuleSet (
  __CR_IN__ holidays_t          hdays,
  __CR_IN__ const sHOLI_RULE*   rule
    )
{
    return (holidays_rule_set(hdays, rule));
}

/*
=======================================
    Holidays 设置节假日
=======================================
*/
CR_DLL sint_t STDCALL
Holidays_HdaySet (
  __CR_IN__ holidays_t      hdays,
  __CR_IN__ const byte_t*   holi
    )
{
    return (holidays_hday_set(hdays, (sHOLI_DAYS*)(holi + 1), holi[0]));
}

/*
=======================================
    Holidays 设置周计划
=======================================
*/
CR_DLL sint_t STDCALL
Holidays_WeekSet (
  __CR_IN__ holidays_t      hdays,
  __CR_IN__ const byte_t*   weeks,
  __CR_IN__ uint_t          index
    )
{
    return (holidays_week_set(hdays, weeks, index));
}

/*
=======================================
    Holidays 获取免费时间
=======================================
*/
CR_DLL sint_t STDCALL
Holidays_FreeGet (
  __CR_IN__ holidays_t      hdays,
  __CR_IN__ const ansi_t*   date,
  __CR_IN__ sint_t          rule_id,
  __CR_IN__ sint_t          week_id,
  __CR_IN__ sint_t          holi_id
    )
{
    sDATETIME   s_date;

    if (rule_id < 0 || rule_id > 255)
        return (-2);
    if (week_id < 0 || week_id > 255)
        return (-2);
    if (holi_id < 0 || holi_id > 255)
        return (-2);
    if (!str2datetimeA(&s_date, date, '@'))
        return (-2);
    return (holidays_free_get(hdays, &s_date, (byte_t)rule_id,
                             (byte_t)week_id, (byte_t)holi_id));
}

/*
=======================================
    Holidays 计算停车费用
=======================================
*/
CR_DLL sint_t STDCALL
Holidays_Money (
  __CR_IN__ holidays_t      hdays,
  __CR_IN__ const ansi_t*   leave,
  __CR_IN__ const ansi_t*   enter,
  __CR_IN__ const ansi_t*   mbeg,
  __CR_IN__ const ansi_t*   mend,
  __CR_IN__ sint_t          is_pre,
  __CR_IN__ sint_t          rule_id,
  __CR_IN__ sint_t          rule_wid,
  __CR_IN__ sint_t          rule_hid,
  __CR_IN__ sint_t          month_id,
  __CR_IN__ sint_t          month_wid,
  __CR_IN__ sint_t          month_hid
    )
{
    sDATETIME   *p_mbeg = NULL;
    sDATETIME   *p_mend = NULL;
    sDATETIME   s_mbeg, s_mend;
    sDATETIME   s_leave, s_enter;

    if (rule_id < 0 || rule_id > 255)
        return (-2);
    if (rule_wid < 0 || rule_wid > 255)
        return (-2);
    if (rule_hid < 0 || rule_hid > 255)
        return (-2);
    if (month_id < 0 || month_id > 255)
        return (-2);
    if (month_wid < 0 || month_wid > 255)
        return (-2);
    if (month_hid < 0 || month_hid > 255)
        return (-2);
    if (!str2datetimeA(&s_leave, leave, '@'))
        return (-2);
    if (!str2datetimeA(&s_enter, enter, '@'))
        return (-2);
    if (mbeg != NULL) {
        if (!str2datetimeA(&s_mbeg, mbeg, '@'))
            return (-2);
        p_mbeg = &s_mbeg;
    }
    if (mend != NULL) {
        if (!str2datetimeA(&s_mend, mend, '@'))
            return (-2);
        p_mend = &s_mend;
    }
    return (holidays_money(hdays, &s_leave, &s_enter,
                    p_mbeg, p_mend, (bool_t)is_pre, (byte_t)rule_id,
                    (byte_t)rule_wid, (byte_t)rule_hid, (byte_t)month_id,
                            (byte_t)month_wid, (byte_t)month_hid));
}

/*
=======================================
    Holidays 获取剩余时间
=======================================
*/
CR_DLL sint_t STDCALL
Holidays_Rests (
  __CR_IN__ holidays_t      hdays,
  __CR_IN__ const ansi_t*   leave,
  __CR_IN__ const ansi_t*   enter,
  __CR_IN__ const ansi_t*   mbeg,
  __CR_IN__ const ansi_t*   mend,
  __CR_IN__ sint_t          is_pre,
  __CR_IN__ sint_t          rule_id,
  __CR_IN__ sint_t          rule_wid,
  __CR_IN__ sint_t          rule_hid,
  __CR_IN__ sint_t          month_id,
  __CR_IN__ sint_t          month_wid,
  __CR_IN__ sint_t          month_hid
    )
{
    sDATETIME   *p_mbeg = NULL;
    sDATETIME   *p_mend = NULL;
    sDATETIME   s_mbeg, s_mend;
    sDATETIME   s_leave, s_enter;

    if (rule_id < 0 || rule_id > 255)
        return (-2);
    if (rule_wid < 0 || rule_wid > 255)
        return (-2);
    if (rule_hid < 0 || rule_hid > 255)
        return (-2);
    if (month_id < 0 || month_id > 255)
        return (-2);
    if (month_wid < 0 || month_wid > 255)
        return (-2);
    if (month_hid < 0 || month_hid > 255)
        return (-2);
    if (!str2datetimeA(&s_leave, leave, '@'))
        return (-2);
    if (!str2datetimeA(&s_enter, enter, '@'))
        return (-2);
    if (mbeg != NULL) {
        if (!str2datetimeA(&s_mbeg, mbeg, '@'))
            return (-2);
        p_mbeg = &s_mbeg;
    }
    if (mend != NULL) {
        if (!str2datetimeA(&s_mend, mend, '@'))
            return (-2);
        p_mend = &s_mend;
    }
    return (holidays_rests(hdays, &s_leave, &s_enter,
                    p_mbeg, p_mend, (bool_t)is_pre, (byte_t)rule_id,
                    (byte_t)rule_wid, (byte_t)rule_hid, (byte_t)month_id,
                            (byte_t)month_wid, (byte_t)month_hid));
}

/*
=======================================
    Holidays 获取下个缴费点
=======================================
*/
CR_DLL sint_t STDCALL
Holidays_Stepa (
  __CR_IN__ holidays_t      hdays,
  __CR_IN__ const ansi_t*   leave,
  __CR_IN__ const ansi_t*   enter,
  __CR_IN__ const ansi_t*   mbeg,
  __CR_IN__ const ansi_t*   mend,
  __CR_IN__ sint_t          is_pre,
  __CR_IN__ sint_t          rule_id,
  __CR_IN__ sint_t          rule_wid,
  __CR_IN__ sint_t          rule_hid,
  __CR_IN__ sint_t          month_id,
  __CR_IN__ sint_t          month_wid,
  __CR_IN__ sint_t          month_hid
    )
{
    sDATETIME   *p_mbeg = NULL;
    sDATETIME   *p_mend = NULL;
    sDATETIME   s_mbeg, s_mend;
    sDATETIME   s_leave, s_enter;

    if (rule_id < 0 || rule_id > 255)
        return (-2);
    if (rule_wid < 0 || rule_wid > 255)
        return (-2);
    if (rule_hid < 0 || rule_hid > 255)
        return (-2);
    if (month_id < 0 || month_id > 255)
        return (-2);
    if (month_wid < 0 || month_wid > 255)
        return (-2);
    if (month_hid < 0 || month_hid > 255)
        return (-2);
    if (!str2datetimeA(&s_leave, leave, '@'))
        return (-2);
    if (!str2datetimeA(&s_enter, enter, '@'))
        return (-2);
    if (mbeg != NULL) {
        if (!str2datetimeA(&s_mbeg, mbeg, '@'))
            return (-2);
        p_mbeg = &s_mbeg;
    }
    if (mend != NULL) {
        if (!str2datetimeA(&s_mend, mend, '@'))
            return (-2);
        p_mend = &s_mend;
    }
    return (holidays_stepa(hdays, &s_leave, &s_enter,
                    p_mbeg, p_mend, (bool_t)is_pre, (byte_t)rule_id,
                    (byte_t)rule_wid, (byte_t)rule_hid, (byte_t)month_id,
                            (byte_t)month_wid, (byte_t)month_hid));
}

/*
=======================================
    Holidays 计算停车费用 (简化版)
=======================================
*/
CR_DLL sint_t STDCALL
Holidays_Money2 (
  __CR_IN__ holidays_t      hdays,
  __CR_IN__ const ansi_t*   leave,
  __CR_IN__ const ansi_t*   enter,
  __CR_IN__ sint_t          is_pre,
  __CR_IN__ sint_t          rule_id,
  __CR_IN__ sint_t          week_id,
  __CR_IN__ sint_t          holi_id
    )
{
    return (Holidays_Money(hdays, leave, enter, NULL, NULL, is_pre,
                           rule_id, week_id, holi_id, 0, 0, 0));
}

/*
=======================================
    Holidays 获取剩余时间 (简化版)
=======================================
*/
CR_DLL sint_t STDCALL
Holidays_Rests2 (
  __CR_IN__ holidays_t      hdays,
  __CR_IN__ const ansi_t*   leave,
  __CR_IN__ const ansi_t*   enter,
  __CR_IN__ sint_t          is_pre,
  __CR_IN__ sint_t          rule_id,
  __CR_IN__ sint_t          week_id,
  __CR_IN__ sint_t          holi_id
    )
{
    return (Holidays_Rests(hdays, leave, enter, NULL, NULL, is_pre,
                           rule_id, week_id, holi_id, 0, 0, 0));
}

/*
=======================================
    Holidays 获取下个缴费点 (简化版)
=======================================
*/
CR_DLL sint_t STDCALL
Holidays_Stepa2 (
  __CR_IN__ holidays_t      hdays,
  __CR_IN__ const ansi_t*   leave,
  __CR_IN__ const ansi_t*   enter,
  __CR_IN__ sint_t          is_pre,
  __CR_IN__ sint_t          rule_id,
  __CR_IN__ sint_t          week_id,
  __CR_IN__ sint_t          holi_id
    )
{
    return (Holidays_Stepa(hdays, leave, enter, NULL, NULL, is_pre,
                           rule_id, week_id, holi_id, 0, 0, 0));
}

/*
---------------------------------------
    Holidays 分区段排序比较
---------------------------------------
*/
static sint_t
range_fcmp (
  __CR_IN__ const void_t*   elem1,
  __CR_IN__ const void_t*   elem2
    )
{
    sHOLI_RNGS* obj1 = (sHOLI_RNGS*)elem1;
    sHOLI_RNGS* obj2 = (sHOLI_RNGS*)elem2;

    return (datetime_cmp(&obj1->date, &obj2->date));
}

/*
---------------------------------------
    Holidays 转换区域字符串
---------------------------------------
*/
static sHOLI_RNGS*
Holidays_Ranges (
  __CR_IN__ const ansi_t*   range,
  __CR_OT__ uint_t*         count
    )
{
    leng_t      idx;
    leng_t      num;
    uint_t      tmp;
    uint_t      cnt;
    ansi_t*     str;
    ansi_t**    lst;
    sHOLI_RNGS* ret;

    /* 逗号分隔的串 */
    str = str_dupA(range);
    if (str == NULL)
        return (NULL);
    lst = str_splitA(str, ',', &num);
    if (lst == NULL)
        goto _failure1;
    if (num % 4 != 0)
        goto _failure2;
    ret = mem_talloc(num, sHOLI_RNGS);
    if (ret == NULL)
        goto _failure2;

    /* 解析每个时间点 */
    for (cnt = 0, idx = 0; idx < num; idx += 4, cnt++) {
        if (!str2datetimeA(&ret[cnt].date, skip_spaceA(lst[idx]), '@'))
            goto _failure3;
        tmp = str2intxA(lst[idx + 1], NULL);
        if (tmp > 255)
            goto _failure3;
        ret[cnt].rule = (byte_t)tmp;
        tmp = str2intxA(lst[idx + 2], NULL);
        if (tmp > 255)
            goto _failure3;
        ret[cnt].week = (byte_t)tmp;
        tmp = str2intxA(lst[idx + 3], NULL);
        if (tmp > 255)
            goto _failure3;
        ret[cnt].holi = (byte_t)tmp;
    }
    *count = cnt;
    quick_sort(ret, cnt, sizeof(sHOLI_RNGS), range_fcmp);
    mem_free(lst);
    mem_free(str);
    return (ret);

_failure3:
    mem_free(ret);
_failure2:
    mem_free(lst);
_failure1:
    mem_free(str);
    return (NULL);
}

/*
=======================================
    Holidays 获取免费时间 (加强版)
=======================================
*/
CR_DLL sint_t STDCALL
Holidays_FreeGetEx (
  __CR_IN__ holidays_t      hdays,
  __CR_IN__ const ansi_t*   date,
  __CR_IN__ const ansi_t*   range,
  __CR_IN__ sint_t          rule_id,
  __CR_IN__ sint_t          week_id,
  __CR_IN__ sint_t          holi_id
    )
{
    sint_t      back;
    uint_t      count;
    sDATETIME   s_date;
    sHOLI_RNGS* s_range;

    if (rule_id < 0 || rule_id > 255)
        return (-2);
    if (week_id < 0 || week_id > 255)
        return (-2);
    if (holi_id < 0 || holi_id > 255)
        return (-2);
    if (!str2datetimeA(&s_date, date, '@'))
        return (-2);
    if (range != NULL) {
        s_range = Holidays_Ranges(range, &count);
        if (s_range == NULL)
            return (-2);
    }
    else {
        count = 0;
        s_range = NULL;
    }
    back = holidays_free_get2(hdays, &s_date, s_range, count,
                (byte_t)rule_id, (byte_t)week_id, (byte_t)holi_id);
    TRY_FREE(s_range);
    return (back);
}

/*
=======================================
    Holidays 计算停车费用 (加强版)
=======================================
*/
CR_DLL sint_t STDCALL
Holidays_MoneyEx (
  __CR_IN__ holidays_t      hdays,
  __CR_IN__ const ansi_t*   leave,
  __CR_IN__ const ansi_t*   enter,
  __CR_IN__ const ansi_t*   range,
  __CR_IN__ sint_t          is_pre,
  __CR_IN__ sint_t          rule_id,
  __CR_IN__ sint_t          week_id,
  __CR_IN__ sint_t          holi_id
    )
{
    sint_t      back;
    uint_t      count;
    sDATETIME   s_leave;
    sDATETIME   s_enter;
    sHOLI_RNGS* s_range;

    if (rule_id < 0 || rule_id > 255)
        return (-2);
    if (week_id < 0 || week_id > 255)
        return (-2);
    if (holi_id < 0 || holi_id > 255)
        return (-2);
    if (!str2datetimeA(&s_leave, leave, '@'))
        return (-2);
    if (!str2datetimeA(&s_enter, enter, '@'))
        return (-2);
    if (range != NULL) {
        s_range = Holidays_Ranges(range, &count);
        if (s_range == NULL)
            return (-2);
    }
    else {
        count = 0;
        s_range = NULL;
    }
    back = holidays_money2(hdays, &s_leave, &s_enter, s_range, count,
        (bool_t)is_pre, (byte_t)rule_id, (byte_t)week_id, (byte_t)holi_id);
    TRY_FREE(s_range);
    return (back);
}

/*
=======================================
    Holidays 获取剩余时间 (加强版)
=======================================
*/
CR_DLL sint_t STDCALL
Holidays_RestsEx (
  __CR_IN__ holidays_t      hdays,
  __CR_IN__ const ansi_t*   leave,
  __CR_IN__ const ansi_t*   enter,
  __CR_IN__ const ansi_t*   range,
  __CR_IN__ sint_t          is_pre,
  __CR_IN__ sint_t          rule_id,
  __CR_IN__ sint_t          week_id,
  __CR_IN__ sint_t          holi_id
    )
{
    sint_t      back;
    uint_t      count;
    sDATETIME   s_leave;
    sDATETIME   s_enter;
    sHOLI_RNGS* s_range;

    if (rule_id < 0 || rule_id > 255)
        return (-2);
    if (week_id < 0 || week_id > 255)
        return (-2);
    if (holi_id < 0 || holi_id > 255)
        return (-2);
    if (!str2datetimeA(&s_leave, leave, '@'))
        return (-2);
    if (!str2datetimeA(&s_enter, enter, '@'))
        return (-2);
    if (range != NULL) {
        s_range = Holidays_Ranges(range, &count);
        if (s_range == NULL)
            return (-2);
    }
    else {
        count = 0;
        s_range = NULL;
    }
    back = holidays_rests2(hdays, &s_leave, &s_enter, s_range, count,
        (bool_t)is_pre, (byte_t)rule_id, (byte_t)week_id, (byte_t)holi_id);
    TRY_FREE(s_range);
    return (back);
}

/*
=======================================
    Holidays 获取下个缴费点 (加强版)
=======================================
*/
CR_DLL sint_t STDCALL
Holidays_StepaEx (
  __CR_IN__ holidays_t      hdays,
  __CR_IN__ const ansi_t*   leave,
  __CR_IN__ const ansi_t*   enter,
  __CR_IN__ const ansi_t*   range,
  __CR_IN__ sint_t          is_pre,
  __CR_IN__ sint_t          rule_id,
  __CR_IN__ sint_t          week_id,
  __CR_IN__ sint_t          holi_id
    )
{
    sint_t      back;
    uint_t      count;
    sDATETIME   s_leave;
    sDATETIME   s_enter;
    sHOLI_RNGS* s_range;

    if (rule_id < 0 || rule_id > 255)
        return (-2);
    if (week_id < 0 || week_id > 255)
        return (-2);
    if (holi_id < 0 || holi_id > 255)
        return (-2);
    if (!str2datetimeA(&s_leave, leave, '@'))
        return (-2);
    if (!str2datetimeA(&s_enter, enter, '@'))
        return (-2);
    if (range != NULL) {
        s_range = Holidays_Ranges(range, &count);
        if (s_range == NULL)
            return (-2);
    }
    else {
        count = 0;
        s_range = NULL;
    }
    back = holidays_stepa2(hdays, &s_leave, &s_enter, s_range, count,
        (bool_t)is_pre, (byte_t)rule_id, (byte_t)week_id, (byte_t)holi_id);
    TRY_FREE(s_range);
    return (back);
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
#define JNI_JLONG2PTR(l)    ((holidays_t)((size_t)(l)))

/*
=======================================
    Holidays 初始化
=======================================
*/
JNIEXPORT jlong JNICALL
Java_com_BlowJobs_Holidays_init (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz
    )
{
    return (JNI_PTR2JLONG(holidays_init()));
}

/*
=======================================
    Holidays 释放
=======================================
*/
JNIEXPORT void JNICALL
Java_com_BlowJobs_Holidays_kill (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   hdays
    )
{
    holidays_kill(JNI_JLONG2PTR(hdays));
}

/*
=======================================
    Holidays 设置规则
=======================================
*/
JNIEXPORT jint JNICALL
Java_com_BlowJobs_Holidays_ruleSet (
  __CR_IN__ JNIEnv*     env,
  __CR_IN__ jclass      thiz,
  __CR_IN__ jlong       hdays,
  __CR_IN__ jbyteArray  rule
    )
{
    sHOLI_RULE s_rule;
    jsize len = (*env)->GetArrayLength(env, rule);

    if (len != sizeof(sHOLI_RULE))
        return (-2);
    (*env)->GetByteArrayRegion(env, rule, 0, len, (jbyte*)&s_rule);

    return (holidays_rule_set(JNI_JLONG2PTR(hdays), &s_rule));
}

/*
=======================================
    Holidays 设置节假日
=======================================
*/
JNIEXPORT jint JNICALL
Java_com_BlowJobs_Holidays_hdaySet (
  __CR_IN__ JNIEnv*     env,
  __CR_IN__ jclass      thiz,
  __CR_IN__ jlong       hdays,
  __CR_IN__ jbyteArray  holi
    )
{
    jint back;
    jbyte *buf;
    jsize len = (*env)->GetArrayLength(env, holi);

    if (len <= 1 || (len - 1) % sizeof(sHOLI_DAYS) != 0)
        return (-2);
    buf = (jbyte*)mem_malloc(len);
    if (buf == NULL)
        return (-3);
    (*env)->GetByteArrayRegion(env, holi, 0, len, buf);
    if ((jsize)buf[0] * sizeof(sHOLI_DAYS) != len - 1) {
        mem_free(buf);
        return (-2);
    }

    back = Holidays_HdaySet(JNI_JLONG2PTR(hdays), (byte_t*)buf);

    mem_free(buf);
    return (back);
}

/*
=======================================
    Holidays 设置周计划
=======================================
*/
JNIEXPORT jint JNICALL
Java_com_BlowJobs_Holidays_weekSet (
  __CR_IN__ JNIEnv*     env,
  __CR_IN__ jclass      thiz,
  __CR_IN__ jlong       hdays,
  __CR_IN__ jbyteArray  weeks,
  __CR_IN__ jint        index
    )
{
    jbyte buf[8];
    jsize len = (*env)->GetArrayLength(env, weeks);

    if (len != sizeof(buf))
        return (-2);
    (*env)->GetByteArrayRegion(env, weeks, 0, len, buf);

    return (holidays_week_set(JNI_JLONG2PTR(hdays), (byte_t*)buf, index));
}

/*
=======================================
    Holidays 获取免费时间
=======================================
*/
JNIEXPORT jint JNICALL
Java_com_BlowJobs_Holidays_freeGet (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   hdays,
  __CR_IN__ jstring date,
  __CR_IN__ jint    rule_id,
  __CR_IN__ jint    week_id,
  __CR_IN__ jint    holi_id
    )
{
    jint back;
    const char *s_date = (*env)->GetStringUTFChars(env, date, NULL);

    back = Holidays_FreeGet(JNI_JLONG2PTR(hdays), s_date,
                            rule_id, week_id, holi_id);

    (*env)->ReleaseStringUTFChars(env, date, s_date);
    return (back);
}

/*
=======================================
    Holidays 计算停车费用
=======================================
*/
JNIEXPORT jint JNICALL
Java_com_BlowJobs_Holidays_money (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   hdays,
  __CR_IN__ jstring leave,
  __CR_IN__ jstring enter,
  __CR_IN__ jstring mbeg,
  __CR_IN__ jstring mend,
  __CR_IN__ jint    is_pre,
  __CR_IN__ jint    rule_id,
  __CR_IN__ jint    rule_wid,
  __CR_IN__ jint    rule_hid,
  __CR_IN__ jint    month_id,
  __CR_IN__ jint    month_wid,
  __CR_IN__ jint    month_hid
    )
{
    jint back;
    const char *s_mbeg, *s_mend;
    const char *s_leave = (*env)->GetStringUTFChars(env, leave, NULL);
    const char *s_enter = (*env)->GetStringUTFChars(env, enter, NULL);

    if (mbeg != NULL)
        s_mbeg = (*env)->GetStringUTFChars(env, mbeg, NULL);
    else
        s_mbeg = NULL;
    if (mend != NULL)
        s_mend = (*env)->GetStringUTFChars(env, mend, NULL);
    else
        s_mend = NULL;

    back = Holidays_Money(JNI_JLONG2PTR(hdays), s_leave, s_enter,
                          s_mbeg, s_mend, is_pre, rule_id, rule_wid,
                          rule_hid, month_id, month_wid, month_hid);
    if (mend != NULL)
        (*env)->ReleaseStringUTFChars(env, mend, s_mend);
    if (mbeg != NULL)
        (*env)->ReleaseStringUTFChars(env, mbeg, s_mbeg);
    (*env)->ReleaseStringUTFChars(env, enter, s_enter);
    (*env)->ReleaseStringUTFChars(env, leave, s_leave);
    return (back);
}

/*
=======================================
    Holidays 获取剩余时间
=======================================
*/
JNIEXPORT jint JNICALL
Java_com_BlowJobs_Holidays_rests (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   hdays,
  __CR_IN__ jstring leave,
  __CR_IN__ jstring enter,
  __CR_IN__ jstring mbeg,
  __CR_IN__ jstring mend,
  __CR_IN__ jint    is_pre,
  __CR_IN__ jint    rule_id,
  __CR_IN__ jint    rule_wid,
  __CR_IN__ jint    rule_hid,
  __CR_IN__ jint    month_id,
  __CR_IN__ jint    month_wid,
  __CR_IN__ jint    month_hid
    )
{
    jint back;
    const char *s_mbeg, *s_mend;
    const char *s_leave = (*env)->GetStringUTFChars(env, leave, NULL);
    const char *s_enter = (*env)->GetStringUTFChars(env, enter, NULL);

    if (mbeg != NULL)
        s_mbeg = (*env)->GetStringUTFChars(env, mbeg, NULL);
    else
        s_mbeg = NULL;
    if (mend != NULL)
        s_mend = (*env)->GetStringUTFChars(env, mend, NULL);
    else
        s_mend = NULL;

    back = Holidays_Rests(JNI_JLONG2PTR(hdays), s_leave, s_enter,
                          s_mbeg, s_mend, is_pre, rule_id, rule_wid,
                          rule_hid, month_id, month_wid, month_hid);
    if (mend != NULL)
        (*env)->ReleaseStringUTFChars(env, mend, s_mend);
    if (mbeg != NULL)
        (*env)->ReleaseStringUTFChars(env, mbeg, s_mbeg);
    (*env)->ReleaseStringUTFChars(env, enter, s_enter);
    (*env)->ReleaseStringUTFChars(env, leave, s_leave);
    return (back);
}

/*
=======================================
    Holidays 获取下个缴费点
=======================================
*/
JNIEXPORT jint JNICALL
Java_com_BlowJobs_Holidays_stepa (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   hdays,
  __CR_IN__ jstring leave,
  __CR_IN__ jstring enter,
  __CR_IN__ jstring mbeg,
  __CR_IN__ jstring mend,
  __CR_IN__ jint    is_pre,
  __CR_IN__ jint    rule_id,
  __CR_IN__ jint    rule_wid,
  __CR_IN__ jint    rule_hid,
  __CR_IN__ jint    month_id,
  __CR_IN__ jint    month_wid,
  __CR_IN__ jint    month_hid
    )
{
    jint back;
    const char *s_mbeg, *s_mend;
    const char *s_leave = (*env)->GetStringUTFChars(env, leave, NULL);
    const char *s_enter = (*env)->GetStringUTFChars(env, enter, NULL);

    if (mbeg != NULL)
        s_mbeg = (*env)->GetStringUTFChars(env, mbeg, NULL);
    else
        s_mbeg = NULL;
    if (mend != NULL)
        s_mend = (*env)->GetStringUTFChars(env, mend, NULL);
    else
        s_mend = NULL;

    back = Holidays_Stepa(JNI_JLONG2PTR(hdays), s_leave, s_enter,
                          s_mbeg, s_mend, is_pre, rule_id, rule_wid,
                          rule_hid, month_id, month_wid, month_hid);
    if (mend != NULL)
        (*env)->ReleaseStringUTFChars(env, mend, s_mend);
    if (mbeg != NULL)
        (*env)->ReleaseStringUTFChars(env, mbeg, s_mbeg);
    (*env)->ReleaseStringUTFChars(env, enter, s_enter);
    (*env)->ReleaseStringUTFChars(env, leave, s_leave);
    return (back);
}

/*
=======================================
    Holidays 计算停车费用 (简化版)
=======================================
*/
JNIEXPORT jint JNICALL
Java_com_BlowJobs_Holidays_money2 (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   hdays,
  __CR_IN__ jstring leave,
  __CR_IN__ jstring enter,
  __CR_IN__ jint    is_pre,
  __CR_IN__ jint    rule_id,
  __CR_IN__ jint    week_id,
  __CR_IN__ jint    holi_id
    )
{
    jint back;
    const char *s_leave = (*env)->GetStringUTFChars(env, leave, NULL);
    const char *s_enter = (*env)->GetStringUTFChars(env, enter, NULL);

    back = Holidays_Money2(JNI_JLONG2PTR(hdays), s_leave, s_enter,
                           is_pre, rule_id, week_id, holi_id);

    (*env)->ReleaseStringUTFChars(env, enter, s_enter);
    (*env)->ReleaseStringUTFChars(env, leave, s_leave);
    return (back);
}

/*
=======================================
    Holidays 获取剩余时间 (简化版)
=======================================
*/
JNIEXPORT jint JNICALL
Java_com_BlowJobs_Holidays_rests2 (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   hdays,
  __CR_IN__ jstring leave,
  __CR_IN__ jstring enter,
  __CR_IN__ jint    is_pre,
  __CR_IN__ jint    rule_id,
  __CR_IN__ jint    week_id,
  __CR_IN__ jint    holi_id
    )
{
    jint back;
    const char *s_leave = (*env)->GetStringUTFChars(env, leave, NULL);
    const char *s_enter = (*env)->GetStringUTFChars(env, enter, NULL);

    back = Holidays_Rests2(JNI_JLONG2PTR(hdays), s_leave, s_enter,
                           is_pre, rule_id, week_id, holi_id);

    (*env)->ReleaseStringUTFChars(env, enter, s_enter);
    (*env)->ReleaseStringUTFChars(env, leave, s_leave);
    return (back);
}

/*
=======================================
    Holidays 获取下个缴费点 (简化版)
=======================================
*/
JNIEXPORT jint JNICALL
Java_com_BlowJobs_Holidays_stepa2 (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   hdays,
  __CR_IN__ jstring leave,
  __CR_IN__ jstring enter,
  __CR_IN__ jint    is_pre,
  __CR_IN__ jint    rule_id,
  __CR_IN__ jint    week_id,
  __CR_IN__ jint    holi_id
    )
{
    jint back;
    const char *s_leave = (*env)->GetStringUTFChars(env, leave, NULL);
    const char *s_enter = (*env)->GetStringUTFChars(env, enter, NULL);

    back = Holidays_Stepa2(JNI_JLONG2PTR(hdays), s_leave, s_enter,
                           is_pre, rule_id, week_id, holi_id);

    (*env)->ReleaseStringUTFChars(env, enter, s_enter);
    (*env)->ReleaseStringUTFChars(env, leave, s_leave);
    return (back);
}

/*
=======================================
    Holidays 获取免费时间 (加强版)
=======================================
*/
JNIEXPORT jint JNICALL
Java_com_BlowJobs_Holidays_freeGetEx (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   hdays,
  __CR_IN__ jstring date,
  __CR_IN__ jstring range,
  __CR_IN__ jint    rule_id,
  __CR_IN__ jint    week_id,
  __CR_IN__ jint    holi_id
    )
{
    jint back;
    const char *s_range;
    const char *s_date = (*env)->GetStringUTFChars(env, date, NULL);

    if (range != NULL)
        s_range = (*env)->GetStringUTFChars(env, range, NULL);
    else
        s_range = NULL;

    back = Holidays_FreeGetEx(JNI_JLONG2PTR(hdays), s_date, s_range,
                              rule_id, week_id, holi_id);
    if (range != NULL)
        (*env)->ReleaseStringUTFChars(env, range, s_range);
    (*env)->ReleaseStringUTFChars(env, date, s_date);
    return (back);
}

/*
=======================================
    Holidays 计算停车费用 (加强版)
=======================================
*/
JNIEXPORT jint JNICALL
Java_com_BlowJobs_Holidays_moneyEx (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   hdays,
  __CR_IN__ jstring leave,
  __CR_IN__ jstring enter,
  __CR_IN__ jstring range,
  __CR_IN__ jint    is_pre,
  __CR_IN__ jint    rule_id,
  __CR_IN__ jint    week_id,
  __CR_IN__ jint    holi_id
    )
{
    jint back;
    const char *s_range;
    const char *s_leave = (*env)->GetStringUTFChars(env, leave, NULL);
    const char *s_enter = (*env)->GetStringUTFChars(env, enter, NULL);

    if (range != NULL)
        s_range = (*env)->GetStringUTFChars(env, range, NULL);
    else
        s_range = NULL;

    back = Holidays_MoneyEx(JNI_JLONG2PTR(hdays), s_leave, s_enter,
                            s_range, is_pre, rule_id, week_id, holi_id);
    if (range != NULL)
        (*env)->ReleaseStringUTFChars(env, range, s_range);
    (*env)->ReleaseStringUTFChars(env, enter, s_enter);
    (*env)->ReleaseStringUTFChars(env, leave, s_leave);
    return (back);
}

/*
=======================================
    Holidays 获取剩余时间 (加强版)
=======================================
*/
JNIEXPORT jint JNICALL
Java_com_BlowJobs_Holidays_restsEx (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   hdays,
  __CR_IN__ jstring leave,
  __CR_IN__ jstring enter,
  __CR_IN__ jstring range,
  __CR_IN__ jint    is_pre,
  __CR_IN__ jint    rule_id,
  __CR_IN__ jint    week_id,
  __CR_IN__ jint    holi_id
    )
{
    jint back;
    const char *s_range;
    const char *s_leave = (*env)->GetStringUTFChars(env, leave, NULL);
    const char *s_enter = (*env)->GetStringUTFChars(env, enter, NULL);

    if (range != NULL)
        s_range = (*env)->GetStringUTFChars(env, range, NULL);
    else
        s_range = NULL;

    back = Holidays_RestsEx(JNI_JLONG2PTR(hdays), s_leave, s_enter,
                            s_range, is_pre, rule_id, week_id, holi_id);
    if (range != NULL)
        (*env)->ReleaseStringUTFChars(env, range, s_range);
    (*env)->ReleaseStringUTFChars(env, enter, s_enter);
    (*env)->ReleaseStringUTFChars(env, leave, s_leave);
    return (back);
}

/*
=======================================
    Holidays 获取下个缴费点 (加强版)
=======================================
*/
JNIEXPORT jint JNICALL
Java_com_BlowJobs_Holidays_stepaEx (
  __CR_IN__ JNIEnv* env,
  __CR_IN__ jclass  thiz,
  __CR_IN__ jlong   hdays,
  __CR_IN__ jstring leave,
  __CR_IN__ jstring enter,
  __CR_IN__ jstring range,
  __CR_IN__ jint    is_pre,
  __CR_IN__ jint    rule_id,
  __CR_IN__ jint    week_id,
  __CR_IN__ jint    holi_id
    )
{
    jint back;
    const char *s_range;
    const char *s_leave = (*env)->GetStringUTFChars(env, leave, NULL);
    const char *s_enter = (*env)->GetStringUTFChars(env, enter, NULL);

    if (range != NULL)
        s_range = (*env)->GetStringUTFChars(env, range, NULL);
    else
        s_range = NULL;

    back = Holidays_StepaEx(JNI_JLONG2PTR(hdays), s_leave, s_enter,
                            s_range, is_pre, rule_id, week_id, holi_id);
    if (range != NULL)
        (*env)->ReleaseStringUTFChars(env, range, s_range);
    (*env)->ReleaseStringUTFChars(env, enter, s_enter);
    (*env)->ReleaseStringUTFChars(env, leave, s_leave);
    return (back);
}
