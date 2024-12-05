#include "ql_uart.h"
#include "ql_api_osi.h"
#include "ql_debug.h"
#define UART_TEST_DEBUG
#ifdef UART_TEST_DEBUG
#define ql_uart_log os_printf
#else
#endif
#define QL_WAIT_FOROVER 0xFFFFFFFF
#define QL_UART_TEST_PORT_BUFFER 512
/***************uart test******************/
#define UART_TEST_LEN 20
#if 1
#define UART_TEST_PORT QL_UART_PORT_2
#elif
#define UART_TEST_PORT QL_UART_PORT_0
#endif
ql_sem_t uart_rx_test_sema = NULL;
ql_sem_t uart_tx_test_sema = NULL;

void ql_uart_demo_thread(void *param)
{
	int i = 0, ret = 0; // stop_cmd;set 1 enable , set 0 disable
	UINT8 *recv_buf;
	UINT8 *send_buf;
	ql_uart_config_s uart_test_cfg;
	ql_uart_config_s uart_get_cfg;

	ql_rtos_task_sleep_ms(3000);
	recv_buf = os_malloc(UART_TEST_LEN * sizeof(UINT8));
	send_buf = os_malloc(UART_TEST_LEN * sizeof(UINT8));
	if (recv_buf == NULL || send_buf == NULL)
	{
		ql_uart_log(LM_OS, LL_INFO, "buf malloc failed\r\n");
		return;
	}
	memset(recv_buf, 0, UART_TEST_LEN);
	memset(send_buf, 0, UART_TEST_LEN);

	uart_test_cfg.baudrate = QL_UART_BAUD_115200;
	uart_test_cfg.data_bit = QL_UART_DATABIT_8;
	uart_test_cfg.parity_bit = QL_UART_PARITY_NONE;
	uart_test_cfg.stop_bit = QL_UART_STOP_1;
	uart_test_cfg.flow_ctrl = QL_FC_NONE;
	uart_test_cfg.uart_tx_mode = Ql_UART_TX_MODE_POLL;
	uart_test_cfg.uart_rx_mode = Ql_UART_RX_MODE_INTR;
	ql_uart_set_dcbconfig(UART_TEST_PORT, &uart_test_cfg);
	ret = ql_uart_open(UART_TEST_PORT);
	ql_uart_log(LM_OS, LL_INFO, "%d \r\n", ret);

	ret = ql_rtos_semaphore_create(&uart_tx_test_sema, 1);
	if (ret != 0)
	{
		ql_uart_log(LM_OS, LL_INFO, "rtos_init_semaphore err:%d\r\n", ret);
		goto uart_test_exit;
	}

	for (i = 0; i < UART_TEST_LEN; i++)
		send_buf[i] = i;

	ret = ql_uart_write(UART_TEST_PORT, send_buf, 20);
	if (ret == 0)
	{
		ql_rtos_semaphore_release(uart_tx_test_sema);
	}

	ql_uart_log(LM_OS, LL_INFO, "uart send succ\r\n");
	ql_uart_log(LM_OS, LL_INFO, "uart rx test,please enter %d bytes using uart2 \r\n", UART_TEST_LEN);

	ql_rtos_semaphore_wait(uart_tx_test_sema, QL_WAIT_FOROVER);

	while (1)
	{

		if (ql_uart_read(UART_TEST_PORT, recv_buf, UART_TEST_LEN, QL_WAIT_FOROVER) == UART_TEST_LEN)
		{
			break;
		}
		ql_rtos_task_sleep_ms(10);
	}
	ql_uart_log(LM_OS, LL_INFO, "recv_buf:\r\n");
	for (i = 0; i < UART_TEST_LEN; i++)
	{
		ql_uart_log(LM_OS, LL_INFO, "[%d] =0x%x", i, *recv_buf);
		recv_buf++;
	}
	ql_uart_log(LM_OS, LL_INFO, "\r\n");
	ql_uart_get_dcbconfig(UART_TEST_PORT, &uart_get_cfg);

	ql_uart_log(LM_OS, LL_INFO, "uart get config:\r\n");
	ql_uart_log(LM_OS, LL_INFO, "uart baudrate:%d\r\n", uart_get_cfg.baudrate);
	ql_uart_log(LM_OS, LL_INFO, "uart data_bit:%d\r\n", uart_get_cfg.data_bit);
	ql_uart_log(LM_OS, LL_INFO, "uart stop_bit:%d\r\n", uart_get_cfg.stop_bit);
	ql_uart_log(LM_OS, LL_INFO, "uart parity_bit:%d\r\n", uart_get_cfg.parity_bit);
	ql_uart_log(LM_OS, LL_INFO, "uart flow_ctrl:%d\r\n", uart_get_cfg.flow_ctrl);
	ql_uart_log(LM_OS, LL_INFO, "\r\n");
	ql_uart_log(LM_OS, LL_INFO, "uart  test over \r\n");
	ql_uart_close(UART_TEST_PORT);

uart_test_exit:

	if (uart_rx_test_sema)
		ql_rtos_semaphore_delete(uart_rx_test_sema);
	if (uart_tx_test_sema)
		ql_rtos_semaphore_delete(uart_tx_test_sema);
	while (1)
	{
		ql_rtos_task_sleep_ms(1000);
	}
}

ql_task_t uart_test_thread_handle = NULL;
void ql_uart_demo_thread_creat(void)
{
	int ret;
	ret = ql_rtos_task_create(&uart_test_thread_handle,
							  (unsigned short)1024 * 4,
							  RTOS_HIGHER_PRIORTIY_THAN(5),
							  "uart_test",
							  ql_uart_demo_thread,
							  0);

	if (ret != QL_OSI_SUCCESS)
	{
		os_printf(LM_OS, LL_INFO, "Error: Failed to create uart test thread: %d\r\n", ret);
		goto init_err;
	}

	return;

init_err:
	if (uart_test_thread_handle != NULL)
	{
		ql_rtos_task_delete(uart_test_thread_handle);
	}
}

static int ql_cmd_uart_test(cmd_tbl_t *t, int argc, char *argv[])
{
	uint8_t uart_num = 0;
	char *uart_mode = argv[1];
	uart_num = strtoul(argv[2], NULL, 0);
	uint8_t recv_buf[UART_TEST_LEN] = {0};
	uint8_t send_buf[UART_TEST_LEN] = {0};
	if (strcmp("set", uart_mode) == 0)
	{
		if (argc != 10)
		{
			return CMD_RET_FAILURE;
		}
		ql_uart_config_s uart_test_cfg = {0};
		uart_test_cfg.baudrate = strtoul(argv[3], NULL, 0);
		uart_test_cfg.data_bit = strtoul(argv[4], NULL, 0);
		uart_test_cfg.parity_bit = strtoul(argv[5], NULL, 0);
		uart_test_cfg.stop_bit = strtoul(argv[6], NULL, 0);
		uart_test_cfg.flow_ctrl = strtoul(argv[7], NULL, 0);
		uart_test_cfg.uart_tx_mode = strtoul(argv[8], NULL, 0);
		uart_test_cfg.uart_rx_mode = strtoul(argv[9], NULL, 0);
		ql_uart_set_dcbconfig(uart_num, &uart_test_cfg);
		ql_uart_open(uart_num);
		for (int i = 0; i < UART_TEST_LEN; i++)
		{
			send_buf[i] = i;
		}
		ql_uart_write(uart_num, send_buf, 20);
	}
	else if (strcmp("recv", uart_mode) == 0)
	{
		while (1)
		{

			if (ql_uart_read(uart_num, recv_buf, UART_TEST_LEN, QL_WAIT_FOROVER) == UART_TEST_LEN)
			{
				break;
			}
			ql_rtos_task_sleep_ms(10);
		}
		for (int i = 0; i < UART_TEST_LEN; i++)
		{
			ql_debug("recv_buf[%d] =0x%x\r\n", i, recv_buf[i]);
		}
	}
	else if (strcmp("get", uart_mode) == 0)
	{
		if (argc != 3)
		{
			return CMD_RET_FAILURE;
		}
		ql_uart_config_s uart_get_cfg;

		ql_uart_get_dcbconfig(uart_num, &uart_get_cfg);

		ql_uart_log(LM_OS, LL_INFO, "uart get config:\r\n");
		ql_uart_log(LM_OS, LL_INFO, "uart baudrate:%d\r\n", uart_get_cfg.baudrate);
		ql_uart_log(LM_OS, LL_INFO, "uart data_bit:%d\r\n", uart_get_cfg.data_bit);
		ql_uart_log(LM_OS, LL_INFO, "uart stop_bit:%d\r\n", uart_get_cfg.stop_bit);
		ql_uart_log(LM_OS, LL_INFO, "uart parity_bit:%d\r\n", uart_get_cfg.parity_bit);
		ql_uart_log(LM_OS, LL_INFO, "uart flow_ctrl:%d\r\n", uart_get_cfg.flow_ctrl);
		ql_uart_log(LM_OS, LL_INFO, "\r\n");
		ql_uart_log(LM_OS, LL_INFO, "uart  test over \r\n");
	}
	else
	{
		return CMD_RET_FAILURE;
	}
	return CMD_RET_SUCCESS;
}
CLI_CMD_F(ql_uart, ql_cmd_uart_test, "ql_cmd_uart_test", "ql_cmd_uart_test", true);