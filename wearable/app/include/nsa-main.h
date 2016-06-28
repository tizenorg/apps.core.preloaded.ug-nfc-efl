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


#ifndef __NFC_SETTING_APP_MAIN_H__
#define __NFC_SETTING_APP_MAIN_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <Elementary.h>
#include <efl_extension.h>
#include <glib.h>
#include <app.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Package */
#define APPNAME	PACKAGE
#define GRP_MAIN	"main"
#define ICON_PATH	RESDIR"/icons"
#define IMAGES_PATH	IMGDIR
#define TABLE_PATH	TABLEDIR
#define EDJ_FILE	EDJDIR"/"APPNAME".edj"

enum {
	V_MAIN = 0,
	V_TAP_N_PAY
};

typedef struct {
	char *appid;
	int aid_count;
} wallet_info_t;

typedef struct {
	Evas_Object *main_win;
	Evas_Object *main_layout;
	Evas_Object *main_bg;
	Evas_Object *conform;
	Eext_Circle_Surface *circle_surface;

	Evas_Object *popup;
	Evas_Object *navi_frame;
	int current_view;

	char *payment_handler;
	Elm_Object_Item *on_off;
	Elm_Object_Item *tap_n_pay;
	Evas_Object *main_genlist;

	int payment_wallet_cnt;
	GArray *payment_wallet_list;
	Evas_Object *radio_main;
} appdata;

#ifdef __cplusplus
}
#endif
#endif /*__NFC_SETTING_APP_MAIN_H__*/
