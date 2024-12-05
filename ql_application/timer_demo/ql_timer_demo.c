/********************/

#include "ql_timer.h"
#include "ql_debug.h"
#include "ql_api_osi.h"
#include "ql_gpio.h"
#define TIMER_TEST_DEBUG
#ifdef TIMER_TEST_DEBUG
#define ql_timer_log  os_printf

#else

#endif
#define TIMER_TEST_ID QL_TIMER_0
/***************timer test******************/

void ql_timer_test_isr_cb(void *arg)
{
    ql_gpio_set_level_reverse(QL_GPIO21);
	ql_debug("timer test isr\r\n");

}

void ql_timer_demo_thread(void *param)
{
	ql_timer_log(LM_OS, LL_INFO,"timer test start~\r\n");
	UINT32 ret;
	ql_rtos_task_sleep_ms(3000);
	ql_timer_dev dev;
	ql_gpio_init(QL_GPIO21, QL_GMODE_OUTPUT,QL_GPIO_OUTPUT_LOW);
    ql_gpio_set_level(QL_GPIO21, QL_GPIO_OUTPUT_LOW);

	ret = ql_TimerInit(TIMER_TEST_ID,1000,ql_timer_test_isr_cb,&dev,0);

    if(ret == QL_TIMER_SUCCESS )
	 ql_timer_log(LM_OS, LL_INFO,"timer init success\r\n");
	else
	 ql_timer_log(LM_OS, LL_INFO,"timer init fail\r\n");

	ql_TimerStart(TIMER_TEST_ID);

	ql_rtos_task_sleep_ms(20010);

	ql_TimerStop(TIMER_TEST_ID);

    if(ret == QL_TIMER_SUCCESS )
	 ql_timer_log(LM_OS, LL_INFO,"timer stop success\r\n");
	else
	 ql_timer_log(LM_OS, LL_INFO,"timer stop fail\r\n");

	while(1)
	{
	  ql_rtos_task_sleep_ms(1000);
	}

}

ql_task_t	timer_test_thread_handle = NULL;
void ql_timer_demo_thread_creat(void)
{
   int ret;
   ret = ql_rtos_task_create(&timer_test_thread_handle,
            	(unsigned short)4096,
				 RTOS_HIGHER_PRIORTIY_THAN(5),
				 "timer_test",
				 ql_timer_demo_thread,
				 0);


	if (ret != QL_OSI_SUCCESS) {
	os_printf(LM_OS, LL_INFO,"Error: Failed to create timer test thread: %d\r\n",ret);
	goto init_err;
	}

	 return;

	 init_err:
	  if( timer_test_thread_handle != NULL ) {
		  ql_rtos_task_delete(timer_test_thread_handle);
	  }

}
static int ql_cmd_timer_test(cmd_tbl_t *t, int argc, char *argv[])
{
    char *timer_key ;
    timer_key = argv[1];
    int timer_num = (int)strtoul(argv[2], NULL, 0);
    int timer_period = (int)strtoul(argv[3], NULL, 0);
	if(strcmp("init", timer_key)==0)
	{
		ql_gpio_init(QL_GPIO21, QL_GMODE_OUTPUT,QL_GPIO_OUTPUT_LOW);
    	ql_gpio_set_level(QL_GPIO21, QL_GPIO_OUTPUT_LOW);
		ql_timer_dev dev;
        ql_TimerInit(timer_num, timer_period,ql_timer_test_isr_cb,&dev,0);
	}
    else if(strcmp("start", timer_key)==0)
    {
        if(argc != 3)
        {
            return CMD_RET_FAILURE;
        }
        ql_TimerStart(timer_num);
    }
    else if(strcmp("stop", timer_key)==0)
    {
        if(argc != 3)
        {
            return CMD_RET_FAILURE;
        }
        ql_TimerStop(timer_num);
    }
	else if(strcmp("change", timer_key)==0)
	{
		ql_TimerChangePeriod(timer_num,timer_period);
	}
	else if(strcmp("del", timer_key)==0)
    {
        if(argc != 3)
        {
            return CMD_RET_FAILURE;
        }
        ql_timer_delete(timer_num);
    }
	return CMD_RET_SUCCESS;
}
CLI_CMD_F(ql_timer, ql_cmd_timer_test,"ql_cmd_timer_test",  "ql_cmd_timer_test",true);