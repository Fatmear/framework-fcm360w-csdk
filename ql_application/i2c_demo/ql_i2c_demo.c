#include "ql_api_osi.h"
#include "ql_debug.h"
#include "ql_i2c.h"
#define I2C_TEST_LEN 10
ql_i2c_config_t i2c_cmd_config = {0};
uint8_t recv_buf[I2C_TEST_LEN] = {0};
	uint8_t send_buf[I2C_TEST_LEN] = {0};
ql_task_t i2c_test_thread_handle = NULL;
static int  ql_i2c_demo_init(void)
{
    ql_i2c_config_t i2c_cmd_config=
    {
        .ql_i2c_speed      = ql_I2C_Speed_100K,
        .ql_i2c_addressbit = ql_I2C_Address_7bit,
        .ql_i2c_mode       = ql_I2C_Mode_Master,
        .ql_i2c_dma        = ql_I2C_DMA_Disable,
        .ql_scl_num        = 2,
        .ql_sda_num        = 2,
    };
    if(ql_i2c_init(&i2c_cmd_config) < 0)
        return -1;
    for (int i = 0; i < I2C_TEST_LEN; i++)
    {
        send_buf[i] = i+0x10;
    }
    return 0;
}

void ql_i2c_demo_thread(void *param)
{
    uint8_t ql_i2c_slave_addr = 0x50;
    ql_rtos_task_sleep_ms(3000);
    ql_i2c_demo_init();
    ql_i2c_master_write(ql_i2c_slave_addr,(unsigned char *)send_buf,I2C_TEST_LEN);
    ql_rtos_task_sleep_ms(1000);
    ql_i2c_master_read(ql_i2c_slave_addr,(unsigned char *)recv_buf,I2C_TEST_LEN,2);
    ql_debug("i2c test read:");
    for (int j = 0; j < I2C_TEST_LEN; j++)
    {
        ql_debug("%02x ",recv_buf[j]);
    }
    ql_debug("\r\n");
    ql_i2c_close();
}
void ql_i2c_demo_thread_creat(void)
{
    // ql_spi_flash_dma_tx_rx_init();
    int ret;
    ret = ql_rtos_task_create(&i2c_test_thread_handle,
                              (unsigned short)1024*10,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "i2c_test",
                              ql_i2c_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO, "Error: Failed to create spi test thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (i2c_test_thread_handle != NULL)
    {
        ql_rtos_task_delete(i2c_test_thread_handle);
    }
}
static int ql_cmd_i2c_test(cmd_tbl_t *t, int argc, char *argv[])
{
    char *i2c_key ;
    i2c_key = argv[1];
    uint32_t slave_addr = 0;
    if(strcmp("init", i2c_key)==0)
    {
        i2c_cmd_config.ql_i2c_mode = strtoul(argv[2], NULL, 0); //mode
        i2c_cmd_config.ql_i2c_speed = strtoul(argv[3], NULL, 0);//speed
        i2c_cmd_config.ql_i2c_addressbit = strtoul(argv[4], NULL, 0);//addrbit
        i2c_cmd_config.ql_i2c_dma = strtoul(argv[5], NULL, 0);//dma
        i2c_cmd_config.ql_scl_num = strtoul(argv[6], NULL, 0);//scl
        i2c_cmd_config.ql_sda_num = strtoul(argv[7], NULL, 0);//sda
    //     I2C_InitTypeDef I2C_InitTypeDef=
    // {
    //     .I2C_Speed      = I2C_Speed_100K,
    //     .I2C_AddressBit = I2C_Address_7bit,
    //     .I2C_Mode       = I2C_Mode_Master,
    //     .I2C_DMA        = I2C_DMA_Disable,
    //     .scl_num        = I2C_SCL_USED_GPIO2,
    //     .sda_num        = I2C_SDA_USED_GPIO3,
    // };
        ql_i2c_init(&i2c_cmd_config);
        for (int i = 2; i < I2C_TEST_LEN; i++)
		{
			send_buf[i] = i+0x10;
		}
    }
    else if(strcmp("write", i2c_key)==0)
    {
        slave_addr = strtoul(argv[2], NULL, 0);
        ql_i2c_master_write(slave_addr,(unsigned char *)send_buf,I2C_TEST_LEN);
        // ql_i2c_close();
    }
    else if(strcmp("read", i2c_key)==0)
    {
        slave_addr = strtoul(argv[2], NULL, 0);
        // ql_i2c_master_write(slave_addr,(unsigned char *)recv_buf,2);
        ql_i2c_master_read(slave_addr,(unsigned char *)recv_buf,I2C_TEST_LEN-2,2);
        ql_debug("i2c test read:");
        for (int j = 2; j < I2C_TEST_LEN; j++)
		{
			ql_debug("%02x ",recv_buf[j]);
		}
        ql_debug("\r\n");
        ql_i2c_close();
    }
	return CMD_RET_SUCCESS;
}
CLI_CMD_F(ql_i2c,ql_cmd_i2c_test,"ql_cmd_i2c_test",  "ql_cmd_i2c_test",true);