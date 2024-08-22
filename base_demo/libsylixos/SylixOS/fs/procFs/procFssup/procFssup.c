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
** 文   件   名: procFssup.c
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2013 年 08 月 13 日
**
** 描        述: proc 文件系统对文件系统信息的获取.
*********************************************************************************************************/
#define  __SYLIXOS_STDIO
#define  __SYLIXOS_KERNEL
#include "../SylixOS/kernel/include/k_kernel.h"
#include "../SylixOS/system/include/s_system.h"
#include "../procFs.h"
/*********************************************************************************************************
  裁剪宏
*********************************************************************************************************/
#if LW_CFG_PROCFS_EN > 0
#if LW_CFG_DWATCH_EN > 0
#include "sys/dwatch.h"
#endif
/*********************************************************************************************************
  文件系统信息 proc 文件函数声明
*********************************************************************************************************/
static ssize_t  __procFssupRead(PLW_PROCFS_NODE  p_pfsn, 
                                PCHAR            pcBuffer, 
                                size_t           stMaxBytes,
                                off_t            oft);
static ssize_t  __procFssupStatRead(PLW_PROCFS_NODE  p_pfsn, 
                                    PCHAR            pcBuffer, 
                                    size_t           stMaxBytes,
                                    off_t            oft);
#if LW_CFG_DWATCH_EN > 0
static ssize_t  __procFssupDWatchRead(PLW_PROCFS_NODE  p_pfsn,
                                      PCHAR            pcBuffer,
                                      size_t           stMaxBytes,
                                      off_t            oft);
#endif                                                                  /*  LW_CFG_DWATCH_EN > 0        */
/*********************************************************************************************************
  文件系统信息 proc 文件操作函数组
*********************************************************************************************************/
static LW_PROCFS_NODE_OP    _G_pfsnoFssupFuncs = {
    __procFssupRead, LW_NULL
};
static LW_PROCFS_NODE_OP    _G_pfsnoFssupRootfsFuncs = {
    __procFssupStatRead, LW_NULL
};
static LW_PROCFS_NODE_OP    _G_pfsnoFssupProcfsFuncs = {
    __procFssupStatRead, LW_NULL
};
#if LW_CFG_DWATCH_EN > 0
static LW_PROCFS_NODE_OP    _G_pfsnoFssupDWatchFuncs = {
    __procFssupDWatchRead, LW_NULL
};
#endif                                                                  /*  LW_CFG_DWATCH_EN > 0        */
/*********************************************************************************************************
  文件系统信息 proc 文件目录树
*********************************************************************************************************/
#define __PROCFS_BUFFER_SIZE_FSSUP      256
#define __PROCFS_BUFFER_SIZE_ROOTFS     128
#define __PROCFS_BUFFER_SIZE_PROCFS     128

static LW_PROCFS_NODE           _G_pfsnFssup[] = 
{
    LW_PROCFS_INIT_NODE("fs",  
                        (S_IFDIR | S_IRUSR | S_IRGRP | S_IROTH), 
                        LW_NULL, 
                        LW_NULL,  
                        0),
                        
    LW_PROCFS_INIT_NODE("rootfs",  
                        (S_IFDIR | S_IRUSR | S_IRGRP | S_IROTH), 
                        LW_NULL, 
                        LW_NULL,  
                        0),
                        
    LW_PROCFS_INIT_NODE("procfs",  
                        (S_IFDIR | S_IRUSR | S_IRGRP | S_IROTH), 
                        LW_NULL, 
                        LW_NULL,  
                        0),
                        
    LW_PROCFS_INIT_NODE("fssup", 
                        (S_IFREG | S_IRUSR | S_IRGRP | S_IROTH), 
                        &_G_pfsnoFssupFuncs, 
                        "F",
                        __PROCFS_BUFFER_SIZE_FSSUP),
                        
    LW_PROCFS_INIT_NODE("stat", 
                        (S_IFREG | S_IRUSR | S_IRGRP | S_IROTH), 
                        &_G_pfsnoFssupRootfsFuncs, 
                        "M",
                        __PROCFS_BUFFER_SIZE_ROOTFS),
                        
    LW_PROCFS_INIT_NODE("stat", 
                        (S_IFREG | S_IRUSR | S_IRGRP | S_IROTH), 
                        &_G_pfsnoFssupProcfsFuncs, 
                        "M",
                        __PROCFS_BUFFER_SIZE_PROCFS),

#if LW_CFG_DWATCH_EN > 0
    LW_PROCFS_INIT_NODE("dwatch",
                        (S_IFREG | S_IRUSR | S_IRGRP | S_IROTH),
                        &_G_pfsnoFssupDWatchFuncs,
                        "D",
                        0),
#endif                                                                  /*  LW_CFG_DWATCH_EN > 0        */
};
/*********************************************************************************************************
** 函数名称: __procFssupRootfsRead
** 功能描述: procfs 读一个根文件系统支持信息 proc 文件
** 输　入  : p_pfsn        节点控制块
**           pcBuffer      缓冲区
**           stMaxBytes    缓冲区大小
**           oft           文件指针
** 输　出  : 实际读取的数目
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static ssize_t  __procFssupRead (PLW_PROCFS_NODE  p_pfsn, 
                                 PCHAR            pcBuffer, 
                                 size_t           stMaxBytes,
                                 off_t            oft)
{
    REGISTER PCHAR      pcFileBuffer;
             size_t     stRealSize;                                     /*  实际的文件内容大小          */
             size_t     stCopeBytes;

    /*
     *  程序运行到这里, 文件缓冲一定已经分配了预置的内存大小(创建节点时预置大小为 256 字节).
     */
    pcFileBuffer = (PCHAR)API_ProcFsNodeBuffer(p_pfsn);
    if (pcFileBuffer == LW_NULL) {
        _ErrorHandle(ENOMEM);
        return  (0);
    }
     
    stRealSize = API_ProcFsNodeGetRealFileSize(p_pfsn);
    if (stRealSize == 0) {                                              /*  需要生成文件                */
        lib_strcpy(pcFileBuffer, "rootfs procfs ramfs romfs ");
        
#if LW_CFG_FATFS_EN > 0
        lib_strlcat(pcFileBuffer, "vfat ", __PROCFS_BUFFER_SIZE_FSSUP);
#endif                                                                  /*  LW_CFG_FATFS_EN             */
        
#if LW_CFG_NFS_EN > 0
        lib_strlcat(pcFileBuffer, "nfs ", __PROCFS_BUFFER_SIZE_FSSUP);
#endif                                                                  /*  LW_CFG_NFS_EN               */

#if LW_CFG_YAFFS_EN > 0
        lib_strlcat(pcFileBuffer, "yaffs ", __PROCFS_BUFFER_SIZE_FSSUP);
#endif                                                                  /*  LW_CFG_YAFFS_EN             */
        
#if LW_CFG_TPSFS_EN > 0
        lib_strlcat(pcFileBuffer, "tpsfs ", __PROCFS_BUFFER_SIZE_FSSUP);
#endif                                                                  /*  LW_CFG_TPSFS_EN             */

#if LW_CFG_ISO9660FS_EN > 0
        lib_strlcat(pcFileBuffer, "iso9660 ", __PROCFS_BUFFER_SIZE_FSSUP);
#endif                                                                  /*  LW_CFG_ISO9660FS_EN         */

        stRealSize = lib_strlen(pcFileBuffer);
        
        API_ProcFsNodeSetRealFileSize(p_pfsn, stRealSize);
    }
    if (oft >= stRealSize) {
        _ErrorHandle(ENOSPC);
        return  (0);
    }
    
    stCopeBytes  = __MIN(stMaxBytes, (size_t)(stRealSize - oft));       /*  计算实际拷贝的字节数        */
    lib_memcpy(pcBuffer, (CPVOID)(pcFileBuffer + oft), (UINT)stCopeBytes);
    
    return  ((ssize_t)stCopeBytes);
}
/*********************************************************************************************************
** 函数名称: __procFssupRootfsRead
** 功能描述: procfs 读一个根文件系统信息 proc 文件
** 输　入  : p_pfsn        节点控制块
**           pcBuffer      缓冲区
**           stMaxBytes    缓冲区大小
**           oft           文件指针
** 输　出  : 实际读取的数目
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static ssize_t  __procFssupStatRead (PLW_PROCFS_NODE  p_pfsn, 
                                     PCHAR            pcBuffer, 
                                     size_t           stMaxBytes,
                                     off_t            oft)
{
    REGISTER PCHAR      pcFileBuffer;
             size_t     stRealSize;                                     /*  实际的文件内容大小          */
             size_t     stCopeBytes;

    /*
     *  程序运行到这里, 文件缓冲一定已经分配了预置的内存大小(创建节点时预置大小为 64 字节).
     */
    pcFileBuffer = (PCHAR)API_ProcFsNodeBuffer(p_pfsn);
    if (pcFileBuffer == LW_NULL) {
        _ErrorHandle(ENOMEM);
        return  (0);
    }
     
    stRealSize = API_ProcFsNodeGetRealFileSize(p_pfsn);
    if (stRealSize == 0) {                                              /*  需要生成文件                */
        struct statfs  statfsBuf;
        PCHAR          pcFs;
        
        if (p_pfsn->PFSN_p_pfsnoFuncs == &_G_pfsnoFssupRootfsFuncs) {
            pcFs = "/";
        } else if (p_pfsn->PFSN_p_pfsnoFuncs == &_G_pfsnoFssupProcfsFuncs) {
            pcFs = "/proc";
        } else {
            pcFs = "/";                                                 /*  不可能运行到这里            */
        }
        
        if (statfs(pcFs, &statfsBuf) < ERROR_NONE) {
            stRealSize = bnprintf(pcFileBuffer, 
                                  __PROCFS_BUFFER_SIZE_ROOTFS, 0,
                                  "get root file system status error.");
        } else {
            stRealSize = bnprintf(pcFileBuffer, 
                                  __PROCFS_BUFFER_SIZE_ROOTFS, 0,
                                  "memory used: %ld bytes\n"
                                  "total files: %ld\n",
                                  (ULONG)(statfsBuf.f_bsize * statfsBuf.f_blocks),
                                  statfsBuf.f_files);
        }
        API_ProcFsNodeSetRealFileSize(p_pfsn, stRealSize);
    }
    if (oft >= stRealSize) {
        _ErrorHandle(ENOSPC);
        return  (0);
    }
    
    stCopeBytes  = __MIN(stMaxBytes, (size_t)(stRealSize - oft));       /*  计算实际拷贝的字节数        */
    lib_memcpy(pcBuffer, (CPVOID)(pcFileBuffer + oft), (UINT)stCopeBytes);
    
    return  ((ssize_t)stCopeBytes);
}
/*********************************************************************************************************
** 函数名称: __procFssupDWatchRead
** 功能描述: procfs 读一个根文件系统信息 dwatch 文件
** 输　入  : p_pfsn        节点控制块
**           pcBuffer      缓冲区
**           stMaxBytes    缓冲区大小
**           oft           文件指针
** 输　出  : 实际读取的数目
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_DWATCH_EN > 0

static ssize_t  __procFssupDWatchRead (PLW_PROCFS_NODE  p_pfsn,
                                       PCHAR            pcBuffer,
                                       size_t           stMaxBytes,
                                       off_t            oft)
{
#define __DWATCH_LOCK(pdwatchdev)     API_SemaphoreMPend(pdwatchdev->DW_ulLock, LW_OPTION_WAIT_INFINITE)
#define __DWATCH_UNLOCK(pdwatchdev)   API_SemaphoreMPost(pdwatchdev->DW_ulLock)

    const CHAR      cDWatchInfoHdr[] =
    "EVENTS SUB  Q-LEN            PATH\n"
    "------ --- ------- -------------------------\n";
          PCHAR     pcFileBuffer;
          size_t    stRealSize;                                         /*  实际的文件内容大小          */
          size_t    stCopeBytes;

    /*
     *  由于预置内存大小为 0 , 所以打开后第一次读取需要手动开辟内存.
     */
    pcFileBuffer = (PCHAR)API_ProcFsNodeBuffer(p_pfsn);
    if (pcFileBuffer == LW_NULL) {                                      /*  还没有分配内存              */
        size_t              stNeedBufferSize;
        CHAR                cEvents[10];
        PLW_LIST_LINE       pline;
        PLW_DWATCH_DEV      pdwatchdev;
        PLW_DWATCH_FILE     pdwatchfil;

        pdwatchdev = (PLW_DWATCH_DEV)iosDevMatchFull(LW_DWATCH_DEV_PATH);
        if (!pdwatchdev) {
            return  (0);
        }

        stNeedBufferSize = sizeof(cDWatchInfoHdr);

        __DWATCH_LOCK(pdwatchdev);
        for (pline  = pdwatchdev->DW_plineFiles;
             pline != LW_NULL;
             pline  = _list_line_get_next(pline)) {

            pdwatchfil = (PLW_DWATCH_FILE)pline;
            if (pdwatchfil->DF_pdevhdr) {
                stNeedBufferSize += lib_strlen(pdwatchfil->DF_pdevhdr->DEVHDR_pcName)
                                  + pdwatchfil->DF_stDirPrefixLen
                                  + 24;
            }
        }
        __DWATCH_UNLOCK(pdwatchdev);

        if (API_ProcFsAllocNodeBuffer(p_pfsn, stNeedBufferSize)) {
            _ErrorHandle(ENOMEM);
            return  (0);
        }
        pcFileBuffer = (PCHAR)API_ProcFsNodeBuffer(p_pfsn);             /*  重新获得文件缓冲区地址      */

        stRealSize = bnprintf(pcFileBuffer, stNeedBufferSize, 0, cDWatchInfoHdr);

        __DWATCH_LOCK(pdwatchdev);
        for (pline  = pdwatchdev->DW_plineFiles;
             pline != LW_NULL;
             pline  = _list_line_get_next(pline)) {

            pdwatchfil = (PLW_DWATCH_FILE)pline;
            if (pdwatchfil->DF_pdevhdr) {
                cEvents[0] = '\0';
                if (pdwatchfil->DF_uiEvents & DWATCH_CREATE) {
                    lib_strcat(cEvents, "C");
                }
                if (pdwatchfil->DF_uiEvents & DWATCH_DELETE) {
                    lib_strcat(cEvents, "D");
                }
                if (pdwatchfil->DF_uiEvents & DWATCH_MODIFY) {
                    lib_strcat(cEvents, "M");
                }
                if (pdwatchfil->DF_uiEvents & DWATCH_ATTRIBUTES) {
                    lib_strcat(cEvents, "A");
                }
                if (pdwatchfil->DF_uiEvents & (DWATCH_MOVE_FROM | DWATCH_MOVE_TO)) {
                    lib_strcat(cEvents, "R");
                }

                stRealSize = bnprintf(pcFileBuffer, stNeedBufferSize, stRealSize,
                                      "%-6s %3s %7u %s%s\n",
                                      cEvents,
                                      (pdwatchfil->DF_uiEvents & DWATCH_SUBTREE) ? "YES" : "NO",
                                      pdwatchfil->DF_uiQueued, pdwatchfil->DF_pdevhdr->DEVHDR_pcName,
                                      pdwatchfil->DF_stDirPrefixLen ? pdwatchfil->DF_pcDirPrefix : "");
            }
        }
        __DWATCH_UNLOCK(pdwatchdev);

        API_ProcFsNodeSetRealFileSize(p_pfsn, stRealSize);
    } else {
        stRealSize = API_ProcFsNodeGetRealFileSize(p_pfsn);
    }
    if (oft >= stRealSize) {
        _ErrorHandle(ENOSPC);
        return  (0);
    }

    stCopeBytes  = __MIN(stMaxBytes, (size_t)(stRealSize - oft));       /*  计算实际拷贝的字节数        */
    lib_memcpy(pcBuffer, (CPVOID)(pcFileBuffer + oft), (UINT)stCopeBytes);

    return  ((ssize_t)stCopeBytes);
}

#endif                                                                  /*  LW_CFG_DWATCH_EN > 0        */
/*********************************************************************************************************
** 函数名称: __procFssupInit
** 功能描述: procfs 初始化文件系统信息 proc 文件
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  __procFssupInit (VOID)
{
    API_ProcFsMakeNode(&_G_pfsnFssup[0], "/");
    API_ProcFsMakeNode(&_G_pfsnFssup[1], "/fs");
    API_ProcFsMakeNode(&_G_pfsnFssup[2], "/fs");
    API_ProcFsMakeNode(&_G_pfsnFssup[3], "/fs");
    API_ProcFsMakeNode(&_G_pfsnFssup[4], "/fs/rootfs");
    API_ProcFsMakeNode(&_G_pfsnFssup[5], "/fs/procfs");
#if LW_CFG_DWATCH_EN > 0
    API_ProcFsMakeNode(&_G_pfsnFssup[6], "/fs");
#endif                                                                  /*  LW_CFG_DWATCH_EN > 0        */
}

#endif                                                                  /*  LW_CFG_PROCFS_EN > 0        */
/*********************************************************************************************************
  END
*********************************************************************************************************/
