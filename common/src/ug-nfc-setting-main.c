/*
  * Copyright (c) 2012, 2013 Samsung Electronics Co., Ltd.
  *
  * Licensed under the Flora License, Version 1.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at

  *     http://floralicense.org/license/
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */

#include <efl_extension.h>
#include <notification.h>

#include "ug-nfc-setting-main.h"

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

static Elm_Genlist_Item_Class itc_helptext;
static Elm_Genlist_Item_Class itc_onoff;

static bool pending_status = false;

static bool __get_pending_status(void)
{
	return pending_status;
}

static void __set_pending_status(bool status)
{
	pending_status = status;
}

static void __update_title_onoff_obj(void *data)
{
	ugdata_t *ug_data = (ugdata_t *)data;
	int boolval;

	LOGD("BEGIN >>>>");

	if (!ug_data)
		return;

	if (__get_pending_status()) {
		elm_object_disabled_set(ug_data->ns_on_off, EINA_TRUE);
		return;
	}

	elm_object_disabled_set(ug_data->ns_on_off, EINA_FALSE);

	if (!vconf_get_bool(VCONFKEY_NFC_STATE, &boolval) && boolval)
		elm_check_state_set(ug_data->ns_on_off, EINA_TRUE);
	else
		elm_check_state_set(ug_data->ns_on_off, EINA_FALSE);

	LOGD("END >>>>");
}

static bool __reply_to_launch_request(app_control_h service, app_control_result_e result)
{
	app_control_h reply;
	char *operation = NULL;
	bool ret = false;

	LOGD("BEGIN >>>>");

	if (service != NULL) {
		app_control_create(&reply);
		app_control_get_operation(service, &operation);

		if (operation != NULL) {
			LOGD("reply to launch request : operation %s", operation);
			app_control_reply_to_launch_request(reply, service, result);
			ret = true;
		}

		app_control_destroy(reply);
	}

	LOGD("END >>>>");

	return ret;
}

static void __nfc_activation_completed_cb(nfc_error_e error,
	void *user_data)
{
	LOGD("BEGIN >>>>");

	if (error != NFC_ERROR_NONE) {
		LOGE("__nfc_activation_completed_cb failed");

		/* show failure popup */
	}

	LOGD("END >>>>");
}

static void __nfc_activation_changed_cb(bool activated , void *user_data)
{
	ugdata_t *ug_data = (ugdata_t *)user_data;

	LOGD("BEGIN >>>>");

	LOGD("nfc mode %s ", activated ? "ON" : "OFF");

	nfc_manager_unset_activation_changed_cb();

	/* nfc setting ui updated */
	__set_pending_status(false);

	__update_title_onoff_obj(ug_data);

	if (__reply_to_launch_request(ug_data->service, APP_CONTROL_RESULT_SUCCEEDED) == true)
		ug_destroy_me(ug_data->nfc_setting_ug);

	LOGD("END >>>>");
}

static Eina_Bool __back_clicked_cb(void *data, Elm_Object_Item *it)
{
	ugdata_t *ug_data = (ugdata_t *)data;

	LOGD("BEGIN >>>>");

	if (!ug_data) {
		LOGE("data is null");
		return EINA_FALSE;
	}

	__reply_to_launch_request(ug_data->service, APP_CONTROL_RESULT_FAILED);

	ug_destroy_me(ug_data->nfc_setting_ug);

	LOGD("END >>>>");

	return EINA_FALSE;
}

static void __change_nfc_onoff_setting(void *data)
{
	ugdata_t *ug_data = (ugdata_t *)data;
	int result, boolval;

	LOGD("BEGIN >>>>");

	if (!ug_data)
		return;

	if (!vconf_get_bool(VCONFKEY_NFC_STATE, &boolval)) {
		LOGD("vconf_get_bool status [%d]", boolval);

		if (NFC_ERROR_NONE == nfc_manager_initialize()) {

			/* Register activation changed callback */
			nfc_manager_set_activation_changed_cb(
				__nfc_activation_changed_cb, ug_data);

			result = nfc_manager_set_activation(!boolval,
				__nfc_activation_completed_cb, ug_data);
			if (result != NFC_ERROR_NONE) {
				LOGE("nfc_manager_set_activation failed");
				return;
			}

			__set_pending_status(true);
		} else {
			LOGE("nfc_manager_initialize FAIL!!!!");
		}


	} else {
		LOGE("vconf_get_bool failed");
	}

	__update_title_onoff_obj(ug_data);

	LOGD("END >>>>");
}

static void __title_ns_on_off_clicked_cb(void *data, Evas_Object *obj,
	void *event_info)
{
	ugdata_t *ug_data = (ugdata_t *)data;

	LOGD("BEGIN >>>>");

	if (ug_data == NULL) {
		LOGE("data is null");
		return;
	}

	if (__get_pending_status())
		return;

	__change_nfc_onoff_setting(ug_data);

	LOGD("END >>>>");
}

static void __nfc_sel(void *data, Evas_Object *obj, void *event_info)
{
	LOGD("BEGIN >>>>");

	__title_ns_on_off_clicked_cb(data, obj, event_info);

	elm_genlist_item_selected_set((Elm_Object_Item *)event_info, EINA_FALSE);

	LOGD("END >>>>");
}

static Evas_Object *__gl_content_get(void *data, Evas_Object *obj,
	const char *part)
{
	ugdata_t *ug_data = (ugdata_t *)data;
	int boolval = false;

	LOGD("BEGIN >>>>");

	if (!ug_data) {
		LOGE("invalid parameter");
		return NULL;
	}

	if (!strncmp(part, "elm.swallow.end", strlen(part))) {
		Evas_Object *btn = NULL;

		vconf_get_bool(VCONFKEY_NFC_STATE, &boolval);

		LOGD("elm.swallow.end");

		btn = elm_check_add(obj);
		elm_object_style_set(btn, "on&off");
		elm_check_state_set(btn, boolval);
		evas_object_propagate_events_set(btn, EINA_FALSE);
		evas_object_smart_callback_add(btn, "changed", __title_ns_on_off_clicked_cb, ug_data);
		evas_object_show(btn);

		ug_data->ns_on_off = btn;

		__update_title_onoff_obj(ug_data);

		return btn;
	}

	LOGD("END >>>>");

	return NULL;
}

static char *__gl_text_get(void *data, Evas_Object *obj, const char *part)
{
	return strcmp(part, "elm.text") == 0 ? strdup(IDS_NFC_NFC) : NULL;
}

static char *__gl_text_get_desc(void *data, Evas_Object *obj, const char *part)
{
	return strcmp(part, "elm.text.multiline") == 0 ? strdup(IDS_NFC_DESCRIPTION_MSG) : NULL;
}

static Evas_Object *__create_nfc_list(void *data)
{
	ugdata_t *ug_data = (ugdata_t *)data;
	Evas_Object *genlist = NULL;
	Elm_Object_Item *onoff_item;

	LOGD("BEGIN >>>>");

	/* make genlist */
	genlist = elm_genlist_add(ug_data->base_naviframe);
	if (genlist == NULL) {
		LOGE("genlist is null");
		return NULL;
	}

	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);

	itc_onoff.item_style = "type1";
	itc_onoff.func.text_get = __gl_text_get;
	itc_onoff.func.content_get = __gl_content_get;

	itc_helptext.item_style = "multiline";
	itc_helptext.func.text_get = __gl_text_get_desc;

	/* Append ON / OFF list */
	onoff_item = elm_genlist_item_append(genlist, &itc_onoff,
				(void *) ug_data, NULL, ELM_GENLIST_ITEM_NONE,
				__nfc_sel, (void *) ug_data);

	elm_object_item_signal_emit(onoff_item, "elm,state,top", "");

	/* Append help text */
	static Elm_Object_Item *help_item;
	help_item = elm_genlist_item_append(genlist, &itc_helptext, (void *)0,
		NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(help_item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	elm_object_item_access_unregister(help_item);

	evas_object_show(genlist);

	LOGD("END >>>>");

	return genlist;
}

static Evas_Object *__create_bg(Evas_Object *parent, char *style)
{
	Evas_Object *bg = elm_bg_add(parent);

	LOGD("BEGIN >>>>");

	evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if (style)
		elm_object_style_set(bg, style);

	elm_win_resize_object_add(parent, bg);

	evas_object_show(bg);

	LOGD("END >>>>");

	return bg;
}

static Evas_Object *__create_main_layout(Evas_Object *parent)
{
	Evas_Object *layout;

	LOGD("BEGIN >>>>");

	if (parent == NULL)
		return NULL;

	layout = elm_layout_add(parent);

	elm_layout_theme_set(layout, "layout", "application", "default");

	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	evas_object_show(layout);

	LOGD("END >>>>");

	return layout;
}

static void *__ug_nfc_create(ui_gadget_h ug, enum ug_mode mode,
	app_control_h service, void *priv)
{
	ugdata_t *ug_data = (ugdata_t *)priv;
	Evas_Object *parent = NULL;
	Evas_Object *l_button = NULL;
	Evas_Object *nfc_setting_list = NULL;

	LOGD("BEGIN >>>>");

	/* set text domain */
	bindtextdomain(NFCUG_TEXT_DOMAIN, NFCUG_LOCALEDIR);

	/* create base view */
	parent = ug_get_parent_layout(ug);
	if (!parent)
		return NULL;

	ug_data->ug_win_main = parent;
	evas_object_show(ug_data->ug_win_main);
	ug_data->nfc_setting_ug = ug;

	ug_data->base_layout = __create_main_layout(ug_data->ug_win_main);
	ug_data->bg = __create_bg(ug_data->ug_win_main, "group_list");
	elm_object_part_content_set(ug_data->base_layout, "elm.swallow.bg", ug_data->bg);

	ug_data->base_naviframe = elm_naviframe_add(ug_data->base_layout);
	elm_object_part_content_set(ug_data->base_layout, "elm.swallow.content",
		ug_data->base_naviframe);

	evas_object_show(ug_data->base_naviframe);

	ug_data->service = service;
	nfc_setting_list = __create_nfc_list(ug_data);

	if (nfc_setting_list == NULL) {
		LOGE("create nfc list failed");
		return NULL;
	}

	/* Push navifreme */
	l_button = elm_button_add(ug_data->base_naviframe);
	elm_object_style_set(l_button, "naviframe/back_btn/default");
	evas_object_smart_callback_add(l_button, "clicked", __back_clicked_cb, ug_data);

	eext_object_event_callback_add(ug_data->base_naviframe, EEXT_CALLBACK_BACK,
		eext_naviframe_back_cb, NULL);

	ug_data->base_navi_it = elm_naviframe_item_push(ug_data->base_naviframe,
		IDS_NFC_NFC, l_button, NULL, nfc_setting_list, NULL);

	elm_naviframe_item_pop_cb_set(ug_data->base_navi_it, __back_clicked_cb, ug_data);

	LOGD("END >>>>");

	return ug_data->base_layout;
}

static void __ug_nfc_destroy(ui_gadget_h ug, app_control_h service, void *priv)
{
	ugdata_t *ug_data = (ugdata_t *)priv;

	LOGD("BEGIN >>>>");

	if ((ug_data == NULL) || (ug == NULL))
		return;

	if (nfc_manager_deinitialize() != NFC_ERROR_NONE)
		LOGE("NFC deinitialize failed");

	evas_object_del(ug_get_layout(ug));

	LOGD("END >>>>");
}

static void __ug_nfc_start(ui_gadget_h ug, app_control_h service, void *priv)
{
	LOGD("BEGIN >>>>");

	if (nfc_manager_is_supported() == false) {
		LOGE("NFC not supported");
		/* popup */
	}

	if (nfc_manager_initialize() != NFC_ERROR_NONE)
		LOGE("NFC initialize failed");

	LOGD("END >>>>");
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	ugdata_t *ug_data;

	LOGD("UG_MODULE_INIT!!");

	if (!ops)
		return -1;

	ug_data = (ugdata_t *)g_malloc0((gint)sizeof(ugdata_t));
	if (!ug_data)
		return -1;

	ops->create = __ug_nfc_create;
	ops->start = __ug_nfc_start;
	ops->destroy = __ug_nfc_destroy;
	ops->priv = ug_data;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	ugdata_t *ug_data;

	LOGD("UG_MODULE_EXIT!!");

	if (!ops)
		return;

	ug_data = ops->priv;

	if (ug_data)
		free(ug_data);

	ops->priv = NULL;
}
