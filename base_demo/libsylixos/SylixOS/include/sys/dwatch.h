/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: dwatch.h
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2023 年 06 月 23 日
**
** 描        述: 信号量设备实现.
*********************************************************************************************************/

#ifndef __SYS_DWATCH_H
#define __SYS_DWATCH_H

#include <unistd.h>
#include <fcntl.h>
#include <sys/cdefs.h>

/*********************************************************************************************************
 dwatch() flags
*********************************************************************************************************/

#define DWATCH_CLOEXEC          O_CLOEXEC
#define DWATCH_NONBLOCK         O_NONBLOCK

/*********************************************************************************************************
 dwatch events filter
*********************************************************************************************************/

#define DWATCH_CREATE           0x01                                    /*  file creation               */
#define DWATCH_DELETE           0x02                                    /*  file deletion               */
#define DWATCH_MODIFY           0x04                                    /*  file content change         */
#define DWATCH_ATTRIBUTES       0x08                                    /*  file attribute change       */
#define DWATCH_MOVE_FROM        0x100                                   /*  file move from (rename)     */
#define DWATCH_MOVE_TO          0x200                                   /*  file move to                */
#define DWATCH_SUBTREE          0x80000000                              /*  watch subtree               */

/*********************************************************************************************************
 All events
*********************************************************************************************************/

#define DWATCH_ALL              (DWATCH_CREATE | DWATCH_DELETE | DWATCH_MODIFY | DWATCH_ATTRIBUTES | \
                                 DWATCH_MOVE_FROM | DWATCH_MOVE_TO)
#define DWATCH_ALL_SUBTREE      (DWATCH_ALL | DWATCH_SUBTREE)

/*********************************************************************************************************
 dwatch event

 NOTICE: The `fname` field is present only when an event is returned for a
         file inside a watched directory; it identifies the filename
         within the watched directory.  This filename is null-terminated,
         and may include further null bytes ('\0') to align subsequent
         reads to a suitable address boundary.

         The `fnlen` field counts all of the bytes in name, including the null
         bytes; the length of each `dwatch_event` structure is thus
         `sizeof(struct dwatch_event) + fnlen`.

         `sizeof(struct dwatch_event) + NAME_MAX + 1` will be sufficient to read at least one event.

         The `DWATCH_MOVE_FROM` event will be followed by the `DWATCH_MOVE_TO` event.
         If `DWATCH_MOVE_TO` event `fname[0] == '\0'`, it means that the file has
         been moved out of the watching directory.

         If there is no `DWATCH_MOVE_FROM` before `DWATCH_MOVE_TO`,
         it means moving to watching directory from outside of watching directory.
*********************************************************************************************************/

struct dwatch_event {
    unsigned int  event;                                                /*  event                       */
    unsigned int  fnlen;                                                /*  file name length            */
    char          fname __flexarr;                                      /*  file name buffer            */
};

/*********************************************************************************************************
 dwatch() api

 NOTICE: The file system directory specified by dirname must support DWATCH features, such as TpsFS
         otherwise an error will be returned.
*********************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif                                                                  /*  __cplusplus                 */

int dwatch(const char *dirname, int flags, unsigned int events);

#ifdef __cplusplus
}
#endif                                                                  /*  __cplusplus                 */

#endif                                                                  /*  __SYS_DWATCH_H              */
/*********************************************************************************************************
  END
*********************************************************************************************************/
