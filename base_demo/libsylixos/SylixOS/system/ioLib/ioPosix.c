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
** 文   件   名: ioPosix.c
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2023 年 08 月 19 日
**
** 描        述: POSIX 扩展 IO 函数.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "../SylixOS/kernel/include/k_kernel.h"
#include "../SylixOS/system/include/s_system.h"
/*********************************************************************************************************
  加入裁剪支持
*********************************************************************************************************/
#if LW_CFG_DEVICE_EN > 0
/*********************************************************************************************************
** 函数名称: posix_fadvise
** 功能描述: 应用程序告知操作系统对目标文件区域预期操作建议.
** 输　入  : iFd           文件描述符
**           oftOffset     起始偏移量
**           oftLen        区域长度
**           iAdvice       操作建议, 可能的建议包括:
**                         POSIX_FADV_NORMAL
**                         POSIX_FADV_SEQUENTIAL
**                         POSIX_FADV_RANDOM
**                         POSIX_FADV_WILLNEED
**                         POSIX_FADV_DONTNEED
**                         POSIX_FADV_NOREUSE
** 输　出  : ERROR CODE
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
LW_API
INT  posix_fadvise (INT iFd, off_t oftOffset, off_t oftLen, INT iAdvice)
{
    if ((oftOffset < 0) || (oftLen < 0) ||
        (iAdvice < POSIX_FADV_NORMAL) || (iAdvice > POSIX_FADV_NOREUSE)) {
        _ErrorHandle(EINVAL);
        return  (EINVAL);
    }

    if (fcntl(iFd, F_VALID)) {
        _ErrorHandle(EBADF);
        return  (EBADF);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: posix_fallocate
** 功能描述: 文件空间控制.
** 输　入  : iFd           文件描述符
**           oftOffet      起始偏移量
**           oftLen        区域长度
** 输　出  : ERROR CODE
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
LW_API
INT  posix_fallocate (INT iFd, off_t oftOffset, off_t oftLen)
{
    off_t        oftArg[2];
    struct stat  statFile;

    if ((oftOffset < 0) || (oftLen < 0)) {
        _ErrorHandle(EINVAL);
        return  (EINVAL);
    }

    if ((oftOffset + oftLen) < oftOffset) {
        _ErrorHandle(EOVERFLOW);
        return  (EOVERFLOW);
    }

    if (fstat(iFd, &statFile) < 0) {
        _ErrorHandle(EBADF);
        return  (EBADF);
    }

    if (S_ISFIFO(statFile.st_mode)) {
        _ErrorHandle(ESPIPE);
        return  (ESPIPE);

    } else if (!S_ISREG(statFile.st_mode)) {
        _ErrorHandle(ENODEV);
        return  (ENODEV);
    }

    oftArg[0] = oftOffset;
    oftArg[1] = oftLen;

    if (ioctl(iFd, FIOALLOCATE, oftArg)) {
        return  (errno);
    } else {
        return  (ERROR_NONE);
    }
}

#endif                                                                  /*  LW_CFG_DEVICE_EN            */
/*********************************************************************************************************
  END
*********************************************************************************************************/
