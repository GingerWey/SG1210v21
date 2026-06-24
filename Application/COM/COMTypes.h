//-----------------------------------------------------------------------------
/*
 File        : CommTypes.h
 Version     : V1.01
 By          : 银网科技

 Description :定义通讯公用宏和数据结构
        
 Date       : 2017.8.19
*/
//-----------------------------------------------------------------------------
#ifndef COM_TYPES_H
#define COM_TYPES_H

#include "DevTypes.h"
#include "DevFixed.h"

#include <cstdint>
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
// 全局宏
//-----------------------------------------------------------------------------
// 信息点属性
#define CIP_RW_MASK       (1 << 0)
#define CIP_RW_READONLY   (1 << 0)
#define CIP_GetRW(x)      ((x) & CIP_RW_MASK)

//-----------------------------------------------------------------------------
// 寄存器的通讯访问属性标识
//-----------------------------------------------------------------------------
#define SPF_READ          0x0001        // 可读
#define SPF_WRITE         0x0002        // 可写
#define SPF_BIT           0x0010        // 位类型数据
#define SPF_BYTE          0x0020        // 位类型数据
#define SPF_WORD          0x0040        // 双字节类型数据
#define SPF_DWORD         0x0080        // 四字节类型数据

#define SPF_CONTROL       0x0100
#define SPF_CONTROLYH     0x1100
#define SPF_CONTROLYF     0x2100

#define SPF_WordRW        (SPF_READ | SPF_WRITE | SPF_WORD)
#define SPF_WordRO        (SPF_READ | SPF_WORD )
#define SPF_WordWO        (SPF_WRITE | SPF_WORD )

#define SPF_BitRW         (SPF_READ | SPF_WRITE | SPF_BIT)
#define SPF_BitRO         (SPF_READ | SPF_WRITE | SPF_BIT)

#define SPF_DWordRO       (SPF_READ | SPF_DWORD)
//-----------------------------------------------------------------------------
// 通信寄存器的分组标识
//-----------------------------------------------------------------------------
typedef enum tagCommInfoGroupType
{
  SIG_UNKNOWN             = 0x00,       // 不确定
  SIG_DevOption           = 0x01,       // 系统 配置
  SIG_DevConfig           = 0x02,       // 系统 参数
  SIG_VDI                 = 0x03,       // 软压板
  SIG_SetPoint            = 0x04,       // 定值
  SIG_Event               = 0x05,       // 事件
  SIG_ProMeasure          = 0x08,       // 保护测量
  SIG_Measure             = 0x09,       // 远动测量
  SIG_Metering            = 0x0A,       // 电量
  SIG_BinInput            = 0x0B,       // 遥信
  SIG_Coils               = 0x0C,       // 遥控点
  SIG_Adjustment          = 0x0D,       // 遥调
  SIG_DebugInfo           = 0x0E,       //
  SIG_WLGADCChnl          = 0x0F,       // 录波模拟通道
  SIG_WLGDgtChnl          = 0x10,       // 录波数字通道
  SIG_Calibration         = 0x11,       // 校准数据
  SIG_GOOSEBinary         = 0x12        // GOOSE数据集
} TCommInfoGroupType;
//=============================================================================
// 全局数据结构
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 通信分组定义
//-----------------------------------------------------------------------------
// CommInfos
//   |-Cluster[0]        -- for Protocol:0                       
//   |   |-Groups[0] 
//   |   |   |-Item[0]
//   |   |   |-...
//   |   |   |_Item[n]
//   |   |-...
//   |   |_Groups[n] 
//   |_ ...                -- for Protocol:1 
//   |_ ...                -- for Protocol:2
//   |_ Cluster[n]         -- for Protocol:n 
//-----------------------------------------------------------------------------
// 信息点项
typedef struct tagCommInfoItem
{
  uint32_t        RegNum;               // 寄存器号
  uint16_t        InfoAddr;             // 通讯信息地址， 点号
  uint16_t        Property;             // 属性
} TCommInfoItem;
//-----------------------------------------------------------------------------
// 信息点表分组
// 规约中的分组
typedef struct tagCommInfoGroup
{
  const char*             Name;         // 组名称
  TCommInfoGroupType      Type;         // 组类型
  uint16_t                InfBegin;     // 起始INF号
  uint16_t                InfEnd;       // 结束INF号  
  uint32_t                Count;        // TCommInfoItem信息项数量
  const TCommInfoItem    *Items;        // 信息项
} TCommInfoGroup;
//-----------------------------------------------------------------------------
// 信息点集合
// 供某个规约类型使用
typedef struct tagCommInfoCluster
{
  const char*             Name;         // TCommInfoCluster名称
  uint32_t                Count;        // TCommInfoGroup信息组的数量
  const TCommInfoGroup  **Groups;       // 分组指针
} TCommInfoCluster;
//-----------------------------------------------------------------------------
// 通信信息包
// 包括一个功能类型中所有支持的规约要用到的信息集合，顺序按规约号
typedef struct tagCommInfoPackage
{
  uint32_t                 Count;        // TCommInfoCluster的数量
  const TCommInfoCluster **Clusters;     // 集合指针
} TCommInfoPackage;
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 遥控操作定义
//-----------------------------------------------------------------------------
typedef struct tagRemoteCtrlItem
{
  uint32_t       RelayIdx;               // 继电器序号 1 ~ 19
  uint32_t       DigInReg;               // 开入寄存器 用于反馈遥控结果
  uint8_t        State;                  // 开入的目标结果 STATE_TRUE/FALSE
} TRemoteCtrlItem;

typedef struct tagRemoteCtrlList
{
  uint32_t                LocalLatchReg; // 就地操作闭锁信号寄存器
  uint32_t                ServiceReg;    // 检修操作信号寄存器
  uint32_t                Count;         // Items数量
  const TRemoteCtrlItem  *Items;         // 控制项
} TRemoteCtrlList; 
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif
