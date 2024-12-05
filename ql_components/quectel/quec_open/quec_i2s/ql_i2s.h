#ifndef _QL_I2S_H_
#define _QL_I2S_H_

#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief I2S mode set
 */
typedef enum
{
    QL_I2S_MODE_MASTER = 0,
    QL_I2S_MODE_SLAVE
} ql_i2s_mode_e;

/**
 * @brief I2S transfer data align format
 */
typedef enum
{
    QL_DATA_ALIGN_LEFT = 0,
    QL_DATA_ALIGN_RIGHT = 1,
    QL_DATA_ALIGN_I2S
} ql_i2s_data_align_format_e;

/**
 * @brief I2S audio file format
 */
typedef enum
{
    QL_STEREO_MODE = 0,
    QL_COMPRESSED_STEREO_MODE = 1,
    QL_MONO_MODE = 2,
    QL_RESERVE = 3
} ql_i2s_trans_mode_e;

/**
 * @brief I2S transfer data width
 */
typedef enum
{
    QL_I2S_BIT_8_WIDTH = 1,
    QL_I2S_BIT_16_WIDTH,
    QL_I2S_BIT_32_WIDTH
} ql_i2s_bit_width_e;

/**
 * @brief I2S compressed stereo data width
 */
typedef enum
{
    QL_COMPRE_BIT_8_WIDTH = 0,
    QL_COMPRE_BIT_16_WIDTH
} ql_i2s_compre_bit_width_e;

/**
 * @brief I2S Initialize variable definition
 */
typedef struct
{
    ql_i2s_mode_e mode;
    float sample_rate;
    ql_i2s_bit_width_e sample_width;
    ql_i2s_data_align_format_e data_align_format;
    ql_i2s_trans_mode_e trans_mode;
    ql_i2s_compre_bit_width_e compre_bit_width;
    unsigned int i2s_dma;
} ql_i2s_config_t;

// i2s_cfg_dev i2s_cfg;

/**
 * @brief I2S Initialize function
 */
int ql_i2s_init_cfg(ql_i2s_config_t *ql_i2s_cfg);

/**
@brief Statement i2s close
*/
int ql_drv_i2s_close();

/**
@brief     Statement i2s master transmit data
@param[in]  *bufptr : Buffer for sending data
@param[in]  length : The length of the data sent by the host
*/
void ql_i2s_master_write(unsigned char *bufptr, unsigned int length);

/**
@brief     Statement i2c master receive data
@param[in]  *bufptr : The data read by the master is stored in this data
@param[in]  length : The length of the data the host wants to read
*/
void ql_i2s_master_read(unsigned char *bufptr, unsigned int length);
#ifdef __cplusplus
} /*"C" */
#endif

#endif