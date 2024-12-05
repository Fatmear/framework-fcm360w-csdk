
#include "ql_api_osi.h"
#include "oshal.h"
#include "ql_flash.h"
#include "cli.h"
#define TEST_FLASH_LEN 256
#define TEST_FLASH_ADDR 0x1F7000 // 固定地址 最大0x4000长度
#define FLASH_TEST_MAX_LAN 0x4000
#define TEST_FLASH_ERASE_LEN    4096
static ql_task_t flash_test_thread_handle_1 = NULL;
static ql_task_t flash_test_thread_handle_2 = NULL;
void ql_flash_demo_thread_1(void *param)
{
    ql_rtos_task_sleep_ms(3000);
    uint8_t *buf = malloc(TEST_FLASH_LEN);
    if (buf == NULL)
    {
        goto flash_test_exit;
    }

    for (int i = 0; i < TEST_FLASH_LEN; i++)
        buf[i] = i % 256;
    int ret = ql_flash_write(TEST_FLASH_ADDR, buf, TEST_FLASH_LEN);
    if (ret == 0)
    {
        os_printf(LM_APP, LL_INFO, "flash write succ\r\n");
    }
    memset(buf, 0, TEST_FLASH_LEN);
    ret = ql_flash_read(TEST_FLASH_ADDR, buf, TEST_FLASH_LEN);
    if (ret == 0)
    {
        os_printf(LM_APP, LL_INFO, "flash read succ\r\n");
        for (int i = 0; i < TEST_FLASH_LEN; i++)
        {
            os_printf(LM_APP, LL_INFO, "%02x,", buf[i]);
            if ((i + 1) % 32 == 0)
                os_printf(LM_APP, LL_INFO, "\r\n");
        }
    }
    ret = ql_flash_erase(TEST_FLASH_ADDR, TEST_FLASH_ERASE_LEN);
    if (ret == 0)
    {
        os_printf(LM_APP, LL_INFO, "flash erase succ\r\n");
        ret = ql_flash_read(TEST_FLASH_ADDR, buf, TEST_FLASH_LEN);
        if (ret == 0)
        {
            os_printf(LM_APP, LL_INFO, "flash read succ\r\n");
            for (int i = 0; i < TEST_FLASH_LEN; i++)
            {
                os_printf(LM_APP, LL_INFO, "%02x,", buf[i]);
                if ((i + 1) % 32 == 0)
                    os_printf(LM_APP, LL_INFO, "\r\n");
            }
        }
    }
flash_test_exit:
    if (buf != NULL)
    {
        os_free(buf);
    }
    while (1)
    {
        ql_rtos_task_sleep_ms(1000);
    }
}
void ql_flash_demo_thread_2(void *param)
{
    ql_rtos_task_sleep_ms(10000);
    os_printf(LM_APP, LL_INFO, "flash parttion write start\r\n");
    char *parttion_buf = "mcu_wifi_test_1";
    int ret = ql_flash_cus_parttion_write("custtest", parttion_buf, strlen(parttion_buf));
    if (ret == 0)
    {
        os_printf(LM_APP, LL_INFO, "flash parttion write succ\r\n");
    }
    else
    {
        os_printf(LM_APP, LL_INFO, "flash parttion write fail [%d]\r\n",ret);
    }
    while (1)
    {
        ql_rtos_task_sleep_ms(1000);
    }
}
void ql_flash_demo_thread_creat(void)
{
    int ret;
#if 1
    ret = ql_rtos_task_create(&flash_test_thread_handle_1,
                              (unsigned short)4096,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "flash_test_1",
                              ql_flash_demo_thread_1,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO, "Error: Failed to create flash test thread: %d\r\n", ret);
        goto init_err;
    }
#elif 0
    ret = ql_rtos_task_create(&flash_test_thread_handle_2,
                              (unsigned short)4096,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "flash_test_2",
                              ql_flash_demo_thread_2,
                              0);

    if (ret != TRUE)
    {
        os_printf(LM_OS, LL_INFO, "Error: Failed to create flash test thread: %d\r\n", ret);
        goto init_err;
    }
#endif
    return;

init_err:
    if (flash_test_thread_handle_1 != NULL)
    {
        ql_rtos_task_delete(flash_test_thread_handle_1);
    }
    if(flash_test_thread_handle_2 == NULL)
    {
        ql_rtos_task_delete(flash_test_thread_handle_2);
    }
}