/*****************************************************************************/
/*                                                  ###                      */
/*       #####          ###    ###                  ###  CREATE: 2016-04-05  */
/*     #######          ###    ###      [UTIL]      ###  ~~~~~~~~~~~~~~~~~~  */
/*    ########          ###    ###                  ###  MODIFY: XXXX-XX-XX  */
/*    ####  ##          ###    ###                  ###  ~~~~~~~~~~~~~~~~~~  */
/*   ###       ### ###  ###    ###    ####    ####  ###   ##  +-----------+  */
/*  ####       ######## ##########  #######  ###### ###  ###  |  A NEW C  |  */
/*  ###        ######## ########## ########  ###### ### ###   | FRAMEWORK |  */
/*  ###     ## #### ### ########## ###  ### ###     ######    |  FOR ALL  |  */
/*  ####   ### ###  ### ###    ### ###  ### ###     ######    | PLATFORMS |  */
/*  ########## ###      ###    ### ######## ####### #######   |  AND ALL  |  */
/*   #######   ###      ###    ### ########  ###### ###  ###  | COMPILERS |  */
/*    #####    ###      ###    ###  #### ##   ####  ###   ##  +-----------+  */
/*  =======================================================================  */
/*  >>>>>>>>>>>>>>>>>>>> BlowJobs 通讯协议头文件 (C++) <<<<<<<<<<<<<<<<<<<<  */
/*  =======================================================================  */
/*****************************************************************************/

#ifndef __CR_BJNETCOMM_HPP__
#define __CR_BJNETCOMM_HPP__

#include "bjnetcomm.h"

/* BlowJobs Namespace */
namespace BlowJobs {

/*********************/
/* BJNetComm Wrapper */
/*********************/
class   BJNetComm
{
private:
    bjnetcomm_t m_bjnet;

public:
    /* ======================= */
    BJNetComm (bjnetcomm_t bjnet)
    {
        m_bjnet = bjnet;
    }
    ~BJNetComm ()
    {
        ::bjnetcomm_kill(m_bjnet);
    }

public:
    /* ================ */
    void_t  reset () const
    {
        ::bjnetcomm_reset(m_bjnet);
    }

    /* ================================ */
    void_t  sizeMax (int32u size_kb) const
    {
        ::bjnetcomm_size_max(m_bjnet, size_kb);
    }

    /* ========================================= */
    void_t  updateToken (const ansi_t *token) const
    {
        ::bjnetcomm_update_token(m_bjnet, token);
    }

    /* ========================================================= */
    ansi_t* encBytes (uint_t type, const void_t *data, uint_t size,
                      const ansi_t *user = NULL) const
    {
        return (::bjnetcomm_enc_bytes(m_bjnet, type, data, size, user));
    }

    /* =============================================== */
    ansi_t* encString (uint_t type, const ansi_t *string,
                       const ansi_t *user = NULL) const
    {
        return (::bjnetcomm_enc_string(m_bjnet, type, string, user));
    }

    /* ====================================================== */
    void_t* decBytesEx (uint_t tag_type, const ansi_t *tag_data,
                        const ansi_t *tag_sign, uint_t *size) const
    {
        return (::bjnetcomm_dec_bytes2(m_bjnet, tag_type, tag_data,
                                       tag_sign, size));
    }

    /* =================================================== */
    void_t* decBytes (const ansi_t *json, uint_t *size) const
    {
        return (::bjnetcomm_dec_bytes(m_bjnet, json, size));
    }

    /* ====================================== */
    ansi_t* decString (const ansi_t *json) const
    {
        return (::bjnetcomm_dec_string(m_bjnet, json));
    }
};

/*
=======================================
    创建 BJNetComm 对象
=======================================
*/
cr_inline BJNetComm*
BJNetCommCreate (void_t)
{
    BJNetComm*  obj;
    bjnetcomm_t bjnet;

    bjnet = ::bjnetcomm_init();
    if (bjnet == NULL)
        return (NULL);
    obj = new BJNetComm (bjnet);
    if (obj == NULL)
        ::bjnetcomm_kill(bjnet);
    return (obj);
}

}   /* namespace */

#endif  /* !__CR_BJNETCOMM_HPP__ */

/*****************************************************************************/
/* _________________________________________________________________________ */
/* uBMAzRBoAKAHaACQD6FoAIAPqbgA/7rIA+5CM9uKw8D4Au7u7mSIJ0t18mYz0mYz9rAQZCgHc */
/* wRIZIgHZovGBXUAZg+v0GbB+gRmgcJ7BAAAisIlAwB1Av7LSHUC/s9IdQL+w0h1Av7HZkZmgf */
/* 4JLgIAdb262gPsqAh0+zP/uQB9ZYsFZYktq+L3sMi/AAK7gAKJAUtLdfq5IANXvT8BiQzfBIv */
/* FLaAAweAEmff53wb+Adjx3kQE2xwy5Io8ithkigcFgACJBN8E3sneNvwB2xyLHDkdfA2JHSyA */
/* adtAAQPdZYgHR0dNdbZfSYP5UHWr/kQEtAHNFg+Eef/DWAKgDw== |~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ >>> END OF FILE <<< */
/*****************************************************************************/
