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
** ��   ��   ��: _CandTable.c
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2011 �� 12 �� 05 ��
**
** ��        ��: ����ϵͳ�ں˺�ѡ���б������(�� k_sched.h �е� inline �������󵽴�)

** BUG:
2013.07.29  ���ļ�����Ϊ _CandTable.c ��ʾ��ѡ���б����.
2014.01.10  ����ѡ����� CPU �ṹ��.
2015.04.22  ���� _CandTableResel() ��������ٶ�.
2017.10.31  �� _CandTableUpdate() ��һ�������Ƴ�һ������ʱ, �������û�а� CPU, ��Ӧ�������� CPU �ϳ���
            ����.
2018.06.06  ֧��ǿ�׺Ͷȵ���.
2022.06.06  �Ż����̴߳���������.
2023.06.06  ֧���칹�����ص���.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "../SylixOS/kernel/include/k_kernel.h"
/*********************************************************************************************************
  �칹�������׺�ʱ, �Ƿ񻹳��Ա�������Ǵ� CPU
*********************************************************************************************************/
#define LW_HETRCC_WAFFINITY_TRY_OTHER   0
/*********************************************************************************************************
  �칹�������׺�ʱ, �������Ե������� (��ֵԽ����� Weak �׺Ͷ�Խ��)
*********************************************************************************************************/
#define LW_HETRCC_WIELD_MAX_DEPTH   8
/*********************************************************************************************************
  �칹�������׺�ʱ���� weak ��ѡ
*********************************************************************************************************/
typedef struct {
    PLW_CLASS_TCB   ptcb;                                               /*  Weak ������ƿ�             */
    PLW_CLASS_PCB   ppcb;                                               /*  Weak �������ȼ����ƿ�       */
} LW_HETRCC_WEAK_TASK;
/*********************************************************************************************************
  �칹�������׺�ʱ weak ���Դ��� (1 ~ LW_HETRCC_WIELD_MAX_DEPTH)
*********************************************************************************************************/
#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0)
static INT  _k_iWieldMaxTry = 1;
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */
/*********************************************************************************************************
** ��������: _CandTableSeek
** ��������: ��������ѯ�о����̵߳�������ȼ���������.
** �䡡��  : pcpu              CPU
**           pucPriority       ���ȼ�����ֵ
** �䡡��  : ������.
** ȫ�ֱ���: 
** ����ģ��: 
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
    if (LW_CPU_ONLY_AFFINITY_GET(pcpu)) {                               /*  �������׺Ͷ�����            */
        if (_BitmapIsEmpty(&pcbbmapCpu->PCBM_bmap)) {
            return  (LW_NULL);                                          /*  ���׺Ͷ�����                */
        } else {
            *pucPriority = _BitmapHigh(&pcbbmapCpu->PCBM_bmap);
            return  (pcbbmapCpu);                                       /*  ������ CPU �׺Ͷȵ��ȱ�     */
        }
    }

    if (_BitmapIsEmpty(&pcbbmapCpu->PCBM_bmap)) {
        uiCpu = LW_PRIO_INVALID;                                        /*  ���׺Ͷ��������            */
    } else {
        uiCpu = _BitmapHigh(&pcbbmapCpu->PCBM_bmap);                    /*  ��¼�����׺Ͷ����ȼ�        */
    }

#if LW_CFG_CPU_ARCH_HETRC > 0
    pcbbmapHcc = LW_HCC_RDY_PCBBMAP(pcpu->CPU_ulHccId);
    if (_BitmapIsEmpty(&pcbbmapHcc->PCBM_bmap)) {
        uiHcc = LW_PRIO_INVALID;                                        /*  ���������׺��������        */
    } else {
        uiHcc = _BitmapHigh(&pcbbmapHcc->PCBM_bmap);                    /*  ��¼�����������׺����ȼ�    */
    }
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */

    pcbbmapGlobal = LW_GLOBAL_RDY_PCBBMAP();
    if (_BitmapIsEmpty(&pcbbmapGlobal->PCBM_bmap)) {
        uiGlobal = LW_PRIO_INVALID;                                     /*  ��ȫ���׺��������          */
    } else {
        uiGlobal = _BitmapHigh(&pcbbmapGlobal->PCBM_bmap);              /*  ��¼����ȫ�����ȼ�          */
    }

#if LW_CFG_CPU_ARCH_HETRC > 0
    if (LW_PRIO_IS_HIGH(uiHcc, uiCpu)) {
        uiCpu      = uiHcc;
        pcbbmapCpu = pcbbmapHcc;                                        /*  ѡ����������ȫ�ֽ��бȽ�    */
    }
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */
    
    if ((uiCpu == LW_PRIO_INVALID) &&
        (uiGlobal == LW_PRIO_INVALID)) {                                /*  ���κ���Ч��������          */
        return  (LW_NULL);
    }

    if (LW_PRIO_IS_HIGH_OR_EQU(uiCpu, uiGlobal)) {
        *pucPriority = (UINT8)uiCpu;
        return  (pcbbmapCpu);                                           /*  ѡ���׺;�������            */
    } else {
        *pucPriority = (UINT8)uiGlobal;
        return  (pcbbmapGlobal);                                        /*  ѡ��ȫ�־�������            */
    }
    
#else
    if (_BitmapIsEmpty(LW_GLOBAL_RDY_BMAP())) {                         /*  ��������������              */
        return  (LW_NULL);
    } else {
        *pucPriority = _BitmapHigh(LW_GLOBAL_RDY_BMAP());
        return  (LW_GLOBAL_RDY_PCBBMAP());
    }
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
}
/*********************************************************************************************************
** ��������: _CandTableNext
** ��������: �Ӿ�������ȷ��һ���������е��߳�.
** �䡡��  : ppcbbmap          ������
**           ucPriority        ���ȼ�
** �䡡��  : �ھ�����������Ҫ���е��߳�.
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static PLW_CLASS_TCB  _CandTableNext (PLW_CLASS_PCBBMAP  ppcbbmap, UINT8  ucPriority)
{
    REGISTER PLW_CLASS_PCB      ppcb;
    REGISTER PLW_CLASS_TCB      ptcb;
    
    ppcb = &ppcbbmap->PCBM_pcb[ucPriority];
    ptcb = _LIST_ENTRY(ppcb->PCB_pringReadyHeader, 
                       LW_CLASS_TCB, TCB_ringReady);                    /*  �Ӿ�������ȡ��һ���߳�      */
    
    if (ptcb->TCB_usSchedCounter == 0) {                                /*  ȱ��ʱ��Ƭ                  */
        ptcb->TCB_usSchedCounter = ptcb->TCB_usSchedSlice;              /*  ����ʱ��Ƭ                  */
        _list_ring_next(&ppcb->PCB_pringReadyHeader);                   /*  ��һ��                      */
        ptcb = _LIST_ENTRY(ppcb->PCB_pringReadyHeader, 
                           LW_CLASS_TCB, TCB_ringReady);
    }
    
    return  (ptcb);
}
/*********************************************************************************************************
** ��������: _CandTablePeek
** ��������: �Ӿ�������ȷ��һ���������е��߳�.
** �䡡��  : pcpu              CPU �ṹ
**           pppcbPeek         ���ȼ����ƿ�
** �䡡��  : �ھ�����������Ҫ���е��߳�.
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static PLW_CLASS_TCB  _CandTablePeek (PLW_CLASS_CPU  pcpu, PLW_CLASS_PCB  *pppcbPeek)
{
    REGISTER PLW_CLASS_TCB      ptcb;
    REGISTER PLW_CLASS_PCBBMAP  ppcbbmap;
             UINT8              ucPriority;

    ppcbbmap = _CandTableSeek(pcpu, &ucPriority);
    if (LW_UNLIKELY(ppcbbmap == LW_NULL)) {
        return  (LW_NULL);                                              /*  ����Ч����                  */
    }

    ptcb       = _CandTableNext(ppcbbmap, ucPriority);                  /*  ȷ�����Ժ�ѡ���е��߳�      */
    *pppcbPeek = &ppcbbmap->PCBM_pcb[ucPriority];

    return  (ptcb);
}
/*********************************************************************************************************
** ��������: _CandTableWieldSticky
** ��������: ���õ����� weak �׺Ͷ�ճ��ϵ��
** �䡡��  : iWieldMaxTry   ճ��ϵ�� (1 ~ 4)
** �䡡��  : ���µ�ճ��ϵ��
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: _CandTableWield
** ��������: �Ƿ���Խ� weak �������·Żؾ����� yield (֮ǰ������һ�� CPU �����ȱ�ע)
** �䡡��  : ulHccId           ���׺͵�������
**           ucPrio            ���ȼ�
** �䡡��  : �Ƿ����·Żؾ�����
** ȫ�ֱ���:
** ����ģ��:
** ע  ��  : ������׺͵Ĵ�û�����л���, ͬʱ�����м�ȼ���������Դ, ���ﲢû�г��Խ���,
**           �����Ĵ�����ڸ���, ��Ӱ�������������Ĺ���Ч��. δ�������� xhcesa �ں�ģ����ʵ��.
*********************************************************************************************************/
static BOOL  _CandTableWield (ULONG  ulHccId, UINT8  ucPrio)
{
             ULONG            i;
    REGISTER PLW_CLASS_TCB    ptcb;
    REGISTER PLW_CLASS_CPU    pcpu;
    REGISTER PLW_CLASS_HCCCPU phcccpu;

    phcccpu = LW_HCCCPU_GET(ulHccId);
    if (LW_UNLIKELY(phcccpu->HCCCPU_uiActive == 0)) {
        return  (LW_FALSE);                                             /*  ������������û�м���� CPU  */
    }

    LW_CPU_FOREACH_ACTIVE (i) {
        pcpu = LW_CPU_GET(i);
        if (LW_CPU_ONLY_AFFINITY_GET(pcpu)) {                           /*  ������ǿ�׺Ͷȵ���          */
            continue;
        }

        if (pcpu->CPU_ulHccId == ulHccId) {
            ptcb = LW_CAND_TCB(pcpu);
            if (ptcb && LW_PRIO_IS_HIGH(ucPrio, ptcb->TCB_ucPriority)) {
                LW_CAND_ROT(pcpu) = LW_TRUE;                            /*  ���Խ���Ŀ����ռ            */
                return  (LW_TRUE);
            }
        }
    }

    return  (LW_FALSE);
}

#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */
/*********************************************************************************************************
** ��������: _CandTableSelect
** ��������: ѡ��һ�����ʵ�����׼������
** �䡡��  : pcpu              ��ǰ CPU
**           pppcb             �����������ȼ����ƿ�
**           pweakt            �м���ܲ����� weak yield ����
** �䡡��  : ���ʵ�������ƿ�
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static PLW_CLASS_TCB  _CandTableSelect (PLW_CLASS_CPU  pcpu, PLW_CLASS_PCB  *pppcb)
{
    REGISTER PLW_CLASS_TCB  ptcb;

#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0)
    INT                     widx = 0;
    LW_HETRCC_WEAK_TASK     weakt[LW_HETRCC_WIELD_MAX_DEPTH];
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */

    ptcb = _CandTablePeek(pcpu, pppcb);                                 /*  ��ȡһ������Ҫ���е�����    */
    if (LW_UNLIKELY(ptcb == LW_NULL)) {
        return  (LW_NULL);                                              /*  û�п���������              */
    }

#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0)
    if (LW_KERN_SMP_FSCHED_EN_GET()) {                                  /*  ���ٵ���, ֱ�ӷ��ؼ���      */
        return  (ptcb);
    }

    do {
        if ((ptcb->TCB_iHetrccMode  == LW_HETRCC_WEAK_AFFINITY) &&      /*  ���������׺ͱ��������      */
            (ptcb->TCB_ulHetrccLock != pcpu->CPU_ulHccId) &&
            _CandTableWield(ptcb->TCB_ulHetrccLock,
                            ptcb->TCB_ucPriority)) {                    /*  ����ִ�� weak yield ����    */

            _DelTCBFromReadyRing(ptcb, *pppcb);                         /*  �Ӿ��������˳�              */
            if (_PcbIsEmpty(*pppcb)) {
                __DEL_RDY_MAP(ptcb);                                    /*  �Ӿ�������ɾ��              */
            }

            weakt[widx].ptcb =  ptcb;                                   /*  ��¼                        */
            weakt[widx].ppcb = *pppcb;
            widx++;
            ptcb = _CandTablePeek(pcpu, pppcb);                         /*  �ٻ�ȡһ������Ҫ���е�����  */

        } else {
            break;                                                      /*  ֱ������                    */
        }
    } while ((widx < _k_iWieldMaxTry) && ptcb);

    for (--widx; widx >= 0; widx--) {
        _AddTCBToReadyRing(weakt[widx].ptcb, weakt[widx].ppcb, LW_TRUE);
        if (_PcbIsOne(weakt[widx].ppcb)) {
            __ADD_RDY_MAP(weakt[widx].ptcb);                            /*  ����ȫ�־�����              */
        }
    }
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */

    return  (ptcb);
}
/*********************************************************************************************************
** ��������: _CandTableFill
** ��������: ѡ��һ�����ִ���̷߳����ѡ��.
** �䡡��  : pcpu          CPU �ṹ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  _CandTableFill (PLW_CLASS_CPU   pcpu)
{
    REGISTER PLW_CLASS_TCB          ptcb;
             PLW_CLASS_PCB          ppcb;

    ptcb = _CandTableSelect(pcpu, &ppcb);                               /*  ��ȡһ������Ҫ���е�����    */
    _BugHandle((ptcb == LW_NULL), LW_TRUE, "serious error!\r\n");       /*  ���ٴ���һ������            */

    LW_CAND_TCB(pcpu) = ptcb;                                           /*  �����µĿ�ִ���߳�          */
    ptcb->TCB_ulCPUId = LW_CPU_GET_ID(pcpu);                            /*  ��¼ CPU ��                 */
    ptcb->TCB_bIsCand = LW_TRUE;                                        /*  �����ѡ���б�              */

    _DelTCBFromReadyRing(ptcb, ppcb);                                   /*  �Ӿ��������˳�              */
    if (_PcbIsEmpty(ppcb)) {
        __DEL_RDY_MAP(ptcb);                                            /*  �Ӿ�������ɾ��              */
    }
}
/*********************************************************************************************************
** ��������: _CandTableEmpty
** ��������: ����ѡ���еĺ�ѡ�̷߳��������.
** �䡡��  : pcpu      CPU �ṹ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  _CandTableEmpty (PLW_CLASS_CPU   pcpu)
{
    REGISTER PLW_CLASS_TCB  ptcb;
    REGISTER PLW_CLASS_PCB  ppcb;
    
    ptcb = LW_CAND_TCB(pcpu);
    ppcb = _GetPcb(ptcb);
    ptcb->TCB_bIsCand = LW_FALSE;
                                                                        /*  ���¼��������              */
    _AddTCBToReadyRing(ptcb, ppcb, LW_TRUE);                            /*  ���������ͷ, �´����ȵ���  */
    if (_PcbIsOne(ppcb)) {
        __ADD_RDY_MAP(ptcb);                                            /*  ��ǰ�̱߳���ռ, �ػؾ�����  */
    }
    
    LW_CAND_TCB(pcpu) = LW_NULL;
}
/*********************************************************************************************************
** ��������: _CandTableTryHot
** ��������: ��ͼ��һ�������߳�װ��֮ǰ���е� CPU
** �䡡��  : ptcb          �������߳�
**           pcpu          ָ���� CPU
** �䡡��  : �Ƿ�����˺�ѡ���б�
** ȫ�ֱ���:
** ����ģ��:
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
        return  (bRotIdle);                                             /*  �������׺�����              */
    }

    ptcbCand = LW_CAND_TCB(pcpu);
    if (LW_UNLIKELY(ptcbCand == LW_NULL)) {
        LW_CAND_TCB(pcpu) = ptcb;
        ptcb->TCB_bIsCand = LW_TRUE;                                    /*  �����ѡ���б�              */
        return  (LW_TRUE);
    }

    if (!LW_CAND_ROT(pcpu) &&
        (LW_PRIO_IS_EQU(LW_PRIO_LOWEST,
                        ptcbCand->TCB_ucPriority))) {                   /*  ֮ǰ���е� CPU Ϊ idle      */
        LW_CAND_ROT(pcpu) = LW_TRUE;                                    /*  �������ȼ�����              */
        bRotIdle          = LW_TRUE;
    }

    return  (bRotIdle);
}
/*********************************************************************************************************
** ��������: _CandTableTryIdle
** ��������: ��ͼ��һ�������߳�װ���������ʵ� IDLE CPU
** �䡡��  : ptcb          �������߳�
**           pcpuExcept    ��Ҫ�ų��� CPU
**           ppcpuIdle     ̽��� IDLE �� CPU ID
**           iMode         0: ALL 1: ���ж��칹������ 2: ���ж��칹������
** �䡡��  : �Ƿ�����˺�ѡ���б�
** ȫ�ֱ���:
** ����ģ��:
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
#endif                                                                  /*  ���߳�֧��                  */

    LW_CPU_FOREACH_ACTIVE (i) {                                         /*  CPU ����Ϊ����״̬          */
        pcpu = LW_CPU_GET(i);
        if (pcpu == pcpuExcept) {
            continue;                                                   /*  �����ų��� CPU              */
        }
        if (LW_CPU_ONLY_AFFINITY_GET(pcpu)) {                           /*  ������ǿ�׺Ͷȵ���          */
            continue;
        }

#if LW_CFG_CPU_ARCH_HETRC > 0
        if (iMode == 1 &&
            ptcb->TCB_ulHetrccLock != pcpu->CPU_ulHccId) {
            continue;                                                   /*  ���ж��칹������            */
        } else if (iMode == 2 &&
                   ptcb->TCB_ulHetrccLock == pcpu->CPU_ulHccId) {
            continue;                                                   /*  ���ж��칹������            */
        }
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */

        ptcbCand = LW_CAND_TCB(pcpu);
        if (LW_UNLIKELY(ptcbCand == LW_NULL)) {                         /*  ��ѡ��Ϊ��                  */
            LW_CAND_TCB(pcpu) = ptcb;
            ptcb->TCB_ulCPUId = i;                                      /*  ��¼ CPU ��                 */
            ptcb->TCB_bIsCand = LW_TRUE;                                /*  �����ѡ���б�              */
            return  (LW_TRUE);

        } else if (!LW_CAND_ROT(pcpu) &&
                   (LW_PRIO_IS_EQU(LW_PRIO_LOWEST,
                                   ptcbCand->TCB_ucPriority))) {        /*  ���� idle �������ޱ�ע      */
#if LW_CFG_CPU_ARCH_SMT > 0                                             /*  ���߳� CPU ����ѡ��������� */
            if (LW_KERN_SMT_BSCHED_EN_GET()) {
                pphycpu = LW_PHYCPU_GET(pcpu->CPU_ulPhyId);
                if (pphycpu->PHYCPU_uiNonIdle == 0) {                   /*  �������Ϊ idle             */
                    LW_CAND_ROT(pcpu) = LW_TRUE;
                    bRotIdle          = LW_TRUE;
                    break;                                              /*  ֻ��עһ�� CPU ����         */

                } else if (!(*ppcpuIdle)) {
                    *ppcpuIdle = pcpu;                                  /*  ��¼һ�� idle ���ĵ�λ��    */
                }
            } else
#endif                                                                  /*  LW_CFG_CPU_ARCH_SMT > 0     */
            {
                LW_CAND_ROT(pcpu) = LW_TRUE;
                bRotIdle          = LW_TRUE;
                break;                                                  /*  ֻ��עһ�� CPU ����         */
            }
        }
    }

    return  (bRotIdle);
}
/*********************************************************************************************************
** ��������: _CandTableTryPreemptive
** ��������: ��ͼ��һ�������߳�װ���������ʵ� CPU ������ռ����
** �䡡��  : ptcb          �������߳�
**           pcpuExcept    ��Ҫ�ų��� CPU
**           bHetrcc       �Ƿ��ж��칹������
** �䡡��  : �Ƿ�����˺�ѡ���б�
** ȫ�ֱ���:
** ����ģ��:
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

    LW_CPU_FOREACH_ACTIVE (i) {                                         /*  CPU ����Ϊ����״̬          */
        pcpu = LW_CPU_GET(i);
        if (pcpu == pcpuExcept) {
            continue;                                                   /*  �����ų��� CPU              */
        }
        if (LW_CPU_ONLY_AFFINITY_GET(pcpu)) {                           /*  ������ǿ�׺Ͷȵ���          */
            continue;
        }

#if LW_CFG_CPU_ARCH_HETRC > 0
        if (bHetrcc && ptcb->TCB_ulHetrccLock != pcpu->CPU_ulHccId) {   /*  ���ж��칹������            */
            continue;
        }
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */

        ptcbCand = LW_CAND_TCB(pcpu);
        if (LW_UNLIKELY(ptcbCand == LW_NULL)) {                         /*  ��ѡ��Ϊ��                  */
            LW_CAND_TCB(pcpu) = ptcb;
            ptcb->TCB_ulCPUId = i;                                      /*  ��¼ CPU ��                 */
            ptcb->TCB_bIsCand = LW_TRUE;                                /*  �����ѡ���б�              */
            return  (LW_TRUE);

        } else if (LW_PRIO_IS_HIGH(ptcb->TCB_ucPriority,
                                   ptcbCand->TCB_ucPriority)) {

            if (bFsched) {                                              /*  Fast Sched                  */
                LW_CAND_ROT(pcpu) = LW_TRUE;                            /*  �������ȼ�����              */
                bRotIdle          = LW_TRUE;

            } else if (LW_PRIO_IS_HIGH(uiPrioL,
                                       ptcbCand->TCB_ucPriority)) {     /*  ��������ռ������ȼ�����    */
                uiPrioL = ptcbCand->TCB_ucPriority;
                pcpuLow = pcpu;                                         /*  ��¼������ȼ� CPU          */
            }
        }
    }

    if (!bFsched && pcpuLow) {                                          /*  ��������ռ������ȼ�����    */
        LW_CAND_ROT(pcpuLow) = LW_TRUE;                                 /*  �������ȼ�����              */
        bRotIdle             = LW_TRUE;
    }

    return  (bRotIdle);
}
/*********************************************************************************************************
** ��������: _CandTableTryAll
** ��������: ��ͼ��һ�����ɵ��ȵľ����߳�װ���������ʵ� CPU
** �䡡��  : ptcb              �������߳�
**           bOtherExceptMe    �Ƿ��� TryPreemptive �׶��ų� ptcb ֮ǰ���е� CPU
**           bHasTryHot        �Ƿ���Ҫ try hot cpu
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  _CandTableTryAll (PLW_CLASS_TCB  ptcb, BOOL  bOtherExceptMe, BOOL  bHasTryHot)
{
             INT             iMode;
    REGISTER BOOL            bRotIdle = LW_FALSE;
    REGISTER PLW_CLASS_CPU   pcpu;
             PLW_CLASS_CPU   pcpuIdle;

    pcpu = LW_CPU_GET(ptcb->TCB_ulCPUId);                               /*  ���� CACHE �ȶ�             */
    if (!bHasTryHot) {
        bRotIdle = _CandTableTryHot(ptcb, pcpu);                        /*  ���ȳ��Դ� CPU              */
    }

    if (!bRotIdle) {
#if LW_CFG_CPU_ARCH_HETRC > 0
        iMode = ptcb->TCB_iHetrccMode != LW_HETRCC_NON_AFFINITY ? 2 : 0;
#else
        iMode = 0;
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */

        pcpuIdle = LW_NULL;
        bRotIdle = _CandTableTryIdle(ptcb, pcpu, &pcpuIdle, iMode);     /*  �������� IDLE CPU           */

#if LW_CFG_CPU_ARCH_SMT > 0                                             /*  ���߳��Ƿ��п����߼�����    */
        if (!bRotIdle && pcpuIdle) {
            LW_CAND_ROT(pcpuIdle) = LW_TRUE;
            bRotIdle              = LW_TRUE;
        }
#endif                                                                  /*  LW_CFG_CPU_ARCH_SMT > 0     */

        if (!bRotIdle) {
            _CandTableTryPreemptive(ptcb,
                                    bOtherExceptMe ? pcpu : LW_NULL,
                                    LW_FALSE);                          /*  �������� CPU                */
        }
    }
}
/*********************************************************************************************************
** ��������: _CandTableTryHetrcc
** ��������: ��ͼ��һ�����칹�������׺͵ľ����߳�װ���������ʵ� CPU
** �䡡��  : ptcb              �������߳�
**           bOtherExceptMe    �Ƿ��� TryPreemptive �׶��ų� ptcb ֮ǰ���е� CPU
**           pbHasTryHot       �Ƿ��Ѿ� try hot cpu
** �䡡��  : �Ƿ�����˺�ѡ���б�
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
#if LW_CFG_CPU_ARCH_HETRC > 0

static BOOL  _CandTableTryHetrcc (PLW_CLASS_TCB  ptcb, BOOL  bOtherExceptMe, BOOL  *pbHasTryHot)
{
    REGISTER BOOL            bRotIdle = LW_FALSE;
    REGISTER PLW_CLASS_CPU   pcpu;
             PLW_CLASS_CPU   pcpuIdle;

    pcpu = LW_CPU_GET(ptcb->TCB_ulCPUId);                               /*  ���� CACHE �ȶ�             */
    if (!(*pbHasTryHot) &&
        pcpu->CPU_ulHccId == ptcb->TCB_ulHetrccLock) {                  /*  ����Ԥ��������              */
        bRotIdle     = _CandTableTryHot(ptcb, pcpu);                    /*  ���ȳ��Դ� CPU              */
        *pbHasTryHot = LW_TRUE;
    }

    if (!bRotIdle) {                                                    /*  û�г��Գɹ�                */
        pcpuIdle = LW_NULL;
        bRotIdle = _CandTableTryIdle(ptcb, pcpu, &pcpuIdle, 1);         /*  �������� IDLE CPU           */

#if LW_CFG_CPU_ARCH_SMT > 0                                             /*  ���߳��Ƿ��п����߼�����    */
        if (!bRotIdle && pcpuIdle) {
            LW_CAND_ROT(pcpuIdle) = LW_TRUE;
            bRotIdle              = LW_TRUE;
        }
#endif                                                                  /*  LW_CFG_CPU_ARCH_SMT > 0     */

        if (!bRotIdle) {
            bRotIdle = _CandTableTryPreemptive(ptcb,
                                               bOtherExceptMe ? pcpu : LW_NULL,
                                               LW_TRUE);                /*  �������� CPU                */
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
** ��������: _CandTableTryAdd
** ��������: ��ͼ��һ�������߳�װ���ѡ�̱߳�.
** �䡡��  : ptcb          �������߳�
**           ppcb          ��Ӧ�����ȼ����ƿ�
** �䡡��  : �Ƿ�����˺�ѡ���б�
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
BOOL  _CandTableTryAdd (PLW_CLASS_TCB  ptcb, PLW_CLASS_PCB  ppcb)
{
    REGISTER PLW_CLASS_CPU   pcpu;
    REGISTER PLW_CLASS_TCB   ptcbCand;
    
#if LW_CFG_SMP_EN > 0                                                   /*  SMP ���                    */
    if (ptcb->TCB_bCPULock) {                                           /*  �������� CPU                */
        pcpu = LW_CPU_GET(ptcb->TCB_ulCPULock);
        if (!LW_CPU_IS_ACTIVE(pcpu)) {
            goto    __can_not_cand;
        }
        
        ptcbCand = LW_CAND_TCB(pcpu);
        if (LW_UNLIKELY(ptcbCand == LW_NULL)) {                         /*  ��ѡ��Ϊ��                  */
            LW_CAND_TCB(pcpu) = ptcb;
            ptcb->TCB_ulCPUId = ptcb->TCB_ulCPULock;                    /*  ��¼ CPU ��                 */
            ptcb->TCB_bIsCand = LW_TRUE;                                /*  �����ѡ���б�              */
            return  (LW_TRUE);
            
        } else if (LW_PRIO_IS_HIGH(ptcb->TCB_ucPriority, 
                                   ptcbCand->TCB_ucPriority)) {         /*  ���ȼ����ڵ�ǰ��ѡ�߳�      */
            LW_CAND_ROT(pcpu) = LW_TRUE;                                /*  �������ȼ�����              */
        }

    } else {                                                            /*  �ɽ��п���ĵ���            */
        BOOL  bHasTryHot = LW_FALSE;
        BOOL  bRotIdle   = LW_FALSE;

#if LW_CFG_CPU_ARCH_HETRC > 0
        if (ptcb->TCB_iHetrccMode != LW_HETRCC_NON_AFFINITY) {          /*  �칹�����ص���              */
            if (LW_HCCCPU_GET(ptcb->TCB_ulHetrccLock)->HCCCPU_uiActive) {
                bRotIdle = _CandTableTryHetrcc(ptcb, LW_FALSE, &bHasTryHot);
            }
            if (ptcb->TCB_iHetrccMode == LW_HETRCC_STRONG_AFFINITY) {
                bRotIdle = LW_TRUE;                                     /*  ���ٽ��е��ȳ���            */
            }
        }
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */

        if (!bRotIdle) {
            _CandTableTryAll(ptcb, LW_FALSE, bHasTryHot);               /*  �������� CPU                */
        }
    }

#else                                                                   /*  �������                    */
    pcpu = LW_CPU_GET(0);
    if (!LW_CPU_IS_ACTIVE(pcpu)) {                                      /*  CPU ����Ϊ����״̬          */
        goto    __can_not_cand;
    }
    
    ptcbCand = LW_CAND_TCB(pcpu);
    if (LW_UNLIKELY(ptcbCand == LW_NULL)) {                             /*  ��ѡ��Ϊ��                  */
__can_cand:
        LW_CAND_TCB(pcpu) = ptcb;
        ptcb->TCB_ulCPUId = 0;                                          /*  ��¼ CPU ��                 */
        ptcb->TCB_bIsCand = LW_TRUE;                                    /*  �����ѡ���б�              */
        return  (LW_TRUE);                                              /*  ֱ�Ӽ����ѡ���б�          */
        
    } else if (LW_PRIO_IS_HIGH(ptcb->TCB_ucPriority, 
                               ptcbCand->TCB_ucPriority)) {             /*  ���ȼ��ȵ�ǰ��ѡ�̸߳�      */
        if (__THREAD_LOCK_GET(ptcbCand)) {
            LW_CAND_ROT(pcpu) = LW_TRUE;                                /*  �������ȼ�����              */
        
        } else {
            _CandTableEmpty(pcpu);                                      /*  ��պ�ѡ��                  */
            goto    __can_cand;
        }
    }
#endif                                                                  /*  LW_CFG_SMP_EN               */

__can_not_cand:
    if (_PcbIsEmpty(ppcb)) {
        __ADD_RDY_MAP(ptcb);                                            /*  ��λͼ�����λ��һ          */
    }
    
    return  (LW_FALSE);                                                 /*  �޷������ѡ���б�          */
}
/*********************************************************************************************************
** ��������: _CandTableTryDel
** ��������: ��ͼ��һ�����پ����ĺ�ѡ�̴߳Ӻ�ѡ�����˳�
** �䡡��  : ptcb          �������߳�
**           ppcb          ��Ӧ�����ȼ����ƿ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID _CandTableTryDel (PLW_CLASS_TCB  ptcb, PLW_CLASS_PCB  ppcb)
{
    REGISTER PLW_CLASS_CPU   pcpu = LW_CPU_GET(ptcb->TCB_ulCPUId);
    
    if (LW_CAND_TCB(pcpu) == ptcb) {                                    /*  �ں�ѡ����                  */
        ptcb->TCB_bIsCand = LW_FALSE;                                   /*  �˳���ѡ���б�              */
        _CandTableFill(pcpu);                                           /*  �������һ����ѡ�߳�        */
        LW_CAND_ROT(pcpu) = LW_FALSE;                                   /*  ������ȼ����Ʊ�־          */
    
    } else {                                                            /*  û���ں�ѡ����              */
        if (_PcbIsEmpty(ppcb)) {
            __DEL_RDY_MAP(ptcb);                                        /*  ��λͼ�����λ����          */
        }
    }
}
/*********************************************************************************************************
** ��������: _CandTableNotify
** ��������: ֪ͨ���� CPU ���е��Ȳ鿴. (�������Ҫ���þ��Ʊ�־, ����Ҫ���ͺ˼��ж�, �������л���϶�ᷢ��)
** �䡡��  : ptcb      ��ǰ CPU ��ѡ�߳�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
#if LW_CFG_SMP_EN > 0

VOID _CandTableNotify (PLW_CLASS_TCB  ptcb)
{
#if LW_CFG_CPU_ARCH_HETRC > 0
    BOOL  bHasTryHot = LW_TRUE;                                         /*  �����Ա����ĵ���            */
    BOOL  bRotIdle   = LW_FALSE;

    if (ptcb->TCB_iHetrccMode != LW_HETRCC_NON_AFFINITY) {              /*  �칹�����ص���              */
        if (LW_HCCCPU_GET(ptcb->TCB_ulHetrccLock)->HCCCPU_uiActive) {
            bRotIdle = _CandTableTryHetrcc(ptcb, LW_TRUE, &bHasTryHot);
        }
        if (ptcb->TCB_iHetrccMode == LW_HETRCC_STRONG_AFFINITY) {
            bRotIdle = LW_TRUE;                                         /*  ���ٽ��е��ȳ���            */
        }
    }

    if (!bRotIdle)
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */
    {
        _CandTableTryAll(ptcb, LW_TRUE, LW_TRUE);                       /*  ����װ������ CPU            */
    }
}

#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
/*********************************************************************************************************
** ��������: _CandTableUpdate
** ��������: ���Խ�������ȼ���������װ���ѡ��. 
** �䡡��  : pcpu      CPU �ṹ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
** ע  ��  : ����µĺ�ѡ����Ϊ������ǰ CPU ��������û�����ù����Ʊ�־, ���ͬʱ�������������������������
             CPU ��, ����Ҫ�������� CPU �ľ��Ʊ�־.
*********************************************************************************************************/
VOID _CandTableUpdate (PLW_CLASS_CPU   pcpu)
{
    REGISTER PLW_CLASS_TCB      ptcbCand;
    REGISTER PLW_CLASS_TCB      ptcb;
             PLW_CLASS_PCB      ppcb;
             BOOL               bNeedRotate = LW_FALSE;

    if (!LW_CPU_IS_ACTIVE(pcpu)) {                                      /*  CPU ����Ϊ����״̬          */
        return;
    }

    ptcbCand = LW_CAND_TCB(pcpu);
    if (ptcbCand == LW_NULL) {                                          /*  ��ǰû�к�ѡ�߳�            */
        _CandTableFill(pcpu);
        goto    __update_done;
    }

    ptcb = _CandTableSelect(pcpu, &ppcb);                               /*  ��ȡһ������Ҫ���е�����    */
    if (LW_UNLIKELY(ptcb == LW_NULL)) {
        goto    __update_done;                                          /*  û����Ҫ���е�����          */
    }

#if LW_CFG_SMP_EN > 0
    if (LW_CPU_ONLY_AFFINITY_GET(pcpu) && !ptcbCand->TCB_bCPULock) {    /*  ǿ�������׺Ͷ�����          */
        bNeedRotate = LW_TRUE;                                          /*  ��ǰ��ͨ������Ҫ�ó� CPU    */

    } else
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
    {
        if (ptcbCand->TCB_usSchedCounter == 0) {                        /*  �Ѿ�û��ʱ��Ƭ��            */
            if (LW_PRIO_IS_HIGH_OR_EQU(ptcb->TCB_ucPriority,
                                       ptcbCand->TCB_ucPriority)) {     /*  �Ƿ���Ҫ��ת                */
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
                (ptcbCand->TCB_ulHetrccLock != pcpu->CPU_ulHccId) &&    /*  ��ǰ�������׺ͱ��������    */
                _CandTableWield(ptcbCand->TCB_ulHetrccLock,
                                ptcbCand->TCB_ucPriority)) {            /*  ����ִ�� weak yield ����    */
                bNeedRotate = LW_TRUE;                                  /*  ����ת�Ƶ��׺͵�������      */
            }
        }
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */
    }

    if (bNeedRotate) {                                                  /*  ���ڸ���Ҫ���е��߳�        */
        _CandTableEmpty(pcpu);                                          /*  ��պ�ѡ��                  */

        LW_CAND_TCB(pcpu) = ptcb;                                       /*  �����µĿ�ִ���߳�          */
        ptcb->TCB_ulCPUId = LW_CPU_GET_ID(pcpu);                        /*  ��¼ CPU ��                 */
        ptcb->TCB_bIsCand = LW_TRUE;                                    /*  �����ѡ���б�              */

        _DelTCBFromReadyRing(ptcb, ppcb);                               /*  �Ӿ��������˳�              */
        if (_PcbIsEmpty(ppcb)) {
            __DEL_RDY_MAP(ptcb);                                        /*  �Ӿ�������ɾ��              */
        }

#if LW_CFG_SMP_EN > 0                                                   /*  SMP ���                    */
        if (!ptcbCand->TCB_bCPULock &&
            (LW_CAND_TCB(pcpu) != ptcbCand)) {                          /*  �Ƿ���Ҫ���Ա������ CPU    */
            _CandTableNotify(ptcbCand);                                 /*  ֪ͨ���� CPU ���е��Ȳ鿴   */
        }
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
    }

__update_done:
    LW_CAND_ROT(pcpu) = LW_FALSE;                                       /*  ������ȼ����Ʊ�־          */
}
/*********************************************************************************************************
** ��������: _CandTableRemove
** ��������: ��һ�� CPU ��Ӧ�ĺ�ѡ����� 
** �䡡��  : pcpu      CPU �ṹ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
#if (LW_CFG_SMP_EN > 0) && (LW_CFG_SMP_CPU_DOWN_EN > 0)

VOID _CandTableRemove (PLW_CLASS_CPU   pcpu)
{
    if (LW_CPU_IS_ACTIVE(pcpu)) {                                       /*  CPU ����Ϊ�Ǽ���״̬        */
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
