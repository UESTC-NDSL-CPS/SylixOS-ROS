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
** 文   件   名: pciStorageAta.h
**
** 创   建   人: Gong.YuJian (弓羽箭)
**
** 文件创建日期: 2018 年 09 月 04 日
**
** 描        述: ATA/IDE 驱动.
*********************************************************************************************************/

#ifndef __PCISTORAGEATA_H
#define __PCISTORAGEATA_H

/*********************************************************************************************************
  驱动名称
*********************************************************************************************************/
#define ATA_PCI_DRV_NAME                    "ATA_PCI"                   /* PCI 类型                     */
#define ATA_PCI_DRV_VER_NUM                 0x02000100                  /* 驱动版本数值                 */

/*********************************************************************************************************
  函数声明
*********************************************************************************************************/
INT  pciStorageAtaInit(VOID);

#endif                                                                  /*  __PCISTORAGEATA_H           */
/*********************************************************************************************************
  END
*********************************************************************************************************/
