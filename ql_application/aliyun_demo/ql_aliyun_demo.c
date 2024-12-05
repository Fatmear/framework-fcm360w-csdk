#include "ql_https.h"

#include "ql_api_osi.h"
#include "ql_debug.h"
#include "cli.h"
#include "cJSON.h"
#include "mqtt_client.h"
#include "ql_rtc.h"
#define QL_MAX_MESSAGE_LENGTH 512
#define POST_RESP_BUFSZ 512
#define POST_HEADER_BUFSZ 256
#define GET_RESP_BUFSZ 512
#define QL_AIOT_DEVICE_DYNREG_REQUEST_LEN 512
#define HTTP_TIMEOUT 30 * 1000

char product_key[32] = {0};
char product_secret[32] = {0};
char device_name[32] = {0};
char device_secret[128] = {0};
char value = 0;
#define QL_ALINK_CLIENT_ID_STRINQL_MAX_LEN 256 // defined in Section 1.5.3
#define QL_ALINK_PRINT_STRINQL_MAX_LEN (CLI_PRINT_BUFFER_SIZE - 32)
extern int QL_MQTT_EVENT_CONNECTED;
extern int QL_MQTT_EVENT_DISCONNECTED;
extern int QL_MQTT_EVENT_SUBSCRIBED;
extern int QL_MQTT_EVENT_UNSUBSCRIBED;
extern int QL_MQTT_EVENT_PUBLISHED;
extern int QL_MQTT_EVENT_DATA;
extern int QL_MQTT_EVENT_ERROR;

extern int QL_MQTT_PINQL_SEND_OK;
extern int QL_MQTT_PINQL_SEND_NO;
extern int QL_MQTT_PINQL_OK;
extern int QL_MQTT_PINQL_NO;
extern int QL_mqtt;
static UINT32 ft_test(INT32 t, UINT32 x, UINT32 y, UINT32 z)
{
    UINT32 a, b, c = 0;

    if (t < 20)
    {
        a = x & y;
        b = (~x) & z;
        c = a ^ b;
    }
    else if (t < 40)
    {
        c = x ^ y ^ z;
    }
    else if (t < 60)
    {
        a = x & y;
        b = a ^ (x & z);
        c = b ^ (y & z);
    }
    else if (t < 80)
    {
        c = (x ^ y) ^ z;
    }

    return c;
}
static UINT32 rotl_test(INT32 bits, UINT32 a)
{
    unsigned long int c, d, e, f, g = 0;

    if (bits <= 31)
    {
        c = (0x0001 << (32 - bits)) - 1;
        d = ~c;

        e = (a & c) << bits;
        f = (a & d) >> (32 - bits);

        g = e | f;
    }

    return (g & 0xffffffff);
}
static UINT32 k_test(INT32 t)
{
    UINT32 c = 0;

    if (t < 20)
    {
        c = 0x5a827999;
    }
    else if (t < 40)
    {
        c = 0x6ed9eba1;
    }
    else if (t < 60)
    {
        c = 0x8f1bbcdc;
    }
    else if (t < 80)
    {
        c = 0xca62c1d6;
    }

    return c;
}
static INT32 quec_sha1_test(UINT8 *message, INT32 message_length, UINT8 *digest)
{
    INT32 i;
    INT32 num_blocks;
    INT32 block_remainder;
    INT32 padded_length;

    UINT32 l;
    UINT32 t;
    UINT32 h[5];
    UINT32 a, b, c, d, e;
    UINT32 w[80];
    UINT32 temp;
    INT32 digest_len = 0;

    /* Calculate the number of 512 bit blocks */

    padded_length = message_length + 8; /* Add length for l */
    padded_length = padded_length + 1;  /* Add the 0x01 bit postfix */

    l = message_length * 8;

    num_blocks = padded_length / 64;
    block_remainder = padded_length % 64;

    if (block_remainder > 0)
    {
        num_blocks++;
    }

    padded_length = padded_length + (64 - block_remainder);

    //  ECPLAT_PRINTF(UNILOG_ATCMD,ql_hmac_sha1_2, P_VALUE,1,"\n num_blocks:%d\n",num_blocks * 64);
    /* clear the padding field */
    for (i = message_length; i < (num_blocks * 64); i++)
    {
        message[i] = 0x00;
    }

    /* insert b1 padding bit */
    message[message_length] = 0x80;

    /* Insert l */
    message[(num_blocks * 64) - 1] = (unsigned char)(l & 0xff);
    message[(num_blocks * 64) - 2] = (unsigned char)((l >> 8) & 0xff);
    message[(num_blocks * 64) - 3] = (unsigned char)((l >> 16) & 0xff);
    message[(num_blocks * 64) - 4] = (unsigned char)((l >> 24) & 0xff);

    /* Set initial hash state */
    h[0] = 0x67452301;
    h[1] = 0xefcdab89;
    h[2] = 0x98badcfe;
    h[3] = 0x10325476;
    h[4] = 0xc3d2e1f0;

    for (i = 0; i < num_blocks; i++)
    {
        /* Prepare the message schedule */
        for (t = 0; t < 80; t++)
        {
            if (t < 16)
            {
                w[t] = (256 * 256 * 256) * message[(i * 64) + (t * 4)];
                w[t] += (256 * 256) * message[(i * 64) + (t * 4) + 1];
                w[t] += (256) * message[(i * 64) + (t * 4) + 2];
                w[t] += message[(i * 64) + (t * 4) + 3];
            }
            else if (t < 80)
            {
                w[t] = rotl_test(1, (w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16]));
            }
        }

        /* Initialize the five working variables */
        a = h[0];
        b = h[1];
        c = h[2];
        d = h[3];
        e = h[4];

        /* iterate a-e 80 times */

        for (t = 0; t < 80; t++)
        {
            temp = (rotl_test(5, a) + ft_test(t, b, c, d)) & 0xffffffff;
            temp = (temp + e) & 0xffffffff;
            temp = (temp + k_test(t)) & 0xffffffff;
            temp = (temp + w[t]) & 0xffffffff;
            e = d;
            d = c;
            c = rotl_test(30, b);
            b = a;
            a = temp;
        }

        /* compute the ith intermediate hash value */

        h[0] = (a + h[0]) & 0xffffffff;
        h[1] = (b + h[1]) & 0xffffffff;
        h[2] = (c + h[2]) & 0xffffffff;
        h[3] = (d + h[3]) & 0xffffffff;
        h[4] = (e + h[4]) & 0xffffffff;
    }
    digest[digest_len++] = (unsigned char)((h[0] >> 24) & 0xff);
    digest[digest_len++] = (unsigned char)((h[0] >> 16) & 0xff);
    digest[digest_len++] = (unsigned char)((h[0] >> 8) & 0xff);
    digest[digest_len++] = (unsigned char)(h[0] & 0xff);

    digest[digest_len++] = (unsigned char)((h[1] >> 24) & 0xff);
    digest[digest_len++] = (unsigned char)((h[1] >> 16) & 0xff);
    digest[digest_len++] = (unsigned char)((h[1] >> 8) & 0xff);
    digest[digest_len++] = (unsigned char)(h[1] & 0xff);

    digest[digest_len++] = (unsigned char)((h[2] >> 24) & 0xff);
    digest[digest_len++] = (unsigned char)((h[2] >> 16) & 0xff);
    digest[digest_len++] = (unsigned char)((h[2] >> 8) & 0xff);
    digest[digest_len++] = (unsigned char)(h[2] & 0xff);

    digest[digest_len++] = (unsigned char)((h[3] >> 24) & 0xff);
    digest[digest_len++] = (unsigned char)((h[3] >> 16) & 0xff);
    digest[digest_len++] = (unsigned char)((h[3] >> 8) & 0xff);
    digest[digest_len++] = (unsigned char)(h[3] & 0xff);

    digest[digest_len++] = (unsigned char)((h[4] >> 24) & 0xff);
    digest[digest_len++] = (unsigned char)((h[4] >> 16) & 0xff);
    digest[digest_len++] = (unsigned char)((h[4] >> 8) & 0xff);
    digest[digest_len++] = (unsigned char)(h[4] & 0xff);

    return digest_len;
}
INT32 ql_hmac_sha1_test(UINT8 *key, INT32 key_length, UINT8 *data, INT32 data_length, UINT8 *digest)
{
    INT32 b = 64; /* blocksize */
    UINT8 ipad = 0x36;

    UINT8 opad = 0x5c;

    UINT8 k0[64];
    UINT8 k0xorIpad[64];
    UINT8 step7data[64];
    UINT8 step5data[QL_MAX_MESSAGE_LENGTH + 128];
    UINT8 step8data[64 + 20];
    INT32 i;

    // ECPLAT_PRINTF(UNILOG_ATCMD,ql_hmac_sha1_1, P_VALUE,2,"\n data_length:%d,data_length+128:%d\n", data_length,data_length+128);
    for (i = 0; i < 64; i++)
    {
        k0[i] = 0x00;
    }

    if (key_length != b) /* Step 1 */
    {
        /* Step 2 */
        if (key_length > b)
        {
            int len = quec_sha1_test(key, key_length, digest);
            for (i = 0; i < len; i++)
            {
                k0[i] = digest[i];
            }
        }
        else if (key_length < b) /* Step 3 */
        {
            for (i = 0; i < key_length; i++)
            {
                k0[i] = key[i];
            }
        }
    }
    else
    {
        for (i = 0; i < b; i++)
        {
            k0[i] = key[i];
        }
    }
    /* Step 4 */
    for (i = 0; i < 64; i++)
    {
        k0xorIpad[i] = k0[i] ^ ipad;
    }
    /* Step 5 */
    for (i = 0; i < 64; i++)
    {
        step5data[i] = k0xorIpad[i];
    }
    for (i = 0; i < data_length; i++)
    {
        step5data[i + 64] = data[i];
    }

    /* Step 6 */
    quec_sha1_test(step5data, data_length + b, digest);

    /* Step 7 */
    for (i = 0; i < 64; i++)
    {
        step7data[i] = k0[i] ^ opad;
    }

    /* Step 8 */
    for (i = 0; i < 64; i++)
    {
        step8data[i] = step7data[i];
    }
    for (i = 0; i < 20; i++)
    {
        step8data[i + 64] = digest[i];
    }

    /* Step 9 */
    return quec_sha1_test(step8data, b + 20, digest);
}


//sha256
// void ql_sha256_test(const UINT8 *input, UINT32 ilen, UINT8 output[32])

static int ql_https_post(const char *uri, const void *post_data, uint16_t post_len,
                         const void *res_data, uint16_t *res_len)
{
    struct webclient_session *session = NULL;
    unsigned char *buffer = NULL;
    int index, ret = 0;
    int bytes_read, resp_status;

    buffer = (unsigned char *)os_malloc(POST_RESP_BUFSZ);
    if ((NULL == buffer) || (0 == *res_len))
    {
        ql_debug("ql_https_post input illegal\r\n");
        ret = -1;
        goto __exit;
    }

    /* create webclient session and set header response size */
    session = webclient_session_create(POST_RESP_BUFSZ);
    if (session == NULL)
    {
        ret = -1;
        goto __exit;
    }
    webclient_set_timeout(session, HTTP_TIMEOUT);
    /* build header for upload */
    webclient_header_fields_add(session, "Content-Length: %d\r\n", post_len);
    webclient_header_fields_add(session, "Content-Type: application/x-www-form-urlencoded\r\n");

    /* send POST request by default header */
    if ((resp_status = webclient_post(session, uri, post_data, post_len)) != 200)
    {
        ql_debug("webclient POST request failed, response(%d) error.\n", resp_status);
        ret = -1;
        goto __exit;
    }

    ql_debug("webclient post response data: \n");
    do
    {
        bytes_read = webclient_read(session, buffer, *res_len);
        if (bytes_read < 0)
        {
            ret = -1;
            ql_debug("time_out!\r\n");
            break;
        }
        else if (bytes_read > 0)
        {
            for (index = 0; index < bytes_read; index++)
            {
                ql_debug("%c", buffer[index]);
            }

            if (bytes_read > *res_len)
            {
                ret = -1;
                *res_len = 0;
                ql_debug("recv buffer is short!\r\n");
                break;
            }
            memcpy((void *)res_data, buffer, bytes_read);
            *res_len = bytes_read;
            break;
        }

    } while (1);

__exit:
    if (session)
    {
        webclient_close(session);
    }

    if (buffer)
    {
        os_free(buffer);
    }
    return ret;
}
// productKey=a1234******&deviceName=deviceName1234&rando=567345&sign=adfv123hdfdh&signMethod=HmacMD5
int ql_aliyun_register_device(cmd_tbl_t *t, int argc, char *argv[])
{
    char sign_content[256] = {0};
    unsigned int random = 0;
    char digset[20] = {0};
    char url_str[128] = {0};
    uint16_t len = 512;
    cJSON *root = NULL;
    cJSON *data = NULL;
    cJSON *device_secret_json = NULL;
    char *register_data = os_malloc(QL_AIOT_DEVICE_DYNREG_REQUEST_LEN);
    if (register_data != NULL)
    {
        memset(register_data, 0x00, QL_AIOT_DEVICE_DYNREG_REQUEST_LEN);
    }
    memcpy(device_name ,(char*)argv[1],strlen(argv[1]));
    memcpy(product_key ,(char*)argv[2],strlen(argv[2]));
    memcpy(product_secret ,(char*)argv[3],strlen(argv[3]));
    snprintf(url_str, 128, "https://iot-auth.%s.aliyuncs.com/auth/register/device", "cn-shanghai");
    random = os_random() % 65535;
    snprintf(sign_content, 256, "deviceName%sproductKey%srandom%d", device_name, product_key, random);
    ql_debug("\nsign_content:%s\n", sign_content);
    ql_hmac_sha1_test((UINT8 *)(product_secret), strlen(product_secret), (UINT8 *)sign_content, strlen(sign_content), (UINT8 *)digset);
    snprintf(register_data, 512, "productKey=%s&deviceName=%s&random=%d"
                                 "&sign=%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x&signMethod=hmacsha1",
             product_key, device_name, random,
             (digset[0] & 0xF0) >> 4, digset[0] & 0x0F, (digset[1] & 0xF0) >> 4, digset[1] & 0x0F, (digset[2] & 0xF0) >> 4, digset[2] & 0x0F,
             (digset[3] & 0xF0) >> 4, digset[3] & 0x0F, (digset[4] & 0xF0) >> 4, digset[4] & 0x0F, (digset[5] & 0xF0) >> 4, digset[5] & 0x0F,
             (digset[6] & 0xF0) >> 4, digset[6] & 0x0F, (digset[7] & 0xF0) >> 4, digset[7] & 0x0F, (digset[8] & 0xF0) >> 4, digset[8] & 0x0F,
             (digset[9] & 0xF0) >> 4, digset[9] & 0x0F, (digset[10] & 0xF0) >> 4, digset[10] & 0x0F, (digset[11] & 0xF0) >> 4, digset[11] & 0x0F,
             (digset[12] & 0xF0) >> 4, digset[12] & 0x0F, (digset[13] & 0xF0) >> 4, digset[13] & 0x0F, (digset[14] & 0xF0) >> 4, digset[14] & 0x0F,
             (digset[15] & 0xF0) >> 4, digset[15] & 0x0F, (digset[16] & 0xF0) >> 4, digset[16] & 0x0F, (digset[17] & 0xF0) >> 4, digset[17] & 0x0F,
             (digset[18] & 0xF0) >> 4, digset[18] & 0x0F, (digset[19] & 0xF0) >> 4, digset[19] & 0x0F);
    ql_debug("\nregister_data:%s\r\n", register_data);
    char *respones = (char *)os_malloc(512);
    memset(respones, 0, 512);
    ql_https_post(url_str, register_data, strlen(register_data), respones, &len);
    ql_debug("response:%s\r\n", respones);
    root = cJSON_Parse(respones);
    if (NULL == root)
    {
        ql_debug("root is NULL\r\n");
        goto exit;
    }
    data = cJSON_GetObjectItem(root, "data");
    if (data == NULL)
    {
        goto exit;
    }
    device_secret_json = cJSON_GetObjectItem(data, "deviceSecret");
    if (device_secret_json == NULL)
    {
        goto exit;
    }
    memcpy(device_secret, device_secret_json->valuestring, strlen(device_secret_json->valuestring));
    ql_debug("device_secrect:%s\r\n", device_secret);
exit:
    if (root != NULL)
    {
        cJSON_Delete(root);
    }
    return 0;
}

CLI_CMD_F(ql_aliyun_register, ql_aliyun_register_device, "ql_aliyun_register_device", "ql_aliyun_register_device", true);
//{"clientId":"ii81OYcmV9c.fcm360w_mqtt|securemode=2,signmethod=hmacsha256,timestamp=1681960501363|","username":"fcm360w_mqtt&ii81OYcmV9c","mqttHostUrl":"iot-06z00eugpgw8nhq.mqtt.iothub.aliyuncs.com","passwd":"a5df99754d9f35686635309410c4bf1fe5cd39ad24244f8d5bcd86a000bac63c","port":1883}
trs_mqtt_client_handle_t ql_alink_mqtt_client = NULL;
trs_mqtt_client_config_t ql_alink_mqtt_cfg = {0};
static int ql_alink_mqtt_handle(trs_mqtt_event_handle_t event)
{
    // trs_mqtt_client_handle_t client = event->client;
    // int msQL_id;
    //  your_context_t *context = event->context;
    cJSON *root = NULL;
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        if (QL_mqtt == 0)
        {
            QL_MQTT_EVENT_CONNECTED += 1;
            QL_mqtt = 1;
        }
        os_printf(LM_APP, LL_INFO, "MQTT_EVENT_CONNECTED:%d\n", QL_MQTT_EVENT_CONNECTED);
        break;

    case MQTT_EVENT_DISCONNECTED:
        if (QL_mqtt == 1)
        {
            QL_MQTT_EVENT_DISCONNECTED += 1;
            QL_mqtt = 0;
        }
        os_printf(LM_APP, LL_INFO, "MQTT_EVENT_DISCONNECTED:%d\n", QL_MQTT_EVENT_DISCONNECTED);

        break;

    case MQTT_EVENT_SUBSCRIBED:
        QL_MQTT_EVENT_SUBSCRIBED++;
        os_printf(LM_APP, LL_INFO, "MQTT_EVENT_SUBSCRIBED:%d, msQL_id=%d\n", QL_MQTT_EVENT_SUBSCRIBED, event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        QL_MQTT_EVENT_UNSUBSCRIBED++;
        os_printf(LM_APP, LL_INFO, "MQTT_EVENT_UNSUBSCRIBED:%d, msQL_id=%d\n", QL_MQTT_EVENT_UNSUBSCRIBED, event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        QL_MQTT_EVENT_PUBLISHED++;
        os_printf(LM_APP, LL_INFO, "MQTT_EVENT_PUBLISHED:%d, msQL_id=%d\n", QL_MQTT_EVENT_PUBLISHED, event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        QL_MQTT_EVENT_DATA++;
        os_printf(LM_APP, LL_INFO, "MQTT_EVENT_DATA:%d\n", QL_MQTT_EVENT_DATA);
        os_printf(LM_APP, LL_INFO, "TOPIC_len=%d, DATA_len= %d\r\n", event->topic_len, event->data_len);
        if (event->topic_len < QL_ALINK_PRINT_STRINQL_MAX_LEN)
        {
            os_printf(LM_APP, LL_INFO, "TOPIC=%.*s\r\n", event->topic_len, event->topic);
        }
        else
        {
            os_printf(LM_APP, LL_INFO, "The length of TOPIC exceeds the length of the print buffer, so do't printf!\r\n");
        }

        if (event->data_len < QL_ALINK_PRINT_STRINQL_MAX_LEN)
        {
            if (event->data_len == 0)
            {
                os_printf(LM_APP, LL_INFO, "DATA is null!\r\n");
            }
            else
            {
                os_printf(LM_APP, LL_INFO, "DATA=%.*s\r\n", event->data_len, event->data);
            }
           root = cJSON_Parse(event->data);
           cJSON *property = cJSON_GetObjectItem(root,"params");
           cJSON *property_mode = cJSON_GetObjectItem(property,"LightStatus");
           value = property_mode->valueint;
        }
        else
        {
            os_printf(LM_APP, LL_INFO, "The length of DATA exceeds the length of the print buffer, so do't printf!\r\n");
        }

        break;
    case MQTT_EVENT_ERROR:
        QL_MQTT_EVENT_ERROR++;
        os_printf(LM_APP, LL_INFO, "MQTT_EVENT_ERROR:%d\n", QL_MQTT_EVENT_ERROR);
        break;
    }
    return 0;
}
//a1RDSALHOsY.iot-as-mqtt.cn-shanghai.aliyuncs.com
int ql_aliyun_mqtt_device(cmd_tbl_t *t, int argc, char *argv[])
{
    char digset[20] = {0};
    char url_str[128] = {0};
    char client_id_str[256] = {0};
    char content[256] = {0};
    char identity[64] = {0};
    // char device_connect_id[32] = {0};
    char username_str_test[256] = {0};
    // char password_str_test[128] = "5f56a4eb3be1247d0d13167936d4229bfad46b23be17f8b18b2be0a18e6b63e5";
    char password_str_test[256] = {0};
    uint32_t tv_msec;
    tv_msec = ql_rtc_get_time();
    memset(&ql_alink_mqtt_cfg, 0, sizeof(trs_mqtt_client_config_t));
    // memcpy(device_connect_id ,(char*)argv[1],strlen(argv[1]));
    ql_alink_mqtt_cfg.event_handle = ql_alink_mqtt_handle;
    snprintf(url_str, 128, "mqtt://%s.iot-as-mqtt.cn-shanghai.aliyuncs.com", product_key);
    snprintf(identity, 64, "%s.%s", product_key, device_name);
    snprintf(client_id_str, 256, "%s|securemode=%d,signmethod=hmacsha1,timestamp=%ld|", identity, 2, tv_msec);

    snprintf((char *)username_str_test, 256, "%s&%s", device_name, product_key);

    snprintf((char *)content, 512, "clientId%sdeviceName%sproductKey%stimestamp%ld", identity, device_name, product_key, tv_msec);

    ql_hmac_sha1_test((unsigned char *)device_secret, strlen((char *)device_secret), (unsigned char *)content, strlen((char *)content), (unsigned char *)digset);
    // ql_debug("username_str_test %s password_str_test %s\r\n");
    snprintf((char *)password_str_test, 256, "%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x", 
			 	              (digset[0]&0xF0)>>4, digset[0]&0x0F,(digset[1]&0xF0)>>4, digset[1]&0x0F, (digset[2]&0xF0)>>4, digset[2]&0x0F,
			                     (digset[3]&0xF0)>>4, digset[3]&0x0F,(digset[4]&0xF0)>>4, digset[4]&0x0F, (digset[5]&0xF0)>>4, digset[5]&0x0F,
			                     (digset[6]&0xF0)>>4, digset[6]&0x0F,(digset[7]&0xF0)>>4, digset[7]&0x0F, (digset[8]&0xF0)>>4, digset[8]&0x0F,
                                          (digset[9]&0xF0)>>4, digset[9]&0x0F,(digset[10]&0xF0)>>4, digset[10]&0x0F, (digset[11]&0xF0)>>4, digset[11]&0x0F,
	                                   (digset[12]&0xF0)>>4, digset[12]&0x0F,(digset[13]&0xF0)>>4, digset[13]&0x0F, (digset[14]&0xF0)>>4, digset[14]&0x0F,
                                          (digset[15]&0xF0)>>4, digset[15]&0x0F,(digset[16]&0xF0)>>4, digset[16]&0x0F, (digset[17]&0xF0)>>4, digset[17]&0x0F,
			                     (digset[18]&0xF0)>>4, digset[18]&0x0F,(digset[19]&0xF0)>>4, digset[19]&0x0F);
    ql_debug("username_str_test %s password_str_test %s\r\n",username_str_test,password_str_test);
    ql_alink_mqtt_cfg.uri = url_str;
    ql_alink_mqtt_cfg.transport = MQTT_TRANSPORT_OVER_TCP;
    ql_alink_mqtt_cfg.port = 1883;
    ql_alink_mqtt_cfg.client_id = client_id_str;
    ql_alink_mqtt_cfg.username = username_str_test;
    ql_alink_mqtt_cfg.password = password_str_test;
    ql_alink_mqtt_client = trs_mqtt_client_init(&ql_alink_mqtt_cfg);
    if (0 != trs_mqtt_client_start(ql_alink_mqtt_client))
    {
        os_printf(LM_APP, LL_INFO, "mqtt start failed\r\n");
        trs_mqtt_client_destroy(ql_alink_mqtt_client);
        ql_alink_mqtt_client = NULL;
        return CMD_RET_FAILURE;
    }
    return 0;
}
CLI_CMD_F(ql_aliyun_mqtt, ql_aliyun_mqtt_device, "ql_aliyun_mqtt_device", "ql_aliyun_mqtt_device", true);
int ql_aliyun_mqtt_sub_topic(cmd_tbl_t *t, int argc, char *argv[])
{
    // char *sub_topic = "/ii81OYcmV9c/fcm360w_mqtt/user/get";
    char sub_topic[128] ={0};
    snprintf((char *)sub_topic, 128, "/sys/%s/%s/thing/service/property/set", product_key, device_name);
    int topic_len = strlen(sub_topic);
    MQTT_CFG_MSG_ST msg_send = {0};
    set_mqtt_msg(SET_TOPIC, &msg_send, sub_topic, topic_len);
    msg_send.cfg_type = CFG_MSG_TYPE_SUB;
    msg_send.qos = 0;
    if (ql_alink_mqtt_client->cfg_queue_handle != NULL)
    {
        os_queue_send(ql_alink_mqtt_client->cfg_queue_handle, (char *)&msg_send, sizeof(msg_send), 0);
    }
    return 0;
}
CLI_CMD_F(ql_aliyun_mqtt_sub, ql_aliyun_mqtt_sub_topic, "ql_aliyun_mqtt_sub_topic", "ql_aliyun_mqtt_sub_topic", true);

int ql_aliyun_mqtt_pub_topic(cmd_tbl_t *t, int argc, char *argv[])
{
    // char *pub_topic = "/ii81OYcmV9c/fcm360w_mqtt/user/update";
    // char *pub_payload = "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"fcm360w_mqtt\":0}}";
    ///sys/a1ERUX9M46h/fcm360w_test/thing/service/property/set
    char pub_topic[128] ={0};
    snprintf((char *)pub_topic, 128, "/sys/%s/%s/thing/event/property/post", product_key, device_name);
    int topic_len = strlen(pub_topic);

    char pub_payload[128] ={0};
    snprintf((char *)pub_payload, 128, "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"%s\":%d}}", "LightStatus",value);
    // int topic_len = strlen(pub_topic);
    MQTT_CFG_MSG_ST msg_send = {0};
    set_mqtt_msg(SET_TOPIC, &msg_send, pub_topic, topic_len);

    set_mqtt_msg(SET_DATA, &msg_send, pub_payload, strlen(pub_payload));

    msg_send.qos = 0;
    msg_send.retain = 1;
    msg_send.cfg_type = CFG_MSG_TYPE_PUB;
    if (ql_alink_mqtt_client->cfg_queue_handle != NULL)
    {
        os_queue_send(ql_alink_mqtt_client->cfg_queue_handle, (char *)&msg_send, sizeof(msg_send), 0);
    }
    return 0;
}
CLI_CMD_F(ql_aliyun_mqtt_pub, ql_aliyun_mqtt_pub_topic, "ql_aliyun_mqtt_pub_topic", "ql_aliyun_mqtt_pub_topic", true);