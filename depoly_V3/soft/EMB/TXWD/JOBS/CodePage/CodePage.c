/*
 *********************************
 *  CodePage 自定义编码转换库
 *
 *  创建时间: 2016-05-10
 *********************************
 */

#include "crhack.h"
#define _USE_LFN    1
#include "util/codepage.h"

/* CodePage 导入库 */
#ifndef _CR_NO_PRAGMA_LIB_
    #pragma comment (lib, "COREs.lib")
    #pragma comment (lib, "UTILs.lib")
#endif

#if defined(_CR_OS_MSWIN_)
/*
=======================================
    CodePage DLL 入口点
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
void_t  so_cpage_init (void_t) __attribute__((constructor));
void_t  so_cpage_init (void_t)
{
    crhack_core_init();
}
void_t  so_cpage_fini (void_t) __attribute__((destructor));
void_t  so_cpage_fini (void_t)
{
    crhack_core_free();
}
#endif  /* _CR_OS_MSWIN_ */

/*
=======================================
    设置本地编码
=======================================
*/
CR_DLL void_t
local_cpage (
  __CR_IN__ uint_t  cpage
    )
{
    set_sys_codepage(cpage);
}

/*
=======================================
    多字节转 UNICODE
=======================================
*/
CR_DLL void_t*
str_ccp2uni (
  __CR_IN__ uint_t          codepage,
  __CR_IN__ const ansi_t*   str,
  __CR_OT__ leng_t*         size,
  __CR_IN__ bool_t          use_utf16
    )
{
    ansi_t* utf8;
    wide_t* wide;

    /* 输入必须是多字节编码 */
    if (is_cr_widechar(codepage))
        return (NULL);
    if (codepage == CR_LOCAL)
        codepage = get_sys_codepage();

    /* 转换到 UTF-16 编码 */
    switch (codepage)
    {
        default: return (NULL);
        case CR_LOCAL: wide = str_char2wide(str); break;
        case CR_UTF8: wide = utf8_to_utf16(str); break;
        case 932: wide = cp932_to_utf16(str); break;
        case 936: wide = cp936_to_utf16(str); break;
        case 949: wide = cp949_to_utf16(str); break;
        case 950: wide = cp950_to_utf16(str); break;
        case 437: wide = cp437_to_utf16(str); break;
        case 720: wide = cp720_to_utf16(str); break;
        case 737: wide = cp737_to_utf16(str); break;
        case 771: wide = cp771_to_utf16(str); break;
        case 775: wide = cp775_to_utf16(str); break;
        case 850: wide = cp850_to_utf16(str); break;
        case 852: wide = cp852_to_utf16(str); break;
        case 855: wide = cp855_to_utf16(str); break;
        case 857: wide = cp857_to_utf16(str); break;
        case 860: wide = cp860_to_utf16(str); break;
        case 861: wide = cp861_to_utf16(str); break;
        case 862: wide = cp862_to_utf16(str); break;
        case 863: wide = cp863_to_utf16(str); break;
        case 864: wide = cp864_to_utf16(str); break;
        case 865: wide = cp865_to_utf16(str); break;
        case 866: wide = cp866_to_utf16(str); break;
        case 869: wide = cp869_to_utf16(str); break;
    }

    /* 直接返回 */
    if (use_utf16) {
        if (wide == NULL)
            return (NULL);
        if (size != NULL)
            *size = str_sizeW(wide);
        return (wide);
    }

    /* 转换到 UTF-8 编码 */
    utf8 = utf16_to_utf8(wide);
    mem_free(wide);
    if (utf8 == NULL)
        return (NULL);
    if (size != NULL)
        *size = str_sizeA(utf8);
    return (utf8);
}

/*
=======================================
    UNICODE 转多字节
=======================================
*/
CR_DLL ansi_t*
str_uni2ccp (
  __CR_IN__ uint_t          codepage,
  __CR_IN__ const void_t*   str,
  __CR_OT__ leng_t*         size,
  __CR_IN__ bool_t          use_utf16
    )
{
    ansi_t* ansi;
    wide_t* wide;

    /* 输入必须是多字节编码 */
    if (is_cr_widechar(codepage))
        return (NULL);
    if (codepage == CR_LOCAL)
        codepage = get_sys_codepage();

    if (!use_utf16)
    {
        /* 从 UTF-8 转出 */
        wide = utf8_to_utf16((ansi_t*)str);
        if (wide == NULL)
            return (NULL);
    }
    else
    {
        /* 从 UTF-16 转出 */
        wide = (wide_t*)str;
    }

    switch (codepage)
    {
        default: goto _failure;
        case CR_LOCAL: ansi = str_wide2char(wide); break;
        case CR_UTF8: ansi = utf16_to_utf8(wide); break;
        case 932: ansi = utf16_to_cp932(wide); break;
        case 936: ansi = utf16_to_cp936(wide); break;
        case 949: ansi = utf16_to_cp949(wide); break;
        case 950: ansi = utf16_to_cp950(wide); break;
        case 437: ansi = utf16_to_cp437(wide); break;
        case 720: ansi = utf16_to_cp720(wide); break;
        case 737: ansi = utf16_to_cp737(wide); break;
        case 771: ansi = utf16_to_cp771(wide); break;
        case 775: ansi = utf16_to_cp775(wide); break;
        case 850: ansi = utf16_to_cp850(wide); break;
        case 852: ansi = utf16_to_cp852(wide); break;
        case 855: ansi = utf16_to_cp855(wide); break;
        case 857: ansi = utf16_to_cp857(wide); break;
        case 860: ansi = utf16_to_cp860(wide); break;
        case 861: ansi = utf16_to_cp861(wide); break;
        case 862: ansi = utf16_to_cp862(wide); break;
        case 863: ansi = utf16_to_cp863(wide); break;
        case 864: ansi = utf16_to_cp864(wide); break;
        case 865: ansi = utf16_to_cp865(wide); break;
        case 866: ansi = utf16_to_cp866(wide); break;
        case 869: ansi = utf16_to_cp869(wide); break;
    }

    if (ansi == NULL)
        goto _failure;
    if (size != NULL)
        *size = str_sizeA(ansi);
    if (!use_utf16)
        mem_free(wide);
    return (ansi);

_failure:
    if (!use_utf16)
        mem_free(wide);
    return (NULL);
}
