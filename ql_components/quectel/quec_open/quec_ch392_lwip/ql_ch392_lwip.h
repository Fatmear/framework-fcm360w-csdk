#ifndef _QL_CH392_LWIP_H_
#define _QL_CH392_LWIP_H_

#ifdef __cplusplus
extern "C"
{
#endif
/* Protocal Type */
#define PROTO_TYPE_IP_RAW                 0           /* IP */
#define PROTO_TYPE_MAC_RAW                1           /* MAC */
#define PROTO_TYPE_UDP                    2           /* UDP */
#define PROTO_TYPE_TCP                    3           /* TCP */



#define CH392_SOCKET_NUM  	4
#define TCPSERVER_SOCKET		0
#define TCPCONN_SOCKET_1		1
#define TCPCONN_SOCKET_2		2

//PHY状态指示
#define LINK_UP       1
#define LINK_DOWN     0
//SOCKET状态指示
#define SOCKET_OPEN      1
#define SOCKET_CLOSED    0
//TCP连接状态指示
#define TCP_CONNECTED       1
#define TCP_DISCONNECTED    0

typedef enum
{
	QL_WZ_ETHNETIF_NODE = 0,
	QL_WZ_ETHNETIF_GATEWAY,
}ql_wz_ethnetif_work_mode;


int ql_wz_ethnetif_register(uint8_t mac[6], char *ip_str, char *mask_str, char *gw_str, ql_wz_ethnetif_work_mode mode);
int ql_wz_ethnetif_set_dns(char *pri_str, char *sec_str);
int ql_wz_ethnetif_set_up(void);
int ql_wz_ethnetif_dhcp(void);
// int ql_wait392t_send_ready(void);
#ifdef __cplusplus
} /*"C" */
#endif

#endif