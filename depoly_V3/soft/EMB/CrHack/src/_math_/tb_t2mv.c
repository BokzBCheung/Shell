/*****************************************************************************/
/*                                                  ###                      */
/*       #####          ###    ###                  ###  CREATE: 2011-11-23  */
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
/*  >>>>>>>>>>>>>>>>>>>> CrHack B 分度温度转毫伏函数库 <<<<<<<<<<<<<<<<<<<<  */
/*  =======================================================================  */
/*****************************************************************************/

#include "phylib.h"

/* 0 - 630.615 (摄氏度) */
#define N_COE1  7
static const double _rom_ s_coe1[N_COE1] =
{
    0.000000000000E+00, -0.246508183460E-03,
    0.590404211710E-05, -0.132579316360E-08,
    0.156682919010E-11, -0.169445292400E-14,
    0.629903470940E-18,
};

/* 630.615 - 1820 (摄氏度) */
#define N_COE2  9
static const double _rom_ s_coe2[N_COE2] =
{
    -0.389381686210E+01, 0.285717474700E-01,
    -0.848851047850E-04, 0.157852801640E-06,
    -0.168353448640E-09, 0.111097940130E-12,
    -0.445154310330E-16, 0.989756408210E-20,
    -0.937913302890E-24,
};

/*
=======================================
    B 分度温度转毫伏
=======================================
*/
CR_API double
type_b_t2mv (
  __CR_IN__ double  t
    )
{
    double  mv;
    ufast_t ii;

    if (t < 0.0 || t > 1820.0)
        return (CR_PHY_INV);
    if (t < 630.615) {
        mv = s_coe1[N_COE1 - 1] * t;
        for (ii = N_COE1 - 2; ii != 0; ii--)
            mv = (mv + s_coe1[ii]) * t;
        mv += s_coe1[0];
    }
    else {
        mv = s_coe2[N_COE2 - 1] * t;
        for (ii = N_COE2 - 2; ii != 0; ii--)
            mv = (mv + s_coe2[ii]) * t;
        mv += s_coe2[0];
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
