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
** 文   件   名: phyPage.c
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2008 年 12 月 09 日
**
** 描        述: 物理页面管理.

** BUG
2009.03.05  __vmmPhysicalGetZone() 对连续域的定位错误.
2009.06.23  加入物理内存对齐开辟功能.
2009.11.13  假如获取碎片物理内存大小接口.
            同时允许从指定的 zone 中分配物理内存.
2011.03.02  加入 __vmmPhysicalPageSetListFlag() 函数.
2011.05.19  加入物理页面链遍历功能 __vmmPhysicalPageTraversalList().
2011.08.11  物理页面分配优先使用属相相同的分区, 这样可以避免 DMA 分区被其他分配浪费.
2013.05.30  加入物理页面引用功能.
2014.07.27  加入物理页面 CACHE 操作功能.
2016.08.19  __vmmPhysicalCreate() 支持多次调用添加物理内存.
2020.02.22  增加缺页中断内存最小限制.
2023.08.08  支持 NUMA 簇紧耦合物理内存管理.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "../SylixOS/kernel/include/k_kernel.h"
#include "../SylixOS/system/include/s_system.h"
/*********************************************************************************************************
  加入裁剪支持
*********************************************************************************************************/
#if LW_CFG_VMM_EN > 0
#include "phyPage.h"
#include "virPage.h"
#include "pageTable.h"
/*********************************************************************************************************
  地址空间冲突检查
*********************************************************************************************************/
extern BOOL     __vmmLibVirtualOverlap(addr_t  ulAddr, size_t  stSize);
/*********************************************************************************************************
  物理 zone 控制块数组
*********************************************************************************************************/
LW_VMM_ZONE                     _G_vmzonePhysical[LW_CFG_VMM_ZONE_NUM]; /*  物理区域                    */
/*********************************************************************************************************
  NUMA zone 控制块索引池
*********************************************************************************************************/
#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0) && (LW_CFG_SMP_NUMA_EN > 0)
static ULONG                    _G_ulNumaZone[LW_CFG_MAX_PROCESSORS];   /*  NUMA cluster index 控制块   */
#endif                                                                  /*  LW_CFG_SMP_NUMA_EN > 0      */
/*********************************************************************************************************
  物理内存 text data 段大小
*********************************************************************************************************/
static LW_MMU_PHYSICAL_DESC     _G_vmphydescKernel[2];                  /*  内核内存信息                */
/*********************************************************************************************************
  物理内存配额
*********************************************************************************************************/
static LW_VMM_PAGE_FAULT_LIMIT  _G_vmpflPhysical;                       /*  缺页中断物理内存限制        */
static LW_OBJECT_HANDLE         _G_ulPageFaultGuarder = LW_OBJECT_HANDLE_INVALID;
/*********************************************************************************************************
  ZONE foreach
*********************************************************************************************************/
#define LW_PHY_ZONE_FOREACH(i) \
        for (i = 0; i < LW_CFG_VMM_ZONE_NUM && _G_vmzonePhysical[i].ZONE_stSize; i++)

#define LW_PHY_ZONE_FOREACH_EQU_ATTR(i, attr) \
        LW_PHY_ZONE_FOREACH(i) \
        if (_G_vmzonePhysical[i].ZONE_uiAttr == (attr))

#define LW_PHY_ZONE_FOREACH_HAS_ATTR(i, attr) \
        LW_PHY_ZONE_FOREACH(i) \
        if ((_G_vmzonePhysical[i].ZONE_uiAttr & (attr)) == (attr))
/*********************************************************************************************************
  ZONE numa get current
*********************************************************************************************************/
#define LW_PHY_ZONE_NUMA_CUR(type) \
        ({ \
            PLW_CLASS_TCB  ptcbCur; \
            ULONG          ulZone; \
            LW_TCB_GET_CUR_SAFE(ptcbCur); \
            if ((ptcbCur->TCB_ulOption & LW_OPTION_THREAD_NUMA) && \
                (ptcbCur->TCB_iHetrccMode == LW_HETRCC_STRONG_AFFINITY)) { \
                ulZone = _G_ulNumaZone[ptcbCur->TCB_ulHetrccLock]; \
            } else { \
                ulZone = LW_CFG_VMM_ZONE_NUM; \
            } \
            (type)ulZone; \
        })
/*********************************************************************************************************
** 函数名称: __vmmPhysicalCreate
** 功能描述: 创建一个物理分页区域.
** 输　入  : pphydesc          物理区域描述表
** 输　出  : ERROR CODE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
ULONG  __vmmPhysicalCreate (LW_MMU_PHYSICAL_DESC  pphydesc[])
{
    REGISTER ULONG  ulError = ERROR_NONE;
    static   ULONG  ulZone  = 0;                                        /*  可多次追尾添加内存          */
             BOOL   bStop;
             INT    i;
             
#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0) && (LW_CFG_SMP_NUMA_EN > 0)
    if (ulZone == 0) {
        for (i = 0; i < LW_CFG_MAX_PROCESSORS; i++) {
            _G_ulNumaZone[i] = LW_CFG_VMM_ZONE_NUM;
        }
    }
#endif                                                                  /*  LW_CFG_SMP_NUMA_EN > 0      */

    for (i = 0; ; i++) {
        if (pphydesc[i].PHYD_stSize == 0) {
            break;
        }
        
        bStop = pphydesc[i].PHYD_uiType != LW_PHYSICAL_MEM_BOOTSFR;     /*  BootSFR 支持特殊映射处理    */

        _BugFormat(!ALIGNED(pphydesc[i].PHYD_ulPhyAddr, LW_CFG_VMM_PAGE_SIZE), bStop,
                   "physical zone vaddr 0x%08lx not page aligned.\r\n",
                   pphydesc[i].PHYD_ulPhyAddr);
        
        switch (pphydesc[i].PHYD_uiType) {
        
        case LW_PHYSICAL_MEM_TEXT:
            if (_G_vmphydescKernel[LW_PHYSICAL_MEM_TEXT].PHYD_stSize) {
                _G_vmphydescKernel[LW_PHYSICAL_MEM_TEXT].PHYD_stSize += pphydesc[i].PHYD_stSize;
            
            } else {
                _G_vmphydescKernel[LW_PHYSICAL_MEM_TEXT] = pphydesc[i];
            }
            break;
            
        case LW_PHYSICAL_MEM_DATA:
            if (_G_vmphydescKernel[LW_PHYSICAL_MEM_DATA].PHYD_stSize) {
                _G_vmphydescKernel[LW_PHYSICAL_MEM_DATA].PHYD_stSize += pphydesc[i].PHYD_stSize;
            
            } else {
                _G_vmphydescKernel[LW_PHYSICAL_MEM_DATA] = pphydesc[i];
            }
            break;
        
        case LW_PHYSICAL_MEM_DMA:
            _BugHandle((pphydesc[i].PHYD_ulPhyAddr == (addr_t)LW_NULL), LW_TRUE,
                       "physical DMA zone can not use NULL address, you can move offset page.\r\n");
                                                                        /*  目前不支持 NULL 起始地址    */
            if (ulZone < LW_CFG_VMM_ZONE_NUM) {
                _BugFormat(__vmmLibVirtualOverlap(pphydesc[i].PHYD_ulPhyAddr, 
                                                  pphydesc[i].PHYD_stSize), LW_TRUE,
                           "physical zone paddr 0x%08lx size: 0x%08zx overlap with virtual space.\r\n",
                           pphydesc[i].PHYD_ulPhyAddr, pphydesc[i].PHYD_stSize);
            
                ulError = __pageZoneCreate(&_G_vmzonePhysical[ulZone], 
                                           pphydesc[i].PHYD_ulPhyAddr, 
                                           pphydesc[i].PHYD_stSize,
                                           LW_ZONE_ATTR_DMA, 
                                           __VMM_PAGE_TYPE_PHYSICAL);   /*  初始化物理 zone             */
                if (ulError) {
                    _DebugHandle(__ERRORMESSAGE_LEVEL, "kernel low memory.\r\n");
                    _ErrorHandle(ulError);
                    return  (ulError);
                }
                ulZone++;
            }
            break;
            
        case LW_PHYSICAL_MEM_APP:
            _BugHandle((pphydesc[i].PHYD_ulPhyAddr == (addr_t)LW_NULL), LW_TRUE,
                       "physical APP zone can not use NULL address, you can move offset page.\r\n");
                                                                        /*  目前不支持 NULL 起始地址    */
            if (ulZone < LW_CFG_VMM_ZONE_NUM) {
                ulError = __pageZoneCreate(&_G_vmzonePhysical[ulZone], 
                                           pphydesc[i].PHYD_ulPhyAddr, 
                                           pphydesc[i].PHYD_stSize,
                                           LW_ZONE_ATTR_NONE, 
                                           __VMM_PAGE_TYPE_PHYSICAL);   /*  初始化物理 zone             */
                if (ulError) {
                    _DebugHandle(__ERRORMESSAGE_LEVEL, "kernel low memory.\r\n");
                    _ErrorHandle(ulError);
                    return  (ulError);
                }

#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0) && (LW_CFG_SMP_NUMA_EN > 0)
                if (pphydesc[i].PHYD_uiNumaClu & LW_PHYSICAL_MEM_NUMA) {
                    UINT32  uiNumaClu = pphydesc[i].PHYD_uiNumaClu & ~LW_PHYSICAL_MEM_NUMA;
                    if (uiNumaClu < LW_CFG_MAX_PROCESSORS) {
                        _G_ulNumaZone[uiNumaClu] = ulZone;              /*  记录 NUMA 亲和 zone         */
                    }
                }
#endif                                                                  /*  LW_CFG_SMP_NUMA_EN > 0      */
                ulZone++;
            }
            break;
            
        default:
            break;
        }
    }
    
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageAlloc
** 功能描述: 分配指定的连续物理页面
** 输　入  : ulPageNum     需要分配的物理页面个数
**           uiAttr        需要满足的物理页面属性
**           pulZoneIndex  物理 zone 下标
** 输　出  : 页面控制块
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
PLW_VMM_PAGE  __vmmPhysicalPageAlloc (ULONG  ulPageNum, UINT  uiAttr, ULONG  *pulZoneIndex)
{
             INT            i;
    REGISTER PLW_VMM_ZONE   pvmzone;
    REGISTER PLW_VMM_PAGE   pvmpage = LW_NULL;
    
#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0) && (LW_CFG_SMP_NUMA_EN > 0)
    if (uiAttr == LW_ZONE_ATTR_NONE) {
        i = LW_PHY_ZONE_NUMA_CUR(INT);
        if (i < LW_CFG_VMM_ZONE_NUM) {
            pvmzone = &_G_vmzonePhysical[i];
            if (pvmzone->ZONE_ulFreePage >= ulPageNum) {
                pvmpage = __pageAllocate(pvmzone, ulPageNum, __VMM_PAGE_TYPE_PHYSICAL);
                if (pvmpage) {
                    *pulZoneIndex = (ULONG)i;
                    return  (pvmpage);
                }
            }
        }
    }
#endif                                                                  /*  LW_CFG_SMP_NUMA_EN > 0      */

    LW_PHY_ZONE_FOREACH_EQU_ATTR (i, uiAttr) {                          /*  最佳属性匹配                */
        pvmzone = &_G_vmzonePhysical[i];
        if (pvmzone->ZONE_ulFreePage >= ulPageNum) {
            pvmpage = __pageAllocate(pvmzone, ulPageNum, __VMM_PAGE_TYPE_PHYSICAL);
            if (pvmpage) {
                *pulZoneIndex = (ULONG)i;
                return  (pvmpage);
            }
        }
    }

    LW_PHY_ZONE_FOREACH_HAS_ATTR (i, uiAttr) {                          /*  包含属性匹配                */
        pvmzone = &_G_vmzonePhysical[i];
        if (pvmzone->ZONE_ulFreePage >= ulPageNum) {
            pvmpage = __pageAllocate(pvmzone, ulPageNum, __VMM_PAGE_TYPE_PHYSICAL);
            if (pvmpage) {
                *pulZoneIndex = (ULONG)i;
                return  (pvmpage);
            }
        }
    }
    
    return  (LW_NULL);
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageAllocAlign
** 功能描述: 分配指定的连续物理页面 (可指定对齐关系)
** 输　入  : ulPageNum     需要分配的物理页面个数
**           stAlign       内存对齐关系
**           uiAttr        需要满足的物理页面属性
**           pulZoneIndex  物理 zone 下标
** 输　出  : 页面控制块
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
PLW_VMM_PAGE  __vmmPhysicalPageAllocAlign (ULONG   ulPageNum, 
                                           size_t  stAlign, 
                                           UINT    uiAttr, 
                                           ULONG  *pulZoneIndex)
{
             INT            i;
    REGISTER PLW_VMM_ZONE   pvmzone;
    REGISTER PLW_VMM_PAGE   pvmpage = LW_NULL;
    
#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0) && (LW_CFG_SMP_NUMA_EN > 0)
    if (uiAttr == LW_ZONE_ATTR_NONE) {
        i = LW_PHY_ZONE_NUMA_CUR(INT);
        if (i < LW_CFG_VMM_ZONE_NUM) {
            pvmzone = &_G_vmzonePhysical[i];
            if (pvmzone->ZONE_ulFreePage >= ulPageNum) {
                pvmpage = __pageAllocateAlign(pvmzone, ulPageNum, stAlign, __VMM_PAGE_TYPE_PHYSICAL);
                if (pvmpage) {
                    *pulZoneIndex = (ULONG)i;
                    return  (pvmpage);
                }
            }
        }
    }
#endif                                                                  /*  LW_CFG_SMP_NUMA_EN > 0      */

    LW_PHY_ZONE_FOREACH_EQU_ATTR (i, uiAttr) {                          /*  最佳属性匹配                */
        pvmzone = &_G_vmzonePhysical[i];
        if (pvmzone->ZONE_ulFreePage >= ulPageNum) {
            pvmpage = __pageAllocateAlign(pvmzone, ulPageNum, stAlign, __VMM_PAGE_TYPE_PHYSICAL);
            if (pvmpage) {
                *pulZoneIndex = (ULONG)i;
                return  (pvmpage);
            }
        }
    }
    
    LW_PHY_ZONE_FOREACH_HAS_ATTR (i, uiAttr) {                          /*  包含属性匹配                */
        pvmzone = &_G_vmzonePhysical[i];
        if (pvmzone->ZONE_ulFreePage >= ulPageNum) {
            pvmpage = __pageAllocateAlign(pvmzone, ulPageNum, stAlign, __VMM_PAGE_TYPE_PHYSICAL);
            if (pvmpage) {
                *pulZoneIndex = (ULONG)i;
                return  (pvmpage);
            }
        }
    }
    
    return  (LW_NULL);
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageAllocZone
** 功能描述: 分配指定的连续物理页面 (指定物理区域)
** 输　入  : ulZoneIndex   物理区域
**           ulPageNum     需要分配的物理页面个数
**           uiAttr        需要满足的页面属性
** 输　出  : 页面控制块
** 全局变量:
** 调用模块:
*********************************************************************************************************/
PLW_VMM_PAGE  __vmmPhysicalPageAllocZone (ULONG  ulZoneIndex, ULONG  ulPageNum, UINT  uiAttr)
{
    REGISTER PLW_VMM_ZONE   pvmzone;
    REGISTER PLW_VMM_PAGE   pvmpage = LW_NULL;

    if (ulZoneIndex >= LW_CFG_VMM_ZONE_NUM) {
        return  (LW_NULL);
    }

    pvmzone = &_G_vmzonePhysical[ulZoneIndex];
    if (!pvmzone->ZONE_stSize) {                                        /*  无效 zone                   */
        return  (LW_NULL);
    }

    if ((pvmzone->ZONE_uiAttr & uiAttr) == uiAttr) {
        if (pvmzone->ZONE_ulFreePage >= ulPageNum) {
            pvmpage = __pageAllocate(pvmzone, ulPageNum, __VMM_PAGE_TYPE_PHYSICAL);
            if (pvmpage) {
                return  (pvmpage);
            }
        }
    }

    return  (LW_NULL);
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageClone
** 功能描述: 克隆一个物理页面
** 输　入  : pvmpage       页面控制块
** 输　出  : 新的物理页面
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
PLW_VMM_PAGE  __vmmPhysicalPageClone (PLW_VMM_PAGE  pvmpage)
{
    PLW_VMM_PAGE    pvmpageNew;
    addr_t          ulSwitchAddr = __vmmVirtualSwitch();
    ULONG           ulZoneIndex;
    ULONG           ulError;
    
    if ((pvmpage->PAGE_ulCount != 1) ||
        (pvmpage->PAGE_iPageType != __VMM_PAGE_TYPE_PHYSICAL) ||
        (pvmpage->PAGE_ulMapPageAddr == PAGE_MAP_ADDR_INV)) {           /*  必须是有映射关系的单页面    */
        _ErrorHandle(ERROR_VMM_PHYSICAL_PAGE);
        return  (LW_NULL);
    }
    
    pvmpageNew = __vmmPhysicalPageAlloc(1, LW_ZONE_ATTR_NONE, &ulZoneIndex);
    if (pvmpageNew == LW_NULL) {
        _ErrorHandle(ERROR_VMM_LOW_PHYSICAL_PAGE);
        return  (LW_NULL);
    }
    
    ulError = __vmmLibPageMap(pvmpageNew->PAGE_ulPageAddr,              /*  使用 CACHE 操作             */
                              ulSwitchAddr, 1,                          /*  缓冲区虚拟地址              */
                              LW_VMM_FLAG_RDWR);                        /*  映射指定的虚拟地址          */
    if (ulError) {
        __vmmPhysicalPageFree(pvmpageNew);
        return  (LW_NULL);
    }
    
    KN_COPY_PAGE((PVOID)ulSwitchAddr, 
                 (PVOID)pvmpage->PAGE_ulMapPageAddr);                   /*  拷贝页面内容                */
               
    if (API_CacheAliasProb()) {                                         /*  cache 别名可能              */
        API_CacheClearPage(DATA_CACHE, 
                           (PVOID)ulSwitchAddr,
                           (PVOID)pvmpageNew->PAGE_ulPageAddr,
                           LW_CFG_VMM_PAGE_SIZE);                       /*  将数据写入内存并不再命中    */
    }
    
    __vmmLibSetFlag(ulSwitchAddr, 1, LW_VMM_FLAG_FAIL, LW_TRUE);        /*  VIRTUAL_SWITCH 不允许访问   */
    
    return  (pvmpageNew);
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageRef
** 功能描述: 引用物理页面
** 输　入  : pvmpage       页面控制块
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
PLW_VMM_PAGE  __vmmPhysicalPageRef (PLW_VMM_PAGE  pvmpage)
{
    PLW_VMM_PAGE    pvmpageFake = (PLW_VMM_PAGE)__KHEAP_ALLOC(sizeof(LW_VMM_PAGE));
    PLW_VMM_PAGE    pvmpageReal;
    
    if (pvmpageFake == LW_NULL) {
        _DebugHandle(__ERRORMESSAGE_LEVEL, "kernel low memory.\r\n");
        _ErrorHandle(ERROR_KERNEL_LOW_MEMORY);                          /*  缺少内核内存                */
        return  (LW_NULL);
    }
    
    *pvmpageFake = *pvmpage;
    
    if (LW_VMM_PAGE_IS_FAKE(pvmpage)) {
        pvmpageReal = LW_VMM_PAGE_GET_REAL(pvmpage);
    
    } else {
        pvmpageReal = pvmpage;
    }
    
    pvmpageReal->PAGE_ulRef++;                                          /*  真实页面引用++              */
    
    _INIT_LIST_LINE_HEAD(&pvmpageFake->PAGE_lineFreeHash);
    _INIT_LIST_LINE_HEAD(&pvmpageFake->PAGE_lineManage);
    
    pvmpageFake->PAGE_ulRef         = 0ul;                              /*  fake 页面, 引用计数无效     */
    pvmpageFake->PAGE_pvmpageReal   = pvmpageReal;
    pvmpageFake->PAGE_ulMapPageAddr = PAGE_MAP_ADDR_INV;
    
    return  (pvmpageFake);
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageFree
** 功能描述: 回收指定的连续物理页面
** 输　入  : pvmpage       页面控制块
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  __vmmPhysicalPageFree (PLW_VMM_PAGE  pvmpage)
{
    REGISTER PLW_VMM_ZONE   pvmzone = pvmpage->PAGE_pvmzoneOwner;
             PLW_VMM_PAGE   pvmpageReal;
    
    if (LW_VMM_PAGE_IS_FAKE(pvmpage)) {
        pvmpageReal = LW_VMM_PAGE_GET_REAL(pvmpage);
        __KHEAP_FREE(pvmpage);                                          /*  释放 fake 控制块            */
    
    } else {
        pvmpageReal = pvmpage;
        pvmpageReal->PAGE_ulMapPageAddr = PAGE_MAP_ADDR_INV;            /*  对应的地址不再有效          */
    }
    
    pvmpageReal->PAGE_ulRef--;
    
    if (pvmpageReal->PAGE_ulRef == 0) {
        __pageFree(pvmzone, pvmpageReal);                               /*  没有引用则直接释放          */
    }
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageFreeAll
** 功能描述: 回收指定虚拟空间的所有物理页面
** 输　入  : pvmpageVirtual        虚拟空间
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
** 注  意  : 这里没有为每一个物理页面调用 __pageUnlink 因为调用此函数后, 虚拟空间即将被释放.
*********************************************************************************************************/
VOID  __vmmPhysicalPageFreeAll (PLW_VMM_PAGE  pvmpageVirtual)
{
    __pageTraversalLink(pvmpageVirtual, __vmmPhysicalPageFree, 
                        LW_NULL, LW_NULL, LW_NULL, LW_NULL, LW_NULL, LW_NULL);
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageSetFlag
** 功能描述: 设置物理页面的 flag 标志
** 输　入  : pvmpage       页面控制块
**           ulFlag        flag 标志
**           bFlushTlb     清除 TLB
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  __vmmPhysicalPageSetFlag (PLW_VMM_PAGE  pvmpage, ULONG  ulFlag, BOOL  bFlushTlb)
{
    if (pvmpage->PAGE_ulMapPageAddr != PAGE_MAP_ADDR_INV) {
        __vmmLibSetFlag(pvmpage->PAGE_ulMapPageAddr, pvmpage->PAGE_ulCount, ulFlag, bFlushTlb);
        pvmpage->PAGE_ulFlags = ulFlag;
    }
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageSetFlagAll
** 功能描述: 设置虚拟空间内所有物理页面的 flag 标志
** 输　入  : pvmpageVirtual 虚拟空间
**           ulFlag         flag 标志
**           bFlushTlb      清除 TLB
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  __vmmPhysicalPageSetFlagAll (PLW_VMM_PAGE  pvmpageVirtual, ULONG  ulFlag)
{
#if LW_CFG_VMM_L4_HYPERVISOR_EN == 0
    PLW_MMU_CONTEXT    pmmuctx = __vmmGetCurCtx();
#endif                                                                  /* !LW_CFG_VMM_L4_HYPERVISOR_EN */

    __pageTraversalLink(pvmpageVirtual, __vmmPhysicalPageSetFlag, (PVOID)ulFlag,
                        (PVOID)LW_FALSE, LW_NULL, LW_NULL, LW_NULL, LW_NULL);
                        
#if LW_CFG_VMM_L4_HYPERVISOR_EN == 0
    __vmmLibFlushTlb(pmmuctx, pvmpageVirtual->PAGE_ulPageAddr, pvmpageVirtual->PAGE_ulCount);
#endif                                                                  /* !LW_CFG_VMM_L4_HYPERVISOR_EN */
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageFlush
** 功能描述: 设置物理页面所有 CACHE 回写
** 输　入  : pvmpage       页面控制块
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
#if LW_CFG_CACHE_EN > 0

VOID  __vmmPhysicalPageFlush (PLW_VMM_PAGE  pvmpage)
{
    if (pvmpage->PAGE_ulMapPageAddr != PAGE_MAP_ADDR_INV) {
        API_CacheFlushPage(DATA_CACHE, 
                           (PVOID)pvmpage->PAGE_ulMapPageAddr, 
                           (PVOID)pvmpage->PAGE_ulPageAddr,
                           (size_t)(pvmpage->PAGE_ulCount << LW_CFG_VMM_PAGE_SHIFT));
    }
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageFlushAll
** 功能描述: 使虚拟空间内所有物理页面回写
** 输　入  : pvmpageVirtual 虚拟空间
**           ulFlag         flag 标志
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  __vmmPhysicalPageFlushAll (PLW_VMM_PAGE  pvmpageVirtual)
{
    __pageTraversalLink(pvmpageVirtual, __vmmPhysicalPageFlush,
                        LW_NULL, LW_NULL, LW_NULL, LW_NULL, LW_NULL, LW_NULL);
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageInvalidate
** 功能描述: 设置物理页面所有 CACHE 无效
** 输　入  : pvmpage       页面控制块
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  __vmmPhysicalPageInvalidate (PLW_VMM_PAGE  pvmpage)
{
    if (pvmpage->PAGE_ulMapPageAddr != PAGE_MAP_ADDR_INV) {
        API_CacheInvalidatePage(DATA_CACHE, 
                                (PVOID)pvmpage->PAGE_ulMapPageAddr, 
                                (PVOID)pvmpage->PAGE_ulPageAddr,
                                (size_t)(pvmpage->PAGE_ulCount << LW_CFG_VMM_PAGE_SHIFT));
    }
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageInvalidateAll
** 功能描述: 使虚拟空间内所有物理页面无效
** 输　入  : pvmpageVirtual 虚拟空间
**           ulFlag         flag 标志
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  __vmmPhysicalPageInvalidateAll (PLW_VMM_PAGE  pvmpageVirtual)
{
    __pageTraversalLink(pvmpageVirtual, __vmmPhysicalPageInvalidate,
                        LW_NULL, LW_NULL, LW_NULL, LW_NULL, LW_NULL, LW_NULL);
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageClear
** 功能描述: 设置物理页面所有 CACHE 回写并无效
** 输　入  : pvmpage       页面控制块
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  __vmmPhysicalPageClear (PLW_VMM_PAGE  pvmpage)
{
    if (pvmpage->PAGE_ulMapPageAddr != PAGE_MAP_ADDR_INV) {
        API_CacheClearPage(DATA_CACHE, 
                           (PVOID)pvmpage->PAGE_ulMapPageAddr, 
                           (PVOID)pvmpage->PAGE_ulPageAddr,
                           (size_t)(pvmpage->PAGE_ulCount << LW_CFG_VMM_PAGE_SHIFT));
    }
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageClearAll
** 功能描述: 使虚拟空间内所有物理页面回写并无效
** 输　入  : pvmpageVirtual 虚拟空间
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  __vmmPhysicalPageClearAll (PLW_VMM_PAGE  pvmpageVirtual)
{
    __pageTraversalLink(pvmpageVirtual, __vmmPhysicalPageClear,
                        LW_NULL, LW_NULL, LW_NULL, LW_NULL, LW_NULL, LW_NULL);
}

#endif                                                                  /*  LW_CFG_CACHE_EN > 0         */
/*********************************************************************************************************
** 函数名称: __vmmPhysicalGetZone
** 功能描述: 根据物理地址, 确定有效的 zone
** 输　入  : ulAddr        物理地址
** 输　出  : ulZoneIndex   物理 zone 下标
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
ULONG  __vmmPhysicalGetZone (addr_t  ulAddr)
{
             INT            i;
    REGISTER PLW_VMM_ZONE   pvmzone;

    LW_PHY_ZONE_FOREACH (i) {
        pvmzone = &_G_vmzonePhysical[i];
        if ((ulAddr >= pvmzone->ZONE_ulAddr) &&
            (ulAddr <= pvmzone->ZONE_ulAddr + pvmzone->ZONE_stSize - 1)) {
            return  ((ULONG)i);
        }
    }
    
    return  (LW_CFG_VMM_ZONE_NUM);
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageGetMinContinue
** 功能描述: 获得物理页面内, 最小连续分页的个数
** 输　入  : pulZoneIndex      获得最小页面的 zone 下标
**           uiAttr            需要满足的物理页面属性
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
ULONG  __vmmPhysicalPageGetMinContinue (ULONG  *pulZoneIndex, UINT  uiAttr)
{
             INT            i;
    REGISTER PLW_VMM_ZONE   pvmzone;
             ULONG          ulMin = __ARCH_ULONG_MAX;
             ULONG          ulTemp;
             
#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0) && (LW_CFG_SMP_NUMA_EN > 0)
    if (uiAttr == LW_ZONE_ATTR_NONE) {
        i = LW_PHY_ZONE_NUMA_CUR(INT);
        if (i < LW_CFG_VMM_ZONE_NUM) {
            pvmzone = &_G_vmzonePhysical[i];
            ulTemp  = __pageGetMinContinue(pvmzone);
            if (ulTemp > 0) {
                *pulZoneIndex = (ULONG)i;
                return  (ulTemp);
            }
        }
    }
#endif                                                                  /*  LW_CFG_SMP_NUMA_EN > 0      */

    LW_PHY_ZONE_FOREACH_EQU_ATTR (i, uiAttr) {                          /*  最佳属性匹配                */
        pvmzone = &_G_vmzonePhysical[i];
        ulTemp = __pageGetMinContinue(pvmzone);
        if ((ulTemp > 0) && (ulMin > ulTemp)) {
            ulMin = ulTemp;
            *pulZoneIndex = (ULONG)i;
        }
    }
    
    if (ulMin != __ARCH_ULONG_MAX) {
        return  (ulMin);
    }
    
    LW_PHY_ZONE_FOREACH_HAS_ATTR (i, uiAttr) {                          /*  包含属性匹配                */
        pvmzone = &_G_vmzonePhysical[i];
        ulTemp = __pageGetMinContinue(pvmzone);
        if ((ulTemp > 0) && (ulMin > ulTemp)) {
            ulMin = ulTemp;
            *pulZoneIndex = (ULONG)i;
        }
    }
    
    return  (ulMin);
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageFaultLimit
** 功能描述: 设置缺页中断物理内存限制.
** 输　入  : pvpflNew          新的设置
**           pvpflOld          之前的设置
** 输　出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  __vmmPhysicalPageFaultLimit (PLW_VMM_PAGE_FAULT_LIMIT  pvpflNew,
                                  PLW_VMM_PAGE_FAULT_LIMIT  pvpflOld)
{
    if (pvpflOld) {
        *pvpflOld = _G_vmpflPhysical;
    }

    if (pvpflNew) {
        if ((pvpflNew->VPFL_ulRootWarnPages < pvpflNew->VPFL_ulRootMinPages) ||
            (pvpflNew->VPFL_ulUserWarnPages < pvpflNew->VPFL_ulUserMinPages)) {
            _ErrorHandle(EINVAL);
            return  (PX_ERROR);

        } else {
            _G_vmpflPhysical = *pvpflNew;
        }
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageFaultCheck
** 功能描述: 检查缺页中断物理内存限制.
** 输　入  : ulPageNum         需要分配的物理页面个数
**           ptcbCur           当前任务控制块
**           pulWarn           需要警告时, 通知的守卫线程.
** 输　出  : TRUE: 允许分配, FALSE 拒绝分配
** 全局变量:
** 调用模块:
*********************************************************************************************************/
BOOL  __vmmPhysicalPageFaultCheck (ULONG  ulPageNum, PLW_CLASS_TCB  ptcbCur, LW_OBJECT_HANDLE  *pulWarn)
{
#define __VMM_PAGE_FAULT_CHECK(warnpages, minpages) \
        ulFreePage = 0; \
        LW_PHY_ZONE_FOREACH (i) { \
            pvmzone = &_G_vmzonePhysical[i]; \
            ulFreePage += pvmzone->ZONE_ulFreePage; \
            if (ulFreePage > (warnpages)) { \
                return  (LW_TRUE); \
            } \
        } \
        if (ulFreePage > (minpages)) { \
            if (_G_ulPageFaultGuarder) { \
                *pulWarn = _G_ulPageFaultGuarder; \
                _G_ulPageFaultGuarder = LW_OBJECT_HANDLE_INVALID; \
            } \
            return  (LW_TRUE); \
        }

             INT            i;
    REGISTER PLW_VMM_ZONE   pvmzone;
    REGISTER ULONG          ulFreePage;

    if (ptcbCur->TCB_euid == 0) {
        if (_G_vmpflPhysical.VPFL_ulRootWarnPages == 0) {
            return  (LW_TRUE);
        } else {
            __VMM_PAGE_FAULT_CHECK(_G_vmpflPhysical.VPFL_ulRootWarnPages,
                                   _G_vmpflPhysical.VPFL_ulRootMinPages);
        }

    } else {
        if (_G_vmpflPhysical.VPFL_ulUserWarnPages == 0) {
            return  (LW_TRUE);
        } else {
            __VMM_PAGE_FAULT_CHECK(_G_vmpflPhysical.VPFL_ulUserWarnPages,
                                   _G_vmpflPhysical.VPFL_ulUserMinPages);
        }
    }

    return  (LW_FALSE);
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageFaultClear
** 功能描述: 缺页中断警戒管理任务删除
** 输　入  : ulId      被删除的任务
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_THREAD_DEL_EN > 0

VOID  __vmmPhysicalPageFaultClear (LW_OBJECT_HANDLE  ulId)
{
    if (_G_ulPageFaultGuarder == ulId) {
        _G_ulPageFaultGuarder =  LW_OBJECT_HANDLE_INVALID;
        KN_SMP_WMB();
    }
}

#endif                                                                  /*  LW_CFG_THREAD_DEL_EN > 0    */
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageFaultWarn
** 功能描述: 物理内存缺少告警
** 输　入  : ulGuarder      警卫线程 ID
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  __vmmPhysicalPageFaultWarn (LW_OBJECT_HANDLE  ulGuarder)
{
#if LW_CFG_SIGNAL_EN > 0
    struct sigevent  sigeventWarn;

    sigeventWarn.sigev_signo  = SIGLOWMEM;
    sigeventWarn.sigev_notify = SIGEV_SIGNAL;
    sigeventWarn.sigev_value.sival_ptr = LW_NULL;

    _doSigEvent(ulGuarder, &sigeventWarn, SI_KILL);
#endif                                                                  /*  LW_CFG_SIGNAL_EN > 0        */
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalPageFaultGuarder
** 功能描述: 设置缺页中断警卫线程
** 输　入  : ulGuarder      新的警卫线程 ID (单次生效)
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  __vmmPhysicalPageFaultGuarder (LW_OBJECT_HANDLE  ulGuarder)
{
    _G_ulPageFaultGuarder = ulGuarder;
    KN_SMP_WMB();

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __vmmPhysicalGetKernelDesc
** 功能描述: 获得物理内存内核 TEXT DATA 段描述
** 输　入  : pphydescText      内核 TEXT 段描述
**           pphydescData      内核 DATA 段描述
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  __vmmPhysicalGetKernelDesc (PLW_MMU_PHYSICAL_DESC  pphydescText, 
                                  PLW_MMU_PHYSICAL_DESC  pphydescData)
{
    if (pphydescText) {
        *pphydescText = _G_vmphydescKernel[LW_PHYSICAL_MEM_TEXT];
    }
    
    if (pphydescData) {
        *pphydescData = _G_vmphydescKernel[LW_PHYSICAL_MEM_DATA];
    }
}

#endif                                                                  /*  LW_CFG_VMM_EN > 0           */
/*********************************************************************************************************
  END
*********************************************************************************************************/
