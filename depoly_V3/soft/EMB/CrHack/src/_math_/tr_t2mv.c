/*****************************************************************************/
/*                                                  ###                      */
/*       #####          ###    ###                  ###  CREATE: 2011-11-24  */
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
/*  >>>>>>>>>>>>>>>>>>>> CrHack R 分度温度转毫伏函数库 <<<<<<<<<<<<<<<<<<<<  */
/*  =======================================================================  */
/*****************************************************************************/

#include "phylib.h"

/* -50 - 1064.18 (摄氏度) */
#define N_COE1  10
static const double _rom_ s_coe1[N_COE1] =
{
    0.000000000000E+00,  0.528961729765E-02,
    0.139166589782E-04, -0.238855693017E-07,
    0.356916001063E-10, -0.462347666298E-13,
    0.500777441034E-16, -0.373105886191E-19,
    0.157716482367E-22, -0.281038625251E-26,
};

/* 1064.18 - 1664.5 (摄氏度) */
#define N_COE2  6
static const double _rom_ s_coe2[N_COE2] =
{
    0.295157925316E+01, -0.252061251332E-02,
    0.159564501865E-04, -0.764085947576E-08,
    0.205305291024E-11, -0.293359668173E-15,
};

/* 1664.5 - 1768.1 (摄氏度) */
#define N_COE3  5
static const double _rom_ s_coe3[N_COE3] =
{
     0.152232118209E+03, -0.268819888545E+00,
     0.171280280471E-03, -0.345895706453E-07,
    -0.934633971046E-14,
};

/*
=======================================
    R 分度温度转毫伏
=======================================
*/
CR_API double
type_r_t2mv (
  __CR_IN__ double  t
    )
{
    double  mv;
    ufast_t ii;

    if (t < -50.0 || t > 1768.1)
        return (CR_PHY_INV);
    if (t < 1064.18) {
        mv = s_coe1[N_COE1 - 1] * t;
        for (ii = N_COE1 - 2; ii != 0; ii--)
            mv = (mv + s_coe1[ii]) * t;
        mv += s_coe1[0];
    }
    else
    if (t < 1664.5) {
        mv = s_coe2[N_COE2 - 1] * t;
        for (ii = N_COE2 - 2; ii != 0; ii--)
            mv = (mv + s_coe2[ii]) * t;
        mv += s_coe2[0];
    }
    else {
        mv = s_coe3[N_COE3 - 1] * t;
        for (ii = N_COE3 - 2; ii != 0; ii--)
            mv = (mv + s_coe3[ii]) * t;
        mv += s_coe3[0];
    }
    return (mv);
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