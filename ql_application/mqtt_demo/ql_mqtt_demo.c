/**  @file
  ql_mqtt_demo.c

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
#include "ql_type.h"
#include "ql_debug.h"
#include "ql_api_osi.h"
#include "mqtt_client.h"
#include "system_wifi.h"
#include "ql_wlan.h"


#define WIFI_SSID   "zxcvbnm"
#define WIFI_KEY    "12345678"

#define MQTT_CLIENT_URI             "mqtt://220.173.137.202"
#define MQTT_CLIENT_PORT            65153
#define MQTT_CLIENT_IDENTITY        "quectel_01"
#define MQTT_CLIENT_USER            ""
#define MQTT_CLIENT_PASS            ""

#define IS_ASCII(c) (c > 0x1F && c < 0x7F)

//#ifndef MIN
//#define MIN(x,y) (x < y ? x : y)
//#endif

#define QL_CLIENT_ID_STRINQL_MAX_LEN 256//defined in Section 1.5.3
#define QL_PRINT_STRINQL_MAX_LEN (CLI_PRINT_BUFFER_SIZE - 32) 


#define QL_QOS_PROMPT "qos error! please use qos xx(0 or 1 or 2)\r\n"
#define QL_CLEAN_SESSION_PROMPT "clean_session err! please use clean_session xx(0 or 1)\r\n"
#define QL_URL_PROMPT "url err! please use mqtt_start url xx\r\n"
#define QL_RETAIN_PROMPT "retain error! please use retain xx(0 or 1)\r\n"
#define QL_CLIENT_ID_TOO_LONQL_PROMPT "The length of ClientID must be between 1 and 23 bytes!\r\n"

/****************************************************************************
* 	                                           Local Types
****************************************************************************/

/****************************************************************************
* 	                                           Local Constants
****************************************************************************/

/****************************************************************************
* 	                                           Local Function Prototypes
****************************************************************************/

/****************************************************************************
* 	                                          Global Constants
****************************************************************************/

/****************************************************************************
* 	                                          Global Variables
****************************************************************************/

/****************************************************************************
* 	                                          Global Function Prototypes
****************************************************************************/

/****************************************************************************
* 	                                          Function Definitions
****************************************************************************/


trs_mqtt_client_handle_t ql_mqtt_client = NULL;
trs_mqtt_client_config_t ql_mqtt_cfg = {0};

int QL_MQTT_EVENT_CONNECTED = 0;
int QL_MQTT_EVENT_DISCONNECTED = 0;
int QL_MQTT_EVENT_SUBSCRIBED = 0;
int QL_MQTT_EVENT_UNSUBSCRIBED = 0;
int QL_MQTT_EVENT_PUBLISHED = 0;
int QL_MQTT_EVENT_DATA = 0;
int QL_MQTT_EVENT_ERROR = 0;

int QL_MQTT_PINQL_SEND_OK = 0;
int QL_MQTT_PINQL_SEND_NO = 0;
int QL_MQTT_PINQL_OK = 0;
int QL_MQTT_PINQL_NO = 0;
int QL_mqtt = 0;

void ql_mqtt_time(void *env)
{
	os_printf(LM_APP, LL_INFO, "MQTT PING\r\n\tSEND_OK:%d\r\n\tSEND_NO:%d\r\n\tOK:%d\r\n\tNO:%d\r\n", QL_MQTT_PINQL_SEND_OK, QL_MQTT_PINQL_SEND_NO, QL_MQTT_PINQL_OK, QL_MQTT_PINQL_NO);
	os_printf(LM_APP, LL_INFO, "MQTT EVENT\r\n\tCON:%d\r\n\tDISCON:%d\r\n\tSUBSCRIBED:%d\r\n\tUNSUBSCRIBED:%d\r\n\tPUBLISHED:%d\r\n\tD:%d\r\n\tE:%d\r\n", 
		QL_MQTT_EVENT_CONNECTED, QL_MQTT_EVENT_DISCONNECTED, QL_MQTT_EVENT_SUBSCRIBED, QL_MQTT_EVENT_UNSUBSCRIBED, 
		QL_MQTT_EVENT_PUBLISHED, QL_MQTT_EVENT_DATA, QL_MQTT_EVENT_ERROR);
}

os_timer_handle_t ql_mqtt_timer_handle = NULL;
extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[]   asm("_binary_server_root_cert_pem_end");
static int default_mqtt_handle(trs_mqtt_event_handle_t event)
{
    //trs_mqtt_client_handle_t client = event->client;
    //int msQL_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
			if(QL_mqtt == 0)
			{
				QL_MQTT_EVENT_CONNECTED += 1;
				QL_mqtt = 1;
			}
            os_printf(LM_APP, LL_INFO, "MQTT_EVENT_CONNECTED:%d\n", QL_MQTT_EVENT_CONNECTED);
            break;
			
        case MQTT_EVENT_DISCONNECTED:
			if(QL_mqtt == 1)
			{
				QL_MQTT_EVENT_DISCONNECTED += 1;
				QL_mqtt = 0;
			}
            os_printf(LM_APP, LL_INFO, "MQTT_EVENT_DISCONNECTED:%d\n", QL_MQTT_EVENT_DISCONNECTED);
			
            break;

        case MQTT_EVENT_SUBSCRIBED:
			QL_MQTT_EVENT_SUBSCRIBED++;
            os_printf(LM_APP, LL_INFO, "MQTT_EVENT_SUBSCRIBED:%d, msQL_id=%d\n", QL_MQTT_EVENT_SUBSCRIBED, event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
			QL_MQTT_EVENT_UNSUBSCRIBED++;
            os_printf(LM_APP, LL_INFO, "MQTT_EVENT_UNSUBSCRIBED:%d, msQL_id=%d\n", QL_MQTT_EVENT_UNSUBSCRIBED, event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
			QL_MQTT_EVENT_PUBLISHED++;
            os_printf(LM_APP, LL_INFO, "MQTT_EVENT_PUBLISHED:%d, msQL_id=%d\n", QL_MQTT_EVENT_PUBLISHED, event->msg_id);
            break;
        case MQTT_EVENT_DATA:
			QL_MQTT_EVENT_DATA++;
            os_printf(LM_APP, LL_INFO, "MQTT_EVENT_DATA:%d\n", QL_MQTT_EVENT_DATA);
            os_printf(LM_APP, LL_INFO, "TOPIC_len=%d, DATA_len= %d\r\n", event->topic_len, event->data_len);
            if (event->topic_len < QL_PRINT_STRINQL_MAX_LEN)
            {
                os_printf(LM_APP, LL_INFO, "TOPIC=%.*s\r\n", event->topic_len, event->topic);
            }
            else
            {
                os_printf(LM_APP, LL_INFO, "The length of TOPIC exceeds the length of the print buffer, so do't printf!\r\n");
            }

            if (event->data_len < QL_PRINT_STRINQL_MAX_LEN)
            {
                if (event->data_len == 0)
                {
                    os_printf(LM_APP, LL_INFO, "DATA is null!\r\n");
                }
                else
                {
                    os_printf(LM_APP, LL_INFO, "DATA=%.*s\r\n", event->data_len, event->data);
                }
            }
            else
            {
                os_printf(LM_APP, LL_INFO, "The length of DATA exceeds the length of the print buffer, so do't printf!\r\n");
            }
            
            break;
        case MQTT_EVENT_ERROR:
			QL_MQTT_EVENT_ERROR++;
            os_printf(LM_APP, LL_INFO, "MQTT_EVENT_ERROR:%d\n", QL_MQTT_EVENT_ERROR);
            break;
    }
    return 0;
}


int ql_cmd_mqtt_start(char* uri, uint32_t port, char *client_id, char *username, char *password, int clean_session, int keepalive)
{
    //uint8_t i;
    memset(&ql_mqtt_cfg, 0, sizeof(trs_mqtt_client_config_t));
    ql_mqtt_cfg.event_handle = default_mqtt_handle;

#if 0
    ql_mqtt_cfg.cert_pem = (const char *)server_root_cert_pem_start;
#endif

    if(strncasecmp("mqtts://", uri, 8) == 0)
    {
        ql_mqtt_cfg.transport = MQTT_TRANSPORT_OVER_SSL;
        ql_mqtt_cfg.uri = uri;
    }
    else if(strncasecmp("mqtt://", uri, 7) == 0)
    {
        ql_mqtt_cfg.transport = MQTT_TRANSPORT_OVER_TCP;
        ql_mqtt_cfg.uri = uri;
    }
    else
    {
        ql_mqtt_cfg.transport = MQTT_TRANSPORT_UNKNOWN;
        os_printf(LM_APP, LL_ERR, "ERROR MQTT_TRANSPORT_UNKNOWN\r\n");
        return CMD_RET_FAILURE;
    }

    if(port >= 0 && port < 65535)
    {
        ql_mqtt_cfg.port = port;
    }
    else
    {
        os_printf(LM_APP, LL_ERR,"mqtt_start parameters port is wrong! \r\n");
        return CMD_RET_FAILURE;
    }

    ql_mqtt_cfg.client_id = client_id;
    ql_mqtt_cfg.username = username;
    ql_mqtt_cfg.password = password;

    if(clean_session < 0 || clean_session > 1)
    {
        os_printf(LM_CMD, LL_INFO, QL_CLEAN_SESSION_PROMPT);
        return CMD_RET_FAILURE;
    }
    else
    {
        ql_mqtt_cfg.clean_session = clean_session;
    }

    if(keepalive >= 0 && keepalive <= 7200)
    {
        ql_mqtt_cfg.keepalive = keepalive;
    }
    else
    {
        os_printf(LM_APP, LL_ERR,"mqtt_start parameters keepalive is wrong! \r\n");
        return CMD_RET_FAILURE;
    }

    if (ql_mqtt_client != NULL)
    {
        os_printf(LM_CMD, LL_INFO, "mqtt has started!\r\n");
        return CMD_RET_SUCCESS;
    }

    if (ql_mqtt_cfg.uri == NULL)  // must set url
    {
        os_printf(LM_CMD, LL_INFO, QL_URL_PROMPT);
        return CMD_RET_FAILURE;
    }

    int client_id_len = strlen(ql_mqtt_cfg.client_id);
    if (client_id_len > QL_CLIENT_ID_STRINQL_MAX_LEN)
    {
        os_printf(LM_CMD, LL_INFO, QL_CLIENT_ID_TOO_LONQL_PROMPT);
        return CMD_RET_FAILURE;
    }

    os_printf(LM_APP, LL_INFO, "In mqtt_start(),ql_mqtt_cfg.url = %s\r\n",ql_mqtt_cfg.uri);
    ql_mqtt_client = trs_mqtt_client_init(&ql_mqtt_cfg);
    if(ql_mqtt_client == NULL){
        os_printf(LM_APP, LL_INFO, "mqtt init failed\n");
        return CMD_RET_FAILURE;
    }
    if (0 != trs_mqtt_client_start(ql_mqtt_client)){
        os_printf(LM_APP, LL_INFO, "mqtt start failed\r\n");
        trs_mqtt_client_destroy(ql_mqtt_client);
        ql_mqtt_client = NULL;
        return CMD_RET_FAILURE;
    }
	os_printf(LM_APP, LL_INFO, "mqtt start done\n");

	if(ql_mqtt_timer_handle == NULL)
	{
		os_printf(LM_CMD, LL_INFO, "* os_timer_create mqtt timer *\n");
		extern void ql_mqtt_time(void *env);
		ql_mqtt_timer_handle = os_timer_create("ql_mqtt_time", 60000, 1, ql_mqtt_time, NULL);
		os_timer_start(ql_mqtt_timer_handle);
	}
    return CMD_RET_SUCCESS;
}


int ql_cmd_mqtt_stop(void)
{
    if (ql_mqtt_client == NULL)
    {
        os_printf(LM_APP, LL_INFO, "not start mqtt\n");
        return CMD_RET_FAILURE;
    }
    MQTT_CFG_MSG_ST msg_send = {0};
    msg_send.cfg_type = CFG_MSG_TYPE_STOP;
    os_queue_send(ql_mqtt_client->cfg_queue_handle, (char *)&msg_send, sizeof(msg_send), 0);
    ql_mqtt_client = NULL;

    if(ql_mqtt_timer_handle != NULL)
    {
        int ret = os_timer_delete(ql_mqtt_timer_handle);
        if (ret == 0)
        {
            os_printf(LM_CMD, LL_INFO, "* delete mqtt timer success *\n");
        }
        else
        {
            os_printf(LM_CMD, LL_INFO, "* delete mqtt timer failed *\n");
        }
        ql_mqtt_timer_handle = NULL;
    }

    return CMD_RET_SUCCESS;
}


int ql_cmd_mqtt_sub(char *topic, int qos)
{
    //int i = 0;
    //char * topic = NULL;
    //int qos = 0;
    if (ql_mqtt_client == NULL)
    {
        os_printf(LM_APP, LL_INFO, "not start mqtt\n");
        return CMD_RET_FAILURE;
    }

    int topic_len = strlen(topic);
    if ((topic_len <= 0) || (topic_len >= QL_PRINT_STRINQL_MAX_LEN))
    {
        os_printf(LM_APP, LL_INFO, "topic error\n");
        return CMD_RET_FAILURE;
    }

    if ((qos < MQTT_QOS0) || (qos > MQTT_QOS2))
    {
        os_printf(LM_CMD, LL_INFO, QL_QOS_PROMPT);
        return CMD_RET_FAILURE;
    }

    MQTT_CFG_MSG_ST msg_send = {0};
    set_mqtt_msg(SET_TOPIC, &msg_send, topic, topic_len);
    msg_send.cfg_type = CFG_MSG_TYPE_SUB;
    msg_send.qos = qos;
    if (ql_mqtt_client->cfg_queue_handle != NULL)
    {
        os_queue_send(ql_mqtt_client->cfg_queue_handle, (char *)&msg_send, sizeof(msg_send), 0);
    }
    else
    {
        os_printf(LM_APP, LL_INFO, "ql_mqtt_client->cfg_queue_handle is null! \r\n");
        return CMD_RET_FAILURE;
    }

    return CMD_RET_SUCCESS;
}


int ql_cmd_mqtt_unsub(char *topic)
{
    //int i = 0;
    //char * topic = NULL;

    if (ql_mqtt_client == NULL)
    {
        os_printf(LM_APP, LL_INFO, "not start mqtt\n");
        return CMD_RET_FAILURE;
    }

    int topic_len = strlen(topic);
    if ((topic_len <= 0) || (topic_len >= QL_PRINT_STRINQL_MAX_LEN))
    {
        os_printf(LM_APP, LL_INFO, "topic error\n");
        return CMD_RET_FAILURE;
    }

    MQTT_CFG_MSG_ST msg_send = {0};
    set_mqtt_msg(SET_TOPIC, &msg_send, topic, topic_len);
    msg_send.cfg_type = CFG_MSG_TYPE_UNSUB;
    if (ql_mqtt_client->cfg_queue_handle != NULL)
    {
        os_queue_send(ql_mqtt_client->cfg_queue_handle, (char *)&msg_send, sizeof(msg_send), 0);
    }
    else
    {
        os_printf(LM_APP, LL_INFO, "ql_mqtt_client->cfg_queue_handle is null! \r\n");
        return CMD_RET_FAILURE;
    }

    return CMD_RET_SUCCESS;
}


int ql_cmd_mqtt_pub(char *topic, char *data, int qos, int retain)
{
    //char *topic = NULL;
    //char *data = NULL;
    //int qos = 0;
    //int retain = 0;
    //int i =0;
    if (ql_mqtt_client == NULL)
    {
        os_printf(LM_APP, LL_INFO, "not start mqtt\n");
        return CMD_RET_FAILURE;
    }


    int topic_len = strlen(topic);
    if ((topic_len <= 0) || (topic_len >= QL_PRINT_STRINQL_MAX_LEN))
    {
        os_printf(LM_APP, LL_INFO, "topic long is error\r\n");
        return CMD_RET_FAILURE;
    }

    int data_len = strlen(data);
    if (data_len < 0 || data_len >= QL_PRINT_STRINQL_MAX_LEN)
    {
        os_printf(LM_APP, LL_INFO, "data long is error\r\n");
        return CMD_RET_FAILURE;
    }

    if ((qos < MQTT_QOS0) || (qos > MQTT_QOS2))
    {
        os_printf(LM_CMD, LL_INFO, QL_QOS_PROMPT);
        return CMD_RET_FAILURE;
    }

    if ((retain != 0) && (retain != 1))
    {
        os_printf(LM_CMD, LL_INFO, QL_RETAIN_PROMPT);
        return CMD_RET_FAILURE;
    }

    MQTT_CFG_MSG_ST msg_send = {0};
    set_mqtt_msg(SET_TOPIC, &msg_send, topic, topic_len);

    if (data_len != 0)
    {
        set_mqtt_msg(SET_DATA, &msg_send, data, data_len);
    }

    msg_send.qos = qos;
    msg_send.retain = retain;
    msg_send.cfg_type = CFG_MSG_TYPE_PUB;
    if (ql_mqtt_client->cfg_queue_handle != NULL)
    {
        os_queue_send(ql_mqtt_client->cfg_queue_handle, (char *)&msg_send, sizeof(msg_send), 0);
    }
    else
    {
        os_printf(LM_CMD, LL_INFO, "ql_mqtt_client->cfg_queue_handle is null! \r\n");
        return CMD_RET_FAILURE;
    }

    return CMD_RET_SUCCESS;
}



ql_task_t mqtt_thread_handle = NULL;



void ql_mqtt_demo_thread(void *param)
{
    ql_rtos_task_sleep_ms(1000);
    os_printf(LM_APP, LL_INFO, "mqtt demo start\r\n");
    int ret = 0;
    ql_network_init_s wificonfig;
    memset(&wificonfig, 0, sizeof(wificonfig));
    wificonfig.wifi_mode = QL_STATION;
    memcpy(&wificonfig.wifi_ssid, WIFI_SSID, sizeof(WIFI_SSID));
    memcpy(&wificonfig.wifi_key, WIFI_KEY, sizeof(WIFI_KEY));
    wificonfig.dhcp_mode = 1;
    os_printf(LM_APP, LL_INFO, "wlan start  %s  %s\r\n",wificonfig.wifi_ssid,wificonfig.wifi_key);
    ret = ql_wlan_start(&wificonfig);
    if(ret != QL_WIFI_SUCCESS)
    {
        os_printf(LM_APP, LL_INFO, "wlan start failed %d\r\n",ret);
        goto exit;
    }
    ql_rtos_task_sleep_ms(5000);

    ret = ql_cmd_mqtt_start(MQTT_CLIENT_URI, MQTT_CLIENT_PORT, MQTT_CLIENT_IDENTITY, MQTT_CLIENT_USER, MQTT_CLIENT_PASS, 1, 120);
    if(ret != CMD_RET_SUCCESS)
    {
        goto exit;
    }
    ql_rtos_task_sleep_ms(2000);

    ret = ql_cmd_mqtt_sub("MQTT-TEST", 1);
    if(ret != CMD_RET_SUCCESS)
    {
        goto exit;
    }
    ql_rtos_task_sleep_ms(2000);

    ret = ql_cmd_mqtt_pub("MQTT-TEST", "hi, mqtt qos 1", 1, 0);
    if(ret != CMD_RET_SUCCESS)
    {
        goto exit;
    }
    ql_rtos_task_sleep_ms(2000);

    ret = ql_cmd_mqtt_unsub("MQTT-TEST");
    if(ret != CMD_RET_SUCCESS)
    {
        goto exit;
    }
    ql_rtos_task_sleep_ms(2000);

    ql_cmd_mqtt_stop();

exit:
    os_printf(LM_APP, LL_INFO, "mqtt thread exit\r\n");
    ql_rtos_task_delete(mqtt_thread_handle);
}


void ql_mqtt_demo_thread_creat(void)
{
    int ret;
    ret = ql_rtos_task_create(&mqtt_thread_handle,
                              (unsigned short)3*1024,
                              RTOS_HIGHER_PRIORTIY_THAN(6),
                              "mqtt_demo",
                              ql_mqtt_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO, "Error: Failed to create mqtt thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (mqtt_thread_handle != NULL)
    {
        ql_rtos_task_delete(mqtt_thread_handle);
    }
}

