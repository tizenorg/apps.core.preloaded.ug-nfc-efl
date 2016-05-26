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


typedef enum {
	T_NFC = 0,
	T_TAP_N_PAY,

	T_MAX
} nfc_setting_type_e;

typedef struct {
	Elm_Object_Item *obj_item[T_MAX];
	Evas_Object *nf;
	bool isProgressing;

	struct appdata *ad;
} main_view_data_t;

static main_view_data_t *view_data;
static char *payment_handler;

///////////////////////////////////////////////////////////////////////////////

static Eina_Bool __back_clicked_cb(void *data, Elm_Object_Item *it)
{
	main_view_data_t *vd = (main_view_data_t *)data;

	NSA_DEBUG("__back_clicked_cb");

	if (vd != NULL) {
		struct appdata *ad = vd->ad;

		_nsa_view_main_destroy(ad);
	}

	ui_app_exit();

	return EINA_FALSE;
}

#if 0
static void __MDM_restriced_popup_response_cb(void *data,
	Evas_Object *obj, void *event_info)
{
	main_view_data_t *vd = (main_view_data_t *)data;
	struct appdata *ad;

	ret_if(vd == NULL);

	ad = (struct appdata *)vd->ad;
	ret_if(ad == NULL);

	ret_if(ad == NULL);
	ret_if(ad->popup == NULL);

	NSA_DEBUG("__MDM_restriced_popup_response_cb");

	elm_popup_dismiss(ad->popup);
}
#endif

static main_view_data_t *__get_view_data()
{
	return view_data;
}

static void __set_view_data(main_view_data_t *data)
{
	view_data  = data;
}

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
	main_view_data_t *vd = (main_view_data_t *)data;
	Elm_Object_Item *it;

	ret_if(vd == NULL);

	it = vd->obj_item[T_NFC];
	ret_if(it == NULL);

	NSA_DEBUG("__update_onoff_obj");

	/* V1.9 : instead of toast popup, disable item during operation */
	elm_object_item_disabled_set(it, false);
	elm_genlist_item_fields_update(it, "*",
		ELM_GENLIST_ITEM_FIELD_CONTENT);
}

static void __update_tap_and_pay_obj(bool activated, void *data)
{
	main_view_data_t *vd = (main_view_data_t *)data;
	Elm_Object_Item *it;

	ret_if(vd == NULL);

	it = vd->obj_item[T_TAP_N_PAY];
	ret_if(it == NULL);

	/* V2.1 */
	if (activated) {
		elm_object_item_disabled_set(it, false);
	} else {
		elm_object_item_disabled_set(it, true);
	}
	elm_genlist_item_fields_update(it, "*",
		ELM_GENLIST_ITEM_FIELD_TEXT);
}

static void __handle_nfc_activation_completed_event(bool activated,
	main_view_data_t *data)
{
	main_view_data_t *vd = (main_view_data_t *)data;

	ret_if(vd == NULL);


	NSA_DEBUG("activated [%d]", activated);

	/* update ui */
	__update_onoff_obj(vd);
	__update_tap_and_pay_obj(activated, vd);
}

static void __nfc_vconf_key_changed (keynode_t *key, void *data)
{
	main_view_data_t *vd = (main_view_data_t *)data;
	int boolval = 0;
	bool activated;

	g_assert(vd != NULL);

	if (VCONF_OK != vconf_get_bool(VCONFKEY_NFC_STATE, &boolval)) {
		activated = false;
	} else {
		if (boolval) {
			activated = true;
		} else {
			activated = false;
		}
	}

	NSA_DEBUG("boolval %d", boolval);
	NSA_DEBUG("nfc mode %s ", activated ? "ON" : "OFF");

	if (!vd->isProgressing) {
		__handle_nfc_activation_completed_event(activated, vd);
	}
}

static void __payment_handler_vconf_key_changed (keynode_t *key,
	void *data)
{
	main_view_data_t *vd = (main_view_data_t *)data;

	NSA_DEBUG("__payment_handler_vconf_key_changed");

	/* Get changed payment handler */
	__get_payment_handler(&payment_handler);

	/* update tap and pay */
	if (nfc_manager_is_activated()) {
		__update_tap_and_pay_obj(true, vd);
	} else {
		__update_tap_and_pay_obj(false, vd);
}
}

/* called when ifself performs activation 	*/
/* vd->isProgressing is set by itself	*/
static void __nfc_activation_completed_cb(nfc_error_e error,
	void *user_data)
{
	main_view_data_t *vd = (main_view_data_t *)user_data;;
	bool activated;

	g_assert(vd != NULL);

	/* handle error cases */
	if (error != NFC_ERROR_NONE) {
		NSA_DEBUG_ERR("__nfc_activation_completed_cb failed");
	}

	/* update ui after activation is completed */
	if (vd->isProgressing) {
		vd->isProgressing = false;
		activated = nfc_manager_is_activated();

		__handle_nfc_activation_completed_event(activated, vd);
	}
}

/* called when someone performs activation except for itself */
static void __nfc_activation_changed_cb(bool activated , void *user_data)
{
	main_view_data_t *vd = (main_view_data_t *)user_data;

	g_assert(vd != NULL);

	NSA_DEBUG("nfc mode %s ", activated ? "ON" : "OFF");

	if (!vd->isProgressing) {
		__handle_nfc_activation_completed_event(activated, vd);
	}
}

static void __change_nfc_onoff_setting(void *data)
{
	main_view_data_t *vd = (main_view_data_t *)data;
	int result;
	bool activated;
	Elm_Object_Item *it;

	ret_if(vd == NULL);

	NSA_BEGIN();

	/* V1.9 : instead of toast popup, disable item during operation */
	it = vd->obj_item[T_NFC];
	if(it != NULL)
		elm_object_item_disabled_set(it, true);

	activated = nfc_manager_is_activated();

	result = nfc_manager_set_activation(!activated,
		__nfc_activation_completed_cb, vd);
	ret_if(result != NFC_ERROR_NONE);

	vd->isProgressing = true;

	NSA_END();
}

static void __gl_list_clicked_cb(void *data, Evas_Object *obj,
	void *event_info)
{
	Elm_Object_Item* it = (Elm_Object_Item *)event_info;
	struct appdata *ad;
	main_view_data_t *vd;
	int index;

	index = (int) data;

	vd = __get_view_data();
	ret_if(vd == NULL);

	ad = vd->ad;
	ret_if(ad == NULL);

	elm_genlist_item_selected_set(it, EINA_FALSE);

	NSA_DEBUG("__gl_list_clicked_cb");

#ifdef MDM_PHASE_2
	if(!__is_nfc_allowed(ad)) {
		__update_onoff_obj(vd);

		/* show popup */
#if 0
		ad->popup = nsa_create_popup_text_1button(
			ad->ly_main,
			IDS_SECURITY_POLICY_PREVENTS_USE_OF_NFC_ABB,
			IMAGES_PATH"/tw_ic_popup_btn_check.png",
			__MDM_restriced_popup_response_cb,
			(void *)vd);
#else
		nsa_create_toast_popup(ad->ly_main,
			IDS_SECURITY_POLICY_PREVENTS_USE_OF_NFC_ABB);
#endif
		return;
	}
#endif

	if (index == T_NFC) {
		__change_nfc_onoff_setting(vd);
	} else if (index == T_TAP_N_PAY){
		_nsa_view_tap_n_pay_create(ad);
	} else {
		return ;
	}
}

static Evas_Object *__gl_content_get_on_off(void *data, Evas_Object *obj,
	const char *part)
{
	main_view_data_t *vd;
	Evas_Object *check;
	int boolval = 0;
	int index;

	index = (int) data;

	vd = __get_view_data();
	retv_if(vd == NULL, NULL);

	if (!strcmp(part, "elm.icon")) {
		check = elm_check_add(obj);
		retv_if(check == NULL, NULL);
		elm_access_object_unregister(check);
		elm_object_style_set(check, "on&off/list");

		if (index == T_NFC) {
			if (VCONF_OK != vconf_get_bool(VCONFKEY_NFC_STATE, &boolval)) {
				elm_check_state_set(check, EINA_FALSE);
			} else {
				if (boolval) {
					elm_check_state_set(check, EINA_TRUE);
				} else {
					elm_check_state_set(check, EINA_FALSE);
				}
			}
		}
		evas_object_size_hint_align_set(check, EVAS_HINT_FILL,
			EVAS_HINT_FILL);
		evas_object_size_hint_weight_set(check, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
		evas_object_propagate_events_set(check, EINA_FALSE);
		evas_object_repeat_events_set(check, EINA_TRUE);

		return check;
	} else {
		return NULL;
	}
}

static char *__gl_text_get_title(void *data, Evas_Object *obj,
	const char *part)
{
	//NSA_DEBUG("part:%s", part);

	if (!strcmp(part, "elm.text")) {
		return strdup(IDS_NFC_NFC);
	} else {
		return NULL;
	}
}

static char *__gl_text_get(void *data, Evas_Object *obj,
	const char *part)
{
	int index = (int)data;

	//NSA_DEBUG("index:%d", index);
	//NSA_DEBUG("part:%s", part);

	if (!strcmp(part, "elm.text")) {
		if (index == T_NFC) {
			return strdup(IDS_NFC_NFC);
		} else if (index == T_TAP_N_PAY){
			return strdup(IDS_TAP_AND_PAY);
		} else {
			return NULL;
		} // elm.text.2
	} else if (!strcmp(part, "elm.text.1")) {
		if (index == T_TAP_N_PAY) {
			if (payment_handler != NULL)
				return strdup(payment_handler);
			else
				return NULL;
		} else {
			return NULL;
		}
	} else {
		return NULL;
	}
}

static Evas_Object *__create_nfc_setting_list(void *data)
{
	main_view_data_t *vd = (main_view_data_t *)data;
	struct appdata *ad;
	Evas_Object *genlist;
	Elm_Genlist_Item_Class *itc_title, *itc_on_off, *itc_list;
	Evas_Object *circle_obj;
	int i;

	retv_if(vd == NULL, NULL);

	ad = vd->ad;
	retv_if(ad == NULL, NULL);

	/* make genlist */
	genlist = elm_genlist_add(vd->nf);
	retv_if(genlist == NULL, NULL);

	/* set genlist property */
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
//	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
//	uxt_genlist_set_bottom_margin_enabled(genlist, EINA_TRUE);

	circle_obj = eext_circle_object_genlist_add(genlist,
		ad->circle_surface);
	eext_circle_object_genlist_scroller_policy_set(circle_obj,
		ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_obj, EINA_TRUE);
	g_array_append_val(ad->circle_obj_list, circle_obj);
	ad->circle_obj_list_idx ++;
	NSA_DEBUG("ad->circle_obj_list_idx %d", ad->circle_obj_list_idx);
	NSA_DEBUG("circle_obj 0x%p", circle_obj);

	/* create genlist class + */
	itc_title = elm_genlist_item_class_new();
	itc_title->item_style = "title";
	itc_title->func.text_get = __gl_text_get_title;
	itc_title->func.content_get = NULL;
	itc_title->func.del = NULL;

	itc_on_off = elm_genlist_item_class_new();
	itc_on_off->item_style = "1text.1icon.1";
	itc_on_off->func.text_get = __gl_text_get;
	itc_on_off->func.content_get = __gl_content_get_on_off;
	itc_on_off->func.state_get = NULL;
	itc_on_off->func.del = NULL;

	itc_list = elm_genlist_item_class_new();
	__get_payment_handler(&payment_handler);
	if (payment_handler != NULL)
		itc_list->item_style = "2text";
	else
		itc_list->item_style = "1text";
	itc_list->func.text_get = __gl_text_get;
	itc_list->func.content_get = NULL;
	itc_list->func.state_get = NULL;
	itc_list->func.del = NULL;
	/* create genlist class - */

	/* ADD: ITEM */
	elm_genlist_item_append(genlist,
		itc_title, NULL,
		NULL, ELM_GENLIST_ITEM_NONE,
		NULL, NULL);

	for (i = 0; i < T_MAX; i++) {
		Elm_Genlist_Item_Class *itc;

		if (i == T_NFC)
			itc = itc_on_off;
		else
			itc = itc_list;

		vd->obj_item[i] = elm_genlist_item_append(genlist,
			itc, (void *)i,
			NULL, ELM_GENLIST_ITEM_NONE,
			__gl_list_clicked_cb, (void *)i);
	}

	elm_genlist_item_class_free(itc_title);
	elm_genlist_item_class_free(itc_on_off);
	elm_genlist_item_class_free(itc_list);

	/* V2.1 */
	if (!nfc_manager_is_activated())
		__update_tap_and_pay_obj(false, vd);

	evas_object_show(genlist);

	return genlist;
}

int _nsa_view_main_create(void *data)
{
	struct appdata *ad = (struct appdata *)data;
	main_view_data_t *vd;
	Evas_Object *list;

	/* create data */
	NSA_MEM_MALLOC(vd, 1, main_view_data_t);
	retv_if(vd == NULL, NSA_ERROR);
	__set_view_data(vd);
	vd->ad = ad;

	/* set appdata to main view data */
	vd->nf = ad->navi_frame;

	/* Draw UI */
	list = __create_nfc_setting_list(vd);
	retv_if(list == NULL, NSA_ERROR);

	/* Push navifreme + */
	eext_object_event_callback_add(vd->nf, EEXT_CALLBACK_BACK,
		eext_naviframe_back_cb, NULL);

	ad->base_navi_it = elm_naviframe_item_push(
		vd->nf,
		NULL,
		NULL,
		NULL,
		list,
		"empty");
	elm_naviframe_item_pop_cb_set(ad->base_navi_it, __back_clicked_cb,
		vd);
	/* Push navifreme - */

	/* register nfc callback */
	nfc_manager_set_activation_changed_cb(
				__nfc_activation_changed_cb, vd);

	if (VCONF_OK != vconf_notify_key_changed(VCONFKEY_NFC_STATE,
		__nfc_vconf_key_changed, vd)) {
		NSA_DEBUG_ERR("vconf_notify_key_changed error");
	}

	if (VCONF_OK != vconf_notify_key_changed(VCONFKEY_NFC_PAYMENT_HANDLERS,
		__payment_handler_vconf_key_changed, vd)) {
		NSA_DEBUG_ERR("vconf_notify_key_changed error");
	}

	return NSA_OK;
}

void _nsa_view_main_destroy(void *data)
{
	struct appdata *ad;

	__set_view_data(NULL);

	/* unregister nfc callback */
	nfc_manager_unset_activation_changed_cb();

	if (VCONF_OK != vconf_ignore_key_changed(VCONFKEY_NFC_STATE,
		__nfc_vconf_key_changed)) {
		NSA_DEBUG_ERR("vconf_notify_key_changed error");
	}

	if (VCONF_OK != vconf_ignore_key_changed(VCONFKEY_NFC_PAYMENT_HANDLERS,
		__payment_handler_vconf_key_changed)) {
		NSA_DEBUG_ERR("vconf_ignore_key_changed error");
	}

	ad = (struct appdata *)data;
	ret_if(ad == NULL);
}

