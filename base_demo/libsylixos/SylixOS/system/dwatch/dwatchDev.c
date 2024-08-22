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
** 文   件   名: dwatchDev.c
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2023 年 06 月 21 日
**
** 描        述: 目录监控器.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
  裁剪控制
*********************************************************************************************************/
#if (LW_CFG_DEVICE_EN > 0) && (LW_CFG_DWATCH_EN > 0)
#include "sys/dwatch.h"
/*********************************************************************************************************
  驱动程序全局变量
*********************************************************************************************************/
static INT              _G_iDWatchDrvNum = PX_ERROR;
static LW_DWATCH_DEV    _G_dwatchdev;
static LW_OBJECT_HANDLE _G_hDWatchSelMutex;
/*********************************************************************************************************
  驱动程序
*********************************************************************************************************/
static LONG     _dwatchOpen(PLW_DWATCH_DEV    pdwatchdev, PCHAR  pcName, INT  iFlags, INT  iMode);
static INT      _dwatchClose(PLW_DWATCH_FILE  pdwatchfil);
static ssize_t  _dwatchRead(PLW_DWATCH_FILE   pdwatchfil, PCHAR  pcBuffer, size_t  stMaxBytes);
static ssize_t  _dwatchWrite(PLW_DWATCH_FILE  pdwatchfil, PCHAR  pcBuffer, size_t  stNBytes);
static INT      _dwatchIoctl(PLW_DWATCH_FILE  pdwatchfil, INT    iRequest, LONG  lArg);
/*********************************************************************************************************
  全局锁操作
*********************************************************************************************************/
#define DWATCH_IO_LOCK()            _IosLock()
#define DWATCH_IO_UNLOCK()          _IosUnlock()
/*********************************************************************************************************
  DWATCH 锁操作
*********************************************************************************************************/
#define DWATCH_LOCK()               API_SemaphoreMPend(_G_dwatchdev.DW_ulLock, LW_OPTION_WAIT_INFINITE)
#define DWATCH_UNLOCK()             API_SemaphoreMPost(_G_dwatchdev.DW_ulLock)
#define DWATCH_FILE_LOCK(file)      API_SemaphoreMPend(file->DF_ulLock, LW_OPTION_WAIT_INFINITE)
#define DWATCH_FILE_UNLOCK(file)    API_SemaphoreMPost(file->DF_ulLock)
/*********************************************************************************************************
  缓冲大小计算
*********************************************************************************************************/
#define DWATCH_NREAD(pwatchevt)     ROUND_UP(sizeof(struct dwatch_event) + \
                                             (pwatchevt)->DE_stNameLen + 1, sizeof(LW_STACK))
/*********************************************************************************************************
** 函数名称: API_DWatchDrvInstall
** 功能描述: 安装 dwatch 设备驱动程序
** 输　入  : NONE
** 输　出  : 驱动是否安装成功
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
LW_API
INT  API_DWatchDrvInstall (VOID)
{
    if (_G_iDWatchDrvNum <= 0) {
        _G_iDWatchDrvNum  = iosDrvInstall(LW_NULL,
                                          LW_NULL,
                                          _dwatchOpen,
                                          _dwatchClose,
                                          _dwatchRead,
                                          _dwatchWrite,
                                          _dwatchIoctl);
        DRIVER_LICENSE(_G_iDWatchDrvNum,     "GPL->Ver 2.0");
        DRIVER_AUTHOR(_G_iDWatchDrvNum,      "Han.hui");
        DRIVER_DESCRIPTION(_G_iDWatchDrvNum, "DWatch driver.");
    }

    if (_G_hDWatchSelMutex == LW_OBJECT_HANDLE_INVALID) {
        _G_hDWatchSelMutex =  API_SemaphoreMCreate("dwatchsel_lock", LW_PRIO_DEF_CEILING,
                                                   LW_OPTION_WAIT_PRIORITY | LW_OPTION_DELETE_SAFE |
                                                   LW_OPTION_INHERIT_PRIORITY | LW_OPTION_OBJECT_GLOBAL,
                                                   LW_NULL);
    }

    return  ((_G_hDWatchSelMutex == (PX_ERROR)) ? (PX_ERROR) : (ERROR_NONE));
}
/*********************************************************************************************************
** 函数名称: API_DWatchDevCreate
** 功能描述: 安装 dwatch 设备
** 输　入  : NONE
** 输　出  : 设备是否创建成功
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
LW_API
INT  API_DWatchDevCreate (VOID)
{
    if (_G_iDWatchDrvNum <= 0) {
        _DebugHandle(__ERRORMESSAGE_LEVEL, "no driver.\r\n");
        _ErrorHandle(ERROR_IO_NO_DRIVER);
        return  (PX_ERROR);
    }

    _G_dwatchdev.DW_ulLock = API_SemaphoreMCreate("dwatch_lock", LW_PRIO_DEF_CEILING,
                                                  LW_OPTION_WAIT_PRIORITY | LW_OPTION_DELETE_SAFE |
                                                  LW_OPTION_INHERIT_PRIORITY | LW_OPTION_OBJECT_GLOBAL,
                                                  LW_NULL);
    if (_G_dwatchdev.DW_ulLock == LW_OBJECT_HANDLE_INVALID) {
        return  (PX_ERROR);
    }

    if (iosDevAddEx(&_G_dwatchdev.DW_devhdrHdr, LW_DWATCH_DEV_PATH,
                    _G_iDWatchDrvNum, DT_CHR) != ERROR_NONE) {
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: _dwatchOpen
** 功能描述: 打开 dwatch 设备
** 输　入  : pdwatchdev       dwatch 设备
**           pcName           名称
**           iFlags           方式
**           iMode            方法
** 输　出  : ERROR
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LONG  _dwatchOpen (PLW_DWATCH_DEV  pdwatchdev, PCHAR  pcName, INT  iFlags, INT  iMode)
{
    PLW_DWATCH_FILE  pdwatchfil;

    if (pcName == LW_NULL) {
        _DebugHandle(__ERRORMESSAGE_LEVEL, "device name invalidate.\r\n");
        _ErrorHandle(ERROR_IO_NO_DEVICE_NAME_IN_PATH);
        return  (PX_ERROR);

    } else {
        if (iFlags & O_CREAT) {
            _ErrorHandle(ERROR_IO_FILE_EXIST);
            return  (PX_ERROR);
        }

        pdwatchfil = (PLW_DWATCH_FILE)__SHEAP_ALLOC(sizeof(LW_DWATCH_FILE));
        if (!pdwatchfil) {
            _DebugHandle(__ERRORMESSAGE_LEVEL, "system low memory.\r\n");
            _ErrorHandle(ERROR_SYSTEM_LOW_MEMORY);
            return  (PX_ERROR);
        }
        lib_bzero(pdwatchfil, sizeof(LW_DWATCH_FILE));

        pdwatchfil->DF_ulLock = API_SemaphoreMCreate("dwatchf_lock", LW_PRIO_DEF_CEILING,
                                                     LW_OPTION_WAIT_PRIORITY | LW_OPTION_DELETE_SAFE |
                                                     LW_OPTION_INHERIT_PRIORITY | LW_OPTION_OBJECT_GLOBAL,
                                                     LW_NULL);
        if (pdwatchfil->DF_ulLock == LW_OBJECT_HANDLE_INVALID) {
            __SHEAP_FREE(pdwatchfil);
            return  (PX_ERROR);
        }

        pdwatchfil->DF_ulRdSync = API_SemaphoreBCreate("dwatchf_rsync", LW_FALSE,
                                                       LW_OPTION_WAIT_PRIORITY |
                                                       LW_OPTION_OBJECT_GLOBAL, LW_NULL);
        if (pdwatchfil->DF_ulRdSync == LW_OBJECT_HANDLE_INVALID) {
            API_SemaphoreMDelete(&pdwatchfil->DF_ulLock);
            __SHEAP_FREE(pdwatchfil);
            return  (PX_ERROR);
        }

        pdwatchfil->DF_iFlags                     = iFlags;
        pdwatchfil->DF_selwulist.SELWUL_hListLock = _G_hDWatchSelMutex;

        DWATCH_LOCK();
        _List_Line_Add_Ahead(&pdwatchfil->DF_lineList, &pdwatchdev->DW_plineFiles);
        DWATCH_UNLOCK();

        LW_DEV_INC_USE_COUNT(&_G_dwatchdev.DW_devhdrHdr);

        return  ((LONG)pdwatchfil);
    }
}
/*********************************************************************************************************
** 函数名称: _dwatchClose
** 功能描述: 关闭 dwatch 文件
** 输　入  : pdwatchfil         dwatch 文件
** 输　出  : ERROR
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  _dwatchClose (PLW_DWATCH_FILE  pdwatchfil)
{
    PLW_DWATCH_EVENT  pwatchevt;

    if (pdwatchfil == LW_NULL) {
        return  (PX_ERROR);
    }

    DWATCH_LOCK();
    _List_Line_Del(&pdwatchfil->DF_lineList, &_G_dwatchdev.DW_plineFiles);
    DWATCH_UNLOCK();

    SEL_WAKE_UP_TERM(&pdwatchfil->DF_selwulist);

    LW_DEV_DEC_USE_COUNT(&_G_dwatchdev.DW_devhdrHdr);

    DWATCH_FILE_LOCK(pdwatchfil);
    while (pdwatchfil->DF_pmonoHead) {
        pwatchevt = (PLW_DWATCH_EVENT)_list_mono_allocate_seq(&pdwatchfil->DF_pmonoHead,
                                                              &pdwatchfil->DF_pmonoTail);
        __SHEAP_FREE(pwatchevt);
    }
    DWATCH_FILE_UNLOCK(pdwatchfil);

    if (pdwatchfil->DF_pcDirPrefix) {
        __SHEAP_FREE(pdwatchfil->DF_pcDirPrefix);
    }

    DWATCH_IO_LOCK();
    if (pdwatchfil->DF_pdevhdr) {
        if (pdwatchfil->DF_pdevhdr->DEVHDR_ulDWatchs) {
            pdwatchfil->DF_pdevhdr->DEVHDR_ulDWatchs--;
        }
    }
    DWATCH_IO_UNLOCK();

    API_SemaphoreBDelete(&pdwatchfil->DF_ulRdSync);
    API_SemaphoreMDelete(&pdwatchfil->DF_ulLock);
    __SHEAP_FREE(pdwatchfil);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: _dwatchRead
** 功能描述: 读 dwatch 文件
** 输　入  : pdwatchfil       dwatch 文件
**           pcBuffer         接收缓冲区
**           stMaxBytes       接收缓冲区大小
** 输　出  : 读出的字节数
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static ssize_t  _dwatchRead (PLW_DWATCH_FILE  pdwatchfil, PCHAR  pcBuffer, size_t  stMaxBytes)
{
    ULONG                ulErrCode;
    ULONG                ulTimeout;
    size_t               stSize;
    ssize_t              sstNum;
    PLW_LIST_MONO        pmono;
    PLW_DWATCH_EVENT     pwatchevt;
    struct dwatch_event *dwevent;

    if (!pcBuffer || !ALIGNED(pcBuffer, sizeof(LW_STACK))) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    if (!stMaxBytes) {
        return  (0);
    }

    if (pdwatchfil->DF_iFlags & O_NONBLOCK) {                           /*  非阻塞 IO                   */
        ulTimeout = LW_OPTION_NOT_WAIT;
    } else {
        ulTimeout = LW_OPTION_WAIT_INFINITE;
    }

    for (;;) {
        ulErrCode = API_SemaphoreBPend(pdwatchfil->DF_ulRdSync,         /*  等待数据有效                */
                                       ulTimeout);
        if (ulErrCode != ERROR_NONE) {                                  /*  超时                        */
            _ErrorHandle(EAGAIN);
            return  (0);
        }

        DWATCH_FILE_LOCK(pdwatchfil);

        if (pdwatchfil->DF_pmonoHead) {
            pwatchevt = (PLW_DWATCH_EVENT)pdwatchfil->DF_pmonoHead;
            if (DWATCH_NREAD(pwatchevt) > stMaxBytes) {
                API_SemaphoreBPost(pdwatchfil->DF_ulRdSync);
                _ErrorHandle(EMSGSIZE);                                 /*  缓冲区太小                  */
                return  (PX_ERROR);
            }
            break;                                                      /*  开始接收                    */
        }

        DWATCH_FILE_UNLOCK(pdwatchfil);
    }

    sstNum  = 0;
    dwevent = (struct dwatch_event *)pcBuffer;                          /*  接收起始缓冲地址            */

    while (pdwatchfil->DF_pmonoHead) {
        pmono     = pdwatchfil->DF_pmonoHead;
        pwatchevt = (PLW_DWATCH_EVENT)pmono;
        stSize    = DWATCH_NREAD(pwatchevt);
        if (((PCHAR)dwevent + stSize) > (pcBuffer + stMaxBytes)) {      /*  缓冲区大小不够              */
            break;
        }

        _list_mono_allocate_seq(&pdwatchfil->DF_pmonoHead,
                                &pdwatchfil->DF_pmonoTail);             /*  从队列中删除                */
        pdwatchfil->DF_uiQueued--;

        dwevent->event = pwatchevt->DE_uiEvent;
        dwevent->fnlen = stSize - offsetof(struct dwatch_event, fname);
        lib_memcpy(dwevent->fname, pwatchevt->DE_cFileName, pwatchevt->DE_stNameLen);
        lib_bzero(&dwevent->fname[pwatchevt->DE_stNameLen], dwevent->fnlen - pwatchevt->DE_stNameLen);
        __SHEAP_FREE(pwatchevt);

        sstNum += stSize;
        dwevent = (struct dwatch_event *)((PCHAR)dwevent + stSize);
    }

    if (pdwatchfil->DF_pmonoHead) {
        API_SemaphoreBPost(pdwatchfil->DF_ulRdSync);                    /*  还有剩余数据可读            */
    }

    DWATCH_FILE_UNLOCK(pdwatchfil);

    return  (sstNum);
}
/*********************************************************************************************************
** 函数名称: _dwatchWrite
** 功能描述: 写 dwatch 文件
** 输　入  : pdwatchfil       dwatch 文件
**           pcBuffer         将要写入的数据指针
**           stNBytes         写入数据大小
** 输　出  : 写入的字节数
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static ssize_t  _dwatchWrite (PLW_DWATCH_FILE  pdwatchfil, PCHAR  pcBuffer, size_t  stNBytes)
{
    _ErrorHandle(ENOSYS);
    return  (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: _dwatchIoctl
** 功能描述: 控制 dwatch 文件
** 输　入  : pdwatchfil       dwatch 文件
**           iRequest         功能
**           lArg             参数
** 输　出  : ERROR
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  _dwatchIoctl (PLW_DWATCH_FILE  pdwatchfil, INT  iRequest, LONG  lArg)
{
    struct stat        *pstatGet;
    INT                 iNRead;
    PLW_LIST_MONO       pmono;
    PLW_SEL_WAKEUPNODE  pselwunNode;

    switch (iRequest) {

    case FIONBIO:
        if (*(INT *)lArg) {
            pdwatchfil->DF_iFlags |= O_NONBLOCK;
        } else {
            pdwatchfil->DF_iFlags &= ~O_NONBLOCK;
        }
        break;

    case FIONREAD:
        iNRead = 0;
        DWATCH_FILE_LOCK(pdwatchfil);
        for (pmono  = pdwatchfil->DF_pmonoHead;
             pmono != LW_NULL;
             pmono  = _list_mono_get_next(pmono)) {
            iNRead += DWATCH_NREAD((PLW_DWATCH_EVENT)pmono);
        }
        DWATCH_FILE_UNLOCK(pdwatchfil);
        *(INT *)lArg = iNRead;
        break;

    case FIOFSTATGET:
        pstatGet = (struct stat *)lArg;
        if (pstatGet) {
            pstatGet->st_dev     = LW_DEV_MAKE_STDEV(&_G_dwatchdev.DW_devhdrHdr);
            pstatGet->st_ino     = (ino_t)0;                            /*  相当于唯一节点              */
            pstatGet->st_mode    = 0444 | S_IFCHR;
            pstatGet->st_nlink   = 1;
            pstatGet->st_uid     = 0;
            pstatGet->st_gid     = 0;
            pstatGet->st_rdev    = 1;
            pstatGet->st_size    = 0;
            pstatGet->st_blksize = 0;
            pstatGet->st_blocks  = 0;
            pstatGet->st_atime   = API_RootFsTime(LW_NULL);
            pstatGet->st_mtime   = API_RootFsTime(LW_NULL);
            pstatGet->st_ctime   = API_RootFsTime(LW_NULL);
        } else {
            _ErrorHandle(EINVAL);
            return  (PX_ERROR);
        }
        break;

    case FIOSELECT:
        pselwunNode = (PLW_SEL_WAKEUPNODE)lArg;
        SEL_WAKE_NODE_ADD(&pdwatchfil->DF_selwulist, pselwunNode);
        if (pselwunNode->SELWUN_seltypType == SELREAD) {
            if (pdwatchfil->DF_pmonoHead) {
                SEL_WAKE_UP(pselwunNode);
            }
        }
        break;

    case FIOUNSELECT:
        SEL_WAKE_NODE_DELETE(&pdwatchfil->DF_selwulist, (PLW_SEL_WAKEUPNODE)lArg);
        break;

    default:
        _ErrorHandle(ERROR_IO_UNKNOWN_REQUEST);
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: _dwatchNotify
** 功能描述: dwatch 文件通知
** 输　入  : pdwatchfil       dwatch 文件
**           uiEvent          产生的事件
**           pcTail           除设备头外的路径
**           stTailLen        除设备头外的路径长度
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  _dwatchNotify (PLW_DWATCH_FILE  pdwatchfil, UINT32  uiEvent, PCHAR  pcTail, size_t stTailLen)
{
    BOOL              bOutOfWatch;
    size_t            stNameLen;
    size_t            stPrefixLen = pdwatchfil->DF_stDirPrefixLen;
    PLW_DWATCH_EVENT  pwatchevt;

    if ((uiEvent == DWATCH_MOVE_TO) && (pdwatchfil->DF_uiEventLast == DWATCH_MOVE_FROM)) {
        bOutOfWatch = LW_FALSE;
        if (pdwatchfil->DF_stDirPrefixLen >= stTailLen) {
            bOutOfWatch = LW_TRUE;

        } else if ((pcTail[pdwatchfil->DF_stDirPrefixLen] != PX_DIVIDER) ||
                   (stPrefixLen && lib_memcmp(pdwatchfil->DF_pcDirPrefix, pcTail, stPrefixLen))) {
            bOutOfWatch = LW_TRUE;

        } else if (!(pdwatchfil->DF_uiEvents & DWATCH_SUBTREE) &&
                   lib_index(&pcTail[stPrefixLen + 1], PX_DIVIDER)) {
            bOutOfWatch = LW_TRUE;
        }

        if (bOutOfWatch) {
            stNameLen = 0;
            pwatchevt = (PLW_DWATCH_EVENT)__SHEAP_ALLOC(sizeof(LW_DWATCH_EVENT) + 1);
        } else {
            stPrefixLen++;
            stNameLen = stTailLen - stPrefixLen;
            pwatchevt = (PLW_DWATCH_EVENT)__SHEAP_ALLOC(sizeof(LW_DWATCH_EVENT) + stNameLen);
        }

    } else {
        if (pcTail[stPrefixLen] != PX_DIVIDER) {
            return;
        }
        if (stPrefixLen && lib_memcmp(pdwatchfil->DF_pcDirPrefix, pcTail, stPrefixLen)) {
            return;
        }
        stPrefixLen++;
        if (!(pdwatchfil->DF_uiEvents & DWATCH_SUBTREE) && lib_index(&pcTail[stPrefixLen], PX_DIVIDER)) {
            return;
        }

        stNameLen = stTailLen - stPrefixLen;
        pwatchevt = (PLW_DWATCH_EVENT)__SHEAP_ALLOC(sizeof(LW_DWATCH_EVENT) + stNameLen);
    }

    if (pwatchevt == LW_NULL) {
        _DebugHandle(__ERRORMESSAGE_LEVEL, "dwatch event notify no memory!\r\n");
        return;
    }

    pwatchevt->DE_uiEvent              = uiEvent;
    pwatchevt->DE_stNameLen            = stNameLen;
    pwatchevt->DE_cFileName[stNameLen] = PX_EOS;
    if (stNameLen) {
        lib_memcpy(pwatchevt->DE_cFileName, &pcTail[stPrefixLen], stNameLen);
    }

    pdwatchfil->DF_uiEventLast = uiEvent;

    DWATCH_FILE_LOCK(pdwatchfil);
    _list_mono_free_seq(&pdwatchfil->DF_pmonoHead, &pdwatchfil->DF_pmonoTail, &pwatchevt->DE_monoList);
    pdwatchfil->DF_uiQueued++;
    DWATCH_FILE_UNLOCK(pdwatchfil);

    API_SemaphoreBPost(pdwatchfil->DF_ulRdSync);
    SEL_WAKE_UP_ALL(&pdwatchfil->DF_selwulist, SELREAD);
}
/*********************************************************************************************************
** 函数名称: dwatch
** 功能描述: 打开 dwatch 文件
** 输　入  : dirname   检查的目录名
**           flags     打开标志 DWATCH_CLOEXEC / DWATCH_NONBLOCK
** 输　出  : events    检查的事件类型掩码
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
LW_API
int  dwatch (const char *dirname, int flags, unsigned int events)
{
    INT              iDir = PX_ERROR;
    INT              iWatch;
    INT              iFeatures;
    CHAR             cRealName[MAX_FILENAME_LENGTH];
    size_t           stDevNameLen;
    size_t           stDirNameLen;
    PLW_DEV_HDR      pdevhdr;
    PLW_DWATCH_FILE  pdwatchfil;
    struct stat      statGet;

    if (!dirname || !events) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    flags  &= (DWATCH_CLOEXEC | DWATCH_NONBLOCK);
    iWatch  = open(LW_DWATCH_DEV_PATH, O_RDONLY | flags);
    if (iWatch < 0) {
        return  (PX_ERROR);
    }

    iDir = open(dirname, O_RDONLY | O_DIRECTORY);
    if (iDir < 0) {
        goto    __error_handle;
    }

    if (fstat(iDir, &statGet) || !S_ISDIR(statGet.st_mode)) {           /*  检测是否为目录文件          */
        _ErrorHandle(ENOTDIR);
        goto    __error_handle;
    }

    DWATCH_IO_LOCK();
    pdevhdr = iosFdDevFind(iDir);
    if (!pdevhdr ||
        iosDrvGetFeatures(pdevhdr->DEVHDR_usDrvNum, &iFeatures) ||
        !(iFeatures & LW_DRV_FEATURE_DWATCH)) {                         /*  驱动程序不支持              */
        DWATCH_IO_UNLOCK();
        _ErrorHandle(ENOTSUP);
        goto    __error_handle;
    }

    if (iosFdGetRealName(iDir, cRealName, MAX_FILENAME_LENGTH)) {
        DWATCH_IO_UNLOCK();
        goto    __error_handle;
    }

    stDevNameLen = lib_strlen(pdevhdr->DEVHDR_pcName);
    stDirNameLen = lib_strlen(cRealName);

    pdwatchfil = (PLW_DWATCH_FILE)iosFdValue(iWatch);
    if (stDirNameLen > stDevNameLen) {
        pdwatchfil->DF_stDirPrefixLen = stDirNameLen - stDevNameLen;
        pdwatchfil->DF_pcDirPrefix    = (PCHAR)__SHEAP_ALLOC(pdwatchfil->DF_stDirPrefixLen + 1);
        if (pdwatchfil->DF_pcDirPrefix) {
            lib_strcpy(pdwatchfil->DF_pcDirPrefix, &cRealName[stDevNameLen]);

        } else {                                                        /*  记录目录前缀                */
            DWATCH_IO_UNLOCK();
            _ErrorHandle(ERROR_SYSTEM_LOW_MEMORY);
            goto    __error_handle;
        }
    }

    pdwatchfil->DF_uiEvents = events;                                   /*  需要检测的事件              */

    KN_SMP_WMB();
    pdevhdr->DEVHDR_ulDWatchs++;                                        /*  DWATCH 数量增加             */
    pdwatchfil->DF_pdevhdr = pdevhdr;                                   /*  记录设备头                  */
    DWATCH_IO_UNLOCK();

    close(iDir);

    return  (iWatch);

__error_handle:
    if (iDir >= 0) {
        close(iDir);
    }
    close(iWatch);

    return  (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: dwatchDevDeleteHook
** 功能描述: 设备删除 hook
** 输　入  : pdevhdr          被删除的设备
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  dwatchDevDeleteHook (PLW_DEV_HDR  pdevhdr)
{
    PLW_LIST_LINE    pline;
    PLW_DWATCH_FILE  pdwatchfil;

    if (pdevhdr->DEVHDR_ulDWatchs == 0ul) {
        return;
    }

    DWATCH_LOCK();
    for (pline  = _G_dwatchdev.DW_plineFiles;
         pline != LW_NULL;
         pline  = _list_line_get_next(pline)) {

        pdwatchfil = (PLW_DWATCH_FILE)pline;
        if (pdwatchfil->DF_pdevhdr == pdevhdr) {
            pdwatchfil->DF_pdevhdr =  LW_NULL;
            SEL_WAKE_UP_ALL(&pdwatchfil->DF_selwulist, SELEXCEPT);
        }
    }
    DWATCH_UNLOCK();
}
/*********************************************************************************************************
** 函数名称: dwatchDevNotifyHook
** 功能描述: 设备文件事件通告 hook
** 输　入  : pdevhdr          设备头
**           uiEvent          产生的事件
**           pcTail           除设备头外的路径
**           stTailLen        除设备头外的路径长度
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  dwatchDevNotifyHook (PLW_DEV_HDR  pdevhdr, UINT32  uiEvent, PCHAR  pcTail, size_t  stTailLen)
{
    PLW_LIST_LINE    pline;
    PLW_DWATCH_FILE  pdwatchfil;

    if (pdevhdr->DEVHDR_ulDWatchs == 0ul) {
        return;
    }

    DWATCH_LOCK();
    for (pline  = _G_dwatchdev.DW_plineFiles;
         pline != LW_NULL;
         pline  = _list_line_get_next(pline)) {

        pdwatchfil = (PLW_DWATCH_FILE)pline;
        if ((pdwatchfil->DF_pdevhdr != pdevhdr) ||
           !(pdwatchfil->DF_uiEvents & uiEvent)) {
            continue;
        }

        if (uiEvent == DWATCH_MOVE_TO) {
            _dwatchNotify(pdwatchfil, uiEvent, pcTail, stTailLen);

        } else if ((pdwatchfil->DF_stDirPrefixLen < stTailLen) &&
                   (pdwatchfil->DF_uiQueued < LW_CFG_DWATCH_MAX_EVENTS)) {
            _dwatchNotify(pdwatchfil, uiEvent, pcTail, stTailLen);
        }
    }
    DWATCH_UNLOCK();

    errno = 0;                                                          /*  清除 errno                  */
}

#endif                                                                  /*  LW_CFG_DEVICE_EN > 0        */
                                                                        /*  LW_CFG_DWATCH_EN > 0        */
/*********************************************************************************************************
  END
*********************************************************************************************************/
