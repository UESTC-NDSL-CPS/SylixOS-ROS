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
** 文   件   名: lib_file.h
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2008 年 09 月 04 日
**
** 描        述: 类标准 C 库支持, 标准文件创建及获取函数
*********************************************************************************************************/

#ifndef __LIB_FILE_H
#define __LIB_FILE_H

/*********************************************************************************************************
  内部函数声明
*********************************************************************************************************/
#if (LW_CFG_DEVICE_EN > 0) && (LW_CFG_FIO_LIB_EN > 0)

FILE  *__lib_newfile(FILE  *pfFile);
VOID   __lib_delfile(FILE  *pfFile);

#endif                                                                  /*  (LW_CFG_DEVICE_EN > 0)      */
                                                                        /*  (LW_CFG_FIO_LIB_EN > 0)*/
#endif                                                                  /*  __LIB_FILE_H                */
/*********************************************************************************************************
  END
*********************************************************************************************************/
