/**  @file
  ql_https_demo.c

  @brief
  TODO

*/

/*================================================================
  Copyright (c) 2022 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ql_api_osi.h"
#include "ql_debug.h"
#include "ql_wlan.h"

#include "curl/curl.h"
#include "curl/easy.h"

ql_task_t http_test_thread_handle = NULL;
ql_sem_t  wlan_semp = NULL;

static ql_sys_err_t ql_waln_event_cb(void *ctx, ql_system_event_t *event)
{
    ql_debug("\n%s,event_id:%d\n",__func__, event->event_id);
    switch(event->event_id){
    case QL_SYSTEM_EVENT_STA_GOT_IP:
        ql_debug("wlan_semp:%p\n", wlan_semp);
        if(wlan_semp != NULL){
            ql_rtos_semaphore_release(wlan_semp);
        }
        
        break;
    default:
        break;
    }
    return 0;
}
static size_t f_write_callback(void *ptr, size_t size, size_t nmemb, void *stream) //回调函数
{
    if(ptr != NULL){
       ql_debug("\necv data: %s\n",(char *)ptr);
    }
    return size*nmemb;
}
#if 0

static void ql_http_get_demo_thread(void * arg)
{
    struct Curl_easy  *curlP = NULL;
    ql_network_init_s staconfig;

    ql_rtos_semaphore_create(&wlan_semp, 0);
    ql_wlan_status_register_cb(ql_waln_event_cb);
    memset(&staconfig, 0, sizeof(staconfig));
    staconfig.wifi_mode = QL_STATION;
    memcpy(&staconfig.wifi_ssid, "H3C_2", sizeof("H3C_2"));
    memcpy(&staconfig.wifi_key, "12345678", sizeof("12345678"));
    staconfig.dhcp_mode = 1;
    if( ql_wlan_start(&staconfig) != 0){
        ql_debug("\n wifi sta startup failed!!!\n");
        goto end;
    }
    ql_rtos_semaphore_wait(wlan_semp, QL_WAIT_FOREVER);
    ql_debug("\n====start http get test=====\n");
    if(curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK){
        goto end;
    }
    curlP = curl_easy_init();
    if(curlP == NULL){
        goto end;
    }
    curl_easy_setopt(curlP, CURLOPT_URL, "http://www.quectel.com");
    curl_easy_setopt(curlP, CURLOPT_VERBOSE, 01L);
    curl_easy_setopt(curlP, CURLOPT_WRITEFUNCTION, f_write_callback);
    curl_easy_setopt(curlP, CURLOPT_DNS_SERVERS, "8.8.8.8");
    curl_easy_setopt(curlP, CURLOPT_CONNECTTIMEOUT, 120);
    curl_easy_perform(curlP);
    curl_easy_cleanup(curlP);
end:
    ql_rtos_semaphore_delete(wlan_semp);
    ql_rtos_task_delete(NULL);
    
}
#endif
static void ql_https_get_demo_thread(void * arg)
{
    struct Curl_easy  *curlP = NULL;

    ql_network_init_s staconfig;
    ql_rtos_semaphore_create(&wlan_semp, 0);
    ql_wlan_status_register_cb(ql_waln_event_cb);
    memset(&staconfig, 0, sizeof(staconfig));
    staconfig.wifi_mode = QL_STATION;
    memcpy(&staconfig.wifi_ssid, "H3C_2", sizeof("H3C_2"));
    memcpy(&staconfig.wifi_key, "12345678", sizeof("12345678"));
    staconfig.dhcp_mode = 1;
    if( ql_wlan_start(&staconfig) != 0){
        ql_debug("\n wifi sta startup failed!!!\n");
        goto end;
    }
    ql_rtos_semaphore_wait(wlan_semp, QL_WAIT_FOREVER);
  
    ql_debug("\n====start http get test=====\n");
    
    if(curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK){
        goto end;
    }

    curlP = curl_easy_init();
    if(curlP == NULL){
        goto end;
    }
    curl_easy_setopt(curlP, CURLOPT_URL, "https://www.baidu.com");
    curl_easy_setopt(curlP, CURLOPT_VERBOSE, 01L);
    curl_easy_setopt(curlP, CURLOPT_WRITEFUNCTION, f_write_callback);
    curl_easy_setopt(curlP, CURLOPT_DNS_SERVERS, "8.8.8.8");
    curl_easy_setopt(curlP, CURLOPT_CONNECTTIMEOUT, 120);
    curl_easy_setopt(curlP, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curlP, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_perform(curlP);
    curl_easy_cleanup(curlP);

end:
    ql_rtos_semaphore_delete(wlan_semp);
    ql_rtos_task_delete(NULL);
    
}

void ql_https_demo_thread_creat()
{
    int ret;
    ret = ql_rtos_task_create(&http_test_thread_handle,
                              (unsigned short)4*1024,
                              RTOS_HIGHER_PRIORTIY_THAN(3),
                              "http_test",
                              ql_https_get_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        ql_debug("Error: Failed to create http test thread: %d\r\n", ret);
        return;
    }

    return;
}
