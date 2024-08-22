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
** 文   件   名: hwcap.h
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2016 年 06 月 25 日
**
** 描        述: x86 硬件特性标记.
*********************************************************************************************************/

#ifndef __ASMX86_HWCAP_H
#define __ASMX86_HWCAP_H

#define HWCAP_FPU       (1 << 0)
#define HWCAP_MMX       (1 << 1)
#define HWCAP_SSE       (1 << 2)
#define HWCAP_SSE2      (1 << 3)
#define HWCAP_AVX       (1 << 4)

#endif                                                                  /*  __ASMX86_HWCAP_H            */
/*********************************************************************************************************
  END
*********************************************************************************************************/
