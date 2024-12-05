#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oshal.h"
#include "ql_spi.h"
#include "ql_api_osi.h"
#include "ql_debug.h"
#include "cli.h"

#define ql_standard_api 1

static int ql_spi_slave_init_test(cmd_tbl_t *t, int argc, char *argv[])
{
#if ql_standard_api
    ql_spi_config_s spi_cfg;
    spi_cfg.clk = atoi(argv[2]);
    spi_cfg.dma = (unsigned char)atoi(argv[1]);
    ql_debug("spi_dma_open:%d\r\n", spi_cfg.dma);
    spi_cfg.pol_pha_mode = QL_SPI_POL_PHA_MODE0;
    spi_cfg.role = QL_SPI_ROLE_SLAVE;
    spi_cfg.wire_mode = QL_SPI_4WIRE_MODE;
    ql_spi_init(QL_SPI_ID1, &spi_cfg);
#else
    unsigned char spi_dma_open = 0;
    ql_spi_slave_interface_config_t spi_slave_dev;
    spi_dma_open = atoi(argv[1]);
    ql_debug("spi_dma_open:%d\r\n", spi_dma_open);
    spi_slave_dev.addr_len = 3;
    spi_slave_dev.data_len = 8;
    spi_slave_dev.spi_clk_pol = 0;
    spi_slave_dev.spi_clk_pha = 0;
    spi_slave_dev.slave_clk = atoi(argv[2]);
    if (spi_dma_open == 1)
    {
        spi_slave_dev.inten = 0x30;
        spi_slave_dev.spi_slave_dma_enable = 1;
    }
    else if (spi_dma_open == 0)
    {
        spi_slave_dev.inten = 0x20;
        spi_slave_dev.spi_slave_dma_enable = 0;
    }
    else
    {
        return CMD_RET_FAILURE;
    }
    ql_debug("spi_slave_dev_config.master_clk:%d\r\n", spi_slave_dev.slave_clk);
    ql_spi_slave_init(&spi_slave_dev);
#endif
    return CMD_RET_SUCCESS;
}

CLI_CMD(ql_slave_init, ql_spi_slave_init_test, "spi_slave_init", "spi_slave_init");

static int ql_spi_slave_read_test(cmd_tbl_t *t, int argc, char *argv[])
{
    unsigned char test[256] = {0};
    short i = 0;
#if ql_standard_api
   
    ql_spi_read_bytes(QL_SPI_ID1, test, 16);
    // len = ql_spi_read_bytes(QL_SPI_ID1, test, 16);
    // if (len != 0)
    // {
    //     i = strlen((char *)test);
    //     ql_debug("spi master recv %d:%s\r\n", i, test);
    //     memset(test, 0, i);
    //     ql_spi_read_bytes(QL_SPI_ID1, test, len);
    // }
#else
    ql_spi_slave_read((unsigned char *)test, 256);
#endif
    if (argv[1][0] == '1')
    {
        for (i = 0; i < 256; ++i)
        {
            os_printf(LM_APP, LL_INFO, "test[%d]=%d\n", i, test[i]);
        }
    }
    else if (argv[1][0] == '2')
    {
        i = strlen((char *)test);
        ql_debug("spi slave recv %d:%s\r\n", i, test);
    }
    return CMD_RET_SUCCESS;
}

CLI_CMD(ql_slave_rx, ql_spi_slave_read_test, "test_spi_slave_read", "test_spi_slave_read");

static int ql_spi_slave_write_test(cmd_tbl_t *t, int argc, char *argv[])
{
    unsigned char test[256] = {0};
    unsigned int i;
    ql_debug("argc:%d\r\n", argc);
    if (argc == 1)
    {
        for (i = 0; i < 256; ++i)
        {
            test[i] = (unsigned char)(i % 256);
        }
    }
    else if (argc == 2)
    {
        strcpy((char *)test, argv[1]);
        i = strlen((char *)test);
        ql_debug("spi slave send len:%d\r\n", i);
    }
#if ql_standard_api
    ql_spi_write_bytes(QL_SPI_ID1, test, 256);
#else
    ql_spi_slave_write((unsigned char *)test, 256);
#endif
    return CMD_RET_SUCCESS;
}

CLI_CMD(ql_slave_tx, ql_spi_slave_write_test, "test_spi_slave_write", "test_spi_slave_write");