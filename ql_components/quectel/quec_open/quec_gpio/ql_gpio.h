#ifndef _QL_GPIO_H_
#define _QL_GPIO_H_
#include <stdint.h>

typedef struct _Ql_GPIO_ISR_CALLBACK_t
{
	void (* gpio_callback)(void *);
	void *gpio_data;
}Ql_GPIO_ISR_CALLBACK_TypeDef_st;

typedef void (*ql_gpio_irq_callback)(void *arg);

typedef enum
{
    QL_GPIO_SUCCESS = 0,
    QL_GPIO_EXECUTE_ERR,
    QL_GPIO_INVALID_PARAM_ERR,
} ql_gpio_errcode_e;

typedef enum
{
    QL_GPIO0 = 0,
    QL_GPIO1 = 1,
    QL_GPIO2 = 2,
    QL_GPIO3 = 3,
    QL_GPIO4 = 4,
    QL_GPIO5 = 5,
    QL_GPIO6 = 6,
    QL_GPIO7 = 7,
    QL_GPIO8 = 8,
    QL_GPIO9 = 9,
    QL_GPIO10 = 10,
    QL_GPIO11 = 11,
    QL_GPIO12 = 12,
    QL_GPIO13 = 13,
    QL_GPIO14 = 14,
    QL_GPIO15 = 15,
    QL_GPIO16 = 16,
    QL_GPIO17 = 17,
    QL_GPIO18 = 18,
    QL_GPIO19 = 19,
    QL_GPIO20 = 20,
    QL_GPIO21 = 21,
    QL_GPIO22 = 22,
    QL_GPIO23 = 23,
    QL_GPIO24 = 24,
    QL_GPIO25 = 25,
} ql_gpio_num_e;

// typedef enum
// {
//     QL_GMODE_INPUT_PULLDOWN = 0,
//     QL_GMODE_OUTPUT,
//     QL_GMODE_SECOND_FUNC,
//     QL_GMODE_INPUT_PULLUP,
//     QL_GMODE_INPUT,
//     QL_GMODE_SECOND_FUNC_PULL_UP, // Special for uart1
//     QL_GMODE_OUTPUT_PULLUP,
//     QL_GMODE_SET_HIGH_IMPENDANCE,
// } ql_gpio_mode_e;
typedef enum{  
    QL_GMODE_HIGH_IMPENDANCE = 0,
    QL_GMODE_INPUT,
    QL_GMODE_IN_FLOATING,
    QL_GMODE_IN_PD,
    QL_GMODE_IN_PU,
    QL_GMODE_OUTPUT,
    QL_GMODE_OUT_OD,
    QL_GMODE_OUT_PP,
}ql_gpio_mode_e;
typedef enum
{
    QL_GPIO_OUTPUT_LOW = 0,
    QL_GPIO_OUTPUT_HIGH,

} ql_gpio_level_e;

typedef enum
{
    Ql_GPIO_ARG_INTR_MODE_NOP = 0,
    Ql_GPIO_ARG_INTR_MODE_HIGH = 2,
    Ql_GPIO_ARG_INTR_MODE_LOW = 3 ,
    Ql_GPIO_ARG_INTR_MODE_N_EDGE = 5,
    Ql_GPIO_ARG_INTR_MODE_P_EDGE = 6,
    Ql_GPIO_ARG_INTR_MODE_DUAL_EDGE = 7,
} ql_gpio_irq_trigger_e;

ql_gpio_errcode_e ql_gpio_init(ql_gpio_num_e gpio_num, ql_gpio_mode_e mode, ql_gpio_level_e level);
ql_gpio_errcode_e ql_gpio_set_level(ql_gpio_num_e gpio_num, ql_gpio_level_e output_level);
ql_gpio_errcode_e ql_gpio_set_level_reverse(ql_gpio_num_e gpio_num);
ql_gpio_errcode_e ql_gpio_get_level(ql_gpio_num_e gpio_num, uint32_t *input_level);
ql_gpio_errcode_e ql_gpio_int_init(ql_gpio_num_e gpio_num, ql_gpio_irq_trigger_e trigger, ql_gpio_irq_callback calback);
ql_gpio_errcode_e ql_gpio_int_disable(ql_gpio_num_e gpio_num);

#endif
