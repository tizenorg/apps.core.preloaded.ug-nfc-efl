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


#ifndef __NFC_SETTING_APP_DEBUG_H__
#define __NFC_SETTING_APP_DEBUG_H__

#include <dlog.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define COLOR_RED	"\033[0;31m"
#define COLOR_GREEN	"\033[0;32m"
#define COLOR_BROWN	"\033[0;33m"
#define COLOR_BLUE	"\033[0;34m"
#define COLOR_PURPLE	"\033[0;35m"
#define COLOR_CYAN	"\033[0;36m"
#define COLOR_LIGHTBLUE	"\033[0;37m"
#define COLOR_END	"\033[0;m"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG	"NFC_SETTING_APP"

#define NSA_ENABLE_DLOG

#ifdef NSA_ENABLE_DLOG
#if defined(NSA_LOG_LEVEL_1)
#define NSA_BEGIN()
#define NSA_END()
#define NSA_DEBUG(fmt, ...)
#define NSA_DEBUG_ERR(fmt, ...) \
	do { \
		LOGE(COLOR_RED" " fmt COLOR_END, ##__VA_ARGS__); \
	} while (0)
#elif defined(NSA_LOG_LEVEL_2)
#define NSA_BEGIN()
#define NSA_END()
#define NSA_DEBUG(fmt, ...) \
	do { \
		LOGD(COLOR_BROWN" " fmt COLOR_END, ##__VA_ARGS__); \
	} while (0)
#define NSA_DEBUG_ERR(fmt, ...) \
	do { \
		LOGE(COLOR_RED" " fmt COLOR_END, ##__VA_ARGS__); \
	} while (0)
#elif defined(NSA_LOG_LEVEL_3)
#define NSA_BEGIN() \
	do { \
		LOGD(COLOR_BLUE" BEGIN >>>>"COLOR_END); \
	} while (0)
#define NSA_END() \
	do { \
		LOGD(COLOR_BLUE" END <<<<"COLOR_END); \
	} while (0)
#define NSA_DEBUG(fmt, ...) \
	do { \
		LOGD(COLOR_BROWN" " fmt COLOR_END, ##__VA_ARGS__); \
	} while (0)
#define NSA_DEBUG_ERR(fmt, ...) \
	do { \
		LOGE(COLOR_RED" " fmt COLOR_END, ##__VA_ARGS__); \
	} while (0)
#endif
#else
#define NSA_BEGIN() \
	do { \
		printf("\n [%s: %s: %d] : BEGIN >>>> %s() \n", APPNAME, \
			rindex(__FILE__, '/')+1,  __LINE__ , __FUNCTION__); \
	} while (0)

#define NSA_END() \
	do { \
		printf("\n [%s: %s: %d]: END   <<<< %s()\n", APPNAME, \
			rindex(__FILE__, '/')+1,  __LINE__ , __FUNCTION__); \
	} while (0)
#define NSA_DEBUG(fmt, ...) \
	do { \
		printf("\n [%s: %s: %s(): %d] " fmt"\n",  APPNAME, \
			rindex(__FILE__, '/')+1, __FUNCTION__, __LINE__, \
			##__VA_ARGS__); \
	} while (0)
#define NSA_DEBUG_ERR(fmt, ...) \
	do { \
		printf("\n [%s: %s: %s(): %d] " fmt"\n",  APPNAME, \
			rindex(__FILE__, '/')+1, __FUNCTION__, __LINE__, \
			##__VA_ARGS__); \
	} while (0)
#endif

#define ret_if(expr) \
	do { \
		if (__builtin_expect(expr, 0)) { \
			NSA_DEBUG_ERR("(%s) ", #expr); \
			return; \
		} \
	} while (0)
#define retm_if(expr, fmt, arg...) \
	do { \
		if (__builtin_expect(expr, 0)) { \
			NSA_DEBUG_ERR("(%s) " fmt, #expr, ##arg); \
			return; \
		} \
	} while (0)
#define retv_if(expr, val)\
	do { \
		if (__builtin_expect(expr, 0)) { \
			NSA_DEBUG_ERR("(%s) ", #expr); \
			return (val);\
		} \
	} while (0)
#define retvm_if(expr, val, fmt, arg...) \
	do { \
		if (__builtin_expect(expr, 0)) { \
			NSA_DEBUG_ERR("(%s) " fmt, #expr, ##arg); \
			return (val);\
		} \
	} while (0)

#ifdef __cplusplus
}
#endif
#endif /* __NFC_SETTING_APP_DEBUG_H__ */