/**
 * @file
 * SylixOS multi-input device support kernel module test program.
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
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <keyboard.h>
#include <mouse.h>
#include <kmodule/xinput/xinput.h>

int main (int argc, char **argv)
{
    int                      mouse_fd;
    int                      keyboard_fd;
    int                      max_fd;
    int                      cnt;
    int                      x = 0, y = 0;
    fd_set                   rfds;
    struct timeval           tv;
    char                     devpath[PATH_MAX];
    xinput_device_info_arg_t info;

    keyboard_fd = open("/dev/input/xkbd", O_RDONLY, 0666);
    if (keyboard_fd < 0) {
        fprintf(stderr, "Failed to open /dev/input/xkbd\n");
        return -1;
    }

    mouse_fd = open("/dev/input/xmse", O_RDONLY, 0666);
    if (mouse_fd < 0) {
        fprintf(stderr, "Failed to open /dev/input/xmse\n");
        return -1;
    }

    /*
     * Rewind keyboard devices read pointer
     */
    ioctl(keyboard_fd, XINPUT_CMD_REWIND_DEV, NULL);

    /*
     * Read all keyboard devices(which in KEYBOARD envi and have been plugin
     */
    while (1) {
        if (ioctl(keyboard_fd, XINPUT_CMD_GET_DEV, devpath) < 0) {
            break;
        }

        if (devpath[0] == '\0') {
            /*
             * end
             */
            break;
        }

        printf("Found keyboard %s\n", devpath);

        /*
         * Get keyboard device info and print it
         */
        info.dev = devpath;
        if (ioctl(keyboard_fd, XINPUT_CMD_GET_DEV_INFO, &info) == 0) {
            printf("\tvendor:  %s\n", info.vendor);
            printf("\tproduct: %s\n", info.product);
            printf("\tserial:  %s\n", info.serial);
        }
    }

    /*
     * Clean listened keyboard device list
     */
    ioctl(keyboard_fd, XINPUT_CMD_CLEAR_DEVS, NULL);

    /*
     * Add /dev/input/kbd0 to listened keyboard device list
     */
    ioctl(keyboard_fd, XINPUT_CMD_ADD_DEV, "/dev/input/kbd0");

    /*
     * Rewind mouse devices read pointer
     */
    ioctl(mouse_fd, XINPUT_CMD_REWIND_DEV, NULL);

    /*
     * Read all mouse devices(which in KEYBOARD envi and have been plugin
     */
    while (1) {
        if (ioctl(mouse_fd, XINPUT_CMD_GET_DEV, devpath) < 0) {
            break;
        }

        if (devpath[0] == '\0') {
            /*
             * end
             */
            break;
        }

        printf("Found mouse %s\n", devpath);

        /*
         * Get mouse device info and print it
         */
        info.dev = devpath;
        if (ioctl(mouse_fd, XINPUT_CMD_GET_DEV_INFO, &info) == 0) {
            printf("\tvendor:  %s\n", info.vendor);
            printf("\tproduct: %s\n", info.product);
            printf("\tserial:  %s\n", info.serial);
        }
    }

    /*
     * Clean listened mouse device list
     */
    ioctl(mouse_fd, XINPUT_CMD_CLEAR_DEVS, NULL);

    /*
     * Add /dev/input/mse0 to listened mouse device list
     */
    ioctl(mouse_fd, XINPUT_CMD_ADD_DEV, "/dev/input/mse0");

    /*
     * Turn off all listened keyboard leds
     */
    printf("Turn off all keyboard leds...\n");

    ioctl(keyboard_fd, SIO_KYBD_LED_SET, 0);

    sleep(3);

    /*
     * Turn on all listened keyboard leds
     */
    printf("Turn on all keyboard leds...\n");

    ioctl(keyboard_fd, SIO_KYBD_LED_SET, SIO_KYBD_LED_NUM | SIO_KYBD_LED_CAP | SIO_KYBD_LED_SCR);

    /*
     * Read keyboard and mouse event
     */
    printf("Keyboard and mouse input test...\n");

    FD_ZERO(&rfds);

    if (mouse_fd > keyboard_fd) {
        max_fd = mouse_fd + 1;
    } else {
        max_fd = keyboard_fd + 1;
    }

    while (1) {
        FD_SET(mouse_fd, &rfds);
        FD_SET(keyboard_fd, &rfds);

        tv.tv_sec  = 60;
        tv.tv_usec = 0;

        cnt = select(max_fd, &rfds, NULL, NULL, &tv);
        if (cnt < 0) {
            printf("Select failed, exit...\n");
            break;

        } else if (cnt == 0) {
            printf("Timeout, exit...\n");
            break;

        } else if (cnt > 0) {
            if (FD_ISSET(mouse_fd, &rfds)) {
                mouse_event_notify notify[64];
                ssize_t            len;
                int                i;

                len = read(mouse_fd, notify, sizeof(notify));
                if (len > 0) {
                    len /= sizeof(notify[0]);
                    for (i = 0; i < len; i++) {
                        if (notify[i].ctype == MOUSE_CTYPE_REL) {
                            x += notify[i].xmovement;
                            y += notify[i].ymovement;
                            if (x < 0) {
                                x = 0;
                            }
                            if (x > 1920) {
                                x = 1920;
                            }
                            if (y < 0) {
                                y = 0;
                            }
                            if (y > 1080) {
                                y = 1080;
                            }
                        } else {
                            x = notify[i].xmovement;
                            y = notify[i].ymovement;
                        }

                        printf("mouse %d %d\n", x, y);
                    }
                }
            }

            if (FD_ISSET(keyboard_fd, &rfds)) {
                keyboard_event_notify notify[32];
                ssize_t               len;
                int                   i;

                len = read(keyboard_fd, notify, sizeof(notify));
                if (len > 0) {
                    len /= sizeof(notify[0]);
                    for (i = 0; i < len; i++) {
                        if (notify[i].type == KE_PRESS) {
                            printf("keyboard pressed\n");
                        } else {
                            printf("keyboard release\n");
                        }
                    }
                }
            }
        }
    }

    close(mouse_fd);
    close(keyboard_fd);

    return  (0);
}

/*
 * end
 */
