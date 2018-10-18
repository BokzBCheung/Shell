/*
 *********************************
 *  �����ͷ�ļ�
 *
 *  ����ʱ��: 2016-05-07
 *********************************
 */

#ifndef __DHCAMERA_H__
#define __DHCAMERA_H__

typedef void*   dhcamera_t;
typedef void*   dhnotify_t;
typedef void_t  (*dh_callback1_t) (dhcamera_t, int);
typedef void_t  (*dh_callback2_t) (dhcamera_t, dhnotify_t);

#ifdef __cplusplus
extern
{
#endif

/* ��� API �� */
dhcamera_t dhcamera_open (const char *addr, unsigned short port,
                          const char *password);
dhcamera_t dhcamera_reopen (dhcamera_t dhcam, const char *password);
void dhcamera_close (dhcamera_t dhcam);
void dhcamera_callback (dhcamera_t dhcam, const char *type, void *func);
char* dhcamera_addr (dhcamera_t dhcam);
int dhcamera_setkey (dhcamera_t dhcam, const char *password);
int dhcamera_attach (dhcamera_t dhcam);
int dhcamera_detach (dhcamera_t dhcam);
int dhcamera_strobe (dhcamera_t dhcam, unsigned int is_open);
int dhcamera_kalive (dhcamera_t dhcam, unsigned int seconds);

/* ��� API �� */
unsigned int dhnotify_count (dhnotify_t dhsnap);
void* dhnotify_image (dhnotify_t dhsnap, unsigned int *size);
void* dhnotify_plate (dhnotify_t dhsnap, unsigned int index,
                      unsigned int *size);
int dhnotify_number (dhnotify_t dhsnap, unsigned int index,
                     const char *name);
char* dhnotify_string (dhnotify_t dhsnap, unsigned int index,
                       const char *name);
int dhnotify_colour (dhnotify_t dhsnap, unsigned int index,
                     const char *name);

/**************************
    �����͵Ľ�� KEY ֵ
"Confidence"
    �ַ����Ľ�� KEY ֵ
"DeviceID", "PlateNumber", "PlateType"
"SnapSource", "SnapTime", "StrobeState"
"VehicleType"
    ��ɫ�͵Ľ�� KEY ֵ
"PlateColor", "VehicleColor"
***************************/

#ifdef __cplusplus
}
#endif

#endif  /* !__DHCAMERA_H__ */
