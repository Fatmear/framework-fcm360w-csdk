#ifndef _QL_PWM_H
#define _QL_PWM_H
#include "ql_gpio.h"

typedef void (*pwm_callbak)(UINT8);

// #define PWM_TEST_CHANNEL QL_PWM_0
// #define PWM_TEST_DUTY 13000

typedef enum
{
    QL_PWM_SUCCESS = 0,
    QL_PWM_EXECUTE_ERR,
    QL_PWM_INVALID_PARAM_ERR,
} ql_pwm_errcode_e;

typedef enum
{
    QL_PWM_0,
    QL_PWM_1,
    QL_PWM_2,
    QL_PWM_3,
    QL_PWM_4,
    QL_PWM_5,
} ql_pwm_channel_e;

ql_pwm_errcode_e ql_pwmInit(ql_pwm_channel_e pwm,ql_gpio_num_e gpio_num,unsigned int period, unsigned int duty_cycle);
ql_pwm_errcode_e ql_pwm_enable(ql_pwm_channel_e pwm);
ql_pwm_errcode_e ql_pwm_disable(ql_pwm_channel_e pwm);
ql_pwm_errcode_e ql_pwm_update_param(ql_pwm_channel_e pwm, unsigned int period, unsigned int duty_cycle);

#endif