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


#include <app.h>
#include <nfc.h>
#include <vconf.h>
#include <pkgmgr-info.h>
#include "nsa-view.h"
#include "nsa-main.h"
#include "nsa-debug.h"
#include "nsa-util.h"
#include "nsa-ui-widget.h"
#include "nsa-popup.h"
#include "nsa-string.h"

static void __get_payment_handler(char **handler)
{
	char *appid;

	NSA_MEM_FREE(*handler);

	appid = vconf_get_str(VCONFKEY_NFC_PAYMENT_HANDLERS);

	if (appid != NULL) {
		pkgmgrinfo_pkginfo_h pkginfo = NULL;
		char *label = NULL;
		int ret;

		ret = pkgmgrinfo_pkginfo_get_pkginfo(appid, &pkginfo);
		if (ret != 0) {
			return;
		}

		ret = pkgmgrinfo_pkginfo_get_label(pkginfo, &label);
		if (ret != 0) {
			pkgmgrinfo_pkginfo_destroy_pkginfo(pkginfo);
			return;
		} else {
			NSA_MEM_STRDUP(*handler, label);
			if (*handler != NULL)
				NSA_DEBUG("text: %s", *handler);

			pkgmgrinfo_pkginfo_destroy_pkginfo(pkginfo);
		}
	}
}

static void __update_onoff_obj(void *data)
{
	appdata *ad = data;

	ret_if(ad == NULL);
	NSA_DEBUG("__update_onoff_obj");

	elm_object_item_disabled_set(ad->on_off, false);
	elm_genlist_item_fields_update(ad->on_off, "*", ELM_GENLIST_ITEM_FIELD_CONTENT);
}

static void __update_tap_and_pay_obj(void *data)
{
	appdata *ad = data;

	ret_if(ad == NULL);
	NSA_DEBUG("__update_tap_and_pay_obj");

	elm_object_item_disabled_set(ad->tap_n_pay, !nfc_manager_is_activated());
	elm_genlist_item_fields_update(ad->tap_n_pay, "*", ELM_GENLIST_ITEM_FIELD_TEXT);
}

static void __nfc_activation_completed_cb(nfc_error_e error,
	void *user_data)
{
	appdata *ad = user_data;

	g_assert(ad != NULL);

	/* handle error cases */
	if (error != NFC_ERROR_NONE)
		NSA_DEBUG_ERR("__nfc_activation_completed_cb failed");

	/* update ui after activation is completed */
	__update_onoff_obj(ad);
	__update_tap_and_pay_obj(ad);
}

static void __change_nfc_onoff_setting(void *data)
{
	appdata *ad = data;
	int result;

	ret_if(ad == NULL);

	NSA_BEGIN();

	/* V1.9 : instead of toast popup, disable item during operation */
	if(ad->on_off != NULL)
		elm_object_item_disabled_set(ad->on_off, true);

	result = nfc_manager_set_activation(!nfc_manager_is_activated(),
		__nfc_activation_completed_cb, ad);

	ret_if(result != NFC_ERROR_NONE);

	NSA_END();
}

static void __gl_on_off_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata *ad = data;
	Elm_Object_Item* it = (Elm_Object_Item *)event_info;

	elm_genlist_item_selected_set(it, EINA_FALSE);

	NSA_DEBUG("__gl_on_off_clicked_cb");

	__change_nfc_onoff_setting(ad);
}

static void __gl_tap_n_pay_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata *ad = data;

	NSA_DEBUG("__gl_tap_n_pay_clicked_cb");

	_nsa_view_tap_n_pay_create(ad);
}

/* for item class */
static Evas_Object *__gl_content_get_on_off(void *data, Evas_Object *obj,
	const char *part)
{
	if (!strcmp(part, "elm.icon")) {
		Evas_Object *check;

		check = elm_check_add(obj);
		retv_if(check == NULL, NULL);
		elm_access_object_unregister(check);
		elm_object_style_set(check, "on&off/list");

		if (nfc_manager_is_activated())
			elm_check_state_set(check, EINA_TRUE);
		else
			elm_check_state_set(check, EINA_FALSE);

		evas_object_size_hint_align_set(check, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_weight_set(check, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_propagate_events_set(check, EINA_FALSE);
		evas_object_repeat_events_set(check, EINA_TRUE);

		return check;
	} else {
		return NULL;
	}
}

static char *__gl_text_get_nfc(void *data, Evas_Object *obj,
	const char *part)
{
	if (!strcmp(part, "elm.text")) {
		return strdup(IDS_NFC_NFC);
	} else {
		return NULL;
	}
}

static char *__gl_text_get_tap_n_pay(void *data, Evas_Object *obj,
	const char *part)
{
	appdata *ad = data;
	NSA_DEBUG("part : %s", part);
	if (!strcmp(part, "elm.text")) {
		return strdup(IDS_TAP_AND_PAY);
	} else if (!strcmp(part, "elm.text.1")) {
		if (ad->payment_handler != NULL) {
			NSA_DEBUG("ad->payment_handler : %s", ad->payment_handler);
			return strdup(ad->payment_handler);
		}
		else
			return NULL;
	} else {
		return NULL;
	}
}

static Evas_Object *__create_nfc_setting_list(void *data)
{
	appdata *ad = data;

	Evas_Object *genlist;
	Evas_Object *circle_obj;
	Elm_Genlist_Item_Class *itc_title, *itc_on_off, *itc_list, *itc_padding;

	retv_if(ad == NULL, NULL);

	__get_payment_handler(&ad->payment_handler);

	/* make genlist */
	genlist = elm_genlist_add(ad->navi_frame);
	retv_if(genlist == NULL, NULL);

	/* set genlist property */
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);

	circle_obj = eext_circle_object_genlist_add(genlist, ad->circle_surface);
	eext_circle_object_genlist_scroller_policy_set(circle_obj,
		ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_obj, EINA_TRUE);

	/* create genlist class + */
	itc_title = elm_genlist_item_class_new();
	itc_title->item_style = "title";
	itc_title->func.text_get = __gl_text_get_nfc;

	itc_on_off = elm_genlist_item_class_new();
	itc_on_off->item_style = "1text.1icon.1";
	itc_on_off->func.text_get = __gl_text_get_nfc;
	itc_on_off->func.content_get = __gl_content_get_on_off;

	itc_list = elm_genlist_item_class_new();
	if (ad->payment_handler != NULL)
		itc_list->item_style = "2text";
	else
		itc_list->item_style = "1text";
	itc_list->func.text_get = __gl_text_get_tap_n_pay;

	itc_padding = elm_genlist_item_class_new();
	itc_padding->item_style = "padding";
	/* create genlist class - */

	/* ADD: ITEM */
	elm_genlist_item_append(genlist, itc_title,
		NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	ad->on_off = elm_genlist_item_append(genlist, itc_on_off,
		NULL, NULL, ELM_GENLIST_ITEM_NONE, __gl_on_off_clicked_cb, ad);

	ad->tap_n_pay = elm_genlist_item_append(genlist, itc_list,
		ad, NULL, ELM_GENLIST_ITEM_NONE, __gl_tap_n_pay_clicked_cb, ad);

	elm_genlist_item_append(genlist, itc_padding,
		NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	elm_genlist_item_class_free(itc_title);
	elm_genlist_item_class_free(itc_on_off);
	elm_genlist_item_class_free(itc_list);
	elm_genlist_item_class_free(itc_padding);

	/* V2.1 */
	__update_tap_and_pay_obj(ad);

	evas_object_show(genlist);

	return genlist;
}

static void __payment_handler_vconf_key_changed (keynode_t *key,
	void *data)
{
	appdata *ad = data;

	NSA_DEBUG("__payment_handler_vconf_key_changed");

	/* Get changed payment handler */
	__get_payment_handler(&ad->payment_handler);

	/* update ui tap and pay */
	__update_tap_and_pay_obj(ad);
}

bool _nsa_view_main_create(void *data)
{
	appdata *ad = data;
	Evas_Object *genlist;
	retv_if(ad == NULL, false);

	/* Draw UI */
	ad->main_genlist = __create_nfc_setting_list(ad);
	retv_if(ad->main_genlist == NULL, false);

	elm_naviframe_item_push(ad->navi_frame, NULL, NULL, NULL, ad->main_genlist, "empty");
	ad->current_view = V_MAIN;

	if (VCONF_OK != vconf_notify_key_changed(VCONFKEY_NFC_PAYMENT_HANDLERS,
		__payment_handler_vconf_key_changed, ad)) {
		NSA_DEBUG_ERR("vconf_notify_key_changed error");
	}

	return true;
}
