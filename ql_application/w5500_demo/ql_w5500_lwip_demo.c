
#include <stdio.h>
#include "oshal.h"
#include "ql_spi.h"
#include "ql_api_osi.h"
#include "ql_wizchip_ethnetif.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"

#define USE_DHCP 0

#define W5500_MAC                          \
	{                                      \
		0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC \
	}
#define W5500_MAC_1                        \
	{                                      \
		0x00, 0x0e, 0xc6, 0x25, 0x8a, 0x9e \
	}

#if USE_DHCP
#define W5500_IP "0.0.0.0"
#define W5500_MASK "0.0.0.0"
#define W5500_GW "0.0.0.0"

#else
#define W5500_IP "192.168.31.115"
// #define W5500_MASK NULL
#define W5500_MASK "255.255.255.0"

#define W5500_GW "192.168.31.1"
#endif

#define TCP_SERVER_DOMAIN "192.168.31.221"
#define TCP_SERVER_PORT 8000
#define TCP_CONNECT_TIMEOUT_S 10
#define TCP_RECV_TIMEOUT_S 3
#define TCP_CLOSE_LINGER_TIME_S 10
#define TCP_CLIENT_SEND_STR "GET / HTTP/1.1\r\nHost: www.baidu.com:80\r\n\r\n"
ql_task_t w5500_lwip_thread_handle = NULL;

void w5500_lwip_thread(void *param)
{
	int ret = 0;
	int sock_nbio = 1;
	int sock_fd = -1;
	int sock_error = 0;
	socklen_t optlen = 0;
	fd_set read_fds, write_fds;
	struct timeval t;
	struct addrinfo *res, hints;
	struct sockaddr_in *ip4_svr_addr;
	int dns_success = 0;
	unsigned char recv_buf[1024] = {0};
	// struct netif *netif = NULL;
	// char ip[16];

	// ip_addr_t ip_t, mask, gw;

	ql_rtos_task_sleep_ms(3000);

	os_printf(LM_APP, LL_INFO, "wz_socket_test start\n");

	uint8_t mac[6] = W5500_MAC;

	if (wz_ethnetif_register(mac, W5500_IP, W5500_MASK, W5500_GW, WZ_ETHNETIF_NODE))
	{
		os_printf(LM_APP, LL_INFO, "W5500 NIC FAILLLLLLL");
		goto exit;
	}
	else
	{
		os_printf(LM_APP, LL_INFO, "W5500 NIC OKKKKKKKKK \n");
		wz_ethnetif_set_dns("8.8.8.8", "114.114.114.114");
#if USE_DHCP
		wz_ethnetif_dhcp();
#else
		wz_ethnetif_set_up();
#endif
	}

#if 1
	memset(&hints, 0, sizeof(struct addrinfo));
	// return;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(TCP_SERVER_DOMAIN, NULL, &hints, &res) != 0)
	{
		// printf("*** DNS fail ***\r\n");
		goto exit;
	}

	dns_success = 1;

	ret = socket(AF_INET, SOCK_STREAM, 0);
	if (ret < 0)
	{
		os_printf(LM_APP, LL_INFO, "*** socket create fail ***\r\n");
		goto exit;
	}

	sock_fd = ret;

	ioctlsocket(sock_fd, FIONBIO, &sock_nbio);

	ip4_svr_addr = (struct sockaddr_in *)res->ai_addr;
	ip4_svr_addr->sin_port = htons(TCP_SERVER_PORT);

	ret = connect(sock_fd, (struct sockaddr *)ip4_svr_addr, sizeof(struct sockaddr));

	os_printf(LM_APP, LL_INFO, "connect ret: %d, errno: %u\r\n", ret, errno);

	if (ret == -1 && errno != EINPROGRESS)
	{
		os_printf(LM_APP, LL_INFO, "*** connect fail ***\r\n");
		goto exit;
	}

	t.tv_sec = TCP_CONNECT_TIMEOUT_S;
	t.tv_usec = 0;

	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);

	FD_SET(sock_fd, &read_fds);
	FD_SET(sock_fd, &write_fds);

	ret = select(sock_fd + 1, &read_fds, &write_fds, NULL, &t);

	os_printf(LM_APP, LL_INFO, "select ret: %d\r\n", ret);

	if (ret <= 0)
	{
		os_printf(LM_APP, LL_INFO, "*** select timeout or error ***\r\n");
		// return;
		goto exit;
	}

	if (!FD_ISSET(sock_fd, &read_fds) && !FD_ISSET(sock_fd, &write_fds))
	{
		os_printf(LM_APP, LL_INFO, "*** connect fail ***\r\n");
		goto exit;
	}
	else if (FD_ISSET(sock_fd, &read_fds) && FD_ISSET(sock_fd, &write_fds))
	{
		optlen = sizeof(sock_error);
		ret = getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &sock_error, &optlen);
		if (ret == 0 && sock_error == 0)
		{
			os_printf(LM_APP, LL_INFO, "connect success\r\n");
		}
		else
		{
			os_printf(LM_APP, LL_INFO, "*** connect fail, sock_err = %d, errno = %u ***\r\n", sock_error, errno);
			goto exit;
		}
	}
	else if (!FD_ISSET(sock_fd, &read_fds) && FD_ISSET(sock_fd, &write_fds))
	{
		os_printf(LM_APP, LL_INFO, "connect success\r\n");
	}
	else if (FD_ISSET(sock_fd, &read_fds) && !FD_ISSET(sock_fd, &write_fds))
	{
		os_printf(LM_APP, LL_INFO, "*** connect fail ***\r\n");
		goto exit;
	}
	else
	{
		os_printf(LM_APP, LL_INFO, "*** connect fail ***\r\n");
		goto exit;
	}

	ret = send(sock_fd, (const void *)TCP_CLIENT_SEND_STR, strlen(TCP_CLIENT_SEND_STR), 0);
	if (ret < 0)
	{
		os_printf(LM_APP, LL_INFO, "*** send fail ***\r\n");
		goto exit;
	}

_recv_:

	// t.tv_sec = TCP_RECV_TIMEOUT_S;
	t.tv_sec = 30;
	t.tv_usec = 0;

	FD_ZERO(&read_fds);
	FD_SET(sock_fd, &read_fds);

	ret = select(sock_fd + 1, &read_fds, NULL, NULL, &t);

	os_printf(LM_APP, LL_INFO, "select ret: %d\r\n", ret);

	if (ret <= 0)
	{
		os_printf(LM_APP, LL_INFO, "*** select timeout or error ***\r\n");
		// return;
		goto exit;
	}

	if (FD_ISSET(sock_fd, &read_fds))
	{
		ret = recv(sock_fd, recv_buf, sizeof(recv_buf), 0);
		if (ret > 0)
		{
			os_printf(LM_APP, LL_INFO, "recv data: [%d]%s\r\n", ret, recv_buf);
			goto _recv_;
		}
		else if (ret == 0)
		{
			os_printf(LM_APP, LL_INFO, "*** peer closed ***\r\n");
			goto exit;
		}
		else
		{
			if (!(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
			{
				os_printf(LM_APP, LL_INFO, "*** error occurs ***\r\n");
				goto exit;
			}
			else
			{
				os_printf(LM_APP, LL_INFO, "wait for a while\r\n");
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
		//  setsockopt(sock_fd, IPPROTO_TCP, TCP_CLOSE_TIMEROUT, &linger.l_linger, sizeof(linger.l_linger));

		close(sock_fd);
	}

	ql_rtos_task_delete(w5500_lwip_thread_handle);
#endif
}
void ql_w5500_lwip_demo_thread_creat(void)
{
	int ret;
	ret = ql_rtos_task_create(&w5500_lwip_thread_handle,
							  (unsigned short)1024 * 10,
							  RTOS_HIGHER_PRIORTIY_THAN(5),
							  "w5500_lwip",
							  w5500_lwip_thread,
							  0);

	if (ret != QL_OSI_SUCCESS)
	{
		os_printf(LM_OS, LL_INFO, "Error: Failed to create spi test thread: %d\r\n", ret);
		goto init_err;
	}

	return;

init_err:
	if (w5500_lwip_thread_handle != NULL)
	{
		ql_rtos_task_delete(w5500_lwip_thread_handle);
	}
}