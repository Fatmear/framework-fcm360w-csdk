#include <stdio.h>
#include "oshal.h"
#include "ql_spi.h"
#include "ql_spi_ch392.h"
#include "ql_api_osi.h"
#include "ql_ch392_lwip.h"
#include "ql_gpio.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"
#define USE_DHCP 1
ql_task_t ch392_lwip_thread_handle = NULL;
// ql_task_t ch392_dhcp_thread_handle = NULL;
// 用于存放本机MAC地址
uint8_t LOCAL_MAC[6];
#if USE_DHCP
#define CH392_IP "0.0.0.0"
#define CH392_SUB "0.0.0.0"
#define CH392_GW "0.0.0.0"
#else
#define CH392_IP "192.168.31.115"
#define CH392_SUB "255.255.255.0"
#define CH392_GW "192.168.31.1"
#endif

#define TCP_SERVER_DOMAIN "192.168.31.221"
#define TCP_SERVER_PORT 8000
#define TCP_CONNECT_TIMEOUT_S 10
#define TCP_RECV_TIMEOUT_S 3
#define TCP_CLOSE_LINGER_TIME_S 10
#define TCP_CLIENT_SEND_STR "GET / HTTP/1.1\r\nHost: www.baidu.com:80\r\n\r\n"

_CH392_INF ch392_inf = {0};
_CH392_SOCKINF ch392_sockinf[CH392_SOCKET_NUM];

/*******   SOCKET INFOMATION   *******/
// socket0.....TCP_SERVER
uint8_t prototype_0 = PROTO_TYPE_TCP;
uint16_t localport_0 = 2000;
void ql_ch392_lwip_demo_thread(void *param)
{
    // int ret = 0;
    // int sock_nbio = 1;
    // int sock_fd = -1;
    // int sock_error = 0;
    // socklen_t optlen = 0;
    // fd_set read_fds, write_fds;
    // struct timeval t;
    // struct addrinfo *res, hints;
    // struct sockaddr_in *ip4_svr_addr;
    // int dns_success = 0;
    // unsigned char recv_buf[1024] = {0};
    // struct netif *netif = NULL;
    // char ip[16];
    ql_rtos_task_sleep_ms(2000);
    // 对CH392T的接口进行配置
    drv_ch392_periph_init();
    ql_rtos_task_sleep_ms(100);
    // 对CH392T进行复位
    ch392_extenal_reset();
    ql_rtos_task_sleep_ms(500);
    ch395_cmd_reset();
    ql_rtos_task_sleep_ms(500);
    ch392_cmd_get_ver();
    // 对SPI接口进行检查
    while (drv_ch392_spi_check() != 0)
    {
        ql_rtos_task_sleep_ms(800);
    }
    // ch395_cmd_reset();
    ch392_dhcp_enable(0);
    // 读取本机MAC地址
    ch392_cmd_get_macaddr(LOCAL_MAC);
    ql_wz_ethnetif_register(LOCAL_MAC, (char *)CH392_IP, (char *)CH392_SUB, (char *)CH392_GW, QL_WZ_ETHNETIF_NODE);
    ql_wz_ethnetif_set_dns("8.8.8.8", "114.114.114.114");
#if USE_DHCP
    ql_wz_ethnetif_dhcp();
#else
    ql_wz_ethnetif_set_up();
#endif
    // ch392_set_macraw();
    // ch392_open_socket(0);
#if 0
    memset(&hints, 0, sizeof(struct addrinfo));
    // return;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(TCP_SERVER_DOMAIN, NULL, &hints, &res) != 0)
    {
        os_printf(LM_APP, LL_INFO, "*** DNS fail ***\r\n");
        goto exit;
    }

    dns_success = 1;

    ret = socket(AF_INET, SOCK_STREAM, 0);
    if (ret < 0)
    {
        goto exit;
    }

    sock_fd = ret;

    ioctlsocket(sock_fd, FIONBIO, &sock_nbio);

    ip4_svr_addr = (struct sockaddr_in *)res->ai_addr;
    ip4_svr_addr->sin_port = htons(TCP_SERVER_PORT);

    ret = connect(sock_fd, (struct sockaddr *)ip4_svr_addr, sizeof(struct sockaddr));

    if (ret == -1 && errno != EINPROGRESS)
    {
        goto exit;
    }

    t.tv_sec = TCP_CONNECT_TIMEOUT_S;
    t.tv_usec = 0;

    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);

    FD_SET(sock_fd, &read_fds);
    FD_SET(sock_fd, &write_fds);

    ret = select(sock_fd + 1, &read_fds, &write_fds, NULL, &t);

    if (ret <= 0)
    {
        goto exit;
    }

    if (!FD_ISSET(sock_fd, &read_fds) && !FD_ISSET(sock_fd, &write_fds))
    {
        goto exit;
    }
    else if (FD_ISSET(sock_fd, &read_fds) && FD_ISSET(sock_fd, &write_fds))
    {
        optlen = sizeof(sock_error);
        ret = getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &sock_error, &optlen);
        if (ret == 0 && sock_error == 0)
        {
        }
        else
        {
            goto exit;
        }
    }
    else if (!FD_ISSET(sock_fd, &read_fds) && FD_ISSET(sock_fd, &write_fds))
    {
    }
    else if (FD_ISSET(sock_fd, &read_fds) && !FD_ISSET(sock_fd, &write_fds))
    {

        goto exit;
    }
    else
    {

        goto exit;
    }

    ret = send(sock_fd, (const void *)TCP_CLIENT_SEND_STR, strlen(TCP_CLIENT_SEND_STR), 0);
    if (ret < 0)
    {

        goto exit;
    }

_recv_:

    t.tv_sec = 30;
    t.tv_usec = 0;

    FD_ZERO(&read_fds);
    FD_SET(sock_fd, &read_fds);

    ret = select(sock_fd + 1, &read_fds, NULL, NULL, &t);

    if (ret <= 0)
    {

        goto exit;
    }

    if (FD_ISSET(sock_fd, &read_fds))
    {
        ret = recv(sock_fd, recv_buf, sizeof(recv_buf), 0);
        if (ret > 0)
        {
            goto _recv_;
        }
        else if (ret == 0)
        {
            goto exit;
        }
        else
        {
            if (!(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
            {
                goto exit;
            }
            else
            {
                goto _recv_;
            }
        }
    }

exit:
    if (dns_success)
        freeaddrinfo(res);

    if (sock_fd >= 0)
    {
        struct linger linger = {0};

        linger.l_onoff = 1;
        linger.l_linger = TCP_CLOSE_LINGER_TIME_S;

        setsockopt(sock_fd, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger));

        close(sock_fd);
    }
#endif
    // 初始化使能CH392
    while (1)
    {
        // 主循环中断查询
        if (drv_ch392_int_status() == 0)
        {
            drv_ch392_global_int_process();
        }
        ql_rtos_task_sleep_ms(3);
    }
    while (1)
    {
        ql_rtos_task_sleep_ms(1000);
    }
    ql_rtos_task_delete(ch392_lwip_thread_handle);
}
void ql_ch392_lwip_demo_thread_creat(void)
{
    int ret;
    ret = ql_rtos_task_create(&ch392_lwip_thread_handle,
                              (unsigned short)1024 * 10,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "ch392_lwip",
                              ql_ch392_lwip_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO, "Error: Failed to create spi test thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (ch392_lwip_thread_handle != NULL)
    {
        ql_rtos_task_delete(ch392_lwip_thread_handle);
    }
}