#include <stdio.h>
#include <string.h>
#include "ql_debug.h"
#include "ql_flash.h"
#include "ql_wlan.h"
#ifdef CONFIG_QL_AT_SUPPORT
#include "ql_atcmd_task.h"
#endif
#include "quec_apimf.h"

// #include "ql_sdk_version.h"
char __ql_release_version[] = {__QL_RELEASE_VERSION};

#define CFG_ENABLE_QUECTEL_OSI 0
#define CFG_ENABLE_QUECTEL_ADC 0
#define CFG_ENABLE_QUECTEL_TIMER 0
#define CFG_ENABLE_QUECTEL_GPIO 0
#define CFG_ENABLE_QUECTEL_UART 0
#define CFG_ENABLE_QUECTEL_UART_USER 0
#define CFG_ENABLE_QUECTEL_WLAN 0
#define CFG_ENABLE_QUECTEL_PWM 0
#define CFG_ENABLE_QUECTEL_BLE 0
#define CFG_ENABLE_QUECTEL_BLE_NET 0
#define CFG_ENABLE_QUECTEL_AP_NET 0
#define CFG_ENABLE_QUECTEL_LOW_POWER 0
#define CFG_ENABLE_QUECTEL_SPI 0
#define CFG_ENABLE_QUECTEL_FLASH 0
#define CFG_ENABLE_QUECTEL_SPI_FLASH 0
#define CFG_ENABLE_QUECTEL_OTA 0
#define CFG_ENABLE_QUECTEL_CH392 0
#define CFG_ENABLE_QUECTEL_AP_BLE_NET 0
#define CFG_ENABLE_QUECTEL_W5500 0
#define CFG_ENABLE_QUECTEL_WDT 0
#define CFG_ENABLE_QUECTEL_I2C 0
#define CFG_ENABLE_QUECTEL_ST 0
#define CFG_ENABLE_QUECTEL_FS 0
#define CFG_ENABLE_QUECTEL_ALIYUN 0
#define CFG_ENABLE_QUECTEL_HTTP 0
#define CFG_ENABLE_QUECTEL_FTP 0
#define CFG_ENABLE_QUECTEL_MQTT 0
#define CFG_ENABLE_QUECTEL_RTC 0
#define CFG_ENABLE_QUECTEL_SL_TLS 0
#define CFG_ENABLE_QUECTEL_SOCKETS 0

#if CFG_ENABLE_QUECTEL_OSI
void ql_osi_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_ADC
void ql_adc_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_TIMER
void ql_timer_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_GPIO
void ql_gpio_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_UART
void ql_uart_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_UART_USER
void ql_uart_user_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_WLAN
void ql_wlan_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_SPI
void ql_spi_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_PWM
void ql_pwm_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_BLE
void ql_ble_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_BLE_NET
void ql_ble_config_network_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_AP_NET
void ql_wlan_cfgnet_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_LOW_POWER
void ql_lowpower_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_SPI
void ql_spi_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_FLASH
void ql_flash_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_SPI_FLASH
void ql_spi_flash_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_OTA
void ql_ota_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_CH392
void ql_ch392_lwip_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_AP_BLE_NET
void ql_ap_ble_net_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_W5500
void ql_w5500_lwip_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_WDT
void ql_wdg_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_I2C
void ql_i2c_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_ST
void quecst_apimf_start(void);
#endif
#if CFG_ENABLE_QUECTEL_FS
void ql_fs_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_ALIYUN

#endif
#if CFG_ENABLE_QUECTEL_HTTP
void ql_https_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_FTP
void ql_ftp_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_MQTT
void ql_mqtt_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_RTC

#endif
#if CFG_ENABLE_QUECTEL_SL_TLS
void ql_sl_tls_demo_thread_creat(void);
#endif
#if CFG_ENABLE_QUECTEL_SOCKETS
void ql_sockets_demo_thread_creat(void);
#endif

extern void app_main(void);
void __attribute__((weak)) app_main(void){ }

ql_task_t app_main_thread_handle = NULL;

static void app_main_thread(void *args){
   ql_rtos_task_sleep_ms(3000);
   app_main();
   ql_rtos_task_delete(NULL);
}

void app_main_thread_creat(void)
{
    int ret;
    ret = ql_rtos_task_create(&app_main_thread_handle,
                              (unsigned short)1024*4,
                              RTOS_HIGHER_PRIORTIY_THAN(3),
                              "app_main",
                              app_main_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO, "Error: Failed to create fs test thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (app_main_thread_handle != NULL)
    {
        ql_rtos_task_delete(app_main_thread_handle);
    }
}

void ql_app_main()
{
   ql_debug("======ql_app_main start=====\n");
   os_printf(LM_OS, LL_INFO, "Release version %s\r\n", (char *)(&__ql_release_version[4]));
   app_main_thread_creat();

#ifdef CONFIG_QL_AT_SUPPORT
   ql_atcmd_task_init();
#else
#if CFG_ENABLE_QUECTEL_FS
  ql_fs_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_OSI
   ql_osi_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_ADC
   ql_adc_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_TIMER
   ql_timer_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_GPIO
   ql_gpio_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_UART
   ql_uart_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_UART_USER
   ql_uart_user_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_WLAN
   ql_wlan_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_SPI
   ql_spi_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_PWM
   ql_pwm_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_BLE
   ql_ble_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_BLE_NET
   ql_ble_config_network_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_AP_NET
   ql_wlan_cfgnet_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_LOW_POWER
   ql_lowpower_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_FLASH
   ql_flash_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_SPI_FLASH
   ql_spi_flash_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_OTA
   ql_ota_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_CH392
   ql_ch392_lwip_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_AP_BLE_NET
   ql_ap_ble_net_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_W5500
   ql_w5500_lwip_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_WDT
   ql_wdg_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_I2C
   ql_i2c_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_ST
   quecst_apimf_start();
#endif
#if CFG_ENABLE_QUECTEL_ALIYUN

#endif
#if CFG_ENABLE_QUECTEL_HTTP
   ql_https_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_MQTT
   ql_mqtt_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_RTC

#endif
#if CFG_ENABLE_QUECTEL_SL_TLS
   ql_sl_tls_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_SOCKETS
   ql_sockets_demo_thread_creat();
#endif
#if CFG_ENABLE_QUECTEL_FTP
  ql_ftp_demo_thread_creat();
#endif
#endif
}
