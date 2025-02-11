/*******************************************************************************
* @file  rsi_wlan_power_save_profile.c
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
 * Include files
 * */

//! include file to refer data types
#include "rsi_data_types.h"

//! COMMON include file to refer wlan APIs
#include "rsi_common_apis.h"

//! WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"
#include "rsi_wlan_non_rom.h"

//! socket include file to refer socket APIs
#include "rsi_socket.h"

#include "rsi_bootup_config.h"
//! Error include files
#include "rsi_error.h"

#include "rsi_wlan.h"

#include "rsi_wlan_config.h"
//! OS include file to refer OS specific functionality
#include "rsi_os.h"
#include "rsi_utils.h"
#include <string.h>
//! Access point SSID to connect
#define SSID "SILABS_AP"

//! Security type
#define SECURITY_TYPE RSI_WPA2

//! Password
#define PSK "1234567890"

//! DHCP mode 1- Enable 0- Disable
#define DHCP_MODE 1

//! If DHCP mode is disabled given IP statically
#if !(DHCP_MODE)

//! IP address of the module
//! E.g: 0x650AA8C0 == 192.168.10.101
#define DEVICE_IP 0x650AA8C0

//! IP address of Gateway
//! E.g: 0x010AA8C0 == 192.168.10.1
#define GATEWAY 0x010AA8C0

//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define NETMASK 0x00FFFFFF

#endif

//! Server port number
#define SERVER_PORT 5001

//! Server IP address. Should be in reverse long format
//! E.g: 0x640AA8C0 == 192.168.10.100
#define SERVER_IP_ADDRESS 0x6700A8C0

//! Number of packet to send or receive
#define NUMBER_OF_PACKETS 1000

//! Power Save Profile Mode
#define PSP_TYPE RSI_MAX_PSP

//! Runs scheduler for some delay and do not send any command
#define RSI_DELAY 6000000

//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000

//! Wlan task priority
#define RSI_WLAN_TASK_PRIORITY 1

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

//! Wlan task stack size
#define RSI_WLAN_TASK_STACK_SIZE 500

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 500

//! set sleep timer  enable or disable
#define SET_SLEEP_TIME RSI_DISABLE

//! Sleep Timer value in seconds,Minimum value is 1sec, and maximum value is 2100seconds.
#define SLEEP_TIME 30

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];

int32_t rsi_powersave_profile_app()
{
  int32_t client_socket;
  struct rsi_sockaddr_in server_addr;
  int32_t status       = RSI_SUCCESS;
  int32_t packet_count = 0;
  uint32_t delay;
#if !(DHCP_MODE)
  uint32_t ip_addr      = DEVICE_IP;
  uint32_t network_mask = NETMASK;
  uint32_t gateway      = GATEWAY;
#else
  uint8_t dhcp_mode = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif

  //! WC initialization
  status = rsi_wireless_init(0, 0);
  if (status != RSI_SUCCESS) {
    return status;
  }

  while (1) {
    //! Wlan radio init
    status = rsi_wlan_radio_init();
    if (status != RSI_SUCCESS) {

      //! Return the status if error in sending command occurs
      return status;
    }

#if ((SET_SLEEP_TIME) && (RSI_HAND_SHAKE_TYPE == MSG_BASED))
    status = rsi_wlan_set_sleep_timer(SLEEP_TIME);
    if (status != RSI_SUCCESS) {
      return status;
    }
#endif
    //! Apply power save profile with deep sleep
    status = rsi_wlan_power_save_profile(RSI_SLEEP_MODE_8, PSP_TYPE);
    if (status != RSI_SUCCESS) {
      return status;
    }

    //! wait in scheduler for some time
    for (delay = 0; delay < RSI_DELAY; delay++) {
#ifndef RSI_WITH_OS
      rsi_wireless_driver_task();
#endif
    }

    //! Disable power save profile
    status = rsi_wlan_power_save_profile(RSI_ACTIVE, PSP_TYPE);
    if (status != RSI_SUCCESS) {
      return status;
    }

    //! Connect to an Acces point
    status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
    if (status != RSI_SUCCESS) {
      return status;
    }

    //! Configure IP
#if DHCP_MODE
    status = rsi_config_ipaddress(RSI_IP_VERSION_4, dhcp_mode, 0, 0, 0, NULL, 0, 0);
#else
    status = rsi_config_ipaddress(RSI_IP_VERSION_4,
                                  RSI_STATIC,
                                  (uint8_t *)&ip_addr,
                                  (uint8_t *)&network_mask,
                                  (uint8_t *)&gateway,
                                  NULL,
                                  0,
                                  0);
#endif

    //! Create socket
    client_socket = rsi_socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
      status = rsi_wlan_get_status();
      return status;
    }

    //! Set server structure
    memset(&server_addr, 0, sizeof(server_addr));

    //! Set server address family
    server_addr.sin_family = AF_INET;

    //! Set server port number, using htons function to use proper byte order
    server_addr.sin_port = htons(SERVER_PORT);

    //! Set IP address to localhost
    server_addr.sin_addr.s_addr = SERVER_IP_ADDRESS;

    while (packet_count < NUMBER_OF_PACKETS) {
      //! Send data on socket
      status = rsi_sendto(client_socket,
                          (int8_t *)"Hello from UDP client!!!",
                          (sizeof("Hello from UDP client!!!") - 1),
                          0,
                          (struct rsi_sockaddr *)&server_addr,
                          sizeof(server_addr));
      if (status < 0) {
        status = rsi_wlan_get_status();
        return status;
      }

      //! Increase packet count
      packet_count++;
    }

    //! Disconnect from AP
    rsi_wlan_disconnect();

    //! setting packet count to 0 to send data again
    packet_count = 0;
  }
}

void app_task()
{
  ////////////////////////
  //! Application code ///
  ////////////////////////
  return;
}

void main_loop(void)
{
  while (1) {
    //! Application task
    app_task();

    //! event loop
    rsi_wireless_driver_task();
  }
}

int main()
{
  int32_t status;

#ifdef RSI_WITH_OS

  rsi_task_handle_t wlan_task_handle = NULL;

  rsi_task_handle_t driver_task_handle = NULL;
#endif

  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  //! Redpine module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    return status;
  }

#ifdef RSI_WITH_OS
  //! OS case
  //! Task created for WLAN task
  rsi_task_create((rsi_task_function_t)rsi_powersave_profile_app,
                  (uint8_t *)"wlan_task",
                  RSI_WLAN_TASK_STACK_SIZE,
                  NULL,
                  RSI_WLAN_TASK_PRIORITY,
                  &wlan_task_handle);

  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  //! NON - OS case
  //! CallPower save application
  status = rsi_powersave_profile_app();

  //! Application main loop
  main_loop();
#endif
  return status;
}
