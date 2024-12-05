/**
 * @file ql_mem.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __QL_MEM_H_
#define __QL_MEM_H_

#include <stdint.h>
#include "ql_type.h"
#include "ql_api_common.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 
 * 
 * @param ptr 
 */
void ql_free(void *ptr);

/**
 * @brief 
 * 
 * @param ptr 
 * @param size 
 * @return void* 
 */
void *ql_realloc(void *ptr, size_t size);

/**
 * @brief 
 * 
 * @param ptr 
 * @param c 
 * @param n 
 * @return void* 
 */
void *ql_memset(void *ptr, int c, size_t n);

/**
 * @brief 
 * 
 * @param dest 
 * @param src 
 * @param n 
 * @return void* 
 */
void *ql_memcpy(void *dest, const void *src, size_t n);

/**
 * @brief 
 * 
 * @param size 
 * @return void* 
 */
void *ql_malloc(size_t size);

/**
 * @brief 
 * 
 * @param dest 
 * @param src 
 * @param n 
 * @return void* 
 */
void *ql_memmove(void *dest, const void *src, size_t n);

/**
 * @brief 
 * 
 * @param str1 
 * @param str2 
 * @param n 
 * @return int32_t 
 */
int32_t ql_memcmp(const void *str1, const void *str2, size_t n);

#ifdef __cplusplus
}
#endif
#endif
