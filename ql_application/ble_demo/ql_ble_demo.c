/*================================================================
  Copyright (c) 2023, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
=================================================================*/
/**************************************************************************************************
* include
**************************************************************************************************/
#include <string.h>
#include "oshal.h"
#include "ql_ble.h"
#include "ql_api_osi.h"
#include "ql_ble_errcode.h"
#include "ql_ori.h"
#include "ql_mem.h"
#include "ql_debug.h"
/**************************************************************************************************
* define
**************************************************************************************************/
#define QL_BLE_GATT_INVALID_HANDLE (0xFFFF)     /**< Invalid Connect Handle */
#define QL_BLE_GAP_ADV_STATE_IDLE (0x00)        /**< Idle, no advertising */
#define QL_BLE_GAP_ADV_STATE_START (0x01)       /**< Start Advertising. A temporary state, haven't received the result.*/
#define QL_BLE_GAP_ADV_STATE_ADVERTISING (0x02) /**< Advertising State */
#define QL_BLE_GATT_SERVICE_MAX_NUM (1)
#define QL_BLE_GATT_CHAR_MAX_NUM (3)
#define QL_BLE_DEMO_CLEINT_MAX_MTU 500
#define CFG_ENABLE_QUECTEL_BLE_PERIPHERA 1
#define CFG_ENABLE_QUECTEL_BLE_CENTRAL 0

#define QL_BLE_DEMO_START_SCAN           (1 << 0)
#define QL_BLE_DEMO_START_CONN           (1 << 1)
#define QL_BLE_DEMO_GATTC_SET_CHARA_NTF  (1 << 2)
#define QL_BLE_DEMO_GATTC_READ           (1 << 3)
#define QL_BLE_DEMO_GATTC_WRITE          (1 << 4)

#define QL_BLE_CMD_SERVICE_UUID     (0x180b)
#define QL_BLE_CMD_WRITE_CHAR_UUID  (0x2a2d)
#define QL_BLE_CMD_READ_CHAR_UUID   (0x2a2e)
#define QL_BLE_CMD_WRITE_NOTIFY_CHAR_UUID (0xFF01)
#define QL_COMMON_CHAR_MAX_NUM (3)
/**************************************************************************************************
* param
**************************************************************************************************/
uint16_t conn_handle = 0;
uint8_t  demo_peer_addr[6] = {0};
uint8_t  demo_peer_addr_type = 0;
uint16_t ble_demo_state = QL_BLE_DEMO_START_SCAN;
uint16_t rd_chara_hdl_demo = 0;
uint16_t wr_chara_hdl_demo = 0;
/**************************************************************************************************
* Function
**************************************************************************************************/
/**************************************************************************************************
* Function: ql_ble_analyze_adv_data
*
* Author  : Lei.wang
*
* Date    : 2023/12/11 
*
* Description:
*	
* 
* Parameters:
*	none
*	
* Return:
*	none
*	
**************************************************************************************************/
static int ql_ble_analyze_adv_data(QL_BLE_DATA_T *adv_data)
{
    QL_BLE_DATA_T dev_name_config = {0};
    uint8_t index = 0;
    while ((adv_data->length)--)
    {
        if (adv_data->p_data[index] == 0x09)
        {
            // dev_name_config.length = adv_data->p_data[index -1] -1;
            dev_name_config.p_data = (uint8_t *)&(adv_data->p_data[index + 1]);
            if (!memcmp(dev_name_config.p_data, "fcm360w_ble", strlen("fcm360w_ble")))
            {
                return 1;
            }
        }
        else
        {
            index++;
        }
    }
    return 0;
}

/**************************************************************************************************
* Function: ql_ble_gap_event_cb
*
* Author  : Lei.wang
*
* Date    : 2023/12/11 
*
* Description:
*	
* 
* Parameters:
*	none
*	
* Return:
*	none
*	
**************************************************************************************************/
void ql_ble_gap_event_cb(QL_BLE_GAP_PARAMS_EVT_T *p_event)
{
    switch (p_event->type)
    {
        case QL_BLE_GAP_EVT_RESET:
        {
            os_printf(LM_CMD, LL_INFO, "QL_ble_gap_event_reset\n");
            break;
        }
        
        case QL_BLE_GAP_EVT_CONNECT:
        {
            if (QL_BT_ERROR_NO_ERROR == p_event->result)
            {
                conn_handle = p_event->conn_handle;
                os_printf(LM_CMD, LL_INFO, "Device connect succcess\n");
                os_printf(LM_CMD, LL_INFO, "[fcm360w][conn_hadle]:%d\r\n", p_event->conn_handle); 
                
                #if CFG_ENABLE_QUECTEL_BLE_CENTRAL
                ql_ble_gattc_exchange_mtu_req(conn_handle, QL_BLE_DEMO_CLEINT_MAX_MTU);
                ql_ble_gattc_all_service_discovery(conn_handle);
                #endif
            }
            else
            {
                os_printf(LM_CMD, LL_INFO, "Device connect fail\n");
            }
            
            break;
        }

        case QL_BLE_GAP_EVT_DISCONNECT:
        {
            os_printf(LM_CMD, LL_INFO, "conn_handle: [%d], p_event->result: [%d] \r\n", p_event->conn_handle, p_event->result);
            os_printf(LM_CMD, LL_INFO, "role: [%d], reason: [%02x] \r\n", 
                p_event->gap_event.disconnect.role, p_event->gap_event.disconnect.reason);

            break;
        }
        
        case QL_BLE_GAP_EVT_ADV_STATE:
        {
            if (QL_BLE_GAP_ADV_STATE_ADVERTISING == p_event->result)
            {
                os_printf(LM_CMD, LL_INFO, "adv activity success\n");                                
            }
            if (QL_BLE_GAP_ADV_STATE_IDLE == p_event->result)
            {
                os_printf(LM_CMD, LL_INFO, "stop adv success\n");
            }
            
            break;
        }

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
            if (ql_ble_analyze_adv_data(&(p_event->gap_event.adv_report.data)) == 1)
            {
                demo_peer_addr_type = p_event->gap_event.adv_report.peer_addr.type;
                memcpy(demo_peer_addr, p_event->gap_event.adv_report.peer_addr.addr, 6);
                ble_demo_state |= QL_BLE_DEMO_START_CONN;
            }
            
            break;
        }
        
        case QL_BLE_GAP_EVT_CONN_PARAM_UPDATE:
        {
            os_printf(LM_CMD, LL_INFO, "conn_interval=0x%04x\n", p_event->gap_event.conn_param.conn_interval_max);
            os_printf(LM_CMD, LL_INFO, "conn_latency=0x%04x\n", p_event->gap_event.conn_param.conn_latency);
            os_printf(LM_CMD, LL_INFO, "conn_sup_timeout=0x%04x\n\r", p_event->gap_event.conn_param.conn_sup_timeout);
            os_printf(LM_CMD, LL_INFO, "conn_establish_timeout=0x%04x\n\r", p_event->gap_event.conn_param.conn_establish_timeout);

            break;
        }

        default:
            break;
    }
}

/**************************************************************************************************
* Function: ql_ble_gatt_event_cb
*
* Author  : Lei.wang
*
* Date    : 2023/12/11 
*
* Description:
*	
* 
* Parameters:
*	none
*	
* Return:
*	none
*	
**************************************************************************************************/
void ql_ble_gatt_event_cb(QL_BLE_GATT_PARAMS_EVT_T *p_event)
{
    os_printf(LM_BLE, LL_INFO, "[p_event->type] %d\r\n", p_event->type);
    switch (p_event->type)
    {
        case QL_BLE_GATT_EVT_MTU_REQUEST:
        {
            ql_debug("QL_BLE_GATT_EVT_MTU_REQUEST exchange_mtu: [%d] \r\n", p_event->gatt_event.exchange_mtu);
            break;
        }

        case QL_BLE_GATT_EVT_MTU_RSP:
        {
            ql_debug("QL_BLE_GATT_EVT_MTU_RSP exchange_mtu: [%d,%d] \r\n", p_event->gatt_event.exchange_mtu, ql_ble_gattc_get_mtu(conn_handle));
            
            break;
        }
        
        case QL_BLE_GATT_EVT_WRITE_REQ:
        {
            uint16_t char_handle = p_event->gatt_event.write_report.char_handle;
            char *write_data = (char *)p_event->gatt_event.write_report.report.p_data;
            uint8_t len = p_event->gatt_event.write_report.report.length;
            write_data[len] = '\0';
            os_printf(LM_CMD, LL_INFO, "Char handle:%#02x, write_data:%s\n", p_event->gatt_event.write_report.char_handle, write_data);

            #if CFG_ENABLE_QUECTEL_BLE_PERIPHERA
            ql_ble_gatts_value_notify(conn_handle, char_handle, (uint8_t *)write_data, len);
            #endif
            break;
        }

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
            os_printf(LM_BLE, LL_INFO, "\n");

            #if CFG_ENABLE_QUECTEL_BLE_CENTRAL
            ql_ble_gattc_write(conn_handle, p_event->gatt_event.data_report.char_handle, (uint8_t *)(p_event->gatt_event.data_report.report.p_data), p_event->gatt_event.data_report.report.length);
            #endif
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
                ql_debug("chara handle: [%d] \r\n", p_event->gatt_event.char_disc.handle);
            
                if(p_event->gatt_event.char_disc.uuid.uuid_type == QL_BLE_UUID_TYPE_16)
                {
                    ql_debug("chara uuid len: [2] uuid: [%04x] \r\n", p_event->gatt_event.char_disc.uuid.uuid.uuid16);
                    
                    if(p_event->gatt_event.char_disc.uuid.uuid.uuid16 == 0x2a00)
                    {
                        ble_demo_state |= QL_BLE_DEMO_GATTC_READ;
                        rd_chara_hdl_demo = p_event->gatt_event.char_disc.handle;
                        
                        ql_debug("ble_demo_state: [%d], rd_chara_hdl_demo: [%d] \r\n", ble_demo_state, rd_chara_hdl_demo);
                    }
                    
                    if(p_event->gatt_event.char_disc.uuid.uuid.uuid16 == 0xff01)
                    {
                        ble_demo_state |= QL_BLE_DEMO_GATTC_SET_CHARA_NTF;
                        wr_chara_hdl_demo = p_event->gatt_event.char_disc.handle;
                        
                        ql_debug("wr_chara_hdl_demo: [%d] \r\n", wr_chara_hdl_demo);
                    }
                }
                else
                if(p_event->gatt_event.char_disc.uuid.uuid_type == QL_BLE_UUID_TYPE_128)
                {
                    ql_debug("chara uuid len: [16] uuid: [");
                    
                    for(uint8_t idx = 0; idx < 16; idx++)
                    {
                        ql_debug("%02x", p_event->gatt_event.char_disc.uuid.uuid.uuid128[16-idx-1]);
                    }
                    
                    ql_debug("] \r\n");
                }
            }
            
            break;
        }

        case QL_BLE_GATT_EVT_CHAR_DESC_DISCOVERY:
        {
            if (p_event->result != QL_BT_ERROR_DISC_DONE)
            {                
                os_printf(LM_CMD, LL_INFO, "char descriptors handle=%#02x\n", p_event->gatt_event.desc_disc.cccd_handle);
                os_printf(LM_CMD, LL_INFO, "char descriptors uuid16=%#04x\n", p_event->gatt_event.desc_disc.uuid16);
            }
            
            break;
        }

        case QL_BLE_GATT_EVT_PRIM_SEV_DISCOVERY:
        {
            if (p_event->result != QL_BT_ERROR_DISC_DONE)
            {
                ql_debug("services start_handle: [%d], end_handle: [%d] \r\n", 
                    p_event->gatt_event.svc_disc.start_handle, p_event->gatt_event.svc_disc.end_handle);
                
                if(p_event->gatt_event.svc_disc.uuid.uuid_type == QL_BLE_UUID_TYPE_16)
                {
                    ql_debug("services uuid len: [2], uuid: [%04x] \r\n", p_event->gatt_event.svc_disc.uuid.uuid.uuid16);
                }
                else
                if(p_event->gatt_event.svc_disc.uuid.uuid_type == QL_BLE_UUID_TYPE_128)
                {
                    ql_debug("services uuid len: [16], uuid: [");
                    
                    for(uint8_t idx = 0; idx < 16; idx++)
                    {
                        ql_debug("%02x", p_event->gatt_event.svc_disc.uuid.uuid.uuid128[16-idx-1]);
                    }
                    
                    ql_debug("] \r\n");
                }
            }
            
            #if CFG_ENABLE_QUECTEL_BLE_CENTRAL
            ql_ble_gattc_all_char_discovery(conn_handle, 
                                            p_event->gatt_event.svc_disc.start_handle, 
                                            p_event->gatt_event.svc_disc.end_handle);
            #endif
            
            break;
        }

        default:
            break;
    }
}


#if CFG_ENABLE_QUECTEL_BLE_PERIPHERA
/**************************************************************************************************
* Function  PERIPHERA
**************************************************************************************************/
static ql_task_t ble_periphera_hdl = NULL;
int ble_periphera_state = 0;

static QL_BLE_GATTS_PARAMS_T ql_ble_gatt_service = {0};
static QL_BLE_SERVICE_PARAMS_T ql_ble_common_service[QL_BLE_GATT_SERVICE_MAX_NUM] = {0};
static QL_BLE_CHAR_PARAMS_T ql_ble_common_char1[QL_BLE_GATT_CHAR_MAX_NUM] = {0};
/**************************************************************************************************
* Function: ql_ble_set_adv_data
*
* Author  : Lei.wang
*
* Date    : 2023/12/11 
*
* Description:
*	
* 
* Parameters:
*	none
*	
* Return:
*	none
*	
**************************************************************************************************/
static void ql_ble_set_adv_data(QL_BLE_DATA_T *pdata)
{
    uint16_t index = 0;
    uint8_t adv_mode[3] = {0x02, 0x01, 0x06};
    static uint8_t adv_data[31] = {0};

    //set adv mode --- flag
    memcpy(&adv_data[index], adv_mode, 3);
    index += 3;

    pdata->length = index;
    pdata->p_data = adv_data;
}

/**************************************************************************************************
* Function: ql_ble_set_scan_rsp_data
*
* Author  : Lei.wang
*
* Date    : 2023/12/11 
*
* Description:
*	
* 
* Parameters:
*	none
*	
* Return:
*	none
*	
**************************************************************************************************/
static void ql_ble_set_scan_rsp_data(QL_BLE_DATA_T *pdata)
{
	uint16_t index = 0;
    uint8_t ble_name[] = "fcm360w_ble";
	static uint8_t rsp_data[31] = {0};    
    
    //set device name
	rsp_data[index++] = strlen((char *)ble_name)+1;
	rsp_data[index++] = 0x09; /* Complete name */
	memcpy(&rsp_data[index], ble_name, strlen((char *)ble_name));
	index += strlen((char *)ble_name);

    pdata->length = index;
    pdata->p_data = rsp_data;
}

/**************************************************************************************************
* Function: ql_ble_server_init
*
* Author  : Lei.wang
*
* Date    : 2023/12/11 
*
* Description:
*	
* 
* Parameters:
*	none
*	
* Return:
*	none
*	
**************************************************************************************************/
void ql_ble_server_init()
{
    memset(ql_ble_common_service, 0x00, sizeof(QL_BLE_SERVICE_PARAMS_T) * QL_BLE_GATT_SERVICE_MAX_NUM);

    QL_BLE_GATTS_PARAMS_T *p_ble_service = &ql_ble_gatt_service;
    p_ble_service->svc_num = 1;
    p_ble_service->p_service = ql_ble_common_service;

    /*First service add*/
    QL_BLE_SERVICE_PARAMS_T *p_common_service = ql_ble_common_service;
    p_common_service->handle = QL_BLE_GATT_INVALID_HANDLE;
    p_common_service->svc_uuid.uuid_type = QL_BLE_UUID_TYPE_16;
    p_common_service->svc_uuid.uuid.uuid16 = QL_BLE_CMD_SERVICE_UUID;
    p_common_service->type = QL_BLE_UUID_SERVICE_PRIMARY;
    p_common_service->char_num = QL_COMMON_CHAR_MAX_NUM;
    p_common_service->p_char = ql_ble_common_char1;

    /*Add write characteristic*/
    QL_BLE_CHAR_PARAMS_T *p_common_char = ql_ble_common_char1;
    p_common_char->handle = QL_BLE_GATT_INVALID_HANDLE;
    p_common_char->char_uuid.uuid_type = QL_BLE_UUID_TYPE_16;
    p_common_char->char_uuid.uuid.uuid16 = QL_BLE_CMD_WRITE_NOTIFY_CHAR_UUID;

    p_common_char->property = QL_BLE_GATT_CHAR_PROP_WRITE | QL_BLE_GATT_CHAR_PROP_NOTIFY;
    p_common_char->permission = QL_BLE_GATT_PERM_READ | QL_BLE_GATT_PERM_WRITE;
    p_common_char->value_len = 512;
    p_common_char++;

    /*Add Notify characteristic*/
    p_common_char->handle = QL_BLE_GATT_INVALID_HANDLE;
    p_common_char->char_uuid.uuid_type = QL_BLE_UUID_TYPE_16;
    p_common_char->char_uuid.uuid.uuid16 = QL_BLE_CMD_WRITE_CHAR_UUID;

    p_common_char->property = QL_BLE_GATT_CHAR_PROP_NOTIFY | QL_BLE_GATT_CHAR_PROP_WRITE;
    p_common_char->permission = QL_BLE_GATT_PERM_READ | QL_BLE_GATT_PERM_WRITE;
    p_common_char->value_len = 512;
    p_common_char++;

    /*Add Read && write characteristic*/
    p_common_char->handle = QL_BLE_GATT_INVALID_HANDLE;
    p_common_char->char_uuid.uuid_type = QL_BLE_UUID_TYPE_16;
    p_common_char->char_uuid.uuid.uuid16 = QL_BLE_CMD_READ_CHAR_UUID;

    p_common_char->property = QL_BLE_GATT_CHAR_PROP_READ | QL_BLE_GATT_CHAR_PROP_WRITE;
    p_common_char->permission = QL_BLE_GATT_PERM_READ | QL_BLE_GATT_PERM_WRITE;
    p_common_char->value_len = 512;

    ql_ble_gatts_service_add(p_ble_service, false);
}

/**************************************************************************************************
* Function: ql_ble_periphera_demo_entry
*
* Author  : Lei.wang
*
* Date    : 2023/12/11 
*
* Description:
*	
* 
* Parameters:
*	none
*	
* Return:
*	none
*	
**************************************************************************************************/
void ql_ble_periphera_demo_entry(void *arg)
{    
    while (1)
    {
        if (ble_periphera_state == 0)
        {
            ql_ble_server_init();
            
            QL_BLE_GAP_ADV_PARAMS_T adv_param = {0};

            adv_param.adv_interval_max = 160;
            adv_param.adv_interval_min = 160;
            adv_param.adv_channel_map = 0x07;
            adv_param.adv_type = QL_BLE_GAP_ADV_TYPE_CONN_SCANNABLE_UNDIRECTED;
            
            /* Set Bluetooth broadcast data  */
            QL_BLE_DATA_T  padv_data = {0};
            QL_BLE_DATA_T  prsp_data = {0};
            
            ql_ble_set_adv_data(&padv_data);
            ql_ble_set_scan_rsp_data(&prsp_data);
            ql_ble_adv_param_set(&padv_data, &prsp_data);

            /* adv */
            if (ql_ble_gap_adv_start(&adv_param) != QL_BT_ERROR_NO_ERROR)
            {
                os_printf(LM_CMD, LL_INFO, "adv start fail\r\n");
            }
            
            ble_periphera_state = 1;
        }

        
        ql_rtos_task_sleep_ms(20);
    }
}

/**************************************************************************************************
* Function: ql_ble_demo_periphera_thread_creat
*
* Author  : Lei.wang
*
* Date    : 2023/12/11 
*
* Description:
*	
* 
* Parameters:
*	none
*	
* Return:
*	none
*	
**************************************************************************************************/
void ql_ble_demo_periphera_thread_creat(void)
{
    int ret;
    // todo  create new thread;
    ret = ql_rtos_task_create(&ble_periphera_hdl,
                              8 * 1024,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "ble_per_demo",
                              ql_ble_periphera_demo_entry,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        return;
    }

    return;
}

#endif


#if CFG_ENABLE_QUECTEL_BLE_CENTRAL
/**************************************************************************************************
* Function  CENTRAL
**************************************************************************************************/
static ql_task_t ble_central_hdl = NULL;

/*! Definition of an AD Structure as contained in Advertising and Scan Response packets. An Advertising or Scan Response packet contains several AD Structures. */
typedef struct gapAdStructure_tag
{
    uint8_t length; /*!< Total length of the [adType + aData] fields. Equal to 1 + lengthOf(aData). */
    uint8_t adType; /*!< AD Type of this AD Structure. */
    uint8_t *aData; /*!< Data contained in this AD Structure; length of this array is equal to (gapAdStructure_t.length - 1). */
} gapAdStructure_t;
/**************************************************************************************************
* Function: ql_ble_central_demo_entry
*
* Author  : Lei.wang
*
* Date    : 2023/12/11 
*
* Description:
*	
* 
* Parameters:
*	none
*	
* Return:
*	none
*	
**************************************************************************************************/
void ql_ble_central_demo_entry(void *param)
{    
    while (1)
    {
        if (ble_demo_state & QL_BLE_DEMO_START_SCAN)
        {
            QL_BLE_GAP_SCAN_PARAMS_T QL_BLE_GAP_SCAN_PARAMS = {0};

            ble_demo_state &= ~QL_BLE_DEMO_START_SCAN;

            QL_BLE_GAP_SCAN_PARAMS.scan_type = QL_SCAN_TYPE_CONN_DISC;
            QL_BLE_GAP_SCAN_PARAMS.dup_filt_pol = QL_DUP_FILT_DIS;
            QL_BLE_GAP_SCAN_PARAMS.scan_prop = QL_BLE_SCAN_PROP_PHY_1M_BIT | QL_BLE_SCAN_PROP_ACTIVE_1M_BIT;

            QL_BLE_GAP_SCAN_PARAMS.interval = 160;
            QL_BLE_GAP_SCAN_PARAMS.window = 48;
            QL_BLE_GAP_SCAN_PARAMS.duration = 0;
            QL_BLE_GAP_SCAN_PARAMS.period = 0;
            QL_BLE_GAP_SCAN_PARAMS.scan_channel_map = 0x07;
            ql_ble_gap_scan_start(&QL_BLE_GAP_SCAN_PARAMS);
        }
        
        if (ble_demo_state & QL_BLE_DEMO_START_CONN)
        {
            ble_demo_state &= ~QL_BLE_DEMO_START_CONN;
            
            ql_ble_gap_scan_stop();
            
            QL_BLE_GAP_CONN_PARAMS_T cen_p_conn_params = {0};
            QL_BLE_GAP_ADDR_T conn_peer_addr = {0};
            
            cen_p_conn_params.conn_interval_min = 20;
            cen_p_conn_params.conn_interval_max = 20;
            cen_p_conn_params.conn_latency = 0;
            cen_p_conn_params.conn_sup_timeout = 0x01f4;
            cen_p_conn_params.conn_establish_timeout = 1000;
            
            conn_peer_addr.type = demo_peer_addr_type;
            memcpy(conn_peer_addr.addr, demo_peer_addr, 6);
            
            if (ql_ble_gap_connect(&conn_peer_addr, &cen_p_conn_params) == QL_BT_ERROR_NO_ERROR)
            {
                os_printf(LM_CMD, LL_INFO, "connect succ\r\n");
            }
            else
            {
                os_printf(LM_CMD, LL_INFO, "connect fail\r\n");
            }
        }

        if(ble_demo_state & QL_BLE_DEMO_GATTC_READ)
        {
            ql_rtos_task_sleep_ms(10);
            ql_debug("QL_BLE_DEMO_GATTC_READ conn_handle: [%d], rd_chara_hdl_demo: [%d] \r\n", conn_handle, rd_chara_hdl_demo);
            ble_demo_state &= ~QL_BLE_DEMO_GATTC_READ;
            ql_ble_gattc_read(conn_handle, rd_chara_hdl_demo);  
        }

        if(ble_demo_state & QL_BLE_DEMO_GATTC_SET_CHARA_NTF)
        {
            ql_debug("QL_BLE_DEMO_GATTC_SET_CHARA_NTF conn_handle: [%d], wr_chara_hdl_demo: [%d] \r\n", conn_handle, wr_chara_hdl_demo);
            ble_demo_state &= ~QL_BLE_DEMO_GATTC_SET_CHARA_NTF;
            ql_ble_gattc_set_characteristic_notification(conn_handle, wr_chara_hdl_demo+1, QL_BLE_GATT_NOTIFICATION);
            ble_demo_state |= QL_BLE_DEMO_GATTC_WRITE;
        }

        if(ble_demo_state & QL_BLE_DEMO_GATTC_WRITE)
        {
            ql_debug("conn_handle: [%d], wr_chara_hdl_demo: [%d] \r\n", conn_handle, wr_chara_hdl_demo);
            ble_demo_state &= ~QL_BLE_DEMO_GATTC_WRITE;
            ql_ble_gattc_write(conn_handle, wr_chara_hdl_demo, (uint8_t *)"12345678", 8);
        }

        ql_rtos_task_sleep_ms(20);
    }
}

/**************************************************************************************************
* Function: ql_ble_demo_central_thread_creat
*
* Author  : Lei.wang
*
* Date    : 2023/12/11 
*
* Description:
*	
* 
* Parameters:
*	none
*	
* Return:
*	none
*	
**************************************************************************************************/
void ql_ble_demo_central_thread_creat(void)
{
    int ret;
    // todo  create new thread;
    ret = ql_rtos_task_create(&ble_central_hdl,
                              8 * 1024,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "ble_cen_demo",
                              ql_ble_central_demo_entry,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        return;
    }

    return;
}
#endif

/**************************************************************************************************
* Function: ql_ble_demo_thread_creat
*
* Author  : Lei.wang
*
* Date    : 2023/12/11 
*
* Description:
*	
* 
* Parameters:
*	none
*	
* Return:
*	none
*	
**************************************************************************************************/
void ql_ble_demo_thread_creat(void)
{
    unsigned char ble_mac[6] = {0};
    ql_ble_get_mac_addr((char *)ble_mac);

    ql_debug("ble mac: [%02x:%02x:%02x:%02x:%02x:%02x] \r\n", 
        ble_mac[0], ble_mac[1], ble_mac[2], ble_mac[3], ble_mac[4], ble_mac[5]);

    /* set_dev_config */
    QL_BLE_DEV_CONFIG dev_config = {0};
    
    #if CFG_ENABLE_QUECTEL_BLE_PERIPHERA
    uint8_t periphera_demo_name[] = "FCM360W";
    
    dev_config.role = QL_BLE_ROLE_PERIPHERAL;
    ql_ble_set_device_name((uint8_t *)periphera_demo_name, strlen((char *)periphera_demo_name));
    
    #elif CFG_ENABLE_QUECTEL_BLE_CENTRAL
    dev_config.role = QL_BLE_ROLE_MASTER;
    #endif
    
    dev_config.renew_dur = QL_BLE_TMR_PRIV_ADDR_MIN;
    dev_config.privacy_cfg = QL_BLE_PRIV_CFG_PRIV_ADDR_POS;
    ql_ble_set_dev_config(&dev_config);
    
    /* set ble event callback */
    ql_ble_gap_callback_register(ql_ble_gap_event_cb);
    ql_ble_gatt_callback_register(ql_ble_gatt_event_cb);
    ql_ble_reset_all();

    #if CFG_ENABLE_QUECTEL_BLE_PERIPHERA
    ql_ble_demo_periphera_thread_creat();
    #endif
    
    #if CFG_ENABLE_QUECTEL_BLE_CENTRAL
    ql_ble_demo_central_thread_creat();
    #endif
}

