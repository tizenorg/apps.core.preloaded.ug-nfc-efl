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


#ifndef __NFC_SETTING_APP_STRING_H__
#define __NFC_SETTING_APP_STRING_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Buffer length for Text */
#define POPUP_TEXT_SIZE	1024
#define LABEL_TEXT_SIZE	1024

#define _(s)   gettext(s)

/////////////////////////////////////////////////////////////////////////////
// NFC SETTING START ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/* Registered string in STMS NFC */
#define IDS_NFC_NFC	\
	_("WDS_ST_MBODY_NFC_ABB")
#define IDS_TAP_AND_PAY	\
	_("WDS_ST_MBODY_TAP_AND_PAY_ABB")
#define IDS_NO_INSTALLED_PAYMENT_APPS	\
	_("WDS_ST_NPBODY_NO_INSTALLED_PAYMENT_APPS_ABB")
#define IDS_UNABLE_TO_SELECT_PAYMENT_APP_WITH_MORE_THAN_50_REGISTERED_CARDS	\
	_("WDS_ST_POP_UNABLE_TO_SELECT_PAYMENT_APP_WITH_MORE_THAN_50_REGISTERED_CARDS")
#define IDS_ONE_OF_THE_CARDS_REGISTERED_IN_P1SS_WAS_ALREADY_REGISTERED_IN_P2SS_THIS_CARD_CANNOT_BE_USED_IN_P3SS	\
	_("WDS_ST_POP_ONE_OF_THE_CARDS_REGISTERED_IN_P1SS_WAS_ALREADY_REGISTERED_IN_P2SS_THIS_CARD_CANNOT_BE_USED_IN_P3SS")
#define IDS_SECURITY_POLICY_PREVENTS_USE_OF_NFC_ABB	\
	_("WDS_ST_TPOP_SECURITY_POLICY_PREVENTS_USE_OF_NFC_ABB")

#ifdef __cplusplus
}
#endif
#endif /*__NFC_SETTING_APP_STRING_H__*/
