/**
 * @file
 * SylixOS multi-input device support kernel module.
 */

/*
 * Copyright (c) 2006-2022 SylixOS Group.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 4. This code has been or is applying for intellectual property protection
 *    and can only be used with acoinfo software products.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * Author: Jiao.jinxing <jiaojinxing@acoinfo.com>
 *
 */
#ifndef __XINPUT_H
#define __XINPUT_H

#define XINPUT_DEVICE_INFO_STR_SZ       64

/*
 * XINPUT_CMD_GET_KBD_LED and XINPUT_CMD_SET_KBD_LED command argument
 */
typedef struct {
    const char  *dev;
    int         led;
} xinput_led_arg_t;

/*
 * XINPUT_CMD_GET_DEV_INFO command argument
 */
typedef struct {
    const char  *dev;
    char        vendor[XINPUT_DEVICE_INFO_STR_SZ];
    char        product[XINPUT_DEVICE_INFO_STR_SZ];
    char        serial[XINPUT_DEVICE_INFO_STR_SZ];
} xinput_device_info_arg_t;

/*
 * Rewind current get plugged-in device pointer
 */
#define XINPUT_CMD_REWIND_DEV   LW_OSIO('x',  1000)

/*
 * Get the plugged-in device from current pointer
 */
#define XINPUT_CMD_GET_DEV      LW_OSIOR('x', 1001, char *)

/*
 * Get the device info of specified device
 */
#define XINPUT_CMD_GET_DEV_INFO LW_OSIOR('x', 1002, xinput_device_info_arg_t)

/*
 * Clear listen device list
 */
#define XINPUT_CMD_CLEAR_DEVS   LW_OSIO('x',  1003)

/*
 * Add device to listen list
 */
#define XINPUT_CMD_ADD_DEV      LW_OSIOW('x', 1004, char *)

/*
 * Remove device from listen list
 */
#define XINPUT_CMD_DEL_DEV      LW_OSIOW('x', 1005, char *)

/*
 * Get the led state of specified keyboard
 */
#define XINPUT_CMD_GET_KBD_LED  LW_OSIOR('x', 1006, xinput_led_arg_t)

/*
 * Set the led state of specified keyboard
 */
#define XINPUT_CMD_SET_KBD_LED  LW_OSIOW('x', 1007, xinput_led_arg_t)

#endif /* __XINPUT_H */

/*
 * end
 */
