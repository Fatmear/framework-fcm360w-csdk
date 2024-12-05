#ifndef _QL_LOWPOWER_H_
#define _QL_LOWPOWER_H_
#include "ql_api_osi.h"
#ifdef __cplusplus
extern "C"
{
#endif
typedef enum{
    QL_Modem_Sleep,
    QL_Light_Sleep,
    QL_Deep_Sleep,
}ql_sleep_mode_e;

typedef enum{
    QL_Dtim_0,
    QL_Dtim_1 = 1,
    QL_Dtim_3 = 3,
    QL_Dtim_10 =  10,
}ql_sleep_dtim_e;

uint8_t ql_psm_set_sleep_mode(unsigned int sleep_mode, unsigned char listen_interval);
#ifdef __cplusplus
} /*"C" */
#endif

#endif