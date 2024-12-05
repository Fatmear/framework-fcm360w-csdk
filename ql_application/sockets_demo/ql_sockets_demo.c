
/**  @file
  ql_sockets_demo.c

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ql_type.h"
#include "ql_debug.h"
#include "ql_api_osi.h"
#include "lwip/api.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/ip4_addr.h"
#include "ql_wlan.h"
#include "stdint.h"
#include "http_server_service.h"

#define QL_SOCKET_DEMO_URL "112.31.84.164"
#define QL_SOCKET_DEMO_PORT 8305

#define WIFI_SSID   "zxcvbnm"
#define WIFI_KEY    "12345678"

//#include "http_server_service.h"

#define TCP_CONNECT_TIMEOUT_S 10
#define TCP_RECV_TIMEOUT_S 10
#define TCP_CLOSE_LINGER_TIME_S 10
#define TCP_CLIENT_SEND_STR "tcp client send string"
#define PROFILE_IDX 1

ql_task_t socket_client_thread_handle = NULL;
ql_task_t socket_server_thread_handle = NULL;
struct ip_port_msg
{
	ip4_addr_t IP;
	in_port_t sin_port;
};
void do_tcp_client_thread(void *arg)
{
	int ret = 0;
	int sock_fd = -1;
	int sock_error = 0;
	socklen_t optlen = 0;
	int sock_nbio = 0;

	fd_set read_fds, write_fds;
	struct timeval t;

	u8 recv_buf[128] = {0};

	int err = 0;
	struct ip_port_msg *i_pMsg;
	struct sockaddr_in server_addr;

	struct sockaddr_in ip4_local_addr = {0};

	i_pMsg = (struct ip_port_msg *)arg;
	if (i_pMsg == NULL)
	{
		goto exit;
	}

	ret = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ret < 0)
	{
		ql_debug("*** socket create fail ***\r\n");
		goto exit;
	}
	sock_fd = ret;

	ioctl(sock_fd, FIONBIO, &sock_nbio);
	int on = 1;
	ip4_local_addr.sin_family = AF_INET;
	ip4_local_addr.sin_port = htons(2023);
	ip4_local_addr.sin_addr.s_addr = INADDR_ANY;
	setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	on = 1;
	setsockopt(sock_fd,SOL_SOCKET,SO_REUSEPORT,&on,sizeof(on));
	ret = bind(sock_fd, (struct sockaddr *)&ip4_local_addr, sizeof(ip4_local_addr));
	if (ret < 0)
	{
		ql_debug("*** bind fail ***\r\n");
		goto exit;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = i_pMsg->IP.addr;	// 0x0728a8c0 /*192.168.40.7*/
	server_addr.sin_port = htons(i_pMsg->sin_port); // htons( 12345 );

	ret = connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret < 0)
	{
		ql_debug("connect err: %d\r\n", err);
		goto exit;
	}

	t.tv_sec = TCP_CONNECT_TIMEOUT_S;
	t.tv_usec = 0;

	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);

	FD_SET(sock_fd, &read_fds);
	FD_SET(sock_fd, &write_fds);

	ret = select(sock_fd + 1, &read_fds, &write_fds, NULL, &t);

	ql_debug("select ret: %d\r\n", ret);

	if (ret <= 0)
	{
		ql_debug("*** select timeout or error ***\r\n");
		goto exit;
	}

	if (!FD_ISSET(sock_fd, &read_fds) && !FD_ISSET(sock_fd, &write_fds))
	{
		ql_debug("*** connect fail ***\r\n");
		goto exit;
	}
	else if (FD_ISSET(sock_fd, &read_fds) && FD_ISSET(sock_fd, &write_fds))
	{
		optlen = sizeof(sock_error);
		ret = getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &sock_error, &optlen);
		if (ret == 0 && sock_error == 0)
		{
			ql_debug("connect success\r\n");
		}
		else
		{
			ql_debug("*** connect fail, sock_err = %d, errno = %u ***\r\n", sock_error, errno);
			goto exit;
		}
	}
	else if (!FD_ISSET(sock_fd, &read_fds) && FD_ISSET(sock_fd, &write_fds))
	{
		ql_debug("connect success\r\n");
	}
	else if (FD_ISSET(sock_fd, &read_fds) && !FD_ISSET(sock_fd, &write_fds))
	{
		ql_debug("*** connect fail ***\r\n");
		goto exit;
	}
	else
	{
		ql_debug("*** connect fail ***\r\n");
		goto exit;
	}

	ret = send(sock_fd, (const void *)TCP_CLIENT_SEND_STR, strlen(TCP_CLIENT_SEND_STR), 0);

	if (ret < 0)
	{
		ql_debug("*** send fail ***\r\n");
		goto exit;
	}

_recv_:

	FD_ZERO(&read_fds);
	FD_SET(sock_fd, &read_fds);

	ret = select(sock_fd + 1, &read_fds, NULL, NULL, NULL);

	ql_debug("select ret: %d\r\n", ret);

	if (ret <= 0)
	{
		ql_debug("*** select timeout or error ***\r\n");
		goto exit;
	}
	if (FD_ISSET(sock_fd, &read_fds))
	{
		ret = recv(sock_fd, recv_buf, sizeof(recv_buf), 0);
		if (ret > 0)
		{
			ql_debug("recv data: [%d]%s\r\n", ret, recv_buf);
			while(1)
			{
				ql_rtos_task_sleep_ms(20);
				ret = recv(sock_fd, recv_buf, sizeof(recv_buf), 0);
				if(ret == -1)
				{
					ql_debug("*** recv error ***\r\n");
					goto exit;
				}
				ql_debug("recv data: [%d]%s\r\n", ret, recv_buf);
			}
		}
		else if (ret == 0)
		{
			ql_debug("*** peer closed ***\r\n");
			goto exit;
		}
		else
		{
			if (!(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
			{
				ql_debug("*** error occurs ***\r\n");
				goto exit;
			}
			else
			{
				ql_debug("wait for a while\r\n");
				ql_rtos_task_sleep_ms(20);
				goto _recv_;
			}
		}
	}

exit:
	if (sock_fd >= 0)
	{
		struct linger linger = {0};

		linger.l_onoff = 1;
		linger.l_linger = TCP_CLOSE_LINGER_TIME_S;

		setsockopt(sock_fd, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger));
		setsockopt(sock_fd, IPPROTO_TCP, TCP_KEEPALIVE, &linger.l_linger, sizeof(linger.l_linger));
		close(sock_fd);
	}
	ql_rtos_task_delete(socket_client_thread_handle);
}


void do_tcp_server_thread(void *arg)
{
	int ret = 0;
	int sock_fd = -1;
	// int sock_error = 0;
	// socklen_t optlen = 0;
	int sock_nbio = 0;

	// fd_set read_fds, write_fds;
	// struct timeval t;

	u8 recv_buf[1024] = {0};

	// int err = 0;
	struct ip_port_msg *i_pMsg;
	struct sockaddr_in server_addr;

	// struct sockaddr_in ip4_local_addr = {0};

	i_pMsg = (struct ip_port_msg *)arg;
	if (i_pMsg == NULL)
	{
		goto exit;
	}

	ret = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ret < 0)
	{
		ql_debug("*** socket create fail ***\r\n");
		goto exit;
	}
	sock_fd = ret;

	ioctl(sock_fd, FIONBIO, &sock_nbio);
	int on = 1;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = i_pMsg->IP.addr;	// 0x0728a8c0 /*192.168.40.7*/
	server_addr.sin_port = htons(i_pMsg->sin_port); // htons( 12345 );
	setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	on = 1;
	setsockopt(sock_fd,SOL_SOCKET,SO_REUSEPORT,&on,sizeof(on));
	ret = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret < 0)
	{
		ql_debug("*** bind fail ***\r\n");
		goto exit;
	}

	// server_addr.sin_family = AF_INET;
	// server_addr.sin_addr.s_addr = i_pMsg->IP.addr;	// 0x0728a8c0 /*192.168.40.7*/
	// server_addr.sin_port = htons(i_pMsg->sin_port); // htons( 12345 );
	if(listen(sock_fd, 5) == -1)
	{
		ql_debug("*** listen fail ***\r\n");
		goto exit;
	}
	struct sockaddr_in clientaddr;
	int len = sizeof(clientaddr);
	int client_fd = accept(sock_fd,(struct sockaddr *)&clientaddr,(socklen_t *)&len);
	if(client_fd == -1)
	{
		ql_debug("*** accept fail ***\r\n");
		goto exit;
	}
	ql_debug("*** connect succ ***\r\n");
	while(1)
	{
		int num = recv(client_fd,recv_buf,sizeof(recv_buf),0);
		if(num == -1)
		{
			ql_debug("*** accept fail ***\r\n");
			goto exit;
		}
		recv_buf[num]= '\0';
		ql_debug("recv data: [%d]%s\r\n", ret, recv_buf);
	}
exit:
	if (sock_fd >= 0)
	{
		struct linger linger = {0};

		linger.l_onoff = 1;
		linger.l_linger = TCP_CLOSE_LINGER_TIME_S;

		setsockopt(sock_fd, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger));
		setsockopt(sock_fd, IPPROTO_TCP, TCP_KEEPALIVE, &linger.l_linger, sizeof(linger.l_linger));
		close(sock_fd);
	}
	ql_rtos_task_delete(socket_server_thread_handle);
}

__attribute__((unused)) static int ql_cmd_tcp_client_test(cmd_tbl_t *t, int argc, char *argv[])
{
	struct ip_port_msg *i_pMsg = NULL;
	int port;
	i_pMsg = (struct ip_port_msg *)os_malloc(sizeof(struct ip_port_msg));
	if (!inet_aton(argv[1], &(i_pMsg->IP.addr)))
	{
		ql_debug("inet_aton Failed\r\n");
		goto exit;
	}
	port = (int)strtoul(argv[2], NULL, 0);
	i_pMsg->sin_port = port;
	int ret = ql_rtos_task_create(socket_client_thread_handle,
								  (unsigned short)1024 * 4,
								  RTOS_HIGHER_PRIORTIY_THAN(3),
								  "connt_server",
								  do_tcp_client_thread,
								  (void *)i_pMsg);
	if (ret != TRUE)
	{
		os_printf(LM_OS, LL_INFO, "Error: Failed to create at_ssl thread: %d\r\n", ret);
		return CMD_RET_FAILURE;
	}
exit:
	if (i_pMsg)
	{
		os_free(i_pMsg);
	}
	return CMD_RET_SUCCESS;
}


__attribute__((unused)) static int ql_cmd_tcp_server_test(cmd_tbl_t *t, int argc, char *argv[])
{
	struct ip_port_msg *i_pMsg = NULL;
	int port;
	i_pMsg = (struct ip_port_msg *)os_malloc(sizeof(struct ip_port_msg));
	if (!inet_aton(argv[1], &(i_pMsg->IP.addr)))
	{
		ql_debug("inet_aton Failed\r\n");
		goto exit;
	}
	port = (int)strtoul(argv[2], NULL, 0);
	i_pMsg->sin_port = port;
	int ret = ql_rtos_task_create(socket_client_thread_handle,
								  (unsigned short)1024 * 4,
								  RTOS_HIGHER_PRIORTIY_THAN(3),
								  "creat_server",
								  do_tcp_server_thread,
								  (void *)i_pMsg);
	if (ret != TRUE)
	{
		os_printf(LM_OS, LL_INFO, "Error: Failed to create at_ssl thread: %d\r\n", ret);
		return CMD_RET_FAILURE;
	}
exit:
	if (i_pMsg)
	{
		os_free(i_pMsg);
	}
	return CMD_RET_SUCCESS;
}



ql_task_t socket_thread_handle = NULL;

void ql_sockets_demo_thread(void *arg)
{
	int ret = 0;
	int i = 0;
	fd_set read_fds;
	fd_set write_fds;
	fd_set exp_fds;
	int socket_fd = -1;
	int flags = 0;
	int fd_changed = 0;
	int connected = 0;
	int closing = FALSE;
	struct sockaddr_in local4, server_ipv4;
	struct addrinfo *pres = NULL;
	struct addrinfo *temp = NULL;
	char send_buf[128] = {0};
	int send_len = 0;
	char recv_buf[128] = {0};
	int recv_len = 0;

	ql_rtos_task_sleep_ms(2000);
	ql_debug("========== socket demo start ==========\n");

	ql_rtos_task_sleep_ms(4000);

    ql_network_init_s wificonfig;
    memset(&wificonfig, 0, sizeof(wificonfig));
    wificonfig.wifi_mode = QL_STATION;
    memcpy(&wificonfig.wifi_ssid, WIFI_SSID, sizeof(WIFI_SSID));
    memcpy(&wificonfig.wifi_key, WIFI_KEY, sizeof(WIFI_KEY));
    wificonfig.dhcp_mode = 1;
    os_printf(LM_APP, LL_INFO, "wlan start  %s     %s\r\n",wificonfig.wifi_ssid,wificonfig.wifi_key);
    ret = ql_wlan_start(&wificonfig);
    if(ret != QL_WIFI_SUCCESS)
    {
        os_printf(LM_APP, LL_INFO, "wlan start failed %d\r\n",ret);
        goto exit;
    }
    ql_rtos_task_sleep_ms(5000);

	memset(&local4, 0x00, sizeof(struct sockaddr_in));
	local4.sin_family = AF_INET;
	local4.sin_port = 2020;

loop:
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_ZERO(&exp_fds);
	flags = 0;
	connected = 0;
	closing = FALSE;
	i = 0;
	ql_debug("===socket demo %d====\n", __LINE__);

	ret = lwip_getaddrinfo(QL_SOCKET_DEMO_URL, NULL, NULL, &pres);
	if (ret < 0 || pres == NULL)
	{
		ql_debug("DNS getaddrinfo failed! ret=%d; pres=%p!\n", ret, pres);
		goto exit;
	}

	for (temp = pres; temp != NULL; temp = temp->ai_next)
	{
		struct sockaddr_in *sin_res = (struct sockaddr_in *)temp->ai_addr;
		if (temp->ai_family == AF_INET)
		{
			socket_fd = socket(temp->ai_family, SOCK_STREAM, 0);
			if (socket_fd < 0)
			{
				continue;
			}

			ret = bind(socket_fd, (struct sockaddr *)&local4, sizeof(struct sockaddr));
			if (ret < 0)
			{
				close(socket_fd);
				socket_fd = -1;
				continue;
			}

			flags |= O_NONBLOCK;
			fcntl(socket_fd, F_SETFL, flags);

			memset(&server_ipv4, 0x00, sizeof(struct sockaddr_in));
			server_ipv4.sin_family = temp->ai_family;
			server_ipv4.sin_port = htons(QL_SOCKET_DEMO_PORT);
			server_ipv4.sin_addr = sin_res->sin_addr;

			ret = connect(socket_fd, (struct sockaddr *)&server_ipv4, sizeof(server_ipv4));
			ql_debug("===socket demo %d====", __LINE__);
			if (ret == 0)
			{
				connected = 1;
				break;
			}
			else
			{
				ql_debug("===socket demo %d====\n", __LINE__);
				// goto exit;
				break;
			}
		}
	}
	if (socket_fd < 0)
	{
		ql_debug("===socket demo %d====", __LINE__);
		goto exit;
	}
	ql_debug("===socket demo %d====", __LINE__);
	if (connected == 1)
	{
		FD_SET(socket_fd, &read_fds);
		ql_debug("=====connect to \"%s:%d\" success=====\n",QL_SOCKET_DEMO_URL, QL_SOCKET_DEMO_PORT);
		memset(send_buf, 0x00, 128);
		send_len = snprintf(send_buf, 128, "%d%s%d\r\n", i, "startAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAend", i);

		if (write(socket_fd, send_buf, send_len) != send_len)
		{
			FD_SET(socket_fd, &write_fds);
		}
		else
		{
			i++;
		}
	}
	else
	{
		FD_SET(socket_fd, &write_fds);
	}
	FD_SET(socket_fd, &exp_fds);

	while (1)
	{
		ql_debug("===socket demo %d====\n", __LINE__);
		fd_changed = select(socket_fd + 1, &read_fds, &write_fds, &exp_fds, NULL);
		ql_debug("===socket demo %d====\n", __LINE__);
		if (fd_changed > 0)
		{
			if (FD_ISSET(socket_fd, &write_fds))
			{
				FD_CLR(socket_fd, &write_fds);
				if (connected == 0)
				{
					int value = 0;
					socklen_t len = 0;

					len = sizeof(value);
					getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &value, &len);

					ql_debug("errno: %d", value);
					if (value == 0 || value == EISCONN)
					{
						ql_debug("=====connect to \"%s:%d\" success=====\n",QL_SOCKET_DEMO_URL, QL_SOCKET_DEMO_PORT);
						connected = 1;
						FD_SET(socket_fd, &read_fds);

						memset(send_buf, 0x00, 128);
						send_len = snprintf(send_buf, 128, "%d%s%d\r\n", i, "startAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAend", i);
						write(socket_fd, send_buf, send_len);
						i++;
					}
					else
					{
						ql_debug("=====connect to \"%s:%d\" failed=====\n",QL_SOCKET_DEMO_URL, QL_SOCKET_DEMO_PORT);
						close(socket_fd);
						socket_fd = -1;
						break;
					}
				}
				else
				{
					memset(send_buf, 0x00, 128);
					send_len = snprintf(send_buf, 128, "%d%s%d\r\n", i, "startAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAend", i);
					write(socket_fd, send_buf, send_len);
					i++;
				}
			}
			else if (FD_ISSET(socket_fd, &read_fds))
			{
				FD_CLR(socket_fd, &read_fds);
				memset(recv_buf, 0x00, 128);
				recv_len = read(socket_fd, recv_buf, 128);
				if (recv_len > 0)
				{
					ql_debug(">>>>Recv: %s\n", recv_buf);
					memset(send_buf, 0x00, 128);
					send_len = snprintf(send_buf, 128, "%d%s%d\r\n", i, "startAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAend", i);
					write(socket_fd, send_buf, send_len);
					i++;
					FD_SET(socket_fd, &read_fds);
				}
				else if (recv_len == 0)
				{
					if (closing == FALSE)
					{
						ql_debug("===passive close!!!!\n");
						shutdown(socket_fd, SHUT_WR);
						closing = TRUE;
						FD_SET(socket_fd, &read_fds);
					}
					else
					{
						close(socket_fd);
						socket_fd = -1;
						break;
					}
				}
				else
				{
				}
			}
			else if (FD_ISSET(socket_fd, &exp_fds))
			{
				FD_CLR(socket_fd, &exp_fds);
				close(socket_fd);
				socket_fd = -1;
				break;
			}
		}
	}
	if (pres != NULL)
	{
		freeaddrinfo(pres);
		pres = NULL;
	}
	goto loop;
exit:
	ql_rtos_task_delete(socket_thread_handle);

	return;
}

void ql_sockets_demo_thread_creat(void)
{
    int ret;
    ret = ql_rtos_task_create(&socket_thread_handle,
                              (unsigned short)2048,
                              RTOS_HIGHER_PRIORTIY_THAN(5),
                              "socket_demo",
                              ql_sockets_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO, "Error: Failed to create socket thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (socket_thread_handle != NULL)
    {
        ql_rtos_task_delete(socket_thread_handle);
    }
}
