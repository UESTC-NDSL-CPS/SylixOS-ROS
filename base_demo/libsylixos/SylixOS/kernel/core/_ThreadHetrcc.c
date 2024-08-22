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
** 文   件   名: _ThreadHetrcc.c
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2023 年 06 月 07 日
**
** 描        述: SMP 系统异构算力簇调度模型.
**
** 注        意: 当某算力簇停止运行时, 如果存在强算力簇亲和的锁定线程, 则线程自动解锁.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "../SylixOS/kernel/include/k_kernel.h"
/*********************************************************************************************************
  裁剪支持
*********************************************************************************************************/
#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0)
/*********************************************************************************************************
** 函数名称: _ThreadSetHetrcc
** 功能描述: 设置指定的线程异构算力簇调度参数. (进入内核被调用)
** 输　入  : ptcb          线程控制块
**           iMode         模式 (LW_HETRCC_NON_AFFINITY,
                                 LW_HETRCC_WEAK_AFFINITY,
                                 LW_HETRCC_STRONG_AFFINITY)
             ulHccId       异构算力簇 ID (编号越大算力越强)
** 输　出  : NONE
** 全局变量:
** 调用模块:
** 注  意  : 则外层必须保证非本线程目标线程没有就绪或运行.
*********************************************************************************************************/
VOID  _ThreadSetHetrcc (PLW_CLASS_TCB  ptcb, INT  iMode, ULONG  ulHccId)
{
    INTREG         iregInterLevel;
    BOOL           bReReady;
    PLW_CLASS_TCB  ptcbCur;
    PLW_CLASS_PCB  ppcb;
    PLW_CLASS_CPU  pcpu;

    LW_TCB_GET_CUR(ptcbCur);

    if ((ptcbCur != ptcb) || (iMode == LW_HETRCC_NON_AFFINITY)) {       /*  其他任务或者当前自由调度    */
        ptcb->TCB_iHetrccMode  = iMode;
        ptcb->TCB_ulHetrccLock = ulHccId;                               /*  设置算力簇                  */
        return;
    }

    bReReady = LW_FALSE;
    if ((ptcbCur->TCB_iHetrccMode  != iMode) ||
        (ptcbCur->TCB_ulHetrccLock != ulHccId)) {                       /*  与设置不匹配                */
        bReReady = LW_TRUE;                                             /*  弱亲和也执行此操作          */
                                                                        /*  尝试一次候选表选择          */
    } else if (iMode == LW_HETRCC_WEAK_AFFINITY) {                      /*  弱亲和重复调用              */
        pcpu = LW_CPU_GET(ptcbCur->TCB_ulCPUId);
        if (pcpu->CPU_ulHccId != ulHccId) {                             /*  未在弱亲和算力簇上运行      */
            bReReady = LW_TRUE;                                         /*  再尝试一次                  */
        }
    }

    if (bReReady) {                                                     /*  重置就绪表                  */
        iregInterLevel = KN_INT_DISABLE();
        ppcb = _GetPcb(ptcbCur);
        __DEL_FROM_READY_RING(ptcbCur, ppcb);                           /*  从就绪表中删除              */

        ptcbCur->TCB_iHetrccMode  = iMode;
        ptcbCur->TCB_ulHetrccLock = ulHccId;                            /*  设置算力簇                  */

        ptcbCur->TCB_ucSchedActivate = LW_SCHED_ACT_INTERRUPT;          /*  中断激活方式                */
        ppcb = _GetPcb(ptcbCur);
        __ADD_TO_READY_RING(ptcbCur, ppcb);                             /*  加入到相对优先级就绪环      */
        KN_INT_ENABLE(iregInterLevel);
    }
}
/*********************************************************************************************************
** 函数名称: _ThreadGetHetrcc
** 功能描述: 获取线程异构算力簇调度参数.  (进入内核被调用)
** 输　入  : ptcb          线程控制块
**           piMode        模式 (LW_HETRCC_NON_AFFINITY,
                                 LW_HETRCC_WEAK_AFFINITY,
                                 LW_HETRCC_STRONG_AFFINITY)
             pulHccId      异构算力簇 ID (编号越大算力越强)
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  _ThreadGetHetrcc (PLW_CLASS_TCB  ptcb, INT  *piMode, ULONG  *pulHccId)
{
    if (piMode) {
        *piMode = ptcb->TCB_iHetrccMode;
    }

    if (pulHccId) {
        *pulHccId = ptcb->TCB_ulHetrccLock;
    }
}
/*********************************************************************************************************
** 函数名称: _ThreadOffHetrcc
** 功能描述: 将与指定异构算力簇有强亲和度设置的线程全部关闭异构算力簇亲和度调度. (进入内核被调用)
** 输　入  : pcpu          CPU
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_SMP_CPU_DOWN_EN > 0

VOID  _ThreadOffHetrcc (ULONG  ulHccId)
{
             INTREG         iregInterLevel;
    REGISTER PLW_CLASS_TCB  ptcb;
    REGISTER PLW_CLASS_PCB  ppcb;
    REGISTER PLW_LIST_LINE  plineList;

    for (plineList  = _K_plineTCBHeader;
         plineList != LW_NULL;
         plineList  = _list_line_get_next(plineList)) {                 /*  遍历线程                    */

        iregInterLevel = KN_INT_DISABLE();                              /*  关闭中断                    */
        ptcb = _LIST_ENTRY(plineList, LW_CLASS_TCB, TCB_lineManage);
        if ((ptcb->TCB_iHetrccMode  == LW_HETRCC_STRONG_AFFINITY) &&
            (ptcb->TCB_ulHetrccLock == ulHccId)) {
            if (__LW_THREAD_IS_READY(ptcb)) {
                if (ptcb->TCB_bIsCand) {
                    ptcb->TCB_iHetrccMode = LW_HETRCC_NON_AFFINITY;     /*  设置为自由调度              */

                } else {
                    ppcb = LW_HCC_RDY_PPCB(ptcb->TCB_ulHetrccLock,
                                           ptcb->TCB_ucPriority);
                    _DelTCBFromReadyRing(ptcb, ppcb);                   /*  从 CPU 私有就绪表中退出     */
                    if (_PcbIsEmpty(ppcb)) {
                        __DEL_RDY_MAP(ptcb);
                    }

                    ptcb->TCB_iHetrccMode = LW_HETRCC_NON_AFFINITY;     /*  设置为自由调度              */
                    ppcb = LW_GLOBAL_RDY_PPCB(ptcb->TCB_ucPriority);
                    _AddTCBToReadyRing(ptcb, ppcb, LW_FALSE);           /*  加入全局就绪表              */
                    if (_PcbIsOne(ppcb)) {
                        __ADD_RDY_MAP(ptcb);
                    }
                }
            } else {
                ptcb->TCB_iHetrccMode = LW_HETRCC_NON_AFFINITY;         /*  设置为自由调度              */
            }
        }
        KN_INT_ENABLE(iregInterLevel);                                  /*  打开中断                    */
    }
}

#endif                                                                  /*  LW_CFG_SMP_CPU_DOWN_EN > 0  */
/*********************************************************************************************************
** 函数名称: _ThreadTickHetrcc
** 功能描述: 线程记录算力使用情况. (进入内核被调用)
** 输　入  : ptcb      任务控制块
**           pcpu      CPU 控制块
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  _ThreadTickHetrcc (PLW_CLASS_TCB  ptcb, PLW_CLASS_CPU  pcpu)
{
    PLW_CLASS_HCCCPU  phcccpu;

    phcccpu = LW_HCCCPU_GET(pcpu->CPU_ulHccId);
    if (phcccpu->HCCCPU_uiComputPow) {
        ptcb->TCB_ulComputPow += (ULONG)phcccpu->HCCCPU_uiComputPow;    /*  当前线程记录算力使用情况    */
    }
}
/*********************************************************************************************************
** 函数名称: _ThreadClearHetrcc
** 功能描述: 线程清除记录算力信息. (进入内核被调用)
** 输　入  : ptcb      任务控制块 (NULL 表示清除所有任务)
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  _ThreadClearHetrcc (PLW_CLASS_TCB  ptcb)
{
    REGISTER PLW_LIST_LINE  pline;

    if (ptcb) {
        ptcb->TCB_ulComputPow = 0ul;
        return;
    }

    for (pline  = _K_plineTCBHeader;
         pline != NULL;
         pline  = _list_line_get_next(pline)) {

        ptcb = _LIST_ENTRY(pline, LW_CLASS_TCB, TCB_lineManage);
        ptcb->TCB_ulComputPow = 0ul;
    }
}

#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */
/*********************************************************************************************************
  END
*********************************************************************************************************/
