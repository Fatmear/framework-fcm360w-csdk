#include <string.h>
#include "ql_wlan.h"
#include "ql_ori.h"
#include "ql_api_osi.h"
//#include <esp_log.h>
//#include <esp_http_server.h>
#include "cJSON.h"
#include "oshal.h"
#include "ql_ble.h"
#include "ql_ble_errcode.h"
static ql_task_t ap_ble_net_thread_handle = NULL;
extern void ql_wlan_ap_start();
extern void ql_ble_demo_get_mac_address(char *ble_mac);
extern void ql_ble_netconfig_gatt_event_cb(QL_BLE_GATT_PARAMS_EVT_T *p_event);
extern void ql_ble_netconfig_gap_event_cb(QL_BLE_GAP_PARAMS_EVT_T *p_event);
extern void ql_ble_net_config_server_init();
extern void ql_ble_set_adv_data(QL_BLE_DATA_T *pdata);
extern void ql_ble_set_scan_rsp_data(QL_BLE_DATA_T *pdata);
extern httpd_handle_t ql_start_webserver(void);
extern void ql_sta_connect_ap(char *ssid, char *password);
extern uint8_t ql_wlan_scan_start();
extern int ble_net_periphera_state;
extern ql_task_t ble_config_network_hdl;
extern ql_task_t ble_config_network_scan_hdl;
extern ql_queue_t ble_net_config_queue;
extern QL_BLE_GATTS_PARAMS_T ql_ble_gatt_service_net;
extern QL_BLE_SERVICE_PARAMS_T ql_ble_common_service_net[1];
extern QL_BLE_CHAR_PARAMS_T ql_ble_common_char1_net[2];
extern uint16_t g_ucBleMtu;
extern char g_cApSsid[32];
extern char g_cApPwd[64];
extern uint16_t net_conn_handle;
extern uint16_t net_char_handle;
extern uint8_t scan_start_flag;
int ap_net_state = 0;
void ql_ap_ble_net_demo(void *param)
{
    unsigned char ble_mac[6] = {0};
    ql_ble_demo_get_mac_address((char *)ble_mac);
    char periphera_demo_name[] = "FCM360W-BLE";
    QL_BLE_NET_MSG_ST msg_recv = {0};
    while (1)
    {
        if (ble_net_periphera_state == 0)
        {
            /* set_dev_config */
            QL_BLE_DEV_CONFIG dev_config = {0};
            dev_config.role = QL_BLE_ROLE_SLAVE;
            dev_config.renew_dur = QL_BLE_TMR_PRIV_ADDR_MIN;
            dev_config.privacy_cfg = QL_BLE_PRIV_CFG_PRIV_ADDR_BIT;
            memcpy(dev_config.addr.addr, ble_mac, 6);
            ql_ble_set_dev_config(&dev_config);
            ql_ble_set_device_name((uint8_t *)periphera_demo_name, strlen(periphera_demo_name));
            /* ble init */
            ql_ble_gap_callback_register(ql_ble_netconfig_gap_event_cb);
            ql_ble_gatt_callback_register(ql_ble_netconfig_gatt_event_cb);
            ql_ble_reset_all();
            ql_ble_net_config_server_init();
            /* 设置广播参数 */
            QL_BLE_DATA_T padv_data = {0};
            QL_BLE_DATA_T prsp_data = {0};
            QL_BLE_GAP_ADV_PARAMS_T adv_param = {0};

            adv_param.adv_interval_max = 160;
            adv_param.adv_interval_min = 160;
            adv_param.adv_channel_map = 1;
            adv_param.adv_type = QL_BLE_ADV_RSP_DATA;

            /* 设置蓝牙广播数据  */
            ql_ble_set_adv_data(&padv_data);
            ql_ble_set_scan_rsp_data(&prsp_data);
            ql_ble_adv_param_set(&padv_data, &prsp_data);

            /* adv */
            if (ql_ble_gap_adv_start(&adv_param) != QL_BT_ERROR_NO_ERROR)
            {
                os_printf(LM_CMD, LL_INFO, "adv start fail\r\n");
            }
            ble_net_periphera_state = 1;
        }
        if (ap_net_state == 0)
        {
            ql_rtos_task_sleep_ms(3000);
            ql_wlan_ap_start();
            if (ql_wlan_get_ap_status() == QL_AP_STATUS_STARTED)
            {
                httpd_handle_t server = ql_start_webserver();
                if (server)
                {
                    os_printf(LM_APP, LL_INFO, "ql_start_webserver!\r\n");
                }
            }
            ap_net_state = 1;
            ql_rtos_task_sleep_ms(1000);
            break;
        }
        if (0 == ql_rtos_queue_wait(ble_net_config_queue, sizeof(QL_BLE_NET_MSG_ST), (uint8_t *)&msg_recv, QL_WAIT_FOREVER))
        {
            switch (msg_recv.msg_type)
            {
            case QL_BLE_CMD_MSG_RECV_SCAN:
            {
                ql_wlan_scan_start();
            }
            break;
            case QL_BLE_CMD_MSG_RECV_NET_CONFIG:
            {
                memset(g_cApSsid, 0, sizeof(g_cApSsid));
                memset(g_cApPwd, 0, sizeof(g_cApPwd));
                char *_ssid = strchr((char *)msg_recv.data, '=');
                if (NULL != _ssid)
                {
                    _ssid++;
                    for (uint8_t i = 0; i < sizeof(g_cApSsid); i++)
                    {
                        if (_ssid[i] != ',')
                            g_cApSsid[i] = _ssid[i];
                        else
                            break;
                    }
                }
                char *pwd = strchr(_ssid, ',');
                if (NULL != pwd)
                {
                    pwd++;
                    for (uint8_t i = 0; i < sizeof(g_cApPwd); i++)
                    {
                        if (pwd[i] != '\r')
                            g_cApPwd[i] = pwd[i];
                        else
                            break;
                    }
                }
                os_printf(LM_WIFI, LL_INFO, "ssid = %s , pwd = %s\r\n", g_cApSsid, g_cApPwd);
                // connect ap
                ql_sta_connect_ap(g_cApSsid, g_cApPwd);
            }
            break;
            }
        }
        ql_rtos_task_sleep_ms(20);
    }
    ql_rtos_task_delete(NULL);
}
void ql_ap_ble_net_demo_thread_creat(void)
{
    int ret;
    ql_rtos_queue_create(&ble_net_config_queue, sizeof(QL_BLE_NET_MSG_ST), 10);
    ret = ql_rtos_task_create(&ap_ble_net_thread_handle,
                              (unsigned short)8 * 1024,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "ap_ble_net_test",
                              ql_ap_ble_net_demo,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_WIFI, LL_INFO, "Error: Failed to create wlan config net test thread: %d\r\n", ret);
        goto init_err;
    }
    return;

init_err:
    if (ap_ble_net_thread_handle != NULL)
    {
        ql_rtos_task_delete(ap_ble_net_thread_handle);
    }
}
