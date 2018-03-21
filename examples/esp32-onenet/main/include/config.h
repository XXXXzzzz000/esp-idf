#ifndef __ONENET_CONFIG_H__
#define __ONENET_CONFIG_H__

//#define CPU_FREQ_160MHZ

/* Here needs to be changed according to your envirnoment. */
#define WIFI_SSID           "X-elerator"
#define WIFI_PASS           "sz12345678"

#define ONENET_HOST         "183.230.40.39"
#define ONENET_PORT         (6002)

/* Here needs to be changed accoding to your oneONET configure. */
#define ONENET_DEVICE_ID    "26943530"                  // mqtt client id
#define ONENET_PROJECT_ID   "124643"                    // mqtt username
#define ONENET_AUTH_INFO    "123456789"   // mqtt password

/* Here needs to be changed accoding to your oneONET configure. */
#define ONENET_DATA_STREAM  "temperature"

#define ONENET_PUB_INTERVAL (60) // unit: s

#endif

