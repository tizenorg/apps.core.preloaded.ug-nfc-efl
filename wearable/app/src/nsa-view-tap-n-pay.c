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
#include <nfc_internal.h>
#include <vconf.h>
#include <pkgmgr-info.h>
#include "nsa-view.h"
#include "nsa-main.h"
#include "nsa-debug.h"
#include "nsa-util.h"
#include "nsa-ui-widget.h"
#include "nsa-popup.h"
#include "nsa-string.h"

#define APP_CONTROL_DEFAULT_CHANGED	\
	"http://tizen.org/appcontrol/operation/nfc/card_emulation/default_changed"

#define MAX_AIDS_COUNT	50

typedef struct {
	int index;
	appdata *ad;
	Elm_Object_Item *it;
} item_data_t;

static void __add_item_to_wallet_list(const char *handler, int count, void *data)
{
	appdata *ad = data;
	wallet_info_t *winfo;

	NSA_MEM_MALLOC(winfo, 1, wallet_info_t);
	ret_if(winfo == NULL);
	NSA_MEM_STRDUP(winfo->appid, handler);
	winfo->aid_count = count;

	g_array_append_val(ad->payment_wallet_list, winfo);
	ad->payment_wallet_cnt++;
}

static bool __registered_handler_cb(const char *handler, int count, void *user_data)
{
	appdata *ad = user_data;

	retv_if(ad == NULL, false);
	retv_if(ad->payment_wallet_list == NULL, false);

	NSA_DEBUG("package name: [%s]", handler);
	NSA_DEBUG("AID number: [%d]", count);

	__add_item_to_wallet_list(handler, count, ad);

	return true;
}

static void __get_wallet_info(void *ad)
{
	NSA_DEBUG("__get_wallet_info");

	nfc_se_foreach_registered_handlers(NFC_CARD_EMULATION_CATEGORY_PAYMENT,
		__registered_handler_cb, ad);
}

static Evas_Object *__create_no_contents_ly(void *data)
{
	appdata *ad = data;
	Evas_Object *sc, *ly, *image, *circle_obj;

	retv_if(ad == NULL, NULL);

	/* create scroller */
	sc = elm_scroller_add(ad->navi_frame);
	evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(sc, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_scroller_single_direction_set(sc, ELM_SCROLLER_SINGLE_DIRECTION_HARD);
	elm_object_style_set(sc, "effect");
	evas_object_show(sc);

	circle_obj = eext_circle_object_scroller_add(sc, ad->circle_surface);
	eext_circle_object_genlist_scroller_policy_set(circle_obj,
		ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_obj, EINA_TRUE);

	/* create layout */
	ly = nsa_create_edj_layout(sc, "tap_n_pay/nocontents");
	retv_if(ly == NULL, NULL);

	image = elm_image_add(ly);
	retv_if(image == NULL, NULL);
	elm_image_file_set(image, IMAGES_PATH"/b_setting_payment_help_image.png", NULL);
	elm_object_part_content_set(ly, "elm.swallow.icon", image);

	elm_object_part_text_set(ly, "elm.text", IDS_NO_INSTALLED_PAYMENT_APPS);
	evas_object_show(ly);

	/* content set */
	elm_object_content_set(sc, ly);

	return sc;
}


static void __max_AID_popup_response_cb(void *data,
	Evas_Object *obj, void *event_info)
{
	item_data_t *item = (item_data_t *)data;
	appdata *ad = item->ad;
	char *wallet_handler;

	ret_if(ad == NULL);
	ret_if(ad->popup == NULL);

	NSA_DEBUG("__max_AID_popup_response_cb");

	wallet_handler = vconf_get_str(VCONFKEY_NFC_PAYMENT_HANDLERS);
	if (wallet_handler != NULL) {
		int i;

		for (i = 0; i < ad->payment_wallet_cnt; i++) {
			wallet_info_t *winfo;

			winfo = g_array_index(ad->payment_wallet_list, wallet_info_t *, i);
			if (!winfo)
				continue;

			if (strlen(wallet_handler) > 0 &&
				strcmp(winfo->appid, wallet_handler) == 0) {
				NSA_DEBUG("matched, [%s]", winfo->appid);

				if (ad->radio_main != NULL)
					elm_radio_value_set(ad->radio_main, i);
			}
		}

		NSA_MEM_FREE(wallet_handler);
	}

	elm_popup_dismiss(ad->popup);
}


static char *__get_launch_app_id(char *pkgid)
{
	pkgmgrinfo_pkginfo_h pkginfo = NULL;
	char *main_appid = NULL;
	int ret;

	retv_if(pkgid == NULL, NULL);

	NSA_DEBUG("__get_launch_app_id [%s]", pkgid);

	ret = pkgmgrinfo_pkginfo_get_pkginfo(pkgid, &pkginfo);
	if (ret != 0) {
		return strdup(pkgid);
	}

	ret = pkgmgrinfo_pkginfo_get_mainappid(pkginfo, &main_appid);
	if (ret == 0) {
		return strdup(main_appid);
	} else {
		return strdup(pkgid);
	}
}

static void __launch_application(char *appid)
{
	app_control_h service;
	int ret;

	ret_if(appid == NULL);

	NSA_DEBUG("__launch_application [%s]", appid);

	app_control_create(&service);
	ret_if(service == NULL);

	ret = app_control_set_operation(service, APP_CONTROL_DEFAULT_CHANGED);
	if (ret != APP_CONTROL_ERROR_NONE) {
		NSA_DEBUG_ERR("app_control_set_operation failed[%d]", ret);
		goto END;
	}

	ret = app_control_set_app_id(service, appid);
	if (ret != APP_CONTROL_ERROR_NONE) {
		NSA_DEBUG_ERR("app_control_set_app_id failed[%d]", ret);
		goto END;
	}

	ret = app_control_send_launch_request(service, NULL, NULL);
	if(ret != APP_CONTROL_ERROR_NONE) {
		NSA_DEBUG_ERR("app_control_send_launch_request failed[%d]", ret);
	}

END:
	app_control_destroy(service);
}


static bool __set_default_wallet(item_data_t *item)
{
	char *new_wallet = NULL;
	char *old_wallet = NULL;
	wallet_info_t *winfo;
	bool duplicate = false;
	appdata *ad = item->ad;

	winfo = g_array_index(ad->payment_wallet_list, wallet_info_t *, item->index);

	new_wallet = winfo->appid;
	old_wallet = vconf_get_str(VCONFKEY_NFC_PAYMENT_HANDLERS);

	if (old_wallet != NULL) {
		if (strlen(new_wallet) > 0 && strcmp(old_wallet, new_wallet) == 0) {
			duplicate = true;
			NSA_DEBUG("Same wallet is selected!");
		}
		NSA_MEM_FREE(old_wallet);
	}

	if (!duplicate) {
		int ret;
		char *launch_app_id = NULL;

		if (winfo->aid_count > MAX_AIDS_COUNT) {
			if (ad->radio_main != NULL)
				elm_radio_value_set(ad->radio_main, -1);

			ad->popup = nsa_create_popup_text_1button(
				ad->main_layout,
				IDS_UNABLE_TO_SELECT_PAYMENT_APP_WITH_MORE_THAN_50_REGISTERED_CARDS,
				IMAGES_PATH"/tw_ic_popup_btn_check.png",
				__max_AID_popup_response_cb,
				(void *)item);

			return false;
		}

		ret = vconf_set_str(VCONFKEY_NFC_PAYMENT_HANDLERS, new_wallet);
		if (ret != VCONF_OK) {
			NSA_DEBUG_ERR("error vconf_set_str : %d", ret);

			return false;
		}

		launch_app_id = __get_launch_app_id(new_wallet);

		__launch_application(launch_app_id);

		return true;
	}

	return false;
}

static void __gl_list_clicked_cb(void *data, Evas_Object *obj,
	void *event_info)
{
	Elm_Object_Item* it = (Elm_Object_Item *)event_info;
	item_data_t *item = (item_data_t *)data;
	appdata *ad;

	elm_genlist_item_selected_set(it, EINA_FALSE);

	ret_if(item == NULL);
	ret_if(item->ad == NULL);

	ad = item->ad;

	ret_if(ad->payment_wallet_list == NULL);

	NSA_DEBUG("selected value for the group:%d", item->index);

	if (__set_default_wallet(item)) {
		elm_radio_value_set(ad->radio_main, item->index);
		elm_genlist_item_bring_in(item->it, ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);

		NSA_DEBUG("__set_default_wallet success");
	} else {
		NSA_DEBUG_ERR("set default wallet failed");
	}
}

static char *__gl_text_get_title(void *data, Evas_Object *obj,
	const char *part)
{
	if (!strcmp(part, "elm.text")) {
		return strdup(IDS_TAP_AND_PAY);
	} else {
		return NULL;
	}
}

static char *__gl_text_get(void *data, Evas_Object *obj,
	const char *part)
{
	if (!strcmp(part, "elm.text")) {
		item_data_t *item = data;
		appdata *ad = item->ad;
		wallet_info_t *winfo;
		char *appid;

		retv_if(ad == NULL, NULL);
		/* How to get index? */
		winfo = g_array_index(ad->payment_wallet_list, wallet_info_t *, item->index);
		retv_if(winfo == NULL, NULL);
		appid = winfo->appid;
		if (appid != NULL) {
			pkgmgrinfo_pkginfo_h pkginfo = NULL;
			char *label = NULL;
			int ret;

			NSA_DEBUG("pkg id: %s", appid);

			ret = pkgmgrinfo_pkginfo_get_pkginfo(appid, &pkginfo);
			if (ret != 0) {
				NSA_DEBUG_ERR("ret [%d]", ret);
				return NULL;
			}

			ret = pkgmgrinfo_pkginfo_get_label(pkginfo, &label);
			if (ret != 0) {
				pkgmgrinfo_pkginfo_destroy_pkginfo(pkginfo);
				return NULL;
			} else {
				char *text = NULL;

				NSA_MEM_STRDUP(text, label);
				if (text != NULL)
					NSA_DEBUG("text: %s", text);

				pkgmgrinfo_pkginfo_destroy_pkginfo(pkginfo);
				return text;
			}
		} else {
			return NULL;
		}
	} else {
		return NULL;
	}
}

static Evas_Object *__gl_content_get(void *data, Evas_Object *obj,
	const char *part)
{
	if (!strcmp(part, "elm.icon")) {
		item_data_t *item = (item_data_t *) data;
		appdata *ad = item->ad;
		char *wallet_handler;
		Evas_Object *radio_main;
		Evas_Object *radio;

		retv_if(item == NULL, NULL);

		radio_main = evas_object_data_get(obj, "payment_rdg");
		retv_if(radio_main == NULL, NULL);

		radio = elm_radio_add(obj);
		retv_if(radio == NULL, NULL);

		NSA_DEBUG("item->index [%d]", item->index);

		elm_object_style_set(radio, "list");
		elm_radio_state_value_set(radio, item->index);
		elm_radio_group_add(radio, radio_main);

		wallet_handler = vconf_get_str(VCONFKEY_NFC_PAYMENT_HANDLERS);

		if (wallet_handler == NULL) {
			NSA_DEBUG_ERR("vconf_get_str error");
		} else {
			wallet_info_t *winfo;
			char *appid;

			winfo = g_array_index(ad->payment_wallet_list, wallet_info_t *, item->index);
			retv_if(winfo == NULL, NULL);
			appid = winfo->appid;
			if (strlen(wallet_handler) > 0 && strcmp(appid, wallet_handler) == 0) {
				NSA_DEBUG("matched, [%s]", appid);

				elm_radio_value_set(radio_main, item->index);
				elm_genlist_item_bring_in(item->it, ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
			}
			NSA_MEM_FREE(wallet_handler);
		}

		return radio;
	} else {
		return NULL;
	}
}

static Evas_Object *__create_tap_and_pay_list(void *data)
{
	int i;
	appdata *ad = data;
	Evas_Object *genlist;
	Elm_Genlist_Item_Class *itc_title, *itc_list, *itc_padding;
	Evas_Object *circle_obj;

	genlist = elm_genlist_add(ad->navi_frame);
	retv_if(genlist == NULL, NULL);

	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);

	itc_title = elm_genlist_item_class_new();
	itc_title->item_style = "title";
	itc_title->func.text_get = __gl_text_get_title;
	itc_title->func.content_get = NULL;
	itc_title->func.del = NULL;

	itc_list = elm_genlist_item_class_new();
	itc_list->item_style = "1text.1icon.1";
	itc_list->func.text_get = __gl_text_get;
	itc_list->func.content_get = __gl_content_get;
	itc_list->func.state_get = NULL;
	itc_list->func.del = NULL;

	itc_padding = elm_genlist_item_class_new();
	itc_padding->item_style = "padding";

	elm_genlist_item_append(genlist, itc_title, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	for (i = 0; i < ad->payment_wallet_cnt; i++) {
		item_data_t *item;

		NSA_MEM_MALLOC(item, 1, item_data_t);

		item->index = i;
		item->ad = ad;
		item->it = elm_genlist_item_append(genlist, itc_list, item, NULL, ELM_GENLIST_ITEM_NONE, __gl_list_clicked_cb, item);
	}

	elm_genlist_item_append(genlist, itc_padding,
		NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	ad->radio_main = elm_radio_add(genlist);
	elm_radio_state_value_set(ad->radio_main, -1);
	elm_radio_value_set(ad->radio_main, -1);
	evas_object_data_set(genlist, "payment_rdg", ad->radio_main);

	elm_genlist_item_class_free(itc_title);
	elm_genlist_item_class_free(itc_list);
	elm_genlist_item_class_free(itc_padding);

	circle_obj = eext_circle_object_genlist_add(genlist, ad->circle_surface);
	eext_circle_object_genlist_scroller_policy_set(circle_obj,
		ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_obj, EINA_TRUE);

	evas_object_show(genlist);

	return genlist;
}

bool _nsa_view_tap_n_pay_create(void *data)
{
	appdata *ad = data;
	Evas_Object *layout = NULL;

	ad->payment_wallet_list = g_array_new(FALSE, FALSE, sizeof(wallet_info_t *));
	ad->payment_wallet_cnt = 0;

	/* get wallet info */
	__get_wallet_info(ad);

	/* Draw UI */
	if (ad->payment_wallet_cnt == 0)
		layout = __create_no_contents_ly(ad);
	else
		layout = __create_tap_and_pay_list(ad);

	retv_if(layout == NULL, false);

	elm_naviframe_item_push(ad->navi_frame, NULL, NULL, NULL, layout, "empty");
	ad->current_view = V_TAP_N_PAY;

	return true;
}
