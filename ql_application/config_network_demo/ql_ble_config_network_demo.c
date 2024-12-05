#include <string.h>
#include "oshal.h"
#include "ql_ble.h"
#include "ql_api_osi.h"
#include "ql_ble_errcode.h"
#include "ql_ori.h"
#include "ql_wlan.h"
// char temp_buf[] = "+QWSCAN:\"quectel_wifi_test\",WPA2_AES_PSK,93,74:d2:1d:b7:ca:e1,7";
#define QL_RSSI_AP_POWER(rssi) (((rssi + 100) > 100) ? 100 : (rssi + 100))
#define QL_BLE_GAP_ADV_STATE_ADVERTISING_NET (0x02) /**< Advertising State */
#define QL_BLE_GAP_ADV_STATE_IDLE_NET (0x00)        /**< Idle, no advertising */
#define QL_BLE_GATT_SERVICE_MAX_NUM_NET (1)
#define QL_BLE_GATT_CHAR_MAX_NUM_NET (2)
#define QL_BLE_GATT_INVALID_HANDLE_NET (0xFFFF) /**< Invalid Connect Handle */
#define QL_BLE_CMD_SERVICE_UUID_NET (0xFFFF)
#define QL_BLE_CMD_WRITE_CHAR_UUID_NET (0xFF01)
#define QL_BLE_CMD_READ_CHAR_UUID_NET (0x2a2e)
#define QL_BLE_CMD_NOTIFY_CHAR_UUID_NET (0x2a2f)
#define QL_COMMON_CHAR_MAX_NUM_NET (2)
 ql_task_t ble_config_network_hdl = NULL;
 ql_task_t ble_config_network_scan_hdl = NULL;
 ql_queue_t ble_net_config_queue = NULL;
 QL_BLE_GATTS_PARAMS_T ql_ble_gatt_service_net = {0};
 QL_BLE_SERVICE_PARAMS_T ql_ble_common_service_net[QL_BLE_GATT_SERVICE_MAX_NUM_NET] = {0};
 QL_BLE_CHAR_PARAMS_T ql_ble_common_char1_net[QL_BLE_GATT_CHAR_MAX_NUM_NET] = {0};
 uint16_t g_ucBleMtu = 512 + 3;
 char g_cApSsid[32] = {0};
 char g_cApPwd[64] = {0};
int ble_net_periphera_state = 0;
uint16_t net_conn_handle = 0;
uint16_t net_char_handle = 0;
uint8_t scan_start_flag = 0;

void ql_ble_send_net_info(char *msg, unsigned int msg_len)
{
    int send = msg_len;
    char *p = msg;
    int mtu = g_ucBleMtu - 3;
    os_printf(LM_BLE, LL_INFO, "%s:%d,mtu=%d + 3 , msg_len = %d\r\n", __FUNCTION__, __LINE__, mtu, msg_len);
    while (send > mtu)
    {

        ql_ble_gatts_value_notify(net_conn_handle, ql_ble_common_char1_net[1].handle, (uint8_t *)p, mtu);
        send = send - mtu;
        p = p + mtu;
    }
    ql_ble_gatts_value_notify(net_conn_handle, ql_ble_common_char1_net[1].handle, (uint8_t *)p, send);
}
static int ql_demo_get_scan_result(void)
{
    char *resultbuf = NULL;
    int i;
    int n = 0;
#define WLAN_SCAN_BUF_SIZE (5 * 1024)
    resultbuf = os_malloc(WLAN_SCAN_BUF_SIZE);
    if (resultbuf == NULL)
    {
        os_printf(LM_WIFI, LL_INFO, "resultbuf malloc failed!\r\n");
        return 1;
    }

    QlWifiInfoTypeDef_st info = {0};
    int ap_num = ql_wlan_get_scan_num();
    os_printf(LM_WIFI, LL_INFO, "ap_num :%d\r\n", ap_num);
    resultbuf[0] = '\0';
    for (i = 0; i < ap_num; i++)
    {
        // memset((void *)&info, 0, sizeof(info));
        if (ql_wlan_scan_ap_result(i, &info) == 0)
        {
            if (n >= 512)
            {
                ql_ble_send_net_info(resultbuf, n);
                resultbuf[0] = '\0';
                n = 0;
            }
            if (info.ssid_len >= 32)
            {
                char temp[33];
                memcpy(temp, info.ssid, 32);
                temp[32] = '\0';
                n += sprintf(resultbuf + n, "+QWSCAN:\"%s\",", temp);
            }
            else
            {
                n += sprintf(resultbuf + n, "+QWSCAN:\"%s\",", info.ssid);
            }
            switch (info.auth)
            {
            case QL_WLAN_AUTH_OPEN:
            {
                n += sprintf(resultbuf + n, "%s", "OPEN");
            }
            break;
            case QL_WLAN_AUTH_WEP:
            {
                n += sprintf(resultbuf + n, "%s", "WEP_SHARED");
            }
            break;
            case QL_WLAN_AUTH_WPA_PSK:
            {
                n += sprintf(resultbuf + n, "%s", "WPA_PSK");
            }
            break;
            case QL_WLAN_AUTH_WPA2_PSK:
            {
                n += sprintf(resultbuf + n, "%s", "WPA2_PSK");
            }
            break;
            case QL_WLAN_AUTH_WPA_WPA2_PSK:
            {
                n += sprintf(resultbuf + n, "%s", "WPA_WPA3_PSK");
            }
            break;
            }
            n += sprintf(resultbuf + n, ",%d", QL_RSSI_AP_POWER(info.rssi)); /// RSSI
            n += sprintf(resultbuf + n, ","
                                        "%02x:%02x:%02x:%02x:%02x:%02x ",
                         info.bssid[0], info.bssid[1], info.bssid[2], info.bssid[3], info.bssid[4], info.bssid[5]);
            n += sprintf(resultbuf + n, ",%d\r\n", info.channel);
            if (n > (WLAN_SCAN_BUF_SIZE - 512))
            {
                ql_ble_send_net_info(resultbuf, n);
                resultbuf[0] = '\0';
                n = 0;
            }
        }
    }
    ql_ble_send_net_info(resultbuf, n);
    os_printf(LM_CMD, LL_INFO, "scan_res:%d\r\n", n);
    if (resultbuf)
    {
        free(resultbuf);
        resultbuf = NULL;
    }

    return 0;
}
uint8_t ql_wlan_scan_start()
{
    // if (!ql_wlan_start_scan(true,NULL))
    if (!ql_wlan_start_scan())
    {
        if (ql_wlan_get_scan_num())
        {
            if (ql_demo_get_scan_result() != 0)
            {
                return -1;
            }
        }
        else
        {
            os_printf(LM_WIFI, LL_INFO, "not found bss.\n");
        }

        return CMD_RET_SUCCESS;
    }
    else
    {
        return CMD_RET_FAILURE;
    }
}
void ql_sta_connect_ap(char *ssid, char *password)
{
    os_printf(LM_WIFI, LL_INFO, "wifi connect.\r\n");
    
    ql_network_init_s wificonfig = {0};
    
    memset(&wificonfig, 0, sizeof(wificonfig));
    wificonfig.wifi_mode = QL_STATION;
    memcpy(&wificonfig.wifi_ssid, ssid,     strlen((char*)ssid));
    memcpy(&wificonfig.wifi_key,  password, strlen((char*)password));
    wificonfig.dhcp_mode = 1;
    os_printf(LM_CMD, LL_INFO, "ssid %s,pas :%s\r\n", wificonfig.wifi_ssid, wificonfig.wifi_key);
    ql_wlan_start(&wificonfig);
}
void ql_ble_netconfig_gap_event_cb(QL_BLE_GAP_PARAMS_EVT_T *p_event)
{
    switch (p_event->type)
    {
    case QL_BLE_GAP_EVT_RESET:
        os_printf(LM_CMD, LL_INFO, "QL_ble_gap_event_reset\n");
        break;
    case QL_BLE_GAP_EVT_CONNECT:
    {
        if (QL_BT_ERROR_NO_ERROR == p_event->result)
        {
            net_conn_handle = p_event->conn_handle;
            os_printf(LM_CMD, LL_INFO, "Device connect succcess\n");
            os_printf(LM_CMD, LL_INFO, "[fcm360w][conn_hadle]:%d\r\n", p_event->conn_handle);
            ql_ble_gattc_exchange_mtu_req(net_conn_handle, 512);
        }
        else
        {
            os_printf(LM_CMD, LL_INFO, "Device connect fail\n");
        }
    }
    break;
    case QL_BLE_GAP_EVT_ADV_STATE:
    {
        if (QL_BLE_GAP_ADV_STATE_ADVERTISING_NET == p_event->result)
        {
            os_printf(LM_CMD, LL_INFO, "adv activity success\n");
        }
        if (QL_BLE_GAP_ADV_STATE_IDLE_NET == p_event->result)
        {
            os_printf(LM_CMD, LL_INFO, "stop adv success\n");
        }
    }
    break;
    case QL_BLE_GAP_EVT_ADV_REPORT:
    {
        if ((p_event->gap_event.adv_report.data.p_data == NULL) || (p_event->gap_event.adv_report.data.length == 0))
        {
            // empty packet
            return;
        }
        int8_t i = 0;
        os_printf(LM_CMD, LL_INFO, "adv_addr");

        for (i = 5; i >= 0; i--)
        {
            os_printf(LM_CMD, LL_INFO, ":%02X", p_event->gap_event.adv_report.peer_addr.addr[i]);
        }
        os_printf(LM_CMD, LL_INFO, "\n\r");
        os_printf(LM_CMD, LL_INFO, "rssi:%d dBm\n\r", p_event->gap_event.adv_report.rssi);
    }
    break;
    case QL_BLE_GAP_EVT_CONN_PARAM_UPDATE:
        os_printf(LM_CMD, LL_INFO, "conn_interval=0x%04x\n", p_event->gap_event.conn_param.conn_interval_max);
        os_printf(LM_CMD, LL_INFO, "conn_latency=0x%04x\n", p_event->gap_event.conn_param.conn_latency);
        os_printf(LM_CMD, LL_INFO, "conn_sup_timeout=0x%04x\n\r", p_event->gap_event.conn_param.conn_sup_timeout);
        break;
    default:
        break;
    }
}
void ql_ble_netconfig_gatt_event_cb(QL_BLE_GATT_PARAMS_EVT_T *p_event)
{
    os_printf(LM_BLE, LL_INFO, "[p_event->type] %d\r\n", p_event->type);
    switch (p_event->type)
    {
    case QL_BLE_GATT_EVT_WRITE_REQ:
    {
        net_char_handle = p_event->gatt_event.write_report.char_handle;
        char *write_data = (char *)p_event->gatt_event.write_report.report.p_data;
        uint8_t len = p_event->gatt_event.write_report.report.length;
        os_printf(LM_BLE, LL_INFO, "len%d !\r\n",len);
        // write_data[len] = '\0';
        os_printf(LM_CMD, LL_INFO, "Char handle:%#02x, write_data:%s\n", p_event->gatt_event.write_report.char_handle, write_data);
        if (NULL != strstr(write_data, "AT+QWSCAN"))
        {
            // start wifi scan
            os_printf(LM_BLE, LL_INFO, "start wifi scan !\r\n");
            QL_BLE_NET_MSG_ST msg_send = {0};
            msg_send.msg_type = QL_BLE_CMD_MSG_RECV_SCAN;
            ql_rtos_queue_release(ble_net_config_queue, sizeof(QL_BLE_NET_MSG_ST), (uint8_t *)&msg_send, 0);
        }
        else if (NULL != strstr(write_data, "AT+QSTAAPINFO"))
        {
             // start wifi connect
            os_printf(LM_BLE, LL_INFO, "start wifi connect !\r\n");
            QL_BLE_NET_MSG_ST msg_send = {0};
            msg_send.msg_type = QL_BLE_CMD_MSG_RECV_NET_CONFIG;
            msg_send.data = malloc(64);
            memcpy(msg_send.data,write_data,strlen(write_data)+ 3);
            ql_rtos_queue_release(ble_net_config_queue, sizeof(QL_BLE_NET_MSG_ST), (uint8_t *)&msg_send, 0);
        }
    }
    break;

    case QL_BLE_GATT_EVT_WRITE_CMP:
    {
        if (p_event->gatt_event.write_result.result == QL_BT_ERROR_NO_ERROR)
        {
            os_printf(LM_BLE, LL_INFO, "The operation of write char handle %#02x is proceed\n", p_event->gatt_event.write_result.char_handle);
        }
        else
        {
            os_printf(LM_BLE, LL_INFO, "The operation of write char handle %#02x is failed,reason:%#02x\n", p_event->gatt_event.write_result.char_handle,
                      p_event->gatt_event.write_result.result);
        }
    }
    break;

    case QL_BLE_GATT_EVT_READ_RX:
    {
        os_printf(LM_BLE, LL_INFO, "Char handle %#02x,data:%s\n", p_event->gatt_event.data_read.char_handle,
                  p_event->gatt_event.data_read.report.p_data);
    }
    break;

    case QL_BLE_GATT_EVT_NOTIFY_INDICATE_RX:
    {
        os_printf(LM_CMD, LL_INFO, "notify/indicate char handle=%#02x\n", p_event->gatt_event.data_report.char_handle);
        os_printf(LM_CMD, LL_INFO, "notify/indicate char value length=%#02x\n", p_event->gatt_event.data_report.report.length);
        os_printf(LM_CMD, LL_INFO, "notify/indicate char value=");
        for (int i = 0; i < p_event->gatt_event.data_report.report.length; i++)
        {
            os_printf(LM_BLE, LL_INFO, "%c", p_event->gatt_event.data_report.report.p_data[i]);
        }
        os_printf(LM_BLE, LL_INFO, "\r\n");
    }
    break;

    case QL_BLE_GATT_EVT_NOTIFY_INDICATE_TX:
    {

        if (p_event->gatt_event.notify_result.result == QL_BT_ERROR_NO_ERROR)
        {
            os_printf(LM_BLE, LL_INFO, "The operation of notify/indicate char handle %#02x is proceed\n", p_event->gatt_event.notify_result.char_handle);
        }
        else
        {
            os_printf(LM_BLE, LL_INFO, "The operation of notify/indicate char handle %#02x is failed,reason:%#02x\n", p_event->gatt_event.notify_result.char_handle,
                      p_event->gatt_event.notify_result.result);
        }
    }
    break;

    case QL_BLE_GATT_EVT_CHAR_DISCOVERY:
    {
        if (p_event->result != QL_BT_ERROR_DISC_DONE)
        {
            os_printf(LM_CMD, LL_INFO, "char handle=%#02x\n", p_event->gatt_event.char_disc.handle);
            if (QL_BLE_UUID_TYPE_16 == p_event->gatt_event.char_disc.uuid.uuid_type)
                os_printf(LM_CMD, LL_INFO, "char uuid=%#04x\n", p_event->gatt_event.char_disc.uuid.uuid.uuid16);
            else if (QL_BLE_UUID_TYPE_32 == p_event->gatt_event.char_disc.uuid.uuid_type)
                os_printf(LM_CMD, LL_INFO, "char uuid=%#x\n", p_event->gatt_event.char_disc.uuid.uuid.uuid32);
            else if (QL_BLE_UUID_TYPE_128 == p_event->gatt_event.char_disc.uuid.uuid_type)
                os_printf(LM_CMD, LL_INFO, "char uuid=%#x\n", p_event->gatt_event.char_disc.uuid.uuid.uuid128);
        }
    }
    break;

    case QL_BLE_GATT_EVT_CHAR_DESC_DISCOVERY:
    {
        if (p_event->result != QL_BT_ERROR_DISC_DONE)
        {
            os_printf(LM_CMD, LL_INFO, "char descriptors handle=%#02x\n", p_event->gatt_event.desc_disc.cccd_handle);
            os_printf(LM_CMD, LL_INFO, "char descriptors uuid16=%#04x\n", p_event->gatt_event.desc_disc.uuid16);
        }
    }
    break;

    case QL_BLE_GATT_EVT_PRIM_SEV_DISCOVERY:
    {
        if (p_event->result != QL_BT_ERROR_DISC_DONE)
        {
            os_printf(LM_CMD, LL_INFO, "services start handle:%#02x\n", p_event->gatt_event.svc_disc.start_handle);
            os_printf(LM_CMD, LL_INFO, "services end handle:%#02x\n", p_event->gatt_event.svc_disc.end_handle);
            os_printf(LM_CMD, LL_INFO, "services uuid:%#04x\n", p_event->gatt_event.svc_disc.uuid.uuid.uuid16);
        }
    }
    break;

    default:
        break;
    }
}
void ql_ble_demo_get_mac_address(char *ble_mac)
{
    unsigned char mac[6] = {0};
    if (ble_mac == NULL)
    {
        return;
    }
    hal_system_get_config(STA_MAC, mac, sizeof(mac));
    memcpy(ble_mac, mac, 6);
}
void ql_ble_set_adv_data(QL_BLE_DATA_T *pdata)
{
    uint16_t index = 0;
    static uint8_t adv_data[31] = {0};
    char adv_mode[3] = {2, 1, 6};
    char service_uuid[4] = {3, 2, 1, 0xa2};
    memcpy(&adv_data[index], adv_mode, sizeof(adv_mode) / sizeof(adv_mode[0]));
    index += sizeof(adv_mode) / sizeof(adv_mode[0]);

    // set service uuid
    memcpy(&adv_data[index], service_uuid, sizeof(service_uuid) / sizeof(service_uuid[0]));
    index += sizeof(service_uuid) / sizeof(service_uuid[0]);

    pdata->length = index;
    pdata->p_data = adv_data;
}
void ql_ble_set_scan_rsp_data(QL_BLE_DATA_T *pdata)
{
    uint16_t index = 0;
    static uint8_t rsp_data[31] = {0};
    char temp_name[18] = {'\0'};
    char default_name[13] = {0x0C, 0x09, 'F', 'C', 'M', '3', '6', '0', 'W', '-', 'B', 'L', 'E'};
    memcpy(&rsp_data[index], temp_name, strlen(temp_name));
    int ret = hal_system_get_config(CUSTOMER_NV_BLE_DEVICE_NAME, temp_name, sizeof(temp_name));
    if (ret)
    {
        // set nv device name
        rsp_data[index++] = strlen(temp_name) + 1;
        rsp_data[index++] = 0x09;
        memcpy(&rsp_data[index], temp_name, strlen(temp_name));
        index += strlen(temp_name);
    }
    else
    {
        // set default device name
        memcpy(&rsp_data[index], default_name, sizeof(default_name) / sizeof(default_name[0]));
        index += sizeof(default_name) / sizeof(default_name[0]);
    }
    pdata->length = index;
    pdata->p_data = rsp_data;
}

void ql_ble_net_config_server_init()
{
    memset(ql_ble_common_service_net, 0x00, sizeof(QL_BLE_SERVICE_PARAMS_T) * QL_BLE_GATT_SERVICE_MAX_NUM_NET);

    QL_BLE_GATTS_PARAMS_T *p_ble_service = &ql_ble_gatt_service_net;
    p_ble_service->svc_num = 1;
    p_ble_service->p_service = ql_ble_common_service_net;

    /*First service add*/
    QL_BLE_SERVICE_PARAMS_T *p_common_service = ql_ble_common_service_net;
    p_common_service->handle = QL_BLE_GATT_INVALID_HANDLE_NET;
    p_common_service->svc_uuid.uuid_type = QL_BLE_UUID_TYPE_16;
    p_common_service->svc_uuid.uuid.uuid16 = QL_BLE_CMD_SERVICE_UUID_NET;
    p_common_service->type = QL_BLE_UUID_SERVICE_PRIMARY;
    p_common_service->char_num = QL_COMMON_CHAR_MAX_NUM_NET;
    p_common_service->p_char = ql_ble_common_char1_net;

    /*Add write characteristic*/
    QL_BLE_CHAR_PARAMS_T *p_common_char = ql_ble_common_char1_net;
    p_common_char->handle = QL_BLE_GATT_INVALID_HANDLE_NET;
    p_common_char->char_uuid.uuid_type = QL_BLE_UUID_TYPE_16;
    p_common_char->char_uuid.uuid.uuid16 = QL_BLE_CMD_WRITE_CHAR_UUID_NET;

    p_common_char->property = QL_BLE_GATT_CHAR_PROP_WRITE | QL_BLE_GATT_CHAR_PROP_NOTIFY;
    p_common_char->permission = QL_BLE_GATT_PERM_READ | QL_BLE_GATT_PERM_WRITE;
    p_common_char->value_len = 252;
    p_common_char++;

    /*Add Notify characteristic*/
    p_common_char->handle = QL_BLE_GATT_INVALID_HANDLE_NET;
    p_common_char->char_uuid.uuid_type = QL_BLE_UUID_TYPE_16;
    p_common_char->char_uuid.uuid.uuid16 = QL_BLE_CMD_NOTIFY_CHAR_UUID_NET;

    p_common_char->property = QL_BLE_GATT_CHAR_PROP_NOTIFY | QL_BLE_GATT_CHAR_PROP_INDICATE;
    p_common_char->permission = QL_BLE_GATT_PERM_READ | QL_BLE_GATT_PERM_WRITE;
    p_common_char->value_len = 252;
    p_common_char++;

    ql_ble_gatts_service_add(p_ble_service, false);
}
void ql_ble_net_demo_entry(void *param)
{
    QL_BLE_NET_MSG_ST msg_recv = {0};
    while (1)
    {
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
                    char *_ssid = strchr((char*)msg_recv.data, '=');
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
}
void ql_ble_config_network_demo_entry(void *param)
{
    unsigned char ble_mac[6] = {0};
    ql_ble_demo_get_mac_address((char *)ble_mac);
    char periphera_demo_name[] = "FCM360W-BLE";
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
            /* Set broadcast parameters */
            QL_BLE_DATA_T padv_data = {0};
            QL_BLE_DATA_T prsp_data = {0};
            QL_BLE_GAP_ADV_PARAMS_T adv_param = {0};

            adv_param.adv_interval_max = 160;
            adv_param.adv_interval_min = 160;
            adv_param.adv_channel_map = 1;
            adv_param.adv_type = QL_BLE_ADV_RSP_DATA;

            /* Set Bluetooth broadcast data  */
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
        ql_rtos_task_sleep_ms(20);
    }

    ql_rtos_task_delete(NULL);
}
void ql_ble_config_network_thread_creat(void)
{
    int ret;
    ql_rtos_queue_create(&ble_net_config_queue, sizeof(QL_BLE_NET_MSG_ST), 10);
    ret = ql_rtos_task_create(&ble_config_network_hdl,
                              8 * 1024,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "ble_network",
                              ql_ble_config_network_demo_entry,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        return;
    }

    ret = ql_rtos_task_create(&ble_config_network_scan_hdl,
                              8 * 1024,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "ble_net_scan",
                              ql_ble_net_demo_entry,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        return;
    }
    return;
}