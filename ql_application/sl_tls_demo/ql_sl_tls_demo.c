#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "oshal.h"
#include "ql_debug.h"
#include "sl_tls.h"
#include "lwip/sockets.h"
#include "ql_api_osi.h"
#include "ql_wlan.h"
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/timing.h"
#include "mbedtls/debug.h"

#define ROOT_CA  \
"-----BEGIN CERTIFICATE-----\r\n"\
"MIIE5TCCA02gAwIBAgIJAP5uWtPRe+IxMA0GCSqGSIb3DQEBCwUAMIGHMQswCQYD\r\n"\
"VQQGEwJDTjELMAkGA1UECAwCQUgxCzAJBgNVBAcMAkhGMRwwGgYDVQQKDBNEZWZh\r\n"\
"dWx0IENvbXBhbnkgTHRkMRgwFgYDVQQDDA8yMjAuMTgwLjIzOS4yMTIxJjAkBgkq\r\n"\
"hkiG9w0BCQEWF2VkZGllLnpoYW5nQHF1ZWN0ZWwuY29tMCAXDTIxMDEyNzAzMzk0\r\n"\
"M1oYDzIxMjEwMTAzMDMzOTQzWjCBhzELMAkGA1UEBhMCQ04xCzAJBgNVBAgMAkFI\r\n"\
"MQswCQYDVQQHDAJIRjEcMBoGA1UECgwTRGVmYXVsdCBDb21wYW55IEx0ZDEYMBYG\r\n"\
"A1UEAwwPMjIwLjE4MC4yMzkuMjEyMSYwJAYJKoZIhvcNAQkBFhdlZGRpZS56aGFu\r\n"\
"Z0BxdWVjdGVsLmNvbTCCAaIwDQYJKoZIhvcNAQEBBQADggGPADCCAYoCggGBANf3\r\n"\
"w0ep+Sv4qDjhafwc3wiaGdzwAXOsRgfGgBEGYL5MuPbpzjERo21yHae6Tx19DIUt\r\n"\
"g6hgW5N/bvDqXLbEMChy9b8aNl62+Y6O35Tya2hoz4XXQkrPkRXvlP2eonjPMfQB\r\n"\
"Yu4wtAfNKPa10MJF8qjFI7WWBpcQMpoQvt/SekoD6IyxgRu8ApjtmJicvvWu3BqW\r\n"\
"W93MZqLhSFf57pKLQEhgskpl/gb+rLiUwObMTRM948J04NoYBx1jwMZLNEFs6os6\r\n"\
"91J8B5oHlgQAsgcXjoXORxncgt2d/fZyr9NjVevc/GIkcbfVAAWohFCQmjpgLESW\r\n"\
"iTPaRlLyeLLjTb+WrSOKosinVH+1R8ozfBnkzO+tf9lpBzdP9QCDYzNQeU4Xp0wJ\r\n"\
"8Neo5tA4tioDEZjkbsHL+bVvssqOwfupR9b03Z2ZLfgEzs3TwYbPB7ULJQrdqe9x\r\n"\
"QtpaFSsDeyKJ6CR8yEHz5d0lypPeRLCjZGb49Yo3FKhMdzP+ZMT+Ku001cr4GwID\r\n"\
"AQABo1AwTjAdBgNVHQ4EFgQU4FfBsWrgpoGtvuF+3XN9kbUx0yUwHwYDVR0jBBgw\r\n"\
"FoAU4FfBsWrgpoGtvuF+3XN9kbUx0yUwDAYDVR0TBAUwAwEB/zANBgkqhkiG9w0B\r\n"\
"AQsFAAOCAYEAqfVX0LhhxYZ/KsC9jhWYHgmYggVcsTa7AtZsNKW79TLmz98iWdfM\r\n"\
"wpi44rwbn1Xek5pDH/rimaJuwxcX8Q4cFHqeQJStRkcuU6CgyurVBpjGWRv5qt3W\r\n"\
"nJc/z92x3TPzW1VJv5rXj4pzX9N4hlAHGGzFBm+VMAexS006/dksGa9uEKE/2A0+\r\n"\
"9W/V9YbcjhejdENiRAvJB4J0QsOrNUjsH5bPEa3CxdXbKOQzGjJtS7f0BH38Fmyi\r\n"\
"C+Cui8U0c+BwRGY3HXL7ANhCe0vdUbUGCG2L6byRvf1TIkuGpi0RxtQfEF3sTDH5\r\n"\
"jAot50rJhbckQyLH0xkIOQ9qmU/gbt/wgoZ9AzUVIyh0RsyWo19BGz2DpsuYNBXD\r\n"\
"4jqL4NMqsyGRq5YUTrJlIi9PVUp176Ec79xSffvUbitiq9fMmxuhsRbkP4piM1TE\r\n"\
"D5oXKPme86RvR1/foRqAdbJg5RPYdah3LdOlAE2HePVy6b0xQ5dcCHaqHmR2SVlY\r\n"\
"m7TQs6tfvfhy\r\n"\
"-----END CERTIFICATE-----\r\n"

#define CLIENT_CERT \
"-----BEGIN CERTIFICATE-----\r\n"\
"MIIEjzCCAvcCCQD57PGIsuol7jANBgkqhkiG9w0BAQsFADCBhzELMAkGA1UEBhMC\r\n"\
"Q04xCzAJBgNVBAgMAkFIMQswCQYDVQQHDAJIRjEcMBoGA1UECgwTRGVmYXVsdCBD\r\n"\
"b21wYW55IEx0ZDEYMBYGA1UEAwwPMjIwLjE4MC4yMzkuMjEyMSYwJAYJKoZIhvcN\r\n"\
"AQkBFhdlZGRpZS56aGFuZ0BxdWVjdGVsLmNvbTAgFw0yMTAxMjcwMzQyMjZaGA8y\r\n"\
"MTIxMDEwMzAzNDIyNlowgYgxCzAJBgNVBAYTAkNOMQswCQYDVQQIDAJBSDELMAkG\r\n"\
"A1UEBwwCSEYxHDAaBgNVBAoME0RlZmF1bHQgQ29tcGFueSBMdGQxGDAWBgNVBAMM\r\n"\
"DzIyMC4xODAuMjM5LjIxMjEnMCUGCSqGSIb3DQEJARYYZWRkaWUuemhhbmdAcWkI\r\n"\
"ZWN0ZWwuY29tMIIBojANBgkqhkiG9w0BAQEFAAOCAY8AMIIBigKCAYEAstvaVIF1\r\n"\
"JDm0/ABzFUvcCSDY7El0W5cmBRz1i+W0G+GL3iPjUvdNpWdNC3KovCXVUcFpA5Jh\r\n"\
"EPp3rYMEELHAVh1Ylbk4kdGh6I3SRGx+qaISQmtjRiwhZu1l6HavOhPWfY+pWc6F\r\n"\
"uIqoyYvuB01SJIIeqxz/1A3rPrM4GgARPYDnSBhxBxfY8lYjKbT6kgxnlkZOMyMb\r\n"\
"HZYzcbJLUdeH0457cMFQenvSH3roAMrUKQEGG4iiHH6ozaN7IyNat91fM9f3uzpl\r\n"\
"h6T8UEB0SC45uVzQNB0RY4Yof8VNfjkN8djTutzkB/1Fhz2y4eunqlVK40J0MgCR\r\n"\
"sD7MCjSDEgl4mHe1XEH26AggszyE0qfIUhbyYIT3h1/4Gor3YbKKQsl6GpJ/2+fU\r\n"\
"bOkhiAbtGvefE3DvFZzcsu//rWT7ZO0F0WOj6VPcfNDZP5kQVxnfbqXRS6YTbWcG\r\n"\
"6SBmIL05/DXEDRGEgxov2WqbdNO/0hL/xO0HFyEs6ELWFUT5wpd2Yx3XAgMBAAEw\r\n"\
"DQYJKoZIhvcNAQELBQADggGBAH8yycWHdMKyISll4tz9nKCaXTwZDB87FhRIjnzu\r\n"\
"Lg1wgK08L/owI696qY3Z8fhMjJvnxf1X3Oj8dgddbaH1quVDYFt7lKulMEs2t0/F\r\n"\
"lruTkclXOYRi4G7OQ5DSjI9cVF69zxsZ1mH9hWnvhSbKl0Du1w/ZUaOJeoNaV8vv\r\n"\
"EG2marvmV07VOhwdbEJbvtGN14kOGHJq2SzeMXoC9MBHWdQpQu0SHl0bBs4paENd\r\n"\
"Cl7R3Mz6RDuIhd4U59Hsj3sEPlSHbyldbWRUyAgFhf1Ja3AGsEPpX3qiRO6EllpP\r\n"\
"1pgMqUyyXjNScYlYKuamZqa+KkMo0uLYKt7e4Q/oiWNMtB/LNeUOOpiE33l0VKc6\r\n"\
"zDk+DXs7FZ/U5YOqahS0P/Ob8nfnJVNKtQebrOFXxCn+Agps+5882KWpAp7pGfwD\r\n"\
"jZpeYEKOWgYl3UQMd2c5SbICqOMocUWDTSknUPy9odWKGKCKpZ56/qD2BS45ShmE\r\n"\
"Jyhxprn0pX/Kz8jW2l1Wk2LEqg==\r\n"\
"-----END CERTIFICATE-----\r\n"

#define CLIENT_KEY \
"-----BEGIN RSA PRIVATE KEY-----\r\n"\
"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\r\n"\
"-----END RSA PRIVATE KEY-----\r\n"

#define REQ_MSG \
"GET /1K.txt HTTP/1.1\r\n"\
"Host: 220.180.239.212\r\n"\
"Connection: keep-alive\r\n"\
"Agent: QOCPU\r\n"\
"\r\n"

#define remote_addr  "220.180.239.212"
#define remote_port  8301

ql_task_t tls_test_thread_handle = NULL;
ql_sem_t  tls_wlan_semp = NULL;

typedef struct{
    mbedtls_ssl_context               mbedSslCtx;
    mbedtls_ssl_config                mbedSslConf;
    mbedtls_net_context               mbedNetCtx;
    mbedtls_ctr_drbg_context          mbedDrbgCtx;
    mbedtls_entropy_context           mbedEtpyCtx;
    mbedtls_x509_crt                  mbedX509Ca;
    mbedtls_pk_context                mbedPkUser;
    mbedtls_x509_crt                  mbedX509User;
    mbedtls_timing_delay_context      timer;
}tls_client_t;



static ql_sys_err_t ql_waln_event_cb(void *ctx, ql_system_event_t *event)
{
    ql_debug("\n%s,event_id:%d\n",__func__, event->event_id);
    switch(event->event_id){
    case QL_SYSTEM_EVENT_STA_GOT_IP:
        ql_debug("wlan_semp:%p\n", tls_wlan_semp);
        if(tls_wlan_semp != NULL){
            ql_rtos_semaphore_release(tls_wlan_semp);
        }
        
        break;
    default:
        break;
    }
    return 0;
}
static void ql_tls_client_debug(void * p, int level, const char * file, int line, const char * s)
{
	ql_debug("\n[%s:%d] %s\n", file, line, s);
}
static void ql_tls_client_init(tls_client_t  *client)
{
    mbedtls_ssl_init(&client->mbedSslCtx);

  	mbedtls_ssl_config_init(&client->mbedSslConf);

    mbedtls_net_init(&client->mbedNetCtx);

   	mbedtls_ctr_drbg_init(&client->mbedDrbgCtx);

	mbedtls_entropy_init(&client->mbedEtpyCtx);

	mbedtls_x509_crt_init(&client->mbedX509Ca);

	mbedtls_pk_init(&client->mbedPkUser);

   	mbedtls_x509_crt_init(&client->mbedX509User);
}
static void ql_tls_client_free(tls_client_t  *client)
{
    mbedtls_ssl_config_free(&client->mbedSslConf);
	mbedtls_x509_crt_free(&client->mbedX509Ca);
    mbedtls_net_free(&client->mbedNetCtx);
	mbedtls_x509_crt_free(&client->mbedX509User);
	mbedtls_pk_free(&client->mbedPkUser);
	mbedtls_ssl_free(&client->mbedSslCtx);
	mbedtls_ctr_drbg_free(&(client->mbedDrbgCtx));
    mbedtls_entropy_free(&(client->mbedEtpyCtx));
}

static int  ql_tls_client_cert_verify(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
    return 0;
}
static void ql_tls_demo_thread(void * arg)
{
    tls_client_t  tls_client;
    int  cs_id;
    const char ca_cert[] = ROOT_CA;
    int ca_len = sizeof(ca_cert);
    const char cli_cert[] = CLIENT_CERT;
    int  cli_cert_len = sizeof(cli_cert);
    const char cli_key[] = CLIENT_KEY;
    int  cli_key_len = sizeof(cli_key);
    char   pers[20] = {0};
    int mbed_ret = 0;
    unsigned int  cert_verify_item = 0;
    ql_network_init_s staconfig;
    char port[20] = {0};
    unsigned char recv_buf[512] = {0};

    ql_rtos_semaphore_create(&tls_wlan_semp, 0);
    ql_wlan_status_register_cb(ql_waln_event_cb);
    memset(&staconfig, 0, sizeof(staconfig));
    staconfig.wifi_mode = QL_STATION;
    memcpy(&staconfig.wifi_ssid, "iPhone", sizeof("iPhone"));
    memcpy(&staconfig.wifi_key, "12345678", sizeof("12345678"));
    staconfig.dhcp_mode = 1;
    if( ql_wlan_start(&staconfig) != 0){
        ql_debug("\n wifi sta startup failed!!!\n");
        goto end;
    }
    ql_debug("\n===wait wifi connect=====\n");
    ql_rtos_semaphore_wait(tls_wlan_semp, QL_WAIT_FOREVER);
  
    ql_debug("\n====start tls test=====\n");

    memset(&tls_client, 0x00, sizeof(tls_client_t));   
    ql_tls_client_init(&tls_client);

    mbedtls_ssl_config_defaults(&tls_client.mbedSslConf,
	                       	     MBEDTLS_SSL_IS_CLIENT,
	                       		 MBEDTLS_SSL_TRANSPORT_STREAM,
	                       		 MBEDTLS_SSL_PRESET_DEFAULT);

#if defined(MBEDTLS_ARC4_C)
    mbedtls_ssl_conf_arc4_support(&tls_client.mbedSslConf, MBEDTLS_SSL_ARC4_ENABLED);
#endif
    /*configure Tls version: TLS1.2*/
    mbedtls_ssl_conf_min_version(&tls_client.mbedSslConf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_0);

    mbedtls_ssl_conf_max_version(&tls_client.mbedSslConf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);   
    /*configure ciphersuite*/
    cs_id = mbedtls_ssl_get_ciphersuite_id("TLS-ECDHE-RSA-WITH-AES-128-CBC-SHA256");
    if(cs_id != 0){
        mbedtls_ssl_conf_ciphersuites(&tls_client.mbedSslConf, (const int *)&cs_id);
    }else{
        mbedtls_ssl_conf_ciphersuites(&tls_client.mbedSslConf, mbedtls_ssl_list_ciphersuites());
    }
    /*load ca cert*/
    if((mbed_ret = mbedtls_x509_crt_parse(&tls_client.mbedX509Ca, (const unsigned char *)ca_cert, ca_len)) != 0){
        ql_debug("mbedtls_x509_crt_parse err returned -0x%x\r\n", -mbed_ret);
        ql_tls_client_free(&tls_client);
        goto end;
    }
    mbedtls_ssl_conf_ca_chain(&tls_client.mbedSslConf, &tls_client.mbedX509Ca, NULL);
    /*load client cert and key, this step can omit*/
    if((mbed_ret = mbedtls_x509_crt_parse(&tls_client.mbedX509User, (const unsigned char *)cli_cert, cli_cert_len)) != 0){
		ql_debug("mbedtls_x509_crt_parse err returned -0x%x\r\n", -mbed_ret);
        ql_tls_client_free(&tls_client);
        goto end;
	}
	if((mbed_ret = mbedtls_pk_parse_key(&tls_client.mbedPkUser, (const unsigned char *)cli_key, cli_key_len, NULL, 0)) != 0){
		ql_debug("mbedtls_pk_parse_key err returned -0x%x\r\n", -mbed_ret);
        ql_tls_client_free(&tls_client);
        goto end;
	}
    /*configure verify level*/
    mbedtls_ssl_conf_authmode(&tls_client.mbedSslConf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_verify(&tls_client.mbedSslConf, ql_tls_client_cert_verify, (void *)cert_verify_item);
    
    snprintf(pers, 20, "ssl_test_%ld",  os_random()%1000);
	mbedtls_ctr_drbg_seed(&tls_client.mbedDrbgCtx, mbedtls_entropy_func, &tls_client.mbedEtpyCtx,
                          (const unsigned char *)pers, strlen(pers));
	mbedtls_ssl_conf_rng(&tls_client.mbedSslConf, mbedtls_ctr_drbg_random, &tls_client.mbedDrbgCtx);  
	
    /*config debug level*/
	mbedtls_ssl_conf_dbg(&tls_client.mbedSslConf, ql_tls_client_debug, "[MBEDTLS]");
    mbedtls_debug_set_threshold(1);
    mbedtls_ssl_setup(&tls_client.mbedSslCtx, &tls_client.mbedSslConf);
    /*configure SNI,this step can omit*/
    mbedtls_ssl_set_hostname(&tls_client.mbedSslCtx, remote_addr);
    snprintf(port, 20, "%d", remote_port);
    if ((mbed_ret = mbedtls_net_connect(&tls_client.mbedNetCtx, remote_addr, port, MBEDTLS_NET_PROTO_TCP)) != 0)
    {
        ql_debug("mbedtls_net_connect err returned -0x%x\r\n", -mbed_ret);
        ql_tls_client_free(&tls_client);
        goto end;
    }

    ql_debug("Connected %s:%d success...\r\n", remote_addr, remote_port);

    mbedtls_ssl_set_bio(&tls_client.mbedSslCtx, &tls_client.mbedNetCtx, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    while ((mbed_ret = mbedtls_ssl_handshake(&tls_client.mbedSslCtx)) != 0)
    {
        if (mbed_ret != MBEDTLS_ERR_SSL_WANT_READ && mbed_ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            ql_debug("mbedtls_ssl_handshake returned -0x%x\r\n", -mbed_ret);
            break;
        }
    }
    if(mbed_ret == 0){
        const unsigned char send_msg[] = REQ_MSG;
        mbedtls_ssl_write(&tls_client.mbedSslCtx, send_msg, sizeof(send_msg));

        while(1){
            memset(recv_buf, 0x00, 512);
            mbed_ret = mbedtls_ssl_read(&tls_client.mbedSslCtx, recv_buf, 512);
            if(mbed_ret > 0){
                ql_debug("%s\n", recv_buf);
            }else{
                if(mbed_ret != MBEDTLS_ERR_SSL_WANT_READ){
                    break;
                }
            }
        }
    }    
    mbedtls_ssl_close_notify(&tls_client.mbedSslCtx);
    ql_tls_client_free(&tls_client);
end:
    ql_rtos_semaphore_delete(tls_wlan_semp);
    ql_rtos_task_delete(NULL);
    
}

void ql_sl_tls_demo_thread_creat(void)
{

    int ret;
    ret = ql_rtos_task_create(&tls_test_thread_handle,
                              (unsigned short)1024 * 16,
                              RTOS_HIGHER_PRIORTIY_THAN(1),
                              "tls_test",
                              ql_tls_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        ql_debug("Error: Failed to create tls test thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (tls_test_thread_handle != NULL)
    {
        ql_rtos_task_delete(tls_test_thread_handle);
    }
}
