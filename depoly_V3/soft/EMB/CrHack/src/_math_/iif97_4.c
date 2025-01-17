/*****************************************************************************/
/*                                                  ###                      */
/*       #####          ###    ###                  ###  CREATE: 2012-01-13  */
/*     #######          ###    ###      [MATH]      ###  ~~~~~~~~~~~~~~~~~~  */
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
/*  >>>>>>>>>>>>>>>>>>>>> CrHack IAPWS-IF97 四区函数库 <<<<<<<<<<<<<<<<<<<<  */
/*  =======================================================================  */
/*****************************************************************************/

#include "phylib.h"

#ifndef _CR_NO_STDC_
    #if defined(_CR_OS_ANDROID_) && (ANDROID_API <= 19)
        #if defined(__pure2)
            #undef  __pure2
        #endif
    #endif
    #include <math.h>
#endif

/* IAPWS-IF97 四区（饱和区）计算常数 */
#define IIF97_N01   ( 0.11670521452767E+04)
#define IIF97_N02   (-0.72421316703206E+06)
#define IIF97_N03   (-0.17073846940092E+02)
#define IIF97_N04   ( 0.12020824702470E+05)
#define IIF97_N05   (-0.32325550322333E+07)
#define IIF97_N06   ( 0.14915108613530E+02)
#define IIF97_N07   (-0.48232657361591E+04)
#define IIF97_N08   ( 0.40511340542057E+06)
#define IIF97_N09   (-0.23855557567849E+00)
#define IIF97_N10   ( 0.65017534844798E+03)

/*
=======================================
    四区边界温度转压力
=======================================
*/
CR_API double
iif97_4_t2mpa (
  __CR_IN__ double  t
    )
{
    double  mpa;
    double  aa, bb, cc;

    t += 273.15;
    t += IIF97_N09 / (t - IIF97_N10);
    aa = t * (t             + IIF97_N01) + IIF97_N02;
    bb = t * (t * IIF97_N03 + IIF97_N04) + IIF97_N05;
    cc = t * (t * IIF97_N06 + IIF97_N07) + IIF97_N08;
    mpa = (-2.0 * cc) / (bb - sqrt(bb * bb - 4.0 * aa * cc));
    return (mpa * mpa * mpa * mpa);
}

/*
=======================================
    四区边界压力转温度
=======================================
*/
CR_API double
iif97_4_mpa2t (
  __CR_IN__ double  mpa
    )
{
    double  tt, dd;
    double  ee, ff, gg;

    mpa = sqrt(mpa);
    mpa = sqrt(mpa);
    ee = mpa * (mpa             + IIF97_N03) + IIF97_N06;
    ff = mpa * (mpa * IIF97_N01 + IIF97_N04) + IIF97_N07;
    gg = mpa * (mpa * IIF97_N02 + IIF97_N05) + IIF97_N08;
    dd = (-2.0 * gg) / (ff + sqrt(ff * ff - 4.0 * ee * gg));
    ee = IIF97_N10 + dd;
    tt = sqrt(ee * ee - 4.0 * (IIF97_N09 + IIF97_N10 * dd));
    return ((ee - tt) / 2.0 - 273.15);
}

/*****************************************************************************/
/* _________________________________________________________________________ */
/* uBMAzRBoAKAHaACQD6FoAIAPqbgA/7rIA+5CM9uKw8D4Au7u7mSIJ0t18mYz0mYz9rAQZCgHc */
/* wRIZIgHZovGBXUAZg+v0GbB+gRmgcJ7BAAAisIlAwB1Av7LSHUC/s9IdQL+w0h1Av7HZkZmgf */
/* 4JLgIAdb262gPsqAh0+zP/uQB9ZYsFZYktq+L3sMi/AAK7gAKJAUtLdfq5IANXvT8BiQzfBIv */
/* FLaAAweAEmff53wb+Adjx3kQE2xwy5Io8ithkigcFgACJBN8E3sneNvwB2xyLHDkdfA2JHSyA */
/* adtAAQPdZYgHR0dNdbZfSYP5UHWr/kQEtAHNFg+Eef/DWAKgDw== |~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ >>> END OF FILE <<< */
/*****************************************************************************/
