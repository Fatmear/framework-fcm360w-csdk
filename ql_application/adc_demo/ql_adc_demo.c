
/**  @file
  ql_adc_demo.c

  @brief
  TODO

*/

/*================================================================
  Copyright (c) 2022 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/

#include "ql_api_osi.h"
#include "ql_debug.h"
#include "ql_adc.h"

#define new_api 1
#define ADC_TEST_DEBUG
#ifdef ADC_TEST_DEBUG
#define ql_adc_log os_printf

#else

#endif

/***************adc test******************/
ql_adc_obj_s ql_test_adc;
ql_task_t adc_test_thread_handle = NULL;

#define DEMO_ADC_SINGLE_ENABLE 0
#define DEMO_ADC_DIFF_ENABLE 0
#define DEMO_ADC_VBAT_ENABLE 1

void ql_adc_demo_thread(void *param)
{
    ql_rtos_task_sleep_ms(3000);

    ql_adc_log(LM_OS, LL_INFO, "adc test start\r\n");
#if DEMO_ADC_SINGLE_ENABLE
    // The following is the single-ended test of GPIO20
    int signalvbat = 0;
    ql_test_adc.channel_a = QL_ADC_A_INPUT_GPIO_2;
    ql_test_adc.channel_b = QL_ADC_B_INPUT_VREF_BUFFER;
    ql_test_adc.volt = QL_ADC_EXPECT_MAX_VOLT3;
#if new_api
    ql_adc_init(QL_ADC_CHANNEL2, QL_ADC_VREF_BUFFER, QL_ADC_SINGE, QL_ADC_EXPECT_MAX_VOLT3);
    ql_adc_start(QL_ADC_CHANNEL2);
    ql_adc_read(QL_ADC_CHANNEL2, &signalvbat, 0);
#else
    ql_adc_channel_init(&ql_test_adc);
    ql_adc_channel_start(QL_ADC_SINGE, &signalvbat);
    os_printf(LM_OS, LL_INFO, "signalvbat = [%d]mv\r\n", signalvbat / 1000);
#endif

#elif DEMO_ADC_DIFF_ENABLE
    int diffvbat = 0;
    ql_test_adc.channel_a = QL_ADC_A_INPUT_GPIO_2;
    ql_test_adc.channel_b = QL_ADC_A_INPUT_GPIO_3;
    ql_test_adc.volt = QL_ADC_EXPECT_MAX_VOLT2;
#if new_api
    ql_adc_init(QL_ADC_CHANNEL0, QL_ADC_CHANNEL1, QL_ADC_DIFF, QL_ADC_EXPECT_MAX_VOLT3);
    ql_adc_start(QL_ADC_CHANNEL0);
    ql_adc_read(QL_ADC_CHANNEL0, &diffvbat, 0);
#else
    // The following is the differential test of GPIO20 and GPIO18
    ql_adc_channel_init(&ql_test_adc);
    ql_adc_channel_start(QL_ADC_DIFF, &diffvbat);
    os_printf(LM_OS, LL_INFO, "diffvbat = [%d]\r\n", diffvbat);
#endif

#elif DEMO_ADC_VBAT_ENABLE
    int vbat = 0;
    ql_test_adc.channel_a = QL_ADC_A_INPUT_VBAT;
#if new_api
    ql_adc_init(QL_ADC_VREF_BUFFER, QL_ADC_VREF_BUFFER, QL_ADC_VBAT, QL_ADC_EXPECT_MAX_VOLT3);
    ql_adc_start(QL_ADC_VREF_BUFFER);
    ql_adc_read(QL_ADC_VREF_BUFFER, &vbat, 0);
#else
    ql_adc_channel_init(&ql_test_adc);
    ql_adc_channel_start(QL_ADC_VBAT, &vbat);
    os_printf(LM_OS, LL_INFO, "vbat = [%d]\r\n", vbat);
#endif

#endif
    while (1)
    {
        ql_rtos_task_sleep_ms(3000);
#if DEMO_ADC_SINGLE_ENABLE
#if new_api
        ql_adc_start(QL_ADC_CHANNEL2);
        ql_adc_read(QL_ADC_CHANNEL2, &signalvbat, 0);
        os_printf(LM_OS, LL_INFO, "signalvbat = [%d]mv\r\n", signalvbat / 1000);
#else
        ql_adc_channel_start(QL_ADC_SINGE, &signalvbat);
        os_printf(LM_OS, LL_INFO, "signalvbat = [%d]mv\r\n", signalvbat / 1000);
#endif
#elif DEMO_ADC_DIFF_ENABLE
#if new_api
        ql_adc_start(QL_ADC_CHANNEL0);
        ql_adc_read(QL_ADC_CHANNEL0, &diffvbat, 0);
        os_printf(LM_OS, LL_INFO, "diffvbat = [%d]mv\r\n", diffvbat / 1000);
#else
        ql_adc_channel_start(QL_ADC_DIFF, &diffvbat);
        os_printf(LM_OS, LL_INFO, "diffvbat = [%d]mv\r\n", diffvbat / 1000);
#endif
#elif DEMO_ADC_VBAT_ENABLE
#if new_api
        ql_adc_start(QL_ADC_INVBAT);
        ql_adc_read(QL_ADC_INVBAT, &vbat, 0);
        os_printf(LM_OS, LL_INFO, "vbat = [%d]mv\r\n", vbat / 1000);
#else
        ql_adc_channel_start(QL_ADC_VBAT, &vbat);
        os_printf(LM_OS, LL_INFO, "vbat = [%d]mv\r\n", vbat / 1000);
#endif
#endif
    }
    ql_rtos_task_delete(adc_test_thread_handle);
}

void ql_adc_demo_thread_creat(void)
{
    int ret;
    ret = ql_rtos_task_create(&adc_test_thread_handle,
                              (unsigned short)2048,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "adc_test",
                              ql_adc_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO, "Error: Failed to create adc test thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (adc_test_thread_handle != NULL)
    {
        ql_rtos_task_delete(adc_test_thread_handle);
    }
}