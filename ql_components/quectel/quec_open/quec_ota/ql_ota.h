#ifndef _QL_OTA_H_
#define _QL_OTA_H_
#include "ql_type.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/def.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define QL_OTA_DEFAULT_BUFF_LEN    256
#define QL_OTA_VERSION_NUM         6

#define CONFIG_LOCAL_OTA_CONNET_TIMEOUT  20


#define QL_LOCAL_OTA_RECV_PORT         5300
#define QL_LOCAL_OTA_SEND_PORT         5301

#define QL_LOCAL_OTA_DEFAULT_BUFF_LEN  	256
#define QL_LOCAL_OTA_VERSION_NUM       	6
#define QL_LOCAL_OTA_VERSION_GREATER   	1
#define QL_LOCAL_OTA_VERSION_LESS      	2
#define QL_LOCAL_OTA_REPORT_CNT        	5
typedef void (*ql_ota_cb)(void);
typedef struct
{
	ql_ota_cb ota_success_cb;
	ql_ota_cb ota_fail_cb;
	ql_ota_cb ota_action_cb;
} ql_ota_cb_t;

typedef enum
{
	QL_OTA_PACKET_BROADCAST = 0x01,
	QL_OTA_PACKET_REPORT    = 0x10,
	QL_OTA_PACKET_ACTION    = 0x11,
} ql_ota_packet_type;
typedef enum
{
	QL_OTA_ACTION_SUCESS,
	QL_OTA_ACTION_UPDATE,
	QL_OTA_ACTION_ILLEGAL,
	QL_OTA_ACTION_FAIL,
} ql_ota_action;
typedef struct
{
	unsigned char version[QL_OTA_VERSION_NUM];
	char path[QL_OTA_DEFAULT_BUFF_LEN];
	unsigned char src_ip[4];
	char dst_ip[16];
	unsigned char mac[6];
	struct sockaddr_in client_addr;
} ql_ota_info_t;
typedef struct
{
	unsigned char type;
	unsigned char len;
	char url[QL_OTA_DEFAULT_BUFF_LEN];
	unsigned char srcver[QL_OTA_VERSION_NUM];
	unsigned char destver[QL_OTA_VERSION_NUM];
	unsigned int fm_len;
	unsigned int fm_crc;
} ql_ota_recv_pack_t;

int ql_ota_init(void);
int ql_ota_write(unsigned char *data, unsigned int len);
int ql_ota_done(unsigned char  reset);
int ql_ota_confirm_update(void);
int ql_ota_get_flash_crc(unsigned int addr, unsigned int size);
int ql_ota_firmware_check(unsigned int data_len, unsigned int crc);


//http
void ql_http_ota_download(const char *uri);
int ql_local_ota_version_register(const char *version);
ql_ota_info_t *ql_get_local_ota_info(void);
int ql_local_ota_socket_init(void);
int ql_local_ota_message_send(ql_ota_packet_type type, ql_ota_action action);
int ql_local_ota_start(void);
#ifdef __cplusplus
} /*"C" */
#endif

#endif