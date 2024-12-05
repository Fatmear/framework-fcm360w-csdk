#ifndef __EXAMPLE_DEFINE_DEMO_H
#define __EXAMPLE_DEFINE_DEMO_H

#include <stdio.h>
#include <string.h>
#include "oshal.h"
// #include "quec_apimf_def.h"

#define VERSION_1601	1601
#define VERSION_1606	1606

enum {
	_QLST_LOG_COMMAND,

	_MAX_MODE
};

#define quecst_printf(sync,fmt,...) do {\
	if (sync){\
		quecst_send_upper_data("[%s]<%s>:"fmt"", __FILE__, __func__, ##__VA_ARGS__);\
	}\
	os_printf(LM_OS, LL_INFO," <%s> ",__func__);\
	os_printf(LM_OS, LL_INFO,fmt, ##__VA_ARGS__);\
}while(0)

#define quecst_printf_error(exp, fmt,...) do{\
	if (exp){\
		quecst_printf(exp,"*** error occured *** | %s", mkstr(exp));\
		os_printf(LM_OS, LL_INFO,fmt, ##__VA_ARGS__);\
	}\
}while(0)

#define MAIN_TASK_NAME "rtos_test_main_task"
#define SUB_TASK_NAME "rtos_test_sub_task"

#define R_TASK_STACK_SIZE 1024*2
#define S_TASK_STACK_SIZE 1024*2
#define TASK_PRIORITY 7

#define MSG_MAX_SIZE 256
#define MSG_MAX_NUM 10

#define PARAM_COUNT 8
#define PARAM_SIZE 128

#define SYNC_LOG 0
#define ASYNC_LOG 0

#define PARAM_ERROR 0xFF
#define PARAM_INVALID 0xFF

#define DEF_PAR_NUM	0
#define DEF_PAR_STR	NULL

#define PARAM_NUM_COUNT(num)	num+1

#define CH_LF		0x0A
#define CH_RC		0x0D

#define QLST_SUCCESS 0
#define QLST_FAILURE 1
#define QLST_INVALID -1

#define CURRENT_FUNC_TYPE 	0
#define CALLBACK_FUNC_TYPE 	1

#define FUNCTION_CALL_TYPE 0	//1 通过函数id调用功能函数，0 通过函数名称调用功能函数
#define FUNCTION_NAME_SIZE 32


extern char funcid_str[48];
enum{
	PARAM_NUM_1,
	PARAM_NUM_2,
	PARAM_NUM_3,
	PARAM_NUM_4,
	PARAM_NUM_5,
	PARAM_NUM_6,
	PARAM_NUM_7,
	PARAM_NUM_8
};

typedef void (*pQUECSTFUNC)(int argc, char* argv[]);

// unsigned int pointer_str_to_address_num(char* str_pointer);

// void quecst_apimf_start(void);
void quecst_send_upper_data(const char *fmt, ...);
#endif /*__EXAMPLE_DEFINE_DEMO_H*/


