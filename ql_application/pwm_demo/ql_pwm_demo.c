#include "ql_api_osi.h"
#include "ql_pwm.h"
#include "ql_gpio.h"
#include "ql_debug.h"

#define PWM_TEST_PERIOD 1000
#define PWM_TEST_DEBUG
#ifdef PWM_TEST_DEBUG
#define ql_pwm_log               os_printf

#else
#endif
ql_task_t pwm_test_thread_handle = NULL;
void ql_pwm_demo_thread(void *param)
{
    int ret;
    uint8_t dir = 1;
    int pause = 0;
    uint8_t cnt = 0;
    ql_rtos_task_sleep_ms(3000);

    ret = ql_pwmInit(QL_PWM_0, QL_GPIO22,PWM_TEST_PERIOD, pause);
    if (ret == QL_PWM_SUCCESS)
        ql_pwm_log(LM_OS, LL_INFO,"pwm init success\r\n");
    else
    {
        ql_pwm_log(LM_OS, LL_INFO,"pwm init fail\r\n");
        goto pwm_test_exit;
    }

    ql_pwm_enable(QL_PWM_0);

pwm_test_exit:

    while (1)
    {
        ql_rtos_task_sleep_ms(5);
        if(dir)
        {
            pause += 100;
        }
        else
        {
            pause -= 100;
        }
        if(pause >= 1000)
        {
            dir = 0;
        }
        if(pause == 0)
        {
            dir = 1;
            cnt++;
        }
        ql_pwm_update_param(QL_PWM_0, PWM_TEST_PERIOD, pause);
        if(cnt >= 10)
        {
            break;
        }
    }
    ql_pwm_disable(QL_PWM_0);
    ql_pwm_log(LM_OS, LL_INFO,"pwm  test over \r\n");
    ql_rtos_task_delete(pwm_test_thread_handle);
}
void ql_pwm_demo_thread_creat(void)
{
    int ret;
    ret = ql_rtos_task_create(&pwm_test_thread_handle,
                              (unsigned short)2048,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "pwm_test",
                              ql_pwm_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO,"Error: Failed to create pwm test thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (pwm_test_thread_handle != NULL)
    {
        ql_rtos_task_delete(pwm_test_thread_handle);
    }
}
static int ql_cmd_pwm_test(cmd_tbl_t *t, int argc, char *argv[])
{
    char *pwm_key ;
    pwm_key = argv[1];
    int pwm_num = 0;
    int pwm_gpio = 0;
    unsigned int  pwm_period = 0;
    unsigned int pwm_pause = 0;
    if(strcmp("init", pwm_key)==0)
    {
        if(argc != 6)
        {
            return CMD_RET_FAILURE;
        }
        pwm_num = (int)strtoul(argv[2], NULL, 0);
        pwm_gpio = (int)strtoul(argv[3], NULL, 0);
        pwm_period = (int)strtoul(argv[4], NULL, 0);
        pwm_pause = (int)strtoul(argv[5], NULL, 0);
        ql_pwmInit(pwm_num, pwm_gpio,pwm_period, pwm_pause);
    }
    else if(strcmp("start", pwm_key)==0)
    {
        if(argc != 3 )
        {
            return CMD_RET_FAILURE;
        }
        pwm_num = (int)strtoul(argv[2], NULL, 0);
        ql_pwm_enable(pwm_num);
    }
    else if(strcmp("update", pwm_key)==0)
    {
        if(argc != 5 )
        {
            return CMD_RET_FAILURE;
        }
        pwm_num = (int)strtoul(argv[2], NULL, 0);
        pwm_period = (int)strtoul(argv[3], NULL, 0);
        pwm_pause = (int)strtoul(argv[4], NULL, 0);
        ql_pwm_update_param(pwm_num, pwm_period, pwm_pause);
    }
    else if(strcmp("stop", pwm_key)==0)
    {
        if(argc != 3)
        {
            return CMD_RET_FAILURE;
        }
        pwm_num = (int)strtoul(argv[2], NULL, 0);
        ql_pwm_disable(pwm_num);
    }
	return CMD_RET_SUCCESS;
}
CLI_CMD_F(ql_pwm, ql_cmd_pwm_test,"ql_cmd_pwm_test",  "ql_cmd_pwm_test",true);