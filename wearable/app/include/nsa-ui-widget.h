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


#ifndef __NFC_SETTING_APP_UI_WIDGET_H__
#define __NFC_SETTING_APP_UI_WIDGET_H__

#include <stdbool.h>
#include <Elementary.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/************************** API **************************/
Evas_Object* nsa_create_main_win(const char *name);
Evas_Object *nsa_create_main_layout(Evas_Object *parent);
Evas_Object *nsa_create_bg(Evas_Object *parent);
Evas_Object* nsa_create_conformant(Evas_Object *parent);
Evas_Object* nsa_create_edj_layout(Evas_Object* parent, char *edc_path);

#ifdef __cplusplus
}
#endif
#endif /* __NFC_SETTING_APP_UI_WIDGET_H__ */
