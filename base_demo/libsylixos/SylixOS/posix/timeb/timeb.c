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
** 文   件   名: timeb.c
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2011 年 03 月 01 日
**
** 描        述: posix timeb 兼容库.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "../include/px_timeb.h"                                        /*  已包含操作系统头文件        */
/*********************************************************************************************************
  裁剪支持
*********************************************************************************************************/
#if LW_CFG_POSIX_EN > 0
/*********************************************************************************************************
** 函数名称: ftime
** 功能描述: 获得当前时间与相关参数
** 输　入  : tp            struct timeb 参数
** 输　出  : ERROR or OK
** 全局变量: 
** 调用模块: 
                                           API 函数
*********************************************************************************************************/
LW_API 
int  ftime (struct timeb *tp)
{
    struct timeval      tv;
    struct timezone     tz;

    if (!tp) {
        errno = EINVAL;
        return  (PX_ERROR);
    }
    
    lib_gettimeofday(&tv, &tz);
    
    tp->time     = tv.tv_sec;
    tp->millitm  = (unsigned short)(tv.tv_usec / 1000);
    tp->timezone = (short)tz.tz_minuteswest;
    
    if (tz.tz_dsttime) {
        tp->dstflag = LW_TRUE;
    } else {
        tp->dstflag = LW_FALSE;
    }
    
    return  (ERROR_NONE);
}

#endif                                                                  /*  LW_CFG_POSIX_EN > 0         */
/*********************************************************************************************************
  END
*********************************************************************************************************/
