#include "ql_uart.h"
#include "ql_api_osi.h"
#include "ql_debug.h"
#include "string.h"
#include "stdlib.h"
#include "ql_ota.h"
/***************uart test******************/
#if 1
#define UART_USER_TEST_PORT QL_UART_PORT_0
#elif
#define UART_TEST_PORT QL_UART_PORT_0
#endif
#define UART_BUFFER 513
unsigned int OTA_len = 0; // OTA包大小
unsigned char g_uart_buf[UART_BUFFER];
int g_uart_num = 0;
unsigned int ota_len = 0;
static void uart2_rx_irq_handler(void *data)
{
    uint8_t c;
    unsigned int uart_base_reg = MEM_BASE_UART0;
    while (ql_uart_rx_tstc(uart_base_reg))
    {
        c = (uint8_t)ql_uart_rx_getc(uart_base_reg);
        g_uart_buf[g_uart_num++] = c;
        ota_len++;
        if (g_uart_num >= 512 || ota_len >= OTA_len)
        {
            break;
        }
    }
    if (g_uart_num >= 512)
    {
        // os_printf(LM_OS, LL_INFO, "%s\r\n", g_uart_buf);
        ql_ota_write((unsigned char *)g_uart_buf, 512);
        memset(g_uart_buf, '\0', sizeof(g_uart_buf));
        g_uart_num = 0;
    }
    else if (ota_len >= OTA_len)
    {
        ql_ota_write((unsigned char *)g_uart_buf, g_uart_num);
        g_uart_num = 0xffff;
    }
}

void ql_uart_user_demo_thread(void *param)
{
    ql_uart_config_s uart_test_cfg;
    ql_rtos_task_sleep_ms(3000);

    os_printf(LM_OS, LL_INFO, "UART CTRL FL OTA START 1\r\n");
    uart_test_cfg.baudrate = QL_UART_BAUD_115200;
    uart_test_cfg.data_bit = QL_UART_DATABIT_8;
    uart_test_cfg.parity_bit = QL_UART_PARITY_NONE;
    uart_test_cfg.stop_bit = QL_UART_STOP_1;
    uart_test_cfg.flow_ctrl = QL_FC_HW_RTS_CTS;
    uart_test_cfg.uart_tx_mode = Ql_UART_TX_MODE_POLL;
    uart_test_cfg.uart_rx_mode = Ql_UART_RX_MODE_USER;
    uart_test_cfg.uart_rx_buf_size = 1024;
    ql_uart_set_dcbconfig(UART_USER_TEST_PORT, &uart_test_cfg);
    ql_uart_open(UART_USER_TEST_PORT);
    ql_uart_callback arg;
    arg.uart_callback = uart2_rx_irq_handler;
    ql_uart_set_rx_cb(UART_USER_TEST_PORT, &arg);
    ql_ota_init();
    while (1)
    {
        if (g_uart_num == 0xffff)
        {
            g_uart_num = 0;
            ql_ota_done(1);
        }
        ql_rtos_task_sleep_ms(3000);
    }
}
ql_task_t uart_user_test_thread_handle = NULL;
void ql_uart_user_demo_thread_creat(void)
{
    int ret;
    ret = ql_rtos_task_create(&uart_user_test_thread_handle,
                              (unsigned short)1024 * 4,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "uart_test",
                              ql_uart_user_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO, "Error: Failed to create uart test thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (uart_user_test_thread_handle != NULL)
    {
        ql_rtos_task_delete(uart_user_test_thread_handle);
    }
}

static int ql_cmd_set_ota_len(cmd_tbl_t *t, int argc, char *argv[])
{
    if (argv[1] != NULL)
    {
        OTA_len = atoi(argv[1]);
        os_printf(LM_OS, LL_INFO, "\r\nOTA_len=%d\r\n", OTA_len);
    }
    return 0;
}

CLI_CMD(ql_set_ota_len, ql_cmd_set_ota_len, "set ota len", "set ota len");