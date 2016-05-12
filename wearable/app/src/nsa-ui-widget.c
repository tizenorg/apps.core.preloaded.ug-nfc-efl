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
#include "nsa-main.h"
#include "nsa-ui-widget.h"
#include "nsa-debug.h"
#include "nsa-util.h"


Evas_Object *nsa_create_main_layout(Evas_Object *parent)
{
	Evas_Object *layout;

	retv_if(parent == NULL, NULL);

	NSA_BEGIN();

	layout = elm_layout_add(parent);
	retv_if(layout == NULL, NULL);

	elm_object_focus_set(layout, EINA_FALSE);
	elm_layout_theme_set(layout, "layout", "application", "default");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);

	evas_object_show(layout);

	NSA_END();

	return layout;
}

Evas_Object *nsa_create_theme_layout(Evas_Object *parent,
	const char *clas, const char *group, const char *style)
{
	Evas_Object *layout;

	retv_if(parent == NULL, NULL);

	NSA_BEGIN();

	layout = elm_layout_add(parent);
	retv_if(layout == NULL, NULL);

	elm_object_focus_set(layout, EINA_FALSE);
	elm_layout_theme_set(layout, clas, group, style);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);

	evas_object_show(layout);

	NSA_END();

	return layout;
}

Evas_Object *nsa_create_bg(Evas_Object *parent)
{
	Evas_Object *bg;
#if defined(PROCESS_POOL)
	Evas_Object *win;
#endif

	retv_if(parent == NULL, NULL);

	NSA_BEGIN();

#if defined(PROCESS_POOL)
	win = (Evas_Object *)app_get_preinitialized_window(PACKAGE);
	if (win == parent) {
		bg = app_get_preinitialized_background();
		if (bg == NULL) {
			NSA_DEBUG_ERR("get bg fail");
			bg = elm_bg_add(parent);
		}
	} else {
		NSA_DEBUG_ERR("win is different");
		bg = elm_bg_add(parent);
	}
#else
	bg = elm_bg_add(parent);
#endif
	retv_if(bg == NULL, NULL);

	evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
	elm_win_resize_object_add(parent, bg);

	evas_object_show(bg);

	NSA_END();

	return bg;
}

Evas_Object *nsa_create_conformant(Evas_Object *parent)
{
	Evas_Object *conform;
#if defined(PROCESS_POOL)
	Evas_Object *win;
#endif

	retv_if(parent == NULL, NULL);

	NSA_BEGIN();

#if defined(PROCESS_POOL)
	win = (Evas_Object *)app_get_preinitialized_window(PACKAGE);
	if (win == parent) {
		conform = (Evas_Object *)app_get_preinitialized_conformant();
		if (conform == NULL) {
			NSA_DEBUG_ERR("get conform fail");
			conform = elm_conformant_add(parent);
		}
	} else {
		NSA_DEBUG_ERR("win is different");
		conform = elm_conformant_add(parent);
	}
#else
	conform = elm_conformant_add(parent);
#endif
	retv_if(conform == NULL, NULL);

	evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
	elm_win_resize_object_add(parent, conform);

	evas_object_show(conform);

	NSA_END();

	return conform;
}

Evas_Object* nsa_create_edj_layout(Evas_Object* parent,
	char *edc_path)
{
	Evas_Object *layout;

	NSA_BEGIN();

	retv_if(parent == NULL, NULL);

	layout = elm_layout_add(parent);
	retv_if(layout == NULL, NULL);

	elm_object_focus_set(layout, EINA_FALSE);

	if(edc_path != NULL)
		elm_layout_file_set(layout, EDJ_FILE, edc_path);

	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);

	evas_object_show(layout);

	NSA_END();

	return layout;
}


