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
** 文   件   名: _CandTable.c
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2011 年 12 月 05 日
**
** 描        述: 这是系统内核候选运行表操作。(将 k_sched.h 中的 inline 函数抽象到此)

** BUG:
2013.07.29  此文件改名为 _CandTable.c 表示候选运行表操作.
2014.01.10  将候选表放入 CPU 结构中.
2015.04.22  加入 _CandTableResel() 提高运算速度.
2017.10.31  当 _CandTableUpdate() 从一个核中移除一个任务时, 这个任务没有绑定 CPU, 则应该在其他 CPU 上尝试
            调度.
2018.06.06  支持强亲和度调度.
2022.06.06  优化超线程处理器调度.
2023.06.06  支持异构算力簇调度.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "../SylixOS/kernel/include/k_kernel.h"
/*********************************************************************************************************
  异构算力弱亲和时, 是否还尝试标记其他非簇 CPU
*********************************************************************************************************/
#define LW_HETRCC_WAFFINITY_TRY_OTHER   0
/*********************************************************************************************************
  异构算力弱亲和时, 允许重试的最大次数 (数值越大代表 Weak 亲和度越高)
*********************************************************************************************************/
#define LW_HETRCC_WIELD_MAX_DEPTH   8
/*********************************************************************************************************
  异构算力弱亲和时保存 weak 候选
*********************************************************************************************************/
typedef struct {
    PLW_CLASS_TCB   ptcb;                                               /*  Weak 任务控制块             */
    PLW_CLASS_PCB   ppcb;                                               /*  Weak 任务优先级控制块       */
} LW_HETRCC_WEAK_TASK;
/*********************************************************************************************************
  异构算力弱亲和时 weak 重试次数 (1 ~ LW_HETRCC_WIELD_MAX_DEPTH)
*********************************************************************************************************/
#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0)
static INT  _k_iWieldMaxTry = 1;
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */
/*********************************************************************************************************
** 函数名称: _CandTableSeek
** 功能描述: 调度器查询有就绪线程的最高优先级内联函数.
** 输　入  : pcpu              CPU
**           pucPriority       优先级返回值
** 输　出  : 就绪表.
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static PLW_CLASS_PCBBMAP  _CandTableSeek (PLW_CLASS_CPU  pcpu, UINT8 *pucPriority)
{
#if LW_CFG_SMP_EN > 0
    REGISTER UINT               uiCpu, uiGlobal;
    REGISTER PLW_CLASS_PCBBMAP  pcbbmapCpu, pcbbmapGlobal;

#if LW_CFG_CPU_ARCH_HETRC > 0
    REGISTER UINT               uiHcc;
    REGISTER PLW_CLASS_PCBBMAP  pcbbmapHcc;
#endif
    
    pcbbmapCpu = LW_CPU_RDY_PCBBMAP(pcpu);
    if (LW_CPU_ONLY_AFFINITY_GET(pcpu)) {                               /*  仅运行亲和度任务            */
        if (_BitmapIsEmpty(&pcbbmapCpu->PCBM_bmap)) {
            return  (LW_NULL);                                          /*  无亲和度任务                */
        } else {
            *pucPriority = _BitmapHigh(&pcbbmapCpu->PCBM_bmap);
            return  (pcbbmapCpu);                                       /*  仅返回 CPU 亲和度调度表     */
        }
    }

    if (_BitmapIsEmpty(&pcbbmapCpu->PCBM_bmap)) {
        uiCpu = LW_PRIO_INVALID;                                        /*  无亲和度任务就绪            */
    } else {
        uiCpu = _BitmapHigh(&pcbbmapCpu->PCBM_bmap);                    /*  记录就绪亲和度优先级        */
    }

#if LW_CFG_CPU_ARCH_HETRC > 0
    pcbbmapHcc = LW_HCC_RDY_PCBBMAP(pcpu->CPU_ulHccId);
    if (_BitmapIsEmpty(&pcbbmapHcc->PCBM_bmap)) {
        uiHcc = LW_PRIO_INVALID;                                        /*  无算力簇亲和任务就绪        */
    } else {
        uiHcc = _BitmapHigh(&pcbbmapHcc->PCBM_bmap);                    /*  记录就绪算力簇亲和优先级    */
    }
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */

    pcbbmapGlobal = LW_GLOBAL_RDY_PCBBMAP();
    if (_BitmapIsEmpty(&pcbbmapGlobal->PCBM_bmap)) {
        uiGlobal = LW_PRIO_INVALID;                                     /*  无全局亲和任务就绪          */
    } else {
        uiGlobal = _BitmapHigh(&pcbbmapGlobal->PCBM_bmap);              /*  记录就绪全局优先级          */
    }

#if LW_CFG_CPU_ARCH_HETRC > 0
    if (LW_PRIO_IS_HIGH(uiHcc, uiCpu)) {
        uiCpu      = uiHcc;
        pcbbmapCpu = pcbbmapHcc;                                        /*  选择算力簇与全局进行比较    */
    }
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */
    
    if ((uiCpu == LW_PRIO_INVALID) &&
        (uiGlobal == LW_PRIO_INVALID)) {                                /*  无任何有效就绪任务          */
        return  (LW_NULL);
    }

    if (LW_PRIO_IS_HIGH_OR_EQU(uiCpu, uiGlobal)) {
        *pucPriority = (UINT8)uiCpu;
        return  (pcbbmapCpu);                                           /*  选择亲和就绪任务            */
    } else {
        *pucPriority = (UINT8)uiGlobal;
        return  (pcbbmapGlobal);                                        /*  选择全局就绪任务            */
    }
    
#else
    if (_BitmapIsEmpty(LW_GLOBAL_RDY_BMAP())) {                         /*  就绪表中无任务              */
        return  (LW_NULL);
    } else {
        *pucPriority = _BitmapHigh(LW_GLOBAL_RDY_BMAP());
        return  (LW_GLOBAL_RDY_PCBBMAP());
    }
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
}
/*********************************************************************************************************
** 函数名称: _CandTableNext
** 功能描述: 从就绪表中确定一个最需运行的线程.
** 输　入  : ppcbbmap          就绪表
**           ucPriority        优先级
** 输　出  : 在就绪表中最需要运行的线程.
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static PLW_CLASS_TCB  _CandTableNext (PLW_CLASS_PCBBMAP  ppcbbmap, UINT8  ucPriority)
{
    REGISTER PLW_CLASS_PCB      ppcb;
    REGISTER PLW_CLASS_TCB      ptcb;
    
    ppcb = &ppcbbmap->PCBM_pcb[ucPriority];
    ptcb = _LIST_ENTRY(ppcb->PCB_pringReadyHeader, 
                       LW_CLASS_TCB, TCB_ringReady);                    /*  从就绪环中取出一个线程      */
    
    if (ptcb->TCB_usSchedCounter == 0) {                                /*  缺少时间片                  */
        ptcb->TCB_usSchedCounter = ptcb->TCB_usSchedSlice;              /*  补充时间片                  */
        _list_ring_next(&ppcb->PCB_pringReadyHeader);                   /*  下一个                      */
        ptcb = _LIST_ENTRY(ppcb->PCB_pringReadyHeader, 
                           LW_CLASS_TCB, TCB_ringReady);
    }
    
    return  (ptcb);
}
/*********************************************************************************************************
** 函数名称: _CandTablePeek
** 功能描述: 从就绪表中确定一个最需运行的线程.
** 输　入  : pcpu              CPU 结构
**           pppcbPeek         优先级控制块
** 输　出  : 在就绪表中最需要运行的线程.
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static PLW_CLASS_TCB  _CandTablePeek (PLW_CLASS_CPU  pcpu, PLW_CLASS_PCB  *pppcbPeek)
{
    REGISTER PLW_CLASS_TCB      ptcb;
    REGISTER PLW_CLASS_PCBBMAP  ppcbbmap;
             UINT8              ucPriority;

    ppcbbmap = _CandTableSeek(pcpu, &ucPriority);
    if (LW_UNLIKELY(ppcbbmap == LW_NULL)) {
        return  (LW_NULL);                                              /*  无有效任务                  */
    }

    ptcb       = _CandTableNext(ppcbbmap, ucPriority);                  /*  确定可以候选运行的线程      */
    *pppcbPeek = &ppcbbmap->PCBM_pcb[ucPriority];

    return  (ptcb);
}
/*********************************************************************************************************
** 函数名称: _CandTableWieldSticky
** 功能描述: 设置调度器 weak 亲和度粘滞系数
** 输　入  : iWieldMaxTry   粘滞系数 (1 ~ 4)
** 输　出  : 最新的粘滞系数
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0)

INT  _CandTableWieldSticky (INT  iWieldSticky)
{
    if ((iWieldSticky > 0) && (iWieldSticky <= LW_HETRCC_WIELD_MAX_DEPTH)) {
        _k_iWieldMaxTry = iWieldSticky;
    }

    return  (_k_iWieldMaxTry);
}
/*********************************************************************************************************
** 函数名称: _CandTableWield
** 功能描述: 是否可以将 weak 任务重新放回就绪表 yield (之前至少有一个 CPU 被调度标注)
** 输　入  : ulHccId           弱亲和的算力簇
**           ucPrio            优先级
** 输　出  : 是否重新放回就绪表
** 全局变量:
** 调用模块:
** 注  意  : 如果弱亲和的簇没有运行机会, 同时存在中间等级算力簇资源, 这里并没有尝试降级,
**           这样的处理过于复杂, 将影响整个调度器的工作效率. 未来尝试在 xhcesa 内核模块中实现.
*********************************************************************************************************/
static BOOL  _CandTableWield (ULONG  ulHccId, UINT8  ucPrio)
{
             ULONG            i;
    REGISTER PLW_CLASS_TCB    ptcb;
    REGISTER PLW_CLASS_CPU    pcpu;
    REGISTER PLW_CLASS_HCCCPU phcccpu;

    phcccpu = LW_HCCCPU_GET(ulHccId);
    if (LW_UNLIKELY(phcccpu->HCCCPU_uiActive == 0)) {
        return  (LW_FALSE);                                             /*  期望的算力簇没有激活的 CPU  */
    }

    LW_CPU_FOREACH_ACTIVE (i) {
        pcpu = LW_CPU_GET(i);
        if (LW_CPU_ONLY_AFFINITY_GET(pcpu)) {                           /*  设置了强亲和度调度          */
            continue;
        }

        if (pcpu->CPU_ulHccId == ulHccId) {
            ptcb = LW_CAND_TCB(pcpu);
            if (ptcb && LW_PRIO_IS_HIGH(ucPrio, ptcb->TCB_ucPriority)) {
                LW_CAND_ROT(pcpu) = LW_TRUE;                            /*  可以进行目标抢占            */
                return  (LW_TRUE);
            }
        }
    }

    return  (LW_FALSE);
}

#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */
/*********************************************************************************************************
** 函数名称: _CandTableSelect
** 功能描述: 选择一个合适的任务准备运行
** 输　入  : pcpu              当前 CPU
**           pppcb             合适任务优先级控制块
**           pweakt            中间可能产生的 weak yield 任务
** 输　出  : 合适的任务控制块
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static PLW_CLASS_TCB  _CandTableSelect (PLW_CLASS_CPU  pcpu, PLW_CLASS_PCB  *pppcb)
{
    REGISTER PLW_CLASS_TCB  ptcb;

#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0)
    INT                     widx = 0;
    LW_HETRCC_WEAK_TASK     weakt[LW_HETRCC_WIELD_MAX_DEPTH];
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */

    ptcb = _CandTablePeek(pcpu, pppcb);                                 /*  获取一个最需要运行的任务    */
    if (LW_UNLIKELY(ptcb == LW_NULL)) {
        return  (LW_NULL);                                              /*  没有可运行任务              */
    }

#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0)
    if (LW_KERN_SMP_FSCHED_EN_GET()) {                                  /*  快速调度, 直接返回即可      */
        return  (ptcb);
    }

    do {
        if ((ptcb->TCB_iHetrccMode  == LW_HETRCC_WEAK_AFFINITY) &&      /*  此任务弱亲和别的算力簇      */
            (ptcb->TCB_ulHetrccLock != pcpu->CPU_ulHccId) &&
            _CandTableWield(ptcb->TCB_ulHetrccLock,
                            ptcb->TCB_ucPriority)) {                    /*  可以执行 weak yield 操作    */

            _DelTCBFromReadyRing(ptcb, *pppcb);                         /*  从就绪环中退出              */
            if (_PcbIsEmpty(*pppcb)) {
                __DEL_RDY_MAP(ptcb);                                    /*  从就绪表中删除              */
            }

            weakt[widx].ptcb =  ptcb;                                   /*  记录                        */
            weakt[widx].ppcb = *pppcb;
            widx++;
            ptcb = _CandTablePeek(pcpu, pppcb);                         /*  再获取一个最需要运行的任务  */

        } else {
            break;                                                      /*  直接跳出                    */
        }
    } while ((widx < _k_iWieldMaxTry) && ptcb);

    for (--widx; widx >= 0; widx--) {
        _AddTCBToReadyRing(weakt[widx].ptcb, weakt[widx].ppcb, LW_TRUE);
        if (_PcbIsOne(weakt[widx].ppcb)) {
            __ADD_RDY_MAP(weakt[widx].ptcb);                            /*  加入全局就绪表              */
        }
    }
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */

    return  (ptcb);
}
/*********************************************************************************************************
** 函数名称: _CandTableFill
** 功能描述: 选择一个最该执行线程放入候选表.
** 输　入  : pcpu          CPU 结构
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static VOID  _CandTableFill (PLW_CLASS_CPU   pcpu)
{
    REGISTER PLW_CLASS_TCB          ptcb;
             PLW_CLASS_PCB          ppcb;

    ptcb = _CandTableSelect(pcpu, &ppcb);                               /*  获取一个最需要运行的任务    */
    _BugHandle((ptcb == LW_NULL), LW_TRUE, "serious error!\r\n");       /*  至少存在一个任务            */

    LW_CAND_TCB(pcpu) = ptcb;                                           /*  保存新的可执行线程          */
    ptcb->TCB_ulCPUId = LW_CPU_GET_ID(pcpu);                            /*  记录 CPU 号                 */
    ptcb->TCB_bIsCand = LW_TRUE;                                        /*  进入候选运行表              */

    _DelTCBFromReadyRing(ptcb, ppcb);                                   /*  从就绪环中退出              */
    if (_PcbIsEmpty(ppcb)) {
        __DEL_RDY_MAP(ptcb);                                            /*  从就绪表中删除              */
    }
}
/*********************************************************************************************************
** 函数名称: _CandTableEmpty
** 功能描述: 将候选表中的候选线程放入就绪表.
** 输　入  : pcpu      CPU 结构
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static VOID  _CandTableEmpty (PLW_CLASS_CPU   pcpu)
{
    REGISTER PLW_CLASS_TCB  ptcb;
    REGISTER PLW_CLASS_PCB  ppcb;
    
    ptcb = LW_CAND_TCB(pcpu);
    ppcb = _GetPcb(ptcb);
    ptcb->TCB_bIsCand = LW_FALSE;
                                                                        /*  重新加入就绪环              */
    _AddTCBToReadyRing(ptcb, ppcb, LW_TRUE);                            /*  插入就绪环头, 下次优先调度  */
    if (_PcbIsOne(ppcb)) {
        __ADD_RDY_MAP(ptcb);                                            /*  当前线程被抢占, 重回就绪表  */
    }
    
    LW_CAND_TCB(pcpu) = LW_NULL;
}
/*********************************************************************************************************
** 函数名称: _CandTableTryHot
** 功能描述: 试图将一个就绪线程装入之前运行的 CPU
** 输　入  : ptcb          就绪的线程
**           pcpu          指定的 CPU
** 输　出  : 是否加入了候选运行表
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_SMP_EN > 0

static BOOL  _CandTableTryHot (PLW_CLASS_TCB  ptcb, PLW_CLASS_CPU  pcpu)
{
    REGISTER BOOL            bRotIdle = LW_FALSE;
    REGISTER PLW_CLASS_TCB   ptcbCand;

#if LW_CFG_SMP_CPU_DOWN_EN > 0
    if (!LW_CPU_IS_ACTIVE(pcpu)) {
        return  (bRotIdle);
    }
#endif                                                                  /*  LW_CFG_SMP_CPU_DOWN_EN      */

    if (LW_CPU_ONLY_AFFINITY_GET(pcpu)) {
        return  (bRotIdle);                                             /*  仅运行亲和任务              */
    }

    ptcbCand = LW_CAND_TCB(pcpu);
    if (LW_UNLIKELY(ptcbCand == LW_NULL)) {
        LW_CAND_TCB(pcpu) = ptcb;
        ptcb->TCB_bIsCand = LW_TRUE;                                    /*  进入候选运行表              */
        return  (LW_TRUE);
    }

    if (!LW_CAND_ROT(pcpu) &&
        (LW_PRIO_IS_EQU(LW_PRIO_LOWEST,
                        ptcbCand->TCB_ucPriority))) {                   /*  之前运行的 CPU 为 idle      */
        LW_CAND_ROT(pcpu) = LW_TRUE;                                    /*  产生优先级卷绕              */
        bRotIdle          = LW_TRUE;
    }

    return  (bRotIdle);
}
/*********************************************************************************************************
** 函数名称: _CandTableTryIdle
** 功能描述: 试图将一个就绪线程装入其他合适的 IDLE CPU
** 输　入  : ptcb          就绪的线程
**           pcpuExcept    需要排除的 CPU
**           ppcpuIdle     探测出 IDLE 的 CPU ID
**           iMode         0: ALL 1: 仅判断异构算力簇 2: 不判断异构算力簇
** 输　出  : 是否加入了候选运行表
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static BOOL  _CandTableTryIdle (PLW_CLASS_TCB  ptcb,
                                PLW_CLASS_CPU  pcpuExcept,
                                PLW_CLASS_CPU *ppcpuIdle,
                                INT            iMode)
{
    REGISTER ULONG              i;
    REGISTER BOOL               bRotIdle = LW_FALSE;
    REGISTER PLW_CLASS_CPU      pcpu;
    REGISTER PLW_CLASS_TCB      ptcbCand;

#if LW_CFG_CPU_ARCH_SMT > 0
    REGISTER PLW_CLASS_PHYCPU   pphycpu;
#endif                                                                  /*  超线程支持                  */

    LW_CPU_FOREACH_ACTIVE (i) {                                         /*  CPU 必须为激活状态          */
        pcpu = LW_CPU_GET(i);
        if (pcpu == pcpuExcept) {
            continue;                                                   /*  忽略排除的 CPU              */
        }
        if (LW_CPU_ONLY_AFFINITY_GET(pcpu)) {                           /*  设置了强亲和度调度          */
            continue;
        }

#if LW_CFG_CPU_ARCH_HETRC > 0
        if (iMode == 1 &&
            ptcb->TCB_ulHetrccLock != pcpu->CPU_ulHccId) {
            continue;                                                   /*  仅判断异构算力簇            */
        } else if (iMode == 2 &&
                   ptcb->TCB_ulHetrccLock == pcpu->CPU_ulHccId) {
            continue;                                                   /*  不判断异构算力簇            */
        }
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */

        ptcbCand = LW_CAND_TCB(pcpu);
        if (LW_UNLIKELY(ptcbCand == LW_NULL)) {                         /*  候选表为空                  */
            LW_CAND_TCB(pcpu) = ptcb;
            ptcb->TCB_ulCPUId = i;                                      /*  记录 CPU 号                 */
            ptcb->TCB_bIsCand = LW_TRUE;                                /*  进入候选运行表              */
            return  (LW_TRUE);

        } else if (!LW_CAND_ROT(pcpu) &&
                   (LW_PRIO_IS_EQU(LW_PRIO_LOWEST,
                                   ptcbCand->TCB_ucPriority))) {        /*  运行 idle 任务且无标注      */
#if LW_CFG_CPU_ARCH_SMT > 0                                             /*  超线程 CPU 优先选空闲物理核 */
            if (LW_KERN_SMT_BSCHED_EN_GET()) {
                pphycpu = LW_PHYCPU_GET(pcpu->CPU_ulPhyId);
                if (pphycpu->PHYCPU_uiNonIdle == 0) {                   /*  物理核心为 idle             */
                    LW_CAND_ROT(pcpu) = LW_TRUE;
                    bRotIdle          = LW_TRUE;
                    break;                                              /*  只标注一个 CPU 即可         */

                } else if (!(*ppcpuIdle)) {
                    *ppcpuIdle = pcpu;                                  /*  记录一个 idle 核心的位置    */
                }
            } else
#endif                                                                  /*  LW_CFG_CPU_ARCH_SMT > 0     */
            {
                LW_CAND_ROT(pcpu) = LW_TRUE;
                bRotIdle          = LW_TRUE;
                break;                                                  /*  只标注一个 CPU 即可         */
            }
        }
    }

    return  (bRotIdle);
}
/*********************************************************************************************************
** 函数名称: _CandTableTryPreemptive
** 功能描述: 试图将一个就绪线程装入其他合适的 CPU 进行抢占调度
** 输　入  : ptcb          就绪的线程
**           pcpuExcept    需要排除的 CPU
**           bHetrcc       是否判断异构算力簇
** 输　出  : 是否加入了候选运行表
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static BOOL  _CandTableTryPreemptive (PLW_CLASS_TCB  ptcb, PLW_CLASS_CPU  pcpuExcept, BOOL  bHetrcc)
{
    REGISTER ULONG              i;
    REGISTER BOOL               bRotIdle = LW_FALSE;
    REGISTER PLW_CLASS_CPU      pcpu;
    REGISTER PLW_CLASS_TCB      ptcbCand;

    REGISTER BOOL               bFsched = LW_KERN_SMP_FSCHED_EN_GET();
             UINT               uiPrioL = LW_PRIO_HIGHEST;
             PLW_CLASS_CPU      pcpuLow = LW_NULL;

    LW_CPU_FOREACH_ACTIVE (i) {                                         /*  CPU 必须为激活状态          */
        pcpu = LW_CPU_GET(i);
        if (pcpu == pcpuExcept) {
            continue;                                                   /*  忽略排除的 CPU              */
        }
        if (LW_CPU_ONLY_AFFINITY_GET(pcpu)) {                           /*  设置了强亲和度调度          */
            continue;
        }

#if LW_CFG_CPU_ARCH_HETRC > 0
        if (bHetrcc && ptcb->TCB_ulHetrccLock != pcpu->CPU_ulHccId) {   /*  仅判断异构算力簇            */
            continue;
        }
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */

        ptcbCand = LW_CAND_TCB(pcpu);
        if (LW_UNLIKELY(ptcbCand == LW_NULL)) {                         /*  候选表为空                  */
            LW_CAND_TCB(pcpu) = ptcb;
            ptcb->TCB_ulCPUId = i;                                      /*  记录 CPU 号                 */
            ptcb->TCB_bIsCand = LW_TRUE;                                /*  进入候选运行表              */
            return  (LW_TRUE);

        } else if (LW_PRIO_IS_HIGH(ptcb->TCB_ucPriority,
                                   ptcbCand->TCB_ucPriority)) {

            if (bFsched) {                                              /*  Fast Sched                  */
                LW_CAND_ROT(pcpu) = LW_TRUE;                            /*  产生优先级卷绕              */
                bRotIdle          = LW_TRUE;

            } else if (LW_PRIO_IS_HIGH(uiPrioL,
                                       ptcbCand->TCB_ucPriority)) {     /*  仅尝试抢占最低优先级任务    */
                uiPrioL = ptcbCand->TCB_ucPriority;
                pcpuLow = pcpu;                                         /*  记录最低优先级 CPU          */
            }
        }
    }

    if (!bFsched && pcpuLow) {                                          /*  仅尝试抢占最低优先级任务    */
        LW_CAND_ROT(pcpuLow) = LW_TRUE;                                 /*  产生优先级卷绕              */
        bRotIdle             = LW_TRUE;
    }

    return  (bRotIdle);
}
/*********************************************************************************************************
** 函数名称: _CandTableTryAll
** 功能描述: 试图将一个自由调度的就绪线程装入其他合适的 CPU
** 输　入  : ptcb              就绪的线程
**           bOtherExceptMe    是否在 TryPreemptive 阶段排除 ptcb 之前运行的 CPU
**           bHasTryHot        是否不需要 try hot cpu
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  _CandTableTryAll (PLW_CLASS_TCB  ptcb, BOOL  bOtherExceptMe, BOOL  bHasTryHot)
{
             INT             iMode;
    REGISTER BOOL            bRotIdle = LW_FALSE;
    REGISTER PLW_CLASS_CPU   pcpu;
             PLW_CLASS_CPU   pcpuIdle;

    pcpu = LW_CPU_GET(ptcb->TCB_ulCPUId);                               /*  保持 CACHE 热度             */
    if (!bHasTryHot) {
        bRotIdle = _CandTableTryHot(ptcb, pcpu);                        /*  首先尝试此 CPU              */
    }

    if (!bRotIdle) {
#if LW_CFG_CPU_ARCH_HETRC > 0
        iMode = ptcb->TCB_iHetrccMode != LW_HETRCC_NON_AFFINITY ? 2 : 0;
#else
        iMode = 0;
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */

        pcpuIdle = LW_NULL;
        bRotIdle = _CandTableTryIdle(ptcb, pcpu, &pcpuIdle, iMode);     /*  尝试其他 IDLE CPU           */

#if LW_CFG_CPU_ARCH_SMT > 0                                             /*  超线程是否有空闲逻辑核心    */
        if (!bRotIdle && pcpuIdle) {
            LW_CAND_ROT(pcpuIdle) = LW_TRUE;
            bRotIdle              = LW_TRUE;
        }
#endif                                                                  /*  LW_CFG_CPU_ARCH_SMT > 0     */

        if (!bRotIdle) {
            _CandTableTryPreemptive(ptcb,
                                    bOtherExceptMe ? pcpu : LW_NULL,
                                    LW_FALSE);                          /*  尝试其他 CPU                */
        }
    }
}
/*********************************************************************************************************
** 函数名称: _CandTableTryHetrcc
** 功能描述: 试图将一个有异构算力簇亲和的就绪线程装入其他合适的 CPU
** 输　入  : ptcb              就绪的线程
**           bOtherExceptMe    是否在 TryPreemptive 阶段排除 ptcb 之前运行的 CPU
**           pbHasTryHot       是否已经 try hot cpu
** 输　出  : 是否加入了候选运行表
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_CPU_ARCH_HETRC > 0

static BOOL  _CandTableTryHetrcc (PLW_CLASS_TCB  ptcb, BOOL  bOtherExceptMe, BOOL  *pbHasTryHot)
{
    REGISTER BOOL            bRotIdle = LW_FALSE;
    REGISTER PLW_CLASS_CPU   pcpu;
             PLW_CLASS_CPU   pcpuIdle;

    pcpu = LW_CPU_GET(ptcb->TCB_ulCPUId);                               /*  保持 CACHE 热度             */
    if (!(*pbHasTryHot) &&
        pcpu->CPU_ulHccId == ptcb->TCB_ulHetrccLock) {                  /*  复合预期算力簇              */
        bRotIdle     = _CandTableTryHot(ptcb, pcpu);                    /*  首先尝试此 CPU              */
        *pbHasTryHot = LW_TRUE;
    }

    if (!bRotIdle) {                                                    /*  没有尝试成功                */
        pcpuIdle = LW_NULL;
        bRotIdle = _CandTableTryIdle(ptcb, pcpu, &pcpuIdle, 1);         /*  尝试其他 IDLE CPU           */

#if LW_CFG_CPU_ARCH_SMT > 0                                             /*  超线程是否有空闲逻辑核心    */
        if (!bRotIdle && pcpuIdle) {
            LW_CAND_ROT(pcpuIdle) = LW_TRUE;
            bRotIdle              = LW_TRUE;
        }
#endif                                                                  /*  LW_CFG_CPU_ARCH_SMT > 0     */

        if (!bRotIdle) {
            bRotIdle = _CandTableTryPreemptive(ptcb,
                                               bOtherExceptMe ? pcpu : LW_NULL,
                                               LW_TRUE);                /*  尝试其他 CPU                */
        }
    }

#if LW_HETRCC_WAFFINITY_TRY_OTHER > 0
    return  (bRotIdle);
#else
    return  (LW_TRUE);
#endif
}

#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
/*********************************************************************************************************
** 函数名称: _CandTableTryAdd
** 功能描述: 试图将一个就绪线程装入候选线程表.
** 输　入  : ptcb          就绪的线程
**           ppcb          对应的优先级控制块
** 输　出  : 是否加入了候选运行表
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
BOOL  _CandTableTryAdd (PLW_CLASS_TCB  ptcb, PLW_CLASS_PCB  ppcb)
{
    REGISTER PLW_CLASS_CPU   pcpu;
    REGISTER PLW_CLASS_TCB   ptcbCand;
    
#if LW_CFG_SMP_EN > 0                                                   /*  SMP 多核                    */
    if (ptcb->TCB_bCPULock) {                                           /*  任务锁定 CPU                */
        pcpu = LW_CPU_GET(ptcb->TCB_ulCPULock);
        if (!LW_CPU_IS_ACTIVE(pcpu)) {
            goto    __can_not_cand;
        }
        
        ptcbCand = LW_CAND_TCB(pcpu);
        if (LW_UNLIKELY(ptcbCand == LW_NULL)) {                         /*  候选表为空                  */
            LW_CAND_TCB(pcpu) = ptcb;
            ptcb->TCB_ulCPUId = ptcb->TCB_ulCPULock;                    /*  记录 CPU 号                 */
            ptcb->TCB_bIsCand = LW_TRUE;                                /*  进入候选运行表              */
            return  (LW_TRUE);
            
        } else if (LW_PRIO_IS_HIGH(ptcb->TCB_ucPriority, 
                                   ptcbCand->TCB_ucPriority)) {         /*  优先级高于当前候选线程      */
            LW_CAND_ROT(pcpu) = LW_TRUE;                                /*  产生优先级卷绕              */
        }

    } else {                                                            /*  可进行跨核心调度            */
        BOOL  bHasTryHot = LW_FALSE;
        BOOL  bRotIdle   = LW_FALSE;

#if LW_CFG_CPU_ARCH_HETRC > 0
        if (ptcb->TCB_iHetrccMode != LW_HETRCC_NON_AFFINITY) {          /*  异构算力簇调度              */
            if (LW_HCCCPU_GET(ptcb->TCB_ulHetrccLock)->HCCCPU_uiActive) {
                bRotIdle = _CandTableTryHetrcc(ptcb, LW_FALSE, &bHasTryHot);
            }
            if (ptcb->TCB_iHetrccMode == LW_HETRCC_STRONG_AFFINITY) {
                bRotIdle = LW_TRUE;                                     /*  不再进行调度尝试            */
            }
        }
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */

        if (!bRotIdle) {
            _CandTableTryAll(ptcb, LW_FALSE, bHasTryHot);               /*  尝试其他 CPU                */
        }
    }

#else                                                                   /*  单核情况                    */
    pcpu = LW_CPU_GET(0);
    if (!LW_CPU_IS_ACTIVE(pcpu)) {                                      /*  CPU 必须为激活状态          */
        goto    __can_not_cand;
    }
    
    ptcbCand = LW_CAND_TCB(pcpu);
    if (LW_UNLIKELY(ptcbCand == LW_NULL)) {                             /*  候选表为空                  */
__can_cand:
        LW_CAND_TCB(pcpu) = ptcb;
        ptcb->TCB_ulCPUId = 0;                                          /*  记录 CPU 号                 */
        ptcb->TCB_bIsCand = LW_TRUE;                                    /*  进入候选运行表              */
        return  (LW_TRUE);                                              /*  直接加入候选运行表          */
        
    } else if (LW_PRIO_IS_HIGH(ptcb->TCB_ucPriority, 
                               ptcbCand->TCB_ucPriority)) {             /*  优先级比当前候选线程高      */
        if (__THREAD_LOCK_GET(ptcbCand)) {
            LW_CAND_ROT(pcpu) = LW_TRUE;                                /*  产生优先级卷绕              */
        
        } else {
            _CandTableEmpty(pcpu);                                      /*  清空候选表                  */
            goto    __can_cand;
        }
    }
#endif                                                                  /*  LW_CFG_SMP_EN               */

__can_not_cand:
    if (_PcbIsEmpty(ppcb)) {
        __ADD_RDY_MAP(ptcb);                                            /*  将位图的相关位置一          */
    }
    
    return  (LW_FALSE);                                                 /*  无法加入候选运行表          */
}
/*********************************************************************************************************
** 函数名称: _CandTableTryDel
** 功能描述: 试图将一个不再就绪的候选线程从候选表中退出
** 输　入  : ptcb          就绪的线程
**           ppcb          对应的优先级控制块
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID _CandTableTryDel (PLW_CLASS_TCB  ptcb, PLW_CLASS_PCB  ppcb)
{
    REGISTER PLW_CLASS_CPU   pcpu = LW_CPU_GET(ptcb->TCB_ulCPUId);
    
    if (LW_CAND_TCB(pcpu) == ptcb) {                                    /*  在候选表中                  */
        ptcb->TCB_bIsCand = LW_FALSE;                                   /*  退出候选运行表              */
        _CandTableFill(pcpu);                                           /*  重新填充一个候选线程        */
        LW_CAND_ROT(pcpu) = LW_FALSE;                                   /*  清除优先级卷绕标志          */
    
    } else {                                                            /*  没有在候选表中              */
        if (_PcbIsEmpty(ppcb)) {
            __DEL_RDY_MAP(ptcb);                                        /*  将位图的相关位清零          */
        }
    }
}
/*********************************************************************************************************
** 函数名称: _CandTableNotify
** 功能描述: 通知其他 CPU 进行调度查看. (这里仅需要设置卷绕标志, 不需要发送核间中断, 在任务切换间隙会发送)
** 输　入  : ptcb      当前 CPU 候选线程
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
#if LW_CFG_SMP_EN > 0

VOID _CandTableNotify (PLW_CLASS_TCB  ptcb)
{
#if LW_CFG_CPU_ARCH_HETRC > 0
    BOOL  bHasTryHot = LW_TRUE;                                         /*  不尝试本核心调度            */
    BOOL  bRotIdle   = LW_FALSE;

    if (ptcb->TCB_iHetrccMode != LW_HETRCC_NON_AFFINITY) {              /*  异构算力簇调度              */
        if (LW_HCCCPU_GET(ptcb->TCB_ulHetrccLock)->HCCCPU_uiActive) {
            bRotIdle = _CandTableTryHetrcc(ptcb, LW_TRUE, &bHasTryHot);
        }
        if (ptcb->TCB_iHetrccMode == LW_HETRCC_STRONG_AFFINITY) {
            bRotIdle = LW_TRUE;                                         /*  不再进行调度尝试            */
        }
    }

    if (!bRotIdle)
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */
    {
        _CandTableTryAll(ptcb, LW_TRUE, LW_TRUE);                       /*  尝试装入其他 CPU            */
    }
}

#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
/*********************************************************************************************************
** 函数名称: _CandTableUpdate
** 功能描述: 尝试将最高优先级就绪任务装入候选表. 
** 输　入  : pcpu      CPU 结构
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
** 注  意  : 如果新的候选任务为锁定当前 CPU 则其他核没有设置过卷绕标志, 如果同时被换出的任务可以运行在其他
             CPU 上, 则需要设置其他 CPU 的卷绕标志.
*********************************************************************************************************/
VOID _CandTableUpdate (PLW_CLASS_CPU   pcpu)
{
    REGISTER PLW_CLASS_TCB      ptcbCand;
    REGISTER PLW_CLASS_TCB      ptcb;
             PLW_CLASS_PCB      ppcb;
             BOOL               bNeedRotate = LW_FALSE;

    if (!LW_CPU_IS_ACTIVE(pcpu)) {                                      /*  CPU 必须为激活状态          */
        return;
    }

    ptcbCand = LW_CAND_TCB(pcpu);
    if (ptcbCand == LW_NULL) {                                          /*  当前没有候选线程            */
        _CandTableFill(pcpu);
        goto    __update_done;
    }

    ptcb = _CandTableSelect(pcpu, &ppcb);                               /*  获取一个最需要运行的任务    */
    if (LW_UNLIKELY(ptcb == LW_NULL)) {
        goto    __update_done;                                          /*  没有需要运行的任务          */
    }

#if LW_CFG_SMP_EN > 0
    if (LW_CPU_ONLY_AFFINITY_GET(pcpu) && !ptcbCand->TCB_bCPULock) {    /*  强制运行亲和度任务          */
        bNeedRotate = LW_TRUE;                                          /*  当前普通任务需要让出 CPU    */

    } else
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
    {
        if (ptcbCand->TCB_usSchedCounter == 0) {                        /*  已经没有时间片了            */
            if (LW_PRIO_IS_HIGH_OR_EQU(ptcb->TCB_ucPriority,
                                       ptcbCand->TCB_ucPriority)) {     /*  是否需要轮转                */
                bNeedRotate = LW_TRUE;
            }
        } else {
            if (LW_PRIO_IS_HIGH(ptcb->TCB_ucPriority,
                                ptcbCand->TCB_ucPriority)) {
                bNeedRotate = LW_TRUE;
            }
        }

#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0)
        if (!bNeedRotate && !LW_KERN_SMP_FSCHED_EN_GET()) {
            if ((ptcbCand->TCB_iHetrccMode  == LW_HETRCC_WEAK_AFFINITY) &&
                (ptcbCand->TCB_ulHetrccLock != pcpu->CPU_ulHccId) &&    /*  当前任务弱亲和别的算力簇    */
                _CandTableWield(ptcbCand->TCB_ulHetrccLock,
                                ptcbCand->TCB_ucPriority)) {            /*  可以执行 weak yield 操作    */
                bNeedRotate = LW_TRUE;                                  /*  尝试转移到亲和的算力簇      */
            }
        }
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */
    }

    if (bNeedRotate) {                                                  /*  存在更需要运行的线程        */
        _CandTableEmpty(pcpu);                                          /*  清空候选表                  */

        LW_CAND_TCB(pcpu) = ptcb;                                       /*  保存新的可执行线程          */
        ptcb->TCB_ulCPUId = LW_CPU_GET_ID(pcpu);                        /*  记录 CPU 号                 */
        ptcb->TCB_bIsCand = LW_TRUE;                                    /*  进入候选运行表              */

        _DelTCBFromReadyRing(ptcb, ppcb);                               /*  从就绪环中退出              */
        if (_PcbIsEmpty(ppcb)) {
            __DEL_RDY_MAP(ptcb);                                        /*  从就绪表中删除              */
        }

#if LW_CFG_SMP_EN > 0                                                   /*  SMP 多核                    */
        if (!ptcbCand->TCB_bCPULock &&
            (LW_CAND_TCB(pcpu) != ptcbCand)) {                          /*  是否需要尝试标记其他 CPU    */
            _CandTableNotify(ptcbCand);                                 /*  通知其他 CPU 进行调度查看   */
        }
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
    }

__update_done:
    LW_CAND_ROT(pcpu) = LW_FALSE;                                       /*  清除优先级卷绕标志          */
}
/*********************************************************************************************************
** 函数名称: _CandTableRemove
** 功能描述: 将一个 CPU 对应的候选表清除 
** 输　入  : pcpu      CPU 结构
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
#if (LW_CFG_SMP_EN > 0) && (LW_CFG_SMP_CPU_DOWN_EN > 0)

VOID _CandTableRemove (PLW_CLASS_CPU   pcpu)
{
    if (LW_CPU_IS_ACTIVE(pcpu)) {                                       /*  CPU 必须为非激活状态        */
        return;
    }
    
    if (LW_CAND_TCB(pcpu)) {
        _CandTableEmpty(pcpu);
    }
}

#endif                                                                  /*  LW_CFG_SMP_CPU_DOWN_EN > 0  */
/*********************************************************************************************************
  END
*********************************************************************************************************/
