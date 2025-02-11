/*******************************************************************************
* @file  rsi_bt_config.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/
/**
 * @file     rsi_bt_config.h
 * @version  0.1  
 * @date     19 Mar 2021
 *
 *
 *
 *  @brief : This file contain definitions and declarations of BT APIs.
 *
 *  @section Description  This file contains definitions and declarations required to
 *  configure BT module.
 *
 *
 */

#ifndef RSI_BT_CONFIG_H
#define RSI_BT_CONFIG_H

#ifdef FRDM_K28F
#include "rsi_common_app.h"
#endif
#include "rsi_bt_common.h"
#include <rsi_data_types.h>
/***********************************************************************************************************************************************/
//! BT_A2DP_SOURCE APP CONFIG defines
/***********************************************************************************************************************************************/
#define RSI_BT_REMOTE_BD_ADDR (void *)"EA:D1:01:01:82:4C"
#define SPP_SLAVE             0
#define SPP_MASTER            1
#define SPP_MODE              SPP_SLAVE //! type of Module mode
#if (SPP_MODE == SPP_MASTER)
#define RSI_BT_LOCAL_NAME (void *)"SPP_MASTER"
#else
#define RSI_BT_LOCAL_NAME (void *)"SPP_SLAVE" //! Module name
#endif
#define PIN_CODE "0000"

/***********************************************************************************************************************************************/
//! application events list
/***********************************************************************************************************************************************/
#define RSI_APP_EVENT_CONNECTED    1
#define RSI_APP_EVENT_PINCODE_REQ  2
#define RSI_APP_EVENT_LINKKEY_SAVE 3
#define RSI_APP_EVENT_AUTH_COMPLT  4
#define RSI_APP_EVENT_DISCONNECTED 5
#define RSI_APP_EVENT_LINKKEY_REQ  6
#define RSI_APP_EVENT_SPP_CONN     7
#define RSI_APP_EVENT_SPP_DISCONN  8
#define RSI_APP_EVENT_SPP_RX       9
/** ssp related defines********/
#define RSI_APP_EVENT_PASSKEY_DISPLAY 10
#define RSI_APP_EVENT_PASSKEY_REQUEST 11
#define RSI_APP_EVENT_SSP_COMPLETE    12
#define RSI_APP_EVENT_CONFIRM_REQUEST 13

#include <rsi_bt_common_config.h>
#endif
