#ifndef _QL_SPI_CH392_H_
#define _QL_SPI_CH392_H_
// #include "ql_api_osi.h"
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************
**
**     Command Register
**
*******************************/
#define CMD01_GET_IC_VER                  0x01        /* 获取芯片以及固件版本号 */
#define CMD00_ENTER_SLEEP                 0x03        /* 进入睡眠状态 */
#define CMD00_RESET_ALL                   0x05        /* 执行硬件复位 */
#define CMD11_CHECK_EXIST                 0x06        /* 测试通讯接口以及工作状态 */
#define CMD02_GET_GLOB_INT_STATUS_ALL     0x19        /* 获取全局中断状态，V44版本以后的程序由于增加了socket数量需要用此命令获取全部的中断 */
#define CMD60_SET_MAC_ADDR                0x21        /* 设置MAC地址 必须在CMD00H_INIT_CH392之前设置完毕 */
#define CMD40_SET_IP_ADDR                 0x22        /* 设置IP地址 必须在CMD00H_INIT_CH392之前设置完毕 */
#define CMD40_SET_GWIP_ADDR               0x23        /* 设置网关IP地址 必须在CMD00H_INIT_CH392之前设置完毕 */
#define CMD40_SET_MASK_ADDR               0x24        /* 设置子网掩码， 必须在CMD00H_INIT_CH392之前设置完毕 */
#define CMD01_GET_PHY_STATUS              0x26        /* 获取PHY当前状态，如断开连接，10/100M FULL/HALF */
#define CMD0W_INIT_CH392                  0x27        /* 初始化CH392 */
#define CMD01_GET_GLOB_INT_STATUS         0x29        /* 获取全局中断状态，最大值为1S，不可以设置为0 */
#define CMD01_GET_CMD_STATUS              0x2C        /* 获取命令执行状态 */
#define CMD06_GET_REMOT_IPP_SN            0x2D        /* 获取远端的端口以及IP地址，该命令在TCP服务器模式下使用 */
#define CMD10_CLEAR_RECV_BUF_SN           0x2E        /*清除接收缓冲区*/
#define CMD12_GET_SOCKET_STATUS_SN        0x2F        /* 获取socket n状态 */
#define CMD11_GET_INT_STATUS_SN           0x30        /* 获取socket n的中断状态 */
#define CMD50_SET_IP_ADDR_SN              0x31        /* 设置socket n的目的IP地址 */
#define CMD30_SET_DES_PORT_SN             0x32        /* 设置socket n的目的端口 */
#define CMD30_SET_SOUR_PORT_SN            0x33        /* 设置socket n的源端口 */
#define CMD20_SET_PROTO_TYPE_SN           0x34        /* 设置socket n的协议类型 */
#define CMD1W_OPEN_SOCKET_SN              0x35        /* 打开socket n */
#define CMD1W_TCP_LISTEN_SN               0x36        /* socket n监听，收到此命令，socket n进入服务器模式，仅对TCP模式有效 */
#define CMD1W_TCP_CONNECT_SN              0x37        /* socket n连接，收到此命令，socket n进入客户端模式，仅对TCP模式有效 */
#define CMD1W_TCP_DISNCONNECT_SN          0x38        /* socket n断开连接，收到此命令，socket n断开已有连接，仅对TCP模式有效 */
#define CMD30_WRITE_SEND_BUF_SN           0x39        /* 向socket n缓冲区写入数据 */
#define CMD12_GET_RECV_LEN_SN             0x3B        /* 获取socket n接收数据的长度 */
#define CMD30_READ_RECV_BUF_SN            0x3C        /* 读取socket n接收缓冲区数据 */
#define CMD1W_CLOSE_SOCKET_SN             0x3D        /* 关闭socket n */
#define CMD20_SET_IPRAW_PRO_SN            0x3E        /* 在IP RAW下，设置socket n的IP包协议类型 */
#define CMD06_GET_MAC_ADDR                0x40        /* 获取MAC地址 */
#define CMD10_DHCP_ENABLE                 0x41        /* DHCP使能 */
#define CMD01_GET_DHCP_STATUS             0x42        /* 获取DHCP状态 */
#define CMD014_GET_IP_INF                 0x43        /* IP,子网掩码，网关 */
#define CMD20_SET_TTL                     0x51        /* 设置TTL，TTL最大值为128 */
#define CMD30_SET_RECV_BUF                0x52        /* 设置SOCKET接收缓冲区 */
#define CMD30_SET_SEND_BUF                0x53        /* 设置SOCKET发送缓冲区 */
#define CMD40_SET_KEEP_LIVE_IDLE          0x56        /* 设置KEEPLIVE空闲 */
#define CMD40_SET_KEEP_LIVE_INTVL         0x57        /* 设置间隔时间 */
#define CMD10_SET_KEEP_LIVE_CNT           0x58        /* 重试次数 */
#define CMD20_SET_KEEP_LIVE_SN            0X59        /* 设置socket nkeeplive功能*/
#define CMD21_SET_MACRAW                  0XBA        /* 设置进入MACRAW */

 

 
 
/*******************************
**
**    Socket Protocal Type
**
*******************************/
#define PROTO_TYPE_IP_RAW                 0           /* IP层原始数据 */
#define PROTO_TYPE_UDP                    2           /* UDP协议类型 */
#define PROTO_TYPE_TCP                    3           /* TCP协议类型 */



/*******************************
**
**     PHY Command/Status
**
*******************************/
#define PHY_DISCONN                       (1<<0)      /* PHY断开 */
#define PHY_10M_FLL                       (1<<1)      /* 10M全双工 */
#define PHY_10M_HALF                      (1<<2)      /* 10M半双工 */
#define PHY_100M_FLL                      (1<<3)      /* 100M全双工 */
#define PHY_100M_HALF                     (1<<4)      /* 100M半双工 */
#define PHY_AUTO                          (1<<5)      /* PHY自动模式，CMD10H_SET_PHY */




/*******************************
**
**     Global Int Status
**
*******************************/
#define GINT_STAT_UNREACH                 (1<<0)      /* 不可达中断 */
#define GINT_STAT_IP_CONFLI               (1<<1)      /* IP冲突 */
#define GINT_STAT_PHY_CHANGE              (1<<2)      /* PHY状态改变 */
#define GINT_STAT_DHCP                    (1<<3)      /* PHY状态改变 */
#define GINT_STAT_SOCK0                   (1<<4)      /* socket0 产生中断 */
#define GINT_STAT_SOCK1                   (1<<5)      /* socket1 产生中断 */
#define GINT_STAT_SOCK2                   (1<<6)      /* socket2 产生中断 */
#define GINT_STAT_SOCK3                   (1<<7)      /* socket3 产生中断 */
#define GINT_STAT_SOCK4                   (1<<8)      /* scoket4 产生中断 */
#define GINT_STAT_SOCK5                   (1<<9)      /* scoket5 产生中断 */
#define GINT_STAT_SOCK6                   (1<<10)     /* scoket6 产生中断 */
#define GINT_STAT_SOCK7                   (1<<11)     /* scoket7 产生中断 */




/*******************************
**
**     Socket Int Type
**
*******************************/
#define SINT_STAT_SENBUF_FREE             (1<<0)      /* 发送缓冲区空闲 */
#define SINT_STAT_SEND_OK                 (1<<1)      /* 发送成功 */
#define SINT_STAT_RECV                    (1<<2)      /* socket端口接收到数据或者接收缓冲区不为空 */
#define SINT_STAT_CONNECT                 (1<<3)      /* 连接成功,TCP模式下产生此中断 */
#define SINT_STAT_DISCONNECT              (1<<4)      /* 连接断开,TCP模式下产生此中断 */
#define SINT_STAT_TIM_OUT                 (1<<6)      /* ARP和TCP模式下会发生此中断 */




/*******************************
**
**   Command Returned Status
**
*******************************/
#define CMD_ERR_SUCCESS                   0x00        /* 命令操作成功 */
#define CMD_RET_ABORT                     0x5F        /* 命令操作失败 */
#define CH392_ERR_BUSY                    0x10        /* 忙状态，表示当前正在执行命令 */
#define CH392_ERR_MEM                     0x11        /* 内存错误 */
#define CH392_ERR_BUF                     0x12        /* 缓冲区错误 */
#define CH392_ERR_TIMEOUT                 0x13        /* 超时 */
#define CH392_ERR_RTE                     0x14        /* 路由错误*/
#define CH392_ERR_ABRT                    0x15        /* 连接停止*/
#define CH392_ERR_RST                     0x16        /* 连接复位 */
#define CH392_ERR_CLSD                    0x17        /* 连接关闭/socket 在关闭状态 */
#define CH392_ERR_CONN                    0x18        /* 无连接 */
#define CH392_ERR_VAL                     0x19        /* 错误的值 */
#define CH392_ERR_ARG                     0x1a        /* 错误的参数 */
#define CH392_ERR_USE                     0x1b        /* 已经被使用 */
#define CH392_ERR_IF                      0x1c        /* MAC错误  */
#define CH392_ERR_ISCONN                  0x1d        /* 已连接 */
#define CH392_ERR_OPEN                    0X20        /* 已经打开 */
#define CH392_ERR_UNKNOW                  0xFA        /* 未知错误 */



typedef struct{
	  uint8_t   IPAddr[4];
    uint8_t   GWIPAddr[4];
    uint8_t   MASKAddr[4];
    uint8_t   MacAddr[6];
    uint8_t   PHYStat;
    uint8_t   DHCPstates;
    uint8_t   UnreachIPAddr[4];    /* 不可到达IP */
    uint16_t  UnreachPort;         /* 不可到达端口 */
} _CH392_INF;


//func

/* Protocal Type */
#define PROTO_TYPE_IP_RAW                 0           /* IP */
#define PROTO_TYPE_MAC_RAW                1           /* MAC */
#define PROTO_TYPE_UDP                    2           /* UDP */
#define PROTO_TYPE_TCP                    3           /* TCP */

#define SOCKET0		0
#define SOCKET1		1
#define SOCKET2		2
#define SOCKET3		3
#define SOCKET4		4
#define SOCKET5		5
#define SOCKET6		6
#define SOCKET7		7

#define CH392_SOCKET_NUM  4

typedef struct {
		uint8_t  DesIP[4];            /* socket目标IP地址 32bit*/
    uint8_t  MacAddr[6];           /* socket目标地址 48bit*/
    uint8_t  ProtoType;            /* 协议类型 */
    uint8_t  SockStatus;           /* socket状态，参考scoket状态定义 */
    uint8_t  TcpMode;              /* TCP模式 */
    uint32_t IPRAWProtoType;       /* 协议类型 */
    uint16_t DesPort;              /* remote port */
    uint16_t SourPort;             /* local port */
    uint16_t SendLen;              /* 发送数据长度 */
    uint16_t RemLen;               /* 剩余长度 */
    uint8_t  *pSend;               /* 发送指针 */
} _CH392_SOCKINF;

extern unsigned short ch392_TR_flag;

void drv_ch392_periph_init(void);
void ch392_extenal_reset(void);
uint8_t drv_ch392_spi_check(void);
uint8_t ch392_cmd_get_ver(void);
void ch392_cmd_get_macaddr(uint8_t *amcaddr);
void ch392_set_macraw(void);
uint8_t drv_ch392_int_status(void);
uint8_t  ch392_dhcp_enable(uint8_t flag);
uint8_t ch392_get_cmd_status(void);
uint8_t ch392_cmd_get_phy_status(void);
uint8_t  ch392_open_socket(uint8_t sockindex);
void ch395_cmd_reset(void);
//func

uint8_t drv_ch392_init(uint8_t *ip,uint8_t *submask,uint8_t *gateway);
void ch392_set_socket_udp_mode(uint8_t socket,uint16_t localport,uint16_t remoteport,uint8_t *remoteip);
void ch392_set_socket_tcp_client_mode(uint8_t socket,uint8_t *desip, uint16_t localport,uint16_t serverport);
void ch392_set_socket_tcp_server_mode(uint8_t socket,uint16_t port);
void ch392_write_data(uint16_t socket,uint8_t *data,uint16_t size,uint8_t *ip,uint16_t port);
void drv_ch392_global_int_process(void);
void ql_wizchip_set_recvhandler(int(*recv_data_hdlr)(uint8_t *data, uint32_t len));
void ch392_send_data(uint8_t sockindex, uint8_t *databuf, uint16_t len);
uint8_t ch392_cmd_init(void);

int ql_ch392t_get_info(_CH392_INF *ch392t_inf);
#ifdef __cplusplus
} /*"C" */
#endif

#endif