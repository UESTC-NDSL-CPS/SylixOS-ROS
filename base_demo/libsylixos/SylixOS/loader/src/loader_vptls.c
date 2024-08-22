/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: loader_vptls.c
**
** ��   ��   ��: Jiao.jinxing (������)
**
** �ļ���������: 2023 �� 04 �� 25 ��
**
** ��        ��: �������߳� TLS �ڴ����.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
  �ü�֧��
*********************************************************************************************************/
#if LW_CFG_MODULELOADER_EN > 0
#include "../include/loader_lib.h"
/*********************************************************************************************************
** ��������: vprocTlsAlloc
** ��������: ���� TLS
** �䡡��  : ptcbNew       �½�����
**           ulOption      ���񴴽�ѡ��
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
ULONG  vprocTlsAlloc (PLW_CLASS_TCB  ptcbNew, ULONG  ulOption)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_LD_VPROC    *pvproc;
    size_t          stTlsSize;
    ULONG           ulError = ERROR_NONE;

    LW_TCB_GET_CUR_SAFE(ptcbCur);                                       /*  ��ǰ ptcb                   */

    if (ptcbCur && !(ulOption & LW_OPTION_OBJECT_GLOBAL)) {             /*  �̳н��̿��ƿ�              */
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
** ��������: vprocTlsFree
** ��������: �ͷ� TLS
** �䡡��  : ptcbDel       ��ɾ��������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
