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


#ifndef __NFC_SETTING_APP_VIEW_H__
#define __NFC_SETTING_APP_VIEW_H__

#include <Evas.h>
#include <Ecore.h>
#include <Elementary.h>
#include <efl_extension.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

int _nsa_view_main_create(void *data);
void _nsa_view_main_destroy(void *data);

int _nsa_view_tap_n_pay_create(void *data);
void _nsa_view_tap_n_pay_destroy(void *data);

#ifdef __cplusplus
}
#endif
#endif /*__NFC_SETTING_APP_VIEW_H__*/

