#include "ql_api_osi.h"
#include "ql_debug.h"
#include "ql_wdt.h"



/***************wdg test******************/
ql_task_t	wdg_test_thread_handle = NULL;
void ql_wdg_demo_thread(void *param)
{
    int timer  = 2000;
    int count = 0;
	ql_wdg_init(timer);
    // ql_wdg_start();


	while(1)
	{
	    ql_wdg_reload();
		ql_debug("wdg feed %d\r\n",count++);
		ql_rtos_task_sleep_ms(1000);
        if(count >1000)
        {
            ql_wdg_finalize();
            break;
        }
	}
    if(wdg_test_thread_handle != NULL)
    {
        ql_rtos_task_delete(wdg_test_thread_handle);
        wdg_test_thread_handle = NULL;
    }

}
void ql_wdg_demo_thread_creat(void)
{
    int ret;
    ret = ql_rtos_task_create(&wdg_test_thread_handle,
                              (unsigned short)2048,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "wdt_test",
                              ql_wdg_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO,"Error: Failed to create pwm test thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (wdg_test_thread_handle != NULL)
    {
        ql_rtos_task_delete(wdg_test_thread_handle);
    }
}

static int ql_cmd_wdt_test(cmd_tbl_t *t, int argc, char *argv[])
{
    char *wdt_key ;
    wdt_key = argv[1];
    unsigned int  wdt_time = 0;
    if(strcmp("init", wdt_key)==0)
    {
        if(argc != 3)
        {
            return CMD_RET_FAILURE;
        }
        wdt_time = (int)strtoul(argv[2], NULL, 0);
        ql_wdg_init(wdt_time);
    }
    // else if(strcmp("start", wdt_key)==0)
    // {
    //     ql_wdt_start();
    // }
    else if(strcmp("feed", wdt_key)==0)
    {
        ql_wdg_reload();
    }
    else if(strcmp("stop", wdt_key)==0)
    {
       ql_wdg_finalize();
    }
	return CMD_RET_SUCCESS;
}
CLI_CMD_F(ql_wdt,ql_cmd_wdt_test,"ql_cmd_wdt_test",  "ql_cmd_wdt_test",true);

