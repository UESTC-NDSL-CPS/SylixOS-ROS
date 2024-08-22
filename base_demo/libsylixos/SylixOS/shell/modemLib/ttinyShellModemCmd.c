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
** ��   ��   ��: ttinyShellModemCmd.c
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2010 �� 07 �� 28 ��
**
** ��        ��: ϵͳ modem ��������.

** BUG:
2013.06.10  ���� -fsigned-char .
2023.06.28  ֧�� Xmodem1K Э��� CRC16 �����.
*********************************************************************************************************/
#define  __SYLIXOS_STDIO
#define  __SYLIXOS_KERNEL
#include "../SylixOS/kernel/include/k_kernel.h"
#include "../SylixOS/system/include/s_system.h"
#include "../SylixOS/shell/include/ttiny_shell.h"
/*********************************************************************************************************
  �ü�����
*********************************************************************************************************/
#if LW_CFG_SHELL_EN > 0
#include "../SylixOS/shell/ttinyShell/ttinyShell.h"
#include "../SylixOS/shell/ttinyShell/ttinyShellLib.h"
#include "../SylixOS/shell/ttinyVar/ttinyVarLib.h"
/*********************************************************************************************************
  XMODEM ������
*********************************************************************************************************/
#define __LW_XMODEM_SOH             0x01
#define __LW_XMODEM_STX             0x02
#define __LW_XMODEM_EOT             0x04
#define __LW_XMODEM_ACK             0x06
#define __LW_XMODEM_NAK             0x15
#define __LW_XMODEM_CAN             0x18
#define __LW_XMODEM_CRC16           0x43
#define __LW_XMODEM_PAD             0x1A                                /*  ���ݲ�ȫ�����              */
/*********************************************************************************************************
  XMODEM �����ֲ���
*********************************************************************************************************/
#define __LW_XMODEM_SEND_CMD(c)     {                               \
                                        CHAR    cCmd = c;           \
                                        write(STD_OUT, &cCmd, 1);   \
                                    }
/*********************************************************************************************************
  XMODEM ����
*********************************************************************************************************/
#define __LW_XMODEM_TX_TIMEOUT      3                                   /*  ������Ϊ��ʱʱ��ĵ�λ      */
#define __LW_XMODEM_RX_TIMEOUT      3                                   /*  ������Ϊ��ʱʱ��ĵ�λ      */
/*********************************************************************************************************
  X16 + X12 + X5 + 1 ��ʽ��
*********************************************************************************************************/
static const UINT16  _G_usXmodemCrcTable[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};
/*********************************************************************************************************
** ��������: __xmodemCrc16
** ��������: ���� CRC16
** �䡡��  : pucData       ����ָ��
**           stDataLen     ���ݳ���
** �䡡��  : CRC16
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static UINT16  __xmodemCrc16 (PUCHAR  pucData, size_t  stDataLen)
{
    UINT16  usCrc = 0;
    size_t  i;

    for (i = 0; i < stDataLen; i++) {
        usCrc = (usCrc << 8) ^ _G_usXmodemCrcTable[(usCrc >> 8) ^ *pucData++];
    }

    return  (usCrc);
}
/*********************************************************************************************************
** ��������: __xmodemChkSum
** ��������: ����У���
** �䡡��  : pucData       ����ָ��
**           stDataLen     ���ݳ���
** �䡡��  : У���
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static UCHAR  __xmodemChkSum (PUCHAR  pucData, size_t  stDataLen)
{
    UCHAR   ucChkSum = 0;
    size_t  i;

    for (i = 0; i < stDataLen; i++) {
        ucChkSum = (UCHAR)(ucChkSum + pucData[i]);                      /*  ����У���                  */
    }

    return  (ucChkSum);
}
/*********************************************************************************************************
** ��������: __tshellXmodemCleanup
** ��������: Xmodem �������� control-C ʱ���������
** �䡡��  : iFile     �����ļ�������
** �䡡��  : 0
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  __tshellXmodemCleanup (INT  iFile)
{
    struct timeval  tmval = {2, 0};
           CHAR     cTemp[16];
    
    if (iFile >= 0) {
        close(iFile);
    }
    
    /*
     *  ����һЩ�ն���������ļ�����ʱ, ���������� __LW_XMODEM_CAN 0x18 ����, 
     *  ���Ի����ϵͳת��Ϊ OPT_TERMINAL ģʽʱ�� ctrl+x �����ͻ, ���ϵͳ����,
     *  ����������Ҫ�ȴ� 2s ��ƽ��ʱ��.
     */
    while (waitread(STD_IN, &tmval) == 1) {
        if (read(STD_IN, cTemp, sizeof(cTemp)) <= 0) {
            break;
        }
    }
    
    ioctl(STD_IN, FIOSETOPTIONS, OPT_TERMINAL);
    ioctl(STD_OUT, FIOSETOPTIONS, OPT_TERMINAL);
    
    ioctl(STD_IN, FIORFLUSH);                                           /*  �������������              */
}
/*********************************************************************************************************
** ��������: __tshellFsCmdXmodems
** ��������: ϵͳ���� "xmodems" (���͸� remote һ���ļ�)
** �䡡��  : iArgC         ��������
**           ppcArgV       ������
** �䡡��  : 0
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT  __tshellFsCmdXmodems (INT  iArgC, PCHAR  ppcArgV[])
{
    INT             i;
    INT             j;
    
    BOOL            bIsEot = LW_FALSE;
    BOOL            bStart = LW_FALSE;
    BOOL            bCrc16Mode = LW_FALSE;
    
    INT             iFile;
    INT             iRetVal;
    UCHAR           ucRead;
    PUCHAR          pucTemp;
    UCHAR           ucSeq = 1;
    UINT16          usCrc16;

    ssize_t         stPacketLen;
    ssize_t         stDataLen = 128;                                    /*  Ĭ�� Xmodem128              */
    ssize_t         sstRecvNum;
    ssize_t         sstReadNum;
    
    fd_set          fdsetRead;
    struct timeval  timevalTO;
    
    
    if ((iArgC < 2) || (iArgC > 3)) {
        fprintf(stderr, "arguments error!\n");
        return  (-ERROR_TSHELL_EPARAM);

    } else if (iArgC == 3) {
        if (lib_strcmp(ppcArgV[2], "1k") == 0) {                        /*  Xmodem1K                    */
            stDataLen = 1024;
        } else {
            fprintf(stderr, "arguments error!\n");
            return  (-ERROR_TSHELL_EPARAM);
        }
    }

    iFile = open(ppcArgV[1], O_RDONLY);                                 /*  ����Ҫ���͵��ļ�          */
    if (iFile < 0) {
        fprintf(stderr, "can not open source file!\n");
        return  (-ERROR_TSHELL_EPARAM);
    }
    
    API_ThreadCleanupPush(__tshellXmodemCleanup, (PVOID)(LONG)iFile);   /*  �����������                */
    
    ioctl(STD_IN, FIOSETOPTIONS, OPT_RAW);                              /*  ����׼�ļ���Ϊ raw ģʽ     */
    ioctl(STD_OUT, FIOSETOPTIONS, OPT_RAW);                             /*  ����׼�ļ���Ϊ raw ģʽ     */
    
    stPacketLen = stDataLen + 5;                                        /*  �� CRC16 ģʽ���������     */
    pucTemp     = (PUCHAR)__SHEAP_ALLOC(stPacketLen);
    if (!pucTemp) {
        __LW_XMODEM_SEND_CMD(__LW_XMODEM_EOT);                          /*  �������                    */
        API_ThreadCleanupPop(LW_TRUE);
        fprintf(stderr, "can not allocate memory!\n");
        return  (-ERROR_KERNEL_LOW_MEMORY);
    }

    if (stDataLen == 128) {
        pucTemp[0] = __LW_XMODEM_SOH;                                   /*  Xmodem128 ͷ��־            */
    } else {
        pucTemp[0] = __LW_XMODEM_STX;                                   /*  Xmodem1K ͷ��־             */
    }

    /*
     *  ������һ�����ݰ�
     */
    sstReadNum = read(iFile, &pucTemp[3], stDataLen);
    if (sstReadNum <= 0) {
        __LW_XMODEM_SEND_CMD(__LW_XMODEM_EOT);                          /*  �������                    */
        API_ThreadCleanupPop(LW_TRUE);
        __SHEAP_FREE(pucTemp);
        printf("0 bytes read from source file!\n");
        return  (-ERROR_TSHELL_EPARAM);
    }
    for (j = (INT)sstReadNum; j < stDataLen; j++) {                     /*  ��ȫ����                    */
        pucTemp[j + 3] = __LW_XMODEM_PAD;
    }
    
    pucTemp[1] = (UCHAR)(ucSeq);                                         /*  ���к�                     */
    pucTemp[2] = (UCHAR)(~ucSeq);
    
    /*
     *  ��ʼѭ�������ļ�
     */
    FD_ZERO(&fdsetRead);
    /*
     *  ��ʱ����Ĭ��Ϊ 20 ��
     */
    for (i = 0; i < 20; i++) {
        FD_SET(STD_IN, &fdsetRead);
        timevalTO.tv_sec  = __LW_XMODEM_TX_TIMEOUT;
        timevalTO.tv_usec = 0;
        iRetVal = select(1, &fdsetRead, LW_NULL, LW_NULL, &timevalTO);  /*  �ȴ��ɶ�                    */
        if (iRetVal != 1) {
            if (bIsEot) {
                if (bStart) {
                    write(STD_OUT, pucTemp, stPacketLen);               /*  �ط�����                    */
                }
            }
            continue;                                                   /*  ���ճ�ʱ                    */
        }
        i = 0;                                                          /*  ���ó�ʱ����                */
        
        sstRecvNum = read(STD_IN, &ucRead, 1);
        if (sstRecvNum <= 0) {
            API_ThreadCleanupPop(LW_TRUE);
            __SHEAP_FREE(pucTemp);
            fprintf(stderr, "standard in device error!\n");
            return  (PX_ERROR);
        }
        
        if (ucRead == __LW_XMODEM_CAN) {                                /*  ���ս���                    */
            break;
        
        } else if (ucRead == __LW_XMODEM_NAK) {                         /*  ��Ҫ�ط�                    */
            if (!bStart) {                                              /*  У���ģʽ                  */
                pucTemp[stDataLen + 3] = __xmodemChkSum(&pucTemp[3], stDataLen);
                stPacketLen -= 1;                                       /*  ����������                  */
            }
            write(STD_OUT, pucTemp, stPacketLen);                       /*  ��������                    */
            bStart = LW_TRUE;

        } else if (ucRead == __LW_XMODEM_CRC16) {                       /*  CRC16 ģʽ                  */
            if (!bStart) {
                usCrc16 = __xmodemCrc16(&pucTemp[3], stDataLen);        /*  ���� CRC16                  */
                pucTemp[stDataLen + 3] = (UCHAR)((usCrc16 >> 8) & 0xff);
                pucTemp[stDataLen + 4] = (UCHAR)(usCrc16 & 0xff);
                bCrc16Mode = LW_TRUE;
            }
            write(STD_OUT, pucTemp, stPacketLen);                       /*  ��������                    */
            bStart = LW_TRUE;

        } else if (ucRead == __LW_XMODEM_ACK) {
            ucSeq++;
            if (ucSeq > 255) {
                ucSeq = 1;
            }
            bStart = LW_TRUE;
            
            if (bIsEot) {
                break;                                                  /*  ���ͽ���                    */
            
            } else {
                sstReadNum = read(iFile, &pucTemp[3], stDataLen);
                if (sstReadNum <= 0) {
                    bIsEot = LW_TRUE;
                    __LW_XMODEM_SEND_CMD(__LW_XMODEM_EOT);              /*  ���ͽ���                    */
                
                } else {
                    /*
                     *  ��ȫ����
                     */
                    for (j = (INT)sstReadNum; j < stDataLen; j++) {
                        pucTemp[j + 3] = __LW_XMODEM_PAD;
                    }
                    
                    pucTemp[1] = (UCHAR)(ucSeq);                        /*  ���к�                      */
                    pucTemp[2] = (UCHAR)(~ucSeq);
                    
                    if (bCrc16Mode) {
                        usCrc16 = __xmodemCrc16(&pucTemp[3], stDataLen);/*  ���� CRC16                  */
                        pucTemp[stDataLen + 3] = (UCHAR)((usCrc16 >> 8) & 0xff);
                        pucTemp[stDataLen + 4] = (UCHAR)(usCrc16 & 0xff);
                    } else {                                            /*  ����У���                  */
                        pucTemp[stDataLen + 3] = __xmodemChkSum(&pucTemp[3], stDataLen);
                    }
                    
                    write(STD_OUT, pucTemp, stPacketLen);               /*  ��������                    */
                }
            }
        }
    }

    API_ThreadCleanupPop(LW_TRUE);
    __SHEAP_FREE(pucTemp);
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __tshellFsCmdXmodemr
** ��������: ϵͳ���� "xmodemr" (�� remote ����һ���ļ�)
** �䡡��  : iArgC         ��������
**           ppcArgV       ������
** �䡡��  : 0
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static INT  __tshellFsCmdXmodemr (INT  iArgC, PCHAR  ppcArgV[])
{
    INT             i;
    
    BOOL            bStart = LW_FALSE;
    BOOL            bCrc16Mode = LW_TRUE;

    INT             iFile;
    INT             iRetVal;
    PUCHAR          pucTemp;
    UCHAR           ucSeq = 1;

    ssize_t         stPacketLen;
    ssize_t         stDataLen = 1024;                                    /*  Ĭ�� Xmodem1K              */
    ssize_t         sstRecvNum;
    ssize_t         sstWriteNum;
    size_t          stTotalNum = 0;
    
    fd_set          fdsetRead;
    struct timeval  timevalTO;
    
    
    if (iArgC != 2) {
        fprintf(stderr, "arguments error!\n");
        return  (-ERROR_TSHELL_EPARAM);
    }
    
    stPacketLen = stDataLen + 5;                                        /*  �� CRC16 ģʽ���������     */
    pucTemp = (PUCHAR)__SHEAP_ALLOC(stPacketLen);
    if (!pucTemp) {
        fprintf(stderr, "can not allocate memory!\n");
        return  (-ERROR_KERNEL_LOW_MEMORY);
    }

    /*
     *  ����ļ��Ƿ����
     */
    iFile = open(ppcArgV[1], O_RDONLY);
    if (iFile >= 0) {
        close(iFile);                                                   /*  �ر��ļ�                    */
        
__re_select:
        printf("destination file already exists, overwrite? (Y/N)\n");
        read(0, pucTemp, stDataLen);
        if ((pucTemp[0] == 'N') ||
            (pucTemp[0] == 'n')) {                                      /*  ������                      */
            __SHEAP_FREE(pucTemp);
            return  (ERROR_NONE);
        
        } else if ((pucTemp[0] != 'Y') &&
                   (pucTemp[0] != 'y')) {                               /*  ѡ�����                    */
            goto    __re_select;
        }
    }
    
    iFile = open(ppcArgV[1], (O_WRONLY | O_CREAT | O_TRUNC), 0666);     /*  �����ļ�                    */
    if (iFile < 0) {
        __SHEAP_FREE(pucTemp);
        fprintf(stderr, "can not open destination file!\n");
        return  (-ERROR_TSHELL_EPARAM);
    }
    
    API_ThreadCleanupPush(__tshellXmodemCleanup, (PVOID)(LONG)iFile);   /*  �����������                */
    
    ioctl(STD_IN, FIOSETOPTIONS, OPT_RAW);                              /*  ����׼�ļ���Ϊ raw ģʽ     */
    ioctl(STD_OUT, FIOSETOPTIONS, OPT_RAW);                             /*  ����׼�ļ���Ϊ raw ģʽ     */
    
    __LW_XMODEM_SEND_CMD(__LW_XMODEM_CRC16);                            /*  ��������(CRC16 ģʽ)        */
    
    FD_ZERO(&fdsetRead);
    /*
     *  ��ʱ����Ĭ��Ϊ 20 ��
     */
    for (i = 0; i < 20; i++) {
        FD_SET(STD_IN, &fdsetRead);
        timevalTO.tv_sec  = __LW_XMODEM_RX_TIMEOUT;
        timevalTO.tv_usec = 0;
        iRetVal = select(1, &fdsetRead, LW_NULL, LW_NULL, &timevalTO);  /*  �ȴ��ɶ�                    */
        if (iRetVal != 1) {
            stTotalNum = 0;                                             /*  ����ѽ��յ�����            */
            if (bStart) {
                __LW_XMODEM_SEND_CMD(__LW_XMODEM_NAK);                  /*  ��������������ݰ�          */
            } else {
                if (i < 3) {
                    __LW_XMODEM_SEND_CMD(__LW_XMODEM_CRC16);            /*  ��������(CRC16 ģʽ)        */
                } else {
                    __LW_XMODEM_SEND_CMD(__LW_XMODEM_NAK);              /*  ��������(�л���У���ģʽ)  */
                    bCrc16Mode = LW_FALSE;
                }
            }
            continue;                                                   /*  ���ճ�ʱ                    */
        }
        i = 0;                                                          /*  ���ó�ʱ����                */
    
        sstRecvNum = read(STD_IN, &pucTemp[stTotalNum], stPacketLen - stTotalNum);
        if (sstRecvNum <= 0) {
            API_ThreadCleanupPop(LW_TRUE);
            __SHEAP_FREE(pucTemp);
            fprintf(stderr, "standard in device error!\n");
            return  (PX_ERROR);
        }

        if (pucTemp[0] == __LW_XMODEM_EOT) {                            /*  ���ս���                    */
            __LW_XMODEM_SEND_CMD(__LW_XMODEM_ACK);
            break;

        } else if (pucTemp[0] == __LW_XMODEM_STX) {                     /*  Xmodem1K                    */
            bStart    = LW_TRUE;
            stDataLen = 1024;
            if (bCrc16Mode) {
                stPacketLen = stDataLen + 5;
            } else {
                stPacketLen = stDataLen + 4;
            }

        } else if (pucTemp[0] == __LW_XMODEM_SOH) {                     /*  Xmodem128                   */
            bStart    = LW_TRUE;
            stDataLen = 128;
            if (bCrc16Mode) {
                stPacketLen = stDataLen + 5;
            } else {
                stPacketLen = stDataLen + 4;
            }
        }

        stTotalNum += (size_t)sstRecvNum;
        if (stTotalNum < stPacketLen) {                                 /*  ���ݰ�������                */
            continue;
        } else {
            stTotalNum = 0;                                             /*  �Ѿ��ӵ�һ�����������ݰ�    */
        }
        
        /*
         *  ��ʼ�ж����ݰ���ȷ��
         */
        if (pucTemp[1] != ucSeq) {                                      /*  ���к��Ƿ����              */
            if (pucTemp[1] == (ucSeq - 1)) {
                __LW_XMODEM_SEND_CMD(__LW_XMODEM_ACK);                  /*  ��Ҫ��һ������              */
            } else {
                __LW_XMODEM_SEND_CMD(__LW_XMODEM_CAN);                  /*  ����ͨ��                    */
                API_ThreadCleanupPop(LW_TRUE);
                __SHEAP_FREE(pucTemp);
                fprintf(stderr, "sequence number error!\n");
                return  (PX_ERROR);
            }
            continue;
        }
        
        if (~pucTemp[1] == pucTemp[2]) {                                /*  ���к�У�����              */
            __LW_XMODEM_SEND_CMD(__LW_XMODEM_NAK);
            continue;
        }
        
        if (bCrc16Mode) {
            UINT16  usCrc16 = __xmodemCrc16(&pucTemp[3], stDataLen);

            if ((pucTemp[stDataLen + 3] != (UCHAR)((usCrc16 >> 8) & 0xff)) ||
                (pucTemp[stDataLen + 4] != (UCHAR)((usCrc16 & 0xff)))) {/*  CRC16 ����                  */
                __LW_XMODEM_SEND_CMD(__LW_XMODEM_NAK);
                continue;
            }
        } else {
            UCHAR  ucChkSum = __xmodemChkSum(&pucTemp[3], stDataLen);

            if (pucTemp[stDataLen + 3] != ucChkSum) {                   /*  У��ʹ���                  */
                __LW_XMODEM_SEND_CMD(__LW_XMODEM_NAK);
                continue;
            }
        }

        /*
         *  ������д��Ŀ���ļ�
         */
        sstWriteNum = write(iFile, &pucTemp[3], stDataLen);
        if (sstWriteNum != stDataLen) {
            INT     iErrNo = errno;
            __LW_XMODEM_SEND_CMD(__LW_XMODEM_CAN);                      /*  ����ͨ��                    */
            API_ThreadCleanupPop(LW_TRUE);
            __SHEAP_FREE(pucTemp);
            fprintf(stderr, "write file error %s!\n", lib_strerror(iErrNo));
            return  (PX_ERROR);
        }
        
        ucSeq++;
        __LW_XMODEM_SEND_CMD(__LW_XMODEM_ACK);                          /*  ��Ҫ��һ������              */
    }
    
    API_ThreadCleanupPop(LW_TRUE);
    __SHEAP_FREE(pucTemp);
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __tshellModemCmdInit
** ��������: ��ʼ�� modem ������
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  __tshellModemCmdInit (VOID)
{
    API_TShellKeywordAdd("xmodems", __tshellFsCmdXmodems);
    API_TShellFormatAdd("xmodems", " file path [1k]");
    API_TShellHelpAdd("xmodems", "send a file use xmodem protocol.\n"
                                 "xmodems file 1k\n");
    
    API_TShellKeywordAdd("xmodemr", __tshellFsCmdXmodemr);
    API_TShellFormatAdd("xmodemr", " file path");
    API_TShellHelpAdd("xmodemr", "receive a file use xmodem protocol.\n"
                                 "xmodemr file\n");
}

#endif                                                                  /*  LW_CFG_SHELL_EN > 0         */
/*********************************************************************************************************
  END
*********************************************************************************************************/
