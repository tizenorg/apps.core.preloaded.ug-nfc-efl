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
	Evas_Object *nf;
	Evas_Object *genlist;
	Evas_Object *radio_main;
	struct appdata *ad;
} tap_and_pay_view_data_t;

typedef struct {
	GArray *list;
	int index;
	Elm_Object_Item *it;
	void *data;
} item_data_t;

typedef struct {
	char *appid;
	int aid_count;
} wallet_info_t;

typedef enum {
	T_PAYMENT = 0,
	T_OTHERS,

	T_MAX
} tap_and_pay_wallet_type_e;


static int payment_wallet_cnt;
static int other_wallet_cnt;

static GArray *payment_wallet_list;
static GArray *other_wallet_list;

static Eina_Bool __back_clicked_cb(void *data, Elm_Object_Item *it)
{
	tap_and_pay_view_data_t *vd = (tap_and_pay_view_data_t *)data;

	NSA_DEBUG("__back_clicked_cb");

	if (vd != NULL) {
		struct appdata *ad = vd->ad;

		_nsa_view_tap_n_pay_destroy(ad);

		if (ad->circle_obj_list_idx < 0) {
			ui_app_exit();

			return EINA_FALSE;
		}
	}

	return EINA_TRUE;
}

static void __max_AID_popup_response_cb(void *data,
	Evas_Object *obj, void *event_info)
{
	item_data_t *item = (item_data_t *)data;
	tap_and_pay_view_data_t *vd;
	struct appdata *ad;
	const char *vconf_key;
	char *wallet_handler;

	vd = (tap_and_pay_view_data_t *)item->data;
	ret_if(vd == NULL);

	ad = (struct appdata *)vd->ad;
	ret_if(ad == NULL);

	ret_if(ad == NULL);
	ret_if(ad->popup == NULL);

	NSA_DEBUG("__max_AID_popup_response_cb");

	/* restore default handler */
	if (item->list == payment_wallet_list) {
		vconf_key = VCONFKEY_NFC_PAYMENT_HANDLERS;
	} else {
		vconf_key = VCONFKEY_NFC_OTHER_HANDLERS;
	}
	wallet_handler = vconf_get_str(vconf_key);
	if (wallet_handler == NULL || payment_wallet_list == NULL) {
		NSA_DEBUG_ERR("vconf_get_str error");
	} else {
		int i;

		for (i = 0; i < payment_wallet_cnt; i++) {
			wallet_info_t *winfo;

			winfo = g_array_index(item->list, wallet_info_t *, i);
			if (!winfo)
				continue;

			if (strlen(wallet_handler) > 0 &&
				strcmp(winfo->appid, wallet_handler) == 0) {
				NSA_DEBUG("matched, [%s]", winfo->appid);

				if (vd->radio_main != NULL)
					elm_radio_value_set(vd->radio_main, i);
			}
		}
	}
	NSA_MEM_FREE(wallet_handler);

	elm_popup_dismiss(ad->popup);

	if (ad->circle_obj_list_idx >= 0) {
		Evas_Object *circle_obj;

		circle_obj = g_array_index(ad->circle_obj_list, Evas_Object *,
			ad->circle_obj_list_idx);
		eext_rotary_object_event_activated_set(circle_obj, EINA_TRUE);
		NSA_DEBUG("restore circle object 0x%p", circle_obj);
	}
}

static void __add_item_to_wallet_list(const char *item, int aid_count,
	int *wallet_count, GArray **list)
{
	int wcount;
	GArray *wlist;
	wallet_info_t *winfo;

	wcount = *wallet_count;
	wlist = *list;

	if (wlist == NULL) {
		wlist = g_array_new(FALSE, FALSE, sizeof(wallet_info_t *));
		*list = wlist;
	}

	NSA_MEM_MALLOC(winfo, 1, wallet_info_t);
	ret_if(winfo == NULL);
	NSA_MEM_STRDUP(winfo->appid, item);
	winfo->aid_count = aid_count;

	g_array_append_val(wlist, winfo);

	wcount++;
	*wallet_count = wcount;

	NSA_DEBUG("Done");
}

static bool __registered_handler_cb(const char *handler, int count,
	void *user_data)
{
	if (handler) {
		int type = (int)user_data;

		NSA_DEBUG("package name: [%s]", handler);
		NSA_DEBUG("AID number: [%d]", count);

		if (type == T_PAYMENT) {
			__add_item_to_wallet_list(handler, count,
				&payment_wallet_cnt, &payment_wallet_list);
		} else if (type == T_OTHERS) {
			__add_item_to_wallet_list(handler, count,
				&other_wallet_cnt, &other_wallet_list);
		} else {
			NSA_DEBUG_ERR("wallet type error");
			return true;
		}
	}

	return true;
}

static void __get_wallet_info(void)
{
	NSA_DEBUG("__get_wallet_info");

	nfc_se_foreach_registered_handlers(NFC_CARD_EMULATION_CATEGORY_PAYMENT,
		__registered_handler_cb, (void *)T_PAYMENT);

	/* To Do */
#if 0
	nfc_se_foreach_registered_handlers(NFC_CARD_EMULATION_CATEGORY_OTHER,
		__registered_handler_cb, (void *)T_OTHERS);
#endif
}

static int __get_payment_wallet_list_cnt(void)
{
	NSA_DEBUG("__get_payment_wallet_list_cnt [%d]", payment_wallet_cnt);

	return payment_wallet_cnt;
}

static int __get_other_wallet_list_cnt(void)
{
	NSA_DEBUG("__get_other_wallet_list_cnt [%d]", other_wallet_cnt);

	return other_wallet_cnt;
}

static char* __decorated_text_effect_get(int size,
	const char* color_code, const char* text, const char* align)
{
	char* decorated = NULL;
	Eina_Strbuf* buf = NULL;
	int r = 0;
	int g = 0;
	int b = 0;
	int a = 0;

	if(color_code == NULL)
		return NULL;

	if(text == NULL)
		return g_strdup("");

//	uxt_theme_get_color(color_code, &r, &g, &b, &a, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	buf = eina_strbuf_new();

	if(align == NULL) {
		eina_strbuf_append_printf(buf,
			"<font_size=%d><color=#%02x%02x%02x%02x>%s</color></font_size>",
			size, r, g, b, a, text);
	} else {
		eina_strbuf_append_printf(buf,
			"<font_size=%d align=%s><color=#%02x%02x%02x%02x>%s</color></font_size>",
			size, align, r, g, b, a, text);
	}

	decorated = eina_strbuf_string_steal(buf);
	eina_strbuf_free(buf);

	return decorated;
}

static Evas_Object *__create_no_contents_ly(void *data)
{
	tap_and_pay_view_data_t *vd = (tap_and_pay_view_data_t *)data;
	struct appdata *ad;
	Evas_Object *sc;
	Evas_Object *ly;
	Evas_Object *image;
	Evas_Object *circle_obj;
	Evas_Object *label;
	char *title_markup;
	char *title;

	retv_if(vd == NULL, NULL);
	retv_if(vd->nf == NULL, NULL);

	ad = vd->ad;
	retv_if(ad == NULL, NULL);

	sc = elm_scroller_add(vd->nf);
	evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(sc, EVAS_HINT_FILL,
			EVAS_HINT_FILL);
	elm_scroller_single_direction_set(sc, ELM_SCROLLER_SINGLE_DIRECTION_HARD);
	elm_object_style_set(sc, "effect");
	evas_object_show(sc);

	/* create layout */
	ly = nsa_create_edj_layout(sc, "tap_n_pay/nocontents");
	retv_if(ly == NULL, NULL);

	/* create image object */
	image = elm_image_add(ly);
	retv_if(image == NULL, NULL);
	elm_image_file_set(image,
		IMAGES_PATH"/b_setting_payment_help_image.png", NULL);
//	uxt_theme_object_set_color(image, "AO018");
	elm_object_part_content_set(ly, "elm.swallow.icon", image);

	/* set help text */
	elm_object_part_text_set(ly, "elm.text",
		IDS_NO_INSTALLED_PAYMENT_APPS);

	/* get title text */
	title_markup = elm_entry_utf8_to_markup(IDS_TAP_AND_PAY);
	title = __decorated_text_effect_get(30, "T012", title_markup, "center");
	NSA_MEM_FREE(title_markup);

	/* make title label */
	label = elm_label_add(ly);
	evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(label, EVAS_HINT_FILL,
		EVAS_HINT_FILL);
	elm_object_style_set(label, "slide_roll");
	elm_label_wrap_width_set(label, 226);
	elm_label_slide_duration_set(label, 3);
	elm_label_slide_mode_set(label, ELM_LABEL_SLIDE_MODE_AUTO);
	elm_label_ellipsis_set(label, EINA_TRUE);

	/* set title text */
	elm_object_text_set(label, title);

	/* start title sliding  */
	elm_label_slide_go(label);

	/* set title label to layout */
	elm_object_part_content_set(ly, "elm.text.title", label);
	evas_object_show(label);
	NSA_MEM_FREE(title);

	evas_object_show(ly);

	elm_object_content_set(sc, ly);

	/* disable old circle object */
	if (ad->circle_obj_list_idx >= 0) {
		circle_obj = g_array_index(ad->circle_obj_list, Evas_Object *,
			ad->circle_obj_list_idx);
		NSA_DEBUG("old circle_obj 0x%p", circle_obj);
	}

	/* enable new circle object */
	circle_obj = eext_circle_object_scroller_add(sc,
		ad->circle_surface);
	eext_circle_object_scroller_policy_set(circle_obj,
		ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_obj, EINA_TRUE);
	g_array_append_val(ad->circle_obj_list, circle_obj);
	ad->circle_obj_list_idx ++;
	NSA_DEBUG("new circle_obj 0x%p", circle_obj);
	NSA_DEBUG("ad->circle_obj_list_idx %d", ad->circle_obj_list_idx);

	return sc;
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
	const char *vconf_key;
	bool duplicate = false;

	retv_if(item == NULL, false);
	retv_if(item->list == NULL, false);


	winfo = g_array_index(item->list, wallet_info_t *, item->index);
	retv_if(winfo == NULL, false);
	new_wallet = winfo->appid;

	if (item->list == payment_wallet_list) {
		vconf_key = VCONFKEY_NFC_PAYMENT_HANDLERS;
	} else {
		vconf_key = VCONFKEY_NFC_OTHER_HANDLERS;
	}
	NSA_DEBUG("vconf_key [%s]", vconf_key);

	old_wallet = vconf_get_str(vconf_key);
	if (old_wallet != NULL) {
		if (strlen(new_wallet) > 0 &&
			strcmp(old_wallet, new_wallet) == 0) {
			duplicate = true;
			NSA_DEBUG("Same wallet is selected!");
		}
		NSA_MEM_FREE(old_wallet);
	}

	if (!duplicate) {
		int ret;
		char *launch_app_id = NULL;

		/* check if there are more than 50 AIDs */
		if (winfo->aid_count > MAX_AIDS_COUNT) {
			tap_and_pay_view_data_t *vd;
			struct appdata *ad;

			vd = (tap_and_pay_view_data_t *)item->data;
			retv_if(vd == NULL, false);

			ad = (struct appdata *)vd->ad;
			retv_if(ad == NULL, false);

			/* initialize radio button */
			if (vd->radio_main != NULL)
				elm_radio_value_set(vd->radio_main, -1);

			/* show max popup */
			ad->popup = nsa_create_popup_text_1button(
				ad->ly_main,
				IDS_UNABLE_TO_SELECT_PAYMENT_APP_WITH_MORE_THAN_50_REGISTERED_CARDS,
				IMAGES_PATH"/tw_ic_popup_btn_check.png",
				__max_AID_popup_response_cb,
				(void *)item);

			return false;
		}


		ret = vconf_set_str(vconf_key, new_wallet);
		if (ret != VCONF_OK) {
			NSA_DEBUG_ERR("error vconf_set_str");

			return false;
		}

		/* get launch app id */
		launch_app_id = __get_launch_app_id(new_wallet);

		/* launch app */
		__launch_application(launch_app_id);

		return true;
	}

	return false;
}

static void __radio_clicked_cb(void *data, Evas_Object *obj,
	void *event_info)
{
	int index = 0;

	index = elm_radio_value_get(obj);
	NSA_DEBUG("selected value for the group:%d", index);

	/* When radio is clicked, we don't need to call __set_default_wallet */
	/* Because __gl_list_clicked_cb is called, too */
}

static void __gl_list_clicked_cb(void *data, Evas_Object *obj,
	void *event_info)
{
	Elm_Object_Item* it = (Elm_Object_Item *)event_info;
	item_data_t *item = (item_data_t *)data;
	Evas_Object *radio_main = evas_object_data_get(obj, "payment_rdg");

	elm_genlist_item_selected_set(it, EINA_FALSE);

	ret_if(item == NULL);
	ret_if(radio_main == NULL);

	NSA_DEBUG("selected value for the group:%d", item->index);

	if (__set_default_wallet(item)) {
		elm_radio_value_set(radio_main, item->index);

		NSA_DEBUG("__set_default_wallet success");
	} else {
		NSA_DEBUG_ERR("set default wallet failed");
	}
}

static Evas_Object *__gl_content_get(void *data, Evas_Object *obj,
	const char *part)
{
	if (!strcmp(part, "elm.icon")) {
		item_data_t *item = (item_data_t *) data;
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
		evas_object_smart_callback_add(radio, "changed",
				__radio_clicked_cb, (void *)item);

		if (item->list == payment_wallet_list) {
			wallet_handler = vconf_get_str(VCONFKEY_NFC_PAYMENT_HANDLERS);
		} else {
			wallet_handler = vconf_get_str(VCONFKEY_NFC_OTHER_HANDLERS);
		}

		if (wallet_handler == NULL) {
			NSA_DEBUG_ERR("vconf_get_str error");
		} else {
			wallet_info_t *winfo;
			char *appid;

			winfo = g_array_index(item->list, wallet_info_t *,
				item->index);
			retv_if(winfo == NULL, NULL);
			appid = winfo->appid;
			if (strlen(wallet_handler) > 0 &&
				strcmp(appid, wallet_handler) == 0) {
				NSA_DEBUG("matched, [%s]", appid);

				elm_radio_value_set(radio_main, item->index);
				elm_genlist_item_bring_in(item->it,
					ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
			}
			NSA_MEM_FREE(wallet_handler);
		}

		return radio;
	} else {
		return NULL;
	}
}

static char *__gl_text_get_title(void *data, Evas_Object *obj,
	const char *part)
{
	//NSA_DEBUG("part:%s", part);

	if (!strcmp(part, "elm.text")) {
		return strdup(IDS_TAP_AND_PAY);
	} else {
		return NULL;
	}
}

static char *__gl_text_get_group(void *data, Evas_Object *obj,
	const char *part)
{
	int index = (int) data;

	NSA_DEBUG("part: %s", part);
	NSA_DEBUG("index: %d", index);

	if (!strcmp(part, "elm.text")) {
		if (index == T_PAYMENT)
			return strdup("Payment");
		else if (index == T_OTHERS)
			return strdup("Others");
		else
			return NULL;
	} else {
		return NULL;
	}
}


static char *__gl_text_get(void *data, Evas_Object *obj,
	const char *part)
{
	NSA_DEBUG("part: %s", part);

	if (!strcmp(part, "elm.text")) {
		item_data_t *item = (item_data_t *) data;
		wallet_info_t *winfo;
		char *appid;

		retv_if(item == NULL, NULL);

		winfo = g_array_index(item->list, wallet_info_t *,
				item->index);
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

static void __add_group_n_list_to_genlist(Evas_Object *genlist,
	Elm_Genlist_Item_Class *itc_group,
	Elm_Genlist_Item_Class *itc_list,
	tap_and_pay_wallet_type_e type, int count, void *data)
{
	int index;
	GArray *list;

#if 0
	Elm_Object_Item *parent;

	/* add payment group */
	parent = elm_genlist_item_append(genlist,
			itc_group, (void *)type,
			NULL, ELM_GENLIST_ITEM_GROUP,
			NULL, NULL);

	elm_genlist_item_select_mode_set(parent,
		ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
#endif

	if (type == T_PAYMENT) {
		NSA_DEBUG("T_PAYMENT");
		list = payment_wallet_list;
	} else {
		NSA_DEBUG("T_OTHERS");
		list = other_wallet_list;
	}

	/* add payment wallet list */
	for (index = 0; index < count; index++) {
		item_data_t *item;

		NSA_MEM_MALLOC(item, 1, item_data_t);

		item->list = list;
		item->index = index;
		item->it = elm_genlist_item_append(genlist,
			itc_list, (void *)item,
			NULL, ELM_GENLIST_ITEM_NONE,
			__gl_list_clicked_cb, (void *)item);
		item->data = data;
	}
}

static Evas_Object *__create_tap_and_pay_list(void *data)
{
	tap_and_pay_view_data_t *vd = (tap_and_pay_view_data_t *)data;
	struct appdata *ad;
	Evas_Object *genlist;
	Elm_Genlist_Item_Class *itc_title, *itc_group, *itc_list;
	Evas_Object *circle_obj;
	int count;

	retv_if(vd == NULL, NULL);

	ad = vd->ad;
	retv_if(ad == NULL, NULL);

	/* make genlist */
	genlist = elm_genlist_add(vd->nf);
	retv_if(genlist == NULL, NULL);

	/* set genlist property */
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
//	uxt_genlist_set_bottom_margin_enabled(genlist, EINA_TRUE);

	/* disable old circle object */
	if (ad->circle_obj_list_idx >= 0) {
		circle_obj = g_array_index(ad->circle_obj_list, Evas_Object *,
			ad->circle_obj_list_idx);
		NSA_DEBUG("old circle_obj 0x%p", circle_obj);
	}

	/* enable new circle object */
	circle_obj = eext_circle_object_genlist_add(genlist,
		ad->circle_surface);
	eext_circle_object_genlist_scroller_policy_set(circle_obj,
		ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	eext_rotary_object_event_activated_set(circle_obj, EINA_TRUE);
	g_array_append_val(ad->circle_obj_list, circle_obj);
	ad->circle_obj_list_idx ++;
	NSA_DEBUG("new circle_obj 0x%p", circle_obj);
	NSA_DEBUG("ad->circle_obj_list_idx %d", ad->circle_obj_list_idx);

	/* create genlist class + */
	itc_title = elm_genlist_item_class_new();
	itc_title->item_style = "title";
	itc_title->func.text_get = __gl_text_get_title;
	itc_title->func.content_get = NULL;
	itc_title->func.del = NULL;

	itc_group = elm_genlist_item_class_new();
	itc_group->item_style = "groupindex";
	itc_group->func.text_get = __gl_text_get_group;
	itc_group->func.content_get = NULL;
	itc_group->func.del = NULL;

	itc_list = elm_genlist_item_class_new();
	itc_list->item_style = "1text.1icon.1";
	itc_list->func.text_get = __gl_text_get;
	itc_list->func.content_get = __gl_content_get;
	itc_list->func.state_get = NULL;
	itc_list->func.del = NULL;
	/* create genlist class - */

	/* ADD: ITEM */
	elm_genlist_item_append(genlist,
		itc_title, NULL,
		NULL, ELM_GENLIST_ITEM_NONE,
		NULL, NULL);

	if ((count = __get_payment_wallet_list_cnt())) {
		Evas_Object *radio_main;

		__add_group_n_list_to_genlist(genlist, itc_group,
			itc_list, T_PAYMENT, count, vd);

		/*make radio group */
		radio_main = elm_radio_add(genlist);
		elm_radio_state_value_set(radio_main, -1);
		elm_radio_value_set(radio_main, -1);
		evas_object_data_set(genlist, "payment_rdg", radio_main);
		vd->radio_main = radio_main;
	}

	if ((count = __get_other_wallet_list_cnt())) {
		__add_group_n_list_to_genlist(genlist, itc_group,
			itc_list, T_OTHERS, count, vd);
	}


	/* TO DO: get wallet lists*/

	elm_genlist_item_class_free(itc_title);
	elm_genlist_item_class_free(itc_group);
	elm_genlist_item_class_free(itc_list);

	evas_object_show(genlist);

	vd->genlist = genlist;

	return genlist;
}


int _nsa_view_tap_n_pay_create(void *data)
{
	struct appdata *ad = (struct appdata *)data;
	tap_and_pay_view_data_t *vd;
	Evas_Object *layout;
	int count;

	/* create data */
	NSA_MEM_MALLOC(vd, 1, tap_and_pay_view_data_t);
	retv_if(vd == NULL, NSA_ERROR);
	vd->ad = ad;

	/* set appdata to main view data */
	vd->nf = ad->navi_frame;

	/* get wallet info */
	__get_wallet_info();

	/* Draw UI */
	count = (__get_payment_wallet_list_cnt() +
			__get_other_wallet_list_cnt());
	if (count == 0) {
		layout = __create_no_contents_ly(vd);
		retv_if(layout == NULL, NSA_ERROR);
	} else {
		layout = __create_tap_and_pay_list(vd);
		retv_if(layout == NULL, NSA_ERROR);
	}

	/* Push navifreme + */
	eext_object_event_callback_add(vd->nf, EEXT_CALLBACK_BACK,
		eext_naviframe_back_cb, NULL);

	ad->base_navi_it = elm_naviframe_item_push(
		vd->nf,
		NULL,
		NULL,
		NULL,
		layout,
		"empty");
	elm_naviframe_item_pop_cb_set(ad->base_navi_it, __back_clicked_cb,
		vd);
	/* Push navifreme - */

	return NSA_OK;
}

void _nsa_view_tap_n_pay_destroy(void *data)
{
	struct appdata *ad = (struct appdata *)data;
	Evas_Object *circle_obj;

	ret_if(ad == NULL);

	/* remove current circle object */
	circle_obj = g_array_index(ad->circle_obj_list, Evas_Object *,
		ad->circle_obj_list_idx);
	NSA_DEBUG("current view circle_obj 0x%p", circle_obj);
	g_array_remove_index(ad->circle_obj_list, ad->circle_obj_list_idx--);

	/* restore circle object */
	if (ad->circle_obj_list_idx >= 0) {
		circle_obj = g_array_index(ad->circle_obj_list, Evas_Object *,
			ad->circle_obj_list_idx);
		eext_rotary_object_event_activated_set(circle_obj, EINA_TRUE);
		NSA_DEBUG("restored view circle_obj 0x%p", circle_obj);
	}

	/* free wallet list info */
	if (payment_wallet_list != NULL) {
		int i;

		for (i = 0; i < payment_wallet_cnt; i++) {
			wallet_info_t *winfo = NULL;

			winfo = g_array_index(payment_wallet_list,
				wallet_info_t *, i);
			NSA_MEM_FREE(winfo->appid);
			NSA_MEM_FREE(winfo);
		}

		g_array_free(payment_wallet_list, FALSE);
		payment_wallet_list = NULL;
		payment_wallet_cnt = 0;
	}

	NSA_DEBUG("ad->circle_obj_list_idx %d", ad->circle_obj_list_idx);
}


