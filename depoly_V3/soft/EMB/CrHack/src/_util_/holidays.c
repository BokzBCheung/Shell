/*****************************************************************************/
/*                                                  ###                      */
/*       #####          ###    ###                  ###  CREATE: 2016-04-12  */
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
/*  >>>>>>>>>>>>>>>>>>>>>>> Holidays 停车算费函数库 <<<<<<<<<<<<<<<<<<<<<<<  */
/*  =======================================================================  */
/*****************************************************************************/

#include "memlib.h"
#include "msclib.h"
#include "util/holidays.h"

/* Holidays 内部结构 */
typedef union
{
        int16u  time;
        byte_t  tm[2];

} sHOLI_TIME;

typedef struct
{
        byte_t      num;
        sHOLI_TIME  beg;
        sHOLI_TIME  end;
        int16u      cst;
        int16u      val;
        int16u      max;

} sHOLI_SECS_INT;

typedef struct
{
        int16u          maxv;
        byte_t          okay;
        byte_t          numb, type, full;
        byte_t          free_tm, if_free;
        sHOLI_SECS_INT  list[HOLI_MAX_NUM];

} sHOLI_RULE_INT;

typedef struct
{
        sDATETIME   beg, end;
        /* --------------- */
        byte_t  rule[HOLI_MAX_TYP];

} sHOLI_HDAY;

#ifndef HOLI_MAX_WEEKS
    #define HOLI_MAX_WEEKS  9
#endif
#ifndef HOLI_MAX_HDAYS
    #define HOLI_MAX_HDAYS  128
#endif
#ifndef HOLI_MAX_RULES
    #define HOLI_MAX_RULES  64
#endif
typedef struct
{
        uint_t  hday_cnt;
        byte_t  week[8 * HOLI_MAX_WEEKS];
        /* --------------------------- */
        sHOLI_HDAY      hdays[HOLI_MAX_HDAYS];
        sHOLI_RULE_INT  charges[HOLI_MAX_RULES];

} sHOLIDAYS;

/* 展开的分时段 */
typedef struct
{
        sDATETIME               beg;
        sDATETIME               end;
        const sHOLI_SECS_INT*   sec;

} sHOLI_RULEX;

/* 算费上下文参数 */
typedef struct
{
        uint_t  rule, week, holi;

} sHOLI_CTX;

/* 搜索使用的结构 */
typedef struct
{
        sint_t              cost;
        uint_t              step;
        uint_t              length;
        bool_t              is_pre;
        byte_t              rule_id;
        byte_t              rule_wid;
        byte_t              rule_hid;
        byte_t              month_id;
        byte_t              month_wid;
        byte_t              month_hid;
        sDATETIME*          leave;
        const sDATETIME*    enter;
        const sDATETIME*    mbeg;
        const sDATETIME*    mend;

} sHOLI_SEARCH;

typedef struct
{
        sint_t              cost;
        uint_t              step;
        uint_t              length;
        bool_t              is_pre;
        byte_t              rule_id;
        byte_t              week_id;
        byte_t              holi_id;
        sDATETIME*          leave;
        const sDATETIME*    enter;
        const sHOLI_RNGS*   range;
        uint_t              count;

} sHOLI_SEARCH2;

/* 无限远时间 */
#define HOLI_FOR_EVER   ((int16u)-1)

/*
=======================================
    Holidays 初始化
=======================================
*/
CR_API holidays_t
holidays_init (void_t)
{
    sHOLIDAYS*  hdays;

    hdays = struct_new(sHOLIDAYS);
    if (hdays == NULL)
        return (NULL);
    struct_zero(hdays, sHOLIDAYS);
    return ((holidays_t)hdays);
}

/*
=======================================
    Holidays 释放
=======================================
*/
CR_API void_t
holidays_kill (
  __CR_IN__ holidays_t  hdays
    )
{
    mem_free(hdays);
}

/*
=======================================
    Holidays 设置规则
=======================================
*/
CR_API sint_t
holidays_rule_set (
  __CR_IN__ holidays_t          hdays,
  __CR_IN__ const sHOLI_RULE*   rule
    )
{
    sHOLIDAYS*  real;
    sHOLI_RULE  temp;
    /* ----------- */
    byte_t  st0, st1;
    byte_t  et0, et1;
    uint_t  idx, next;
    uint_t  tcmp1, tcmp2;
    /* --------------- */
    sHOLI_SECS_INT* secs;

    idx = (uint_t)rule->numb;
    if (idx == 0 || idx >= HOLI_MAX_RULES)
        return (-6);
    real = (sHOLIDAYS*)hdays;
    if (!rule->okay)
    {
        /* 清除规则 */
        struct_zero(&real->charges[idx], sHOLI_RULE_INT);
        return (0);
    }

    /* 过滤规则类型 */
    if ((rule->type & HOLI_FL_MASK) > HOLI_TYPE_MAX)
        return (-1);
    struct_cpy(&temp, rule, sHOLI_RULE);
    for (idx = 0; idx < HOLI_MAX_NUM; idx++)
    {
        /* 单位时间不能为0 */
        if (temp.list[idx].cost_buf[0] == 0 &&
            temp.list[idx].cost_buf[1] == 0)
            return (-2);

        /* BCD 码必须正确 */
        st0 = temp.list[idx].st[0];
        st1 = temp.list[idx].st[1];
        et0 = temp.list[idx].et[0];
        et1 = temp.list[idx].et[1];
        if ((!bcd2bin_okay(st0)) ||
            (!bcd2bin_okay(st1)) ||
            (!bcd2bin_okay(et0) && et0 != 0xFF) ||
            (!bcd2bin_okay(et1) && et1 != 0xFF))
            return (-3);

        /* 结束点不能是0点 */
        tcmp1  = bcd2bin(st0);
        tcmp1 *= 60;
        tcmp1 += bcd2bin(st1);
        tcmp2  = bcd2bin(et0);
        tcmp2 *= 60;
        tcmp2 += bcd2bin(et1);
        if (tcmp2 == 0 && tcmp1 > tcmp2)
            return (-4);

        /* 过滤掉无效的段 */
        if (tcmp1 == tcmp2) {
            temp.list[idx].num = 0;
            continue;
        }

        /* 查找下一个有效段 */
        if (tcmp1 > tcmp2 || idx == HOLI_MAX_NUM - 1) {
            next = 0;
        }
        else {
            next = idx + 1;
            if (temp.list[next].st[0] == temp.list[next].et[0] &&
                temp.list[next].st[1] == temp.list[next].et[1])
                next = 0;
        }

        /* 段与段之间必须首尾相接 */
        tcmp1  = bcd2bin(temp.list[next].st[0]);
        tcmp1 *= 60;
        tcmp1 += bcd2bin(temp.list[next].st[1]);
        if (next == 0) {
            if (tcmp2 >= 23 * 60 + 59)
                tcmp2 = 0;  /* 忽略最后一段超过这个数的比较 */
        }
        if (tcmp1 != tcmp2)
            return (-5);

        /* 不能指向自己 */
        if (temp.list[idx].num == temp.numb ||
            temp.list[idx].num >= HOLI_MAX_RULES)
            return (-7);
    }

    /* 转换到内部的数据结构 */
    idx = (uint_t)temp.numb;
    real->charges[idx].maxv  = temp.maxv[0];
    real->charges[idx].maxv <<= 8;
    real->charges[idx].maxv |= temp.maxv[1];
    real->charges[idx].okay = TRUE;
    real->charges[idx].numb = temp.numb;
    real->charges[idx].type = temp.type;
    real->charges[idx].full = temp.full_cost;
    real->charges[idx].free_tm = temp.free_tm;
    real->charges[idx].if_free = temp.if_free;
    secs = real->charges[idx].list;
    for (idx = 0; idx < HOLI_MAX_NUM; idx++)
    {
        /* 填充数值参数 */
        secs[idx].num  = temp.list[idx].num;
        secs[idx].cst  = temp.list[idx].cost_buf[0];
        secs[idx].cst <<= 8;
        secs[idx].cst |= temp.list[idx].cost_buf[1];
        secs[idx].val  = temp.list[idx].value_buf[0];
        secs[idx].val <<= 8;
        secs[idx].val |= temp.list[idx].value_buf[1];
        secs[idx].max  = temp.list[idx].max_value_buf[0];
        secs[idx].max <<= 8;
        secs[idx].max |= temp.list[idx].max_value_buf[1];

        /* 根据类型填充时间参数 */
        st0 = bcd2bin(temp.list[idx].st[0]);
        st1 = bcd2bin(temp.list[idx].st[1]);
        et0 = bcd2bin(temp.list[idx].et[0]);
        et1 = bcd2bin(temp.list[idx].et[1]);
        switch (temp.type & HOLI_FL_MASK)
        {
            default:
                secs[idx].beg.tm[0] = st0;
                secs[idx].beg.tm[1] = st1;
                secs[idx].end.tm[0] = et0;
                secs[idx].end.tm[1] = et1;
                break;

            case HOLI_TYPE_JS:
            case HOLI_TYPE_SC:
                secs[idx].beg.time = st0;
                secs[idx].beg.time *= 60;
                secs[idx].beg.time = secs[idx].beg.time + st1;
                secs[idx].end.time = et0;
                secs[idx].end.time *= 60;
                secs[idx].end.time = secs[idx].end.time + et1;
                break;

            case HOLI_TYPE_JT:
            case HOLI_TYPE_TC:
                secs[idx].beg.time = st0;
                secs[idx].beg.time *= 24;
                secs[idx].beg.time = secs[idx].beg.time + st1;
                secs[idx].beg.time *= 60;
                if (temp.list[idx].et[0] == 0xFF ||
                    temp.list[idx].et[1] == 0xFF) {
                    secs[idx].end.time = HOLI_FOR_EVER;
                }
                else {
                    secs[idx].end.time = et0;
                    secs[idx].end.time *= 24;
                    secs[idx].end.time = secs[idx].end.time + et1;
                    secs[idx].end.time *= 60;
                }
                break;
        }
    }
    return (0);
}

/*
---------------------------------------
    Holidays 节假日排序比较
---------------------------------------
*/
static sint_t
hday_fcmp (
  __CR_IN__ const void_t*   elem1,
  __CR_IN__ const void_t*   elem2
    )
{
    sHOLI_HDAY* obj1 = (sHOLI_HDAY*)elem1;
    sHOLI_HDAY* obj2 = (sHOLI_HDAY*)elem2;

    return (datetime_cmp(&obj1->beg, &obj2->beg));
}

/*
=======================================
    Holidays 设置节假日
=======================================
*/
CR_API uint_t
holidays_hday_set (
  __CR_IN__ holidays_t          hdays,
  __CR_IN__ const sHOLI_DAYS*   holi,
  __CR_IN__ uint_t              count
    )
{
    uint_t      idx;
    uint_t      kkk;
    sHOLIDAYS*  real;

    /* 格式转换过去 */
    if (count > HOLI_MAX_HDAYS)
        count = HOLI_MAX_HDAYS;
    real = (sHOLIDAYS*)hdays;
    for (idx = 0; idx < count; idx++) {
        if (!datetime_from_bcd(&real->hdays[idx].beg, &holi[idx].beg))
            break;
        if (!datetime_from_bcd(&real->hdays[idx].end, &holi[idx].end))
            break;
        if (holi[idx].rule[0] == 0)
            break;
        real->hdays[idx].rule[0] = holi[idx].rule[0];
        for (kkk = 1; kkk < HOLI_MAX_TYP; kkk++) {
            if (holi[idx].rule[kkk] == 0 ||
                holi[idx].rule[kkk] >= HOLI_MAX_RULES)
                real->hdays[idx].rule[kkk] = holi[idx].rule[0];
            else
                real->hdays[idx].rule[kkk] = holi[idx].rule[kkk];
        }
    }

    /* 时间按升序排序 */
    real->hday_cnt = idx;
    quick_sort(real->hdays, idx, sizeof(sHOLI_HDAY), hday_fcmp);
    return (idx);
}

/*
=======================================
    Holidays 设置周计划
=======================================
*/
CR_API sint_t
holidays_week_set (
  __CR_IN__ holidays_t      hdays,
  __CR_IN__ const byte_t*   weeks,
  __CR_IN__ uint_t          index
    )
{
    uint_t      idx;
    sHOLIDAYS*  real;

    if (index == 0 || index >= HOLI_MAX_WEEKS)
        return (-1);
    for (idx = 0; idx < 8; idx++) {
        if (weeks[idx] >= HOLI_MAX_RULES)
            return (-1);
    }
    real = (sHOLIDAYS*)hdays;
    mem_cpy(&real->week[index * 8], weeks, 8);
    return (0);
}

/*
---------------------------------------
    Holidays 合并周计划时间
---------------------------------------
*/
static void_t
holidays_week_combine (
  __CR_IO__ sDATETIME*      date,
  __CR_IN__ const byte_t*   week,
  __CR_IN__ uint_t          index,
  __CR_IN__ uint_t          rule_id
    )
{
    uint_t  idx, rule, tcmp;

    /* 这里已经保证了周计划的编号不是都一样的 */
    rule = (uint_t)week[index];
    if (rule == 0)
        rule = rule_id;
    for (idx = 1; idx < 7; idx++) {
        index = (index + 1) % 7;
        tcmp = (uint_t)week[index];
        if (tcmp == 0)
            tcmp = rule_id;
        if (rule != tcmp)
            break;
        date_inc(date, 1);
    }
}

/*
---------------------------------------
    Holidays 获取周计划规则编号
---------------------------------------
*/
static uint_t
holidays_rule_id_week (
  __CR_IN__ const sHOLIDAYS*    hdays,
  __CR_IN__ const sDATETIME*    date,
  __CR_IN__ const sHOLI_CTX*    param,
  __CR_OT__ sDATETIME*          finish
    )
{
    uint_t  today, yeday;
    uint_t  tcmp1, tcmp2, week;
    uint_t  week_id = param->week * 8 + 1;

    /* 判断是否在昨天段里 */
    week = (uint_t)date_get_week(date);
    today = (uint_t)hdays->week[week_id + week];
    if (today == 0)
        today = param->rule;
    if (!hdays->charges[today].okay)
        return (0);
    tcmp1 = (week == 0) ? 6 : (week - 1);
    yeday = (uint_t)hdays->week[week_id + tcmp1];
    if (yeday == 0)
        yeday = param->rule;
    if (!hdays->charges[yeday].okay)
        return (0);
    if (hdays->charges[yeday].type == HOLI_TYPE_FS) {
        tcmp1  = date->hour;
        tcmp1 *= 60;
        tcmp1 += date->minute;
        tcmp2  = hdays->charges[yeday].list[0].beg.tm[0];
        tcmp2 *= 60;
        tcmp2 += hdays->charges[yeday].list[0].beg.tm[1];
        if (tcmp1 < tcmp2) {
            if (finish != NULL)
            {
                /* 结束点到昨天的规则开始点为止 */
                struct_cpy(finish, date, sDATETIME);
                finish->hour   = hdays->charges[yeday].list[0].beg.tm[0];
                finish->minute = hdays->charges[yeday].list[0].beg.tm[1];
                finish->second = 0;
                if (yeday == today) {
                    date_inc(finish, 1);
                    holidays_week_combine(finish, &hdays->week[week_id],
                                          week, param->rule);
                }
            }
            return (yeday);
        }
    }

    /* 不在昨天的时间段里 */
    if (finish != NULL) {
        struct_cpy(finish, date, sDATETIME);
        date_inc(finish, 1);
        if (hdays->charges[today].type == HOLI_TYPE_FS) {
            finish->hour   = hdays->charges[today].list[0].beg.tm[0];
            finish->minute = hdays->charges[today].list[0].beg.tm[1];
            finish->second = 0;
        }
        else {
            finish->hour = finish->minute = finish->second = 0;
        }
        holidays_week_combine(finish, &hdays->week[week_id],
                              week, param->rule);
    }
    return (today);
}

/*
---------------------------------------
    Holidays 获取规则编号
---------------------------------------
*/
static uint_t
holidays_rule_id (
  __CR_IN__ const sHOLIDAYS*    hdays,
  __CR_IN__ const sDATETIME*    date,
  __CR_IN__ const sHOLI_CTX*    param
    )
{
    uint_t  idx;

    /* 搜索节假日的时间段 */
    if (param->holi != 0) {
        for (idx = 0; idx < hdays->hday_cnt; idx++) {
            if (datetime_cmp(date, &hdays->hdays[idx].beg) >= 0 &&
                datetime_cmp(date, &hdays->hdays[idx].end) < 0)
                break;
        }
        if (idx < hdays->hday_cnt)
            return ((uint_t)hdays->hdays[idx].rule[param->holi - 1]);
    }

    /* 普通的收费规则 */
    if (!hdays->week[param->week * 8])
        return (param->rule);

    /* 周计划的收费规则 */
    return (holidays_rule_id_week(hdays, date, param, NULL));
}

/*
=======================================
    Holidays 获取免费时间
=======================================
*/
CR_API sint_t
holidays_free_get (
  __CR_IN__ holidays_t          hdays,
  __CR_IN__ const sDATETIME*    date,
  __CR_IN__ byte_t              rule_id,
  __CR_IN__ byte_t              week_id,
  __CR_IN__ byte_t              holi_id
    )
{
    uint_t      idx;
    sHOLI_CTX   ctx;
    sHOLIDAYS*  real;

    if (holi_id > HOLI_MAX_TYP)
        return (-4);
    if (rule_id >= HOLI_MAX_RULES)
        return (-4);
    if (week_id >= HOLI_MAX_WEEKS)
        return (-4);
    ctx.rule = rule_id;
    ctx.week = week_id;
    ctx.holi = holi_id;
    real = (sHOLIDAYS*)hdays;
    idx = holidays_rule_id(real, date, &ctx);
    if (!real->charges[idx].okay)
        return (-1);
    return (real->charges[idx].free_tm);
}

/*
---------------------------------------
    Holidays 展开分时收费规则
---------------------------------------
*/
static uint_t
holidays_rule_expand (
  __CR_OT__ sHOLI_RULEX*            out,
  __CR_IN__ const sHOLI_RULE_INT*   rule,
  __CR_IN__ const sDATETIME*        date
    )
{
    uint_t  idx, ptr = 1;

    for (idx = 0; idx < HOLI_MAX_NUM; idx++)
    {
        /* 结束退出 */
        if (rule->list[idx].beg.tm[0] == rule->list[idx].end.tm[0] &&
            rule->list[idx].beg.tm[1] == rule->list[idx].end.tm[1])
            break;

        /* 展开起始时间 */
        struct_cpy(&out[ptr].beg, date, sDATETIME);
        out[ptr].beg.hour   = rule->list[idx].beg.tm[0];
        out[ptr].beg.minute = rule->list[idx].beg.tm[1];
        out[ptr].beg.second = 0;
        if (out[ptr].beg.hour == 23 && out[ptr].beg.minute == 59)
            time_inc(&out[ptr].beg, 60);

        /* 展开结束时间 */
        struct_cpy(&out[ptr].end, date, sDATETIME);
        out[ptr].end.hour   = rule->list[idx].end.tm[0];
        out[ptr].end.minute = rule->list[idx].end.tm[1];
        out[ptr].end.second = 0;
        if (out[ptr].end.hour == 23 && out[ptr].end.minute == 59)
            time_inc(&out[ptr].end, 60);
        out[ptr].sec = &rule->list[idx];
        ptr += 1;
    }

    /* 安全检查 */
    if (idx == 0)
        return (0);
    ptr -= 1;

    /* 是否有时间反绕 */
    if (datetime_cmp(&out[ptr].beg, &out[ptr].end) > 0)
    {
        /* 有反绕补上昨天和明天 */
        if (ptr <= 1)
            return (0);
        struct_cpy(&out[0], &out[ptr], sHOLI_RULEX);
        date_dec(&out[0].beg, 1);
        date_inc(&out[ptr].end, 1);
        ptr += 1;
    }
    else
    {
        /* 不需要补时间列表归位 */
        mem_cpy(out, out + 1, ptr * sizeof(sHOLI_RULEX));
    }
    return (ptr);
}

/*
---------------------------------------
    Holidays 计算时长费用
---------------------------------------
*/
static uint_t
count_balance_basic (
  __CR_IN__ const sHOLI_SECS_INT*   secs,
  __CR_IN__ byte_t                  full,
  __CR_IN__ uint_t                  time,
  __CR_IN__ bool_t                  limit
    )
{
    uint_t  money;

    money = time / (uint_t)secs->cst;
    if ((full == 0) && (time % (uint_t)secs->cst != 0))
        money += 1;
    money *= (uint_t)secs->val;
    if (limit) {
        if (money >= (uint_t)secs->max)
            return ((uint_t)secs->max);
    }
    return (money);
}

/*
---------------------------------------
    Holidays 计算分钟数差
---------------------------------------
*/
static uint_t
count_minute_delta (
  __CR_IN__ byte_t              type,
  __CR_IN__ const sDATETIME*    datetime1,
  __CR_IN__ const sDATETIME*    datetime2,
  __CR_OT__ uint_t*             rest
    )
{
    maxu_t  second;
    uint_t  minute, tmp;

    second = datetime_sub(datetime1, datetime2);
    minute = (uint_t)(second / 60);
    if (type & HOLI_FL_STUP) {
        tmp = (uint_t)(second % 60);
        if (tmp != 0) {
            if (rest != NULL)
                *rest = 60 - tmp;
            return (minute + 1);
        }
    }
    if (rest != NULL)
        *rest = 0;
    return (minute);
}

/* 函数提前声明 */
static sint_t
count_balance_jump0 (const sHOLIDAYS *hdays, const sHOLI_RULE_INT *rule,
                     const sDATETIME *leave, const sDATETIME *enter);
static sint_t
count_balance_jump1 (const sHOLIDAYS *hdays, const sHOLI_RULE_INT *rule,
                     const sDATETIME *leave, const sDATETIME *enter);
/*
---------------------------------------
    Holidays 计算停车费用
---------------------------------------
*/
static sint_t
count_balance (
  __CR_IN__ const sHOLIDAYS*        hdays,
  __CR_IN__ const sHOLI_RULE_INT*   rule,
  __CR_IN__ const sDATETIME*        leave,
  __CR_IN__ const sDATETIME*        enter
    )
{
    byte_t  type;

    /* 非法的收费规则 */
    if (!rule->okay)
        return (-1);

    /* 根据类型计算费用 */
    type = rule->type & HOLI_FL_MASK;
    if (type == HOLI_TYPE_AC)
        return ((sint_t)rule->maxv);
    if (type != HOLI_TYPE_FS)
        return (count_balance_jump0(hdays, rule, leave, enter));
    return (count_balance_jump1(hdays, rule, leave, enter));
}

/*
---------------------------------------
    Holidays 时长停车费用计算
---------------------------------------
*/
static sint_t
count_balance_jump0 (
  __CR_IN__ const sHOLIDAYS*        hdays,
  __CR_IN__ const sHOLI_RULE_INT*   rule,
  __CR_IN__ const sDATETIME*        leave,
  __CR_IN__ const sDATETIME*        enter
    )
{
    sDATETIME   cost_ss;
    sDATETIME   cost_ee;
    sDATETIME   leave_t;
    /* -------------- */
    uint_t  tcmp1, tcmp2;
    uint_t  cost1, cost2;
    uint_t  type, rule_id;
    bool_t  tday, limit = TRUE;
    uint_t  delta[HOLI_MAX_NUM];
    uint_t  idx, cnt, time, days;

    type = (uint_t)(rule->type & HOLI_FL_MASK);
    if (type == HOLI_TYPE_JT || type == HOLI_TYPE_TC)
    {
        /* 转换成时长的类型 */
        type = type - HOLI_TYPE_JT + HOLI_TYPE_JS;

        /* 天长算法直接取时长判断 */
        time = count_minute_delta(rule->type, leave, enter, &cnt);
        cost2 = days = 0;

        /* 判断在哪个天长段里 */
        for (idx = 0; idx < HOLI_MAX_NUM; idx++) {
            tcmp1 = rule->list[idx].beg.time;
            if (time >= tcmp1) {
                if (rule->list[idx].end.time == HOLI_FOR_EVER) {
                    limit = FALSE;
                    break;
                }
                tcmp2 = rule->list[idx].end.time;
                delta[idx] = tcmp2 - tcmp1;
                if (time <= tcmp2)
                    break;
            }
        }
        tday = TRUE;
    }
    else
    {
        /* 先把整天的钱算掉 */
        days = count_minute_delta(rule->type, leave, enter, &cnt);
        time = days % (24 * 60);
        days = days / (24 * 60);
        if (days != 0) {
            cost2 = rule->maxv * days;
            if (rule->type & HOLI_FL_ZERO) {
                if (time != 0)
                    cost2 += rule->maxv;
                return ((sint_t)cost2);
            }
        }
        else {
            cost2 = 0;
        }

        /* 判断在哪个时长段里 */
        for (idx = 0; idx < HOLI_MAX_NUM; idx++) {
            tcmp1 = rule->list[idx].beg.time;
            if (time >= tcmp1) {
                tcmp2 = rule->list[idx].end.time;
                delta[idx] = tcmp2 - tcmp1;
                if (time <= tcmp2)
                    break;
            }
        }
        tday = FALSE;
    }

    /* 规则设置有误 */
    if (idx >= HOLI_MAX_NUM)
        return (-1);

    /* 对齐到真实的离场时间点 */
    struct_cpy(&leave_t, leave, sDATETIME);
    if (cnt != 0)
        time_inc(&leave_t, cnt);
    cost1 = 0;

    /* 置起始时间 */
    struct_cpy(&cost_ss, enter, sDATETIME);
    date_inc(&cost_ss, days);

    /* 计时需要一段段算过去 */
    if (type == HOLI_TYPE_JS) {
        cnt = idx;
        struct_cpy(&cost_ee, &cost_ss, sDATETIME);
        for (idx = 0; idx < cnt; idx++) {
            time_inc(&cost_ee, delta[idx] * 60);
            rule_id = (uint_t)rule->list[idx].num;
            if (rule_id == 0)
            {
                /* 直接算费用 */
                cost1 += count_balance_basic(&rule->list[idx], rule->full,
                                             delta[idx], TRUE);
            }
            else
            {
                /* 跳转算费用 */
                tcmp1 = count_balance(hdays, &hdays->charges[rule_id],
                                      &cost_ee, &cost_ss);
                if ((sint_t)tcmp1 == -1)
                    return (-1);
                tcmp2 = (uint_t)rule->list[idx].max;
                if (tcmp1 >= tcmp2)
                    cost1 += tcmp2;
                else
                    cost1 += tcmp1;
            }
            struct_cpy(&cost_ss, &cost_ee, sDATETIME);
        }
    }

    /* 时长只要算在的那一段即可 */
    rule_id = (uint_t)rule->list[idx].num;
    if (rule_id == 0)
    {
        /* 直接算费用 */
        delta[idx] = count_minute_delta(0, &leave_t, &cost_ss, NULL);
        cost1 += count_balance_basic(&rule->list[idx], rule->full,
                                     delta[idx], limit);
    }
    else
    {
        /* 跳转算费用 */
        tcmp1 = count_balance(hdays, &hdays->charges[rule_id],
                              &leave_t, &cost_ss);
        if ((sint_t)tcmp1 == -1)
            return (-1);
        if (!limit) {
            cost1 += tcmp1;
        }
        else {
            tcmp2 = (uint_t)rule->list[idx].max;
            if (tcmp1 >= tcmp2)
                cost1 += tcmp2;
            else
                cost1 += tcmp1;
        }
    }
    if (!tday) {
        if (cost1 > (uint_t)rule->maxv)
            cost1 = (uint_t)rule->maxv;
    }
    return ((sint_t)(cost1 + cost2));
}

/*
---------------------------------------
    Holidays 分时停车费用计算
---------------------------------------
*/
static sint_t
count_balance_jump1 (
  __CR_IN__ const sHOLIDAYS*        hdays,
  __CR_IN__ const sHOLI_RULE_INT*   rule,
  __CR_IN__ const sDATETIME*        leave,
  __CR_IN__ const sDATETIME*        enter
    )
{
    bool_t      okay;
    sDATETIME   next;
    /* ----------- */
    byte_t  zero, crss;
    byte_t  hour, minute;
    uint_t  cost1, cost2;
    uint_t  tcmp1, tcmp2;
    uint_t  idx, cnt, rule_id;
    /* -------------------- */
    sHOLI_RULEX tsec[HOLI_MAX_NUM + 1];

    /* 时间段展开 */
    cnt = holidays_rule_expand(tsec, rule, enter);
    if (cnt == 0)
        return (-1);

    /* 记录一些常用参数 */
    zero = rule->type & HOLI_FL_ZERO;
    crss = rule->type & HOLI_FL_CRSS;
    hour   = rule->list[0].beg.tm[0];
    minute = rule->list[0].beg.tm[1];

    /* 置起始时间 */
    cost1 = cost2 = 0;
    struct_cpy(&next, enter, sDATETIME);

    /* 开始算费直到结束位置 */
    for (;;)
    {
        /* 一段段算过去 */
        for (okay = FALSE, idx = 0; idx < cnt; idx++)
        {
            /* 不在当前时段内继续下一个 */
            if (datetime_cmp(&next, &tsec[idx].end) >= 0)
                continue;
            okay = TRUE;
            rule_id = (uint_t)tsec[idx].sec->num;

            /* 起始时间必定在当前时段内 */
            /* 因为时段必定是首尾相接的, 与节假日不同 */
            if (datetime_cmp(leave, &tsec[idx].end) <= 0)
            {
                /* 全部在当前时段内, 直接结束 */
                if (rule_id == 0) {
                    tcmp1 = count_minute_delta(rule->type, leave,
                                               &next, NULL);
                    cost1 += count_balance_basic(tsec[idx].sec, rule->full,
                                                 tcmp1, TRUE);
                }
                else {
                    tcmp1 = count_balance(hdays, &hdays->charges[rule_id],
                                          leave, &next);
                    if ((sint_t)tcmp1 == -1)
                        return (-1);
                    tcmp2 = tsec[idx].sec->max;
                    if (tcmp1 >= tcmp2)
                        cost1 += tcmp2;
                    else
                        cost1 += tcmp1;
                }
                goto _func_out;
            }

            /* 计算到时段结束的费用, 并开始下一段的计算 */
            if (rule_id == 0) {
                tcmp1 = count_minute_delta(rule->type, &tsec[idx].end,
                                           &next, NULL);
                cost1 += count_balance_basic(tsec[idx].sec, rule->full,
                                             tcmp1, TRUE);
            }
            else {
                tcmp1 = count_balance(hdays, &hdays->charges[rule_id],
                                      &tsec[idx].end, &next);
                if ((sint_t)tcmp1 == -1)
                    return (-1);
                tcmp2 = tsec[idx].sec->max;
                if (tcmp1 >= tcmp2)
                    cost1 += tcmp2;
                else
                    cost1 += tcmp1;
            }
            if (crss) {
                tcmp1 = count_minute_delta(0, &tsec[idx].end, &next, NULL);
                tcmp2 = (uint_t)tsec[idx].sec->cst;
                tcmp1 = (tcmp1 + tcmp2 - 1) / tcmp2;
                tcmp1 *= tcmp2;
                time_inc(&next, tcmp1 * 60);
                if (datetime_cmp(leave, &next) <= 0)
                    goto _func_out;
            }
            else {
                struct_cpy(&next, &tsec[idx].end, sDATETIME);
            }

            /* 结算全天限额 */
            if (tsec[idx].end.hour   == (ufast_t)hour &&
                tsec[idx].end.minute == (ufast_t)minute) {
                if (cost1 >= (uint_t)rule->maxv)
                    cost2 += (uint_t)rule->maxv;
                else
                    cost2 += cost1;
                if (zero) {
                    tcmp1 = count_minute_delta(rule->type, leave,
                                               &tsec[idx].end, NULL);
                    tcmp2 = tcmp1 % (24 * 60);
                    tcmp1 = tcmp1 / (24 * 60);
                    cost1 = tcmp1 * rule->maxv;
                    if (tcmp2 != 0)
                        cost1 += rule->maxv;
                    return ((sint_t)(cost1 + cost2));
                }
                cost1 = 0;
            }
        }

        /* 时段有空洞 */
        if (!okay)
            return (-1);

        /* 时段全部向前推进一天 */
        for (idx = 0; idx < cnt; idx++) {
            date_inc(&tsec[idx].beg, 1);
            date_inc(&tsec[idx].end, 1);
        }
    }
_func_out:
    if (cost1 > (uint_t)rule->maxv)
        cost1 = (uint_t)rule->maxv;
    return ((sint_t)(cost1 + cost2));
}

/*
---------------------------------------
    Holidays 计算停车费用 (周计划)
---------------------------------------
*/
static sint_t
holidays_money_week (
  __CR_IN__ const sHOLIDAYS*    hdays,
  __CR_IN__ const sDATETIME*    leave,
  __CR_IN__ const sDATETIME*    enter,
  __CR_IN__ const sHOLI_CTX*    param
    )
{
    uint_t      rule;
    uint_t      cost;
    uint_t      tcmp;
    sint_t      temp;
    uint_t      week_id;
    sDATETIME   cost_ss;
    sDATETIME   cost_ee;

    /* 无效的周计划按原来的算 */
    week_id = param->week * 8 + 1;
    if (!hdays->week[week_id - 1]) {
        return (count_balance(hdays, &hdays->charges[param->rule],
                              leave, enter));
    }

    /* 判断周计划的规则是否一样 */
    rule = (uint_t)hdays->week[week_id];
    if (rule == 0)
        rule = param->rule;
    for (cost = 1; cost < 7; cost++) {
        tcmp = (uint_t)hdays->week[week_id + cost];
        if (tcmp == 0)
            tcmp = param->rule;
        if (rule != tcmp)
            break;
    }
    if (cost >= 7) {
        return (count_balance(hdays, &hdays->charges[rule],
                              leave, enter));
    }

    /* 开始算费直到结束位置 */
    struct_cpy(&cost_ss, enter, sDATETIME);
    for (cost = 0;;)
    {
        /* 获取规则编号及算到的结束点 */
        rule = holidays_rule_id_week(hdays, &cost_ss, param, &cost_ee);
        if (rule == 0)
            return (-1);

        /* 结束点大于等于离场时间表示计算结束 */
        if (datetime_cmp(leave, &cost_ee) <= 0) {
            temp = count_balance(hdays, &hdays->charges[rule],
                                 leave, &cost_ss);
            if (temp == -1)
                return (-1);
            cost += (uint_t)temp;
            break;
        }

        /* 计算到结束点时间 */
        temp = count_balance(hdays, &hdays->charges[rule],
                             &cost_ee, &cost_ss);
        if (temp == -1)
            return (-1);
        cost += (uint_t)temp;

        /* 更新起始时间 */
        struct_cpy(&cost_ss, &cost_ee, sDATETIME);
    }
    return ((sint_t)cost);
}

/*
---------------------------------------
    Holidays 计算停车费用 (节假日)
---------------------------------------
*/
static sint_t
holidays_money_hday (
  __CR_IN__ const sHOLIDAYS*    hdays,
  __CR_IN__ const sDATETIME*    leave,
  __CR_IN__ const sDATETIME*    enter,
  __CR_IN__ const sHOLI_CTX*    param
    )
{
    uint_t      idx;
    uint_t      rule;
    uint_t      cost;
    sint_t      temp;
    sDATETIME   next;

    /* 节假日为0表示不用 */
    if (param->holi == 0)
        return (holidays_money_week(hdays, leave, enter, param));

    /* 去除已经过了的节假日 */
    struct_cpy(&next, enter, sDATETIME);
    for (idx = 0; idx < hdays->hday_cnt; idx++)
    {
        /* 不在当前节假日段内继续下一个 */
        if (datetime_cmp(&next, &hdays->hdays[idx].end) >= 0)
            continue;
    }

    /* 开始算费直到结束位置 */
    for (cost = 0; idx < hdays->hday_cnt; idx++)
    {
        /* 有一段时间肯定不在节假日内 */
        if (datetime_cmp(&next, &hdays->hdays[idx].beg) < 0)
        {
            /* 全部不在节假日内, 直接结束 */
            if (datetime_cmp(leave, &hdays->hdays[idx].beg) <= 0)
                break;

            /* 计算前一段时间不在节假日内的费用 */
            temp = holidays_money_week(hdays, &hdays->hdays[idx].beg,
                                       &next, param);
            if (temp == -1)
                return (-1);
            cost += (uint_t)temp;

            /* 后一段在节假日内, 计算费用后结束 */
            rule = (uint_t)hdays->hdays[idx].rule[param->holi - 1];
            if (datetime_cmp(leave, &hdays->hdays[idx].end) <= 0) {
                temp = count_balance(hdays, &hdays->charges[rule],
                                     leave, &hdays->hdays[idx].beg);
                if (temp == -1)
                    return (-1);
                cost += (uint_t)temp;
                return ((sint_t)cost);
            }

            /* 后一段待定, 重置起始时间后再计算 */
            temp = count_balance(hdays, &hdays->charges[rule],
                                 &hdays->hdays[idx].end,
                                 &hdays->hdays[idx].beg);
            if (temp == -1)
                return (-1);
            cost += (uint_t)temp;
        }
        else
        {
            /* 全部都在节假日内 */
            rule = (uint_t)hdays->hdays[idx].rule[param->holi - 1];
            if (datetime_cmp(leave, &hdays->hdays[idx].end) <= 0) {
                temp = count_balance(hdays, &hdays->charges[rule],
                                     leave, &next);
                if (temp == -1)
                    return (-1);
                cost += (uint_t)temp;
                return ((sint_t)cost);
            }

            /* 前一段时间在节假日内, 后一段待定, 重置起始时间后再计算 */
            temp = count_balance(hdays, &hdays->charges[rule],
                                 &hdays->hdays[idx].end, &next);
            if (temp == -1)
                return (-1);
            cost += (uint_t)temp;
        }
        struct_cpy(&next, &hdays->hdays[idx].end, sDATETIME);
    }

    /* 计算最后一段的非节假日费用 */
    temp = holidays_money_week(hdays, leave, &next, param);
    if (temp == -1)
        return (-1);
    cost += (uint_t)temp;
    return ((sint_t)cost);
}

/*
---------------------------------------
    Holidays 计算停车费用 (月卡分段)
---------------------------------------
*/
static sint_t
holidays_money_cday (
  __CR_IN__ const sHOLIDAYS*    hdays,
  __CR_IN__ const sDATETIME*    leave,
  __CR_IN__ const sDATETIME*    enter,
  __CR_IN__ const sDATETIME*    mbeg,
  __CR_IN__ const sDATETIME*    mend,
  __CR_IN__ const sHOLI_CTX*    parms
    )
{
    uint_t  cost;
    sint_t  temp;

    /* 无月卡的计算 */
    if (mbeg == NULL || mend == NULL)
        return (holidays_money_hday(hdays, leave, enter, &parms[0]));

    /* 有月卡的计算 */
    if (datetime_cmp(enter, mbeg) >= 0)
    {
        /* 全在月卡段后面 */
        if (datetime_cmp(enter, mend) >= 0)
            return (holidays_money_hday(hdays, leave, enter, &parms[0]));

        /* 全在月卡段里面 */
        if (datetime_cmp(leave, mend) <= 0)
            return (holidays_money_hday(hdays, leave, enter, &parms[1]));

        /* 前半段在月卡段内, 后半段在普通段内 */
        temp = holidays_money_hday(hdays, mend, enter, &parms[1]);
        if (temp == -1)
            return (-1);
        cost = (uint_t)temp;
        temp = holidays_money_hday(hdays, leave, mend, &parms[0]);
        if (temp == -1)
            return (-1);
        cost += (uint_t)temp;
        return ((sint_t)cost);
    }

    /* 全在月卡段前面 */
    if (datetime_cmp(leave, mbeg) <= 0)
        return (holidays_money_hday(hdays, leave, enter, &parms[0]));

    /* 前半段在普通段内, 后半段在月卡段内 */
    if (datetime_cmp(leave, mend) <= 0) {
        temp = holidays_money_hday(hdays, mbeg, enter, &parms[0]);
        if (temp == -1)
            return (-1);
        cost = (uint_t)temp;
        temp = holidays_money_hday(hdays, leave, mbeg, &parms[1]);
        if (temp == -1)
            return (-1);
        cost += (uint_t)temp;
        return ((sint_t)cost);
    }

    /* 分三段计算, 最后一段是普通段 */
    temp = holidays_money_hday(hdays, mbeg, enter, &parms[0]);
    if (temp == -1)
        return (-1);
    cost = (uint_t)temp;
    temp = holidays_money_hday(hdays, mend, mbeg, &parms[1]);
    if (temp == -1)
        return (-1);
    cost += (uint_t)temp;
    temp = holidays_money_hday(hdays, leave, mend, &parms[0]);
    if (temp == -1)
        return (-1);
    cost += (uint_t)temp;
    return ((sint_t)cost);
}

/*
=======================================
    Holidays 计算停车费用
=======================================
*/
CR_API sint_t
holidays_money (
  __CR_IN__ holidays_t          hdays,
  __CR_IN__ const sDATETIME*    leave,
  __CR_IN__ const sDATETIME*    enter,
  __CR_IN__ const sDATETIME*    mbeg,
  __CR_IN__ const sDATETIME*    mend,
  __CR_IN__ bool_t              is_pre,
  __CR_IN__ byte_t              rule_id,
  __CR_IN__ byte_t              rule_wid,
  __CR_IN__ byte_t              rule_hid,
  __CR_IN__ byte_t              month_id,
  __CR_IN__ byte_t              month_wid,
  __CR_IN__ byte_t              month_hid
    )
{
    uint_t      idx;
    uint_t      time;
    maxu_t      secs;
    sDATETIME   next;
    sHOLIDAYS*  real;
    sHOLI_CTX   ctx[2];

    /* 安全检查 */
    if (rule_hid > HOLI_MAX_TYP || month_hid > HOLI_MAX_TYP)
        return (-1);
    if (rule_id >= HOLI_MAX_RULES || month_id >= HOLI_MAX_RULES)
        return (-1);
    if (rule_wid >= HOLI_MAX_WEEKS || month_wid >= HOLI_MAX_WEEKS)
        return (-1);
    if (datetime_cmp(leave, enter) < 0)
        return (-1);
    if (mbeg != NULL && mend != NULL) {
        if (datetime_cmp(mend, mbeg) <= 0)
            return (-1);
    }
    ctx[0].rule = (uint_t)rule_id;
    ctx[0].week = (uint_t)rule_wid;
    ctx[0].holi = (uint_t)rule_hid;
    ctx[1].rule = (uint_t)month_id;
    ctx[1].week = (uint_t)month_wid;
    ctx[1].holi = (uint_t)month_hid;

    /* 二次收费不能算免费时间 */
    real = (sHOLIDAYS*)hdays;
    if (!is_pre)
    {
        /* 免费时间判断, 是否在月卡段内 */
        if (mbeg != NULL && mend != NULL &&
            datetime_cmp(enter, mbeg) >= 0 &&
            datetime_cmp(enter, mend) < 0)
            idx = holidays_rule_id(real, enter, &ctx[1]);
        else
            idx = holidays_rule_id(real, enter, &ctx[0]);
        if (!real->charges[idx].okay)
            return (-1);
        time = (uint_t)real->charges[idx].free_tm;
        if (time != 0) {
            time *= 60;
            secs = datetime_sub(leave, enter);
            if (secs < (maxu_t)time)
                return (0);

            /* 免费时间不算费, 挪动入场时间 */
            if (!real->charges[idx].if_free) {
                struct_cpy(&next, enter, sDATETIME);
                time_inc(&next, time);
                enter = &next;
            }
        }
    }
    return (holidays_money_cday(real, leave, enter, mbeg, mend, ctx));
}

/*
---------------------------------------
    Holidays 查找收费突变
---------------------------------------
*/
static bool_t
holidays_search (
  __CR_IN__ const sHOLIDAYS*    hdays,
  __CR_IN__ const sHOLI_SEARCH* search
    )
{
    uint_t  idx;
    sint_t  temp;

    /* 按步长加, 直到费用变了 */
    for (idx = 0; idx < search->length; idx += search->step) {
        time_inc(search->leave, search->step);
        temp = holidays_money((holidays_t)hdays, search->leave,
                    search->enter, search->mbeg, search->mend, search->is_pre,
                        search->rule_id, search->rule_wid, search->rule_hid,
                                search->month_id, search->month_wid,
                                        search->month_hid);
        if (temp == -1)
            return (FALSE);
        if (temp != search->cost)
            break;
    }
    if (idx >= search->length)
        return (FALSE);
    time_dec(search->leave, search->step);
    return (TRUE);
}

/*
=======================================
    Holidays 获取剩余时间
=======================================
*/
CR_API sint_t
holidays_rests (
  __CR_IN__ holidays_t          hdays,
  __CR_IN__ const sDATETIME*    leave,
  __CR_IN__ const sDATETIME*    enter,
  __CR_IN__ const sDATETIME*    mbeg,
  __CR_IN__ const sDATETIME*    mend,
  __CR_IN__ bool_t              is_pre,
  __CR_IN__ byte_t              rule_id,
  __CR_IN__ byte_t              rule_wid,
  __CR_IN__ byte_t              rule_hid,
  __CR_IN__ byte_t              month_id,
  __CR_IN__ byte_t              month_wid,
  __CR_IN__ byte_t              month_hid
    )
{
    sDATETIME       next;
    sHOLIDAYS*      real;
    sHOLI_SEARCH    fast;

    /* 计算当前的费用 */
    fast.cost = holidays_money(hdays, leave, enter, mbeg, mend,
                               is_pre, rule_id, rule_wid, rule_hid,
                               month_id, month_wid, month_hid);
    if (fast.cost == -1)
        return (-1);
    fast.is_pre = is_pre;
    fast.rule_id = rule_id;
    fast.rule_wid = rule_wid;
    fast.rule_hid = rule_hid;
    fast.month_id = month_id;
    fast.month_wid = month_wid;
    fast.month_hid = month_hid;
    struct_cpy(&next, leave, sDATETIME);
    fast.leave = &next;
    fast.enter = enter;
    fast.mbeg = mbeg;
    fast.mend = mend;

    /* 天, 小时, 分钟, 秒步长搜索 */
    real = (sHOLIDAYS*)hdays;
    fast.step = CR_D2S(1);
    fast.length = CR_D2S(10);
    if (!holidays_search(real, &fast))
        return (-1);
    fast.step = CR_H2S(1);
    fast.length = CR_D2S(1);
    if (!holidays_search(real, &fast))
        return (-1);
    fast.step = CR_M2S(1);
    fast.length = CR_H2S(1);
    if (!holidays_search(real, &fast))
        return (-1);
    fast.step = 1;
    fast.length = CR_M2S(1);
    if (!holidays_search(real, &fast))
        return (-1);
    return ((sint_t)datetime_sub(&next, leave));
}

/*
=======================================
    Holidays 获取下个缴费点
=======================================
*/
CR_API sint_t
holidays_stepa (
  __CR_IN__ holidays_t          hdays,
  __CR_IN__ const sDATETIME*    leave,
  __CR_IN__ const sDATETIME*    enter,
  __CR_IN__ const sDATETIME*    mbeg,
  __CR_IN__ const sDATETIME*    mend,
  __CR_IN__ bool_t              is_pre,
  __CR_IN__ byte_t              rule_id,
  __CR_IN__ byte_t              rule_wid,
  __CR_IN__ byte_t              rule_hid,
  __CR_IN__ byte_t              month_id,
  __CR_IN__ byte_t              month_wid,
  __CR_IN__ byte_t              month_hid
    )
{
    sint_t      temp;
    sDATETIME   next;

    /* 只有刚好在转折点上需要再搜索 */
    temp = holidays_rests(hdays, leave, enter, mbeg, mend,
                          is_pre, rule_id, rule_wid, rule_hid,
                          month_id, month_wid, month_hid);
    if (temp != 0)
        return (temp);
    struct_cpy(&next, leave, sDATETIME);
    time_inc(&next, 1);
    temp = holidays_rests(hdays, &next, enter, mbeg, mend,
                          is_pre, rule_id, rule_wid, rule_hid,
                          month_id, month_wid, month_hid);
    if (temp < 0)
        return (temp);
    return (temp + 1);
}

/*
=======================================
    Holidays 获取免费时间 (加强版)
=======================================
*/
CR_API sint_t
holidays_free_get2 (
  __CR_IN__ holidays_t          hdays,
  __CR_IN__ const sDATETIME*    date,
  __CR_IN__ const sHOLI_RNGS*   range,  /* 必须已排序 */
  __CR_IN__ uint_t              count,
  __CR_IN__ byte_t              rule_id,
  __CR_IN__ byte_t              week_id,
  __CR_IN__ byte_t              holi_id
    )
{
    uint_t  idx;

    /* 走老流程 */
    if (range == NULL || count == 0 ||
        datetime_cmp(date, &range[0].date) < 0)
        return (holidays_free_get(hdays, date, rule_id, week_id, holi_id));

    /* 查找在哪一段里 */
    for (idx = 0; idx < count - 1; idx++) {
        if (datetime_cmp(date, &range[idx + 0].date) >= 0 &&
            datetime_cmp(date, &range[idx + 1].date) < 0)
            break;
    }
    return (holidays_free_get(hdays, date, range[idx].rule, range[idx].week,
                                           range[idx].holi));
}

/*
---------------------------------------
    Holidays 查找并设置参数
---------------------------------------
*/
CR_API uint_t
holidays_find (
  __CR_OT__ sHOLI_CTX*          param,
  __CR_IN__ const sDATETIME*    enter,
  __CR_IN__ const sHOLI_RNGS*   range,  /* 必须已排序 */
  __CR_IN__ uint_t              count,
  __CR_IN__ sint_t              start,
  __CR_IN__ byte_t              rule_id,
  __CR_IN__ byte_t              week_id,
  __CR_IN__ byte_t              holi_id
    )
{
    uint_t  idx;

    idx = (uint_t)start;
    if (start < 0 && datetime_cmp(enter, &range[0].date) < 0) {
        param->rule = (uint_t)rule_id;
        param->week = (uint_t)week_id;
        param->holi = (uint_t)holi_id;
    }
    else {
        if (start < 0) idx = 0;
        for (; idx < count - 1; idx++) {
            if (datetime_cmp(enter, &range[idx + 0].date) >= 0 &&
                datetime_cmp(enter, &range[idx + 1].date) < 0)
                break;
        }
        param->rule = (uint_t)range[idx].rule;
        param->week = (uint_t)range[idx].week;
        param->holi = (uint_t)range[idx].holi;
    }
    return (idx);
}

/*
=======================================
    Holidays 计算停车费用 (加强版)
=======================================
*/
CR_API sint_t
holidays_money2 (
  __CR_IN__ holidays_t          hdays,
  __CR_IN__ const sDATETIME*    leave,
  __CR_IN__ const sDATETIME*    enter,
  __CR_IN__ const sHOLI_RNGS*   range,  /* 必须已排序 */
  __CR_IN__ uint_t              count,
  __CR_IN__ bool_t              is_pre,
  __CR_IN__ byte_t              rule_id,
  __CR_IN__ byte_t              week_id,
  __CR_IN__ byte_t              holi_id
    )
{
    uint_t      idx;
    uint_t      beg;
    uint_t      time;
    uint_t      cost;
    sint_t      temp;
    maxu_t      secs;
    sDATETIME   next;
    sHOLI_CTX   ctx0;
    sHOLIDAYS*  real;

    /* 走老流程 */
    if (range == NULL || count == 0) {
        return (holidays_money(hdays, leave, enter, NULL, NULL, is_pre,
                               rule_id, week_id, holi_id, 0, 0, 0));
    }

    /* 安全检查 */
    if (datetime_cmp(leave, enter) < 0)
        return (-1);

    /* 查找在哪一段里 */
    beg = holidays_find(&ctx0, enter, range, count, -1,
                        rule_id, week_id, holi_id);

    /* 二次收费不能算免费时间 */
    real = (sHOLIDAYS*)hdays;
    struct_cpy(&next, enter, sDATETIME);
    if (!is_pre)
    {
        /* 免费时间判断 */
        if (ctx0.holi > HOLI_MAX_TYP)
            return (-1);
        if (ctx0.rule >= HOLI_MAX_RULES)
            return (-1);
        if (ctx0.week >= HOLI_MAX_WEEKS)
            return (-1);
        idx = holidays_rule_id(real, enter, &ctx0);
        if (!real->charges[idx].okay)
            return (-1);
        time = (uint_t)real->charges[idx].free_tm;
        if (time != 0) {
            time *= 60;
            secs = datetime_sub(leave, enter);
            if (secs < (maxu_t)time)
                return (0);

            /* 免费时间不算费, 挪动入场时间 */
            if (!real->charges[idx].if_free)
            {
                /* 需要重新查找在哪一段里 */
                time_inc(&next, time);
                beg = holidays_find(&ctx0, &next, range, count, beg,
                                     rule_id, week_id, holi_id);
            }
        }
    }

    /* 一段段算过去 */
    for (cost = 0, idx = beg + 1; idx < count; idx++)
    {
        /* 离场时间在时段内, 直接结束 */
        if (datetime_cmp(leave, &range[idx].date) <= 0)
            break;

        /* 计算整段的费用 */
        temp = holidays_money_hday(real, &range[idx].date, &next, &ctx0);
        if (temp == -1)
            return (-1);
        cost += (uint_t)temp;

        /* 置下一段的参数 */
        ctx0.rule = (uint_t)range[idx].rule;
        ctx0.week = (uint_t)range[idx].week;
        ctx0.holi = (uint_t)range[idx].holi;
        if (ctx0.holi > HOLI_MAX_TYP)
            return (-1);
        if (ctx0.rule >= HOLI_MAX_RULES)
            return (-1);
        if (ctx0.week >= HOLI_MAX_WEEKS)
            return (-1);
        struct_cpy(&next, &range[idx].date, sDATETIME);
    }

    /* 计算最后一段的费用 */
    temp = holidays_money_hday(real, leave, &next, &ctx0);
    if (temp == -1)
        return (-1);
    cost += (uint_t)temp;
    return ((uint_t)cost);
}

/*
---------------------------------------
    Holidays 查找收费突变 (加强版)
---------------------------------------
*/
static bool_t
holidays_search2 (
  __CR_IN__ const sHOLIDAYS*        hdays,
  __CR_IN__ const sHOLI_SEARCH2*    search
    )
{
    uint_t  idx;
    sint_t  temp;

    /* 按步长加, 直到费用变了 */
    for (idx = 0; idx < search->length; idx += search->step) {
        time_inc(search->leave, search->step);
        temp = holidays_money2((holidays_t)hdays, search->leave,
                search->enter, search->range, search->count, search->is_pre,
                    search->rule_id, search->week_id, search->holi_id);
        if (temp == -1)
            return (FALSE);
        if (temp != search->cost)
            break;
    }
    if (idx >= search->length)
        return (FALSE);
    time_dec(search->leave, search->step);
    return (TRUE);
}

/*
=======================================
    Holidays 获取剩余时间 (加强版)
=======================================
*/
CR_API sint_t
holidays_rests2 (
  __CR_IN__ holidays_t          hdays,
  __CR_IN__ const sDATETIME*    leave,
  __CR_IN__ const sDATETIME*    enter,
  __CR_IN__ const sHOLI_RNGS*   range,  /* 必须已排序 */
  __CR_IN__ uint_t              count,
  __CR_IN__ bool_t              is_pre,
  __CR_IN__ byte_t              rule_id,
  __CR_IN__ byte_t              week_id,
  __CR_IN__ byte_t              holi_id
    )
{
    sDATETIME       next;
    sHOLIDAYS*      real;
    sHOLI_SEARCH2   fast;

    /* 走老流程 */
    if (range == NULL || count == 0) {
        return (holidays_rests(hdays, leave, enter, NULL, NULL, is_pre,
                               rule_id, week_id, holi_id, 0, 0, 0));
    }

    /* 计算当前的费用 */
    fast.cost = holidays_money2(hdays, leave, enter, range, count,
                                is_pre, rule_id, week_id, holi_id);
    if (fast.cost == -1)
        return (-1);
    fast.is_pre = is_pre;
    fast.rule_id = rule_id;
    fast.week_id = week_id;
    fast.holi_id = holi_id;
    struct_cpy(&next, leave, sDATETIME);
    fast.leave = &next;
    fast.enter = enter;
    fast.range = range;
    fast.count = count;

    /* 天, 小时, 分钟, 秒步长搜索 */
    real = (sHOLIDAYS*)hdays;
    fast.step = CR_D2S(1);
    fast.length = CR_D2S(10);
    if (!holidays_search2(real, &fast))
        return (-1);
    fast.step = CR_H2S(1);
    fast.length = CR_D2S(1);
    if (!holidays_search2(real, &fast))
        return (-1);
    fast.step = CR_M2S(1);
    fast.length = CR_H2S(1);
    if (!holidays_search2(real, &fast))
        return (-1);
    fast.step = 1;
    fast.length = CR_M2S(1);
    if (!holidays_search2(real, &fast))
        return (-1);
    return ((sint_t)datetime_sub(&next, leave));
}

/*
=======================================
    Holidays 获取下个缴费点 (加强版)
=======================================
*/
CR_API sint_t
holidays_stepa2 (
  __CR_IN__ holidays_t          hdays,
  __CR_IN__ const sDATETIME*    leave,
  __CR_IN__ const sDATETIME*    enter,
  __CR_IN__ const sHOLI_RNGS*   range,  /* 必须已排序 */
  __CR_IN__ uint_t              count,
  __CR_IN__ bool_t              is_pre,
  __CR_IN__ byte_t              rule_id,
  __CR_IN__ byte_t              week_id,
  __CR_IN__ byte_t              holi_id
    )
{
    sint_t      temp;
    sDATETIME   next;

    /* 走老流程 */
    if (range == NULL || count == 0) {
        return (holidays_stepa(hdays, leave, enter, NULL, NULL, is_pre,
                               rule_id, week_id, holi_id, 0, 0, 0));
    }

    /* 只有刚好在转折点上需要再搜索 */
    temp = holidays_rests2(hdays, leave, enter, range, count,
                           is_pre, rule_id, week_id, holi_id);
    if (temp != 0)
        return (temp);
    struct_cpy(&next, leave, sDATETIME);
    time_inc(&next, 1);
    temp = holidays_rests2(hdays, &next, enter, range, count,
                           is_pre, rule_id, week_id, holi_id);
    if (temp < 0)
        return (temp);
    return (temp + 1);
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
