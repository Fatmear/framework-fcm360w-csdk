#ifndef _QL_WDT_H
#define _QL_WDT_H
#include "ql_api_osi.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    QL_WDG_SUCCESS      = 0,
    QL_WDG_EXECUTE_ERR,
    QL_WDG_INVALID_PARAM_ERR,
}ql_wdg_errcode_e;

/**
 * @brief 
 * 
 * @param timeout 
 * @return ql_wdg_errcode_e 
 */
ql_wdg_errcode_e ql_wdg_init(uint32_t timeout);

/**
 * @brief 
 * 
 * @return ql_wdg_errcode_e 
 */
ql_wdg_errcode_e ql_wdg_reload(void);


/**
 * @brief 
 * 
 * @return ql_wdg_errcode_e 
 */
ql_wdg_errcode_e ql_wdg_finalize(void);

#ifdef __cplusplus
}
#endif
#endif