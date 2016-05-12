/*
 * Copyright (c) 2000-2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 * You shall not disclose such Confidential Information and shall
 * use it only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 * SAMSUNG make no representations or warranties about the suitability
 * of the software, either express or implied, including but not
 * limited to the implied warranties of merchantability, fitness for
 * a particular purpose, or non-infringement.
 * SAMSUNG shall not be liable for any damages suffered by licensee as
 * a result of using, modifying or distributing this software or its derivatives.

 */

#ifndef __NFC_SETTING_APP_UTIL_H__
#define __NFC_SETTING_APP_UTIL_H__

#include <stdint.h>
#include <glib.h>
#include <assert.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define _EDJ(obj) elm_layout_edje_get(obj)

#define GET_SAFE_STRING(x)	((x) != NULL ? (x) : "NULL")
#define STRING_AND_SIZE(x)	(x), strlen((x))
#define NULL_TO_EMPTY_STRING(x)	(((x) != NULL) ? (x) : "")

#define MAX_HEAP_SIZE	5*1024*1024

/************************** API **************************/
#define NSA_MEM_MALLOC(ptr, no_elements, type) \
	do { \
		if ((gint)(no_elements) <= 0) { \
			ptr = NULL;\
		} else if (MAX_HEAP_SIZE < (gint)(no_elements)*sizeof(type)) { \
			assert(0);\
		} else { \
			ptr = (type *)g_malloc0((gint)(no_elements)*sizeof(type)); \
			assert(ptr); \
		} \
	} while (0)

#define NSA_MEM_STRDUP(ptr, str) \
	do { \
		if ((str) != NULL) { \
			ptr = g_strdup((const char *)(str)); \
			assert(ptr); \
		} else { \
			(ptr) = NULL; \
		} \
	} while (0)

#define NSA_MEM_STRNDUP(ptr, str, buf_size) \
	do { \
		if ((str) != NULL && (buf_size) >= 0) { \
			ptr = g_strndup((const char *)(str), (buf_size)); \
			assert(ptr); \
		} else { \
			(ptr) = NULL; \
		} \
	} while (0)

#define NSA_MEM_MEMDUP(ptr, src, buf_size) \
	do { \
		if ((src) != NULL && (buf_size) >= 0) { \
			ptr = g_malloc0((buf_size)); \
			assert(ptr); \
			memcpy((ptr), (void *)(src), (buf_size)); \
		} else { \
			(ptr) = NULL; \
		} \
	} while (0)

#define NSA_MEM_FREE(ptr) \
	do { \
		if (ptr != NULL) { \
			g_free(ptr); \
			ptr = NULL; \
		} \
	} while (0)

#ifdef __cplusplus
}
#endif
#endif /* __NFC_SETTING_APP_UTIL_H__ */
