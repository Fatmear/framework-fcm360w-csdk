#ifndef _QL_SPI_FLASH_H_
#define _QL_SPI_FLASH_H_
#include "ql_type.h"
typedef enum
{
	 QL_SPI_FLASH_SUCCESS = 0,
	 QL_SPI_FLASH_EXECUTE_ERR,
	 QL_SPI_FLASH_PARAM_ERROR,
} ql_errcode_spi_nor_e;
void ql_spi_flash_dma_tx_rx_init();
extern UINT32 spi_flash_dma_read_id(void);
extern int spi_flash_dma_read(UINT32 addr, UINT32 size, UINT8 * dst);
extern int spi_flash_dma_write(UINT32 addr, UINT32 size, UINT8 * src);
extern int spi_flash_dma_erase(UINT32 addr, UINT32 size);
extern int spi_flash_dma_erase_page(UINT32 addr, UINT32 mode);

ql_errcode_spi_nor_e ql_spi_flash_read( unsigned char* data, unsigned int addr, unsigned int len);
ql_errcode_spi_nor_e ql_spi_flash_write(unsigned char *data, unsigned int addr, unsigned int len);
ql_errcode_spi_nor_e ql_spi_flash_erase_chip(void);
ql_errcode_spi_nor_e ql_spi_flash_erase_sector(unsigned int addr);
ql_errcode_spi_nor_e ql_spi_flash_erase_64k_block(unsigned int addr);
#endif