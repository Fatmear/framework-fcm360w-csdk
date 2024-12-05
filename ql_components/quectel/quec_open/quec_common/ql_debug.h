/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2010 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 |
 | File Description
 | ----------------
 |      Error Code Definition
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by    :    Jay XIN
 |  Coded by       :    Jay XIN
 |  Tested by      :    Stanley YONG
 |
 \=========================================================================*/
#ifndef QL_DEBUG_H
#define QL_DEBUG_H

#include "oshal.h"
#include "cli.h"
#define ql_debug(msg,...)  os_printf(LM_OS, LL_INFO,msg, ##__VA_ARGS__)

#endif

