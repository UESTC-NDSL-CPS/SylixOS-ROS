/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: sysperfLib.c
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2016 �� 07 �� 28 ��
**
** ��        ��: ϵͳ���ܷ�������.
**
** BUG:
2016.08.25  ���²�����ʱ��Ҫ��������������.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
  �ü�֧��
*********************************************************************************************************/
#if (LW_CFG_SYSPERF_EN > 0) && (LW_CFG_MODULELOADER_EN > 0)
#include "../SylixOS/loader/include/loader_lib.h"
#include "dlfcn.h"
#include "sysperf.h"
#include "sysperfLib.h"
/*********************************************************************************************************
  ȫ�ֱ���
*********************************************************************************************************/
LW_SYSPERF_PARAM    _G_sysperfparam;                                    /*  ϵͳ����                    */
/*********************************************************************************************************
  �����
*********************************************************************************************************/
#define LW_SYSPERF_STATE()          (_G_sysperfparam.PERF_iState)
#define LW_SYSPERF_COLLECTOR()      (_G_sysperfparam.PERF_ulThread)
#define LW_SYSPERF_MSGQ()           (_G_sysperfparam.PERF_ulMsgQ)
#define LW_SYSPERF_MUTEX()          (_G_sysperfparam.PERF_ulLock)
#define LW_SYSPERF_HEADER()         (_G_sysperfparam.PERF_pringInfo)

#define LW_SYSPERF_LOCK()           API_SemaphoreMPend(LW_SYSPERF_MUTEX(), LW_OPTION_WAIT_INFINITE)
#define LW_SYSPERF_UNLOCK()         API_SemaphoreMPost(LW_SYSPERF_MUTEX())
/*********************************************************************************************************
** ��������: __sysperfCollector
** ��������: ϵͳ���ܷ����ռ���. (���ж���������ִ��)
** �䡡��  : pcpuCur       ��ǰ���е� CPU
**           psysperfnode  �ڵ���Ϣ
** �䡡��  : 0: ����Ҫ����  1: ���� -1: ����
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT  __sysperfCollector (PLW_CLASS_CPU  pcpuCur, PLW_SYSPERF_NODE  psysperfnode)
{
    PLW_CLASS_TCB  ptcbCur;
    
    ptcbCur = pcpuCur->CPU_ptcbTCBCur;
    if ((ptcbCur->TCB_ulId != LW_SYSPERF_COLLECTOR()) &&
        (ptcbCur->TCB_ucPriority != LW_PRIO_LOWEST)) {
        psysperfnode->PERF_ulCPUId  = LW_CPU_GET_ID(pcpuCur);
        psysperfnode->PERF_ulThread = ptcbCur->TCB_ulId;
        psysperfnode->PERF_pid      = vprocGetPidByTcbNoLock(ptcbCur);
        psysperfnode->PERF_ulAddr   = (addr_t)ARCH_REG_CTX_GET_PC(&ptcbCur->TCB_archRegCtx);
        return  (1);
    }
    
    return  (0);
}
/*********************************************************************************************************
** ��������: __sysperfIpiHook
** ��������: ϵͳ���ܷ����ռ���. (���ж���������ִ��)
** �䡡��  : pcpuCur       ��ǰ���е� CPU
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
#if LW_CFG_SMP_EN > 0

static VOID  __sysperfIpiHook (PLW_CLASS_CPU  pcpuCur)
{
    INT               iRet;
    LW_SYSPERF_NODE   sysperfnode;
    
    iRet = __sysperfCollector(pcpuCur, &sysperfnode);
    if (iRet == 1) {
        API_MsgQueueSend(LW_SYSPERF_MSGQ(), &sysperfnode, sizeof(LW_SYSPERF_NODE));
    }
}

#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
/*********************************************************************************************************
** ��������: __sysperfTickHook
** ��������: ϵͳ���ܷ����ռ���. (���ж���������ִ��)
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  __sysperfTickHook (VOID)
{
#if LW_CFG_SMP_EN > 0
    ULONG   i;
    ULONG   ulCPUId;
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */

    INT               iRet;
    PLW_CLASS_CPU     pcpuCur;
    LW_SYSPERF_NODE   sysperfnode;

    pcpuCur = LW_CPU_GET_CUR();
    iRet    = __sysperfCollector(pcpuCur, &sysperfnode);
    if (iRet == 1) {
        API_MsgQueueSend(LW_SYSPERF_MSGQ(), &sysperfnode, sizeof(LW_SYSPERF_NODE));
    }
    
#if LW_CFG_SMP_EN > 0
    ulCPUId = LW_CPU_GET_ID(pcpuCur);
    LW_CPU_FOREACH_EXCEPT (i, ulCPUId) {
        _SmpSendIpi(i, LW_IPI_PERF, 0, LW_FALSE);
    }
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
}
/*********************************************************************************************************
** ��������: __sysperfCleanup
** ��������: ϵͳ���ܷ����ռ��߳����.
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  __sysperfCleanup (VOID)
{
#if LW_CFG_SMP_EN > 0
    _SmpPerfIpi(LW_NULL);
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
    API_SystemHookDelete(__sysperfTickHook, LW_OPTION_THREAD_TICK_HOOK);
}
/*********************************************************************************************************
** ��������: __sysperfNode2Info
** ��������: ���ɼ��ڵ���Ϣת���ɿ�����Ϣ.
** �䡡��  : psysperfnode  �ڵ���Ϣ
**           psysperfinfo  ������Ϣ
** �䡡��  : LW_NULL
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  __sysperfNode2Info (const PLW_SYSPERF_NODE  psysperfnode, PLW_SYSPERF_INFO  psysperfinfo)
{
    Dl_info      dlinfo;
    LW_LD_VPROC *pvproc;
    
    psysperfinfo->PERF_ulCPUId   = psysperfnode->PERF_ulCPUId;
    psysperfinfo->PERF_ulCounter = 1;
    psysperfinfo->PERF_ulThread  = psysperfnode->PERF_ulThread;
    psysperfinfo->PERF_pid       = psysperfnode->PERF_pid;
    psysperfinfo->PERF_ulSamAddr = psysperfnode->PERF_ulAddr;
    
    LW_LD_LOCK();
    if (psysperfnode->PERF_pid > 0) {
        pvproc = vprocGet(psysperfnode->PERF_pid);
    
    } else {
        pvproc = LW_NULL;
    }
    
    if ((API_ModuleAddr((PVOID)psysperfnode->PERF_ulAddr, &dlinfo, pvproc) == ERROR_NONE) &&
        (dlinfo.dli_sname)) {
        lib_strlcpy(psysperfinfo->PERF_cFunc, dlinfo.dli_sname, LW_SYSPERF_FUNC_NAME_MAX);
        psysperfinfo->PERF_ulSymAddr = (addr_t)dlinfo.dli_saddr;
    
    } else {
        lib_strlcpy(psysperfinfo->PERF_cFunc, "No symbol name", LW_SYSPERF_FUNC_NAME_MAX);
        psysperfinfo->PERF_ulSymAddr = psysperfinfo->PERF_ulSamAddr;
    }
    LW_LD_UNLOCK();
}
/*********************************************************************************************************
** ��������: __sysperfThread
** ��������: ϵͳ���ܷ����ռ��߳�.
** �䡡��  : pvArg         �̲߳���
** �䡡��  : LW_NULL
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
PVOID  __sysperfThread (PVOID  pvArg)
{
    INT                 i;
    LW_SYSPERF_NODE     sysperfnode;
    LW_SYSPERF_INFO     sysperfinfo;
    PLW_SYSPERF_INFO    psysperfinfo;
    PLW_SYSPERF_INFO    psysperfinfoCmp;
    PLW_LIST_RING       pringTemp;

#if LW_CFG_SMP_EN > 0
    _SmpPerfIpi(__sysperfIpiHook);
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */

    API_SystemHookAdd(__sysperfTickHook, 
                      LW_OPTION_THREAD_TICK_HOOK);                      /*  ��װ��Ϣ�ռ���              */
    
    API_ThreadCleanupPush(__sysperfCleanup, LW_NULL);

    for (;;) {
        if (API_MsgQueueReceive(LW_SYSPERF_MSGQ(),
                                &sysperfnode, sizeof(LW_SYSPERF_NODE), 
                                LW_NULL, LW_OPTION_WAIT_INFINITE)) {
            if (LW_SYSPERF_STATE() != LW_SYSPERF_ON) {
                LW_SYSPERF_STATE()  = LW_SYSPERF_END;
                break;
            }
        }
        
        __sysperfNode2Info(&sysperfnode, &sysperfinfo);
        
        i = 0;
        LW_SYSPERF_LOCK();                                              /*  ����                        */
        pringTemp = LW_SYSPERF_HEADER();
        do {
            psysperfinfo = (PLW_SYSPERF_INFO)pringTemp;
            if (psysperfinfo->PERF_ulCounter == 0) {                    /*  δʹ�ýڵ�                  */
                psysperfinfo->PERF_ulCPUId   = sysperfinfo.PERF_ulCPUId;
                psysperfinfo->PERF_ulCounter = sysperfinfo.PERF_ulCounter;
                psysperfinfo->PERF_ulThread  = sysperfinfo.PERF_ulThread;
                psysperfinfo->PERF_pid       = sysperfinfo.PERF_pid;
                psysperfinfo->PERF_ulSymAddr = sysperfinfo.PERF_ulSymAddr;
                psysperfinfo->PERF_ulSamAddr = sysperfinfo.PERF_ulSamAddr;
                lib_strcpy(psysperfinfo->PERF_cFunc, sysperfinfo.PERF_cFunc);
                goto    __save_end;
            }
            if (psysperfinfo->PERF_ulSymAddr == sysperfinfo.PERF_ulSymAddr) {
                psysperfinfo->PERF_ulSamAddr =  sysperfinfo.PERF_ulSamAddr;
                psysperfinfo->PERF_ulCPUId   =  sysperfinfo.PERF_ulCPUId;
                psysperfinfo->PERF_ulThread  =  sysperfinfo.PERF_ulThread;
                psysperfinfo->PERF_pid       =  sysperfinfo.PERF_pid;
                psysperfinfo->PERF_ulCounter++;
                break;
            }
            i++;
            pringTemp = _list_ring_get_next(pringTemp);
        } while (pringTemp != LW_SYSPERF_HEADER());                     /*  ѭ��ƥ��                    */
        
        if ((i == 0) || (pringTemp != LW_SYSPERF_HEADER())) {           /*  �ҵ��ظ��ڵ�                */
            _List_Ring_Del(&psysperfinfo->PERF_ringManage,
                           &LW_SYSPERF_HEADER());                       /*  ��ͬ�Ľڵ����              */
        
        } else {                                                        /*  û���ҵ���ͬ��              */
            pringTemp    =  _list_ring_get_prev(LW_SYSPERF_HEADER());
            psysperfinfo = (PLW_SYSPERF_INFO)pringTemp;
            _List_Ring_Del(&psysperfinfo->PERF_ringManage,
                           &LW_SYSPERF_HEADER());                       /*  ���һ���ڵ����            */
                           
            psysperfinfo->PERF_ulCPUId   = sysperfinfo.PERF_ulCPUId;
            psysperfinfo->PERF_ulCounter = sysperfinfo.PERF_ulCounter;
            psysperfinfo->PERF_ulThread  = sysperfinfo.PERF_ulThread;
            psysperfinfo->PERF_pid       = sysperfinfo.PERF_pid;
            psysperfinfo->PERF_ulSymAddr = sysperfinfo.PERF_ulSymAddr;
            psysperfinfo->PERF_ulSamAddr = sysperfinfo.PERF_ulSamAddr;
            lib_strcpy(psysperfinfo->PERF_cFunc, sysperfinfo.PERF_cFunc);
        }
        
        psysperfinfoCmp = (PLW_SYSPERF_INFO)LW_SYSPERF_HEADER();        /*  ���²�������                */
        if (psysperfinfo->PERF_ulCounter >=
            psysperfinfoCmp->PERF_ulCounter) {
            _List_Ring_Add_Ahead(&psysperfinfo->PERF_ringManage, 
                                 &LW_SYSPERF_HEADER());
        
        } else {
            pringTemp = _list_ring_get_next(LW_SYSPERF_HEADER());
            while (pringTemp != LW_SYSPERF_HEADER()) {
                psysperfinfoCmp = (PLW_SYSPERF_INFO)pringTemp;
                if (psysperfinfo->PERF_ulCounter >=
                    psysperfinfoCmp->PERF_ulCounter) {
                    break;
                }
                pringTemp = _list_ring_get_next(pringTemp);
            }
            _List_Ring_Add_Last(&psysperfinfo->PERF_ringManage, 
                                &pringTemp);
        }
__save_end:
        LW_SYSPERF_UNLOCK();                                            /*  ����                        */
    }
    
    LW_THREAD_UNSAFE();                                                 /*  �˳���ȫģʽ                */
    
    return  (LW_NULL);
}

#endif                                                                  /*  LW_CFG_SYSPERF_EN > 0       */
                                                                        /*  LW_CFG_MODULELOADER_EN > 0  */
/*********************************************************************************************************
  END
*********************************************************************************************************/
