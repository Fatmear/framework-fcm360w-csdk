/**
 * @file ql_sys_pub.h
 * @author lei.wang (lei.wang@quectel.com)
 * @brief
 * @version 0.1
 * @date 2023-06-029
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef _QL_SYS_PUB_H
#define _QL_SYS_PUB_H

#include <stdint.h>
#include "ql_type.h"
#include "ql_api_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    QL_RST_TYPE_POWER_ON = 0,
    QL_RST_TYPE_FATAL_EXCEPTION,
    QL_RST_TYPE_SOFTWARE_REBOOT,
    QL_RST_TYPE_HARDWARE_REBOOT,
    QL_RST_TYPE_OTA,
    QL_RST_TYPE_WAKEUP,
    QL_RST_TYPE_SOFTWARE,             // start from app, GPIO holdon
    QL_RST_TYPE_HARDWARE_WDT_TIMEOUT, // reserve
    QL_RST_TYPE_SOFTWARE_WDT_TIMEOUT, // reserve
    QL_RST_TYPE_UNKOWN                // Possible reasons: 1. PC pointer 0 address jump
} ql_reset_source_status_e;

typedef enum {
    QL_SYS_SUCCESS = 0,
    QL_SYS_EXECUTE_ERR,
    QL_SYS_INVALID_PARAM_ERR,
} ql_sys_errcode_e;

typedef enum
{
    QL_LL_NO = 0,     ///> NO log
    QL_LL_ASSERT = 1, ///> ASSERT log only
    QL_LL_ERR = 2,    ///> Add error log
    QL_LL_WARN = 3,   ///> Add warning log
    QL_LL_INFO = 4,   ///> Add info log
    QL_LL_DBG = 5,    ///> Add debug log
} ql_sys_log_level_e;
/**
 * @brief
 *
 * @return ql_sys_reboot
 */
ql_sys_errcode_e ql_sys_reboot(void);

/**
 * @brief
 *
 * @return ql_reset_source_status_e
 */
ql_reset_source_status_e ql_sys_get_reset_reason(void);

/**
 * @brief
 * @param level : ql_sys_log_level_e
 * @return ql_sys_set_log_level
 */
ql_reset_source_status_e ql_sys_set_log_level(ql_sys_log_level_e level);
#ifdef __cplusplus
}
#endif
#endif