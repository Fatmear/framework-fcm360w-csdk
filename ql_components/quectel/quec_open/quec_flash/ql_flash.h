#ifndef _QL_FLASH_H_
#define _QL_FLASH_H_
#include <stdint.h>

typedef enum {
    QL_EF_NO_ERR = 0,
    QL_EF_ERASE_ERR = -1,
    QL_EF_READ_ERR = -2,
    QL_EF_WRITE_ERR = -3,
    QL_EF_ENV_NAME_ERR = -4,
    QL_EF_ENV_NAME_EXIST = -5,
    QL_EF_ENV_FULL = -6,
    QL_EF_ENV_INIT_FAILED = -7,
    QL_EF_PARTITION_STATUS_ERR = -8,    
    QL_EF_READ_ADDR_OVERFLOW = 9,
} ql_errcode_flash_e;

int ql_flash_read(unsigned int addr, unsigned char * buff, unsigned int len);
int ql_flash_write(unsigned int addr, unsigned char * buff, unsigned int len);
int ql_flash_erase(unsigned int addr, unsigned int len);
signed int ql_flash_cus_parttion_read(char *key, void *value_buf, signed int buf_len, signed int *value_len);
ql_errcode_flash_e ql_flash_cus_parttion_write(char *key, void *value_buf, signed int buf_len);
ql_errcode_flash_e ql_flash_cus_parttion_del(char* key);

signed int ql_flash_dev_parttion_read(char *key, void *value_buf, signed int buf_len, signed int *value_len);
ql_errcode_flash_e ql_flash_dev_parttion_write(char *key, void *value_buf, signed int buf_len);
ql_errcode_flash_e ql_flash_dev_parttion_del(char* key);
#endif