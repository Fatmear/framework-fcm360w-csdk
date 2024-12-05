#ifndef __QL_BLE_H__
#define __QL_BLE_H__
/**************************************************************************************************
* include
**************************************************************************************************/
#include "ql_api_osi.h"
#include <stdint.h>
#include "ql_ble_errcode.h"
/**************************************************************************************************
* define
**************************************************************************************************/
#define QL_BLE_TMR_PRIV_ADDR_MIN (0x003C)
#define QL_BLE_TMR_PRIV_ADDR_MAX (0x0384)
#define QL_GAP_BD_ADDR_LEN (6)
/*******************************************ADV TYPE****************************************************/
#define QL_BLE_GAP_ADV_TYPE_CONN_SCANNABLE_UNDIRECTED                      (0x01)  /**< CONN and scannable undirected*/
#define QL_BLE_GAP_ADV_TYPE_CONN_NONSCANNABLE_DIR_HIGHDUTY_CYCLE           (0x02)  /**< CONN non-scannable directed advertising
                                                                                        events. Advertising interval is less that 3.75 ms.
                                                                                        Use this type for fast reconnections.
                                                                                        @note Advertising data is not supported. */
#define QL_BLE_GAP_ADV_TYPE_CONN_NONSCANNABLE_DIRECTED                     (0x03)  /**< CONN non-scannable directed advertising
                                                                                        events.
                                                                                        @note Advertising data is not supported. */
#define QL_BLE_GAP_ADV_TYPE_NONCONN_SCANNABLE_UNDIRECTED                   (0x04)  /**< Non-CONN scannable undirected
                                                                                        advertising events. */
#define QL_BLE_GAP_ADV_TYPE_NONCONN_NONSCANNABLE_UNDIRECTED                (0x05)  /**< Non-CONN non-scannable undirected
                                                                                        advertising events. */
#define QL_BLE_GAP_ADV_TYPE_EXTENDED_CONN_NONSCANNABLE_UNDIRECTED          (0x06)  /**< CONN non-scannable undirected advertising
                                                                                        events using extended advertising PDUs. */
#define QL_BLE_GAP_ADV_TYPE_EXTENDED_CONN_NONSCANNABLE_DIRECTED            (0x07)  /**< CONN non-scannable directed advertising
                                                                                        events using extended advertising PDUs. */
#define QL_BLE_GAP_ADV_TYPE_EXTENDED_NONCONN_SCANNABLE_UNDIRECTED          (0x08)  /**< Non-CONN scannable undirected advertising
                                                                                        events using extended advertising PDUs.
                                                                                        @note Only scan response data is supported. */
#define QL_BLE_GAP_ADV_TYPE_EXTENDED_NONCONN_SCANNABLE_DIRECTED            (0x09)  /**< Non-CONN scannable directed advertising
                                                                                        events using extended advertising PDUs.
                                                                                        @note Only scan response data is supported. */
#define QL_BLE_GAP_ADV_TYPE_EXTENDED_NONCONN_NONSCANNABLE_UNDIRECTED       (0x0A)  /**< Non-CONN non-scannable undirected advertising
                                                                                        events using extended advertising PDUs. */
#define QL_BLE_GAP_ADV_TYPE_EXTENDED_NONCONN_NONSCANNABLE_DIRECTED         (0x0B)  /**< Non-CONN non-scannable directed advertising
                                                                                        events using extended advertising PDUs. */
/**************************************************************************************************
* typedef enum
**************************************************************************************************/
typedef enum 
{
	QL_BLE_ROLE_NONE		= 0x00, // No role set yet
	QL_BLE_ROLE_OBSERVER	= 0x01, // Observer role
	QL_BLE_ROLE_BROADCASTER = 0x02, // Broadcaster role
	QL_BLE_ROLE_CENTRAL	    = (0x04 | QL_BLE_ROLE_OBSERVER), // Master/Central role
	QL_BLE_ROLE_PERIPHERAL  = (0x08 | QL_BLE_ROLE_BROADCASTER), // Peripheral/Slave role
	QL_BLE_ROLE_ALL		    = (QL_BLE_ROLE_CENTRAL | QL_BLE_ROLE_PERIPHERAL), // Device has all role, both peripheral and central
}ql_ble_role;

typedef enum
{
    QL_BLE_CMD_MSG_RECV_SCAN = 0,
    QL_BLE_CMD_MSG_RECV_NET_CONFIG,
} QL_BLE_CMD_MSG_E;

typedef struct
{
    QL_BLE_CMD_MSG_E msg_type;
    char*data;
} QL_BLE_NET_MSG_ST;
typedef enum
{
    QL_BLE_ROLE_SLAVE,  /* slave */
    QL_BLE_ROLE_MASTER, /* master */
    
} QL_BLE_ROLE_E;


typedef struct
{
    /// 6-byte array address value
    uint8_t addr[QL_GAP_BD_ADDR_LEN];
} ql_bd_addr_t;
typedef enum
{
    QL_BLE_GAP_EVT_RESET, /**< RESET */

    QL_BLE_GAP_EVT_CONNECT, /**< Connected as peripheral role */

    QL_BLE_GAP_EVT_DISCONNECT, /**< Disconnected */

    QL_BLE_GAP_EVT_ADV_STATE, /**< Adv State */

    QL_BLE_GAP_EVT_ADV_REPORT, /**< Scan result report */

    QL_BLE_GAP_EVT_CONN_PARAM_REQ, /**< Parameter update request */

    QL_BLE_GAP_EVT_CONN_PARAM_UPDATE, /**< Parameter update successfully */

    QL_BLE_GAP_EVT_CONN_RSSI, /**< Got RSSI value of link peer device */

    QL_BLE_GAP_IRK_GEN_IND, /**< Generate IRK */
} QL_BLE_GAP_EVT_TYPE_E;
typedef enum
{
    QL_BLE_GATT_CHAR_PROP_BROADCAST = 0x01,           /**< If set, permits broadcasts of the Characteristic Value using Server Characteristic Configuration Descriptor. */
    QL_BLE_GATT_CHAR_PROP_READ = 0x02,                /**< If set, permits reads of the Characteristic Value */
    QL_BLE_GATT_CHAR_PROP_WRITE_NO_RSP = 0x04,        /**< If set, permit writes of the Characteristic Value without response */
    QL_BLE_GATT_CHAR_PROP_WRITE = 0x08,               /**< If set, permits writes of the Characteristic Value with response */
    QL_BLE_GATT_CHAR_PROP_NOTIFY = 0x10,              /**< If set, permits notifications of a Characteristic Value without acknowledgment */
    QL_BLE_GATT_CHAR_PROP_INDICATE = 0x20,            /**< If set, permits indications of a Characteristic Value with acknowledgment */
    QL_BLE_GATT_CHAR_PROP_WRITE_AUTHEN_SIGNED = 0x40, /**< If set, permits signed writes to the Characteristic Value */
    QL_BLE_GATT_CHAR_PROP_EXT_PROP = 0x80,            /**< If set, additional characteristic properties are defined in the Characteristic */
} QL_BLE_CHAR_PROP_TYPE_E;

/**< GATT attribute permission bit field values */
typedef enum
{
    QL_BLE_GATT_PERM_NONE = 0x01,          /**< No operations supported, e.g. for notify-only */
    QL_BLE_GATT_PERM_READ = 0x02,          /**< Attribute read permission. */
    QL_BLE_GATT_PERM_WRITE = 0x04,         /**< Attribute write permission. */
    QL_BLE_GATT_PERM_READ_ENCRYPT = 0x08,  /**< Attribute read permission with encryption. */
    QL_BLE_GATT_PERM_WRITE_ENCRYPT = 0x10, /**< Attribute write permission with encryption. */
    QL_BLE_GATT_PERM_READ_AUTHEN = 0x20,   /**< Attribute read permission with authentication. */
    QL_BLE_GATT_PERM_WRITE_AUTHEN = 0x40,  /**< Attribute write permission with authentication. */
    QL_BLE_GATT_PERM_PREPARE_WRITE = 0x80, /**< Attribute prepare write permission. */
} QL_BLE_ATTR_PERM_E;
typedef enum
{
    QL_BLE_STATIC_ADDR,       /**< Public or Private Static Address according to device address configuration */
    QL_BLE_GEN_RSLV_ADDR,     /**< Generated resolvable private random address */
    QL_BLE_GEN_NON_RSLV_ADDR, /**< Generated non-resolvable private random address */
} QL_BLE_ADDR_TYPE_E;
typedef enum
{
    QL_BLE_ADV_DATA,     /**< Adv Data - Only */
    QL_BLE_RSP_DATA,     /**< Scan Response Data - Only */
    QL_BLE_ADV_RSP_DATA, /**< Adv Data + Scan Response Data */
} QL_BLE_GAP_ADV_TYPE_E;
typedef enum
{
    QL_SCAN_TYPE_GEN_DISC = 0, /**< General discovery */

    QL_SCAN_TYPE_LIM_DISC, /**< Limited discovery */

    QL_SCAN_TYPE_OBSERVER, /**< Observer */

    QL_SCAN_TYPE_SEL_OBSERVER, /**< Selective observer */

    QL_SCAN_TYPE_CONN_DISC, /**< Connectable discovery */

    QL_SCAN_TYPE_SEL_CONN_DISC, /**< Selective connectable discovery */
} QL_BLE_SCAN_TYPE_E;
enum ql_ble_pairing_mode
{
    /// No pairing authorized
    QL_BLE_PAIRING_DISABLE = 0,
    /// Legacy pairing Authorized
    QL_BLE_PAIRING_LEGACY = (1 << 0),
    /// Secure Connection pairing Authorized
    QL_BLE_PAIRING_SEC_CON = (1 << 1),
};
enum ql_ble_priv_cfg
{
    /// Indicate if identity address is a public (0) or static private random (1) address
    QL_BLE_PRIV_CFG_PRIV_ADDR_BIT = (1 << 0),
    QL_BLE_PRIV_CFG_PRIV_ADDR_POS = 0,
    /// Reserved
    QL_BLE_PRIV_CFG_RSVD = (1 << 1),
    /// Indicate if controller privacy is enabled
    QL_BLE_PRIV_CFG_PRIV_EN_BIT = (1 << 2),
    QL_BLE_PRIV_CFG_PRIV_EN_POS = 2,
};
typedef struct
{
    uint16_t conn_interval_min;      /**< Minimum value for the connection interval.  */
    uint16_t conn_interval_max;      /**< Maximum value for the connection interval. */
    uint16_t conn_latency;           /**< Slave latency for the connection in number of connection events.*/
    uint16_t conn_sup_timeout;       /**< Supervision timeout for the LE Link (in unit of 10ms).*/
    uint16_t conn_establish_timeout; /**< Timeout for connection establishment (in unit of 10ms),only for establish connection*/
} QL_BLE_GAP_CONN_PARAMS_T;
typedef struct
{
    QL_BLE_ADDR_TYPE_E type; /**< Mac Address Type*/
    uint8_t addr[6];         /**< Mac Address, Address size, 6 bytes */
} QL_BLE_GAP_ADDR_T;
typedef struct
{
    uint8_t role;                         /**< BLE role for this connection, see @ref QL_BLE_ROLE_MASTER, or QL_BLE_ROLE_SLAVE */
    QL_BLE_GAP_ADDR_T peer_addr;          /**< Reserved, [Ble Central],For some platform, we will get the peer address after connect one device */
    QL_BLE_GAP_CONN_PARAMS_T conn_params; /**< Report Connection Parameters */
} QL_BLE_GAP_CONNECT_EVT_T;
typedef struct
{
    uint8_t role;   /**< BLE role for this disconnection */
    int32_t reason; /**< Report Disconnection Reason */
} QL_BLE_GAP_DISCONNECT_EVT_T;
typedef struct
{
    uint16_t length; /**< Ble Data Len */
    uint8_t *p_data; /**< Ble Data Pointer */
} QL_BLE_DATA_T;
typedef struct
{
    QL_BLE_GAP_ADV_TYPE_E adv_type; /**< Advertising report type. Refer to @QL_BLE_GAP_ADV_TYPE_E */
    QL_BLE_GAP_ADDR_T peer_addr;    /**< Bluetooth address of the peer device. */
    int8_t rssi;                    /**< Received Signal Strength Indication in dBm of the last packet received. */
    uint8_t channel_index;          /**< Channel Index on which the last advertising packet is received (37-39).channel index = 37, it means that we do advertisement in channel 37. */
    QL_BLE_DATA_T data;             /**< Received advertising or scan response data.  */
} QL_BLE_GAP_ADV_REPORT_T;
typedef struct
{
    uint8_t irk[16];
} QL_BLE_GAP_GEN_IRK_T;
typedef struct
{
    QL_BLE_GAP_EVT_TYPE_E type; /**< Gap Event */
    uint16_t conn_handle;       /**< Connection Handle */
    int32_t result;             /**< Will Refer to HOST STACK Error Code */

    union
    {
        QL_BLE_GAP_CONNECT_EVT_T connect;       /**< Receive connect callback, This value can be used with QL_BLE_EVT_PERIPHERAL_CONNECT and QL_BLE_EVT_CENTRAL_CONNECT_DISCOVERY*/
        QL_BLE_GAP_DISCONNECT_EVT_T disconnect; /**< Receive disconnect callback*/
        QL_BLE_GAP_ADV_REPORT_T adv_report;     /**< Receive Adv and Respond report*/
        QL_BLE_GAP_CONN_PARAMS_T conn_param;    /**< We will update connect parameters.This value can be used with QL_BLE_EVT_CONN_PARAM_REQ and QL_BLE_EVT_CONN_PARAM_UPDATE*/
        int8_t link_rssi;                       /**< Peer device RSSI value */
        QL_BLE_GAP_GEN_IRK_T gen_irk;
    } gap_event;
} QL_BLE_GAP_PARAMS_EVT_T;
typedef struct
{
    uint8_t adv_type;              /**< Adv Type. Refer to QL_BLE_GAP_ADV_TYPE_CONN_SCANNABLE_UNDIRECTED etc.*/
    QL_BLE_GAP_ADDR_T direct_addr; /**< For Directed Advertising, you can fill in direct address */

    uint16_t adv_interval_min; /**< Range: 0x0020 to 0x4000  Time = N * 0.625 msec Time Range: 20 ms to 10.24 sec */
    uint16_t adv_interval_max; /**< Range: 0x0020 to 0x4000  Time = N * 0.625 msec Time Range: 20 ms to 10.24 sec */
    uint8_t adv_channel_map;   /**< Advertising Channel Map, 0x01 = adv channel index 37,  0x02 = adv channel index 38,
                                       0x04 = adv channel index 39. Default Value: 0x07*/
} QL_BLE_GAP_ADV_PARAMS_T;
typedef enum
{
    QL_BLE_GATT_EVT_MTU_REQUEST, /**< MTU exchange request event, For Ble peripheral, we need to do reply*/

    QL_BLE_GATT_EVT_MTU_RSP, /**< MTU exchange respond event, For Ble Central, the ble central has finished the MTU-Request */

    QL_BLE_GATT_EVT_PRIM_SEV_DISCOVERY, /**< [Ble Central] Discovery Service */

    QL_BLE_GATT_EVT_CHAR_DISCOVERY, /**< [Ble Central] Discovery Characteristics*/

    QL_BLE_GATT_EVT_CHAR_DESC_DISCOVERY, /**< [Ble Central] Discovery descriptors */

    QL_BLE_GATT_EVT_NOTIFY_INDICATE_TX, /**< [Ble peripheral] Transfer data Callback, only report Result */

    QL_BLE_GATT_EVT_WRITE_REQ, /**< [Ble Peripheral] Get Client-Write Char Request*/

    QL_BLE_GATT_EVT_WRITE_CMP, /**< [Ble Central] Transfer data Callback, only report Result */

    QL_BLE_GATT_EVT_NOTIFY_INDICATE_RX, /**< [Ble Central] Get Notification or Indification data */

    QL_BLE_GATT_EVT_READ_RX, /**< [Ble Central] Get Char-Read Data */
} QL_BLE_GATT_EVT_TYPE_E;
typedef enum
{
    QL_BLE_UUID_TYPE_16,  /**< UUID 16 bit */
    QL_BLE_UUID_TYPE_32,  /**< UUID 32 bit */
    QL_BLE_UUID_TYPE_128, /**< UUID 128 bit */
} QL_BLE_UUID_TYPE_E;

typedef enum
{
    QL_BLE_GATT_DISABLE,
    QL_BLE_GATT_NOTIFICATION,
    QL_BLE_GATT_INDICATION,
}ql_ble_gattc_config_flag_t;

typedef struct
{
    QL_BLE_UUID_TYPE_E uuid_type; /**< UUID Type, Refer to @QL_BLE_UUID_TYPE_E */

    union
    {
        uint16_t uuid16;     /**< 16-bit UUID value  */
        uint32_t uuid32;     /**< 32-bit UUID value */
        uint8_t uuid128[16]; /**< Little-Endian UUID bytes. 128bit uuid*/
    } uuid;
} QL_BLE_UUID_T;
typedef struct
{
    QL_BLE_UUID_T uuid;    /**< Discovery Service UUID */
    uint16_t start_handle; /**< Discovery Start Handle */
    uint16_t end_handle;   /**< Discovery End Handle */
} QL_BLE_GATT_SVC_DISC_TYPE_T;
typedef struct
{
    QL_BLE_UUID_T uuid; /**< Discovery Service UUID */
    uint16_t handle;    /**< Discovery Char value Handle */
} QL_BLE_GATT_CHAR_DISC_TYPE_T;
typedef struct
{
    uint16_t cccd_handle; /**< Discovery Descriptor Handle, Return CCCD Handle */
    uint8_t uuid_len;     /**< UUID length */
    uint16_t uuid16;      /**< Descriptor UUID */
} QL_BLE_GATT_DESC_DISC_TYPE_T;
typedef struct
{
    uint16_t char_handle; /**< Notify Characteristic Handle */
    int32_t result;       /**< Notify Result */
} QL_BLE_NOTIFY_RESULT_EVT_T;
typedef struct
{
    uint16_t char_handle; /**< Specify one characteristic handle */
    QL_BLE_DATA_T report; /**< Report Data, Refer to @ QL_BLE_DATA_T */
} QL_BLE_DATA_REPORT_T;
typedef struct
{
    uint16_t char_handle; /**< Write Characteristic Handle */
    int32_t result;       /**< Write Result */
} QL_BLE_WRITE_RESULT_EVT_T;
typedef enum
{
    QL_BLE_UUID_UNKNOWN = 0x0000,           /**< Reserved UUID. */
    QL_BLE_UUID_SERVICE_PRIMARY = 0x2800,   /**< Primary Service. */
    QL_BLE_UUID_SERVICE_SECONDARY = 0x2801, /**< Secondary Service. */
    QL_BLE_UUID_SERVICE_INCLUDE = 0x2802,   /**< Include. */
    QL_BLE_UUID_CHARACTERISTIC = 0x2803,    /**< Characteristic. */
} QL_BLE_SERVICE_TYPE_E;
typedef struct
{
    uint16_t handle; /**< [Output] After init the characteristic, we will get the char-handle, we need to restore it */

    QL_BLE_UUID_T char_uuid; /**< Characteristics UUID */
    uint8_t property;        /**< Characteristics property , Refer to QL_BLE_CHAR_PROP_TYPE_E */
    uint8_t permission;      /**< Characteristics value attribute permission */
    uint16_t value_len;      /**< Characteristics value length */
} QL_BLE_CHAR_PARAMS_T;
typedef struct
{
    uint16_t handle; /**< After init the service, we will get the svc-handle */

    QL_BLE_UUID_T svc_uuid;     /**< Service UUID */
    QL_BLE_SERVICE_TYPE_E type; /**< Service Type */

    uint8_t char_num;             /**< Number of characteristic */
    QL_BLE_CHAR_PARAMS_T *p_char; /**< Pointer of characteristic */
} QL_BLE_SERVICE_PARAMS_T;
typedef struct
{
    uint8_t svc_num; /**< If we only use service(0xFD50), the svc_num will be set into 1 */
    QL_BLE_SERVICE_PARAMS_T *p_service;
} QL_BLE_GATTS_PARAMS_T;
typedef struct
{
    QL_BLE_GATT_EVT_TYPE_E type; /**< Gatt Event */
    uint16_t conn_handle;        /**< Connection Handle */
    int32_t result;              /**< Will Refer to HOST STACK Error Code */

    union
    {
        uint16_t exchange_mtu;                    /**< This value can be used with QL_BLE_GATT_EVT_MTU_REQUEST and QL_BLE_GATT_EVT_MTU_RSP*/
        QL_BLE_GATT_SVC_DISC_TYPE_T svc_disc;     /**< Discovery All Service, if result is BT_ERROR_DISC_DONE,the procedure ends  */
        QL_BLE_GATT_CHAR_DISC_TYPE_T char_disc;   /**< Discovery Specific Characteristic, if result is BT_ERROR_DISC_DONE,the procedure ends */
        QL_BLE_GATT_DESC_DISC_TYPE_T desc_disc;   /**< Discovery Specific Descriptors*/
        QL_BLE_NOTIFY_RESULT_EVT_T notify_result; /**< This value can be used with QL_BLE_GATT_EVT_NOTIFY_TX*/
        QL_BLE_DATA_REPORT_T write_report;        /**< This value can be used with QL_BLE_GATT_EVT_WRITE_REQ*/
        QL_BLE_WRITE_RESULT_EVT_T write_result;   /**< This value can be used with QL_BLE_GATT_EVT_WRITE_CMP*/
        QL_BLE_DATA_REPORT_T data_report;         /**< This value can be used with QL_BLE_GATT_EVT_NOTIFY_INDICATE_RX*/
        QL_BLE_DATA_REPORT_T data_read;           /**< After we do read attr in central mode, we will get the callback*/
    } gatt_event;
} QL_BLE_GATT_PARAMS_EVT_T;
typedef enum
{
    QL_BLE_SCAN_PROP_PHY_1M_BIT = (1 << 0), /**< Scan advertisement on the LE 1M PHY */

    QL_BLE_SCAN_PROP_PHY_CODED_BIT = (1 << 1), /**< Scan advertisement on the LE Coded PHY */

    QL_BLE_SCAN_PROP_ACTIVE_1M_BIT = (1 << 2), /**< Active scan on LE 1M PHY (Scan Request PDUs may be sent) */

    QL_BLE_SCAN_PROP_ACTIVE_CODED_BIT = (1 << 3), /**< Active scan on LE Coded PHY (Scan Request PDUs may be sent) */

    QL_BLE_SCAN_PROP_ACCEPT_RPA_BIT = (1 << 4), /**< Accept directed advertising packets if we use a RPA and target address cannot be solved by the controller*/

    QL_BLE_SCAN_PROP_FILT_TRUNC_BIT = (1 << 5), /**< Filter truncated advertising or scan response reports */
} QL_BLE_SCAN_PROP;
typedef enum
{
    QL_DUP_FILT_DIS = 0, /**< Disable filtering of duplicated packets */

    QL_DUP_FILT_EN, /**< Enable filtering of duplicated packets */

    QL_DUP_FILT_EN_PERIOD, /**< Enable filtering of duplicated packets, reset for each scan period */
} QL_BLE_DUP_FILTER_POL_E;
typedef struct
{
    uint8_t scan_type;        /**< Type of scanning to be started @QL_BLE_SCAN_TYPE_E*/
    uint8_t dup_filt_pol;     /**< Duplicate packet filtering policy @QL_BLE_DUP_FILTER_POL_E*/
    uint8_t scan_prop;        /**< Properties for the scan procedure (@QL_BLE_SCAN_PROP for bit signification)*/
    uint16_t interval;        /**< Scan interval in 625 us units. */
    uint16_t window;          /**< Scan window in 625 us units. */
    uint16_t duration;        /**< Scan duration (in unit of 10ms). 0 means that the controller will scan continuously until
                              /// reception of a stop command from the application */
    uint16_t period;          /**< Scan period (in unit of 1.28s). Time interval betweem two consequent starts of a scan duration
                              /// by the controller. 0 means that the scan procedure is not periodic */
    uint8_t scan_channel_map; /**< Scan Channel Index, refer to @QL_BLE_GAP_ADV_PARAMS_T*/
} QL_BLE_GAP_SCAN_PARAMS_T;
typedef enum
{
    /// Stopped state
    QL_BLE_GAP_ACT_STATE_STOPPED = (0x01),
    /// Started state
    QL_BLE_GAP_ACT_STATE_STARTED = (0x02),

    /// Invalid state
    QL_BLE_GAP_ACT_STATE_INVALID = (0xFF),
} QL_GAP_ACT_STATE_T;
typedef enum
{
    QL_BT_ADV_IDX,
    QL_BT_SCAN_IDX,
    QL_BT_INIT_IDX,

    QL_BT_IDX_MAX,
} QL_BT_ACT_T;
typedef struct
{
    /// Device Role: Central, Peripheral, Observer, Broadcaster or All roles.
    uint8_t role;

    /// -------------- Privacy Config -----------------------
    /// Duration before regenerate device address when privacy is enabled. - in seconds
    uint16_t renew_dur;
    /// Provided own static private random address
    ql_bd_addr_t addr;
    /// Device IRK used for resolvable random BD address generation (LSB first)
    /// Key value MSB -> LSB
    uint8_t irk[16];
    /// Privacy configuration bit field (@see enum ql_ble_priv_cfg for bit signification)
    uint8_t privacy_cfg;

    /// -------------- Security Config -----------------------

    /// Pairing mode authorized (@see enum ble_pairing_mode)
    uint8_t pairing_mode;

    /// --------------- L2CAP Configuration ---------------------------
    /// Maximal MTU acceptable for device
    uint16_t max_mtu;
    /// Maximal MPS Packet size acceptable for device, MPS <= MTU
    uint16_t max_mps;

} QL_BLE_DEV_CONFIG;

typedef enum
{
    QL_BT_SUCCESS = 0,
    QL_BT_ERR_PROFILE,
    QL_BT_ERR_CREATE_DB,
    QL_BT_ERR_CMD_NOT_SUPPORT,
    QL_BT_ERR_UNKNOW_IDX,
    QL_BT_ERR_BLE_STATUS,
    QL_BT_ERR_BLE_PARAM,
    QL_BT_ERR_ADV_DATA,
    QL_BT_ERR_CMD_RUN,
    QL_BT_ERR_NO_MEM,
    QL_BT_ERR_INIT_CREATE,
    QL_BT_ERR_INIT_STATE,

    QL_BT_ERR_ATTC_WRITE,
    QL_BT_ERR_ATTC_WRITE_UNREGISTER,
} ql_errcode_bt_e;

void ql_ble_reset_all(void);
void ql_ble_gap_callback_register(void *ql_ble_gap_event_cb);
void ql_ble_gatt_callback_register(void *ql_ble_gatt_event_cb);
void ql_ble_set_dev_config(QL_BLE_DEV_CONFIG *dev_config);
void ql_ble_get_mac_addr(char*mac);
QL_BT_RET_T ql_ble_get_device_addr(QL_BLE_GAP_ADDR_T *addr);
QL_BT_RET_T ql_ble_set_device_name(uint8_t *dev_name, uint16_t dev_name_len); /* The maximum length of a local name is 17 bytes*/
QL_BT_RET_T ql_ble_get_device_name(uint8_t *dev_name, uint16_t dev_name_len);
QL_BT_RET_T ql_ble_adv_param_update(QL_BLE_DATA_T *p_adv, QL_BLE_DATA_T *p_scan_rsp);
QL_BT_RET_T ql_ble_adv_param_set(QL_BLE_DATA_T *p_adv, QL_BLE_DATA_T *p_scan_rsp);
QL_BT_RET_T ql_ble_gap_adv_start(QL_BLE_GAP_ADV_PARAMS_T *adv_param);
QL_BT_RET_T ql_ble_gap_adv_stop(void);
QL_BT_RET_T ql_ble_gap_scan_start(QL_BLE_GAP_SCAN_PARAMS_T *p_scan_params);
QL_BT_RET_T ql_ble_gap_scan_stop(void);
QL_BT_RET_T ql_ble_gap_connect(QL_BLE_GAP_ADDR_T *p_peer_addr, QL_BLE_GAP_CONN_PARAMS_T *p_conn_params);
QL_BT_RET_T ql_ble_gap_connect_cancel(void);
QL_BT_RET_T ql_ble_gap_disconnect(uint16_t conn_handle, uint8_t reason);
QL_BT_RET_T ql_ble_connect_param_update(uint16_t conn_handle, QL_BLE_GAP_CONN_PARAMS_T *p_conn_params);
void ql_ble_gatts_service_add(QL_BLE_GATTS_PARAMS_T *p_service, bool is_ues_deatult);  
void ql_ble_gatts_value_notify(uint16_t conn_handle, uint16_t char_handle, uint8_t *p_data, uint16_t length);
void ql_ble_gattc_all_service_discovery(uint16_t conn_handle);
void ql_ble_gattc_all_char_discovery(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle);
QL_BT_RET_T ql_ble_gattc_set_characteristic_notification(uint8_t connection, uint16_t characteristic, uint8_t flags);
void ql_ble_gattc_write_without_rsp(uint16_t conn_handle, uint16_t char_handle, uint8_t *p_data, uint16_t length);
void ql_ble_gattc_write(uint16_t conn_handle, uint16_t char_handle, uint8_t *p_data, uint16_t length);
void ql_ble_gattc_read(uint16_t conn_handle, uint16_t char_handle);
void ql_ble_gattc_exchange_mtu_req(uint16_t conn_handle, uint16_t client_mtu);
uint16_t ql_ble_gattc_get_mtu(uint16_t conn_handle);

#endif