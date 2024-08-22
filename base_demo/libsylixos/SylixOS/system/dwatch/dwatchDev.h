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
** ��   ��   ��: dwatchDev.h
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2023 �� 06 �� 21 ��
**
** ��        ��: Ŀ¼�����.
*********************************************************************************************************/

#ifndef __DWATCHDEV_H
#define __DWATCHDEV_H

/*********************************************************************************************************
  �ü�����
*********************************************************************************************************/
#if (LW_CFG_DEVICE_EN > 0) && (LW_CFG_DWATCH_EN > 0)

/*********************************************************************************************************
  �豸·��
*********************************************************************************************************/

#define LW_DWATCH_DEV_PATH  "/dev/dwatch"

/*********************************************************************************************************
  �豸���ļ��ṹ
*********************************************************************************************************/

typedef struct {
    LW_DEV_HDR           DW_devhdrHdr;                                  /*  �豸ͷ                      */
    LW_OBJECT_HANDLE     DW_ulLock;
    LW_LIST_LINE_HEADER  DW_plineFiles;
} LW_DWATCH_DEV;
typedef LW_DWATCH_DEV   *PLW_DWATCH_DEV;

typedef struct {
    LW_LIST_MONO         DE_monoList;
    UINT32               DE_uiEvent;                                    /*  �¼�����                    */
    size_t               DE_stNameLen;
    CHAR                 DE_cFileName[1];                               /*  �¼��ļ���                  */
} LW_DWATCH_EVENT;
typedef LW_DWATCH_EVENT *PLW_DWATCH_EVENT;

typedef struct {
    LW_LIST_LINE         DF_lineList;                                   /*  �ļ�����                    */
    LW_OBJECT_HANDLE     DF_ulLock;                                     /*  ������                      */
    LW_OBJECT_HANDLE     DF_ulRdSync;                                   /*  ��ͬ����                    */
    PLW_DEV_HDR          DF_pdevhdr;                                    /*  ���� devhdr               */

    INT                  DF_iFlags;                                     /*  ���ļ���ѡ��              */
    PCHAR                DF_pcDirPrefix;
    size_t               DF_stDirPrefixLen;

    UINT32               DF_uiEvents;
    UINT32               DF_uiEventLast;                                /*  ��һ���յ����¼�            */

    PLW_LIST_MONO        DF_pmonoHead;                                  /*  �¼� FIFO ����              */
    PLW_LIST_MONO        DF_pmonoTail;
    UINT32               DF_uiQueued;                                   /*  �¼���������                */

    LW_SEL_WAKEUPLIST    DF_selwulist;
} LW_DWATCH_FILE;
typedef LW_DWATCH_FILE  *PLW_DWATCH_FILE;

/*********************************************************************************************************
  ��ʼ������
*********************************************************************************************************/

LW_API INT  API_DWatchDrvInstall(VOID);
LW_API INT  API_DWatchDevCreate(VOID);

#define dwatchDrv          API_DWatchDrvInstall
#define dwatchDevCreate    API_DWatchDevCreate

/*********************************************************************************************************
  �¼��ص� (DWATCH_MOVE_FROM �� DWATCH_MOVE_TO �¼����뵥���ϱ�)
*********************************************************************************************************/

VOID  dwatchDevDeleteHook(PLW_DEV_HDR  pdevhdr);
VOID  dwatchDevNotifyHook(PLW_DEV_HDR  pdevhdr, UINT32  uiEvent, PCHAR  pcTail, size_t  stTailLen);

#endif                                                                  /*  LW_CFG_DEVICE_EN > 0        */
                                                                        /*  LW_CFG_DWATCH_EN > 0        */
#endif                                                                  /*  __DWATCHDEV_H               */
/*********************************************************************************************************
  END
*********************************************************************************************************/
