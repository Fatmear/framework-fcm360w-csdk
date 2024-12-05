#include <stdio.h>
#include "oshal.h"
#include "ql_gpio.h"
#include "ql_lowpower.h"
#include "ql_api_osi.h"
#include "ql_wlan.h"
#include "system_wifi.h"
#include "cli.h"
#include "arch_irq.h"
ql_task_t lowpower_test_thread_handle = NULL;
// signed int ql_psm_wakeup_func_cb(void)
// {
// 	signed int ret = true;
// 	os_printf(LM_APP,LL_INFO," lowpower demo exit !\r\n ");
// 	return ret;
// }

// signed int ql_psm_sleep_func_cb(void)
// {
// 	signed int ret = true;
// 	os_printf(LM_APP,LL_INFO," lowpower demo enter !\r\n ");
// 	return ret;
// }
extern int psm_clear_pcu_isr();
void psm_usr_disable_func()
{
    os_printf(LM_OS, LL_INFO, "*********************\n");
    os_printf(LM_OS, LL_INFO, "********wake up******\n");
    os_printf(LM_OS, LL_INFO, "*********************\n");
}
void ql_lowpower_demo_thread(void *parpam)
{
    while (1)
    {
        // ql_wlan_get_link_status(&outStatus);
        if (wifi_get_sta_status() == 5)
        {
            break;
        }
        ql_rtos_task_sleep_ms(3000);
    }

    ql_rtos_task_sleep_ms(20000);
    ql_psm_set_sleep_mode(QL_Modem_Sleep, 1);
    extern void psm_at_callback(void_fn cb);
    psm_at_callback(psm_usr_disable_func);
    while (1)
    {
        ql_rtos_task_sleep_ms(1000);
    }
    ql_rtos_task_delete(lowpower_test_thread_handle);
}
void ql_lowpower_demo_thread_creat(void)
{
    // QlWlanLinkStatusTypeDef_st outStatus = {0};
    ql_rtos_task_sleep_ms(3000);
    int ret;
    ret = ql_rtos_task_create(&lowpower_test_thread_handle,
                              (unsigned short)1024 * 4,
                              RTOS_HIGHER_PRIORTIY_THAN(3),
                              "lowpower_test",
                              ql_lowpower_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO, "Error: Failed to create spi test thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (lowpower_test_thread_handle != NULL)
    {
        ql_rtos_task_delete(lowpower_test_thread_handle);
    }
}

static int ql_psm_set_sleep(cmd_tbl_t *t, int argc, char *argv[])
{
    unsigned int sleep_mode = strtoul(argv[1], NULL, 0);
    unsigned int listen_interval = strtoul(argv[2], NULL, 0);

    ql_psm_set_sleep_mode(sleep_mode, listen_interval);
    os_printf(LM_APP, LL_INFO, "set lightsleep gpio=%s\r\n", argv[1]);
    return CMD_RET_SUCCESS;
}
CLI_CMD_F(ql_psm_sleep, ql_psm_set_sleep, "ql_psm_set_sleep", "ql_psm_set_sleep",true);