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
 * Author: Han.hui <sylixos@gmail.com>
 *
 */

#define  __SYLIXOS_KERNEL
#include <module.h>
#include <input_device.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <keyboard.h>
#include <mouse.h>
#include <sys/select.h>

#include "xdev.h"
#include "xlist.h"
#include "xinput.h"

/*
 * procfs add file
 */
extern void xinput_proc_init(void);
extern void xinput_proc_deinit(void);

/*
 * xinput device file struct
 */
struct xinput_file;
typedef struct xinput_file xinput_file_t;

/*
 * xinput device hdr
 */
typedef struct {
    LW_DEV_HDR        devhdr;
    time_t            time;
    LW_OBJECT_HANDLE  lock;
    xinput_file_t     *file_list;
    size_t            mq_size;
    size_t            msg_size;
    const char        *mq_name;
    int               dev_class;
} xinput_dev_t;

/*
 * xinput device file struct
 */
struct xinput_file {
    xinput_file_t     *prev;
    xinput_file_t     *next;
    xinput_dev_t      *dev;
    int               flags;
    LW_HANDLE         queue;
    LW_SEL_WAKEUPLIST sel_list;
    unsigned int      index_filter;
    unsigned int      get_dev_pos;
};

/*
 * xinput devices
 */
static xinput_dev_t kbd_xinput;
static xinput_dev_t mse_xinput;

/*
 * xinput device driver number
 */
static int xinput_drv_num = -1;

/*
 * xinput device hotplug event
 */
static LW_HANDLE  xinput_thread;
static BOOL       xinput_hotplug_fd = -1;
static BOOL       xinput_hotplug = FALSE;
static spinlock_t xinput_sl;

/*
 * xinput device msgq size
 */
static int xinput_kmqsize = MAX_INPUT_KQUEUE;
static int xinput_mmqsize = MAX_INPUT_MQUEUE;

/*
 * xinput device lock
 */
#define XINPUT_LOCK(xinput)     API_SemaphoreMPend((xinput)->lock, LW_OPTION_WAIT_INFINITE)
#define XINPUT_UNLOCK(xinput)   API_SemaphoreMPost((xinput)->lock)

/*
 * xinput open
 */
static long xinput_open (xinput_dev_t *dev, char *name, int flags, int mode)
{
    xinput_file_t  *file;

    if (!name) {
        errno = ERROR_IO_NO_DEVICE_NAME_IN_PATH;
        return  (PX_ERROR);

    } else {
        if (flags & O_CREAT) {
            errno = ERROR_IO_FILE_EXIST;
            return  (PX_ERROR);
        }

        file = (xinput_file_t *)__SHEAP_ALLOC(sizeof(xinput_file_t));
        if (!file) {
            errno = ERROR_SYSTEM_LOW_MEMORY;
            return  (PX_ERROR);
        }

        file->queue = API_MsgQueueCreate(dev->mq_name, (ULONG)dev->mq_size, dev->msg_size,
                                         LW_OPTION_OBJECT_GLOBAL, NULL);
        if (file->queue == LW_OBJECT_HANDLE_INVALID) {
            __SHEAP_FREE(file);
            errno = ERROR_SYSTEM_LOW_MEMORY;
            return  (PX_ERROR);
        }

        file->flags        = flags;
        file->dev          = dev;
        file->get_dev_pos  = 0;
        file->index_filter = (unsigned int)-1;
        INIT_LIST(file);
        SEL_WAKE_UP_LIST_INIT(&file->sel_list);

        XINPUT_LOCK(dev);
        INSERT_TO_HEADER(file, dev->file_list);
        LW_DEV_INC_USE_COUNT(&dev->devhdr);
        XINPUT_UNLOCK(dev);

        return  ((long)file);
    }
}

/*
 * xinput close
 */
static int xinput_close (xinput_file_t *file)
{
    xinput_dev_t  *dev;

    if (file) {
        dev = file->dev;

        XINPUT_LOCK(dev);
        SEL_WAKE_UP_LIST_TERM(&file->sel_list);
        DELETE_FROM_LIST(file, dev->file_list);
        LW_DEV_DEC_USE_COUNT(&dev->devhdr);
        API_MsgQueueDelete(&file->queue);
        __SHEAP_FREE(file);
        XINPUT_UNLOCK(dev);

        return  (ERROR_NONE);
    }

    return  (PX_ERROR);
}

/*
 * xinput read
 */
static ssize_t xinput_read (xinput_file_t *file, char *buffer, size_t maxbytes)
{
    u_long  timeout;
    size_t  msglen = 0;

    if (!file || !buffer) {
        errno = EINVAL;
        return  (PX_ERROR);
    }

    if (!maxbytes) {
        return  (0);
    }

    if (file->flags & O_NONBLOCK) {
        timeout = 0;
    } else {
        timeout = LW_OPTION_WAIT_INFINITE;
    }

    if (API_MsgQueueReceive(file->queue, buffer, maxbytes, &msglen, timeout)) {
        return  (PX_ERROR);
    }

    return  ((ssize_t)msglen);
}

/*
 * xinput ioctl
 */
static int xinput_ioctl (xinput_file_t *file, int cmd, long arg)
{
    struct stat         *pstat;
    PLW_SEL_WAKEUPNODE  pselnode;
    u_long              count = 0;
    size_t              len = 0;
    xinput_dev_t        *dev = file->dev;
    const input_dev_t   *input;

    switch (cmd) {

    case FIONBIO:
        if (*(int *)arg) {
            file->flags |= O_NONBLOCK;
        } else {
            file->flags &= ~O_NONBLOCK;
        }
        return  (ERROR_NONE);

    case FIONREAD:
        API_MsgQueueStatus(file->queue, NULL, &count, &len, NULL, NULL);
        if (count && arg) {
            *(int *)arg = (int)len;
        }
        return  (ERROR_NONE);

    case FIONREAD64:
        API_MsgQueueStatus(file->queue, NULL, &count, &len, NULL, NULL);
        if (count && arg) {
            *(off_t *)arg = (off_t)len;
        }
        return  (ERROR_NONE);

    case FIOFLUSH:
        API_MsgQueueClear(file->queue);
        return  (ERROR_NONE);

    case FIOFSTATGET:
        pstat = (struct stat *)arg;
        if (pstat) {
            pstat->st_dev     = LW_DEV_MAKE_STDEV(&dev->devhdr);
            pstat->st_ino     = (ino_t)0;
            pstat->st_mode    = (S_IRUSR | S_IRGRP | S_IROTH);
            pstat->st_nlink   = 1;
            pstat->st_uid     = 0;
            pstat->st_gid     = 0;
            pstat->st_rdev    = 0;
            pstat->st_size    = 0;
            pstat->st_blksize = 0;
            pstat->st_blocks  = 0;
            pstat->st_atime   = dev->time;
            pstat->st_mtime   = dev->time;
            pstat->st_ctime   = dev->time;
            return  (ERROR_NONE);
        }
        return  (PX_ERROR);

    case FIOSETFL:
        if ((int)arg & O_NONBLOCK) {
            file->flags |= O_NONBLOCK;
        } else {
            file->flags &= ~O_NONBLOCK;
        }
        return  (ERROR_NONE);

    case FIOSELECT:
        pselnode = (PLW_SEL_WAKEUPNODE)arg;
        SEL_WAKE_NODE_ADD(&file->sel_list, pselnode);
        switch (pselnode->SELWUN_seltypType) {

        case SELREAD:
            if (API_MsgQueueStatus(file->queue, NULL, &count, NULL, NULL, NULL)) {
                SEL_WAKE_UP(pselnode);
            } else if (count) {
                SEL_WAKE_UP(pselnode);
            }
            break;

        case SELWRITE:
        case SELEXCEPT:
            break;
        }
        return  (ERROR_NONE);

    case FIOUNSELECT:
        SEL_WAKE_NODE_DELETE(&file->sel_list, (PLW_SEL_WAKEUPNODE)arg);
        return  (ERROR_NONE);

    case XINPUT_CMD_REWIND_DEV:
        file->get_dev_pos = 0;
        return  (ERROR_NONE);

    case XINPUT_CMD_GET_DEV:
        if (arg) {
            input = xdev_get_dev(file->get_dev_pos, dev->dev_class);
            if (input) {
                strcpy((char *)arg, input->dev);
                file->get_dev_pos = input->index + 1;
            } else {
                *(char *)arg = '\0';
            }
            return  (ERROR_NONE);
        }
        errno = EINVAL;
        return  (PX_ERROR);

    case XINPUT_CMD_GET_DEV_INFO:
        if (arg) {
            xinput_device_info_arg_t *info_arg = (xinput_device_info_arg_t *)arg;

            if (info_arg->dev) {
                input = xdev_find_dev(info_arg->dev, dev->dev_class);
                if (input && (input->fd >= 0)) {
                    if (dev->dev_class == XINPUT_KBD_CLASS) {
                        keyboard_device_info info;

                        if (ioctl(input->fd, KBD_CTL_GET_DEVICE_INFO, &info) == 0) {
                            strlcpy(info_arg->vendor,  info.vendor,  sizeof(info_arg->vendor));
                            strlcpy(info_arg->product, info.product, sizeof(info_arg->product));
                            strlcpy(info_arg->serial,  info.serial,  sizeof(info_arg->serial));
                            return  (ERROR_NONE);
                        }
                    } else {
                        mouse_device_info info;

                        if (ioctl(input->fd, MSE_CTL_GET_DEVICE_INFO, &info) == 0) {
                            strlcpy(info_arg->vendor,  info.vendor,  sizeof(info_arg->vendor));
                            strlcpy(info_arg->product, info.product, sizeof(info_arg->product));
                            strlcpy(info_arg->serial,  info.serial,  sizeof(info_arg->serial));
                            return  (ERROR_NONE);
                        }
                    }
                }
            }
        }
        errno = EINVAL;
        return  (PX_ERROR);

    case XINPUT_CMD_CLEAR_DEVS:
        XINPUT_LOCK(dev);
        file->index_filter = 0;
        XINPUT_UNLOCK(dev);
        return  (ERROR_NONE);

    case XINPUT_CMD_ADD_DEV:
        if (arg) {
            input = xdev_find_dev((const char *)arg, dev->dev_class);
            if (input) {
                XINPUT_LOCK(dev);
                file->index_filter |= 1 << input->index;
                XINPUT_UNLOCK(dev);
                return  (ERROR_NONE);
            }
        }
        errno = EINVAL;
        return  (PX_ERROR);

    case XINPUT_CMD_DEL_DEV:
        if (arg) {
            input = xdev_find_dev((const char *)arg, dev->dev_class);
            if (input) {
                XINPUT_LOCK(dev);
                file->index_filter &= ~(1 << input->index);
                XINPUT_UNLOCK(dev);
                return  (ERROR_NONE);
            }
        }
        errno = EINVAL;
        return  (PX_ERROR);

    case XINPUT_CMD_SET_KBD_LED:
        if (arg && (dev->dev_class == XINPUT_KBD_CLASS)) {
            xinput_led_arg_t *led_arg = (xinput_led_arg_t *)arg;

            if (led_arg->dev) {
                input = xdev_find_dev(led_arg->dev, dev->dev_class);
                if (input && (input->fd >= 0) && (file->index_filter & (1 << input->index))) {
                    return  (ioctl(input->fd, SIO_KYBD_LED_SET, led_arg->led));
                }
            }
        }
        errno = EINVAL;
        return  (PX_ERROR);

    case XINPUT_CMD_GET_KBD_LED:
        if (arg && (dev->dev_class == XINPUT_KBD_CLASS)) {
            xinput_led_arg_t *led_arg = (xinput_led_arg_t *)arg;

            if (led_arg->dev) {
                input = xdev_find_dev(led_arg->dev, dev->dev_class);
                if (input && (input->fd >= 0) && (file->index_filter & (1 << input->index))) {
                    return  (ioctl(input->fd, SIO_KYBD_LED_GET, &led_arg->led));
                }
            }
        }
        errno = EINVAL;
        return  (PX_ERROR);

    case SIO_KYBD_LED_SET:
        if (dev->dev_class == XINPUT_KBD_CLASS) {
            int i;

            input = xdev_kbd_array;
            for (i = 0; i < xdev_kbd_num; i++, input++) {
                if (file->index_filter & (1 << i)) {
                    if (input->fd >= 0) {
                        ioctl(input->fd, SIO_KYBD_LED_SET, arg);
                    }
                }
            }
            return  (ERROR_NONE);
        }
        errno = EINVAL;
        return  (PX_ERROR);

    default:
        errno = ENOSYS;
        return  (PX_ERROR);
    }
}

/*
 * xinput device install driver
 */
static int xinput_drv (void)
{
    if (xinput_drv_num >= 0) {
        return  (ERROR_NONE);
    }

    xinput_drv_num = iosDrvInstall(xinput_open, NULL, xinput_open, xinput_close,
                                   xinput_read, NULL, xinput_ioctl);

    DRIVER_LICENSE(xinput_drv_num,     "GPL->Ver 2.0");
    DRIVER_AUTHOR(xinput_drv_num,      "Han.hui & Jiao.jinxing");
    DRIVER_DESCRIPTION(xinput_drv_num, "xinput driver v2.");

    return  ((xinput_drv_num > 0) ? (ERROR_NONE) : (PX_ERROR));
}

/*
 * xinput device install
 */
static int xinput_devadd (void)
{
    kbd_xinput.time = time(NULL);
    kbd_xinput.lock = API_SemaphoreMCreate("xkbd_l", LW_PRIO_DEF_CEILING,
                                           LW_OPTION_WAIT_PRIORITY | LW_OPTION_DELETE_SAFE |
                                           LW_OPTION_INHERIT_PRIORITY | LW_OPTION_OBJECT_GLOBAL,
                                           NULL);
    kbd_xinput.mq_name   = "xkbd_q";
    kbd_xinput.mq_size   = xinput_kmqsize;
    kbd_xinput.msg_size  = sizeof(struct keyboard_event_notify);
    kbd_xinput.dev_class = XINPUT_KBD_CLASS;
    kbd_xinput.file_list = NULL;

    if (iosDevAddEx(&kbd_xinput.devhdr, XINPUT_NAME_KBD, xinput_drv_num, DT_CHR) != ERROR_NONE) {
        printk(KERN_ERR "xinput can not add device: %s.\n", strerror(errno));
        return  (PX_ERROR);
    }

    mse_xinput.time = time(NULL);
    mse_xinput.lock = API_SemaphoreMCreate("xmse_l", LW_PRIO_DEF_CEILING,
                                           LW_OPTION_WAIT_PRIORITY | LW_OPTION_DELETE_SAFE |
                                           LW_OPTION_INHERIT_PRIORITY | LW_OPTION_OBJECT_GLOBAL,
                                           NULL);
    mse_xinput.mq_name   = "xmse_q";
    mse_xinput.mq_size   = xinput_mmqsize;
    mse_xinput.msg_size  = sizeof(struct mouse_event_notify) * MAX_INPUT_POINTS;
    mse_xinput.dev_class = XINPUT_MSE_CLASS;
    mse_xinput.file_list = NULL;

    if (iosDevAddEx(&mse_xinput.devhdr, XINPUT_NAME_MSE, xinput_drv_num, DT_CHR) != ERROR_NONE) {
        printk(KERN_ERR "xinput can not add device: %s.\n", strerror(errno));
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}

/*
 * xinput hotplug callback
 */
static void  xinput_hotplug_cb (unsigned char *phpmsg, size_t size)
{
    INTREG  int_lock;
    int     msg_type;

    (void)size;

    msg_type = (int)((phpmsg[0] << 24) + (phpmsg[1] << 16) + (phpmsg[2] << 8) + phpmsg[3]);
    if ((msg_type == LW_HOTPLUG_MSG_USB_KEYBOARD) ||
        (msg_type == LW_HOTPLUG_MSG_USB_MOUSE)    ||
        (msg_type == LW_HOTPLUG_MSG_PCI_INPUT)) {
        if (phpmsg[4]) {
            LW_SPIN_LOCK_QUICK(&xinput_sl, &int_lock);
            xinput_hotplug = TRUE; /* need open files */
            LW_SPIN_UNLOCK_QUICK(&xinput_sl, int_lock);
        }
    }
}

/*
 * xinput_scan thread
 */
static void  *xinput_scan (void *arg)
{
    INTREG                 int_lock;
    int                    i;
    fd_set                 fdset;
    int                    width;
    int                    ret;

    keyboard_event_notify  knotify;
    mouse_event_notify     mnotify[MAX_INPUT_POINTS];

    BOOL                   need_check;
    xinput_file_t          *file;

    (void)arg;

    xinput_hotplug_fd = open("/dev/hotplug", O_RDONLY);

    for (;;) {
        FD_ZERO(&fdset);

        LW_SPIN_LOCK_QUICK(&xinput_sl, &int_lock);
        need_check     = xinput_hotplug;
        xinput_hotplug = FALSE;
        LW_SPIN_UNLOCK_QUICK(&xinput_sl, int_lock);

        if (need_check) { /* need check if a device has been plug on */
            xdev_try_open();
        }

        width = xdev_set_fdset(&fdset);
        if (xinput_hotplug_fd >= 0) {
            FD_SET(xinput_hotplug_fd, &fdset);
            width = (width > xinput_hotplug_fd) ? width : xinput_hotplug_fd;
        }
        width += 1;

        ret = select(width, &fdset, NULL, NULL, NULL); /* no timeout needed */
        if (ret < 0) {
            if (errno == EINTR) {
                continue; /* EINTR */
            }
            xdev_close_all();

            LW_SPIN_LOCK_QUICK(&xinput_sl, &int_lock);
            xinput_hotplug = TRUE; /* need reopen files */
            LW_SPIN_UNLOCK_QUICK(&xinput_sl, int_lock);

            sleep(XINPUT_SEL_TO);
            continue;

        } else if (ret == 0) {
            continue;

        } else {
            ssize_t     temp;
            input_dev_t *input;

            if (xinput_hotplug_fd >= 0 && FD_ISSET(xinput_hotplug_fd, &fdset)) {
                unsigned char hpmsg[LW_HOTPLUG_DEV_MAX_MSGSIZE];
                temp = read(xinput_hotplug_fd, hpmsg, LW_HOTPLUG_DEV_MAX_MSGSIZE);
                if (temp > 0) {
                    xinput_hotplug_cb(hpmsg, (size_t)temp);
                }
            }

            input = xdev_kbd_array;
            for (i = 0; i < xdev_kbd_num; i++, input++) {
                if (input->fd >= 0) {
                    if (FD_ISSET(input->fd, &fdset)) {
                        temp = read(input->fd, (PVOID)&knotify, sizeof(knotify));
                        if (temp <= 0) {
                            close(input->fd);
                            input->fd = -1;
                        } else {    /* must send kbd message ok     */
                            if (LW_DEV_GET_USE_COUNT(&kbd_xinput.devhdr)) {
                                XINPUT_LOCK(&kbd_xinput);
                                FOREACH_FROM_LIST(file, kbd_xinput.file_list) {
                                    if (file->index_filter & (1 << input->index)) {
                                        API_MsgQueueSend2(file->queue, (void *)&knotify,
                                                          (u_long)temp, LW_OPTION_WAIT_A_SECOND);
                                        SEL_WAKE_UP_ALL(&file->sel_list, SELREAD);
                                    }
                                }
                                XINPUT_UNLOCK(&kbd_xinput);
                            }
                        }
                    }
                }
            }

            input = xdev_mse_array;
            for (i = 0; i < xdev_mse_num; i++, input++) {
                if (input->fd >= 0) {
                    if (FD_ISSET(input->fd, &fdset)) {
                        temp = read(input->fd, (PVOID)mnotify,
                                    sizeof(mouse_event_notify) * MAX_INPUT_POINTS);
                        if (temp <= 0) {
                            close(input->fd);
                            input->fd = -1;
                        } else {
                            if (LW_DEV_GET_USE_COUNT(&mse_xinput.devhdr)) {
                                XINPUT_LOCK(&mse_xinput);
                                FOREACH_FROM_LIST(file, mse_xinput.file_list) {
                                    if (file->index_filter & (1 << input->index)) {
                                        if (!(mnotify[0].kstat & MOUSE_LEFT)) {/* release operate must send suc */
                                            API_MsgQueueSend2(file->queue, (void *)mnotify,
                                                              (u_long)temp, LW_OPTION_WAIT_A_SECOND);
                                        } else {
                                            API_MsgQueueSend(file->queue, (void *)mnotify, (u_long)temp);
                                        }
                                        SEL_WAKE_UP_ALL(&file->sel_list, SELREAD);
                                    }
                                }
                                XINPUT_UNLOCK(&mse_xinput);
                            }
                        }
                    }
                }
            }
        }
    }

    return  (NULL);
}

/*
 * module_init
 */
int module_init (void)
{
    LW_CLASS_THREADATTR threadattr;
    char                temp_str[128];
    int                 prio;

    xdev_init();

    xinput_proc_init();

    if (xinput_drv()) {
        return  (PX_ERROR);
    }

    if (getenv_r("XINPUT_KQSIZE", temp_str, sizeof(temp_str)) == 0) {
        xinput_kmqsize = atoi(temp_str);
    }

    if (getenv_r("XINPUT_MQSIZE", temp_str, sizeof(temp_str)) == 0) {
        xinput_mmqsize = atoi(temp_str);
    }

    if (xinput_devadd()) {
        return  (PX_ERROR);
    }

    LW_SPIN_INIT(&xinput_sl);
    xinput_hotplug = TRUE; /* need open once first */

    threadattr = API_ThreadAttrGetDefault();

    if (getenv_r("XINPUT_PRIO", temp_str, sizeof(temp_str)) == 0) {
        prio = atoi(temp_str);
        threadattr.THREADATTR_ucPriority = (uint8_t)prio;
    }

    threadattr.THREADATTR_ulOption |= LW_OPTION_OBJECT_GLOBAL;
    threadattr.THREADATTR_stStackByteSize = XINPUT_THREAD_SIZE;

    xinput_thread = API_ThreadCreate("t_xinput", xinput_scan, &threadattr, NULL);

    return  (ERROR_NONE);
}

void module_exit (void)
{
    xinput_proc_deinit();

#if LW_CFG_SIGNAL_EN > 0
    kill(xinput_thread, SIGTERM);
#else
    API_ThreadDelete(&xinput_thread, NULL);
#endif

    if (xinput_hotplug_fd >= 0) {
        close(xinput_hotplug_fd);
    }

    iosDevFileAbnormal(&kbd_xinput.devhdr);
    iosDevFileAbnormal(&mse_xinput.devhdr);

    iosDevDelete(&kbd_xinput.devhdr);
    iosDevDelete(&mse_xinput.devhdr);

    API_SemaphoreMDelete(&kbd_xinput.lock);
    API_SemaphoreMDelete(&mse_xinput.lock);

    iosDrvRemove(xinput_drv_num, TRUE);
}

/*
 * end
 */
