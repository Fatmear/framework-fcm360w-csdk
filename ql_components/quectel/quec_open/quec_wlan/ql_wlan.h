#ifndef __QL_WLAN_H_
#define __QL_WLAN_H_
/**************************************************************************************************
* include
**************************************************************************************************/
#include "lwip/dns.h"
#include "lwip/nd6.h"
#include <lwip/sockets.h>
#include "ql_api_osi.h"
/**************************************************************************************************
* define
**************************************************************************************************/
#define QL_WIFI_SSID_MAX_LEN   (32 + 1)
#define QL_WIFI_PWD_MAX_LEN    (64) // pwd string support max length is 63

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x "
#endif

#if LWIP_IPV4 && LWIP_IPV6
#define LWIP_IPV4_V6_ON 
#else
#define LWIP_IPV4_ONLY
#endif

#define QL_WIFI_PROMIS_FILTER_MASK_ALL         (0xFFFFFFFF)  /**< filter all packets */
#define QL_WIFI_PROMIS_FILTER_MASK_MGMT        (1)           /**< filter the packets with type of WIFI_PKT_MGMT */
#define QL_WIFI_PROMIS_FILTER_MASK_CTRL        (1<<1)        /**< filter the packets with type of WIFI_PKT_CTRL */
#define QL_WIFI_PROMIS_FILTER_MASK_DATA        (1<<2)        /**< filter the packets with type of WIFI_PKT_DATA */
#define QL_WIFI_PROMIS_FILTER_MASK_DATA_MPDU   (1<<3)        /**< filter the MPDU which is a kind of WIFI_PKT_DATA */
#define QL_WIFI_PROMIS_FILTER_MASK_DATA_AMPDU  (1<<4)        /**< Todo: filter the AMPDU which is a kind of WIFI_PKT_DATA */
/**************************************************************************************************
* typedef enum
**************************************************************************************************/
typedef int32_t ql_sys_err_t;

typedef enum {
  QL_IP_TYPE_INVALID = 0x00,
  /** IPv4 */
  QL_IP_TYPE_V4 =   0x01,
  /** IPv6 */
  QL_IP_TYPE_V6 =   0x02,
  /** IPv4+IPv6 ("dual-stack") */
  QL_IP_TYPE_ANY =  0x03
}QlIpType_e;

//typedef enum
//{
//    ql_station = 1, /**< Act as a station which can connect to an access point*/
//    ql_softap = 0,  /**< Act as an access point, and other station can connect, 8 stations Max*/
//    ql_ap_softap = 2,
//} QlWlanWorkMode_e;



typedef enum {
    // ap status.
    QL_AP_STATUS_STOP,
    QL_AP_STATUS_STARTED,
    
    // sta status.
    QL_STA_STATUS_STOP,
    QL_STA_STATUS_START,
    QL_STA_STATUS_DISCON,
    QL_STA_STATUS_CONNECTED,

    //error status
    QL_STATUS_ERROR
} ql_wifi_status_e;
#ifdef LWIP_IPV4_ONLY
struct _ip4_addr {
  uint32 addr;
};
typedef struct _ip4_addr ql_ip_addr_t;
#endif

#ifdef  LWIP_IPV4_V6_ON
struct _ip4_addr {
  uint32 addr;
};
typedef struct _ip4_addr ql_ip4_addr_t;

struct _ip6_addr {
  uint32 addr[4];
#if LWIP_IPV6_SCOPES
  uint8_t zone;
#endif /* LWIP_IPV6_SCOPES */
};

/** IPv6 address */
typedef struct _ip6_addr ql_ip6_addr_t;
typedef struct _ip_addr {
  union {
    ql_ip6_addr_t ip6;
    ql_ip4_addr_t ip4;
  } u_addr;
  /** @ref lwip_ip_addr_type */
  uint8_t type;
} ql_ip_addr_t;
#endif
/****************************************************************************
* 	                                        Types
****************************************************************************/



typedef struct _ip_info {
    ql_ip_addr_t ip;      /**< IP address */
    ql_ip_addr_t netmask; /**< netmask */
    ql_ip_addr_t gw;      /**< gateway */
    ql_ip_addr_t dns1;
    ql_ip_addr_t dns2;
} ql_ip_info_t;




typedef struct
{
    uint8_t *ssid;     /* SSID of AP */
    uint8_t *bssid;    /* MAC address of AP */
    uint8_t channel;   /* channel, scan the specific channel */
    uint8_t passive;   /* passive slave or not */
    uint8_t max_item;  /* max scan item */
    int32_t scan_time; /* scan time per channel, units: millisecond */
} QlWlanScanConfigTypeDef_st;

typedef struct
{
    uint8_t ssid[QL_WIFI_SSID_MAX_LEN];
    uint8_t pwd[QL_WIFI_PWD_MAX_LEN];
    uint8_t ssid_len;
    uint8_t pwd_len;
    uint8_t auth;
    uint8_t cipher;
    uint8_t channel;
    uint8_t bssid[6];
    int8_t rssi;
} QlWifiInfoTypeDef_st;

typedef struct _linkStatus_t
{
    uint8_t conn_state;              /**< The link to wlan is established or not, 0: disconnected, 1: connected. */
    int8_t wifi_strength;           /**< Signal strength of the current connected AP */
    uint8_t ssid[QL_WIFI_SSID_MAX_LEN]; /**< SSID of the current connected wlan */
    uint8_t bssid[6];                /**< BSSID of the current connected wlan */
    uint8_t channel;                 /**< Channel of the current connected wlan */
} QlWlanLinkStatusTypeDef_st;

typedef struct
{
	uint8_t dhcp;       /**< DHCP mode: @ref DHCP_Disable,@ref DHCP_Enable*/
	unsigned char ip[4];    /**< Local IP address on the target wlan interface: @ref wlanInterfaceTypedef.*/
    unsigned char    gate[4];   /**< Router IP address on the target wlan interface: @ref wlanInterfaceTypedef.*/
    unsigned char    mask[4];   /**< Netmask on the target wlan interface: @ref wlanInterfaceTypedef.*/
    unsigned char    dns[4];   /**< DNS server IP address.*/
    unsigned char    mac[4];    /**< MAC address, example: "C89346112233".*/
} QlWlanIPStatusTypedef_st;

//================================================================================//

//enum
typedef enum
{
    CN = 0,
    JP,
    USA,
}ql_wifi_country_e;

/**
 *  @brief  Wi-Fi security type enumeration definition.
 */
typedef enum 
{
    QL_SECURITY_TYPE_NONE,        /**< Open system. */
    QL_SECURITY_TYPE_WEP,         /**< Wired Equivalent Privacy. WEP security. */
    QL_SECURITY_TYPE_WPA_TKIP,    /**< WPA /w TKIP */
    QL_SECURITY_TYPE_WPA_AES,     /**< WPA /w AES */
    QL_SECURITY_TYPE_WPA2_TKIP,   /**< WPA2 /w TKIP */
    QL_SECURITY_TYPE_WPA2_AES,    /**< WPA2 /w AES */
    QL_SECURITY_TYPE_WPA2_MIXED,  /**< WPA2 /w AES or TKIP */
    QL_SECURITY_TYPE_WPA3_SAE,	  /**< WPA3 SAE */
    QL_SECURITY_TYPE_WPA3_WPA2_MIXED, /** WPA3 SAE or WPA2 AES */
    QL_SECURITY_TYPE_EAP,
    QL_SECURITY_TYPE_OWE,
    QL_SECURITY_TYPE_AUTO,        /**< It is used when calling @ref bkWlanStartAdv, _BK_ read security type from scan result. */
}ql_wlan_sec_type_e;
//struct



/*
 *  sturcture of wifi event module
*/
typedef struct
{
    int mod;
    int event_id;
}ql_event_module_s;
typedef struct ql_link_status_t
{
    int conn_state;       /**< The link to wlan is established or not, 0: disconnected, 1: connected. */
    int wifi_strength;      /**< Signal strength of the current connected AP */
    uint8_t  ssid[32];      /**< SSID of the current connected wlan */
    uint8_t  bssid[6];      /**< BSSID of the current connected wlan */
    int      channel;       /**< Channel of the current connected wlan */
    uint8_t security;
} ql_link_status_s;
typedef struct
{
	int8_t rssi;
}ql_wifi_link_info_t;

/**************************************************************************************************
* struct
**************************************************************************************************/


typedef struct
{
	uint8_t dhcp;       /**< DHCP mode: @ref DHCP_Disable, @ref DHCP_Client, @ref DHCP_Server.*/
#if LWIP_IPV6
    char    ip[128];     /**< Local IP address on the target wlan interface: @ref wlanInterfaceTypedef.*/
    char    gate[128];   /**< Router IP address on the target wlan interface: @ref wlanInterfaceTypedef.*/
    char    mask[128];   /**< Netmask on the target wlan interface: @ref wlanInterfaceTypedef.*/
    char    dns[128];    /**< DNS server IP address.*/
    char    standby_dns[128];
#else
	char    ip[16];     /**< Local IP address on the target wlan interface: @ref wlanInterfaceTypedef.*/
    char    gate[16];   /**< Router IP address on the target wlan interface: @ref wlanInterfaceTypedef.*/
    char    mask[16];   /**< Netmask on the target wlan interface: @ref wlanInterfaceTypedef.*/
    char    dns[16];    /**< DNS server IP address.*/
    char    standby_dns[16];
#endif
    char    mac[16];    /**< MAC address, example: "C89346112233".*/
    char    broadcastip[16];
} ql_ip_status_s;

/**************************************************************************************************
* callback
**************************************************************************************************/
/**************************************************************************************************
* API enum
**************************************************************************************************/
typedef enum
{
    QL_WIFI_SUCCESS      = 0,
    QL_WIFI_EXECUTE_ERR,
    QL_WIFI_INVALID_PARAM_ERR,
}ql_wifi_errcode_e;

typedef enum {
    QL_SYSTEM_EVENT_WIFI_READY = 0,           /**< WiFi ready */
    QL_SYSTEM_EVENT_SCAN_DONE,                /**< finish scanning AP */
    QL_SYSTEM_EVENT_STA_START,                /**< station start */
    QL_SYSTEM_EVENT_STA_STOP,                 /**< station stop */
    QL_SYSTEM_EVENT_STA_CONNECTED,            /**< station connected to AP */
    QL_SYSTEM_EVENT_STA_DISCONNECTED,         /**< station disconnected from AP */
    QL_SYSTEM_EVENT_STA_AUTHMODE_CHANGE,      /**< the auth mode of AP connected by station changed */
    QL_SYSTEM_EVENT_STA_GTK_REKEY,            /**< station in gtk rekey */
    QL_SYSTEM_EVENT_STA_GOT_IP,               /**< station got IP from connected AP */
    QL_SYSTEM_EVENT_STA_LOST_IP,              /**< station lost IP and the IP is reset to 0 */
    QL_SYSTEM_EVENT_STA_4WAY_HS_FAIL,         /**< station 4-Way Handshake failed, pre-shared key may be incorrect*/
    QL_SYSTEM_EVENT_STA_WPS_ER_SUCCESS,       /**< station wps succeeds in enrollee mode */
    QL_SYSTEM_EVENT_STA_WPS_ER_FAILED,        /**< station wps fails in enrollee mode */
    QL_SYSTEM_EVENT_STA_WPS_ER_TIMEOUT,       /**< station wps timeout in enrollee mode */
    QL_SYSTEM_EVENT_STA_WPS_ER_PIN,           /**< station wps pin code in enrollee mode */
    QL_SYSTEM_EVENT_STA_SAE_AUTH_FAIL,
    QL_SYSTEM_EVENT_AP_START,                 /**< soft-AP start */
    QL_SYSTEM_EVENT_AP_STOP,                  /**< soft-AP stop */
    QL_SYSTEM_EVENT_AP_STACONNECTED,          /**< a station connected to soft-AP */
    QL_SYSTEM_EVENT_AP_STADISCONNECTED,       /**< a station disconnected from soft-AP */
    QL_SYSTEM_EVENT_AP_STAIPASSIGNED,         /**< soft-AP assign an IP to a connected station */
    QL_SYSTEM_EVENT_AP_PROBEREQRECVED,        /**< Receive probe request packet in soft-AP interface */
    QL_SYSTEM_EVENT_GOT_IP6,                  /**< station or ap or ethernet interface v6IP addr is preferred */
    QL_SYSTEM_EVENT_ETH_START,                /**< ethernet start */
    QL_SYSTEM_EVENT_ETH_STOP,                 /**< ethernet stop */
    QL_SYSTEM_EVENT_ETH_CONNECTED,            /**< ethernet phy link up */
    QL_SYSTEM_EVENT_ETH_DISCONNECTED,         /**< ethernet phy link down */
    QL_SYSTEM_EVENT_ETH_GOT_IP,               /**< ethernet got IP from connected AP */
    QL_SYSTEM_EVENT_STA_SCAN_NO_AP,               /**scan not get ap */
    QL_SYSTEM_EVENT_STA_RENEW_IP,             /**renew ip**/
    QL_SYSTEM_EVENT_MAX
} ql_system_event_id_t;

typedef enum {
    QL_WLAN_AUTH_OPEN          = 0,
    QL_WLAN_AUTH_WEP           = 1,
    QL_WLAN_AUTH_WPA_PSK       = 2,
    QL_WLAN_AUTH_WPA2_PSK      = 3,
    QL_WLAN_AUTH_WPA_WPA2_PSK  = 4,
    QL_WLAN_AUTH_WPA3_PSK      = 5,
    QL_WLAN_AUTH_WPA2_WPA3_PSK = 6,
} ql_wlan_auth_mode_e;

typedef enum
{
    QL_SOFT_AP = 0,  
    QL_STATION = 1,  
} ql_wifi_mode_e;
/**************************************************************************************************
* API struct
**************************************************************************************************/
typedef struct 
{
    ql_wifi_mode_e wifi_mode;              
    char wifi_ssid[33];          
    char wifi_key[64];            
    char local_ip_addr[16];       
    char net_mask[16];            
    char gateway_ip_addr[16];     
    char dns_server_ip_addr[16];   
    char dhcp_mode;               
    char wifi_bssid[6];
    ql_wlan_auth_mode_e authmode;
    uint8_t max_connect;
    char reserved[25];
    int  wifi_retry_interval;     
} ql_network_init_s;

typedef struct ql_network_init_ap_st
{
    char wifi_ssid[32];
    char wifi_key[64];
    uint8_t channel;
    ql_wlan_auth_mode_e security; /* not support*/
	uint8_t ssid_hidden;
	uint8_t max_con; /* not support*/
    char local_ip_addr[16];
    char net_mask[16];
    char gateway_ip_addr[16]; /* not support*/
    char dns_server_ip_addr[16];
    char dhcp_mode;
    char reserved[32];
    int  wifi_retry_interval; /* not support*/
} ql_network_init_ap_s;
                                     
typedef struct 
{
    char ssid[33];  
    char appower;   
    uint8_t bssid[6];
    char channel;   
    uint8_t security;   
} ql_apliststruct;

typedef  struct  ql_scan_result_t
{
	char apnum; 	  /**< The number of access points found in scanning.*/
    ql_apliststruct *ql_aplist_s;
} ql_scan_result_s;

typedef struct {
    uint8_t ssid[QL_WIFI_SSID_MAX_LEN];      /**< SSID of connected AP */
    uint8_t ssid_len;                        /**< SSID length of connected AP */
    uint8_t bssid[6];                        /**< BSSID of connected AP*/
    uint8_t channel;                         /**< channel of connected AP*/
    ql_wlan_auth_mode_e authmode;
} ql_system_event_sta_connected_t;

typedef struct {
    uint8_t ssid[QL_WIFI_SSID_MAX_LEN];         /**< SSID of disconnected AP */
    uint8_t ssid_len;                        /**< SSID length of disconnected AP */
    uint8_t bssid[6];                        /**< BSSID of disconnected AP */
    uint16_t reason;                          /**< reason of disconnection */
} ql_system_event_sta_disconnected_t;

typedef struct {
    uint32_t status;          /**< status of scanning APs */
    uint8_t  number;
    uint8_t  scan_id;
} ql_system_event_sta_scan_done_t;

typedef struct {
    ql_wlan_auth_mode_e old_mode;         /**< the old auth mode of AP */
    ql_wlan_auth_mode_e new_mode;         /**< the new auth mode of AP */
} ql_system_event_sta_authmode_change_t;

typedef struct {
    ql_ip_info_t ip_info;
    bool ip_changed;
} ql_system_event_sta_got_ip_t;

typedef struct {
    uint8_t mac[6];           /**< MAC address of the station connected to soft-AP */
    uint8_t aid;              /**< the aid that soft-AP gives to the station connected to  */
    ql_ip_info_t ip_info;
} ql_system_event_ap_staconnected_t;

typedef struct {
    uint8_t mac[6];           /**< MAC address of the station disconnects to soft-AP */
    uint8_t aid;              /**< the aid that soft-AP gave to the station disconnects to  */
    ql_ip_info_t ip_info;
} ql_system_event_ap_stadisconnected_t;

typedef struct {
    uint8_t in_out;           /**< sta enter(1)/exit(0) gtk rekey */
}ql_system_event_sta_gtk_rekey_t;

typedef union {
    ql_system_event_sta_connected_t               connected;          /**< station connected to AP */
    ql_system_event_sta_disconnected_t            disconnected;       /**< station disconnected to AP */
    ql_system_event_sta_scan_done_t               scan_done;          /**< station scan (APs) done */
    ql_system_event_sta_authmode_change_t         auth_change;        /**< the auth mode of AP station connected to changed */
    ql_system_event_sta_got_ip_t                  got_ip;             /**< station got IP, first time got IP or when IP is changed */
    ql_system_event_ap_staconnected_t             sta_connected;      /**< a station connected to soft-AP */
    ql_system_event_ap_stadisconnected_t          sta_disconnected;   /**< a station disconnected to soft-AP */
    ql_system_event_sta_gtk_rekey_t               in_out_rekey;       /**< station enter/exit gtk rekey */
} ql_system_event_info_t;

typedef struct {
    uint8_t                vif;
    ql_system_event_id_t   event_id;      /**< event ID */
    ql_system_event_info_t event_info;    /**< event information */
} ql_system_event_t;


typedef ql_sys_err_t (*ql_system_event_handler_t)(ql_system_event_t *event);
typedef ql_sys_err_t (*ql_system_event_cb_t)(void *ctx, ql_system_event_t *event);
typedef int (*ql_event_cb_t)(void *arg, int event_module,int event_id, void *event_data);
typedef void (*ql_monitor_cb_t)(uint8_t *data, int len, ql_wifi_link_info_t *info);
/**************************************************************************************************
* API function
**************************************************************************************************/
/**
 * @brief
 * @param  ct
 * @return int
 */
ql_wifi_errcode_e ql_wlan_set_country(ql_wifi_country_e ct);

/**
 * @brief
 *
 * @param network_init_para
 * @return int
 */
int ql_wlan_start(ql_network_init_s *network_init_para);
/**
 * @brief 
 * 
 * @param mode 
 * @return int 
 */
int ql_wlan_stop(ql_wifi_mode_e mode);
/**
 * @brief 
 * 
 * @param ssid 
 * @return int 
 */
int ql_wlan_start_scan(void);
/**
 * @brief 
 * 
 * @param ind_cb 
 * @return int 
 */
int ql_wlan_scan_ap_reg_cb(ql_event_cb_t ind_cb);
/**
 * @brief 
 * 
 * @param results 
 * @return int 
 */
int ql_wlan_sta_scan_result(ql_scan_result_s *results);
/**
 * @brief 
 * 
 * @param ssid_ary 
 * @param ssid_num 
 * @return int 
 */
int ql_wlan_start_assign_scan(uint8_t **ssid_ary, uint8_t ssid_num);
/**
 * @brief 
 * 
 * @param evt_mod 
 * @param cb 
 * @return int 
 */
int ql_wlan_status_register_cb(ql_system_event_cb_t cb);
/**
 * @brief 
 * 
 * @return int 
 */
int ql_wlan_start_monitor(uint32_t filter_mask, uint8_t channel);
/**
 * @brief 
 * 
 * @return int 
 */
int ql_wlan_stop_monitor(void);
/**
 * @brief 
 * 
 * @param fn 
 * @return int 
 */
int ql_wlan_register_monitor_cb(ql_monitor_cb_t fn);
/**
 * @brief 
 * 
 * @param ap_info 
 * @return int 
 */
int ql_wlan_ap_para_info_get(ql_network_init_ap_s *ap_info);
/**
 * @brief 
 * 
 * @param outNetpara 
 * @param inInterface 
 * @return int 
 */
int ql_wlan_get_ip_status(ql_ip_status_s *outnetpara, ql_wifi_mode_e interface);

/**
 * @brief 
 * 
 * @param outStatus 
 * @return int 
 */
int ql_wlan_get_link_status(ql_link_status_s *outstatus);
/**
 * @brief 
 * 
 * @return int 
 */
int ql_wlan_get_channel(void);

/**
 * @brief 
 * 
 * @param channel 
 * @return int 
 */
int ql_wlan_set_channel(int channel);
/**
 * @brief 
 * 
 * @param uri 
 * @return int 
 */
ql_wifi_errcode_e ql_wlan_ota_download(const char *uri);

/**
 * @brief Enter the low-power keepalive mode
 * 
 * @param uri 
 * @return int 
 */
uint32_t ql_wlan_pm_enable(void);  //TODO: set pm type
/**
 * @brief 
 * 
 * @return uint32_t 
 */
bool ql_ap_ip_is_start(void);

/**
 * @brief 
 * 
 * @return uint32_t 
 */
bool ql_sta_ip_is_start(void);

/**************************************************************************************************
* other functions
**************************************************************************************************/
int ql_wlan_get_sta_ip(ql_ip_addr_t  *v4_addr, ql_ip_addr_t *v6_addr);
int ql_wlan_get_ap_ip(ql_ip_addr_t  *v4_addr, ql_ip_addr_t *v6_addr);
int ql_wlan_get_scan_num(void);
int ql_sta_chiper_type(void);
ql_wifi_status_e ql_wlan_get_ap_status(void);
ql_wifi_status_e ql_wlan_get_sta_status(void);
ql_system_event_cb_t ql_sys_event_loop_set_cb(ql_system_event_cb_t cb,void *ctx);
int ql_wlan_get_ip6_status(ql_ip_status_s *outNetpara, ql_wifi_mode_e inInterface);
QlIpType_e  ql_wlan_check_ip_type(char *IP);
int ql_wlan_scan_ap_result(uint8_t ap_num_index, QlWifiInfoTypeDef_st*ap_info);

#ifdef __cplusplus
}
#endif
#endif
