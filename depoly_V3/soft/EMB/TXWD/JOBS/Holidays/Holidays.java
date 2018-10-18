/*
 *********************************
 *  Holidays 库包装
 *
 *  创建时间: 2016-04-13
 *********************************
 */

package com.BlowJobs;

public class Holidays
{
    public native static long init ();
    public native static void kill (long hdays);
    public native static int ruleSet (long hdays, byte[] rule);
    public native static int hdaySet (long hdays, byte[] holi);
    public native static int weekSet (long hdays, byte[] weeks, int index);
    public native static int freeGet (long hdays, String date, int rule_id,
                                      int week_id, int holi_id);
    public native static int money (long hdays, String leave, String enter,
                                    String mbeg, String mend, int is_pre,
                                    int rule_id, int rule_wid, int rule_hid,
                                    int month_id, int month_wid, int month_hid);
    public native static int rests (long hdays, String leave, String enter,
                                    String mbeg, String mend, int is_pre,
                                    int rule_id, int rule_wid, int rule_hid,
                                    int month_id, int month_wid, int month_hid);
    public native static int stepa (long hdays, String leave, String enter,
                                    String mbeg, String mend, int is_pre,
                                    int rule_id, int rule_wid, int rule_hid,
                                    int month_id, int month_wid, int month_hid);
    public native static int money2 (long hdays, String leave, String enter,
                                     int is_pre, int rule_id, int week_id,
                                     int holi_id);
    public native static int rests2 (long hdays, String leave, String enter,
                                     int is_pre, int rule_id, int week_id,
                                     int holi_id);
    public native static int stepa2 (long hdays, String leave, String enter,
                                     int is_pre, int rule_id, int week_id,
                                     int holi_id);
    public native static int freeGetEx (long hdays, String date, String range,
                                        int rule_id, int week_id, int holi_id);
    public native static int moneyEx (long hdays, String leave, String enter,
                                      String range, int is_pre, int rule_id,
                                      int week_id, int holi_id);
    public native static int restsEx (long hdays, String leave, String enter,
                                      String range, int is_pre, int rule_id,
                                      int week_id, int holi_id);
    public native static int stepaEx (long hdays, String leave, String enter,
                                      String range, int is_pre, int rule_id,
                                      int week_id, int holi_id);
    static {
        System.loadLibrary("Holidays");
    }
}
