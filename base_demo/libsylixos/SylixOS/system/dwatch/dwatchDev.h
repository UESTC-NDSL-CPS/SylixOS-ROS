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
** 文   件   名: dwatchDev.h
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2023 年 06 月 21 日
**
** 描        述: 目录监控器.
*********************************************************************************************************/

#ifndef __DWATCHDEV_H
#define __DWATCHDEV_H

/*********************************************************************************************************
  裁剪控制
*********************************************************************************************************/
#if (LW_CFG_DEVICE_EN > 0) && (LW_CFG_DWATCH_EN > 0)

/*********************************************************************************************************
  设备路径
*********************************************************************************************************/

#define LW_DWATCH_DEV_PATH  "/dev/dwatch"

/*********************************************************************************************************
  设备与文件结构
*********************************************************************************************************/

typedef struct {
    LW_DEV_HDR           DW_devhdrHdr;                                  /*  设备头                      */
    LW_OBJECT_HANDLE     DW_ulLock;
    LW_LIST_LINE_HEADER  DW_plineFiles;
} LW_DWATCH_DEV;
typedef LW_DWATCH_DEV   *PLW_DWATCH_DEV;

typedef struct {
    LW_LIST_MONO         DE_monoList;
    UINT32               DE_uiEvent;                                    /*  事件类型                    */
    size_t               DE_stNameLen;
    CHAR                 DE_cFileName[1];                               /*  事件文件名                  */
} LW_DWATCH_EVENT;
typedef LW_DWATCH_EVENT *PLW_DWATCH_EVENT;

typedef struct {
    LW_LIST_LINE         DF_lineList;                                   /*  文件链表                    */
    LW_OBJECT_HANDLE     DF_ulLock;                                     /*  操作锁                      */
    LW_OBJECT_HANDLE     DF_ulRdSync;                                   /*  读同步锁                    */
    PLW_DEV_HDR          DF_pdevhdr;                                    /*  侦测的 devhdr               */

    INT                  DF_iFlags;                                     /*  打开文件的选项              */
    PCHAR                DF_pcDirPrefix;
    size_t               DF_stDirPrefixLen;

    UINT32               DF_uiEvents;
    UINT32               DF_uiEventLast;                                /*  上一次收到的事件            */

    PLW_LIST_MONO        DF_pmonoHead;                                  /*  事件 FIFO 队列              */
    PLW_LIST_MONO        DF_pmonoTail;
    UINT32               DF_uiQueued;                                   /*  事件队列数量                */

    LW_SEL_WAKEUPLIST    DF_selwulist;
} LW_DWATCH_FILE;
typedef LW_DWATCH_FILE  *PLW_DWATCH_FILE;

/*********************************************************************************************************
  初始化操作
*********************************************************************************************************/

LW_API INT  API_DWatchDrvInstall(VOID);
LW_API INT  API_DWatchDevCreate(VOID);

#define dwatchDrv          API_DWatchDrvInstall
#define dwatchDevCreate    API_DWatchDevCreate

/*********************************************************************************************************
  事件回调 (DWATCH_MOVE_FROM 与 DWATCH_MOVE_TO 事件必须单独上报)
*********************************************************************************************************/

VOID  dwatchDevDeleteHook(PLW_DEV_HDR  pdevhdr);
VOID  dwatchDevNotifyHook(PLW_DEV_HDR  pdevhdr, UINT32  uiEvent, PCHAR  pcTail, size_t  stTailLen);

#endif                                                                  /*  LW_CFG_DEVICE_EN > 0        */
                                                                        /*  LW_CFG_DWATCH_EN > 0        */
#endif                                                                  /*  __DWATCHDEV_H               */
/*********************************************************************************************************
  END
*********************************************************************************************************/
