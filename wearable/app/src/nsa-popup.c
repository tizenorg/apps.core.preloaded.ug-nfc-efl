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


#include <efl_extension.h>
#include "nsa-popup.h"
#include "nsa-debug.h"

#define POPUP_AUTO_TIMEOUT_SEC	3.0

static void __popup_block_clicked_cb(void *data, Evas_Object *obj,
	void *event_info)
{
	ret_if(obj == NULL);

	elm_popup_dismiss(obj);
}

static void __popup_hide_cb(void *data, Evas_Object *obj,
	void *event_info)
{
	ret_if(obj == NULL);

	elm_popup_dismiss(obj);
}

static void __popup_hide_finished_cb(void *data, Evas_Object *obj,
	void *event_info)
{
	ret_if(obj == NULL);

	evas_object_del(obj);
}

static void __popup_timeout_cb(void *data, Evas_Object *obj,
	void *event_info)
{
	ret_if(obj == NULL);

	evas_object_del(obj);
}

Evas_Object *nsa_create_popup_text_1button(
		Evas_Object *parent,
		const char *text,
		const char *btn1_image_path,
		_POPUP_USER_RESP_CB response_cb,
		void *data)
{
	Evas_Object *popup;
	Evas_Object *btn;
	Evas_Object *icon;
	Evas_Object *layout;

	retv_if(parent == NULL, NULL);
	retv_if(text == NULL, NULL);
	retv_if(btn1_image_path == NULL, NULL);
	retv_if(response_cb == NULL, NULL);

	popup = elm_popup_add(parent);
	retv_if(popup == NULL, NULL);
	elm_object_style_set(popup, "circle");

//	uxt_popup_set_rotary_event_enabled(popup, EINA_TRUE);

	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
		__popup_hide_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed",
		__popup_hide_finished_cb, NULL);

	layout = elm_layout_add(popup);
	retv_if(layout == NULL, NULL);
	elm_layout_theme_set(layout, "layout", "popup",
		"content/circle/buttons1");

	elm_object_part_text_set(layout, "elm.text", text);
	elm_object_content_set(popup, layout);

	btn = elm_button_add(popup);
	retv_if(btn == NULL, NULL);
	elm_object_style_set(btn, "popup/circle");
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
	elm_access_info_set(btn, ELM_ACCESS_INFO, "Cancel");
	elm_object_part_content_set(popup, "button1", btn);
	evas_object_smart_callback_add(btn, "clicked", response_cb, data);

	icon = elm_image_add(btn);
	retv_if(icon == NULL, NULL);
	elm_image_file_set(icon, btn1_image_path, NULL);
	evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
	elm_object_part_content_set(btn, "elm.swallow.content", icon);
	evas_object_show(icon);

	evas_object_show(popup);

	return popup;
}

Evas_Object *nsa_create_toast_popup(Evas_Object *parent,
	const char *text)
{
	Evas_Object *toast;

	NSA_BEGIN();

	toast = elm_popup_add(parent);
	retv_if(toast == NULL, NULL);

	elm_object_style_set(toast, "toast/circle");
	elm_popup_orient_set(toast, ELM_POPUP_ORIENT_BOTTOM);
	evas_object_size_hint_weight_set(toast, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
	eext_object_event_callback_add(toast, EEXT_CALLBACK_BACK,
		__popup_hide_cb, NULL);
	evas_object_smart_callback_add(toast, "dismissed",
		__popup_hide_finished_cb, NULL);
	elm_object_part_text_set(toast, "elm.text", text);

	evas_object_smart_callback_add(toast, "block,clicked",
		__popup_block_clicked_cb, NULL);

	elm_popup_timeout_set(toast, 2.0);
	evas_object_smart_callback_add(toast, "timeout", __popup_timeout_cb,
		NULL);

	evas_object_show(toast);
	NSA_END();

	return toast;
}


