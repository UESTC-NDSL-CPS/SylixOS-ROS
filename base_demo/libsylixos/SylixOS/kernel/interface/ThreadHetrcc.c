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
** 文   件   名: ThreadHetrcc.c
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2023 年 06 月 06 日
**
** 描        述: 线程异构算力簇调度.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "../SylixOS/kernel/include/k_kernel.h"
/*********************************************************************************************************
  裁剪支持
*********************************************************************************************************/
#if LW_CFG_SMP_EN > 0
/*********************************************************************************************************
** 函数名称: API_ThreadSetHetrcc
** 功能描述: 将线程设置到指定的 CPU 算力簇运行.
** 输　入  : ulId          线程
             iMode         模式 (LW_HETRCC_NON_AFFINITY,
                                 LW_HETRCC_WEAK_AFFINITY,
                                 LW_HETRCC_STRONG_AFFINITY)
             ulHccId       异构算力簇 ID (编号越大算力越强)
** 输　出  : ERROR CODE
** 全局变量:
** 调用模块:
                                           API 函数
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
    if (!_ObjectClassOK(ulId, _OBJECT_THREAD)) {                        /*  检查 ID 类型有效性          */
        _ErrorHandle(ERROR_KERNEL_HANDLE_NULL);
        return  (ERROR_KERNEL_HANDLE_NULL);
    }
    if (_Thread_Index_Invalid(usIndex)) {                               /*  检查线程有效性              */
        _ErrorHandle(ERROR_KERNEL_HANDLE_NULL);
        return  (ERROR_KERNEL_HANDLE_NULL);
    }
    if ((iMode != LW_HETRCC_NON_AFFINITY)  &&
        (iMode != LW_HETRCC_WEAK_AFFINITY) &&
        (iMode != LW_HETRCC_STRONG_AFFINITY)) {                         /*  模式错误                    */
        _ErrorHandle(EINVAL);
        return  (EINVAL);
    }
#endif

    if (ulId == API_KernelGetExc()) {                                   /*  不允许设置 exce 线程        */
        _ErrorHandle(EPERM);
        return  (EPERM);
    }

__again:
    __KERNEL_ENTER();                                                   /*  进入内核                    */
    if (_Thread_Invalid(usIndex)) {
        __KERNEL_EXIT();                                                /*  退出内核                    */
        _ErrorHandle(ERROR_KERNEL_HANDLE_NULL);
        return  (ERROR_KERNEL_HANDLE_NULL);
    }

    if ((iMode != LW_HETRCC_NON_AFFINITY) &&
        !_CpuHetrccValid(ulHccId, LW_FALSE, LW_NULL)) {                 /*  HccId 是否有效              */
        __KERNEL_EXIT();                                                /*  退出内核                    */
        _ErrorHandle(ERROR_KERNEL_CPU_NULL);
        return  (ERROR_KERNEL_CPU_NULL);
    }

    ptcb = _K_ptcbTCBIdTable[usIndex];
    if (ptcb->TCB_iDeleteProcStatus) {                                  /*  在删除和重启的过程中        */
        __KERNEL_EXIT();                                                /*  退出内核                    */
        _ErrorHandle(ERROR_THREAD_NULL);
        return  (ERROR_THREAD_NULL);
    }

    bIsSame = LW_FALSE;

    if (iMode == LW_HETRCC_NON_AFFINITY) {
        if (ptcb->TCB_iHetrccMode == iMode) {
            bIsSame = LW_TRUE;                                          /*  无变化                      */
        } else {
            ulHccId = 0ul;                                              /*  自由调度                    */
        }

    } else if (iMode == LW_HETRCC_STRONG_AFFINITY) {
        if ((ptcb->TCB_iHetrccMode  == iMode) &&
            (ptcb->TCB_ulHetrccLock == ulHccId)) {                      /*  Strong 亲和参数相同         */
            bIsSame = LW_TRUE;
        }
    }

    if (bIsSame) {
        __KERNEL_EXIT();                                                /*  退出内核                    */
        return  (ERROR_NONE);
    }

    LW_TCB_GET_CUR(ptcbCur);

    if (ptcb == ptcbCur) {
        if (__THREAD_LOCK_GET(ptcb) > 1) {                              /*  任务被锁定                  */
            __KERNEL_EXIT();                                            /*  退出内核                    */
            _ErrorHandle(EBUSY);
            return  (EBUSY);
        }
        _ThreadSetHetrcc(ptcb, iMode, ulHccId);                         /*  设置                        */

    } else {
        if (__THREAD_LOCK_GET(ptcb)) {                                  /*  任务被锁定                  */
            __KERNEL_EXIT();                                            /*  退出内核                    */
            _ErrorHandle(EBUSY);
            return  (EBUSY);
        }

        ulError = _ThreadStop(ptcb);
        __KERNEL_EXIT();                                                /*  退出内核                    */
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

        __KERNEL_ENTER();                                               /*  进入内核                    */
        _ThreadSetHetrcc(ptcb, iMode, ulHccId);                         /*  设置                        */
        _ThreadContinue(ptcb, LW_FALSE);                                /*  唤醒目标                    */
    }

    __KERNEL_EXIT();                                                    /*  退出内核                    */

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
** 函数名称: API_ThreadGetHetrcc
** 功能描述: 获取线程当前设置的 CPU 算力簇
** 输　入  : ulId          线程
             piMode        模式 (LW_HETRCC_NON_AFFINITY,
                                 LW_HETRCC_WEAK_AFFINITY,
                                 LW_HETRCC_STRONG_AFFINITY)
             pulHccId      异构算力簇 ID (编号越大算力越强)
** 输　出  : ERROR CODE
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
LW_API
ULONG  API_ThreadGetHetrcc (LW_OBJECT_HANDLE  ulId, INT  *piMode, ULONG  *pulHccId)
{
#if LW_CFG_CPU_ARCH_HETRC > 0
    REGISTER UINT16         usIndex;
    REGISTER PLW_CLASS_TCB  ptcb;

    usIndex = _ObjectGetIndex(ulId);

#if LW_CFG_ARG_CHK_EN > 0
    if (!_ObjectClassOK(ulId, _OBJECT_THREAD)) {                        /*  检查 ID 类型有效性          */
        _ErrorHandle(ERROR_KERNEL_HANDLE_NULL);
        return  (ERROR_KERNEL_HANDLE_NULL);
    }
    if (_Thread_Index_Invalid(usIndex)) {                               /*  检查线程有效性              */
        _ErrorHandle(ERROR_KERNEL_HANDLE_NULL);
        return  (ERROR_KERNEL_HANDLE_NULL);
    }
#endif

    __KERNEL_ENTER();                                                   /*  进入内核                    */
    if (_Thread_Invalid(usIndex)) {
        __KERNEL_EXIT();                                                /*  退出内核                    */
        _ErrorHandle(ERROR_KERNEL_HANDLE_NULL);
        return  (ERROR_KERNEL_HANDLE_NULL);
    }

    ptcb = _K_ptcbTCBIdTable[usIndex];
    _ThreadGetHetrcc(ptcb, piMode, pulHccId);
    __KERNEL_EXIT();                                                    /*  退出内核                    */

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
