#ifndef _QL_I2C_H_
#define _QL_I2C_H_

#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief I2C Initialize variable definition.
 * @details The detail description.
 */
typedef struct
{
  unsigned int   ql_i2c_speed;
  unsigned char  ql_i2c_addressbit;//I2C_AddressBit
  unsigned char  ql_i2c_mode;//I2C_Mode
  unsigned int  ql_i2c_dma;//I2C_DMA
  unsigned char ql_scl_num;//scl_num
  unsigned char ql_sda_num;//sda_num
}ql_i2c_config_t;//I2C_InitTypeDef

#define ql_I2C_Address_7bit              0x00
#define ql_I2C_Address_10bit             0x02
#define ql_I2C_Mode_Slave                0x00 
#define ql_I2C_Mode_Master               0x04

#define ql_I2C_DMA_Disable               0x00
#define qL_I2C_DMA_Enable                0x08
 
#define ql_I2C_Speed_100K     				100000
#define ql_I2C_Speed_400K     				400000
#define ql_I2C_Speed_1M     				1024000
#define ql_I2C_Disable                   0x00
#define ql_I2C_Enable                    0x01


int ql_i2c_init(ql_i2c_config_t * i2c_config);
int ql_i2c_master_write(unsigned int slave_addr,unsigned char *bufptr, unsigned int len);
int ql_i2c_master_read(unsigned int slave_addr,unsigned char *bufptr, unsigned int len,unsigned int cmd_len);
int ql_i2c_close();
#ifdef __cplusplus
} /*"C" */
#endif

#endif