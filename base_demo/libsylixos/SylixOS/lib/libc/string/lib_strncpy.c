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
** 文   件   名: lib_strncpy.c
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2006 年 12 月 13 日
**
** 描        述: 库 (注意, 没有处理缓冲区重叠的现象)

** BUG:
2009.07.18  加入 strlcpy() 函数.
*********************************************************************************************************/
#include "../SylixOS/kernel/include/k_kernel.h"
/*********************************************************************************************************
** 函数名称: lib_strncpy
** 功能描述: 
** 输　入  : 
** 输　出  : 
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
PCHAR  lib_strncpy (PCHAR  pcDest, CPCHAR  pcSrc, size_t  stN)
{
    REGISTER PCHAR  pcDestReg;

    if (stN == 0) {
        return  (pcDest);
    }

    pcDestReg = pcDest;

    do {
        if ((*pcDestReg++ = *pcSrc++) == '\0') {
            while (--stN != 0) {
                *pcDestReg++ = '\0';
            }
            break;
        }
    } while (--stN != 0);
    
    return  (pcDest);
}
/*********************************************************************************************************
** 函数名称: lib_strlcpy
** 功能描述: 
** 输　入  : 
** 输　出  : 
** 全局变量: 
** 调用模块: 
** 注  意  : 这里没有处理缓冲区覆盖.
*********************************************************************************************************/
size_t  lib_strlcpy (PCHAR  pcDest, CPCHAR  pcSrc, size_t  stN)
{
    REGISTER PCHAR    pcSrcReg = (PCHAR)pcSrc;
    
    if (stN == 0) {
        return  (lib_strlen(pcSrc));
    }
    
    while (*pcSrcReg != PX_EOS && stN > 1) {
        *pcDest++ = *pcSrcReg++;
        stN--;
    }
    *pcDest = PX_EOS;
    
    while (*pcSrcReg != PX_EOS) {
        pcSrcReg++;
    }
    
    return  ((size_t)(pcSrcReg - pcSrc));                               /*  strlen(pcSrc)               */
}
/*********************************************************************************************************
** 函数名称: lib_bcopy
** 功能描述: 将字符串src的前n个字节复制到dest中 (使用 memcpy 完成)
** 输　入  : 
** 输　出  : 
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID    lib_bcopy (CPVOID pvSrc, PVOID pvDest, size_t  stN)
{
    lib_memcpy(pvDest, pvSrc, stN);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
