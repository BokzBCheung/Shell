/*
 *********************************
 *  Holidays 库包装
 *
 *  创建时间: 2016-04-13
 *********************************
 */

using System;
using System.Runtime.InteropServices;

namespace BlowJobs
{
    public class Holidays
    {
        /* ====================== */
        public static IntPtr init ()
        {
            return Holidays_Init();
        }

        /* ================================ */
        public static void kill (IntPtr hdays)
        {
            Holidays_Kill(hdays);
        }

        /* =============================================== */
        public static int ruleSet (IntPtr hdays, byte[] rule)
        {
            if (rule.Length != 9 + 5 * 11)
                return -2;
            return Holidays_RuleSet(hdays, rule);
        }

        /* =============================================== */
        public static int hdaySet (IntPtr hdays, byte[] holi)
        {
            int len = holi.Length;

            if (len <= 1 || (len - 1) % (7 * 2 + 8) != 0)
                return -2;
            if ((int)holi[0] * (7 * 2 + 8) != len - 1)
                return -2;
            return Holidays_HdaySet(hdays, holi);
        }

        /* =========================================================== */
        public static int weekSet (IntPtr hdays, byte[] weeks, int index)
        {
            if (weeks.Length != 8)
                return -2;
            return Holidays_WeekSet(hdays, weeks, index);
        }

        /* ====================================================================================== */
        public static int freeGet (IntPtr hdays, string date, int rule_id, int week_id, int holi_id)
        {
            return Holidays_FreeGet(hdays, date, rule_id, week_id, holi_id);
        }

        /* ================================================================================================== */
        public static int money (IntPtr hdays, string leave, string enter, string mbeg, string mend, int is_pre,
                                 int rule_id, int rule_wid, int rule_hid, int month_id, int month_wid, int month_hid)
        {
            return Holidays_Money(hdays, leave, enter, mbeg, mend, is_pre, rule_id, rule_wid, rule_hid, month_id, month_wid, month_hid);
        }

        /* ================================================================================================== */
        public static int rests (IntPtr hdays, string leave, string enter, string mbeg, string mend, int is_pre,
                                 int rule_id, int rule_wid, int rule_hid, int month_id, int month_wid, int month_hid)
        {
            return Holidays_Rests(hdays, leave, enter, mbeg, mend, is_pre, rule_id, rule_wid, rule_hid, month_id, month_wid, month_hid);
        }

        /* ================================================================================================== */
        public static int stepa (IntPtr hdays, string leave, string enter, string mbeg, string mend, int is_pre,
                                 int rule_id, int rule_wid, int rule_hid, int month_id, int month_wid, int month_hid)
        {
            return Holidays_Stepa(hdays, leave, enter, mbeg, mend, is_pre, rule_id, rule_wid, rule_hid, month_id, month_wid, month_hid);
        }

        /* ================================================================================================================ */
        public static int money2 (IntPtr hdays, string leave, string enter, int is_pre, int rule_id, int week_id, int holi_id)
        {
            return Holidays_Money2(hdays, leave, enter, is_pre, rule_id, week_id, holi_id);
        }

        /* ================================================================================================================ */
        public static int rests2 (IntPtr hdays, string leave, string enter, int is_pre, int rule_id, int week_id, int holi_id)
        {
            return Holidays_Rests2(hdays, leave, enter, is_pre, rule_id, week_id, holi_id);
        }

        /* ================================================================================================================ */
        public static int stepa2 (IntPtr hdays, string leave, string enter, int is_pre, int rule_id, int week_id, int holi_id)
        {
            return Holidays_Stepa2(hdays, leave, enter, is_pre, rule_id, week_id, holi_id);
        }

        /* ====================================================================================================== */
        public static int freeGetEx (IntPtr hdays, string date, string range, int rule_id, int week_id, int holi_id)
        {
            return Holidays_FreeGetEx(hdays, date, range, rule_id, week_id, holi_id);
        }

        /* =============================================================================================================================== */
        public static int moneyEx (IntPtr hdays, string leave, string enter, string range, int is_pre, int rule_id, int week_id, int holi_id)
        {
            return Holidays_MoneyEx(hdays, leave, enter, range, is_pre, rule_id, week_id, holi_id);
        }

        /* =============================================================================================================================== */
        public static int restsEx (IntPtr hdays, string leave, string enter, string range, int is_pre, int rule_id, int week_id, int holi_id)
        {
            return Holidays_RestsEx(hdays, leave, enter, range, is_pre, rule_id, week_id, holi_id);
        }

        /* =============================================================================================================================== */
        public static int stepaEx (IntPtr hdays, string leave, string enter, string range, int is_pre, int rule_id, int week_id, int holi_id)
        {
            return Holidays_StepaEx(hdays, leave, enter, range, is_pre, rule_id, week_id, holi_id);
        }

        #region importfunctions
        [DllImport ("Holidays.dll")]
        private static extern IntPtr Holidays_Init ();
        [DllImport ("Holidays.dll")]
        private static extern void Holidays_Kill (IntPtr hdays);
        [DllImport ("Holidays.dll")]
        private static extern int Holidays_RuleSet (IntPtr hdays, byte[] rule);
        [DllImport ("Holidays.dll")]
        private static extern int Holidays_HdaySet (IntPtr hdays, byte[] holi);
        [DllImport ("Holidays.dll")]
        private static extern int Holidays_WeekSet (IntPtr hdays, byte[] weeks, int index);
        [DllImport ("Holidays.dll")]
        private static extern int Holidays_FreeGet (IntPtr hdays, string date, int rule_id, int week_id, int holi_id);
        [DllImport ("Holidays.dll")]
        private static extern int Holidays_Money (IntPtr hdays, string leave, string enter, string mbeg, string mend, int is_pre, int rule_id, int rule_wid, int rule_hid, int month_id, int month_wid, int month_hid);
        [DllImport ("Holidays.dll")]
        private static extern int Holidays_Rests (IntPtr hdays, string leave, string enter, string mbeg, string mend, int is_pre, int rule_id, int rule_wid, int rule_hid, int month_id, int month_wid, int month_hid);
        [DllImport ("Holidays.dll")]
        private static extern int Holidays_Stepa (IntPtr hdays, string leave, string enter, string mbeg, string mend, int is_pre, int rule_id, int rule_wid, int rule_hid, int month_id, int month_wid, int month_hid);
        [DllImport ("Holidays.dll")]
        private static extern int Holidays_Money2 (IntPtr hdays, string leave, string enter, int is_pre, int rule_id, int week_id, int holi_id);
        [DllImport ("Holidays.dll")]
        private static extern int Holidays_Rests2 (IntPtr hdays, string leave, string enter, int is_pre, int rule_id, int week_id, int holi_id);
        [DllImport ("Holidays.dll")]
        private static extern int Holidays_Stepa2 (IntPtr hdays, string leave, string enter, int is_pre, int rule_id, int week_id, int holi_id);
        [DllImport ("Holidays.dll")]
        private static extern int Holidays_FreeGetEx (IntPtr hdays, string date, string range, int rule_id, int week_id, int holi_id);
        [DllImport ("Holidays.dll")]
        private static extern int Holidays_MoneyEx (IntPtr hdays, string leave, string enter, string range, int is_pre, int rule_id, int week_id, int holi_id);
        [DllImport ("Holidays.dll")]
        private static extern int Holidays_RestsEx (IntPtr hdays, string leave, string enter, string range, int is_pre, int rule_id, int week_id, int holi_id);
        [DllImport ("Holidays.dll")]
        private static extern int Holidays_StepaEx (IntPtr hdays, string leave, string enter, string range, int is_pre, int rule_id, int week_id, int holi_id);
        #endregion
    }
}
