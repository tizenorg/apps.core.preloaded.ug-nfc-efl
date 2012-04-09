/*
 * Copyright (c) 2000 - 2012 Samsung Electronics Co., Ltd All Rights Reserved 
 *
 * This file is part of the ug-nfc-efl
 * Written by Junyong Sim <junyong.sim@samsung.com>
 *
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of SAMSUNG ELECTRONICS ("Confidential Information").
 * You shall not disclose such Confidential Information and shall use it only in accordance
 * with the terms of the license agreement you entered into with SAMSUNG ELECTRONICS.
 * SAMSUNG make no representations or warranties about the suitability of the software,
 * either express or implied, including but not limited to the implied warranties of merchantability,
 * fitness for a particular purpose, or non-infringement.
 * SAMSUNG shall not be liable for any damages suffered by licensee as a result of using,
 * modifying or distributing this software or its derivatives.
 *
 */

#ifndef __UG_NFC_SHARE_DEBUG_H__
#define __UG_NFC_SHARE_DEBUG_H__

#define COLOR_RED 		"\033[0;31m"
#define COLOR_GREEN 	"\033[0;32m"
#define COLOR_BROWN 	"\033[0;33m"
#define COLOR_BLUE 		"\033[0;34m"
#define COLOR_PURPLE 	"\033[0;35m"
#define COLOR_CYAN 		"\033[0;36m"
#define COLOR_LIGHTBLUE "\033[0;37m"
#define COLOR_END		"\033[0;m"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "UG_NFC_SHARE_EFL"

#define UG_NFC_SHARE_ENABLE_DLOG

#ifdef UG_NFC_SHARE_ENABLE_DLOG
#define UG_NFC_SHARE_DEBUG(fmt, ...)\
	do\
	{\
		LOGD("[%s(): %d]" fmt, __FUNCTION__, __LINE__,##__VA_ARGS__);\
	} while (0)

#define UG_NFC_SHARE_DEBUG_ERR(fmt, ...)\
	do\
	{\
		LOGE(COLOR_RED"[%s(): %d]" fmt COLOR_END, __FUNCTION__, __LINE__,##__VA_ARGS__);\
	}while (0)

#define UG_NFC_SHARE_BEGIN() \
	do\
    {\
		LOGD(COLOR_BLUE"[%s(): %d] BEGIN >>>>"COLOR_END, __FUNCTION__ ,__LINE__);\
    } while( 0 )

#define UG_NFC_SHARE_END() \
	do\
    {\
		LOGD(COLOR_BLUE"[%s(): %d] END <<<<"COLOR_END, __FUNCTION__,__LINE__ );\
    } \
    while( 0 )

#else
#define UG_NFC_SHARE_DEBUG(fmt, ...) \
	do\
	{\
		printf("\n [%s: %s: %s(): %d] " fmt"\n",  APPNAME, rindex(__FILE__, '/')+1, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
	} while (0)

#define UG_NFC_SHARE_BEGIN() \
	do\
    {\
        printf("\n [%s: %s: %d] : BEGIN >>>> %s() \n", APPNAME, rindex(__FILE__, '/')+1,  __LINE__ , __FUNCTION__);\
    } while( 0 )

#define UG_NFC_SHARE_END() \
	do\
    {\
        printf("\n [%s: %s: %d]: END   <<<< %s()\n", APPNAME, rindex(__FILE__, '/')+1,  __LINE__ , __FUNCTION__); \
    } \
    while( 0 )
#endif


#define ret_if(expr) do { \
	if(expr) { \
		UG_NFC_SHARE_DEBUG_ERR("(%s) ", #expr); \
		UG_NFC_SHARE_END();\
		return; \
	} \
} while (0)
#define retm_if(expr, fmt, arg...) do { \
	 if(expr) { \
		 UG_NFC_SHARE_DEBUG_ERR("(%s) "fmt, #expr, ##arg); \
		 UG_NFC_SHARE_END();\
		 return; \
	 } \
 } while (0)
#define retv_if(expr, val) do { \
		if(expr) { \
			UG_NFC_SHARE_DEBUG_ERR("(%s) ", #expr); \
			UG_NFC_SHARE_END();\
			return (val); \
		} \
	} while (0)
#define retvm_if(expr, val, fmt, arg...) do { \
	if(expr) { \
		UG_NFC_SHARE_DEBUG_ERR("(%s) "fmt, #expr, ##arg); \
		UG_NFC_SHARE_END();\
		return (val); \
	} \
} while (0)


#endif /*__UG_NFC_SHARE_DEBUG_H__*/

