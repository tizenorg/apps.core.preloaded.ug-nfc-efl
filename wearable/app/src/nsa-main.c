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


#include <nfc.h>
#include <vconf.h>
#include "nsa-main.h"
#include "nsa-view.h"
#include "nsa-debug.h"
#include "nsa-util.h"
#include "nsa-ui-widget.h"
#include "nsa-popup.h"

#define UG_NFC_APPCONTROL_TIZEN	\
	"http://tizen.org/appcontrol/operation/setting/nfc"
#define SETTING_TYPE_NFC	"nfc"
#define SETTING_TYPE_TAP_N_PAY	"tap_n_pay"

#define NFC_SETTING_APP_ID	\
	"org.tizen.nfc-setting-app"

static struct appdata ad;


static void __tts_vconf_key_changed (keynode_t *key,
	void *data)
{
	int boolval = 0;

	NSA_DEBUG("__tts_vconf_key_changed");


	if(VCONF_OK != vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS,
		&boolval)) {
		NSA_DEBUG_ERR("vconf_get_bool failed");
	} else {
		NSA_DEBUG("boolval[%d]", boolval);

		if (boolval)
			elm_exit();
	}
}

static void __win_del(void *data, Evas_Object *obj, void *event)
{
	elm_exit();
}

static Evas_Object* __nsa_main_create_win(const char *name)
{
	Evas_Object *eo;
	int w, h;

	retv_if(name == NULL, NULL);

	NSA_BEGIN();

#if defined(PROCESS_POOL)
	eo = (Evas_Object *)app_get_preinitialized_window(name);
	if (eo == NULL) {
		NSA_DEBUG_ERR("get window fail");
		eo = elm_win_add(NULL, name, ELM_WIN_BASIC);
	}
#else
	eo = elm_win_add(NULL, name, ELM_WIN_BASIC);
#endif
	retv_if(eo == NULL, NULL);

	if (eo) {
		elm_win_title_set(eo, name);
		evas_object_smart_callback_add(eo, "delete,request", __win_del,
			NULL);
		elm_win_screen_size_get(eo, NULL, NULL, &w, &h);
		evas_object_resize(eo, w, h);
	}

	NSA_END();
	return eo;
}

static int __parse_service(app_control_h service, void *data)
{
	struct appdata *ad = (struct appdata *)data;
	char *operation = NULL;
	char *app_id = NULL;
	char *ex_data = NULL;

	retv_if(ad == NULL, NSA_ERROR);

	app_control_get_operation(service, &operation);
	app_control_get_app_id(service, &app_id);
	app_control_get_extra_data(service, "type", &ex_data);

	NSA_DEBUG("operation [%s] >>>>> ", GET_SAFE_STRING(operation));
	NSA_DEBUG("app_id [%s] >>>>> ", GET_SAFE_STRING(app_id));
	NSA_DEBUG("extra data [%s] >>>>> ", GET_SAFE_STRING(ex_data));

	ad->op_type = NSA_OP_NONE;
	if (!ex_data) {
		NSA_DEBUG_ERR("extra data is NULL");
		goto end;
	}

	if (strncmp(operation, UG_NFC_APPCONTROL_TIZEN,
		strlen(UG_NFC_APPCONTROL_TIZEN)) == 0) {

		/* launched by app-control */
		if (strncmp(ex_data, SETTING_TYPE_NFC,
			strlen(SETTING_TYPE_NFC)) == 0) {
			ad->op_type = NSA_OP_NFC;
		} else if (strncmp(ex_data, SETTING_TYPE_TAP_N_PAY,
			strlen(SETTING_TYPE_TAP_N_PAY)) == 0) {
			ad->op_type = NSA_OP_TAP_N_PAY;
		}
	} else if (strncmp(operation, APP_CONTROL_OPERATION_DEFAULT,
		strlen(APP_CONTROL_OPERATION_DEFAULT)) == 0) {

		if (!app_id) {
			NSA_DEBUG_ERR("app_id is NULL");
			goto end;
		}

		if (strncmp(app_id, NFC_SETTING_APP_ID,
			strlen(NFC_SETTING_APP_ID)) != 0) {
			NSA_DEBUG_ERR("app_id is wrong");
			goto end;
		}
		if (strncmp(ex_data, SETTING_TYPE_NFC,
			strlen(SETTING_TYPE_NFC)) == 0) {
			ad->op_type = NSA_OP_NFC;
		} else if (strncmp(ex_data, SETTING_TYPE_TAP_N_PAY,
			strlen(SETTING_TYPE_TAP_N_PAY)) == 0) {
			ad->op_type = NSA_OP_TAP_N_PAY;
		}
	}

end:
	NSA_MEM_FREE(operation);
	NSA_MEM_FREE(app_id);
	NSA_MEM_FREE(ex_data);

	if (ad->op_type == NSA_OP_NONE)
		return NSA_ERROR;
	else
		return NSA_OK;
}

static int __launch_nfc_setting(void *data)
{
	if (NFC_ERROR_NONE != nfc_manager_initialize())
		NSA_DEBUG_ERR("nfc_manager_initialize FAIL!!!!");

	return _nsa_view_main_create(data);
}

static int __launch_tap_n_pay_setting(void *data)
{
	if (NFC_ERROR_NONE != nfc_manager_initialize())
		NSA_DEBUG_ERR("nfc_manager_initialize FAIL!!!!");

	return _nsa_view_tap_n_pay_create(data);
}


static void __rot_changed_cb(void *data, Evas_Object *obj, void *event)
{
	struct appdata *ad = (struct appdata *)data;
	int angle;

	ret_if(ad == NULL);

	angle = elm_win_rotation_get(ad->win_main);
	elm_win_rotation_with_resize_set(ad->win_main, angle);

	NSA_DEBUG("angle[%d]", angle);
}

static int __transient_app_cb(void *data)
{
	NSA_DEBUG("__transient_app_cb");

	return 0;
}

static void __reply_svc_requests(app_control_h src_svc)
{
	app_control_h service;
	int ret;

	app_control_create(&service);
	ret_if(service == NULL);

	ret = app_control_reply_to_launch_request(service, src_svc,
		APP_CONTROL_RESULT_SUCCEEDED);
	if (ret != APP_CONTROL_ERROR_NONE)
		NSA_DEBUG_ERR("app_control_reply_to_launch_request failed");

	app_control_destroy(service);
}

void _nsa_main_lang_changed(void *data)
{
	struct appdata *ad = (struct appdata *)data;
	ret_if(ad == NULL);

	NSA_BEGIN();

	/* To do */

	NSA_END();
}

void _nsa_main_region_changed(void* data)
{
	NSA_BEGIN();

	NSA_END();
}

int _nsa_main_keydown_cb(void *data, int type, void *event)
{
	struct appdata *ad = (struct appdata *)data;
	Ecore_Event_Key *ev = (Ecore_Event_Key *)event;

	retv_if(ad == NULL, NSA_ERROR);
	retv_if(ev == NULL, NSA_ERROR);

	NSA_BEGIN();

	NSA_END();

	return EXIT_FAILURE;
}

bool _nsa_main_app_create(void *user_data)
{
	struct appdata *ad = (struct appdata *)user_data;

	NSA_DEBUG("_nsa_main_app_create start");

	retv_if(ad == NULL, false);

	bindtextdomain("setting-nfc-efl", "/usr/apps/org.tizen.nfc-setting-app/res/locale");
	textdomain("setting-nfc-efl");

	/* set scale */
	elm_app_base_scale_set(1.3);

	/* create window */
	ad->win_main = __nsa_main_create_win(APPNAME);
	retv_if(ad->win_main == NULL, false);
	evas_object_show(ad->win_main);

	/* create conformant */
	ad->conform = nsa_create_conformant(ad->win_main);
	retv_if(ad->conform == NULL, false);
	ad->circle_surface = eext_circle_surface_conformant_add(ad->conform);

	/* create bg */
	ad->bg = nsa_create_bg(ad->conform);
	retv_if(ad->bg == NULL, false);

	/* create base layout */
	ad->ly_main = nsa_create_main_layout(ad->conform);
	retv_if(ad->ly_main == NULL, false);
	elm_object_content_set(ad->conform, ad->ly_main);
	elm_win_conformant_set(ad->win_main, EINA_TRUE);

	/* create naviframe */
	ad->navi_frame = elm_naviframe_add(ad->ly_main);
	retv_if(ad->navi_frame == NULL, false);
	elm_object_part_content_set(ad->ly_main, "elm.swallow.content",
			ad->navi_frame);
	evas_object_show(ad->navi_frame);

	/* enable rotation */
	if (elm_win_wm_rotation_supported_get(ad->win_main)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win_main,
			(const int *)&rots, 4);
		evas_object_smart_callback_add(ad->win_main,
			"wm,rotation,changed", __rot_changed_cb, ad);
	} else {
		NSA_DEBUG_ERR("elm_win_wm_rotation_supported_get failed");
	}

	/* create circle_obj_list */
	ad->circle_obj_list = g_array_new(FALSE, FALSE, sizeof(Evas_Object *));
	ad->circle_obj_list_idx = -1;

	NSA_DEBUG("_nsa_main_app_create end");

	return true;
}

void _nsa_main_app_terminate(void *user_data)
{
	struct appdata *ad = (struct appdata *)user_data;
	ret_if(ad == NULL);

	NSA_DEBUG("_nsa_main_app_terminate start");

	/* The request from setting team */
	__reply_svc_requests(ad->caller_appctrl);

	/* unset nfc client + */
	if (nfc_manager_deinitialize() != NFC_ERROR_NONE)
		NSA_DEBUG_ERR("nfc_manager_deinitialize failed");
	/* unset nfc client - */

	if (VCONF_OK != vconf_ignore_key_changed(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS,
		__tts_vconf_key_changed)) {
		NSA_DEBUG_ERR("vconf_ignore_key_changed error");
	}

	g_array_free(ad->circle_obj_list, FALSE);

	NSA_DEBUG("_nsa_main_app_terminate end");
}

void _nsa_main_app_pause(void *user_data)
{
	struct appdata *ad = (struct appdata *)user_data;
	ret_if(ad == NULL);

	NSA_DEBUG("_nsa_main_app_pause start");

	NSA_DEBUG("_nsa_main_app_pause end");
}

void _nsa_main_app_resume(void *user_data)
{
	struct appdata *ad = (struct appdata *)user_data;
	ret_if(ad == NULL);

	NSA_DEBUG("_nsa_main_app_resume start");

	NSA_DEBUG("_nsa_main_app_resume end");
}

void _nsa_main_app_service(app_control_h service, void *user_data)
{
	struct appdata *ad = (struct appdata *)user_data;
	int result = NSA_ERROR;

	NSA_DEBUG("_nsa_main_app_service start");

	ret_if(ad == NULL);

	/* parse service data */
	/*
	result = __parse_service(service, ad);
	if (result != NSA_OK) {
		NSA_DEBUG_ERR("__parse_service failed[%d]", result);

		elm_exit();
		return;
	}
	*/
	ad->op_type = NSA_OP_NFC;

	/* launch nss */
	if (ad->op_type == NSA_OP_NFC){
		result = __launch_nfc_setting(ad);
	} else {
		result = __launch_tap_n_pay_setting(ad);
	}
	if (result != NSA_OK) {
		NSA_DEBUG_ERR("__launch_nfc_setting failed[%d]", result);

		elm_exit();
		return;
	}

	elm_win_activate(ad->win_main);

	/* the service will be used when app is terminated */
	app_control_clone(&(ad->caller_appctrl), service);

	NSA_DEBUG("_nss_main_app_service end");
}

int main(int argc, char* argv[])
{
	NSA_DEBUG("main start");

	ui_app_lifecycle_callback_s event_callback = {0,};

	event_callback.create = _nsa_main_app_create;
	event_callback.terminate = _nsa_main_app_terminate;
	event_callback.pause = _nsa_main_app_pause;
	event_callback.resume = _nsa_main_app_resume;
	event_callback.app_control = _nsa_main_app_service;

	return ui_app_main(argc, argv, &event_callback, &ad);
}

inline Evas_Object *nsa_get_main_window(void)
{
	return ad.win_main;
}

