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
** 文   件   名: alloca.h
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2014 年 09 月 09 日
**
** 描        述: 从任务栈中分配内存.
*********************************************************************************************************/

#ifndef __ALLOCA_H
#define __ALLOCA_H

#ifdef __GNUC__
#ifndef alloca
#if defined(__TMS320C6X__)
#include <stddef.h>

extern void *alloca(size_t size);
extern void *alloca_with_align(size_t size, size_t align_bits);
#else
#define alloca(size)                        __builtin_alloca(size)
#define alloca_with_align(size, align_bits) __builtin_alloca_with_align(size, align_bits)
#endif
#endif
#endif                                                                  /*  __GNUC__                    */

#endif                                                                  /*  __ALLOCA_H                  */
/*********************************************************************************************************
  END
*********************************************************************************************************/
