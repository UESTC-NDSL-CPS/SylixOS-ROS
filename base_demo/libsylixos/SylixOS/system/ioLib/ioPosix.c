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
** ��   ��   ��: ioPosix.c
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2023 �� 08 �� 19 ��
**
** ��        ��: POSIX ��չ IO ����.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "../SylixOS/kernel/include/k_kernel.h"
#include "../SylixOS/system/include/s_system.h"
/*********************************************************************************************************
  ����ü�֧��
*********************************************************************************************************/
#if LW_CFG_DEVICE_EN > 0
/*********************************************************************************************************
** ��������: posix_fadvise
** ��������: Ӧ�ó����֪����ϵͳ��Ŀ���ļ�����Ԥ�ڲ�������.
** �䡡��  : iFd           �ļ�������
**           oftOffset     ��ʼƫ����
**           oftLen        ���򳤶�
**           iAdvice       ��������, ���ܵĽ������:
**                         POSIX_FADV_NORMAL
**                         POSIX_FADV_SEQUENTIAL
**                         POSIX_FADV_RANDOM
**                         POSIX_FADV_WILLNEED
**                         POSIX_FADV_DONTNEED
**                         POSIX_FADV_NOREUSE
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
                                           API ����
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
** ��������: posix_fallocate
** ��������: �ļ��ռ����.
** �䡡��  : iFd           �ļ�������
**           oftOffet      ��ʼƫ����
**           oftLen        ���򳤶�
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
                                           API ����
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
