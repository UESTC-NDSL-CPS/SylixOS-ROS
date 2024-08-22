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
** 文   件   名: loader_vptls.c
**
** 创   建   人: Jiao.jinxing (焦进星)
**
** 文件创建日期: 2023 年 04 月 25 日
**
** 描        述: 进程内线程 TLS 内存管理.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
  裁剪支持
*********************************************************************************************************/
#if LW_CFG_MODULELOADER_EN > 0
#include "../include/loader_lib.h"
/*********************************************************************************************************
** 函数名称: vprocTlsAlloc
** 功能描述: 分配 TLS
** 输　入  : ptcbNew       新建任务
**           ulOption      任务创建选项
** 输　出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
ULONG  vprocTlsAlloc (PLW_CLASS_TCB  ptcbNew, ULONG  ulOption)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_LD_VPROC    *pvproc;
    size_t          stTlsSize;
    ULONG           ulError = ERROR_NONE;

    LW_TCB_GET_CUR_SAFE(ptcbCur);                                       /*  当前 ptcb                   */

    if (ptcbCur && !(ulOption & LW_OPTION_OBJECT_GLOBAL)) {             /*  继承进程控制块              */
        pvproc = __LW_VP_GET_TCB_PROC(ptcbCur);
        if (pvproc) {
            stTlsSize = pvproc->VP_stTlsSize;
            if (stTlsSize > 0) {
#if LW_CFG_VMM_EN > 0
                ptcbNew->TCB_pvTlsLowAddr = pvproc->vp_pvTlsMem + ptcbNew->TCB_usIndex * stTlsSize;
#else
                ptcbNew->TCB_pvTlsLowAddr = __KHEAP_ALLOC(stTlsSize);
#endif                                                                  /*  LW_CFG_VMM_EN > 0           */
                if (ptcbNew->TCB_pvTlsLowAddr) {
                    ptcbNew->TCB_pvTlsHighAddr = ptcbNew->TCB_pvTlsLowAddr + stTlsSize - sizeof(LW_STACK);
                } else {
                    ulError = PX_ERROR;
                }
            }
        }
    }

    return  (ulError);
}
/*********************************************************************************************************
** 函数名称: vprocTlsFree
** 功能描述: 释放 TLS
** 输　入  : ptcbDel       被删除的任务
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  vprocTlsFree (PLW_CLASS_TCB  ptcbDel)
{
    if (ptcbDel->TCB_pvTlsLowAddr) {
#if LW_CFG_VMM_EN == 0
        __KHEAP_FREE(ptcbDel->TCB_pvTlsLowAddr);
#endif                                                                  /*  LW_CFG_VMM_EN > 0           */
        ptcbDel->TCB_pvTlsLowAddr  = LW_NULL;
        ptcbDel->TCB_pvTlsHighAddr = LW_NULL;
    }
}

#endif                                                                  /*  LW_CFG_MODULELOADER_EN > 0  */
/*********************************************************************************************************
  END
*********************************************************************************************************/
