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

static void __back_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata *ad = data;
	NSA_DEBUG_ERR("back click cb : view depth is %d", ad->current_view);

	elm_naviframe_item_pop(ad->navi_frame);

	if (ad->current_view == V_TAP_N_PAY) {
		Evas_Object *circle_obj;

		/* release Tap & Pay view */
		if (ad->payment_wallet_list != NULL) {
			int i;
			NSA_DEBUG_ERR("wallet count is %d", ad->payment_wallet_cnt);
			for (i = 0; i < ad->payment_wallet_cnt; i++) {
				wallet_info_t *winfo = NULL;

				winfo = g_array_index(ad->payment_wallet_list, wallet_info_t *, i);
				NSA_MEM_FREE(winfo->appid);
				NSA_MEM_FREE(winfo);
			}

			g_array_free(ad->payment_wallet_list, FALSE);
			ad->payment_wallet_list = NULL;
		}

		ad->payment_wallet_cnt = 0;
		ad->radio_main = NULL;
		ad->current_view = V_MAIN;

		circle_obj = eext_circle_object_genlist_add(ad->main_genlist, ad->circle_surface);
		eext_circle_object_genlist_scroller_policy_set(circle_obj,
			ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
		eext_rotary_object_event_activated_set(circle_obj, EINA_TRUE);
	} else if (ad->current_view == V_MAIN) {
		/* release Main view and exit application */
		NSA_MEM_FREE(ad->payment_handler);

		ui_app_exit();
	}

	return;
}

bool _nsa_main_app_create(void *user_data)
{
	appdata *ad = user_data;

	NSA_DEBUG("_nsa_main_app_create start");

	retv_if(ad == NULL, false);

	bindtextdomain("setting-nfc-efl", "/usr/apps/org.tizen.nfc-setting-app/res/locale");
	textdomain("setting-nfc-efl");

	if (nfc_manager_initialize() != NFC_ERROR_NONE)
		NSA_DEBUG_ERR("nfc_manager_initialize failed");

	/* create window */
	ad->main_win = nsa_create_main_win(APPNAME);
	retv_if(ad->main_win == NULL, false);

	/* create conformant */
	ad->conform = nsa_create_conformant(ad->main_win);
	retv_if(ad->conform == NULL, false);

	/* create circle surface */
	ad->circle_surface = eext_circle_surface_conformant_add(ad->conform);
	retv_if(ad->circle_surface == NULL, false);

	/* create bg */
	ad->main_bg = nsa_create_bg(ad->conform);
	retv_if(ad->main_bg == NULL, false);

	/* create base layout */
	ad->main_layout = nsa_create_main_layout(ad->conform);
	retv_if(ad->main_layout == NULL, false);

	/* create naviframe */
	ad->navi_frame = elm_naviframe_add(ad->main_layout);
	retv_if(ad->navi_frame == NULL, false);

	elm_object_part_content_set(ad->main_layout, "elm.swallow.content", ad->navi_frame);
	evas_object_show(ad->navi_frame);
	eext_object_event_callback_add(ad->navi_frame, EEXT_CALLBACK_BACK, __back_clicked_cb, ad);

	NSA_DEBUG("_nsa_main_app_create end");

	return true;
}

void _nsa_main_app_terminate(void *user_data)
{
	appdata *ad = user_data;

	NSA_DEBUG("_nsa_main_app_terminate start");

	ret_if(ad == NULL);

	if (nfc_manager_deinitialize() != NFC_ERROR_NONE)
		NSA_DEBUG_ERR("nfc_manager_deinitialize failed");

	NSA_DEBUG("_nsa_main_app_terminate end");
}

void _nsa_main_app_service(app_control_h service, void *user_data)
{
	appdata *ad = user_data;

	NSA_DEBUG("_nsa_main_app_service start");

	ret_if(ad == NULL);

	_nsa_view_main_create(ad);
	elm_win_activate(ad->main_win);

	NSA_DEBUG("_nsa_main_app_service end");
}

int main(int argc, char* argv[])
{
	appdata ad;
	ui_app_lifecycle_callback_s event_callback = {0,};

	NSA_DEBUG("app start");

	event_callback.create = _nsa_main_app_create;
	event_callback.terminate = _nsa_main_app_terminate;
	event_callback.app_control = _nsa_main_app_service;

	NSA_DEBUG("app end");

	memset(&ad, 0x0, sizeof(appdata));

	return ui_app_main(argc, argv, &event_callback, &ad);
}
