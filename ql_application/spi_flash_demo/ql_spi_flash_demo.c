


#include <stdio.h>
#include "oshal.h"
#include "ql_spi_flash.h"
#include "ql_api_osi.h"
#define SPI_TEST_FLASH_LEN  256
#define SPI_TEST_FLASH_ADDR  0x232000
ql_task_t spi_flash_test_thread_handle = NULL;

void ql_spi_flash_demo_thread(void *param)
{
	int ret = 0;
    ql_rtos_task_sleep_ms(3000);
    os_printf(LM_OS, LL_INFO,"spi flash test start\r\n");
    spi_flash_dma_read_id();
    uint8_t *buf = os_malloc(SPI_TEST_FLASH_LEN * sizeof(UINT8));
	if(buf == 0)
	{
		os_printf(LM_APP, LL_INFO,"buf malloc failed\r\n");
		goto spi_flash_test_exit;
	}
	    
	memset(buf,0,SPI_TEST_FLASH_LEN);
	os_printf(LM_APP, LL_INFO,"spi flash read start\r\n");
	ret =  ql_spi_flash_read( buf, SPI_TEST_FLASH_ADDR, SPI_TEST_FLASH_LEN);
	if (ret == 0)
		{
		  for (int i = 0; i < SPI_TEST_FLASH_LEN; i++) {
			  os_printf(LM_APP, LL_INFO,"%02x,", buf[i]);
			  if ((i + 1) % 32 == 0)
				  os_printf(LM_APP, LL_INFO,"\r\n");
		   }
		  os_printf(LM_APP, LL_INFO,"\r\nspi flash read success\r\n");

		}
	else
	  os_printf(LM_APP, LL_INFO,"spi flash read fail \r\n");

    os_printf(LM_APP, LL_INFO,"spi flash write start\r\n");
	memset(buf,0,SPI_TEST_FLASH_LEN);

	for(int i=0;i<SPI_TEST_FLASH_LEN;i++)
		buf[i] = os_random()%256;

	for (int i = 0; i < SPI_TEST_FLASH_LEN; i++) {
	  os_printf(LM_APP, LL_INFO,"%02x,", buf[i]);
	  if ((i + 1) % 32 == 0)
		  os_printf(LM_APP, LL_INFO,"\r\n");
    }

	ret  = ql_spi_flash_write(buf, SPI_TEST_FLASH_ADDR, SPI_TEST_FLASH_LEN);
	if (ret == 0)
	  os_printf(LM_APP, LL_INFO,"\r\nspi flash write success\r\n");
	else
	  os_printf(LM_APP, LL_INFO,"\r\nspi flash write fail \r\n");

	os_printf(LM_APP, LL_INFO,"spi flash erase start\r\n");
	ret = ql_spi_flash_erase_sector(SPI_TEST_FLASH_ADDR);
	if (ret == 0)
	  os_printf(LM_APP, LL_INFO,"spi flash erase success\r\n");
	else
	  os_printf(LM_APP, LL_INFO,"spi flash erase fail\r\n \r\n");
	memset(buf,0,SPI_TEST_FLASH_LEN);
	ret =  ql_spi_flash_read(buf, SPI_TEST_FLASH_ADDR, 512);
		if (ret == 0)
			{
			  os_printf(LM_APP, LL_INFO,"spi flash read success\r\n");
			  for (int i = 0; i < 512; i++) {
				  os_printf(LM_APP, LL_INFO,"%02x,", buf[i]);
				  if ((i + 1) % 32 == 0)
					  os_printf(LM_APP, LL_INFO,"\r\n");
			   }

			}
		else
		  os_printf(LM_APP, LL_INFO,"spi flash read fail\r\n \r\n");

	ret = ql_spi_flash_erase_64k_block(SPI_TEST_FLASH_ADDR);
	if (ret == 0)
	  os_printf(LM_APP, LL_INFO,"spi flash erase success\r\n");
	else
	  os_printf(LM_APP, LL_INFO,"spi flash erase fail\r\n \r\n");
	ret =  ql_spi_flash_read(buf, SPI_TEST_FLASH_ADDR, SPI_TEST_FLASH_LEN);
		if (ret == 0)
			{
			  os_printf(LM_APP, LL_INFO,"spi flash read success\r\n");
			  for (int i = 0; i < SPI_TEST_FLASH_LEN; i++) {
				  os_printf(LM_APP, LL_INFO,"%02x,", buf[i]);
				  if ((i + 1) % 32 == 0)
					  os_printf(LM_APP, LL_INFO,"\r\n");
			   }

			}
		else
		  os_printf(LM_APP, LL_INFO,"spi flash read fail\r\n \r\n");
	ret = ql_spi_flash_erase_chip();
	if (ret == 0)
	  os_printf(LM_APP, LL_INFO,"spi flash erase success\r\n");
	else
	  os_printf(LM_APP, LL_INFO,"spi flash erase fail\r\n \r\n");

	memset(buf,0,SPI_TEST_FLASH_LEN);
	ret =  ql_spi_flash_read(buf, SPI_TEST_FLASH_ADDR, SPI_TEST_FLASH_LEN);
		if (ret == 0)
			{
			  os_printf(LM_APP, LL_INFO,"spi flash read success\r\n");
			  for (int i = 0; i < SPI_TEST_FLASH_LEN; i++) {
				  os_printf(LM_APP, LL_INFO,"%02x,", buf[i]);
				  if ((i + 1) % 32 == 0)
					  os_printf(LM_APP, LL_INFO,"\r\n");
			   }

			}
		else
		  os_printf(LM_APP, LL_INFO,"spi flash read fail\r\n \r\n");

	os_printf(LM_APP, LL_INFO,"\r\n spi flash test over \r\n");

spi_flash_test_exit:

	while(1)
	{
	  ql_rtos_task_sleep_ms(1000);
	}
}
void ql_spi_flash_demo_thread_creat(void)
{
    ql_spi_flash_dma_tx_rx_init();
    int ret;
    ret = ql_rtos_task_create(&spi_flash_test_thread_handle,
                              (unsigned short)1024*10,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "spi_flash_test",
                              ql_spi_flash_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO, "Error: Failed to create spi test thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (spi_flash_test_thread_handle != NULL)
    {
        ql_rtos_task_delete(spi_flash_test_thread_handle);
    }
}