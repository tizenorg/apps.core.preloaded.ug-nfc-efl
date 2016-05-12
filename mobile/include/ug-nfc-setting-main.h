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


#ifndef __UG_NFC_SETTING_MAIN_H__
#define __UG_NFC_SETTING_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>
#include <ui-gadget-module.h>
#include <Evas.h>
#include <Ecore.h>
#include <Elementary.h>
#include <bundle.h>
#include <dlog.h>
#include <vconf.h>
#include <nfc.h>
#include <glib.h>

#define LOCALE_PATH	LOCALEDIR
#define ICON_PATH	ICONDIR
#define EDJ_PATH	EDJDIR
#define EDJ_FILE	EDJ_PATH"/"PACKAGE".edj"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "UG_NFC_SETTING"

#define NFCUG_TEXT_DOMAIN	PACKAGE
#define NFCUG_LOCALEDIR	LOCALE_PATH

/* Registered string in STMS NFC */
#define IDS_NFC_NFC	\
	dgettext(PACKAGE, "IDS_NFC_BODY_NFC")
#define IDS_NFC_DESCRIPTION_MSG	\
	dgettext(PACKAGE, "IDS_NFC_BODY_WHEN_NFC_IS_ON_YOU_CAN_SEND_OR_RECEIVE_DATA_WHEN_YOUR_DEVICE_TOUCHES_OTHER_NFC_CAPABLE_DEVICES_OR_NFC_TAGS")

typedef struct {
	ui_gadget_h nfc_setting_ug;

	Evas_Object* ug_win_main;
	Evas_Object* base_layout;
	Evas_Object *bg;
	Evas_Object* base_naviframe;
	Evas_Object* ns_on_off;
	Evas_Object* popup;

	Elm_Object_Item *base_navi_it;

	app_control_h service;
} ugdata_t;

#ifdef __cplusplus
}
#endif

#endif /*__UG_NFC_SETTING_MAIN_H__*/
