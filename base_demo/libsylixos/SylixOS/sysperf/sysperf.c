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
** 文   件   名: sysperf.c
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2016 年 07 月 28 日
**
** 描        述: 系统性能分析工具.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
  裁剪支持
*********************************************************************************************************/
#if (LW_CFG_SYSPERF_EN > 0) && (LW_CFG_MODULELOADER_EN > 0)
#include "sysperf.h"
#include "sysperfLib.h"
/*********************************************************************************************************
  全局变量
*********************************************************************************************************/
extern LW_SYSPERF_PARAM             _G_sysperfparam;                    /*  系统参数                    */
/*********************************************************************************************************
  宏操作
*********************************************************************************************************/
#define LW_SYSPERF_STATE()          (_G_sysperfparam.PERF_iState)
#define LW_SYSPERF_COLLECTOR()      (_G_sysperfparam.PERF_ulThread)
#define LW_SYSPERF_MSGQ()           (_G_sysperfparam.PERF_ulMsgQ)
#define LW_SYSPERF_MUTEX()          (_G_sysperfparam.PERF_ulLock)
#define LW_SYSPERF_HEADER()         (_G_sysperfparam.PERF_pringInfo)
#define LW_SYSPERF_GETNUM()         (_G_sysperfparam.PERF_ulGetNum)
#define LW_SYSPERF_BUFFER()         (_G_sysperfparam.PERF_pvBuffer)

#define LW_SYSPERF_LOCK()           API_SemaphoreMPend(LW_SYSPERF_MUTEX(), LW_OPTION_WAIT_INFINITE)
#define LW_SYSPERF_UNLOCK()         API_SemaphoreMPost(LW_SYSPERF_MUTEX())
/*********************************************************************************************************
** 函数名称: API_SysPerfStart
** 功能描述: 启动系统性能分析器.
** 输　入  : uiPipeLen         测量管道深度      (推荐 1024 ~ 4096)
**           uiPerfNum         性能分析表大小    (推荐 10   ~ 30)
**           uiRefreshPeriod   数据刷新周期      (推荐 1    ~ 5 秒)
** 输　出  : ERROR or OK
** 全局变量: 
** 调用模块: 
                                           API 函数
*********************************************************************************************************/
LW_API 
INT  API_SysPerfStart (UINT  uiPipeLen, UINT  uiPerfNum, UINT  uiRefreshPeriod)
{
    INT                 i;
    PLW_SYSPERF_INFO    psysperfinfo;
    LW_CLASS_THREADATTR threadattr;

    if (LW_SYSPERF_MUTEX() == LW_OBJECT_HANDLE_INVALID) {
        LW_SYSPERF_MUTEX() =  API_SemaphoreMCreate("sysperf_lock", LW_PRIO_DEF_CEILING,
                                                   LW_OPTION_DELETE_SAFE | 
                                                   LW_OPTION_INHERIT_PRIORITY |
                                                   LW_OPTION_OBJECT_GLOBAL, LW_NULL);
    }
    
    if (LW_SYSPERF_STATE() != LW_SYSPERF_END) {
        _ErrorHandle(EBUSY);
        return  (PX_ERROR);
    }
    
    if ((uiPipeLen < 128) || (uiPipeLen > 4096)) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }
    
    if (uiPerfNum < 10) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }
    
    LW_SYSPERF_STATE()  = LW_SYSPERF_ON;
    LW_SYSPERF_GETNUM() = uiPerfNum;
    uiPerfNum           = uiPerfNum * 2;                                /*  保存两倍的节点个数          */
    
    LW_SYSPERF_BUFFER() = __SHEAP_ZALLOC(sizeof(LW_SYSPERF_INFO) * uiPerfNum);
    if (!LW_SYSPERF_BUFFER()) {
        LW_SYSPERF_STATE() = LW_SYSPERF_END;
        _ErrorHandle(ENOMEM);
        return  (PX_ERROR);
    }
    
    psysperfinfo = (PLW_SYSPERF_INFO)LW_SYSPERF_BUFFER();
    
    for (i = 0; i < uiPerfNum; i++) {
        _List_Ring_Add_Ahead(&psysperfinfo->PERF_ringManage, 
                             &LW_SYSPERF_HEADER());
        psysperfinfo++;
    }
    
    LW_SYSPERF_MSGQ() = API_MsgQueueCreate("sysperf_q", (ULONG)uiPipeLen,
                                           sizeof(LW_SYSPERF_NODE), 
                                           LW_OPTION_OBJECT_GLOBAL, LW_NULL);
    if (LW_SYSPERF_MSGQ() == LW_OBJECT_HANDLE_INVALID) {
        LW_SYSPERF_STATE() = LW_SYSPERF_END;
        __SHEAP_FREE(LW_SYSPERF_BUFFER());
        return  (PX_ERROR);
    }
    
    API_ThreadAttrBuild(&threadattr,
                        LW_CFG_THREAD_SIG_STK_SIZE,
                        LW_PRIO_T_SERVICE,
                        LW_OPTION_THREAD_STK_CHK | 
                        LW_OPTION_THREAD_SAFE |
                        LW_OPTION_OBJECT_GLOBAL,
                        LW_NULL);
                        
    LW_SYSPERF_COLLECTOR() = API_ThreadCreate("t_sysperf", __sysperfThread, 
                                              &threadattr, LW_NULL);
    if (LW_SYSPERF_COLLECTOR() == LW_OBJECT_HANDLE_INVALID) {
        LW_SYSPERF_STATE() = LW_SYSPERF_END;
        API_MsgQueueDelete(&LW_SYSPERF_MSGQ());
        __SHEAP_FREE(LW_SYSPERF_BUFFER());
        return  (PX_ERROR);
    }
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: API_SysPerfStop
** 功能描述: 关闭系统性能分析器.
** 输　入  : NONE
** 输　出  : ERROR or OK
** 全局变量: 
** 调用模块: 
                                           API 函数
*********************************************************************************************************/
LW_API 
INT  API_SysPerfStop (VOID)
{
    if (LW_SYSPERF_STATE() != LW_SYSPERF_ON) {
        _ErrorHandle(ESRCH);
        return  (PX_ERROR);
    }
    
    LW_SYSPERF_STATE() = LW_SYSPERF_OFF;
    API_MsgQueueDelete(&LW_SYSPERF_MSGQ());
    API_ThreadJoin(LW_SYSPERF_COLLECTOR(), LW_NULL);
    
    LW_SYSPERF_LOCK();
    LW_SYSPERF_COLLECTOR() = LW_OBJECT_HANDLE_INVALID;
    LW_SYSPERF_HEADER()    = LW_NULL;
    LW_SYSPERF_GETNUM()    = 0;
    LW_SYSPERF_UNLOCK();
    
    __SHEAP_FREE(LW_SYSPERF_BUFFER());
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: API_SysPerfRefresh
** 功能描述: 刷新系统性能分析器.
** 输　入  : NONE
** 输　出  : ERROR or OK
** 全局变量: 
** 调用模块: 
                                           API 函数
*********************************************************************************************************/
LW_API 
INT  API_SysPerfRefresh (VOID)
{
    PLW_SYSPERF_INFO    psysperfinfo;
    PLW_LIST_RING       pringTemp;

    if (LW_SYSPERF_STATE() != LW_SYSPERF_ON) {
        _ErrorHandle(ESRCH);
        return  (PX_ERROR);
    }
    
    LW_SYSPERF_LOCK();
    if (LW_SYSPERF_STATE() != LW_SYSPERF_ON) {
        LW_SYSPERF_UNLOCK();
        _ErrorHandle(ESRCH);
        return  (PX_ERROR);
    }
    
    pringTemp = LW_SYSPERF_HEADER();
    do {
        psysperfinfo = (PLW_SYSPERF_INFO)pringTemp;
        psysperfinfo->PERF_ulCounter = 0;
        pringTemp = _list_ring_get_next(pringTemp);
    } while (pringTemp != LW_SYSPERF_HEADER());
    LW_SYSPERF_UNLOCK();
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: API_SysPerfInfo
** 功能描述: 获得系统性能分析数据.
** 输　入  : psysperf      读取缓冲区
**           uiPerfNum     缓冲区缓冲个数
** 输　出  : 获取的个数
** 全局变量: 
** 调用模块: 
                                           API 函数
*********************************************************************************************************/
LW_API 
INT  API_SysPerfInfo (PLW_SYSPERF_INFO  psysperf, UINT  uiPerfNum)
{
    UINT                i = 0;
    UINT                uiMax;
    PLW_SYSPERF_INFO    psysperfinfo;
    PLW_LIST_RING       pringTemp;
    
    if (LW_SYSPERF_STATE() != LW_SYSPERF_ON) {
        _ErrorHandle(ESRCH);
        return  (PX_ERROR);
    }
    
    if (!psysperf || (uiPerfNum < 1)) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }
    
    LW_SYSPERF_LOCK();
    uiMax = __MAX(uiPerfNum, LW_SYSPERF_GETNUM());
    
    pringTemp = LW_SYSPERF_HEADER();
    do {
        psysperfinfo = (PLW_SYSPERF_INFO)pringTemp;
        if (psysperfinfo->PERF_ulCounter) {
            *psysperf = *psysperfinfo;
            psysperf++;
            i++;
        }
        pringTemp = _list_ring_get_next(pringTemp);
    } while (pringTemp != LW_SYSPERF_HEADER() && (i < uiMax));
    LW_SYSPERF_UNLOCK();
    
    return  (i);
}

#endif                                                                  /*  LW_CFG_SYSPERF_EN > 0       */
                                                                        /*  LW_CFG_MODULELOADER_EN > 0  */
/*********************************************************************************************************
  END
*********************************************************************************************************/
