#ifndef _SL_TLS_H_
#define _SL_TLS_H_




#include "tls_client.h"

#define DEFAULT_MBEDTLS_READ_BUFFER               (1024U)


#include "mbedtls/platform.h"
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/certs.h"

#define CFG_USE_CA_CERTIFICATE                 0
#define CFG_USE_CA_CERTIFICATE_VERIFY          0

#define USE_CA_CERTIFICATE_EN                  ((CFG_USE_CA_CERTIFICATE) || (CFG_USE_CA_CERTIFICATE_VERIFY))

#define CFG_OUT_PUT_MBEDTLS_DEBUG_INFO         0



extern MbedTLSSession * ssl_create(char *url,char *port);
extern int ssl_txdat_sender(MbedTLSSession *tls_session,int len,char *data);
extern int ssl_read_data(MbedTLSSession *session,unsigned char *msg,unsigned int mlen,unsigned int timeout_ms);
extern int ssl_close(MbedTLSSession *session);

#endif
