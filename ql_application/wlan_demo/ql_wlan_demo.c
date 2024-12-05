/**************************************************************************************************
* include
**************************************************************************************************/
#include <string.h>
#include "ql_wlan.h"
#include "ql_ori.h"
#include "ql_api_osi.h"
#include "ql_debug.h"
#include "ql_mem.h"
/**************************************************************************************************
* define
**************************************************************************************************/
#define WLAN_SOFTAP_TEST    1
#define WLAN_STA_TEST       0
#define WLAN_MONITOR_TEST   0

#define QL_WLAN_DEFAULT_IP      "192.168.30.1"
#define QL_WLAN_DEFAULT_GW      "192.168.30.1"
#define QL_WLAN_DEFAULT_MASK    "255.255.255.0"
#define QL_WLAN_DEFAULT_DNS     "114.114.114.114"
/**************************************************************************************************
* Parameter
**************************************************************************************************/
uint8_t get_ap_info = 0;
uint8_t get_sta_info = 0;
/**************************************************************************************************
* Function
**************************************************************************************************/
/**************************************************************************************************
* Function    : ql_wlan_event_cb
*
* Author      : 
*
* Parameters  :
*
* Return      :
*
* Description :
**************************************************************************************************/
ql_sys_err_t ql_wlan_event_cb_demo(void *ctx, ql_system_event_t *event)
{
	ql_debug("event_id:%d \r\n", event->event_id);
    
	switch (event->event_id) 
	{
	    case QL_SYSTEM_EVENT_STA_CONNECTED:
            break;
            
		case QL_SYSTEM_EVENT_STA_GOT_IP:
        {
            get_sta_info = 1;
            break;
        }      

		case QL_SYSTEM_EVENT_STA_DISCONNECTED:
			break;

        case QL_SYSTEM_EVENT_AP_START:
        {
            get_ap_info = 1;
            break;
        }

		case QL_SYSTEM_EVENT_AP_STACONNECTED:
			break;
            
        case QL_SYSTEM_EVENT_AP_STAIPASSIGNED:
            break;

		case QL_SYSTEM_EVENT_AP_STADISCONNECTED:
            break;

		default:
			break;
	}

	return 0;
}

#if WLAN_SOFTAP_TEST
/**************************************************************************************************
* Function    : ql_wlan_softap_demo
*
* Author      : 
*
* Parameters  :
*
* Return      :
*
* Description :
**************************************************************************************************/
static void ql_wlan_softap_demo(void)
{
    ql_network_init_s wificonfig;
    
    memset(&wificonfig, 0, sizeof(wificonfig));
    
    wificonfig.wifi_mode = QL_SOFT_AP;
    memcpy(&wificonfig.wifi_ssid, "FCM360W-40D", sizeof("FCM360M-40D"));
    memcpy(&wificonfig.wifi_key, "12345678", sizeof("12345678"));
    wificonfig.authmode = QL_WLAN_AUTH_WPA2_PSK;
    wificonfig.max_connect = 2;
    wificonfig.dhcp_mode = 0;
    
    strcpy(wificonfig.local_ip_addr,      QL_WLAN_DEFAULT_IP);
    strcpy(wificonfig.net_mask,           QL_WLAN_DEFAULT_MASK);
    strcpy(wificonfig.gateway_ip_addr,    QL_WLAN_DEFAULT_GW);
    strcpy(wificonfig.dns_server_ip_addr, QL_WLAN_DEFAULT_DNS);
    
    ql_wlan_start(&wificonfig);
}
#endif

#if WLAN_STA_TEST
/**************************************************************************************************
* Function    : ql_wlan_sta_demo
*
* Author      : 
*
* Parameters  :
*
* Return      :
*
* Description :
**************************************************************************************************/
static void ql_wlan_sta_demo(void)
{    
    int ret = 0;
    ql_scan_result_s apList = {0};

    memset(&apList, 0x00, sizeof(ql_scan_result_s));
    ret = ql_wlan_start_scan();
    ql_debug("ret:%d \r\n", ret);
    
//    char *ssid="quectel_wifi_test";
//    ql_wlan_start_assign_scan((uint8_t **)&ssid, 1);
    
    apList.ql_aplist_s = (ql_apliststruct*)ql_malloc(32 * sizeof(ql_apliststruct));
    
    ql_wlan_sta_scan_result(&apList);
    ql_debug("ApNum:%d \r\n", apList.apnum);   
    for(uint8_t i = 0; i < apList.apnum; i++)
    {
        ql_debug("scan ap result :%s, %d, %02x:%02x:%02x:%02x:%02x:%02x, channel:%d, appower:%d \r\n",
            (char *)apList.ql_aplist_s[i].ssid, apList.ql_aplist_s[i].security, apList.ql_aplist_s[i].bssid[0],
            apList.ql_aplist_s[i].bssid[1], apList.ql_aplist_s[i].bssid[2],apList.ql_aplist_s[i].bssid[3], 
            apList.ql_aplist_s[i].bssid[4], apList.ql_aplist_s[i].bssid[5], apList.ql_aplist_s[i].channel,
            apList.ql_aplist_s[i].appower);
    }

    ql_free(apList.ql_aplist_s);

    ql_network_init_s wificonfig;
    
    memset(&wificonfig, 0, sizeof(wificonfig));
    wificonfig.wifi_mode = QL_STATION;
    memcpy(&wificonfig.wifi_ssid, "quectel_wifi_test", sizeof("quectel_wifi_test"));
    memcpy(&wificonfig.wifi_key, "12345678", sizeof("12345678"));
    wificonfig.dhcp_mode = 0;
    strcpy((void *)wificonfig.local_ip_addr,      (const void *)QL_WLAN_DEFAULT_IP);
    strcpy((void *)wificonfig.net_mask,           (const void *)QL_WLAN_DEFAULT_MASK);
    strcpy((void *)wificonfig.gateway_ip_addr,    (const void *)QL_WLAN_DEFAULT_GW);
    strcpy((void *)wificonfig.dns_server_ip_addr, (const void *)QL_WLAN_DEFAULT_DNS);
    ql_wlan_start(&wificonfig);
}
#endif

#if WLAN_MONITOR_TEST
/**************************************************************************************************
* Function    : ql_monitor_cb_demo
*
* Author      : 
*
* Parameters  :
*
* Return      :
*
* Description :
**************************************************************************************************/
void ql_monitor_cb_demo(uint8_t *data, int len, ql_wifi_link_info_t *info)
{    
    ql_debug("len=%d rssi=%d \r\n",len, info->rssi);
}

/**************************************************************************************************
* Function    : ql_wlan_monitor_demo
*
* Author      : 
*
* Parameters  :
*
* Return      :
*
* Description :
**************************************************************************************************/
static void ql_wlan_monitor_demo(void)
{
    ql_wlan_register_monitor_cb(ql_monitor_cb_demo);
    
    ql_wlan_start_monitor(QL_WIFI_PROMIS_FILTER_MASK_ALL, 6);
}
#endif

/**************************************************************************************************
* Function    : ql_wlan_demo_thread
*
* Author      : 
*
* Parameters  :
*
* Return      :
*
* Description :
**************************************************************************************************/
void ql_wlan_demo_thread(void *param)
{
    ql_rtos_task_sleep_ms(3000);
    
	ql_wlan_status_register_cb(ql_wlan_event_cb_demo);
    
    #if WLAN_SOFTAP_TEST
    ql_wlan_softap_demo();

    #elif WLAN_STA_TEST
    ql_wlan_sta_demo();
    
    #elif WLAN_MONITOR_TEST
    ql_wlan_monitor_demo();
    #endif

    while (1)
    {
        
#if WLAN_SOFTAP_TEST
        if(get_ap_info)
        {
            get_ap_info = 0;
            
            ql_network_init_ap_s ap_info;

            memset((void *)&ap_info, 0x00, sizeof(ql_network_init_ap_s));
            ql_wlan_ap_para_info_get(&ap_info);
            ql_debug("wifi_ssid=%s, wifi_key=%s, channel:%d, security:%d, ssid_hidden:%d, max_con:%d  \r\n", 
                 ap_info.wifi_ssid, ap_info.wifi_key, ap_info.channel, ap_info.security, ap_info.ssid_hidden, ap_info.max_con);
            ql_debug("local_ip_addr:%s, net_mask:%s, gateway_ip_addr:%s, dns_server_ip_addr:%s, dhcp_mode:%d \r\n", 
                  (char*)ap_info.local_ip_addr, (char*)ap_info.net_mask, (char*)ap_info.gateway_ip_addr,
                 (char*)ap_info.dns_server_ip_addr, ap_info.dhcp_mode);
                 
            ql_ip_status_s ipstatus;
            
            memset((void *)&ipstatus, 0x00, sizeof(ql_ip_status_s));
            ql_wlan_get_ip_status(&ipstatus, QL_SOFT_AP);
            ql_debug("dhcp:%d, ip:%s, gate:%s, mask:%s, dns:%s, standby_dns:%s \r\n",
                ipstatus.dhcp, ipstatus.ip, ipstatus.gate, ipstatus.mask, ipstatus.dns, ipstatus.standby_dns);

            ql_debug("get_channel:%d \r\n", ql_wlan_get_channel());
            ql_debug("ap_ip_is_start:%d \r\n", ql_ap_ip_is_start());

            ql_rtos_task_sleep_ms(10000);
            
            ql_wlan_stop(QL_SOFT_AP);
        }
#endif

#if WLAN_STA_TEST
        if(get_sta_info)
        {
            get_sta_info = 0;
            
            ql_ip_status_s ipstatus;
            
            memset((void *)&ipstatus, 0x00, sizeof(ql_ip_status_s));
            ql_wlan_get_ip_status(&ipstatus, QL_STATION);
            ql_debug("dhcp:%d, ip:%s, gate:%s, mask:%s, dns:%s, standby_dns:%s \r\n",
                ipstatus.dhcp, ipstatus.ip, ipstatus.gate, ipstatus.mask, ipstatus.dns, ipstatus.standby_dns);

            ql_link_status_s linkStatus;

            memset((void *)&linkStatus, 0x00, sizeof(ql_link_status_s));
            ql_wlan_get_link_status(&linkStatus);
            ql_debug("conn_state=%d, channel=%d, security=%d, rssi=%d, ssid=%s, bssid=%02x:%02x:%02x:%02x:%02x:%02x \r\n",
                linkStatus.conn_state, linkStatus.channel, linkStatus.security,
                linkStatus.wifi_strength, linkStatus.ssid, linkStatus.bssid[0],
                linkStatus.bssid[1], linkStatus.bssid[2], linkStatus.bssid[3],
                linkStatus.bssid[4], linkStatus.bssid[5]);
            
            ql_debug("ql_sta_ip_is_start:%d \r\n", ql_sta_ip_is_start());
            
            ql_rtos_task_sleep_ms(10000);
            
            ql_wlan_stop(QL_STATION);
        }

#endif
        ql_rtos_task_sleep_ms(1000);
    }
}

ql_task_t wlan_test_thread_handle = NULL;
/**************************************************************************************************
* Function    : ql_wlan_demo_thread_creat
*
* Author      : 
*
* Parameters  :
*
* Return      :
*
* Description :
**************************************************************************************************/
void ql_wlan_demo_thread_creat(void)
{
    int ret;
    ret = ql_rtos_task_create(&wlan_test_thread_handle,
                              (unsigned short)4096,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "wlan_test",
                              ql_wlan_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO, "Error: Failed to create wlan test thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (wlan_test_thread_handle != NULL)
    {
        ql_rtos_task_delete(wlan_test_thread_handle);
    }
}


