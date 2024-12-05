#ifndef __QL_ADC_H
#define __QL_ADC_H

typedef void (*ql_adc_obj_callback)(int new_mv, void *user_data);

typedef enum
{
	QL_ADC_A_INPUT_GPIO_0 = 0x1,
	QL_ADC_A_INPUT_GPIO_1 = 0x2,
	QL_ADC_A_INPUT_GPIO_2 = 0x4,
	QL_ADC_A_INPUT_GPIO_3 = 0x8,
	QL_ADC_A_INPUT_VBAT = 0x10,
	QL_ADC_A_INPUT_VREF_BUFFER = 0x80,
} ql_adc_channel_a_e;

typedef enum
{
	QL_ADC_B_INPUT_GPIO_0 = 0x1,
	QL_ADC_B_INPUT_GPIO_1 = 0x2,
	QL_ADC_B_INPUT_GPIO_2 = 0x4,
	QL_ADC_B_INPUT_GPIO_3 = 0x8,
	QL_ADC_B_INPUT_VBAT = 0x10,
	QL_ADC_B_INPUT_VREF_BUFFER = 0x80,
} ql_adc_channel_b_e;

typedef enum
{
	QL_ADC_SINGE = 0,
	QL_ADC_DIFF = 1,
	QL_ADC_VBAT = 2
} ql_adc_mode_e;

typedef enum
{
	QL_ADC_SUCCESS = 0,
	QL_ADC_EXECUTE_ERR,
	QL_ADC_INVALID_PARAM_ERR,
} ql_adc_errcode_e;

typedef enum
{
	QL_ADC_EXPECT_MAX_VOLT0 = 900,
	QL_ADC_EXPECT_MAX_VOLT1 = 1800,
	QL_ADC_EXPECT_MAX_VOLT2 = 2400,
	QL_ADC_EXPECT_MAX_VOLT3 = 3600,
} ql_adc_ecpect_volt_e;

typedef struct adc_obj_
{
	ql_adc_channel_a_e channel_a;
	ql_adc_channel_b_e channel_b;
	ql_adc_ecpect_volt_e volt;
} ql_adc_obj_s;

ql_adc_errcode_e ql_adc_channel_init(ql_adc_obj_s *ql_adc_config);
void ql_adc_channel_start(ql_adc_mode_e adc_mode, int *vbat_value);

/*        ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓标准API↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓        */

/*
CHANNEL0:gpio14
CHANNEL1:gpio15
CHANNEL2:gpio20
*/
typedef enum
{
	QL_ADC_CHANNEL0 = 0x1,
	QL_ADC_CHANNEL1 = 0x2,
	QL_ADC_CHANNEL2 = 0x4,
	QL_ADC_INVBAT = 0x10,
	QL_ADC_VREF_BUFFER = 0x80,
} ql_adc_channel_e;

// typedef enum
// {
// 	QL_ADC_SINGE = 0,
// 	QL_ADC_DIFF = 1,
// 	QL_ADC_VBAT = 2,
// } ql_adc_mode_e;

// typedef enum
// {
// 	QL_ADC_SUCCESS = 0,
// 	QL_ADC_EXECUTE_ERR,
// 	QL_ADC_INVALID_PARAM_ERR,
// } ql_adc_errcode_e;

/**
 * @brief
 *
 * @param chan
 * @param sample_rate
 * @param adc_mode
 * @return ql_adc_errcode_e
 */
ql_adc_errcode_e ql_adc_init(ql_adc_channel_e chana, ql_adc_channel_e chanb, ql_adc_mode_e adc_mode, ql_adc_ecpect_volt_e ecpect_volt);

/**
 * @brief
 *
 * @param chan
 * @return ql_adc_errcode_e
 */
ql_adc_errcode_e ql_adc_deinit(ql_adc_channel_e chan);

/**
 * @brief
 *
 * @param chan
 * @return ql_adc_errcode_e
 */
ql_adc_errcode_e ql_adc_start(ql_adc_channel_e chan);

/**
 * @brief
 *
 * @param chan
 * @return ql_adc_errcode_e
 */
ql_adc_errcode_e ql_adc_stop(ql_adc_channel_e chan);

/**
 * @brief
 *
 * @param chan
 * @param data
 * @param timeout
 * @return ql_adc_errcode_e
 */
ql_adc_errcode_e ql_adc_read(ql_adc_channel_e chan, int *data, uint32_t timeout);

#endif