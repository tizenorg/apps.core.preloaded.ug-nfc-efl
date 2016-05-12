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


#ifndef __NFC_SETTING_APP_POPUP_H__
#define __NFC_SETTING_APP_POPUP_H__

#include <Elementary.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*_POPUP_USER_RESP_CB)(void *data, Evas_Object *obj,
	void *event_info);


/************************** API **************************/
Evas_Object *nsa_create_popup_text_1button(
		Evas_Object *parent,
		const char *text,
		const char *btn1_image_path,
		_POPUP_USER_RESP_CB response_cb,
		void *data);

Evas_Object *nsa_create_toast_popup(Evas_Object *parent,
	const char *text);

#ifdef __cplusplus
}
#endif
#endif /* __NFC_SETTING_APP_POPUP_H__ */

