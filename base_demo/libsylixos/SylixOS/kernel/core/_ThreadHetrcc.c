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
** ��   ��   ��: _ThreadHetrcc.c
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2023 �� 06 �� 07 ��
**
** ��        ��: SMP ϵͳ�칹�����ص���ģ��.
**
** ע        ��: ��ĳ������ֹͣ����ʱ, �������ǿ�������׺͵������߳�, ���߳��Զ�����.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "../SylixOS/kernel/include/k_kernel.h"
/*********************************************************************************************************
  �ü�֧��
*********************************************************************************************************/
#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0)
/*********************************************************************************************************
** ��������: _ThreadSetHetrcc
** ��������: ����ָ�����߳��칹�����ص��Ȳ���. (�����ں˱�����)
** �䡡��  : ptcb          �߳̿��ƿ�
**           iMode         ģʽ (LW_HETRCC_NON_AFFINITY,
                                 LW_HETRCC_WEAK_AFFINITY,
                                 LW_HETRCC_STRONG_AFFINITY)
             ulHccId       �칹������ ID (���Խ������Խǿ)
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
** ע  ��  : �������뱣֤�Ǳ��߳�Ŀ���߳�û�о���������.
*********************************************************************************************************/
VOID  _ThreadSetHetrcc (PLW_CLASS_TCB  ptcb, INT  iMode, ULONG  ulHccId)
{
    INTREG         iregInterLevel;
    BOOL           bReReady;
    PLW_CLASS_TCB  ptcbCur;
    PLW_CLASS_PCB  ppcb;
    PLW_CLASS_CPU  pcpu;

    LW_TCB_GET_CUR(ptcbCur);

    if ((ptcbCur != ptcb) || (iMode == LW_HETRCC_NON_AFFINITY)) {       /*  ����������ߵ�ǰ���ɵ���    */
        ptcb->TCB_iHetrccMode  = iMode;
        ptcb->TCB_ulHetrccLock = ulHccId;                               /*  ����������                  */
        return;
    }

    bReReady = LW_FALSE;
    if ((ptcbCur->TCB_iHetrccMode  != iMode) ||
        (ptcbCur->TCB_ulHetrccLock != ulHccId)) {                       /*  �����ò�ƥ��                */
        bReReady = LW_TRUE;                                             /*  ���׺�Ҳִ�д˲���          */
                                                                        /*  ����һ�κ�ѡ��ѡ��          */
    } else if (iMode == LW_HETRCC_WEAK_AFFINITY) {                      /*  ���׺��ظ�����              */
        pcpu = LW_CPU_GET(ptcbCur->TCB_ulCPUId);
        if (pcpu->CPU_ulHccId != ulHccId) {                             /*  δ�����׺�������������      */
            bReReady = LW_TRUE;                                         /*  �ٳ���һ��                  */
        }
    }

    if (bReReady) {                                                     /*  ���þ�����                  */
        iregInterLevel = KN_INT_DISABLE();
        ppcb = _GetPcb(ptcbCur);
        __DEL_FROM_READY_RING(ptcbCur, ppcb);                           /*  �Ӿ�������ɾ��              */

        ptcbCur->TCB_iHetrccMode  = iMode;
        ptcbCur->TCB_ulHetrccLock = ulHccId;                            /*  ����������                  */

        ptcbCur->TCB_ucSchedActivate = LW_SCHED_ACT_INTERRUPT;          /*  �жϼ��ʽ                */
        ppcb = _GetPcb(ptcbCur);
        __ADD_TO_READY_RING(ptcbCur, ppcb);                             /*  ���뵽������ȼ�������      */
        KN_INT_ENABLE(iregInterLevel);
    }
}
/*********************************************************************************************************
** ��������: _ThreadGetHetrcc
** ��������: ��ȡ�߳��칹�����ص��Ȳ���.  (�����ں˱�����)
** �䡡��  : ptcb          �߳̿��ƿ�
**           piMode        ģʽ (LW_HETRCC_NON_AFFINITY,
                                 LW_HETRCC_WEAK_AFFINITY,
                                 LW_HETRCC_STRONG_AFFINITY)
             pulHccId      �칹������ ID (���Խ������Խǿ)
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: _ThreadOffHetrcc
** ��������: ����ָ���칹��������ǿ�׺Ͷ����õ��߳�ȫ���ر��칹�������׺Ͷȵ���. (�����ں˱�����)
** �䡡��  : pcpu          CPU
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
         plineList  = _list_line_get_next(plineList)) {                 /*  �����߳�                    */

        iregInterLevel = KN_INT_DISABLE();                              /*  �ر��ж�                    */
        ptcb = _LIST_ENTRY(plineList, LW_CLASS_TCB, TCB_lineManage);
        if ((ptcb->TCB_iHetrccMode  == LW_HETRCC_STRONG_AFFINITY) &&
            (ptcb->TCB_ulHetrccLock == ulHccId)) {
            if (__LW_THREAD_IS_READY(ptcb)) {
                if (ptcb->TCB_bIsCand) {
                    ptcb->TCB_iHetrccMode = LW_HETRCC_NON_AFFINITY;     /*  ����Ϊ���ɵ���              */

                } else {
                    ppcb = LW_HCC_RDY_PPCB(ptcb->TCB_ulHetrccLock,
                                           ptcb->TCB_ucPriority);
                    _DelTCBFromReadyRing(ptcb, ppcb);                   /*  �� CPU ˽�о��������˳�     */
                    if (_PcbIsEmpty(ppcb)) {
                        __DEL_RDY_MAP(ptcb);
                    }

                    ptcb->TCB_iHetrccMode = LW_HETRCC_NON_AFFINITY;     /*  ����Ϊ���ɵ���              */
                    ppcb = LW_GLOBAL_RDY_PPCB(ptcb->TCB_ucPriority);
                    _AddTCBToReadyRing(ptcb, ppcb, LW_FALSE);           /*  ����ȫ�־�����              */
                    if (_PcbIsOne(ppcb)) {
                        __ADD_RDY_MAP(ptcb);
                    }
                }
            } else {
                ptcb->TCB_iHetrccMode = LW_HETRCC_NON_AFFINITY;         /*  ����Ϊ���ɵ���              */
            }
        }
        KN_INT_ENABLE(iregInterLevel);                                  /*  ���ж�                    */
    }
}

#endif                                                                  /*  LW_CFG_SMP_CPU_DOWN_EN > 0  */
/*********************************************************************************************************
** ��������: _ThreadTickHetrcc
** ��������: �̼߳�¼����ʹ�����. (�����ں˱�����)
** �䡡��  : ptcb      ������ƿ�
**           pcpu      CPU ���ƿ�
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  _ThreadTickHetrcc (PLW_CLASS_TCB  ptcb, PLW_CLASS_CPU  pcpu)
{
    PLW_CLASS_HCCCPU  phcccpu;

    phcccpu = LW_HCCCPU_GET(pcpu->CPU_ulHccId);
    if (phcccpu->HCCCPU_uiComputPow) {
        ptcb->TCB_ulComputPow += (ULONG)phcccpu->HCCCPU_uiComputPow;    /*  ��ǰ�̼߳�¼����ʹ�����    */
    }
}
/*********************************************************************************************************
** ��������: _ThreadClearHetrcc
** ��������: �߳������¼������Ϣ. (�����ں˱�����)
** �䡡��  : ptcb      ������ƿ� (NULL ��ʾ�����������)
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
