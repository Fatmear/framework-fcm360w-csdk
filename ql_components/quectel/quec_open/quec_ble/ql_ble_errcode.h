/**
 ****************************************************************************************
 *
 * @file bluetooth_err.h
 *
 * @brief bluetooth Error Information Header.
 *
 * @par Copyright (C):
 *      ESWIN 2015-2020
 *
 ****************************************************************************************
 */

#ifndef QL_BLE_ERRCODE_H
#define QL_BLE_ERRCODE_H
#include "ql_type.h"


/*
 * DEFINES
 ****************************************************************************************
 */
typedef uint8_t QL_BT_RET_T;

enum ql_bt_error
{
    /*****************************************************
     ***			  ERROR CODES					   ***
     *****************************************************/

    QL_BT_ERROR_NO_ERROR = 0x00,
    QL_BT_ERROR_UNKNOWN_HCI_COMMAND = 0x01,
    QL_BT_ERROR_UNKNOWN_CONNECTION_ID = 0x02,
    QL_BT_ERROR_HARDWARE_FAILURE = 0x03,
    QL_BT_ERROR_PAGE_TIMEOUT = 0x04,
    QL_BT_ERROR_AUTH_FAILURE = 0x05,
    QL_BT_ERROR_PIN_MISSING = 0x06,
    QL_BT_ERROR_MEMORY_CAPA_EXCEED = 0x07,
    QL_BT_ERROR_CON_TIMEOUT = 0x08,
    QL_BT_ERROR_CON_LIMIT_EXCEED = 0x09,
    QL_BT_ERROR_SYNC_CON_LIMIT_DEV_EXCEED = 0x0A,
    QL_BT_ERROR_CON_ALREADY_EXISTS = 0x0B,
    QL_BT_ERROR_COMMAND_DISALLOWED = 0x0C, /* Request not allowed in current state.*/
    QL_BT_ERROR_CONN_REJ_LIMITED_RESOURCES = 0x0D,
    QL_BT_ERROR_CONN_REJ_SECURITY_REASONS = 0x0E,
    QL_BT_ERROR_CONN_REJ_UNACCEPTABLE_BDADDR = 0x0F,
    QL_BT_ERROR_CONN_ACCEPT_TIMEOUT_EXCEED = 0x10,
    QL_BT_ERROR_UNSUPPORTED = 0x11,
    QL_BT_ERROR_INVALID_HCI_PARAM = 0x12,
    QL_BT_ERROR_REMOTE_USER_TERM_CON = 0x13,
    QL_BT_ERROR_REMOTE_DEV_TERM_LOW_RESOURCES = 0x14,
    QL_BT_ERROR_REMOTE_DEV_POWER_OFF = 0x15,
    QL_BT_ERROR_CON_TERM_BY_LOCAL_HOST = 0x16,
    QL_BT_ERROR_REPEATED_ATTEMPTS = 0x17,
    QL_BT_ERROR_PAIRING_NOT_ALLOWED = 0x18,
    QL_BT_ERROR_UNKNOWN_LMP_PDU = 0x19,
    QL_BT_ERROR_UNSUPPORTED_REMOTE_FEATURE = 0x1A,
    QL_BT_ERROR_SCO_OFFSET_REJECTED = 0x1B,
    QL_BT_ERROR_SCO_INTERVAL_REJECTED = 0x1C,
    QL_BT_ERROR_SCO_AIR_MODE_REJECTED = 0x1D,
    QL_BT_ERROR_INVALID_LMP_PARAM = 0x1E,
    QL_BT_ERROR_UNSPECIFIED_ERROR = 0x1F,
    QL_BT_ERROR_UNSUPPORTED_LMP_PARAM_VALUE = 0x20,
    QL_BT_ERROR_ROLE_CHANGE_NOT_ALLOWED = 0x21,
    QL_BT_ERROR_LMP_RSP_TIMEOUT = 0x22,
    QL_BT_ERROR_LMP_COLLISION = 0x23,
    QL_BT_ERROR_LMP_PDU_NOT_ALLOWED = 0x24,
    QL_BT_ERROR_ENC_MODE_NOT_ACCEPT = 0x25,
    QL_BT_ERROR_LINK_KEY_CANT_CHANGE = 0x26,
    QL_BT_ERROR_QOS_NOT_SUPPORTED = 0x27,
    QL_BT_ERROR_INSTANT_PASSED = 0x28,
    QL_BT_ERROR_PAIRING_WITH_UNIT_KEY_NOT_SUP = 0x29,
    QL_BT_ERROR_DIFF_TRANSACTION_COLLISION = 0x2A,
    QL_BT_ERROR_QOS_UNACCEPTABLE_PARAM = 0x2C,
    QL_BT_ERROR_QOS_REJECTED = 0x2D,
    QL_BT_ERROR_CHANNEL_CLASS_NOT_SUP = 0x2E,
    QL_BT_ERROR_INSUFFICIENT_SECURITY = 0x2F,
    QL_BT_ERROR_PARAM_OUT_OF_MAND_RANGE = 0x30,
    QL_BT_ERROR_ROLE_SWITCH_PEND = 0x32,        /* LM_ROLE_SWITCH_PENDING				 */
    QL_BT_ERROR_RESERVED_SLOT_VIOLATION = 0x34, /* LM_RESERVED_SLOT_VIOLATION			 */
    QL_BT_ERROR_ROLE_SWITCH_FAIL = 0x35,        /* LM_ROLE_SWITCH_FAILED				 */
    QL_BT_ERROR_EIR_TOO_LARGE = 0x36,           /* LM_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE */
    QL_BT_ERROR_SP_NOT_SUPPORTED_HOST = 0x37,
    QL_BT_ERROR_HOST_BUSY_PAIRING = 0x38,
    QL_BT_ERROR_CONTROLLER_BUSY = 0x3A,
    QL_BT_ERROR_UNACCEPTABLE_CONN_PARAM = 0x3B,
    QL_BT_ERROR_ADV_TO = 0x3C,
    QL_BT_ERROR_TERMINATED_MIC_FAILURE = 0x3D,
    QL_BT_ERROR_CONN_FAILED_TO_BE_EST = 0x3E,
    QL_BT_ERROR_CCA_REJ_USE_CLOCK_DRAG = 0x40,
    QL_BT_ERROR_TYPE0_SUBMAP_NOT_DEFINED = 0x41,
    QL_BT_ERROR_UNKNOWN_ADVERTISING_ID = 0x42,
    QL_BT_ERROR_LIMIT_REACHED = 0x43,
    QL_BT_ERROR_OPERATION_CANCELED_BY_HOST = 0x44,
    QL_BT_ERROR_PKT_TOO_LONG = 0x45,

    QL_BT_ERROR_INVALID_PARAM = 0xA2, /* Invalid parameters set.*/
    QL_BT_ERROR_DISC_DONE = 0xFD,
    QL_BT_ERROR_CHAR_HANDLE_ERROR = 0xFE,
    QL_BT_ERROR_UNDEFINED = 0xFF,
};

#endif /* QL_BT_ERR_H */