#ifndef _QL_TIMER_H_
#define _QL_TIMER_H_

// #include "ql_api_osi.h"
typedef void (*ql_timer_callback)(void *);
typedef enum
{
    QL_TIMER_SUCCESS = 0,
    QL_TIMER_EXECUTE_ERR,
    QL_TIMER_INVALID_PARAM_ERR,
    QL_TIMER_NOT_OPEN_ERR,
} ql_timer_errcode_e;

typedef enum
{
    QL_TIMER_0 = 0,
    QL_TIMER_1,
    QL_TIMER_2,
    QL_TIMER_3,
} ql_timer_number_e;

typedef struct _ql_timer_dev
{
	int used;
	int num;
} ql_timer_dev;

ql_timer_errcode_e ql_TimerInit(ql_timer_number_e timer_id, unsigned long time_ms, ql_timer_callback timer_cb,void *data,int one_shot);
ql_timer_errcode_e ql_TimerInit_us(ql_timer_number_e timer_id, unsigned long time_us, ql_timer_callback timer_cb,void *data,int one_shot);
ql_timer_errcode_e ql_get_timer_cnt(ql_timer_number_e timer_id, unsigned int *cont);
ql_timer_errcode_e ql_timer_delete(ql_timer_number_e timer_id);
ql_timer_errcode_e ql_TimerStart(ql_timer_number_e timer_id);
ql_timer_errcode_e ql_TimerStop(ql_timer_number_e timer_id);
ql_timer_errcode_e ql_TimerChangePeriod(ql_timer_number_e timer_id, int period_us);

#endif
