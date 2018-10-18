/*
 *********************************
 *  BJNetComm 库包装
 *
 *  创建时间: 2016-04-07
 *********************************
 */

using System;
using System.Text;
using System.Runtime.InteropServices;

namespace BlowJobs
{
    public class BJNetComm
    {
        /* ================================ */
        private static int strlen (IntPtr str)
        {
            int size = 0;
            byte ch = 0x00;

            for (;;) {
                ch = Marshal.ReadByte(str, size);
                if (ch == 0x00)
                    break;
                size += 1;
            }
            return size;
        }

        /* ====================== */
        public static IntPtr init ()
        {
            return BJNetComm_Init();
        }

        /* ================================ */
        public static void kill (IntPtr bjnet)
        {
            BJNetComm_Kill(bjnet);
        }

        /* ================================= */
        public static void reset (IntPtr bjnet)
        {
            BJNetComm_Reset(bjnet);
        }

        /* ================================================= */
        public static void sizeMax (IntPtr bjnet, uint size_kb)
        {
            BJNetComm_SizeMax(bjnet, size_kb);
        }

        /* ===================================================== */
        public static void updateToken (IntPtr bjnet, string token)
        {
            BJNetComm_UpdateToken(bjnet, token);
        }

        /* ================================================================================== */
        public static string encBytes (IntPtr bjnet, uint type, byte[] data, string user = null)
        {
            IntPtr back = new IntPtr ();

            back = BJNetComm_EncBytes(bjnet, type, data, data.Length, user);
            if (back == IntPtr.Zero)
                return null;

            string rett = Marshal.PtrToStringAnsi(back);

            BJNetComm_PtrFree(back);
            return rett;
        }

        /* =================================================================================== */
        public static string encString (IntPtr bjnet, uint type, string data, string user = null)
        {
            byte[] utf8 = System.Text.Encoding.UTF8.GetBytes(data);

            return encBytes(bjnet, type, utf8, user);
        }

        /* ========================================================================================= */
        public static byte[] decBytesEx (IntPtr bjnet, uint tag_type, string tag_data, string tag_sign)
        {
            int size = 0;
            IntPtr back = new IntPtr ();

            back = BJNetComm_DecBytesEx(bjnet, tag_type, tag_data, tag_sign, ref size);
            if (back == IntPtr.Zero)
                return null;

            byte[] rett = new byte [size];

            Marshal.Copy(back, rett, 0, size);
            BJNetComm_PtrFree(back);
            return rett;
        }

        /* =================================================== */
        public static byte[] decBytes (IntPtr bjnet, string json)
        {
            int size = 0;
            IntPtr back = new IntPtr ();

            back = BJNetComm_DecBytes(bjnet, json, ref size);
            if (back == IntPtr.Zero)
                return null;

            byte[] rett = new byte [size];

            Marshal.Copy(back, rett, 0, size);
            BJNetComm_PtrFree(back);
            return rett;
        }

        /* ==================================================== */
        public static string decString (IntPtr bjnet, string json)
        {
            IntPtr back = new IntPtr ();

            back = BJNetComm_DecString(bjnet, json);
            if (back == IntPtr.Zero)
                return null;

            int size = strlen(back);
            byte[] rett = new byte [size];

            Marshal.Copy(back, rett, 0, size);
            BJNetComm_PtrFree(back);
            return System.Text.Encoding.GetEncoding("UTF-8").GetString(rett);
        }

        /* ================================== */
        public static byte[] genSN32 (string sn)
        {
            IntPtr back = new IntPtr ();

            back = BJAutoNet_GenSN32(sn);
            if (back == IntPtr.Zero)
                return null;

            byte[] rett = new byte [32];

            Marshal.Copy(back, rett, 0, 32);
            BJNetComm_PtrFree(back);
            return rett;
        }

        #region importfunctions
        [DllImport ("BJNetComm.dll")]
        private static extern IntPtr BJNetComm_Init ();
        [DllImport ("BJNetComm.dll")]
        private static extern void BJNetComm_Kill (IntPtr bjnet);
        [DllImport ("BJNetComm.dll")]
        private static extern void BJNetComm_Reset (IntPtr bjnet);
        [DllImport ("BJNetComm.dll")]
        private static extern void BJNetComm_SizeMax (IntPtr bjnet, uint size_kb);
        [DllImport ("BJNetComm.dll")]
        private static extern void BJNetComm_UpdateToken (IntPtr bjnet, string token);
        [DllImport ("BJNetComm.dll")]
        private static extern IntPtr BJNetComm_EncBytes (IntPtr bjnet, uint type, byte[] data, int size, string user);
        [DllImport ("BJNetComm.dll")]
        private static extern IntPtr BJNetComm_EncString (IntPtr bjnet, uint type, string data, string user);
        [DllImport ("BJNetComm.dll")]
        private static extern IntPtr BJNetComm_DecBytesEx (IntPtr bjnet, uint tag_type, string tag_data, string tag_sign, ref int size);
        [DllImport ("BJNetComm.dll")]
        private static extern IntPtr BJNetComm_DecBytes (IntPtr bjnet, string json, ref int size);
        [DllImport ("BJNetComm.dll")]
        private static extern IntPtr BJNetComm_DecString (IntPtr bjnet, string json);
        [DllImport ("BJNetComm.dll")]
        private static extern void BJNetComm_PtrFree (IntPtr ptr);
        [DllImport ("BJNetComm.dll")]
        private static extern IntPtr BJAutoNet_GenSN32 (string sn);
        #endregion
    }
}
