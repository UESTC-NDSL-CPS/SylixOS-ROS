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
** ��   ��   ��: diskCacheLib.h
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2008 �� 11 �� 26 ��
**
** ��        ��: ���̸��ٻ���������ڲ�������.

** BUG
2008.12.16  __diskCacheFlushInvalidate2() ����ָ���Ĵ���Ϊ��д������������.
2009.03.16  �����д��·��֧��.
2009.03.18  FIOCANCEL �� CACHE ֹͣ, û�л�д������ȫ������. �ڴ�ص���ʼ״̬.
2009.07.22  ���Ӷ� FIOSYNC ��֧��.
2009.11.03  FIOCANCEL ����Ӧ�ô�����������, ʹ�������������.
            FIODISKCHANGE �� FIOCANCEL ������ͬ.
2015.02.05  ���� __diskCacheNodeReadData() �������жϴ���.
2016.07.12  ����д��������һ���ļ�ϵͳ�����.
2016.07.13  ��� HASH ����Ч��.
2016.07.27  ��ʹ��ר�õĴ���������������.
2017.09.02  �� FIOTRIM��SYNC ����������д�����Ļ������ disk cache �������.
2022.10.25  ֧�ָ����豸��ֱ�Ӷ�д.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "../SylixOS/kernel/include/k_kernel.h"
#include "../SylixOS/system/include/s_system.h"
#include "diskCache.h"
#include "diskCacheLib.h"
#include "diskCachePipeline.h"
/*********************************************************************************************************
  �ü���
*********************************************************************************************************/
#if (LW_CFG_MAX_VOLUMES > 0) && (LW_CFG_DISKCACHE_EN > 0)
/*********************************************************************************************************
  ʹ�����ŵ� LRU ����������������Ч��
*********************************************************************************************************/
#define __LW_DISKCACHE_OPTIMUM_LRU          0
#define __LW_DISKCACHE_FLUSH_META_MAX_HASH  32
/*********************************************************************************************************
  �ڲ������
*********************************************************************************************************/
#define __LW_DISKCACHE_DISK_IOCTL(pdiskcDiskCache)              \
        pdiskcDiskCache->DISKC_pblkdDisk->BLKD_pfuncBlkIoctl
#define __LW_DISKCACHE_DISK_RESET(pdiskcDiskCache)              \
        pdiskcDiskCache->DISKC_pblkdDisk->BLKD_pfuncBlkReset
#define __LW_DISKCACHE_DISK_STATUS(pdiskcDiskCache)             \
        pdiskcDiskCache->DISKC_pblkdDisk->BLKD_pfuncBlkStatusChk
/*********************************************************************************************************
  FS callback
*********************************************************************************************************/
#define __LW_DISKCACHE_FS_CALLBACK(pdiskcDiskCache, pdiskn, ulSectorCount) \
        if (pdiskcDiskCache->DISKC_pfuncFsCallback) { \
            pdiskcDiskCache->DISKC_pfuncFsCallback(pdiskcDiskCache->DISKC_pvFsArg, \
                                                   pdiskn->DISKN_u64FsKey, \
                                                   pdiskn->DISKN_ulSectorNo, \
                                                   ulSectorCount); \
        }
/*********************************************************************************************************
  Is Need Direct Op
*********************************************************************************************************/
#define __LW_DISKCACHE_NEED_DIRECT(pdiskcDiskCache, nsectors) \
        (pdiskcDiskCache->DISKC_iDirectThreshold && \
         (nsectors >= pdiskcDiskCache->DISKC_iDirectThreshold))
/*********************************************************************************************************
  Is Write Through
*********************************************************************************************************/
#define __LW_DISKCACHE_IS_WRITETHROUGH(pdiskcDiskCache) \
        (pdiskcDiskCache->DISKC_iCacheOpt & LW_DCATTR_BOPT_WRITE_THROUGH)
/*********************************************************************************************************
  ���������
*********************************************************************************************************/
#define __LW_DISKCACHE_MAX_SECNO  (ULONG)PX_ERROR
#define __LW_DISKCACHE_SEC_COUNT(s, e)  \
        ({  ULONG c = e - s; if (c != __LW_DISKCACHE_MAX_SECNO) { c++; } c; })
/*********************************************************************************************************
** ��������: __diskCacheMemcpy
** ��������: ��������
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           pvTo               Ŀ��
**           pvFrom             Դ
**           stSize             �ֽ��� (������ 512 �ֽڱ���)
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
LW_WEAK VOID  __diskCacheMemcpy (PVOID  pvTo, CPVOID  pvFrom, size_t  stSize)
{
    REGISTER INT      iMuti512;
    REGISTER INT      i;
    REGISTER UINT64  *pu64To   = (UINT64 *)pvTo;
    REGISTER UINT64  *pu64From = (UINT64 *)pvFrom;
    
    if (((addr_t)pvTo & 0x7) || ((addr_t)pvFrom & 0x7)) {
        lib_memcpy(pvTo, pvFrom, stSize);
        return;
    }
    
    iMuti512 = stSize >> 9;
    
    do {
        for (i = 0; i < 64; i += 16) {
            *pu64To++ = *pu64From++;
            *pu64To++ = *pu64From++;
            *pu64To++ = *pu64From++;
            *pu64To++ = *pu64From++;
            *pu64To++ = *pu64From++;
            *pu64To++ = *pu64From++;
            *pu64To++ = *pu64From++;
            *pu64To++ = *pu64From++;
            *pu64To++ = *pu64From++;
            *pu64To++ = *pu64From++;
            *pu64To++ = *pu64From++;
            *pu64To++ = *pu64From++;
            *pu64To++ = *pu64From++;
            *pu64To++ = *pu64From++;
            *pu64To++ = *pu64From++;
            *pu64To++ = *pu64From++;
        }
    } while (--iMuti512);
}
/*********************************************************************************************************
** ��������: __diskCacheMemReset
** ��������: ���³�ʼ������ CACHE �������ڴ������
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
LW_STATIC INT  __diskCacheMemReset (PLW_DISKCACHE_CB   pdiskcDiskCache)
{
    REGISTER INT                    i;
    REGISTER PLW_DISKCACHE_NODE     pdiskn = (PLW_DISKCACHE_NODE)pdiskcDiskCache->DISKC_pcCacheNodeMem;
    REGISTER PCHAR                  pcData = (PCHAR)pdiskcDiskCache->DISKC_pcCacheMem;
    
    pdiskcDiskCache->DISKC_ulValidCounter = 0;                          /*  û����Ч������              */
    pdiskcDiskCache->DISKC_ulDirtyCounter = 0;                          /*  û����Ҫ��д������          */
    
    pdiskcDiskCache->DISKC_pringLruHeader = LW_NULL;                    /*  ��ʼ�� LRU ��               */
    lib_bzero(pdiskcDiskCache->DISKC_pplineHash,
              (sizeof(PVOID) * pdiskcDiskCache->DISKC_iHashSize));      /*  ��ʼ�� HASH ��              */
    
    for (i = 0; i < pdiskcDiskCache->DISKC_ulNCacheNode; i++) {
        pdiskn->DISKN_ulSectorNo = __LW_DISKCACHE_MAX_SECNO;
        pdiskn->DISKN_iStatus    = 0;
        pdiskn->DISKN_pcData     = pcData;
        
        _List_Ring_Add_Last(&pdiskn->DISKN_ringLru, 
                            &pdiskcDiskCache->DISKC_pringLruHeader);    /*  ���� LRU ��                 */
        _LIST_LINE_INIT_IN_CODE(pdiskn->DISKN_lineHash);                /*  ��ʼ�� HASH ��              */
    
        pdiskn++;
        pcData += pdiskcDiskCache->DISKC_ulBytesPerSector;
    }
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __diskCacheMemInit
** ��������: ��ʼ������ CACHE �������ڴ������
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           pvDiskCacheMem     ������
**           ulBytesPerSector   ������С
**           ulNSector          ��������
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INT  __diskCacheMemInit (PLW_DISKCACHE_CB   pdiskcDiskCache,
                         VOID              *pvDiskCacheMem,
                         ULONG              ulBytesPerSector,
                         ULONG              ulNSector)
{
    REGISTER PCHAR  pcData = (PCHAR)pvDiskCacheMem;

    pdiskcDiskCache->DISKC_ulNCacheNode = ulNSector;
    pdiskcDiskCache->DISKC_pcCacheMem   = (caddr_t)pcData;              /*  CACHE ������                */
    
    pdiskcDiskCache->DISKC_pcCacheNodeMem = (caddr_t)__SHEAP_ALLOC(sizeof(LW_DISKCACHE_NODE) * 
                                                                   (size_t)ulNSector);
    if (pdiskcDiskCache->DISKC_pcCacheNodeMem == LW_NULL) {
        return  (PX_ERROR);
    }
    
    return  (__diskCacheMemReset(pdiskcDiskCache));
}
/*********************************************************************************************************
** ��������: __diskCacheHashAdd
** ��������: ��ָ�� CACHE �ڵ���뵽��ص� HASH ����
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           pdiskn             ��Ҫ����� CACHE �ڵ�
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
LW_STATIC VOID  __diskCacheHashAdd (PLW_DISKCACHE_CB   pdiskcDiskCache, PLW_DISKCACHE_NODE  pdiskn)
{
    REGISTER PLW_LIST_LINE      *pplineHashEntry;
    
    pplineHashEntry = &pdiskcDiskCache->DISKC_pplineHash[
                       pdiskn->DISKN_ulSectorNo &
                       (pdiskcDiskCache->DISKC_iHashSize - 1)];         /*  ��� HASH �����            */
                     
    _List_Line_Add_Ahead(&pdiskn->DISKN_lineHash, pplineHashEntry);     /*  ���� HASH ��                */

    pdiskcDiskCache->DISKC_ulValidCounter++;
}
/*********************************************************************************************************
** ��������: __diskCacheHashRemove
** ��������: ��ָ�� CACHE �ڵ���뵽��ص� HASH ����
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           pdiskn             ��Ҫ����� CACHE �ڵ�
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
LW_STATIC VOID  __diskCacheHashRemove (PLW_DISKCACHE_CB   pdiskcDiskCache, PLW_DISKCACHE_NODE  pdiskn)
{
    REGISTER PLW_LIST_LINE      *pplineHashEntry;
    
    pplineHashEntry = &pdiskcDiskCache->DISKC_pplineHash[
                       pdiskn->DISKN_ulSectorNo &
                       (pdiskcDiskCache->DISKC_iHashSize - 1)];         /*  ��� HASH �����            */
                       
    _List_Line_Del(&pdiskn->DISKN_lineHash, pplineHashEntry);
    
    pdiskn->DISKN_ulSectorNo = __LW_DISKCACHE_MAX_SECNO;
    pdiskn->DISKN_iStatus    = 0;                                       /*  ����λ����Чλ              */

    pdiskcDiskCache->DISKC_ulValidCounter--;
}
/*********************************************************************************************************
** ��������: __diskCacheHashFind
** ��������: �� HASH ����Ѱ��ָ�����Ե�����
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           ulSectorNo         �����ı��
** �䡡��  : Ѱ�ҵ��Ľڵ�
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
LW_STATIC PLW_DISKCACHE_NODE  __diskCacheHashFind (PLW_DISKCACHE_CB   pdiskcDiskCache, ULONG  ulSectorNo)
{
    REGISTER PLW_LIST_LINE          plineHash;
    REGISTER PLW_DISKCACHE_NODE     pdiskn;
    
    plineHash = pdiskcDiskCache->DISKC_pplineHash[
                ulSectorNo & 
                (pdiskcDiskCache->DISKC_iHashSize - 1)];                /*  ��� HASH �����            */
                       
    for (; plineHash != LW_NULL; plineHash = _list_line_get_next(plineHash)) {
        pdiskn = _LIST_ENTRY(plineHash, LW_DISKCACHE_NODE, DISKN_lineHash);
        if (pdiskn->DISKN_ulSectorNo == ulSectorNo) {
            return  (pdiskn);                                           /*  Ѱ�ҵ�                      */
        }
    }
    
    return  (LW_NULL);
}
/*********************************************************************************************************
** ��������: __diskCacheLruFind
** ��������: �� LRU ����Ѱ��ָ�����Ե�����
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           ulSectorNo         �����ı��
** �䡡��  : Ѱ�ҵ��Ľڵ�
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
#if __LW_DISKCACHE_OPTIMUM_LRU > 0

LW_STATIC PLW_DISKCACHE_NODE  __diskCacheLruFind (PLW_DISKCACHE_CB   pdiskcDiskCache, ULONG  ulSectorNo)
{
             PLW_LIST_RING          pringLruHeader = pdiskcDiskCache->DISKC_pringLruHeader;
    REGISTER PLW_LIST_RING          pringTemp;
    REGISTER PLW_DISKCACHE_NODE     pdiskn;
    
    if (_LIST_RING_IS_EMPTY(pringLruHeader)) {
        return  (LW_NULL);
    }
    
    pdiskn = (PLW_DISKCACHE_NODE)pringLruHeader;
    if (pdiskn->DISKN_ulSectorNo == ulSectorNo) {
        return  (pdiskn);                                               /*  Ѱ�ҵ�                      */
    }
    
    for (pringTemp  = _list_ring_get_next(pringLruHeader);
         pringTemp != pringLruHeader;
         pringTemp  = _list_ring_get_next(pringTemp)) {                 /*  ���� LRU ��                 */
         
        pdiskn = (PLW_DISKCACHE_NODE)pringTemp;
        if (pdiskn->DISKN_ulSectorNo == ulSectorNo) {
            return  (pdiskn);                                           /*  Ѱ�ҵ�                      */
        }
    }
    
    return  (LW_NULL);
}

#endif                                                                  /*  __LW_DISKCACHE_OPTIMUM_LRU  */
/*********************************************************************************************************
** ��������: __diskCacheSortListAdd
** ��������: ��ָ�� CACHE �ڵ���뵽һ�����������������
** �䡡��  : pringListHeader    �����ͷ
**           pdiskn             ��Ҫ����� CACHE �ڵ�
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
LW_STATIC VOID  __diskCacheSortListAdd (PLW_LIST_RING  *ppringListHeader, PLW_DISKCACHE_NODE  pdiskn)
{
    REGISTER PLW_LIST_RING          pringTemp;
             PLW_LIST_RING          pringDummyHeader;
    REGISTER PLW_DISKCACHE_NODE     pdisknTemp;

    if (_LIST_RING_IS_EMPTY(*ppringListHeader)) {
        _List_Ring_Add_Ahead(&pdiskn->DISKN_ringLru, 
                             ppringListHeader);
        return;
    }
    
    pdisknTemp = (PLW_DISKCACHE_NODE)(*ppringListHeader);               /*  LRU ��Ϊ CACHE �ڵ��׸�Ԫ�� */
    if (pdiskn->DISKN_ulSectorNo < pdisknTemp->DISKN_ulSectorNo) {
        _List_Ring_Add_Ahead(&pdiskn->DISKN_ringLru, 
                             ppringListHeader);                         /*  ֱ�Ӳ����ͷ                */
        return;
    }
    
    for (pringTemp  = _list_ring_get_next(*ppringListHeader);
         pringTemp != *ppringListHeader;
         pringTemp  = _list_ring_get_next(pringTemp)) {                 /*  �ӵڶ����ڵ����Բ���        */
        
        pdisknTemp = (PLW_DISKCACHE_NODE)pringTemp;
        if (pdiskn->DISKN_ulSectorNo < pdisknTemp->DISKN_ulSectorNo) {
            pringDummyHeader = pringTemp;
            _List_Ring_Add_Last(&pdiskn->DISKN_ringLru,
                                &pringDummyHeader);                     /*  ���뵽���ʵ�λ��            */
            return;
        }
    }
    
    _List_Ring_Add_Last(&pdiskn->DISKN_ringLru,
                        ppringListHeader);                              /*  ���뵽�������              */
}
/*********************************************************************************************************
** ��������: __diskCacheFlushList
** ��������: ��ָ�������ڵ� CACHE �ڵ�����ȫ����д����
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           pringFlushHeader   ����ͷ
**           bMakeInvalidate    �Ƿ��ڻ�д����ؽڵ���Ϊ��Ч״̬
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
LW_STATIC INT  __diskCacheFlushList (PLW_DISKCACHE_CB   pdiskcDiskCache, 
                                     PLW_LIST_RING      pringFlushHeader,
                                     BOOL               bMakeInvalidate)
{
    REGISTER INT                    i;
             PLW_DISKCACHE_NODE     pdiskn;
    REGISTER PLW_DISKCACHE_NODE     pdisknContinue;
             PLW_LIST_RING          pringTemp;
             
    REGISTER INT                    iBurstCount;
             PVOID                  pvBuffer;
             PCHAR                  pcBurstBuffer;
             INT                    iRetVal;
             BOOL                   bHasError = LW_FALSE;
    
    for (pringTemp  = pringFlushHeader;
         pringTemp != LW_NULL;
         pringTemp  = pringFlushHeader) {                               /*  ֱ������Ϊ��Ϊֹ            */
        
        pdiskn = (PLW_DISKCACHE_NODE)pringTemp;                         /*  DISKN_ringLru �ǵ�һ��Ԫ��  */
        pdisknContinue = pdiskn;
        
        for (iBurstCount = 1; 
             iBurstCount < pdiskcDiskCache->DISKC_iMaxWBurstSector;
             iBurstCount++) {
            
            pdisknContinue = 
                (PLW_DISKCACHE_NODE)_list_ring_get_next(&pdisknContinue->DISKN_ringLru);
            if (pdisknContinue->DISKN_ulSectorNo != 
                (pdiskn->DISKN_ulSectorNo + iBurstCount)) {             /*  �ж��Ƿ�Ϊ��������          */
                break;
            }
        }
        
        pvBuffer = __diskCacheWpGetBuffer(&pdiskcDiskCache->DISKC_wpWrite, LW_FALSE);
        if (iBurstCount == 1) {                                         /*  ���ܽ���⧷�����            */
            __diskCacheMemcpy(pvBuffer, pdiskn->DISKN_pcData,
                              (size_t)pdiskcDiskCache->DISKC_ulBytesPerSector);
        
        } else {                                                        /*  ����ʹ��⧷�д��            */
            pcBurstBuffer  = (PCHAR)pvBuffer;
            pdisknContinue = pdiskn;
            
            for (i = 0; i < iBurstCount; i++) {                         /*  װ��⧷���������            */
                __diskCacheMemcpy(pcBurstBuffer, 
                       pdisknContinue->DISKN_pcData, 
                       (size_t)pdiskcDiskCache->DISKC_ulBytesPerSector);/*  ��������                    */
                           
                pdisknContinue = 
                    (PLW_DISKCACHE_NODE)_list_ring_get_next(&pdisknContinue->DISKN_ringLru);
                pcBurstBuffer += pdiskcDiskCache->DISKC_ulBytesPerSector;
            }
        }
        
        iRetVal = __diskCacheWpWrite(pdiskcDiskCache, pdiskcDiskCache->DISKC_pblkdDisk,
                                     pvBuffer, pdiskn->DISKN_ulSectorNo,
                                     iBurstCount);                      /*  ⧷�д������                */
        if (iRetVal < 0) {
            __diskCacheWpPutBuffer(&pdiskcDiskCache->DISKC_wpWrite, pvBuffer);
            bHasError       = LW_TRUE;
            bMakeInvalidate = LW_TRUE;                                  /*  ��дʧ��, ��Ҫ��Ч�豸      */
        
        } else {
            __LW_DISKCACHE_FS_CALLBACK(pdiskcDiskCache,
                                       pdiskn, iBurstCount);            /*  ִ���ļ�ϵͳ�ص�            */
        }
        
        for (i = 0; i < iBurstCount; i++) {                             /*  ��ʼ������Щ��д��������¼  */
            __LW_DISKCACHE_CLR_DIRTY(pdiskn);
            
            if (bMakeInvalidate) {
                __diskCacheHashRemove(pdiskcDiskCache, pdiskn);         /*  ����Ч�� HASH ����ɾ��      */
            }
            
            pdisknContinue = 
                (PLW_DISKCACHE_NODE)_list_ring_get_next(&pdiskn->DISKN_ringLru);
                
            _List_Ring_Del(&pdiskn->DISKN_ringLru, 
                           &pringFlushHeader);                          /*  ����ʱ��д������ɾ��        */
            _List_Ring_Add_Last(&pdiskn->DISKN_ringLru, 
                           &pdiskcDiskCache->DISKC_pringLruHeader);     /*  ���뵽���� CACHE LRU ����   */
                                 
            pdiskn = pdisknContinue;                                    /*  ��һ�������ڵ�              */
        }
        
        pdiskcDiskCache->DISKC_ulDirtyCounter -= iBurstCount;           /*  ���¼�����Ҫ��д��������    */
    }
    
    if (bHasError) {
        return  (PX_ERROR);
    } else {
        return  (ERROR_NONE);
    }
}
/*********************************************************************************************************
** ��������: __diskCacheFlushInvalidate
** ��������: ��ָ������ CACHE ��ָ��������Χ��д���Ұ�Ѱ������Ϊ��Ч.
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           ulStartSector      ��ʼ����
**           ulEndSector        ��������
**           bMakeFlush         �Ƿ���� FLUSH ����
**           bMakeInvalidate    �Ƿ���� INVALIDATE ����
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
LW_STATIC INT  __diskCacheFlushInvRange (PLW_DISKCACHE_CB   pdiskcDiskCache,
                                         ULONG              ulStartSector,
                                         ULONG              ulEndSector,
                                         BOOL               bMakeFlush,
                                         BOOL               bMakeInvalidate)
{
             INT                    i;
    REGISTER PLW_DISKCACHE_NODE     pdiskn;
    REGISTER PLW_DISKCACHE_NODE     pdisknPrev;
             PLW_LIST_RING          pringFlushHeader = LW_NULL;
                                                                        /*  ��������û��ʹ�õĿ�ʼ    */
    pdiskn = (PLW_DISKCACHE_NODE)_list_ring_get_prev(pdiskcDiskCache->DISKC_pringLruHeader);
    
    for (i = 0; i < pdiskcDiskCache->DISKC_ulNCacheNode; i++) {
    
        pdisknPrev = (PLW_DISKCACHE_NODE)_list_ring_get_prev(&pdiskn->DISKN_ringLru);
        
        if ((pdiskn->DISKN_ulSectorNo > ulEndSector) ||
            (pdiskn->DISKN_ulSectorNo < ulStartSector)) {               /*  �����趨�ķ�Χ��            */
            goto    __check_again;
        }
        
        if (__LW_DISKCACHE_IS_DIRTY(pdiskn)) {                          /*  �������Ҫ��д              */
            if (bMakeFlush) {
                _List_Ring_Del(&pdiskn->DISKN_ringLru, 
                               &pdiskcDiskCache->DISKC_pringLruHeader);
                __diskCacheSortListAdd(&pringFlushHeader, pdiskn);      /*  ���뵽����ȴ���д����      */
            }
            goto    __check_again;
        }
        
        if (__LW_DISKCACHE_IS_VALID(pdiskn)) {                          /*  ����Ƿ���Ч                */
            if (bMakeInvalidate) {
                __diskCacheHashRemove(pdiskcDiskCache, pdiskn);         /*  ����Ч�� HASH ����ɾ��      */
            }
        }
        
__check_again:
        pdiskn = pdisknPrev;                                            /*  ���� LRU ��                 */
    }
    
    if (!_LIST_RING_IS_EMPTY(pringFlushHeader)) {                       /*  �Ƿ���Ҫ��д                */
        return  (__diskCacheFlushList(pdiskcDiskCache, pringFlushHeader, bMakeInvalidate));
    } else {
        return  (ERROR_NONE);
    }
}
/*********************************************************************************************************
** ��������: __diskCacheFlushInv
** ��������: ��ָ������ CACHE ��ָ���ؼ�����д���Ұ�Ѱ������Ϊ��Ч.
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           ulStartSector      ��ʼ����
**           ulEndSector        ��������
**           bMakeFlush         �Ƿ���� FLUSH ����
**           bMakeInvalidate    �Ƿ���� INVALIDATE ����
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
LW_STATIC INT  __diskCacheFlushInvMeta (PLW_DISKCACHE_CB   pdiskcDiskCache,
                                        ULONG              ulStartSector,
                                        ULONG              ulEndSector,
                                        BOOL               bMakeFlush,
                                        BOOL               bMakeInvalidate)
{
             ULONG                  i;
             ULONG                  ulCnt;
    REGISTER PLW_DISKCACHE_NODE     pdiskn;
             PLW_LIST_RING          pringFlushHeader = LW_NULL;
    
    ulCnt = __LW_DISKCACHE_SEC_COUNT(ulStartSector, ulEndSector);

    if (ulCnt > __LW_DISKCACHE_FLUSH_META_MAX_HASH) {                   /*  �����Ż���С                */
        return  (__diskCacheFlushInvRange(pdiskcDiskCache, 
                                          ulStartSector, 
                                          ulEndSector, 
                                          bMakeFlush, 
                                          bMakeInvalidate));
    }
    
    for (i = 0; i < ulCnt; i++) {                                       /*  ѭ������                    */
        pdiskn = __diskCacheHashFind(pdiskcDiskCache, ulEndSector);     /*  ���ȴ���Ч�� HASH ���в���  */
        if (pdiskn == LW_NULL) {
            ulEndSector--;
            continue;
        }
        
        if (__LW_DISKCACHE_IS_DIRTY(pdiskn)) {                          /*  �������Ҫ��д              */
            if (bMakeFlush) {
                _List_Ring_Del(&pdiskn->DISKN_ringLru, 
                               &pdiskcDiskCache->DISKC_pringLruHeader);
                __diskCacheSortListAdd(&pringFlushHeader, pdiskn);      /*  ���뵽����ȴ���д����      */
            }
            ulEndSector--;
            continue;
        }
        
        if (bMakeInvalidate) {
            __diskCacheHashRemove(pdiskcDiskCache, pdiskn);             /*  ����Ч�� HASH ����ɾ��      */
        }

        ulEndSector--;                                                  /*  ����ѭ��, �����������      */
    }
    
    if (!_LIST_RING_IS_EMPTY(pringFlushHeader)) {                       /*  �Ƿ���Ҫ��д                */
        return  (__diskCacheFlushList(pdiskcDiskCache, pringFlushHeader, bMakeInvalidate));
    } else {
        return  (ERROR_NONE);
    }
}
/*********************************************************************************************************
** ��������: __diskCacheFlushInvCnt
** ��������: ��ָ������ CACHE ��ָ������������д���Ұ�Ѱ������Ϊ��Ч. (����2)
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           iLruSectorNum      ���δʹ�õ���������
**           bMakeFlush         �Ƿ���� FLUSH ����
**           bMakeInvalidate    �Ƿ���� INVALIDATE ����
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
LW_STATIC INT  __diskCacheFlushInvCnt (PLW_DISKCACHE_CB   pdiskcDiskCache,
                                       INT                iLruSectorNum,
                                       BOOL               bMakeFlush,
                                       BOOL               bMakeInvalidate)
{
             INT                    i = 0;
    REGISTER PLW_DISKCACHE_NODE     pdiskn;
    REGISTER PLW_DISKCACHE_NODE     pdisknPrev;
             PLW_LIST_RING          pringFlushHeader = LW_NULL;
             
    if (iLruSectorNum < 1) {
        return  (ERROR_NONE);                                           /*  ����Ҫ����                  */
    }
    
    if (iLruSectorNum > (INT)pdiskcDiskCache->DISKC_ulDirtyCounter) {
        iLruSectorNum = (INT)pdiskcDiskCache->DISKC_ulDirtyCounter;     /*  ��ദ�� dirty count ������ */
    }
                                                                        /*  ��������û��ʹ�õĿ�ʼ    */
    pdiskn = (PLW_DISKCACHE_NODE)_list_ring_get_prev(pdiskcDiskCache->DISKC_pringLruHeader);
    
    for (i = 0; 
         (i < pdiskcDiskCache->DISKC_ulNCacheNode) && (iLruSectorNum > 0);
         i++) {
    
        pdisknPrev = (PLW_DISKCACHE_NODE)_list_ring_get_prev(&pdiskn->DISKN_ringLru);
        
        if (__LW_DISKCACHE_IS_DIRTY(pdiskn)) {                          /*  �������Ҫ��д              */
            if (bMakeFlush) {
                _List_Ring_Del(&pdiskn->DISKN_ringLru, 
                               &pdiskcDiskCache->DISKC_pringLruHeader);
                __diskCacheSortListAdd(&pringFlushHeader, pdiskn);      /*  ���뵽����ȴ���д����      */
            }
            iLruSectorNum--;
            goto    __check_again;
        }
        
        if (__LW_DISKCACHE_IS_VALID(pdiskn)) {                          /*  ����Ƿ���Ч                */
            if (bMakeInvalidate) {
                __diskCacheHashRemove(pdiskcDiskCache, pdiskn);         /*  ����Ч�� HASH ����ɾ��      */
                iLruSectorNum--;
            }
        }
    
__check_again:
        pdiskn = pdisknPrev;                                            /*  ���� LRU ��                 */
    }
    
    if (!_LIST_RING_IS_EMPTY(pringFlushHeader)) {                       /*  �Ƿ���Ҫ��д                */
        return  (__diskCacheFlushList(pdiskcDiskCache, pringFlushHeader, bMakeInvalidate));
    } else {
        return  (ERROR_NONE);
    }
}
/*********************************************************************************************************
** ��������: __diskCacheNodeFind
** ��������: Ѱ��һ��ָ���� CACHE ��
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           ulSectorNo         ������
** �䡡��  : Ѱ�ҵ��Ľڵ�
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
LW_STATIC PLW_DISKCACHE_NODE  __diskCacheNodeFind (PLW_DISKCACHE_CB   pdiskcDiskCache, ULONG  ulSectorNo)
{
    REGISTER PLW_DISKCACHE_NODE         pdiskn;
    
    pdiskn = __diskCacheHashFind(pdiskcDiskCache, ulSectorNo);          /*  ���ȴ���Ч�� HASH ���в���  */
    
#if __LW_DISKCACHE_OPTIMUM_LRU > 0
    if (pdiskn) {
        return  (pdiskn);                                               /*  HASH ������                 */
    }
    
    pdiskn = __diskCacheLruFind(pdiskcDiskCache, ulSectorNo);           /*  ��ʼ�� LRU ��������         */
#endif                                                                  /*  __LW_DISKCACHE_OPTIMUM_LRU  */
    
    return  (pdiskn);
}
/*********************************************************************************************************
** ��������: __diskCacheNodeAlloc
** ��������: ����һ�� CACHE ��
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           ulSectorNo         ������
**           iExpectation       ����Ԥ���ٵĶ������������ (δ��, ʹ�� DISKC_iMaxBurstSector)
** �䡡��  : �������Ľڵ�
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
LW_STATIC PLW_DISKCACHE_NODE  __diskCacheNodeAlloc (PLW_DISKCACHE_CB   pdiskcDiskCache, 
                                                    ULONG              ulSectorNo,
                                                    INT                iExpectation)
{
    REGISTER PLW_LIST_RING          pringTemp;
    REGISTER PLW_DISKCACHE_NODE     pdiskn = LW_NULL;                   /*  For no warning              */
    
    if (pdiskcDiskCache->DISKC_ulNCacheNode == 
        pdiskcDiskCache->DISKC_ulDirtyCounter) {                        /*  ���� CACHE ȫ����Ϊ��       */
        pringTemp = pdiskcDiskCache->DISKC_pringLruHeader;
        goto    __try_flush;
    }
    
__check_again:                                                          /*  ��������û��ʹ�õĿ�ʼ    */
    for (pringTemp  = _list_ring_get_prev(pdiskcDiskCache->DISKC_pringLruHeader);
         pringTemp != pdiskcDiskCache->DISKC_pringLruHeader;
         pringTemp  = _list_ring_get_prev(pringTemp)) {
        
        pdiskn = (PLW_DISKCACHE_NODE)pringTemp;
        if (__LW_DISKCACHE_IS_DIRTY(pdiskn) == 0) {                     /*  ����Ҫ��д�����ݿ�          */
            break;
        }
    }
    
__try_flush:
    if (pringTemp == pdiskcDiskCache->DISKC_pringLruHeader) {           /*  û�к��ʵĿ��ƿ�            */
        pdiskn = (PLW_DISKCACHE_NODE)pringTemp;
        if (__LW_DISKCACHE_IS_DIRTY(pdiskn)) {                          /*  ��ͷҲ����ʹ��              */
            __diskCacheFlushInvCnt(pdiskcDiskCache, 
                                   pdiskcDiskCache->DISKC_iMaxWBurstSector,
                                   LW_TRUE, LW_FALSE);                  /*  ��дһЩ����������          */
            goto    __check_again;
        }
    }

    if (__LW_DISKCACHE_IS_VALID(pdiskn)) {                              /*  �Ƿ�Ϊ��Ч����              */
        __diskCacheHashRemove(pdiskcDiskCache, pdiskn);                 /*  ����Ч����ɾ��              */
    }

    pdiskn->DISKN_ulSectorNo = ulSectorNo;
    __LW_DISKCACHE_SET_VALID(pdiskn);
    
    __diskCacheHashAdd(pdiskcDiskCache, pdiskn);
    
    return  (pdiskn);
}
/*********************************************************************************************************
** ��������: __diskCacheNodeRead
** ��������: �Ӵ����ж�ȡ������д�� CACHE ����.
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           pdiskn             CACHE ��
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
LW_STATIC INT  __diskCacheNodeRead (PLW_DISKCACHE_CB  pdiskcDiskCache, PLW_DISKCACHE_NODE  pdiskn)
{
    REGISTER INT        i;
    REGISTER INT        iNSector;
             INT        iRetVal;
             ULONG      ulStartSector = pdiskn->DISKN_ulSectorNo;
             PCHAR      pcData;
             PVOID      pvBuffer;
             
    if (__diskCacheWpSteal(pdiskcDiskCache,
                           pdiskn->DISKN_pcData,
                           pdiskn->DISKN_ulSectorNo)) {                 /*  ���Ի�ȡ��д����            */
        return  (ERROR_NONE);
    }

    iNSector = (INT)__MIN((ULONG)pdiskcDiskCache->DISKC_iMaxRBurstSector, 
                          (ULONG)((pdiskcDiskCache->DISKC_ulNCacheNode - 
                          pdiskcDiskCache->DISKC_ulDirtyCounter)));     /*  ��ö������ĸ���            */
                      
    iNSector = (INT)__MIN((ULONG)iNSector,                              /*  �����޷������Ƚ�            */
                          (ULONG)((pdiskcDiskCache->DISKC_ulEndStector + 1) -
                          pdiskn->DISKN_ulSectorNo));                   /*  ���ܳ�Խ���һ������        */
                      
    if (iNSector <= 0) {
        return  (PX_ERROR);
    }

    pvBuffer = __diskCacheWpGetBuffer(&pdiskcDiskCache->DISKC_wpWrite, LW_TRUE);
                                                                        /*  ������ȡ�������������      */
    iRetVal = __diskCacheWpRead(pdiskcDiskCache, pdiskcDiskCache->DISKC_pblkdDisk,
                                 pvBuffer, pdiskn->DISKN_ulSectorNo, iNSector);
    if (iRetVal < 0) {
        __diskCacheWpPutBuffer(&pdiskcDiskCache->DISKC_wpWrite, pvBuffer);
        return  (iRetVal);
    }
    
    __diskCacheMemcpy(pdiskn->DISKN_pcData, pvBuffer,
                      (UINT)pdiskcDiskCache->DISKC_ulBytesPerSector);   /*  ��������                    */

    _List_Ring_Del(&pdiskn->DISKN_ringLru,
                   &pdiskcDiskCache->DISKC_pringLruHeader);             /*  ����ȷ�� LRU ��λ��         */
    _List_Ring_Add_Ahead(&pdiskn->DISKN_ringLru,
                   &pdiskcDiskCache->DISKC_pringLruHeader);

    i = 1;
    pcData = (PCHAR)pvBuffer + pdiskcDiskCache->DISKC_ulBytesPerSector;

    while (i < iNSector) {
        pdiskn = __diskCacheNodeFind(pdiskcDiskCache, (ulStartSector + i));
        if (pdiskn == LW_NULL) {                                        /*  ��һ������������Ч          */
            pdiskn = __diskCacheNodeAlloc(pdiskcDiskCache,
                                          (ulStartSector + i),
                                          (iNSector - i));              /*  ���¿���һ���յĽڵ�        */
            if (pdiskn == LW_NULL) {
                break;
            }

            __diskCacheMemcpy(pdiskn->DISKN_pcData, pcData,             /*  ��������                    */
                              (UINT)pdiskcDiskCache->DISKC_ulBytesPerSector);

            _List_Ring_Del(&pdiskn->DISKN_ringLru,
                           &pdiskcDiskCache->DISKC_pringLruHeader);     /*  ����ȷ�� LRU ��λ��         */
            _List_Ring_Add_Ahead(&pdiskn->DISKN_ringLru,
                           &pdiskcDiskCache->DISKC_pringLruHeader);
        }

        i++;
        pcData += pdiskcDiskCache->DISKC_ulBytesPerSector;
    }
    
    __diskCacheWpPutBuffer(&pdiskcDiskCache->DISKC_wpWrite, pvBuffer);  /*  read ��Ҫ�ͷŻ���           */
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: __diskCacheNodeGet
** ��������: ��ȡһ������ָ������� CACHE �ڵ�
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           ulSectorNo         ������
**           iExpectation       ����Ԥ���ٵĶ������������
**           bIsRead            �Ƿ�Ϊ������
** �䡡��  : �ڵ㴦�����Ľڵ�
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
LW_STATIC PLW_DISKCACHE_NODE  __diskCacheNodeGet (PLW_DISKCACHE_CB   pdiskcDiskCache, 
                                                  ULONG              ulSectorNo,
                                                  INT                iExpectation,
                                                  BOOL               bIsRead)
{
    REGISTER PLW_DISKCACHE_NODE     pdiskn;
             BOOL                   bIsNewNode = LW_FALSE;
             INT                    iError;
    
    pdiskn = __diskCacheNodeFind(pdiskcDiskCache, ulSectorNo);
    if (pdiskn) {                                                       /*  ֱ������                    */
        goto    __data_op;
    }
    
    pdiskn = __diskCacheNodeAlloc(pdiskcDiskCache, ulSectorNo, iExpectation);
    if (pdiskn == LW_NULL) {                                            /*  �����½ڵ�ʧ��              */
        return  (LW_NULL);
    }
    
    bIsNewNode = LW_TRUE;
    
__data_op:
    if (bIsRead) {                                                      /*  ��ȡ����                    */
        if (bIsNewNode) {                                               /*  ��Ҫ�Ӵ��̶�ȡ����          */
            iError = __diskCacheNodeRead(pdiskcDiskCache, pdiskn);
            if (iError < 0) {                                           /*  ��ȡ����                    */
                __diskCacheHashRemove(pdiskcDiskCache, pdiskn);
                return  (LW_NULL);
            }
        }
    
    } else {                                                            /*  д����                      */
        if (__LW_DISKCACHE_IS_DIRTY(pdiskn) == 0) {
            __LW_DISKCACHE_SET_DIRTY(pdiskn);                           /*  ������λ��־                */
            pdiskcDiskCache->DISKC_ulDirtyCounter++;
        }
    }
    
    pdiskcDiskCache->DISKC_disknLuck = pdiskn;                          /*  �������˽ڵ�                */
    
    _List_Ring_Del(&pdiskn->DISKN_ringLru, 
                   &pdiskcDiskCache->DISKC_pringLruHeader);             /*  ����ȷ�� LRU ��λ��         */
    _List_Ring_Add_Ahead(&pdiskn->DISKN_ringLru, 
                   &pdiskcDiskCache->DISKC_pringLruHeader);
                   
    return  (pdiskn);
}
/*********************************************************************************************************
** ��������: __diskCacheNodeRemove
** ��������: ɾ��ָ����Χ�� cache �ڵ�
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           blkrange           ������Χ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
LW_STATIC VOID  __diskCacheNodeRemove (PLW_DISKCACHE_CB   pdiskcDiskCache,
                                       PLW_BLK_RANGE      blkrange)
{
    REGISTER PLW_DISKCACHE_NODE  pdiskn, pdisknPrev;
    REGISTER ULONG               ulStartSector = blkrange->BLKR_ulStartSector;
    REGISTER ULONG               ulEndSector   = blkrange->BLKR_ulEndSector;
             ULONG               ulSectorCount;
             INT                 i;

    if (ulStartSector > ulEndSector) {
        return;
    }

    ulSectorCount = __LW_DISKCACHE_SEC_COUNT(ulStartSector, ulEndSector);

    pdiskn = (PLW_DISKCACHE_NODE)_list_ring_get_prev(pdiskcDiskCache->DISKC_pringLruHeader);

    for (i = 0; i < pdiskcDiskCache->DISKC_ulNCacheNode; i++) {

        pdisknPrev = (PLW_DISKCACHE_NODE)_list_ring_get_prev(&pdiskn->DISKN_ringLru);

        if (!__LW_DISKCACHE_IS_VALID(pdiskn) ||
            (pdiskn->DISKN_ulSectorNo > ulEndSector) ||
            (pdiskn->DISKN_ulSectorNo < ulStartSector)) {
            goto    next;
        }

        if (__LW_DISKCACHE_IS_DIRTY(pdiskn) != 0) {
            __LW_DISKCACHE_CLR_DIRTY(pdiskn);
            pdiskcDiskCache->DISKC_ulDirtyCounter--;
        }

        __diskCacheHashRemove(pdiskcDiskCache, pdiskn);                 /*  �Ƴ���Ч��                  */

        if (_list_ring_get_next(&pdiskn->DISKN_ringLru) !=
            pdiskcDiskCache->DISKC_pringLruHeader) {                    /*  ���� LRU �е�λ��, ������� */

            _List_Ring_Del(&pdiskn->DISKN_ringLru,
                           &pdiskcDiskCache->DISKC_pringLruHeader);
            _List_Ring_Add_Last(&pdiskn->DISKN_ringLru,
                           &pdiskcDiskCache->DISKC_pringLruHeader);
        }

        ulSectorCount--;
        if (ulSectorCount == 0ul) {                                     /*  ���Բ����ټ����            */
            break;
        }

next:
        pdiskn = pdisknPrev;                                            /*  ���� LRU ��                 */
    }
}
/*********************************************************************************************************
** ��������: __diskCacheNodeToBuffer
** ��������: ��������Χ�ڻ������ cache ͬ���� buffer �� (������ LRU)
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           pcBuffer           ������
**           ulStartSector      ��ʼ����
**           ulSectorCount      ������������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: 1: ��� cache ������Ϊ��, ��� cache ����, ������Ҫ�鿴���ߴ�д�����Ƿ����.
*********************************************************************************************************/
LW_STATIC VOID  __diskCacheNodeToBuffer (PLW_DISKCACHE_CB   pdiskcDiskCache,
                                         PCHAR              pcBuffer,
                                         ULONG              ulStartSector,
                                         ULONG              ulSectorCount)
{
    REGISTER PLW_DISKCACHE_NODE  pdiskn;
             ULONG               i;
             size_t              stBytesPerSector;

    stBytesPerSector = (size_t)pdiskcDiskCache->DISKC_ulBytesPerSector;

    for (i = 0; i < ulSectorCount; i++) {
        pdiskn = __diskCacheNodeFind(pdiskcDiskCache, ulStartSector);
        if (pdiskn && __LW_DISKCACHE_IS_DIRTY(pdiskn)) {
            __diskCacheMemcpy(pcBuffer, pdiskn->DISKN_pcData, stBytesPerSector);
        }

        pcBuffer += stBytesPerSector;
        ulStartSector++;
    }
}
/*********************************************************************************************************
** ��������: __diskCacheNodeFromBuffer
** ��������: ʹ�����ݸ�����Ӧ�� cache (������ LRU)
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           pcBuffer           ������
**           ulStartSector      ��ʼ����
**           ulSectorCount      ������������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
LW_STATIC VOID  __diskCacheNodeFromBuffer (PLW_DISKCACHE_CB   pdiskcDiskCache,
                                           PCHAR              pcBuffer,
                                           ULONG              ulStartSector,
                                           ULONG              ulSectorCount)
{
             BOOL                bCallback = LW_FALSE;
    REGISTER PLW_DISKCACHE_NODE  pdiskn;
             ULONG               i;
             size_t              stBytesPerSector;

    stBytesPerSector = (size_t)pdiskcDiskCache->DISKC_ulBytesPerSector;

    for (i = 0; i < ulSectorCount; i++) {
        pdiskn = __diskCacheNodeFind(pdiskcDiskCache, ulStartSector);
        if (pdiskn) {
            __diskCacheMemcpy(pdiskn->DISKN_pcData, pcBuffer, stBytesPerSector);

            if (__LW_DISKCACHE_IS_DIRTY(pdiskn)) {
                __LW_DISKCACHE_CLR_DIRTY(pdiskn);                       /*  �����λ��־                */
                pdiskcDiskCache->DISKC_ulDirtyCounter--;
            }

            if (!bCallback) {
                bCallback = LW_TRUE;
                __LW_DISKCACHE_FS_CALLBACK(pdiskcDiskCache,
                                           pdiskn, ulSectorCount - i);  /*  ִ���ļ�ϵͳ�ص�            */
            }
        }

        pcBuffer += stBytesPerSector;
        ulStartSector++;
    }
}
/*********************************************************************************************************
** ��������: __diskCacheReadDirect
** ��������: ֱ�Ӷ�ȡ���̵�ָ������
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           pvBuffer           ������
**           ulStartSector      ��ʼ����
**           ulSectorCount      ������������
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
LW_STATIC INT  __diskCacheReadDirect (PLW_DISKCACHE_CB   pdiskcDiskCache,
                                      VOID              *pvBuffer,
                                      ULONG              ulStartSector,
                                      ULONG              ulSectorCount)
{
    INT     iError = ERROR_NONE;
    PCHAR   pcData;
    ULONG   ulNSector;
    size_t  stBytes;

    do {
        ulNSector = min(ulSectorCount, pdiskcDiskCache->DISKC_iMaxRBurstSector);
        stBytes   = (size_t)pdiskcDiskCache->DISKC_ulBytesPerSector * ulNSector;

        pcData = __diskCacheWpGetBuffer(&pdiskcDiskCache->DISKC_wpWrite, LW_TRUE);
                                                                        /*  ������ȡ��������            */
        iError = __diskCacheWpRead(pdiskcDiskCache, pdiskcDiskCache->DISKC_pblkdDisk,
                                   pcData, ulStartSector, ulNSector);
        if (iError < 0) {
            __diskCacheWpPutBuffer(&pdiskcDiskCache->DISKC_wpWrite, pcData);
            break;

        } else {
            __diskCacheMemcpy(pvBuffer, pcData, stBytes);               /*  �������ļ�ϵͳ              */

            __diskCacheWpPutBuffer(&pdiskcDiskCache->DISKC_wpWrite, pcData);

            __diskCacheNodeToBuffer(pdiskcDiskCache, pvBuffer,
                                    ulStartSector, ulNSector);          /*  �� cache �����Ѷ�����       */
        }

        pvBuffer = ((UINT8 *)pvBuffer) + stBytes;
        ulStartSector += ulNSector;
        ulSectorCount -= ulNSector;

    } while (ulSectorCount);

    return  (iError);
}
/*********************************************************************************************************
** ��������: __diskCacheWriteDirect
** ��������: ֱ��д����̵�ָ������
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           pvBuffer           ������
**           ulStartSector      ��ʼ����
**           ulSectorCount      ������������
**           u64FsKey           �ļ�ϵͳ�ض�����
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
LW_STATIC INT  __diskCacheWriteDirect (PLW_DISKCACHE_CB   pdiskcDiskCache,
                                       VOID              *pvBuffer,
                                       ULONG              ulStartSector,
                                       ULONG              ulSectorCount,
                                       UINT64             u64FsKey)
{
    INT     iError = ERROR_NONE;
    PCHAR   pcData;
    ULONG   ulNSector;
    size_t  stBytes;

    do {
        ulNSector = min(ulSectorCount, pdiskcDiskCache->DISKC_iMaxWBurstSector);
        stBytes   = (size_t)pdiskcDiskCache->DISKC_ulBytesPerSector * ulNSector;

        pcData = __diskCacheWpGetBuffer(&pdiskcDiskCache->DISKC_wpWrite, LW_FALSE);

        __diskCacheMemcpy(pcData, pvBuffer, stBytes);                   /*  ������д�뻺��              */

        iError = __diskCacheWpWrite(pdiskcDiskCache, pdiskcDiskCache->DISKC_pblkdDisk,
                                    pcData, ulStartSector, ulNSector);  /*  ����д����������            */
        if (iError < 0) {
            __diskCacheWpPutBuffer(&pdiskcDiskCache->DISKC_wpWrite, pcData);
            break;

        } else {
            __diskCacheNodeFromBuffer(pdiskcDiskCache, pvBuffer,
                                      ulStartSector, ulNSector);        /*  ������� cache              */
        }

        pvBuffer = ((UINT8 *)pvBuffer) + stBytes;
        ulStartSector += ulNSector;
        ulSectorCount -= ulNSector;

    } while (ulSectorCount);

    return  (iError);
}
/*********************************************************************************************************
** ��������: __diskCacheRead
** ��������: ͨ������ CACHE ��ȡ���̵�ָ������
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           pvBuffer           ������
**           ulStartSector      ��ʼ����
**           ulSectorCount      ������������
**           ulRemianCount      ���ܵ���ҪԤȡ�ĳ���
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INT  __diskCacheRead (PLW_DISKCACHE_CB   pdiskcDiskCache,
                      VOID              *pvBuffer, 
                      ULONG              ulStartSector, 
                      ULONG              ulSectorCount,
                      ULONG              ulRemianCount)
{
             INT                    i;
             INT                    iError = ERROR_NONE;
    REGISTER PLW_DISKCACHE_NODE     pdiskn;
             PCHAR                  pcData;
             INT                    iMaxBurst;

    __LW_DISKCACHE_LOCK(pdiskcDiskCache);                               /*  �������                    */

    if (__LW_DISKCACHE_NEED_DIRECT(pdiskcDiskCache, ulSectorCount)) {   /*  ֱ�Ӷ�ȡ                    */
        iError = __diskCacheReadDirect(pdiskcDiskCache, pvBuffer,
                                       ulStartSector, ulSectorCount);

    } else {
        pcData = (PCHAR)pvBuffer;

        if (ulRemianCount &&
            ulRemianCount < pdiskcDiskCache->DISKC_iMaxRBurstSector) {  /*  ʹ���Ƽ�������������        */
            iMaxBurst = pdiskcDiskCache->DISKC_iMaxRBurstSector;
            pdiskcDiskCache->DISKC_iMaxRBurstSector = (INT)ulRemianCount;
        } else {
            iMaxBurst = 0;
        }

        for (i = 0; i < ulSectorCount; i++) {
            pdiskn = __diskCacheNodeGet(pdiskcDiskCache,
                                        (ulStartSector + i),
                                        (INT)(ulSectorCount - i),
                                        LW_TRUE);
            if (pdiskn == LW_NULL) {
                iError =  PX_ERROR;
                break;
            }

            __diskCacheMemcpy(pcData, pdiskn->DISKN_pcData,
                       (size_t)pdiskcDiskCache->DISKC_ulBytesPerSector);
                                                                        /*  ��������                    */
            pcData += pdiskcDiskCache->DISKC_ulBytesPerSector;
        }

        if (iMaxBurst) {
            pdiskcDiskCache->DISKC_iMaxRBurstSector = iMaxBurst;        /*  �ָ�����                    */
        }
    }

    __LW_DISKCACHE_UNLOCK(pdiskcDiskCache);                             /*  ����                        */

    return  (iError);
}
/*********************************************************************************************************
** ��������: __diskCacheWrite
** ��������: ͨ������ CACHE д����̵�ָ������
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           pvBuffer           ������
**           ulStartSector      ��ʼ����
**           ulSectorCount      ������������
**           u64FsKey           �ļ�ϵͳ�ض�����
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INT  __diskCacheWrite (PLW_DISKCACHE_CB   pdiskcDiskCache,
                       VOID              *pvBuffer, 
                       ULONG              ulStartSector, 
                       ULONG              ulSectorCount,
                       UINT64             u64FsKey)
{
             INT                    i;
             INT                    iError = ERROR_NONE;
    REGISTER PLW_DISKCACHE_NODE     pdiskn;
             PCHAR                  pcData;
             
    __LW_DISKCACHE_LOCK(pdiskcDiskCache);                               /*  �������                    */

    if (__LW_DISKCACHE_IS_WRITETHROUGH(pdiskcDiskCache) ||
        __LW_DISKCACHE_NEED_DIRECT(pdiskcDiskCache, ulSectorCount)) {   /*  д��͸ģʽ                  */
        iError = __diskCacheWriteDirect(pdiskcDiskCache, pvBuffer,
                                        ulStartSector, ulSectorCount, u64FsKey);
        
    } else {
        pcData = (PCHAR)pvBuffer;

        for (i = 0; i < ulSectorCount; i++) {
            pdiskn = __diskCacheNodeGet(pdiskcDiskCache,
                                        (ulStartSector + i),
                                        (INT)(ulSectorCount - i),
                                        LW_FALSE);
            if (pdiskn == LW_NULL) {
                iError =  PX_ERROR;
                break;
            }

            if (__LW_DISKCACHE_IS_DIRTY(pdiskn) == 0) {
                __LW_DISKCACHE_SET_DIRTY(pdiskn);                       /*  ������λ��־                */
                pdiskcDiskCache->DISKC_ulDirtyCounter++;
            }

            __diskCacheMemcpy(pdiskn->DISKN_pcData, pcData,
                       (size_t)pdiskcDiskCache->DISKC_ulBytesPerSector);/*  д������                    */

            pdiskn->DISKN_u64FsKey = u64FsKey;                          /*  �����ļ�ϵͳ key            */

            pcData += pdiskcDiskCache->DISKC_ulBytesPerSector;
        }
    }

    __LW_DISKCACHE_UNLOCK(pdiskcDiskCache);                             /*  ����                        */

    return  (iError);
}
/*********************************************************************************************************
** ��������: __diskCacheReadMeta
** ��������: ��ȡԪ���� (DISK CACHE δ��������)
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           pvBuffer           ������
**           ulStartSector      ��ʼ����
**           ulSectorCount      ������������
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
LW_STATIC INT  __diskCacheReadMeta (PLW_DISKCACHE_CB   pdiskcDiskCache,
                                    VOID              *pvBuffer,
                                    ULONG              ulStartSector,
                                    ULONG              ulSectorCount)
{
             INT                    i;
             INT                    iError = ERROR_NONE;
    REGISTER PLW_DISKCACHE_NODE     pdiskn;
             PCHAR                  pcData = (PCHAR)pvBuffer;
             INT                    iMaxRBurstSector;

    __LW_DISKCACHE_LOCK(pdiskcDiskCache);                               /*  �������                    */

    iMaxRBurstSector = pdiskcDiskCache->DISKC_iMaxRBurstSector;
                                                                        /*  ���ݴ���������              */
    if (pdiskcDiskCache->DISKC_iMetaBurstSector) {                      /*  ����Ԫ����⧷�����          */
        pdiskcDiskCache->DISKC_iMaxRBurstSector = pdiskcDiskCache->DISKC_iMetaBurstSector;
    } else {
        pdiskcDiskCache->DISKC_iMaxRBurstSector = min(ulSectorCount, iMaxRBurstSector);
    }

    for (i = 0; i < ulSectorCount; i++) {
        pdiskn = __diskCacheNodeGet(pdiskcDiskCache,
                                    (ulStartSector + i),
                                    (INT)(ulSectorCount - i),
                                    LW_TRUE);
        if (pdiskn == LW_NULL) {
            iError =  PX_ERROR;
            break;
        }

        __diskCacheMemcpy(pcData, pdiskn->DISKN_pcData,
                   (size_t)pdiskcDiskCache->DISKC_ulBytesPerSector);    /*  ��������                    */

        pcData += pdiskcDiskCache->DISKC_ulBytesPerSector;
    }

    pdiskcDiskCache->DISKC_iMaxRBurstSector = iMaxRBurstSector;         /*  �ָ�����������              */

    __LW_DISKCACHE_UNLOCK(pdiskcDiskCache);                             /*  ����                        */

    return  (iError);
}
/*********************************************************************************************************
** ��������: __diskCacheWriteMeta
** ��������: дԪ���� (DISK CACHE δ��������)
** �䡡��  : pdiskcDiskCache    ���� CACHE ���ƿ�
**           pvBuffer           ������
**           ulStartSector      ��ʼ����
**           ulSectorCount      ������������
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
LW_STATIC INT  __diskCacheWriteMeta (PLW_DISKCACHE_CB   pdiskcDiskCache,
                                     VOID              *pvBuffer,
                                     ULONG              ulStartSector,
                                     ULONG              ulSectorCount)
{
             INT                    i;
             INT                    iError = ERROR_NONE;
    REGISTER PLW_DISKCACHE_NODE     pdiskn;
             PCHAR                  pcData = (PCHAR)pvBuffer;
             LW_BLK_RANGE           blkrange;

    __LW_DISKCACHE_LOCK(pdiskcDiskCache);                               /*  �������                    */

    iError = __diskCacheWriteDirect(pdiskcDiskCache, pvBuffer,
                                    ulStartSector, ulSectorCount, 0ull);
    if (iError < 0) {
        goto    out;
    }

    __diskCacheWpSync(&pdiskcDiskCache->DISKC_wpWrite, 0);              /*  �ȴ�д����                  */

    blkrange.BLKR_ulStartSector = ulStartSector;
    blkrange.BLKR_ulEndSector   = ulStartSector + ulSectorCount - 1;
    __LW_DISKCACHE_DISK_IOCTL(pdiskcDiskCache)(pdiskcDiskCache->DISKC_pblkdDisk, FIOSYNCMETA, &blkrange);

    for (i = 0; i < ulSectorCount; i++) {
        pdiskn = __diskCacheNodeGet(pdiskcDiskCache,
                                    (ulStartSector + i),
                                    (INT)(ulSectorCount - i),
                                    LW_FALSE);                          /*  ���� cache �����           */
        if (pdiskn == LW_NULL) {
            iError =  PX_ERROR;
            break;
        }

        if (__LW_DISKCACHE_IS_DIRTY(pdiskn) != 0) {
            __LW_DISKCACHE_CLR_DIRTY(pdiskn);
            pdiskcDiskCache->DISKC_ulDirtyCounter--;
        }

        __diskCacheMemcpy(pdiskn->DISKN_pcData, pcData,
                   (size_t)pdiskcDiskCache->DISKC_ulBytesPerSector);    /*  д������                    */

        pcData += pdiskcDiskCache->DISKC_ulBytesPerSector;
    }

out:
    __LW_DISKCACHE_UNLOCK(pdiskcDiskCache);                             /*  ����                        */

    return  (iError);
}
/*********************************************************************************************************
** ��������: __diskCacheIoctl
** ��������: ͨ������ CACHE ����һ������
** �䡡��  : pdiskcDiskCache   ���� CACHE ���ƿ�
**           iCmd              ��������
**           lArg              ���Ʋ���
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INT  __diskCacheIoctl (PLW_DISKCACHE_CB   pdiskcDiskCache, INT  iCmd, LONG  lArg)
{
    REGISTER INT               iError;
    REGISTER BOOL              bMutex;
             PLW_BLK_RANGE     pblkrange;
             PLW_BLK_METADATA  pblkMeta;

    if (iCmd == FIORDMETA) {
        pblkMeta = (PLW_BLK_METADATA)lArg;
        return (__diskCacheReadMeta(pdiskcDiskCache, pblkMeta->BLKM_pucBuf,
                                    pblkMeta->BLKM_ulStartSector, pblkMeta->BLKM_ulSectorCnt));

    } else if (iCmd == FIOWRMETA) {
        pblkMeta = (PLW_BLK_METADATA)lArg;
        return (__diskCacheWriteMeta(pdiskcDiskCache, pblkMeta->BLKM_pucBuf,
                                     pblkMeta->BLKM_ulStartSector, pblkMeta->BLKM_ulSectorCnt));
    }

    if (__LW_DISKCACHE_LOCK(pdiskcDiskCache)) {                         /*  �������                    */
        _ErrorHandle(ENXIO);
        return  (PX_ERROR);
    }

    switch (iCmd) {
    
    case FIOSYNCMETA:                                                   /*  SYNCMETA ��Ҫ��д����       */
        iError    = ERROR_NONE;
        bMutex    = LW_TRUE;
        pblkrange = (PLW_BLK_RANGE)lArg;
        if (!__LW_DISKCACHE_IS_WRITETHROUGH(pdiskcDiskCache)) {         /*  ��д��͸ģʽ                */
            __diskCacheFlushInvMeta(pdiskcDiskCache,
                                    pblkrange->BLKR_ulStartSector,
                                    pblkrange->BLKR_ulEndSector,
                                    LW_TRUE, LW_FALSE);                 /*  ��дָ����Χ������          */
        }
        __diskCacheWpSync(&pdiskcDiskCache->DISKC_wpWrite, 0);          /*  �ȴ�д����                  */
        break;

    case FIOTRIM:                                                       /*  ���̿ռ����                */
        iError    = ERROR_NONE;
        bMutex    = LW_TRUE;
        pblkrange = (PLW_BLK_RANGE)lArg;
        __diskCacheWpSync(&pdiskcDiskCache->DISKC_wpWrite, 0);          /*  �ȴ�д����                  */
        __diskCacheNodeRemove(pdiskcDiskCache, pblkrange);
        break;

    case FIOSYNC:                                                       /*  ͬ������                    */
    case FIODATASYNC:
    case FIOFLUSH:                                                      /*  ȫ����д                    */
        iError = ERROR_NONE;
        bMutex = LW_TRUE;
        if (!__LW_DISKCACHE_IS_WRITETHROUGH(pdiskcDiskCache)) {         /*  ��д��͸ģʽ                */
            __diskCacheFlushInvRange(pdiskcDiskCache,
                                     0, __LW_DISKCACHE_MAX_SECNO, LW_TRUE, LW_FALSE);
        }
        __diskCacheWpSync(&pdiskcDiskCache->DISKC_wpWrite, 0);          /*  �ȴ�д����                  */
        break;

    case FIODISKCHANGE:                                                 /*  ���̷����ı�                */
        pdiskcDiskCache->DISKC_blkdCache.BLKD_bDiskChange = LW_TRUE;
    case FIOUNMOUNT:                                                    /*  ж�ؾ�                      */
        iError = ERROR_NONE;
        bMutex = LW_FALSE;
        __diskCacheWpSync(&pdiskcDiskCache->DISKC_wpWrite, 0);          /*  �ȴ�д����                  */
        __diskCacheMemReset(pdiskcDiskCache);                           /*  ���³�ʼ�� CACHE �ڴ�       */
        break;

    case LW_BLKD_DISKCACHE_GET_OPT:
        *(INT *)lArg = pdiskcDiskCache->DISKC_iCacheOpt;
        __LW_DISKCACHE_UNLOCK(pdiskcDiskCache);                         /*  ����                        */
        return  (ERROR_NONE);
        
    case LW_BLKD_DISKCACHE_INVALID:                                     /*  ȫ��������                  */
        __diskCacheFlushInvRange(pdiskcDiskCache,
                                 0, __LW_DISKCACHE_MAX_SECNO, LW_TRUE, LW_TRUE);
        __LW_DISKCACHE_UNLOCK(pdiskcDiskCache);                         /*  ����                        */
        return  (ERROR_NONE);
        
    case LW_BLKD_DISKCACHE_RAMFLUSH:                                    /*  �����д                    */
        if (!__LW_DISKCACHE_IS_WRITETHROUGH(pdiskcDiskCache)) {         /*  ��д��͸ģʽ                */
            __diskCacheFlushInvCnt(pdiskcDiskCache,
                                   (INT)lArg, LW_TRUE, LW_FALSE);
        }
        __LW_DISKCACHE_UNLOCK(pdiskcDiskCache);                         /*  ����                        */
        return  (ERROR_NONE);
        
    case LW_BLKD_DISKCACHE_CALLBACKFUNC:                                /*  �����ļ�ϵͳ�ص�            */
        pdiskcDiskCache->DISKC_pfuncFsCallback = (VOIDFUNCPTR)lArg;
        __LW_DISKCACHE_UNLOCK(pdiskcDiskCache);                         /*  ����                        */
        return  (ERROR_NONE);
        
    case LW_BLKD_DISKCACHE_CALLBACKARG:                                 /*  �����ļ�ϵͳ�ص�����        */
        pdiskcDiskCache->DISKC_pvFsArg = (PVOID)lArg;
        __LW_DISKCACHE_UNLOCK(pdiskcDiskCache);                         /*  ����                        */
        return  (ERROR_NONE);

    default:
        iError = PX_ERROR;
        bMutex = LW_FALSE;
        break;
    }
    
    if (bMutex) {                                                       /*  ����ִ������ܲ���        */
        __LW_DISKCACHE_DISK_IOCTL(pdiskcDiskCache)(pdiskcDiskCache->DISKC_pblkdDisk, iCmd, lArg);
        __LW_DISKCACHE_UNLOCK(pdiskcDiskCache);                         /*  ����                        */
    
    } else {
        __LW_DISKCACHE_UNLOCK(pdiskcDiskCache);                         /*  ����                        */
        if (iError == ERROR_NONE) {
            __LW_DISKCACHE_DISK_IOCTL(pdiskcDiskCache)(pdiskcDiskCache->DISKC_pblkdDisk, iCmd, lArg);
        
        } else {
            iError = __LW_DISKCACHE_DISK_IOCTL(pdiskcDiskCache)(pdiskcDiskCache->DISKC_pblkdDisk, iCmd, lArg);
        }
        
        if (iCmd == FIODISKINIT) {                                      /*  ��Ҫȷ�����̵����һ������  */
            ULONG           ulNDiskSector = __LW_DISKCACHE_MAX_SECNO;
            PLW_BLK_DEV     pblkdDisk     = pdiskcDiskCache->DISKC_pblkdDisk;

            if (pblkdDisk->BLKD_ulNSector) {
                ulNDiskSector = pblkdDisk->BLKD_ulNSector;

            } else {
                pblkdDisk->BLKD_pfuncBlkIoctl(pblkdDisk,
                                              LW_BLKD_GET_SECNUM,
                                              (LONG)&ulNDiskSector);
            }
            pdiskcDiskCache->DISKC_ulEndStector = ulNDiskSector - 1;    /*  ������һ�������ı��      */
        }
    }

    return  (iError);
}
/*********************************************************************************************************
** ��������: __diskCacheReset
** ��������: ͨ������ CACHE ��λһ������(��ʼ��)
** �䡡��  : pdiskcDiskCache   ���� CACHE ���ƿ�
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INT  __diskCacheReset (PLW_DISKCACHE_CB   pdiskcDiskCache)
{
    __diskCacheIoctl(pdiskcDiskCache, FIOSYNC, 0);                      /*  CACHE ��д����              */

    return  (__LW_DISKCACHE_DISK_RESET(pdiskcDiskCache)(pdiskcDiskCache->DISKC_pblkdDisk));
}
/*********************************************************************************************************
** ��������: __diskCacheStatusChk
** ��������: ͨ������ CACHE ���һ������
** �䡡��  : pdiskcDiskCache   ���� CACHE ���ƿ�
** �䡡��  : ERROR CODE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INT  __diskCacheStatusChk (PLW_DISKCACHE_CB   pdiskcDiskCache)
{
    return  (__LW_DISKCACHE_DISK_STATUS(pdiskcDiskCache)(pdiskcDiskCache->DISKC_pblkdDisk));
}

#endif                                                                  /*  (LW_CFG_MAX_VOLUMES > 0)    */
                                                                        /*  (LW_CFG_DISKCACHE_EN > 0)   */
/*********************************************************************************************************
  END
*********************************************************************************************************/
