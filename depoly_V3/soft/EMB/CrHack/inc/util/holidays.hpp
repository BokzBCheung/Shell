/*****************************************************************************/
/*                                                  ###                      */
/*       #####          ###    ###                  ###  CREATE: 2016-04-13  */
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
/*  >>>>>>>>>>>>>>>>>>>> Holidays 停车算费头文件 (C++) <<<<<<<<<<<<<<<<<<<<  */
/*  =======================================================================  */
/*****************************************************************************/

#ifndef __CR_HOLIDAYS_HPP__
#define __CR_HOLIDAYS_HPP__

#include "holidays.h"

/* BlowJobs Namespace */
namespace BlowJobs {

/********************/
/* Holidays Wrapper */
/********************/
class   Holidays
{
private:
    holidays_t  m_hdays;

public:
    /* ===================== */
    Holidays (holidays_t hdays)
    {
        m_hdays = hdays;
    }
    ~Holidays ()
    {
        ::holidays_kill(m_hdays);
    }

public:
    /* ======================================== */
    sint_t  ruleSet (const sHOLI_RULE *rule) const
    {
        return (::holidays_rule_set(m_hdays, rule));
    }

    /* ====================================================== */
    uint_t  hdaySet (const sHOLI_DAYS *holi, uint_t count) const
    {
        return (::holidays_hday_set(m_hdays, holi, count));
    }

    /* =================================================== */
    sint_t  weekSet (const byte_t *weeks, uint_t index) const
    {
        return (::holidays_week_set(m_hdays, weeks, index));
    }

    /* ================================================= */
    sint_t  freeGet (const sDATETIME *date, byte_t rule_id,
                     byte_t week_id, byte_t holi_id) const
    {
        return (::holidays_free_get(m_hdays, date, rule_id,
                                    week_id, holi_id));
    }

    /* ======================================================== */
    sint_t  money (const sDATETIME *leave, const sDATETIME *enter,
                   const sDATETIME *mbeg, const sDATETIME *mend,
                   bool_t is_pre, byte_t rule_id, byte_t rule_wid,
                   byte_t rule_hid, byte_t month_id, byte_t month_wid,
                   byte_t month_hid) const
    {
        return (::holidays_money(m_hdays, leave, enter, mbeg, mend,
                                 is_pre, rule_id, rule_wid, rule_hid,
                                 month_id, month_wid, month_hid));
    }

    /* ========================================================= */
    sint_t  money2 (const sDATETIME *leave, const sDATETIME *enter,
                    bool_t is_pre, byte_t rule_id, byte_t week_id,
                    byte_t holi_id) const
    {
        return (::holidays_money(m_hdays, leave, enter, NULL, NULL,
                                 is_pre, rule_id, week_id, holi_id,
                                 0, 0, 0));
    }

    /* ======================================================== */
    sint_t  rests (const sDATETIME *leave, const sDATETIME *enter,
                   const sDATETIME *mbeg, const sDATETIME *mend,
                   bool_t is_pre, byte_t rule_id, byte_t rule_wid,
                   byte_t rule_hid, byte_t month_id, byte_t month_wid,
                   byte_t month_hid) const
    {
        return (::holidays_rests(m_hdays, leave, enter, mbeg, mend,
                                 is_pre, rule_id, rule_wid, rule_hid,
                                 month_id, month_wid, month_hid));
    }

    /* ========================================================= */
    sint_t  rests2 (const sDATETIME *leave, const sDATETIME *enter,
                    bool_t is_pre, byte_t rule_id, byte_t week_id,
                    byte_t holi_id) const
    {
        return (::holidays_rests(m_hdays, leave, enter, NULL, NULL,
                                 is_pre, rule_id, week_id, holi_id,
                                 0, 0, 0));
    }

    /* ======================================================== */
    sint_t  stepa (const sDATETIME *leave, const sDATETIME *enter,
                   const sDATETIME *mbeg, const sDATETIME *mend,
                   bool_t is_pre, byte_t rule_id, byte_t rule_wid,
                   byte_t rule_hid, byte_t month_id, byte_t month_wid,
                   byte_t month_hid) const
    {
        return (::holidays_stepa(m_hdays, leave, enter, mbeg, mend,
                                 is_pre, rule_id, rule_wid, rule_hid,
                                 month_id, month_wid, month_hid));
    }

    /* ========================================================= */
    sint_t  stepa2 (const sDATETIME *leave, const sDATETIME *enter,
                    bool_t is_pre, byte_t rule_id, byte_t week_id,
                    byte_t holi_id) const
    {
        return (::holidays_stepa(m_hdays, leave, enter, NULL, NULL,
                                 is_pre, rule_id, week_id, holi_id,
                                 0, 0, 0));
    }

    /* ============================================================ */
    sint_t  freeGetEx (const sDATETIME *date, const sHOLI_RNGS *range,
                       uint_t count, byte_t rule_id, byte_t week_id,
                       byte_t holi_id) const
    {
        return (::holidays_free_get2(m_hdays, date, range, count, rule_id,
                                     week_id, holi_id));
    }

    /* ========================================================== */
    sint_t  moneyEx (const sDATETIME *leave, const sDATETIME *enter,
                     const sHOLI_RNGS *range, uint_t count, bool_t is_pre,
                     byte_t rule_id, byte_t week_id, byte_t holi_id) const
    {
        return (::holidays_money2(m_hdays, leave, enter, range, count,
                                  is_pre, rule_id, week_id, holi_id));
    }

    /* ========================================================== */
    sint_t  restsEx (const sDATETIME *leave, const sDATETIME *enter,
                     const sHOLI_RNGS *range, uint_t count, bool_t is_pre,
                     byte_t rule_id, byte_t week_id, byte_t holi_id) const
    {
        return (::holidays_rests2(m_hdays, leave, enter, range, count,
                                  is_pre, rule_id, week_id, holi_id));
    }

    /* ========================================================== */
    sint_t  stepaEx (const sDATETIME *leave, const sDATETIME *enter,
                     const sHOLI_RNGS *range, uint_t count, bool_t is_pre,
                     byte_t rule_id, byte_t week_id, byte_t holi_id) const
    {
        return (::holidays_stepa2(m_hdays, leave, enter, range, count,
                                  is_pre, rule_id, week_id, holi_id));
    }
};

/*
=======================================
    创建 Holidays 对象
=======================================
*/
cr_inline Holidays*
HolidaysCreate (void_t)
{
    Holidays*   obj;
    holidays_t  hdays;

    hdays = ::holidays_init();
    if (hdays == NULL)
        return (NULL);
    obj = new Holidays (hdays);
    if (obj == NULL)
        ::holidays_kill(hdays);
    return (obj);
}

}   /* namespace */

#endif  /* !__CR_HOLIDAYS_HPP__ */

/*****************************************************************************/
/* _________________________________________________________________________ */
/* uBMAzRBoAKAHaACQD6FoAIAPqbgA/7rIA+5CM9uKw8D4Au7u7mSIJ0t18mYz0mYz9rAQZCgHc */
/* wRIZIgHZovGBXUAZg+v0GbB+gRmgcJ7BAAAisIlAwB1Av7LSHUC/s9IdQL+w0h1Av7HZkZmgf */
/* 4JLgIAdb262gPsqAh0+zP/uQB9ZYsFZYktq+L3sMi/AAK7gAKJAUtLdfq5IANXvT8BiQzfBIv */
/* FLaAAweAEmff53wb+Adjx3kQE2xwy5Io8ithkigcFgACJBN8E3sneNvwB2xyLHDkdfA2JHSyA */
/* adtAAQPdZYgHR0dNdbZfSYP5UHWr/kQEtAHNFg+Eef/DWAKgDw== |~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ >>> END OF FILE <<< */
/*****************************************************************************/
