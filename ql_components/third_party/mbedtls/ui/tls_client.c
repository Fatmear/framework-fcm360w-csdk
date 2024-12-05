/*
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oshal.h"
#include "tls_client.h"
#include "tls_certificate.h"

// #define tls_printf os_printf(LM_OS, LL_INFO,"[TLS]");bk_printf

#if CFG_OUT_PUT_MBEDTLS_DEBUG_INFO
static void my_debug( void *ctx, int level,
                      const char *file, int line,
                      const char *str )
{
    ((void) level);

   // mbedtls_fprintf( (FILE *) ctx, "%s:%04d: %s", file, line, str );
    ///fflush(  (FILE *) ctx  );

    os_printf(LM_OS, LL_INFO,"[mbed]%s:%04d: %s\r\n", file, line, str);
}
#endif 

int mbedtls_client_init(MbedTLSSession *session, void *entropy, size_t entropyLen)
{
    int ret = 0;
    mbedtls_net_init(&session->server_fd);
    mbedtls_ssl_init(&session->ssl);
    mbedtls_ssl_config_init(&session->conf);
    mbedtls_ctr_drbg_init(&session->ctr_drbg);
    mbedtls_entropy_init(&session->entropy);
	#if USE_CA_CERTIFICATE_EN
    mbedtls_x509_crt_init(&session->cacert);
	#endif
	#if CFG_OUT_PUT_MBEDTLS_DEBUG_INFO
    mbedtls_ssl_conf_dbg( &session->conf, my_debug, 0 );
	#endif
    if((ret = mbedtls_ctr_drbg_seed(&session->ctr_drbg, mbedtls_entropy_func, &session->entropy,
                                    (unsigned char *)entropy, entropyLen)) != 0)
    {
        os_printf(LM_OS, LL_INFO,"mbedtls_ctr_drbg_seed returned -0x%x\r\n", -ret);
        return ret;
    }
    os_printf(LM_OS, LL_INFO,"mbedtls client struct init success...\r\n");

    return 0;
}

int mbedtls_client_close(MbedTLSSession *session)
{
    mbedtls_ssl_close_notify(&session->ssl);
    mbedtls_net_free(&session->server_fd);
#if USE_CA_CERTIFICATE_EN
    mbedtls_x509_crt_free(&session->cacert);
#endif
    mbedtls_entropy_free(&session->entropy);
    mbedtls_ctr_drbg_free(&session->ctr_drbg);
    mbedtls_ssl_config_free(&session->conf);
    mbedtls_ssl_free(&session->ssl);
	
    if(session->buffer){
        os_free((void *)session->buffer);
		session->buffer = NULL;
    }
	
    if(session->host){
        os_free((void *)session->host);
		session->host = NULL;
    }
    
    if(session->port){
        os_free((void *)session->port);
		session->port = NULL;
    }
    
    if(session){
        os_free((void *)session);
        session = NULL;
    }
    
    return 0;
}

int mbedtls_client_context(MbedTLSSession *session)
{
    int ret = 0;

    os_printf(LM_OS, LL_INFO,"Loading the CA root certificate success...\r\n");
#if CFG_USE_CA_CERTIFICATE
    ret = mbedtls_x509_crt_parse(&session->cacert, (const unsigned char *)mbedtls_root_certificate,
                                 mbedtls_root_certificate_len);
    if(ret < 0){
		os_printf(LM_OS, LL_INFO,"mbedtls_x509_crt_parse err returned -0x%x\r\n", -ret);
		return ret;
    }
#endif

    /* Hostname set here should match CN in server certificate */
    if((ret = mbedtls_ssl_set_hostname(&session->ssl, session->host)) != 0)
    {
        os_printf(LM_OS, LL_INFO,"mbedtls_ssl_set_hostname err returned -0x%x\r\n", -ret);
        return ret;
    }
	os_printf(LM_OS, LL_INFO,"[MBEDTLS]%s,%d \r\n",__FUNCTION__,__LINE__);

    if((ret = mbedtls_ssl_config_defaults(&session->conf,
                                          MBEDTLS_SSL_IS_CLIENT,
                                          MBEDTLS_SSL_TRANSPORT_STREAM,
                                          MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        os_printf(LM_OS, LL_INFO,"mbedtls_ssl_config_defaults returned -0x%x\n", -ret);
        return ret;
    }
#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
	unsigned char mfl_code = MBEDTLS_SSL_MAX_FRAG_LEN_4096;///MBEDTLS_SSL_MAX_FRAG_LEN_512;
	if( ( ret = mbedtls_ssl_conf_max_frag_len( &session->conf, mfl_code ) ) != 0 )
	{
		os_printf(LM_OS, LL_INFO, "[AM]failed\n  ! mbedtls_ssl_conf_max_frag_len returned %d\n\n", ret );
		return ret;
	}
	os_printf(LM_OS, LL_INFO,"[AM]ssl max len set succend\r\n");
#endif
    // mbedtls_ssl_conf_handshake_timeout(&session->conf,5*1000,30*1000);

#if CFG_USE_CA_CERTIFICATE_VERIFY
    os_printf(LM_OS, LL_INFO,"MBEDTLS_SSL_VERIFY_REQUIRED\r\n");
    mbedtls_ssl_conf_authmode(&session->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
#else
    os_printf(LM_OS, LL_INFO,"MBEDTLS_SSL_VERIFY_NONE\r\n");
	mbedtls_ssl_conf_authmode(&session->conf, MBEDTLS_SSL_VERIFY_NONE);
#endif
#if CFG_USE_CA_CERTIFICATE
	mbedtls_ssl_conf_ca_chain(&session->conf, &session->cacert, NULL);
#endif

    mbedtls_ssl_conf_rng(&session->conf, mbedtls_ctr_drbg_random, &session->ctr_drbg);

    if ((ret = mbedtls_ssl_setup(&session->ssl, &session->conf)) != 0)
    {
        os_printf(LM_OS, LL_INFO,"mbedtls_ssl_setup returned -0x%x\r\n", -ret);
        return ret;
    }
    os_printf(LM_OS, LL_INFO,"mbedtls client context init success...\r\n\n");

    return 0;
}

int mbedtls_client_connect(MbedTLSSession *session)
{
    int ret = 0;

    if ((ret = mbedtls_net_connect(&session->server_fd, session->host,
                                  session->port, MBEDTLS_NET_PROTO_TCP)) != 0)
    {
        os_printf(LM_OS, LL_INFO,"mbedtls_net_connect err returned -0x%x\r\n", -ret);
        return ret;
    }

    os_printf(LM_OS, LL_INFO,"Connected %s:%s success...\r\n", session->host, session->port);

    mbedtls_ssl_set_bio(&session->ssl, &session->server_fd, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    while ((ret = mbedtls_ssl_handshake(&session->ssl)) != 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            os_printf(LM_OS, LL_INFO,"mbedtls_ssl_handshake returned -0x%x\r\n", -ret);
            return ret;
        }
    }
#if (1) || (defined(CFG_USE_CA_CERTIFICATE) && (CFG_USE_CA_CERTIFICATE))
    if ((ret = mbedtls_ssl_get_verify_result(&session->ssl)) != 0)
    {
        os_printf(LM_OS, LL_INFO,"verify peer certificate fail....\r\n");
        memset(session->buffer, 0x00, session->buffer_len);
        mbedtls_x509_crt_verify_info((char *)session->buffer, session->buffer_len, "  ! ", ret);
        os_printf(LM_OS, LL_INFO,"verification info: %s\n", session->buffer);
    }
    else 
    {
        os_printf(LM_OS, LL_INFO,"Certificate verified success...\r\n");
    }
#endif
    return 0;
}

int mbedtls_client_read(MbedTLSSession *session, unsigned char *buf , size_t len)
{
    if(!session || !buf)
        return -1;

    return mbedtls_ssl_read(&session->ssl, (unsigned char *)buf, len);
}

int mbedtls_client_write(MbedTLSSession *session, const unsigned char *buf , size_t len)
{
    if(!session || !buf)
        return -1;

    return mbedtls_ssl_write(&session->ssl, (unsigned char *)buf, len);
}
