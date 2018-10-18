/*
 *********************************
 *  Holidays 头文件
 *
 *  创建时间: 2016-04-13
 *********************************
 */

#ifndef __HOLIDAYS_H__
#define __HOLIDAYS_H__

typedef void* holidays_t;

#ifndef _WIN32
    #define BJSTDCALL
#else
    #define BJSTDCALL   __stdcall
#endif

#ifdef __cplusplus
extern "C"
{
#endif

holidays_t BJSTDCALL Holidays_Init (void);
void BJSTDCALL Holidays_Kill (holidays_t hdays);
int BJSTDCALL Holidays_RuleSet (holidays_t hdays, const void *rule);
int BJSTDCALL Holidays_HdaySet (holidays_t hdays, const void *holi);
int BJSTDCALL Holidays_WeekSet (holidays_t hdays, const void *weeks,
                                int index);
int BJSTDCALL Holidays_FreeGet (holidays_t hdays, const char *date,
                                int rule_id, int week_id, int holi_id);
int BJSTDCALL Holidays_Money (holidays_t hdays, const char *leave,
                              const char *enter, const char *mbeg,
                              const char *mend, int is_pre, int rule_id,
                              int rule_wid, int rule_hid, int month_id,
                              int month_wid, int month_hid);
int BJSTDCALL Holidays_Rests (holidays_t hdays, const char *leave,
                              const char *enter, const char *mbeg,
                              const char *mend, int is_pre, int rule_id,
                              int rule_wid, int rule_hid, int month_id,
                              int month_wid, int month_hid);
int BJSTDCALL Holidays_Stepa (holidays_t hdays, const char *leave,
                              const char *enter, const char *mbeg,
                              const char *mend, int is_pre, int rule_id,
                              int rule_wid, int rule_hid, int month_id,
                              int month_wid, int month_hid);
int BJSTDCALL Holidays_Money2 (holidays_t hdays, const char *leave,
                               const char *enter, int is_pre,
                               int rule_id, int week_id, int holi_id);
int BJSTDCALL Holidays_Rests2 (holidays_t hdays, const char *leave,
                               const char *enter, int is_pre,
                               int rule_id, int week_id, int holi_id);
int BJSTDCALL Holidays_Stepa2 (holidays_t hdays, const char *leave,
                               const char *enter, int is_pre,
                               int rule_id, int week_id, int holi_id);
int BJSTDCALL Holidays_FreeGetEx (holidays_t hdays, const char *date,
                                  const char *range, int rule_id,
                                  int week_id, int holi_id);
int BJSTDCALL Holidays_MoneyEx (holidays_t hdays, const char *leave,
                                const char *enter, const char *range,
                                int is_pre, int rule_id, int week_id,
                                int holi_id);
int BJSTDCALL Holidays_RestsEx (holidays_t hdays, const char *leave,
                                const char *enter, const char *range,
                                int is_pre, int rule_id, int week_id,
                                int holi_id);
int BJSTDCALL Holidays_StepaEx (holidays_t hdays, const char *leave,
                                const char *enter, const char *range,
                                int is_pre, int rule_id, int week_id,
                                int holi_id);

#ifdef __cplusplus
}
#endif

#endif  /* !__HOLIDAYS_H__ */
