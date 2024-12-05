/*================================================================
  Copyright (c) 2021, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
=================================================================*/

#ifndef _QL_API_OSI_H_
#define _QL_API_OSI_H_

#ifdef __cplusplus
extern "C"
{
#endif

//#include "ql_osi_def.h"
#include <stdint.h>
#include "ql_type.h"
#include "ql_api_common.h"
#define QL_TIMER_IN_SERVICE ((void *)0xffffffff)

typedef unsigned int uint;

typedef int  ql_rtos_status;
typedef void *ql_task_t;
typedef void *ql_sem_t;
typedef void *ql_mutex_t;
typedef void *ql_queue_t;
typedef void * ql_flag_t;
typedef void (*timer_handler_t)(void *);
// event
typedef void  *ql_event_group_t;

#if 0
struct Ql_EventGroupDef_t;
typedef struct Ql_EventGroupDef_t*  Ql_EventGroupHandle_t;
typedef uint32_t               Ql_EventBits_t;
typedef long Ql_BaseType_t;
typedef uint32_t Ql_TickType_t;
#endif

#define RTOS_LOWEST_PRIORITY (0)
#define RTOS_HIGHEST_PRIORITY (configMAX_PRIORITIES - 1)
#define RTOS_DEFAULT_THREAD_PRIORITY (1)
#define RTOS_HIGHER_PRIORTIY_THAN(x) (x < RTOS_HIGHEST_PRIORITY ? x + 1 : RTOS_HIGHEST_PRIORITY)
#define RTOS_LOWER_PRIORTIY_THAN(x) (x > RTOS_LOWEST_PRIORITY ? x - 1 : RTOS_LOWEST_PRIORITY)

typedef struct
{
	void *handle;
	timer_handler_t function;
	void *arg;
} ql_timer_t;

// typedef int QuecOSStatus;

typedef enum
{
	QL_WAIT_FOREVER = 0xFFFFFFFFUL,
	QL_NO_WAIT = 0
} ql_wait_e;

typedef enum
{
	Running = 0, /* A task is querying the state of itself, so must be running. */
	Ready,		 /* The task being queried is in a read or pending ready list. */
	Blocked,	 /* The task being queried is in the Blocked state. */
	Suspended,	 /* The task being queried is in the Suspended state, or is in the Blocked state with an infinite time out. */
	Deleted,	 /* The task being queried has been deleted, but its TCB has not yet been freed. */
	Invalid		 /* Used as an 'invalid state' value. */
} ql_task_state_e;

typedef struct
{
	ql_task_t xHandle;				 /* The handle of the task to which the rest of the information in the structure relates. */
	const char *pcTaskName;			 /* A pointer to the task's name.  This value will be invalid if the task was deleted since the structure was populated! */
	ql_task_state_e eCurrentState;	 /* The state in which the task existed when the structure was populated. */
	unsigned long uxCurrentPriority; /* The priority at which the task was running (may be inherited) when the structure was populated. */
	uint16 usStackHighWaterMark;	 /* The minimum amount of stack space that has remained for the task since the task was created.  The closer this value is to zero the closer the task has come to overflowing its stack. */
} ql_task_status_t;

typedef struct
{
	uint32 id;	   ///< event identifier
	uint32 param1; ///< 1st parameter
	uint32 param2; ///< 2nd parameter
	uint32 param3; ///< 3rd parameter
} ql_event_t;

typedef struct
{
	uint32 sec;
	uint32 usec;
} ql_timeval_t;

typedef struct
{
	uint total_idle_tick;
	uint sys_tick_old;
	uint cpu_using;
	uint idle_in_tick;
	uint idle_out_tick;
} ql_cpu_using_info_t;

/****************************  error code about osi    ***************************/
typedef enum
{
	QL_OSI_SUCCESS = 0,

	QL_OSI_TASK_PARAM_INVALID = 1 | (QL_COMPONENT_OSI << 16),
	QL_OSI_TASK_CREATE_FAIL,
	QL_OSI_NO_MEMORY,
	QL_OSI_TASK_DELETE_FAIL,
	QL_OSI_TASK_PRIO_INVALID,
	QL_OSI_TASK_NAME_INVALID,
	QL_OSI_TASK_EVENT_COUNT_INVALID,
	QL_OSI_INVALID_TASK_REF,
	QL_OSI_TASK_CNT_REACH_MAX,
	QL_OSI_TASK_BIND_EVENT_FAIL,
	QL_OSI_TASK_UNBIND_EVENT_FAIL,
	QL_OSI_TASK_GET_REF_FAIL,
	QL_OSI_TASK_GET_PRIO_FAIL,
	QL_OSI_TASK_SET_PRIO_FAIL,
	QL_OSI_TASK_GET_STATUS_FAIL,
	QL_OSI_TASK_HAS_BINDED_TIMER,

	QL_OSI_QUEUE_CREATE_FAIL = 50 | (QL_COMPONENT_OSI << 16),
	QL_OSI_QUEUE_DELETE_FAIL,
	QL_OSI_QUEUE_IS_FULL,
	QL_OSI_QUEUE_RELEASE_FAIL,
	QL_OSI_QUEUE_RECEIVE_FAIL,
	QL_OSI_QUEUE_GET_CNT_FAIL,
	QL_OSI_QUEUE__FAIL,

	QL_OSI_SEMA_CREATE_FAILE = 100 | (QL_COMPONENT_OSI << 16),
	QL_OSI_SEMA_DELETE_FAIL,
	QL_OSI_SEMA_IS_FULL,
	QL_OSI_SEMA_RELEASE_FAIL,
	QL_OSI_SEMA_GET_FAIL,
	QL_OSI_SEMA__FAIL,

	QL_OSI_MUTEX_CREATE_FAIL = 150 | (QL_COMPONENT_OSI << 16),
	QL_OSI_MUTEX_DELETE_FAIL,
	QL_OSI_MUTEX_LOCK_FAIL,
	QL_OSI_MUTEX_UNLOCK_FAIL,

	QL_OSI_EVENT_SEND_FAIL = 200 | (QL_COMPONENT_OSI << 16),
	QL_OSI_EVENT_GET_FAIL,
	QL_OSI_EVENT_REGISTER_FAIL,

	QL_OSI_TIMER_CREATE_FAIL = 250 | (QL_COMPONENT_OSI << 16),
	QL_OSI_TIMER_START_FAIL,
	QL_OSI_TIMER_STOP_FAIL,
	QL_OSI_TIMER_DELETE_FAIL,
	QL_OSI_TIMER_BIND_TASK_FAIL,

	QL_OSI_SWDOG_REGISTER_FAIL = 300 | (QL_COMPONENT_OSI << 16),
	QL_OSI_SWDOG_UNREGISTER_FAIL,
	QL_OSI_SWDOG_FEED_DOG_FAIL,
	QL_OSI_SWDOG_ENABLE_FAIL,
	QL_OSI_SWDOG_DISABLE_FAIL,

} osi_errcode_e;
/*========================================================================
 *	function Definition
 *========================================================================*/

typedef void (*ql_swdog_callback)(uint32 id_type, void *ctx);
typedef void (*lvgl_Callback_t)(void *ctx);

ql_rtos_status ql_rtos_task_create(
	ql_task_t *taskRef,		   /* OS task reference										   				  */
	uint32_t stackSize,		   /* number of bytes in task stack area						   				  */
	uint8_t priority,			   /* task priority, users should use the defination in APP_ThreadPriority_e,
								  and not recommand beyond APP_PRIORITY_ABOVE_NORMAL 						  */
	char *taskName,			   /* task name, no more than 16 bytes, exceed 16 bytes will only save 16 bytes */
	void (*taskStart)(void *), /* pointer to task entry point 			 				   				  */
	void *argv				   /* task entry argument pointer 							   				  */
);

// if you delete the current task itself, the code behind ql_os_task_delete will have no chance to run
ql_rtos_status ql_rtos_task_delete(
	ql_task_t taskRef /* OS task reference	*/
);

ql_rtos_status ql_rtos_task_suspend(
	ql_task_t taskRef /* OS task reference	*/
);

ql_rtos_status ql_rtos_task_resume(
	ql_task_t taskRef /* OS task reference	*/
);

ql_rtos_status ql_rtos_task_get_current_ref(
	ql_task_t *taskRef /* OS task reference	*/
);

ql_rtos_status ql_rtos_task_change_priority(
	ql_task_t taskRef, /* OS task reference			*/
	uint8 new_priority /* OS task new priority	for in	*/
);

void ql_rtos_task_sleep_ms(
	uint32 ms /* OS task sleep time for ms	*/
);

void ql_rtos_task_sleep_s(
	uint32 s /* OS task sleep time for s		*/
);

ql_rtos_status ql_rtos_enter_critical(void);

ql_rtos_status ql_rtos_exit_critical(void);

/*
临界区中需注意：
* 1. 临界区中不可有阻塞，延迟动作
* 2. 临界区中不可使用操作系统带阻塞类接口（事件发送且带timeout，信号量等待且带timeout等）
* 3. 临界区中不可调用Audio停止/开始相关接口, TTS停止/开始接口，文件读写接口
* 4. 临界区中不可调用CFW（RPC相关）接口
*/

ql_rtos_status ql_rtos_semaphore_create(
	ql_sem_t *semaRef,	/* OS semaphore reference                     	*/
	uint32 initialCount /* initial count of the semaphore             	*/
);

ql_rtos_status ql_rtos_semaphore_wait(
	ql_sem_t semaRef, /* OS semaphore reference                     	*/
	uint32 timeout	  /* QL_WAIT_FOREVER, QL_NO_WAIT, or timeout	*/
);

ql_rtos_status ql_rtos_semaphore_release(
	ql_sem_t semaRef /* OS semaphore reference						*/
);

ql_rtos_status ql_rtos_semaphore_get_cnt(
	ql_sem_t semaRef, /* OS semaphore reference           				*/
	uint32 *cnt_ptr	  /* out-parm to save the cnt of semaphore      	*/
);

ql_rtos_status ql_rtos_semaphore_delete(
	ql_sem_t semaRef /* OS semaphore reference                  		*/
);

ql_rtos_status ql_rtos_mutex_create(
	ql_mutex_t *mutexRef /* OS mutex reference                         */
);

ql_rtos_status ql_rtos_mutex_lock(
	ql_mutex_t mutexRef /* OS mutex reference                         */
);

ql_rtos_status ql_rtos_mutex_unlock(
	ql_mutex_t mutexRef /* OS mutex reference                         */
);

ql_rtos_status ql_rtos_mutex_delete(
	ql_mutex_t mutexRef /* OS mutex reference                         */
);

ql_rtos_status ql_rtos_queue_create(
	ql_queue_t *msgQRef, /* OS message queue reference              */
	uint32 maxSize,		 /* max message size the queue supports     */
	uint32 maxNumber	 /* max # of messages in the queue          */
);

ql_rtos_status ql_rtos_queue_wait(
	ql_queue_t msgQRef, /* message queue reference                 		*/
	uint32 size,		/* size of the message                     		*/		
	uint8 *recvMsg,		/* pointer to the message received         		*/
	uint32 timeout		/* QL_WAIT_FOREVER, QL_NO_WAIT, or timeout  */
);

ql_rtos_status ql_rtos_queue_release(
	ql_queue_t msgQRef, /* message queue reference                 			*/
	uint32 size,		/* size of the message                     			*/
	uint8 *msgPtr,		/* start address of the data to be sent    			*/
	uint32 timeout		/* QL_WAIT_FOREVER, QL_NO_WAIT, or timeout   	*/
);

ql_rtos_status ql_rtos_queue_delete(
	ql_queue_t msgQRef /* message queue reference                 		*/
);

/*
 * taskRef取值:
 * 若指定task去运行定时器回调函数，则填入任务句柄；若在系统service中运行定时器回调函数，填入QL_TIMER_IN_SERVICE
 * 若需要在中断中运行定时器回调函数，则填入NULL, 定时器超时时将会产生中断并在中断中调用回调函数，但以下几点需注意：
 * 1. 中断中不可有阻塞，延迟动作，否则可能会引起未知后果
 * 2. 中断中不可进临界区(调用critical相关接口)，操作系统带阻塞类接口（事件发送且带timeout，信号量等待且带timeout等）
 * 3. 中断中不可调用Audio停止/开始相关接口, TTS停止/开始接口，文件读写接口
 * 4. 中断中不可调用CFW（RPC相关）接口
 * 建议中断中只做置标致位，做简单运算，或者中断中调用无阻塞（Timeout设为0）的线程通信函数，通知自己的线程去处理
 */
ql_rtos_status ql_rtos_timer_create(
	ql_timer_t *timerRef, /* OS supplied timer reference	*/
	uint32_t time_ms,
	void (*callBackRoutine)(void *), /* timer call-back routine						   */
	void *timerArgc					 /* argument to be passed to call-back on expiration */
);

ql_rtos_status ql_rtos_timer_start(
	ql_timer_t timerRef /* OS supplied timer reference						*/
);

ql_rtos_status ql_rtos_timer_stop(
	ql_timer_t timerRef /* OS supplied timer reference	*/
);

ql_rtos_status ql_rtos_timer_is_running(
	ql_timer_t timerRef /* OS supplied timer reference	*/
);

ql_rtos_status ql_rtos_timer_delete(
	ql_timer_t timerRef /* OS supplied timer reference	*/
);

ql_rtos_status ql_rtos_timer_change_period(
	ql_timer_t timerRef,
	uint32_t time_ms
);

/**
 * \brief monoclinic system time
 *
 * It is a relative time from system boot. Even after suspend and resume,
 * the monoclinic system time will be contiguous.
 * littlevgl need this time synchronization information
 *
 * \return      monoclinic system time in milliseconds
 */
uint32_t ql_rtos_up_time_ms();

//event
int ql_rtos_event_group_create(ql_event_group_t *event_group);
void ql_rtos_event_group_delete(ql_event_group_t event_group);
uint32_t ql_rtos_event_group_get_bits(ql_event_group_t event_group, bool isr);
uint32_t ql_rtos_event_group_wait_bits(
	ql_event_group_t event_group, 
    const uint32_t val,
    const bool clear_on_exit, 
	const bool wait_all_bits, 
	int timeout
);

uint32_t ql_rtos_event_group_clear_bits(ql_event_group_t event_group, const uint32_t val, bool isr);
uint32_t ql_rtos_event_group_set_bits(ql_event_group_t event_group, const uint32_t val, bool isr);
#if 0
Ql_EventGroupHandle_t ql_xEventGroupCreate();
Ql_EventBits_t ql_xEventGroupWaitBits( Ql_EventGroupHandle_t xEventGroup,
                                 const Ql_EventBits_t uxBitsToWaitFor,
                                 const Ql_BaseType_t xClearOnExit,
                                 const Ql_BaseType_t xWaitForAllBits,
                                 Ql_TickType_t xTicksToWait );
Ql_EventBits_t ql_xEventGroupClearBits( Ql_EventGroupHandle_t xEventGroup,
                                  const Ql_EventBits_t uxBitsToClear );
Ql_BaseType_t ql_xEventGroupClearBitsFromISR( Ql_EventGroupHandle_t xEventGroup,
                                            const Ql_EventBits_t uxBitsToClear );
Ql_EventBits_t ql_xEventGroupSetBits( Ql_EventGroupHandle_t xEventGroup,
                                const Ql_EventBits_t uxBitsToSet ) ;

Ql_BaseType_t ql_xEventGroupSetBitsFromISR( Ql_EventGroupHandle_t xEventGroup,
                                          const Ql_EventBits_t uxBitsToSet,
                                          Ql_BaseType_t * pxHigherPriorityTaskWoken );

void ql_vEventGroupDelete( Ql_EventGroupHandle_t xEventGroup );
#endif
#ifdef __cplusplus
} /*"C" */
#endif

#endif
