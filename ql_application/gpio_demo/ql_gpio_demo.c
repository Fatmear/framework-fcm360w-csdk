
#include "ql_debug.h"
#include "ql_gpio.h"
#include "ql_api_osi.h"
#define GPIO_TEST_DEBUG
#ifdef  GPIO_TEST_DEBUG
#define ql_gpio_log     os_printf

#else

#endif
/***************gpio test******************/
static void gpio_init_test_callback(void *arg)
{
    ql_gpio_errcode_e ret;

    ql_gpio_log(LM_OS, LL_INFO,"gpio intr test\r\n");
    ret = ql_gpio_set_level_reverse(QL_GPIO21);
    if (ret == QL_GPIO_SUCCESS)
        ql_gpio_log(LM_OS, LL_INFO,"gpio reverse success\r\n");
    else
        ql_gpio_log(LM_OS, LL_INFO,"gpio reverse fail\r\n");
}

void ql_gpio_demo_thread(void *param)
{
    ql_gpio_errcode_e ret;

    ql_rtos_task_sleep_ms(5000);

    ret = ql_gpio_init(QL_GPIO21, QL_GMODE_OUTPUT,QL_GPIO_OUTPUT_HIGH);
    if (ret == QL_GPIO_SUCCESS)
        ql_gpio_log(LM_OS, LL_INFO,"gpio init success\r\n");
    else
        ql_gpio_log(LM_OS, LL_INFO,"gpio init fail\r\n");

    ql_gpio_set_level(QL_GPIO21, QL_GPIO_OUTPUT_LOW);
    ql_rtos_task_sleep_ms(10);
    ql_gpio_set_level(QL_GPIO21, QL_GPIO_OUTPUT_HIGH);
    ql_gpio_int_init(QL_GPIO22, Ql_GPIO_ARG_INTR_MODE_P_EDGE, gpio_init_test_callback);

    ql_gpio_log(LM_OS, LL_INFO,"gpio  test start \r\n");

    while (1)
    {
        ql_rtos_task_sleep_ms(1000);
    }
}

ql_task_t gpio_test_thread_handle = NULL;
void ql_gpio_demo_thread_creat(void)
{
    int ret;
    ret = ql_rtos_task_create(&gpio_test_thread_handle,
                              (unsigned short)2048,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "gpio_test",
                              ql_gpio_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO,"Error: Failed to create gpio test thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (gpio_test_thread_handle != NULL)
    {
        ql_rtos_task_delete(gpio_test_thread_handle);
    }
}

static int ql_cmd_gpio_test(cmd_tbl_t *t, int argc, char *argv[])
{
    char *gpio_key ;
    gpio_key = argv[1];
    uint8_t gpio_num = 0;
    uint8_t gpio_mode = 0;
    if(strcmp("direct", gpio_key)==0)
    {
        if(argc != 5)
        {
            return CMD_RET_FAILURE;
        }
        gpio_num = strtoul(argv[2], NULL, 0);
        gpio_mode = strtoul(argv[3], NULL, 0);
        uint8_t gpio_value = strtoul(argv[4], NULL, 0);
        ql_gpio_init(gpio_num, gpio_mode,gpio_value);
        ql_gpio_set_level(gpio_num, gpio_value);
    }
    else if(strcmp("intr", gpio_key)==0)
    {
        if(argc != 4)
        {
            return CMD_RET_FAILURE;
        }
        gpio_num = strtoul(argv[2], NULL, 0);
        gpio_mode = strtoul(argv[3], NULL, 0);
        ql_gpio_int_init(gpio_num, gpio_mode, gpio_init_test_callback);
    }
	return CMD_RET_SUCCESS;
}
CLI_CMD_F(ql_gpio, ql_cmd_gpio_test,"ql_cmd_gpio_test",  "ql_cmd_gpio_test",true);