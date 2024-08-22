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
** ��   ��   ��: ThreadHetrcc.c
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2023 �� 06 �� 06 ��
**
** ��        ��: �߳��칹�����ص���.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "../SylixOS/kernel/include/k_kernel.h"
/*********************************************************************************************************
  �ü�֧��
*********************************************************************************************************/
#if LW_CFG_SMP_EN > 0
/*********************************************************************************************************
** ��������: API_ThreadSetHetrcc
** ��������: ���߳����õ�ָ���� CPU ����������.
** �䡡��  : ulId          �߳�
             iMode         ģʽ (LW_HETRCC_NON_AFFINITY,
                                 LW_HETRCC_WEAK_AFFINITY,
                                 LW_HETRCC_STRONG_AFFINITY)
             ulHccId       �칹������ ID (���Խ������Խǿ)
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
                                           API ����
*********************************************************************************************************/
LW_API
ULONG  API_ThreadSetHetrcc (LW_OBJECT_HANDLE  ulId, INT  iMode, ULONG  ulHccId)
{
#if LW_CFG_CPU_ARCH_HETRC > 0
             BOOL           bIsSame;
    REGISTER UINT16         usIndex;
    REGISTER PLW_CLASS_TCB  ptcb;
             PLW_CLASS_TCB  ptcbCur;
             ULONG          ulError;

    usIndex = _ObjectGetIndex(ulId);

#if LW_CFG_ARG_CHK_EN > 0
    if (!_ObjectClassOK(ulId, _OBJECT_THREAD)) {                        /*  ��� ID ������Ч��          */
        _ErrorHandle(ERROR_KERNEL_HANDLE_NULL);
        return  (ERROR_KERNEL_HANDLE_NULL);
    }
    if (_Thread_Index_Invalid(usIndex)) {                               /*  ����߳���Ч��              */
        _ErrorHandle(ERROR_KERNEL_HANDLE_NULL);
        return  (ERROR_KERNEL_HANDLE_NULL);
    }
    if ((iMode != LW_HETRCC_NON_AFFINITY)  &&
        (iMode != LW_HETRCC_WEAK_AFFINITY) &&
        (iMode != LW_HETRCC_STRONG_AFFINITY)) {                         /*  ģʽ����                    */
        _ErrorHandle(EINVAL);
        return  (EINVAL);
    }
#endif

    if (ulId == API_KernelGetExc()) {                                   /*  ���������� exce �߳�        */
        _ErrorHandle(EPERM);
        return  (EPERM);
    }

__again:
    __KERNEL_ENTER();                                                   /*  �����ں�                    */
    if (_Thread_Invalid(usIndex)) {
        __KERNEL_EXIT();                                                /*  �˳��ں�                    */
        _ErrorHandle(ERROR_KERNEL_HANDLE_NULL);
        return  (ERROR_KERNEL_HANDLE_NULL);
    }

    if ((iMode != LW_HETRCC_NON_AFFINITY) &&
        !_CpuHetrccValid(ulHccId, LW_FALSE, LW_NULL)) {                 /*  HccId �Ƿ���Ч              */
        __KERNEL_EXIT();                                                /*  �˳��ں�                    */
        _ErrorHandle(ERROR_KERNEL_CPU_NULL);
        return  (ERROR_KERNEL_CPU_NULL);
    }

    ptcb = _K_ptcbTCBIdTable[usIndex];
    if (ptcb->TCB_iDeleteProcStatus) {                                  /*  ��ɾ���������Ĺ�����        */
        __KERNEL_EXIT();                                                /*  �˳��ں�                    */
        _ErrorHandle(ERROR_THREAD_NULL);
        return  (ERROR_THREAD_NULL);
    }

    bIsSame = LW_FALSE;

    if (iMode == LW_HETRCC_NON_AFFINITY) {
        if (ptcb->TCB_iHetrccMode == iMode) {
            bIsSame = LW_TRUE;                                          /*  �ޱ仯                      */
        } else {
            ulHccId = 0ul;                                              /*  ���ɵ���                    */
        }

    } else if (iMode == LW_HETRCC_STRONG_AFFINITY) {
        if ((ptcb->TCB_iHetrccMode  == iMode) &&
            (ptcb->TCB_ulHetrccLock == ulHccId)) {                      /*  Strong �׺Ͳ�����ͬ         */
            bIsSame = LW_TRUE;
        }
    }

    if (bIsSame) {
        __KERNEL_EXIT();                                                /*  �˳��ں�                    */
        return  (ERROR_NONE);
    }

    LW_TCB_GET_CUR(ptcbCur);

    if (ptcb == ptcbCur) {
        if (__THREAD_LOCK_GET(ptcb) > 1) {                              /*  ��������                  */
            __KERNEL_EXIT();                                            /*  �˳��ں�                    */
            _ErrorHandle(EBUSY);
            return  (EBUSY);
        }
        _ThreadSetHetrcc(ptcb, iMode, ulHccId);                         /*  ����                        */

    } else {
        if (__THREAD_LOCK_GET(ptcb)) {                                  /*  ��������                  */
            __KERNEL_EXIT();                                            /*  �˳��ں�                    */
            _ErrorHandle(EBUSY);
            return  (EBUSY);
        }

        ulError = _ThreadStop(ptcb);
        __KERNEL_EXIT();                                                /*  �˳��ں�                    */
        if (ulError) {
            if (ulError == EAGAIN) {
                goto    __again;
            }
            return  (ulError);
        }

#if LW_CFG_SMP_EN > 0
        if (ptcbCur->TCB_uiStatusChangeReq) {
            ptcbCur->TCB_uiStatusChangeReq = 0;
            _ErrorHandle(ERROR_THREAD_NULL);
            return  (ERROR_THREAD_NULL);
        }
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */

        __KERNEL_ENTER();                                               /*  �����ں�                    */
        _ThreadSetHetrcc(ptcb, iMode, ulHccId);                         /*  ����                        */
        _ThreadContinue(ptcb, LW_FALSE);                                /*  ����Ŀ��                    */
    }

    __KERNEL_EXIT();                                                    /*  �˳��ں�                    */

    return  (ERROR_NONE);

#else                                                                   /*  LW_CFG_CPU_ARCH_HETRC       */
    if (ulHccId == 0) {
        return  (ERROR_NONE);

    } else {
        _ErrorHandle(ENOSYS);
        return  (ENOSYS);
    }
#endif                                                                  /*  !LW_CFG_CPU_ARCH_HETRC      */
}
/*********************************************************************************************************
** ��������: API_ThreadGetHetrcc
** ��������: ��ȡ�̵߳�ǰ���õ� CPU ������
** �䡡��  : ulId          �߳�
             piMode        ģʽ (LW_HETRCC_NON_AFFINITY,
                                 LW_HETRCC_WEAK_AFFINITY,
                                 LW_HETRCC_STRONG_AFFINITY)
             pulHccId      �칹������ ID (���Խ������Խǿ)
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
                                           API ����
*********************************************************************************************************/
LW_API
ULONG  API_ThreadGetHetrcc (LW_OBJECT_HANDLE  ulId, INT  *piMode, ULONG  *pulHccId)
{
#if LW_CFG_CPU_ARCH_HETRC > 0
    REGISTER UINT16         usIndex;
    REGISTER PLW_CLASS_TCB  ptcb;

    usIndex = _ObjectGetIndex(ulId);

#if LW_CFG_ARG_CHK_EN > 0
    if (!_ObjectClassOK(ulId, _OBJECT_THREAD)) {                        /*  ��� ID ������Ч��          */
        _ErrorHandle(ERROR_KERNEL_HANDLE_NULL);
        return  (ERROR_KERNEL_HANDLE_NULL);
    }
    if (_Thread_Index_Invalid(usIndex)) {                               /*  ����߳���Ч��              */
        _ErrorHandle(ERROR_KERNEL_HANDLE_NULL);
        return  (ERROR_KERNEL_HANDLE_NULL);
    }
#endif

    __KERNEL_ENTER();                                                   /*  �����ں�                    */
    if (_Thread_Invalid(usIndex)) {
        __KERNEL_EXIT();                                                /*  �˳��ں�                    */
        _ErrorHandle(ERROR_KERNEL_HANDLE_NULL);
        return  (ERROR_KERNEL_HANDLE_NULL);
    }

    ptcb = _K_ptcbTCBIdTable[usIndex];
    _ThreadGetHetrcc(ptcb, piMode, pulHccId);
    __KERNEL_EXIT();                                                    /*  �˳��ں�                    */

#else                                                                   /*  LW_CFG_CPU_ARCH_HETRC       */
    if (piMode) {
        *piMode = LW_HETRCC_NON_AFFINITY;
    }
    if (pulHccId) {
        *pulHccId = 0ul;
    }
#endif                                                                  /*  !LW_CFG_CPU_ARCH_HETRC      */

    return  (ERROR_NONE);
}

#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
/*********************************************************************************************************
  END
*********************************************************************************************************/
