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
** 文   件   名: memory_cfg.h
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2006 年 12 月 26 日
**
** 描        述: 这是系统内核内存基本配置文件。
*********************************************************************************************************/

#ifndef __MEMORY_CFG_H
#define __MEMORY_CFG_H

/*********************************************************************************************************
*                                            基本二进制大小单位定义
*********************************************************************************************************/

#ifndef LW_CFG_KB_SIZE
#define LW_CFG_KB_SIZE                              (1024)
#define LW_CFG_MB_SIZE                              (1024 * LW_CFG_KB_SIZE)
#define LW_CFG_GB_SIZE                              (1024 * LW_CFG_MB_SIZE)
#endif

/*********************************************************************************************************
*                                          KERNEL THREAD & INTERUPT STACK 
*                                       (不包括网络与其他扩展子系统相关线程)
*********************************************************************************************************/

#if LW_CFG_CPU_WORD_LENGHT == 32
#define LW_CFG_INT_STK_SIZE                         (8 * LW_CFG_KB_SIZE)/*  系统中断堆栈大小 (字节)     */
#define LW_CFG_THREAD_DEFAULT_STK_SIZE              (4 * LW_CFG_KB_SIZE)/*  系统默认堆栈大小            */
#define LW_CFG_THREAD_DEFAULT_GUARD_SIZE            (1 * LW_CFG_KB_SIZE)/*  堆栈警戒区大小              */

#define LW_CFG_THREAD_IDLE_STK_SIZE                 (4 * LW_CFG_KB_SIZE)/*  系统空闲任务堆栈大小        */
#define LW_CFG_THREAD_SIG_STK_SIZE                  (4 * LW_CFG_KB_SIZE)/*  系统信号管理任务堆栈大小    */
#define LW_CFG_THREAD_DEFER_STK_SIZE                (4 * LW_CFG_KB_SIZE)/*  延迟中断管理任务堆栈大小    */
#define LW_CFG_THREAD_LOG_STK_SIZE                  (4 * LW_CFG_KB_SIZE)/*  系统日志管理任务堆栈大小    */
#define LW_CFG_THREAD_ITMR_STK_SIZE                 (4 * LW_CFG_KB_SIZE)/*  系统任务定时器服务堆栈大小  */
#define LW_CFG_THREAD_POWERM_STK_SIZE               (4 * LW_CFG_KB_SIZE)/*  系统功耗管理器线程堆栈大小  */
#define LW_CFG_THREAD_DISKCACHE_STK_SIZE            (4 * LW_CFG_KB_SIZE)/*  系统磁盘缓冲背景线程堆栈大小*/
#define LW_CFG_THREAD_HOTPLUG_STK_SIZE              (8 * LW_CFG_KB_SIZE)/*  系统热插拔背景线程堆栈大小  */
#define LW_CFG_THREAD_RECLAIM_STK_SIZE             (16 * LW_CFG_KB_SIZE)/*  系统资源回收线程堆栈大小    */

#else
#define LW_CFG_INT_STK_SIZE                         (8 * LW_CFG_KB_SIZE)/*  系统中断堆栈大小 (字节)     */
#define LW_CFG_THREAD_DEFAULT_STK_SIZE              (8 * LW_CFG_KB_SIZE)/*  系统默认堆栈大小            */
#define LW_CFG_THREAD_DEFAULT_GUARD_SIZE            (2 * LW_CFG_KB_SIZE)/*  堆栈警戒区大小              */

#define LW_CFG_THREAD_IDLE_STK_SIZE                 (8 * LW_CFG_KB_SIZE)/*  系统空闲任务堆栈大小        */
#define LW_CFG_THREAD_SIG_STK_SIZE                  (8 * LW_CFG_KB_SIZE)/*  系统信号管理任务堆栈大小    */
#define LW_CFG_THREAD_DEFER_STK_SIZE                (8 * LW_CFG_KB_SIZE)/*  延迟中断管理任务堆栈大小    */
#define LW_CFG_THREAD_LOG_STK_SIZE                  (8 * LW_CFG_KB_SIZE)/*  系统日志管理任务堆栈大小    */
#define LW_CFG_THREAD_ITMR_STK_SIZE                 (8 * LW_CFG_KB_SIZE)/*  系统任务定时器服务堆栈大小  */
#define LW_CFG_THREAD_POWERM_STK_SIZE               (8 * LW_CFG_KB_SIZE)/*  系统功耗管理器线程堆栈大小  */
#define LW_CFG_THREAD_DISKCACHE_STK_SIZE            (8 * LW_CFG_KB_SIZE)/*  系统磁盘缓冲背景线程堆栈大小*/
#define LW_CFG_THREAD_HOTPLUG_STK_SIZE             (16 * LW_CFG_KB_SIZE)/*  系统热插拔背景线程堆栈大小  */
#define LW_CFG_THREAD_RECLAIM_STK_SIZE             (32 * LW_CFG_KB_SIZE)/*  系统资源回收线程堆栈大小    */
#endif

/*********************************************************************************************************
*                                          STDIO DEFAULT BUFFER SIZE
*********************************************************************************************************/

#define LW_CFG_STDIO_BUFFER_SIZE                    512                 /*  标准缓冲 IO 初始缓冲大小    */

/*********************************************************************************************************
*                                          HEAP 
* 注意: malloc 分配默认对齐选项
*********************************************************************************************************/

#if LW_CFG_CPU_WORD_LENGHT == 32
#define LW_CFG_HEAP_ALIGNMENT                       8                   /*  默认内存对齐关系            */
#else
#define LW_CFG_HEAP_ALIGNMENT                       16                  /*  默认内存对齐关系            */
#endif
#define LW_CFG_HEAP_SEG_MIN_SIZE                    16                  /*  分段内存最小值              */
                                                                        /*  必须为对齐关系整数倍        */
/*********************************************************************************************************
*                                      HEAP TYPE CONFIG
* 0 :  使用宏配置的方式, 静态指定系统内存堆的位置和大小.
* 1 :  启动操作系统时, 传入指定的堆内存参数, 动态配置系统内存堆的位置与大小.
*********************************************************************************************************/

#define LW_CFG_MEMORY_HEAP_CONFIG_TYPE              (1)                 /*  内存堆的配置方式            */

/*********************************************************************************************************
*                                      KERNEL HEAP
*********************************************************************************************************/

#define LW_CFG_MEMORY_KERNEL_HEAP_ADDRESS           (0)                 /*  内核内存堆起始地址          */
                                                                        /*  0 表示自动确定起始位置      */
#define LW_CFG_MEMORY_KERNEL_HEAP_SIZE_BYTE         (0)                 /*  内核内存堆大小              */

/*********************************************************************************************************
*                                      SYSTEM HEAP
*********************************************************************************************************/

#define LW_CFG_MEMORY_SYSTEM_HEAP_ADDRESS           (0)                 /*  系统内存堆起始地址          */
                                                                        /*  0 表示自动确定起始位置      */
#define LW_CFG_MEMORY_SYSTEM_HEAP_SIZE_BYTE         (0)                 /*  系统内存堆大小              */
                                                                        
/*********************************************************************************************************
*                                       HEAP security
*********************************************************************************************************/

#define LW_CFG_USER_HEAP_SAFETY                     0                   /*  RegionPut 与 free 调用是否  */
                                                                        /*  进行安全性检查              */
#endif                                                                  /*  __MEMORY_CFG_H              */
/*********************************************************************************************************
  END
*********************************************************************************************************/
