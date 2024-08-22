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
** 文   件   名: fs_fs.h
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2008 年 09 月 28 日
**
** 描        述: 这是文件系统综合头文件库。

** BUG:
2013.01.22  只有内核可以使用各文件系统 api.
*********************************************************************************************************/

#ifndef __FS_FS_H
#define __FS_FS_H

/*********************************************************************************************************
  FS type
*********************************************************************************************************/
#include "fs_type.h"                                                    /*  文件系统类型                */
#ifdef __SYLIXOS_KERNEL
/*********************************************************************************************************
  ROOT FS
*********************************************************************************************************/
#include "../SylixOS/fs/rootFs/rootFs.h"                                /*  根文件系统                  */
#include "../SylixOS/fs/rootFs/rootFsMap.h"                             /*  根文件系统映射              */
/*********************************************************************************************************
  PROC FS
*********************************************************************************************************/
#if LW_CFG_PROCFS_EN > 0
#include "../SylixOS/fs/procFs/procFs.h"                                /*  PROC 文件系统               */
#endif                                                                  /*  LW_CFG_PROCFS_EN > 0        */
/*********************************************************************************************************
  RAM FS
*********************************************************************************************************/
#if LW_CFG_RAMFS_EN > 0
#include "../SylixOS/fs/ramFs/ramFs.h"                                  /*  ram 文件系统                */
#endif                                                                  /*  LW_CFG_RAMFS_EN > 0         */
/*********************************************************************************************************
  ROM FS
*********************************************************************************************************/
#include "../SylixOS/fs/romFs/romFs.h"                                  /*  rom 文件系统                */
/*********************************************************************************************************
  ISO9660 FS
*********************************************************************************************************/
#if LW_CFG_ISO9660FS_EN > 0
#include "../SylixOS/fs/iso9660Fs/iso9660_sylixos.h"                    /*  ISO9660 文件系统            */
#endif                                                                  /*  LW_CFG_ISO9660FS_EN > 0     */
/*********************************************************************************************************
  FAT FS
*********************************************************************************************************/
#if LW_CFG_FATFS_EN > 0
#include "../SylixOS/fs/fatFs/fatFs.h"                                  /*  FAT 文件系统                */
#endif                                                                  /*  LW_CFG_FATFS_EN > 0         */
/*********************************************************************************************************
  TPS FS
*********************************************************************************************************/
#if LW_CFG_TPSFS_EN > 0
#include "../SylixOS/fs/tpsFs/tpsfs_sylixos.h"
#endif                                                                  /*  LW_CFG_TPSFS_EN > 0         */
/*********************************************************************************************************
  NFS
*********************************************************************************************************/
#if LW_CFG_NFS_EN > 0
#include "../SylixOS/fs/nfs/nfs_sylixos.h"                              /*  NFS 文件系统                */
#endif                                                                  /*  LW_CFG_NFS_EN > 0           */
/*********************************************************************************************************
  DISK PARTITION
*********************************************************************************************************/
#include "../SylixOS/fs/diskPartition/diskPartition.h"                  /*  磁盘分区表                  */
#if LW_CFG_CPU_WORD_LENGHT > 32                                         /*  64 位 CPU 使能 GPT 分区     */
#include "../SylixOS/fs/diskPartition/gptPartition.h"                   /*  GPT 分区表                  */
#endif                                                                  /*  LW_CFG_CPU_WORD_LENGHT > 32 */
/*********************************************************************************************************
  BLOCK DISK CACHE
*********************************************************************************************************/
#if LW_CFG_DISKCACHE_EN > 0
#include "../SylixOS/fs/diskCache/diskCache.h"                          /*  磁盘高速缓冲                */
#endif                                                                  /*  LW_CFG_DISKCACHE_EN > 0     */
#include "../SylixOS/fs/nandRCache/nandRCache.h"                        /*  nand flash read cache       */
/*********************************************************************************************************
  DISK RAID
*********************************************************************************************************/
#if LW_CFG_DISKRAID_EN > 0
#include "../SylixOS/fs/diskRaid/diskRaid.h"                            /*  磁盘阵列                    */
#endif                                                                  /*  LW_CFG_DISKRAID_EN > 0      */
/*********************************************************************************************************
  TSYNC
*********************************************************************************************************/
#include "../SylixOS/fs/tsync/tsync.h"                                  /*  背景数据回写                */
/*********************************************************************************************************
  OEM DISK 
*********************************************************************************************************/
#include "../SylixOS/fs/oemDisk/oemDisk.h"                              /*  OEM 磁盘操作                */
#include "../SylixOS/fs/oemDisk/oemFdisk.h"                             /*  OEM 磁盘分区操作            */
#include "../SylixOS/fs/oemDisk/oemGrub.h"                              /*  OEM GRUB 引导程序写入       */
#endif                                                                  /*  __SYLIXOS_KERNEL            */
/*********************************************************************************************************
  MOUNT LIB
*********************************************************************************************************/
#if LW_CFG_MOUNT_EN > 0
#include "../SylixOS/fs/mount/mount.h"                                  /*  mount 库                    */
#endif                                                                  /*  LW_CFG_MOUNT_EN > 0         */
/*********************************************************************************************************
  YAFFS2
*********************************************************************************************************/
#ifdef   __SYLIXOS_YAFFS_DRV
#include "../SylixOS/fs/yaffs2/direct/yaffscfg.h"                       /*  yaffs 驱动相关              */
#include "../SylixOS/fs/yaffs2/direct/yaffsfs.h"                        /*  yaffs 文件系统底层接口      */
#include "../SylixOS/fs/yaffs2/yaffs_trace.h"
#include "../SylixOS/fs/yaffs2/yaffs_guts.h"
#include "../SylixOS/fs/yaffs2/yaffs_nand.h"
#if LW_CFG_YAFFS_NOR_EN > 0
#include "../SylixOS/fs/yaffs2/yaffs_nor.h"
#endif                                                                  /*  LW_CFG_YAFFS_NOR_EN > 0     */
#ifdef   __SYLIXOS_YAFFS_MTD
#include "../SylixOS/fs/yaffs2/yaffs_mtdif.h"
#endif                                                                  /*  __SYLIXOS_YAFFS_MTD         */
#endif                                                                  /*  __SYLIXOS_YAFFS_DRV         */

#include "../SylixOS/fs/yaffs2/yaffs_sylixosapi.h"

#endif                                                                  /*  __FS_FS_H                   */
/*********************************************************************************************************
  END
*********************************************************************************************************/
