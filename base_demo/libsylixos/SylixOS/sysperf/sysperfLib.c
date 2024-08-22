/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: sysperfLib.c
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2016 年 07 月 28 日
**
** 描        述: 系统性能分析工具.
**
** BUG:
2016.08.25  更新采样点时需要更新最新任务名.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
  裁剪支持
*********************************************************************************************************/
#if (LW_CFG_SYSPERF_EN > 0) && (LW_CFG_MODULELOADER_EN > 0)
#include "../SylixOS/loader/include/loader_lib.h"
#include "dlfcn.h"
#include "sysperf.h"
#include "sysperfLib.h"
/*********************************************************************************************************
  全局变量
*********************************************************************************************************/
LW_SYSPERF_PARAM    _G_sysperfparam;                                    /*  系统参数                    */
/*********************************************************************************************************
  宏操作
*********************************************************************************************************/
#define LW_SYSPERF_STATE()          (_G_sysperfparam.PERF_iState)
#define LW_SYSPERF_COLLECTOR()      (_G_sysperfparam.PERF_ulThread)
#define LW_SYSPERF_MSGQ()           (_G_sysperfparam.PERF_ulMsgQ)
#define LW_SYSPERF_MUTEX()          (_G_sysperfparam.PERF_ulLock)
#define LW_SYSPERF_HEADER()         (_G_sysperfparam.PERF_pringInfo)

#define LW_SYSPERF_LOCK()           API_SemaphoreMPend(LW_SYSPERF_MUTEX(), LW_OPTION_WAIT_INFINITE)
#define LW_SYSPERF_UNLOCK()         API_SemaphoreMPost(LW_SYSPERF_MUTEX())
/*********************************************************************************************************
** 函数名称: __sysperfCollector
** 功能描述: 系统性能分析收集器. (在中断上下文中执行)
** 输　入  : pcpuCur       当前运行的 CPU
**           psysperfnode  节点信息
** 输　出  : 0: 不需要发送  1: 发送 -1: 错误
** 全局变量: 
** 调用模块: 
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
** 函数名称: __sysperfIpiHook
** 功能描述: 系统性能分析收集器. (在中断上下文中执行)
** 输　入  : pcpuCur       当前运行的 CPU
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
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
** 函数名称: __sysperfTickHook
** 功能描述: 系统性能分析收集器. (在中断上下文中执行)
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
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
** 函数名称: __sysperfCleanup
** 功能描述: 系统性能分析收集线程清除.
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static VOID  __sysperfCleanup (VOID)
{
#if LW_CFG_SMP_EN > 0
    _SmpPerfIpi(LW_NULL);
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
    API_SystemHookDelete(__sysperfTickHook, LW_OPTION_THREAD_TICK_HOOK);
}
/*********************************************************************************************************
** 函数名称: __sysperfNode2Info
** 功能描述: 将采集节点信息转换成可视信息.
** 输　入  : psysperfnode  节点信息
**           psysperfinfo  可视信息
** 输　出  : LW_NULL
** 全局变量: 
** 调用模块: 
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
** 函数名称: __sysperfThread
** 功能描述: 系统性能分析收集线程.
** 输　入  : pvArg         线程参数
** 输　出  : LW_NULL
** 全局变量: 
** 调用模块: 
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
                      LW_OPTION_THREAD_TICK_HOOK);                      /*  安装信息收集器              */
    
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
        LW_SYSPERF_LOCK();                                              /*  锁定                        */
        pringTemp = LW_SYSPERF_HEADER();
        do {
            psysperfinfo = (PLW_SYSPERF_INFO)pringTemp;
            if (psysperfinfo->PERF_ulCounter == 0) {                    /*  未使用节点                  */
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
        } while (pringTemp != LW_SYSPERF_HEADER());                     /*  循环匹配                    */
        
        if ((i == 0) || (pringTemp != LW_SYSPERF_HEADER())) {           /*  找到重复节点                */
            _List_Ring_Del(&psysperfinfo->PERF_ringManage,
                           &LW_SYSPERF_HEADER());                       /*  相同的节点解链              */
        
        } else {                                                        /*  没有找到相同的              */
            pringTemp    =  _list_ring_get_prev(LW_SYSPERF_HEADER());
            psysperfinfo = (PLW_SYSPERF_INFO)pringTemp;
            _List_Ring_Del(&psysperfinfo->PERF_ringManage,
                           &LW_SYSPERF_HEADER());                       /*  最后一个节点解链            */
                           
            psysperfinfo->PERF_ulCPUId   = sysperfinfo.PERF_ulCPUId;
            psysperfinfo->PERF_ulCounter = sysperfinfo.PERF_ulCounter;
            psysperfinfo->PERF_ulThread  = sysperfinfo.PERF_ulThread;
            psysperfinfo->PERF_pid       = sysperfinfo.PERF_pid;
            psysperfinfo->PERF_ulSymAddr = sysperfinfo.PERF_ulSymAddr;
            psysperfinfo->PERF_ulSamAddr = sysperfinfo.PERF_ulSamAddr;
            lib_strcpy(psysperfinfo->PERF_cFunc, sysperfinfo.PERF_cFunc);
        }
        
        psysperfinfoCmp = (PLW_SYSPERF_INFO)LW_SYSPERF_HEADER();        /*  重新插入链表                */
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
        LW_SYSPERF_UNLOCK();                                            /*  解锁                        */
    }
    
    LW_THREAD_UNSAFE();                                                 /*  退出安全模式                */
    
    return  (LW_NULL);
}

#endif                                                                  /*  LW_CFG_SYSPERF_EN > 0       */
                                                                        /*  LW_CFG_MODULELOADER_EN > 0  */
/*********************************************************************************************************
  END
*********************************************************************************************************/
