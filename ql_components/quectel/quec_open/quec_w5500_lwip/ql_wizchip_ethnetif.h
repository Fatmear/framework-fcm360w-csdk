#ifndef _QL_WIZCHIP_ETHNETIF_H_
#define _QL_WIZCHIP_ETHNETIF_H_

#include "lwip/ip_addr.h"

typedef struct
{
    uint8_t *mac;
    char *hostname;
    struct
    {
        int iptype;
        ip_addr_t ipaddr;
        ip_addr_t netmask;
        ip_addr_t gw;
        ip_addr_t dns_server[2];
    } ipv4_info;
} wz_ethnetif_ipconfig_t;

typedef enum {
	WZ_ETHNETIF_NODE = 0,
	WZ_ETHNETIF_GATEWAY,
}wz_ethnetif_work_mode;


int wz_ethnetif_register(uint8_t mac[6], char *ip_str, char *mask_str, char *gw_str,  wz_ethnetif_work_mode mode);
int wz_ethnetif_set_addr(char *ip_str, char *mask_str, char *gw_str);
int wz_ethnetif_set_dns(char *pri_str, char *sec_str);
int wz_ethnetif_set_up(void);
int wz_ethnetif_set_down(void);
int wz_ethnetif_dhcp(void);
void wz_ethnetif_ipconfig(wz_ethnetif_ipconfig_t *ipconfig);
int wz_ethnetif_set_defult_network_card(char *ip);
void wz_ethnetif_destroy(void);



#endif




