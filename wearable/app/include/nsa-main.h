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

typedef enum {
	NSA_ERROR = -1,
	NSA_OK = 0
} nsa_result_e;

typedef enum {
	NSA_OP_NONE = -1,
	NSA_OP_NFC = 0,
	NSA_OP_NFC_HELP = 1,
	NSA_OP_SBEAM = 2,
	NSA_OP_SBEAM_HELP = 3,
	NSA_OP_TAP_N_PAY = 4
} nsa_operation_type;

struct appdata {
	Evas_Object *win_main;
	Evas_Object *ly_main;
	Evas_Object *bg;
	Evas_Object *conform;
	Eext_Circle_Surface *circle_surface;
	Evas_Object *navi_frame;
	Evas_Object *popup;
	Elm_Object_Item *base_navi_it;
	GArray *circle_obj_list;
	int circle_obj_list_idx;

	nsa_operation_type op_type;

	/* changeable UI */
	Eina_List *color_table;
	Eina_List *font_table;

	app_control_h caller_appctrl;
};

inline Evas_Object *nsa_get_main_window(void);

#ifdef __cplusplus
}
#endif
#endif /*__NFC_SETTING_APP_MAIN_H__*/
