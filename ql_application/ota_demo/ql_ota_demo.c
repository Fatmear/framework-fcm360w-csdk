#include "ql_ota.h"
#include "ql_debug.h"
#include "ql_api_osi.h"
#include "ql_wlan.h"
#include "curl/curl.h"
#include "curl/easy.h"

#define OTA_SERVER "https://dwg-1320338502.cos.ap-guangzhou.myqcloud.com/new3.bin"

ql_task_t ota_test_thread_handle = NULL;
static ql_sem_t  wlan_semp = NULL;


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

static size_t f_write_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    if(ptr != NULL){
        ql_ota_write((unsigned char *)ptr, nmemb);
    }
    return size*nmemb;
}

void ql_ota_demo_thread(void *parpm)
{
    ql_network_init_s staconfig;
    ql_rtos_semaphore_create(&wlan_semp, 0);
    ql_wlan_status_register_cb(ql_waln_event_cb);
    memset(&staconfig, 0, sizeof(staconfig));
    staconfig.wifi_mode = QL_STATION;
    memcpy(&staconfig.wifi_ssid, "nova", sizeof("nova"));
    memcpy(&staconfig.wifi_key, "12345678", sizeof("12345678"));
    staconfig.dhcp_mode = 1;
    if( ql_wlan_start(&staconfig) != QL_WIFI_SUCCESS){
        ql_debug("\n wifi sta startup failed!!!\n");
        goto end;
    }
    ql_rtos_semaphore_wait(wlan_semp, QL_WAIT_FOREVER);

    struct Curl_easy  *curlP = NULL;

    if(curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK){
        goto end;
    }

    curlP = curl_easy_init();
    if(curlP == NULL){
        goto end;
    }
    ql_ota_init();
    curl_easy_setopt(curlP, CURLOPT_URL, OTA_SERVER);
    curl_easy_setopt(curlP, CURLOPT_VERBOSE, 01L);
    curl_easy_setopt(curlP, CURLOPT_WRITEFUNCTION, f_write_callback);
    curl_easy_setopt(curlP, CURLOPT_DNS_SERVERS, "8.8.8.8");
    curl_easy_setopt(curlP, CURLOPT_CONNECTTIMEOUT, 120);
    curl_easy_setopt(curlP, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curlP, CURLOPT_SSL_VERIFYHOST, 0);

    CURLcode code = curl_easy_perform(curlP);
    if(code != CURLE_OK)
    {
        os_printf(LM_OS, LL_INFO, "curl_easy_perform %d\n", code);
        goto end;
    }
    ql_ota_done(1);
    curl_easy_cleanup(curlP);


end:
    os_printf(LM_OS, LL_INFO, "exit ota demo thread\r\n");
    ql_rtos_semaphore_release(wlan_semp);
    ql_rtos_task_delete(ota_test_thread_handle);        
}

void ql_ota_demo_thread_creat(void)
{
    int ret;
    ret = ql_rtos_task_create(&ota_test_thread_handle,
                              (unsigned short)1024*4,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "ota_test",
                              ql_ota_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO,"Error: Failed to create ota test thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (ota_test_thread_handle != NULL)
    {
        ql_rtos_task_delete(ota_test_thread_handle);
    }
}