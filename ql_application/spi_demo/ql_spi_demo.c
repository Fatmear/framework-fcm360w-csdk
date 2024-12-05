#include <stdio.h>
#include "oshal.h"
#include "ql_spi.h"
#include "ql_api_osi.h"
#include "ql_wdt.h"

#define CFG_SPI_MASTER_DEMO 1
#define SPI_TEST_LEN 256
/***************spi test******************/

ql_task_t spi_rx_test_thread_handle = NULL;
unsigned char fsend[SPI_TEST_LEN] = {0};
unsigned char frecv[SPI_TEST_LEN] = {0};

extern void *spi_slave_rx_process;

#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')

void dump_hex_printf(void *_buf, uint32_t size, uint32_t number)
{

    int i, j;
    char buffer[256] = {0};
    int n = 0;
    uint8_t *buf = _buf;
    for (i = 0; i < size; i += number)
    {
        n += snprintf(buffer + n, sizeof(buffer) - n - 1, "%08X: ", i);
        for (j = 0; j < number; j++)
        {
            if (j % 8 == 0)
            {
                n += snprintf(buffer + n, sizeof(buffer) - n - 1, " ");
            }
            if (i + j < size)
                n += snprintf(buffer + n, sizeof(buffer) - n - 1, "%02X ", buf[i + j]);
            else
                n += snprintf(buffer + n, sizeof(buffer) - n - 1, "   ");
        }
        n += snprintf(buffer + n, sizeof(buffer) - n - 1, " ");

        for (j = 0; j < number; j++)
        {
            if (i + j < size)
            {
                n += snprintf(buffer + n, sizeof(buffer) - n - 1, "%c", __is_print(buf[i + j]) ? buf[i + j] : '.');
            }
        }
        n += snprintf(buffer + n, sizeof(buffer) - n - 1, "\n");
        os_printf(LM_APP, LL_INFO, "%s", buffer);
        n = 0;
        memset(buffer, 0, sizeof(buffer));
    }
}

#if CFG_SPI_MASTER_DEMO
ql_spi_master_interface_config_st spi_master_dev_config =
    {
        .addr_len = 3,
        .data_len = 8,
        .spi_clk_pol = 0,
        .spi_clk_pha = 0,
        .spi_trans_mode = QL_SPI_MODE_STANDARD,
        .master_clk = 10,
        .addr_pha_enable = 0,
        .cmd_read = QL_SPI_TRANSCTRL_TRAMODE_DR | QL_SPI_TRANSCTRL_CMDEN,
        .cmd_write = QL_SPI_TRANSCTRL_TRAMODE_DW | QL_SPI_TRANSCTRL_CMDEN,
        .dummy_bit = QL_SPI_TRANSCTRL_DUMMY_CNT_1,
        .spi_dma_enable = 1};

#else
ql_spi_slave_interface_config_t spi_slave_dev =
    {
        .addr_len = 3,
        .data_len = 8,
        .spi_clk_pol = 0,
        .spi_clk_pha = 0,
        .slave_clk = 1,

        .inten = 0x20,
        .spi_slave_dma_enable = 0
        // .inten = 0x30,
        // .spi_slave_dma_enable = 1,
};
#endif

void ql_spi_tx_demo_thread(void *param)
{

#if CFG_SPI_MASTER_DEMO

    ql_spi_master_init(&spi_master_dev_config);

    ql_spi_transaction_st config =
        {
            .cmmand = 0x51,
            .addr = 0x0,
            .length = SPI_TEST_LEN,
        };

    int ret = 0;
    do
    {
        config.cmmand = 0x51;
        memset(fsend, 'B', SPI_TEST_LEN);
        fsend[SPI_TEST_LEN - 1] = 0;
        for (size_t i = 0; i < SPI_TEST_LEN; i++)
        {
            fsend[i] = i;
        }
        config.length = 256;
        fsend[0] = 'B';
        ql_spi_master_write(&config, fsend);

        os_printf(LM_OS, LL_INFO, "spi master fsend ok\r\n");
    
        ql_rtos_task_sleep_ms(1000);
        
        memset(frecv, 0, SPI_TEST_LEN);
        config.cmmand = 0x0B;
        config.length = 256;
        ret = ql_spi_master_read(&config,frecv);
        if (ret == 0)
        {
            os_printf(LM_OS, LL_INFO, "spi master frecv:\r\n");
            dump_hex_printf(frecv, config.length, 8);
        }
        else
        {
            os_printf(LM_OS, LL_INFO, "spi master frecv err:%d\r\n", ret);
        }

        ql_rtos_task_sleep_ms(1000);
    } while (1);

#else
    ql_wdg_init(5000);
    ql_spi_slave_init(&spi_slave_dev);

    int ret = 0;
    uint8_t cnt = 0;
    while (1)
    {
        ret = 0;
        memset(frecv, 0, SPI_TEST_LEN);
        while (!ret)
        {
            ret = ql_spi_slave_read_wait();
            ql_rtos_task_sleep_ms(100);
        }
        ql_spi_slave_read((unsigned char *)frecv, SPI_TEST_LEN);
        dump_hex_printf(frecv, ret, 8);

        memset(fsend, 'A', SPI_TEST_LEN);
        fsend[SPI_TEST_LEN - 1] = 0;
        for (size_t i = 0; i < SPI_TEST_LEN; i++)
        {
            fsend[i] = i;
        }
        fsend[0] = 'A';
        ql_wdg_reload();
        if (cnt == 0)
        {
            ret = ql_spi_slave_write((unsigned char *)fsend, 256);
        }
        if (ret == -1)
        {
            cnt = 1;
            ret = 0;
        }
        if (cnt != 0)
        {
            cnt++;
            if (cnt >= 4)
                cnt = 0;
        }
        os_printf(LM_OS, LL_INFO, "spi slave fsend ok\r\n");
        ql_rtos_task_sleep_ms(1000);
    }
#endif
}

ql_task_t spi_tx_test_thread_handle = NULL;
void ql_spi_demo_thread_creat(void)
{
    int ret;
    ret = ql_rtos_task_create(&spi_tx_test_thread_handle,
                              (unsigned short)1024 * 10,
                              RTOS_HIGHER_PRIORTIY_THAN(3),
                              "spi_tx_test",
                              ql_spi_tx_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO, "Error: Failed to create spi test thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (spi_tx_test_thread_handle != NULL)
    {
        ql_rtos_task_delete(spi_tx_test_thread_handle);
    }
}