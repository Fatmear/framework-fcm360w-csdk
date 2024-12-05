#include <stdio.h>
#include <string.h>
#include "oshal.h"
#include "ql_spi.h"
#include "ql_api_osi.h"
#include "ql_debug.h"
#include "cli.h"

#define ql_standard_api 1

static int ql_master_spi_init(cmd_tbl_t *t, int argc, char *argv[])
{
#if ql_standard_api
    ql_spi_config_s spi_cfg;
    spi_cfg.clk = atoi(argv[2]);
    spi_cfg.cmd_read = QL_SPI_TRANSCTRL_TRAMODE_DR | QL_SPI_TRANSCTRL_CMDEN;
    spi_cfg.cmd_write = QL_SPI_TRANSCTRL_TRAMODE_DW | QL_SPI_TRANSCTRL_CMDEN;
    spi_cfg.dma = (unsigned char)atoi(argv[1]);
    spi_cfg.dummy_bit = QL_SPI_TRANSCTRL_DUMMY_CNT_1;
    spi_cfg.pol_pha_mode = QL_SPI_POL_PHA_MODE0;
    spi_cfg.role = QL_SPI_ROLE_MASTER;
    spi_cfg.wire_mode = QL_SPI_4WIRE_MODE;
    ql_spi_init(QL_SPI_ID1, &spi_cfg);
#else
    ql_spi_master_interface_config_st spi_master_dev_config;
    unsigned char spi_dma_open = 0;
    spi_dma_open = (unsigned char)atoi(argv[1]);
    ql_debug("spi_dma_open:%d\r\n", spi_dma_open);
    spi_master_dev_config.addr_len = 3;
    spi_master_dev_config.data_len = 8;
    spi_master_dev_config.spi_clk_pol = 0;
    spi_master_dev_config.spi_clk_pha = 0;
    spi_master_dev_config.spi_trans_mode = QL_SPI_MODE_STANDARD;
    spi_master_dev_config.master_clk = atoi(argv[2]);
    spi_master_dev_config.addr_pha_enable = 0;
    spi_master_dev_config.cmd_read = QL_SPI_TRANSCTRL_TRAMODE_DR | QL_SPI_TRANSCTRL_CMDEN;
    spi_master_dev_config.cmd_write = QL_SPI_TRANSCTRL_TRAMODE_DW | QL_SPI_TRANSCTRL_CMDEN;
    spi_master_dev_config.dummy_bit = QL_SPI_TRANSCTRL_DUMMY_CNT_1;
    ql_debug("spi_master_dev_config.master_clk:%d\r\n", spi_master_dev_config.master_clk);
    if (spi_dma_open == 1)
    {
        ql_debug("spi_dma_open OK\r\n");
        spi_master_dev_config.spi_dma_enable = 1;
    }
    else if (spi_dma_open == 0)
    {
        ql_debug("spi_dma_close\r\n");
        spi_master_dev_config.spi_dma_enable = 0;
    }
    else
    {
        return CMD_RET_FAILURE;
    }
    ql_spi_master_init(&spi_master_dev_config);
#endif

    return CMD_RET_SUCCESS;
}

CLI_CMD(ql_mspi_init, ql_master_spi_init, "ql_masterspi_init", "ql_masterspi_in");

static int ql_mspi_write(cmd_tbl_t *t, int argc, char *argv[])
{
    ql_spi_transaction_st config;
    config.cmmand = 0x51;
    config.addr = 0x0;
    config.length = 256;
    unsigned char test_tx[256] = {0};
    unsigned int i;
    // unsigned char test_rx[256]={0};
    ql_debug("argc:%d\r\n", argc);
    if (argc == 1)
    {
        i = 0;
        for (i = 0; i < config.length; ++i)
        {
            test_tx[i] = (unsigned char)(i % 256);
        }
        // spi_master_write((unsigned char *)test_tx, &config);
#if ql_standard_api
        ql_spi_write_bytes(QL_SPI_ID1, test_tx, 256);
#else
        ql_spi_master_write(&config, test_tx);
#endif
    }
    else if (argc == 2)
    {
        strcpy((char *)test_tx, argv[1]);
        i = strlen((char *)test_tx);
        ql_debug("spi master send len:%d\r\n", i);
#if ql_standard_api
        ql_spi_write_bytes(QL_SPI_ID1, test_tx, i);
#else
        ql_spi_master_write(&config, test_tx);
#endif
    }
    return CMD_RET_SUCCESS;
}

CLI_CMD(ql_mspi_tx, ql_mspi_write, "ql_master_spi_tx", "ql_master_spi_tx");

static int ql_mspi_read_test(cmd_tbl_t *t, int argc, char *argv[])
{
    ql_spi_transaction_st config;
    config.cmmand = 0x0B;
    config.addr = 0x0;
    config.length = 256;

    unsigned char test[256] = {0};
    uint32_t i = 0;
    if (argv[1][0] == '1')
    {
#if ql_standard_api
        ql_spi_read_bytes(QL_SPI_ID1, test, 256);
#else
        ql_spi_master_read(&config, (unsigned char *)test);
#endif
        for (i = 0; i < config.length; ++i)
        {
            os_printf(LM_APP, LL_INFO, "sfTest[%d]=0x%d\n", i, test[i]);
        }
    }
    else if (argv[1][0] == '2')
    {
#if ql_standard_api
        ql_spi_read_bytes(QL_SPI_ID1, test, 256);
        // short len = 0;
        // len = ql_spi_read_bytes(QL_SPI_ID1, test, i);
        // if (len != 0)
        // {
        //     i = strlen((char *)test);
        //     ql_debug("spi master recv %d:%s\r\n", i, test);
        //     memset(test,0,i);
        //     ql_spi_read_bytes(QL_SPI_ID1, test, len);
        // }
#else
        ql_spi_master_read(&config, (unsigned char *)test);
#endif
        i = strlen((char *)test);
        ql_debug("spi master recv %d:%s\r\n", i, test);
    }
    return CMD_RET_SUCCESS;
}

CLI_CMD(ql_mspi_rx, ql_mspi_read_test, "test_spi_read", "test_spi_read");