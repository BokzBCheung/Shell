/*
 *********************************
 *  BJNetComm 库包装
 *
 *  创建时间: 2016-04-07
 *********************************
 */

package com.BlowJobs;

public class BJNetComm
{
    public native static long init ();
    public native static void kill (long bjnet);
    public native static void reset (long bjnet);
    public native static void sizeMax (long bjnet, int size_kb);
    public native static void updateToken (long bjnet, String token);
    public native static String encBytes (long bjnet, int type, byte[] data, String user);
    public native static String encString (long bjnet, int type, String string, String user);
    public native static byte[] decBytesEx (long bjnet, int tag_type, String tag_data, String tag_sign);
    public native static byte[] decBytes (long bjnet, String json);
    public native static String decString (long bjnet, String json);
    public native static byte[] genSN32 (String sn);

    static {
        System.loadLibrary("BJNetComm");
    }
}
