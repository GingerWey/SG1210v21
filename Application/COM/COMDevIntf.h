//-----------------------------------------------------------------------------
/*
 File        : COMDevIntf.h
 Version     : V1.01
 By          : 卫荣平

 Description :定义 通讯协议与系统之间的接口对象

 Date       : 2017.8.19
*/
//-----------------------------------------------------------------------------
#ifndef COM_DevIntf_H
#define COM_DevIntf_H

#include "COMTypes.h"
#include "DevTypes.h"

#include "DevEvtMgr.h"
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
// 全局宏
//-----------------------------------------------------------------------------
// 接口操作令牌
#define CDI_TOKEN      0x35ACAC35
//=============================================================================
// 全局数据结构
//-----------------------------------------------------------------------------
// 数值类型
typedef enum tagCOMValueType
{
  cvtDWord    = 0,           // 32bits长整数
  cvtWord,                   // 16bits整数
  cvtByte,                   //  8bits短整数
  cvtFloat,                  // 32bits单精度浮点
  cvtBool                    // boolean逻辑状态
} TCOMValueType;
//-----------------------------------------------------------------------------
typedef struct tagCOMValue
{

  TCOMValueType type;        // 值的数据类型

  union
    {
    int32_t     i32Value;    // 32bits长整数
    uint32_t    u32Value;    // 32bits长整数无符号
    int16_t     i16Value;    // 16bits整数
    uint16_t    u16Value;    // 16bits整数无符号
    int8_t      i8Value;     //  8bits短整数
    uint8_t     u8Value;     //  8bits短整数无符号
    float       f32Value;    // 32bits单精度浮点
    bool        bValue;      // boolean逻辑状态
    } Data;

  // 数据点的属性，读取时有效
   int32_t      Scale;       // 缩放倍数 ( >1:放大 <-1:缩小 )
   float        Max;         // 最大值  实际值

} TCOMValue;
//-----------------------------------------------------------------------------
// 用于规约读取事件的数据结构
typedef struct tagCOMEventItem
{
  TEventLogSummary Summary;              // 事件描述

  uint16_t         InfoAddr;             // 事件源寄存器的INF号

  uint16_t         NbrOfData;            // 现场数据个数
  float            Data[NUM_EVENT_DATA]; // 现场数据值
} TCOMEventItem;
//-----------------------------------------------------------------------------
#if WAVELOGGER_EN > 0
// 用于规约读取录波摘要的数据结构
typedef struct tagCOMWaveLogItem
{
  TEventLogSummary Summary;              // 事件描述

  uint16_t         InfoAddr;             // 事件源寄存器的INF号

  uint16_t         NbrOfSamples;         // 录波通道的采样点数量
  uint16_t         PosOfTrigger;         // 零时刻: 触发时刻的位置，采样点序号

  uint16_t         NbrOfMeasChannels;    // 测量道通数量
  uint16_t         NbrOfDIChannels;      // 开入道通数量
  uint16_t         NbrOfDOChannels;      // 开出道通数量

  uint32_t         RegsOfMeas[NUM_ADC_CHANNELS]; // 各测量通道对应的寄存器
  uint32_t         RegsOfDI[NUM_DigInputs];      // 各开入通道对应的寄存器
  uint32_t         RegsOfDO[NUM_Relays];         // 各开出通道对应的寄存器
} TCOMWaveLogItem;
#endif
//-----------------------------------------------------------------------------
// 获取装置状态
typedef enum tagCOMDevState
{
  cdsDevNormal = 1,                     // 装置在正常工作模式
  cdsHWNormal,                          // 装置硬件正常
  cdsCFGNormal,                         // 装置参数正常
  cdsLocal,                             // 远方就地状态
  cdsService                            // 检修状态
} TCOMDevState;
//=============================================================================
// 全局对象
//-----------------------------------------------------------------------------
// TCommDevInterface
//  通讯协议与系统之间的接口
//-----------------------------------------------------------------------------
class TCommDevInterface
{

  private:
    // 遥控选择的起始时刻
    uint32_t                m_uRemoteCtrlStart;
    // 遥控选择的寄存器
    uint32_t                m_uRemoteCtrlReg;

    // 远程维护的起始时刻
    uint32_t                m_uRemoteServiceStart;

  protected:
    // 通讯点表
    const TCommInfoCluster *m_pInfoCluster;

    // 事件过滤器
    uint32_t                m_EventFilter;

    // 事件缓存
    TCOMEventItem           m_Event;
#if WAVELOGGER_EN > 0
    // 录波摘要缓存
    TCOMWaveLogItem         m_WaveLog;
#endif

  public:
    TCommDevInterface(void)
      {
      m_pInfoCluster = nullptr;
      m_EventFilter  = 0;

      m_uRemoteCtrlStart = 0;
      m_uRemoteCtrlReg   = 0;
      m_uRemoteServiceStart = 0;

      InitIntf();
      }

    // 给规约配置通讯点表
    // 输入：
    //   pInfos: 通讯点表
    // 输出：（无）
    // 返回：（无）
    void SetCommInfos( const TCommInfoCluster *pInfos )
      {
      m_pInfoCluster = pInfos;
      }

    // 配置事件过滤器
    // 输入：
    //   aFilter: 事件过滤器
    // 输出：（无）
    // 返回：（无）
    void SetEventFilter(uint32_t aFilter)
      {
      m_EventFilter = aFilter;
      }

    // 定时事件
    // 输入：
    //   uTick: 时脉
    // 输出：（无）
    // 返回：（无）
    virtual void OnTick(uint32_t uTick);

  protected:
    // ------------- 内存管理 ------------
    // 分配置内存空间
    // 输入：
    //  uSize:  需要的内存空间的尺寸
    // 输出： （无）
    // 返回：
    //   0: 分配失败   非0: 内存空间指针
    void* Malloc(uint32_t uSize);

    // 释放内存空间
    // 输入：
    //  pvMem: 内存空间指针
    // 输出： （无）
    // 返回： （无）
    void MemFree(void* pvMem);

    // ------------- 信息点表访问 ------------
    // 取指定的信息点表
    // 输入：
    //  gType:  指定信息组的类型
    // 输出：（无）
    // 返回：
    //   0 = 无指定类型的信息组   else 信息组指针
    const TCommInfoGroup* GetInfoGroup(TCommInfoGroupType gType);

    // ------------- 寄存器访问 ------------
    // 读取寄存器的值
    // 输入：
    //  uRegNum:  寄存器号
    //    Value:  要求填写  Value.type指定返回值的类型
    // 输出：
    //    Value:  读取的结果在 Value.Data中
    // 返回：
    //   true = 读成功   false = 读失败
    bool ReadReg(uint32_t uRegNum,  TCOMValue &Value);

    // 读取寄存器的值
    // 输入：
    // uInfAddr:  信息点号
    //    Value:  要求填写Value.type,指定返回值的类型
    // 输出：
    //    Value:  读取的结果在 Value.Data中
    // 返回：
    //   true = 读成功   false = 读失败
    bool ReadInf(uint32_t uInfAddr, TCOMValue &Value);

    // 写到寄存器 (预写)
    // 输入：
    //  uRegNum:  寄存器号
    //    Value:  要写入的值
    // 输出：（无）
    // 返回：
    //   true = 写成功   false = 写失败
    bool WriteReg(uint32_t uRegNum,  TCOMValue &Value);

    // 写到寄存器
    // 输入：
    // uInfAddr:  信息点号
    //    Value:  要写入的值
    // 输出：（无）
    // 返回：
    //   true = 写成功   false = 写失败
    bool WriteInf(uint32_t uInfAddr, TCOMValue &Value);

    // 写确认
    // 输入：
    //   uToken:  口令 必须是 CDI_TOKEN
    // 输出：（无）
    // 返回：
    //   true = 写成功   false = 写失败
    bool WriteComfirm(uint32_t uToken);

    // 写放弃
    // 输入：（无）
    // 输出：（无）
    // 返回：
    //   true = 写成功   false = 写失败
    bool WriteCancel(void);

    // ------------- 事件相关 ------------
    // 获取最新的事件数量
    // 输入：（无）
    // 输出：（无）
    // 返回：
    //   0: 无新事件   >0: 新事件数量
    uint32_t GetNewEventCount(void);

    // 搜索最新的事件
    // 输入：（无）
    // 输出：（无）
    // 返回：
    //   0: 无新事件   非0: 新事件指针
    const TCOMEventItem* FetchEvent(void);

    // 取事件的属性
    // 事件的属性包括：动作名称、现场数据个数及现场数据寄存器号
    // 输入:
    //   uRegNum: 信息点号
    // 输出：(无)
    // 返回：
    //   0 = 不可识别的属性  else 指定的事件处理属性
    const TEventProperty*  GetEventProp(uint32_t uRegNum);

#if WAVELOGGER_EN > 0
    // ------------- 录波相关 ------------
    // 获取录波记录数量
    // 输入：（无）
    // 输出：（无）
    // 返回：
    //   0: 无新事件   >0: 记录数量
    uint32_t GetWavelogCount(void);

    // 读录波事件摘要
    // 输入：
    //   uIndex: 录波记录的序号 0 ~ GetWavelogCount - 1
    //           按时间Z-Order：最新的是0，次新的是1，......
    // 输出：（无）
    // 返回：
    //   0: 指定的录波记录不存在   非0: 新录波记录指针
    const TCOMWaveLogItem* ReadWavelogSummary(uint32_t uIndex);

    // 读取录波波形数据
    // 输入：
    //     uIndex: 录波记录的序号 0 ~ GetWavelogCount - 1
    //             按时间Z-Order：最新的是0，次新的是1，......
    //     uChlIdx:录波的通道序号 0 ~ TCOMWaveLogItem.NbrOfMeasChannels - 1 是模拟通道
    //   uPosition:起始位置，按采样点计算
    //             模拟通道，每采样点2Bytes
    //             数字通道，每采样点1Bit，按8bits对齐
    //      pBuff: 缓冲区指针，波形数据将保存到该缓冲区
    // uSampsToRead：要读回的采样点数
    //               模拟通道：读回2 x uSampsToRead字节
    //               数字通道：读回((uSampsToRead + 7) / 8)字节
    // 输出：
    //   pBuff：读回的波形数据
    // 返回：
    //   0: 参数错误   非0: 读回的字节数量
    uint32_t ReadWavelogChlData(uint32_t uIndex,         // 录波记录序号，ZOrder
                                uint32_t uChlIdx,        // 通道序号
                                uint32_t uPosition,      // 读的起始位置
                                void*    pBuff,          // 数据区
                                uint32_t uSampsToRead);  // 采样点数
#endif

    // ------------- 远程控制 ------------
    // 遥控选择
    // 输入：
    //     uToken:  令牌 必须是 CDI_TOKEN
    //   uInfAddr:  信息点号
    // 输出：（无）
    // 返回：
    //   true = 成功   false = 失败
    bool RemotrCtrlSelect (uint32_t uToken, uint32_t uInfAddr);

    // 遥控执行
    // 输入：
    //   uToken:  令牌 必须是 CDI_TOKEN
    // 输出：（无）
    // 返回：
    //   true = 成功   false = 失败
    bool RemotrCtrlExecute(uint32_t uToken);

    // 遥控撤消
    // 输入：（无）
    // 输出：（无）
    // 返回：
    //   true = 成功   false = 失败
    bool RemotrCtrlCancel (void);

    // 远程复归
    // 输入：
    //   uToken:  令牌 必须是 CDI_TOKEN
    // 输出：（无）
    // 返回：
    //   true = 成功   false = 失败
    bool RemoteSignalReturn(uint32_t uToken);

    // ------------- 装置相关 ------------
    // 校时
    // 输入：
    //   DateTime: 时间日期
    // 输出：（无）
    // 返回：
    //   true = 成功   false = 失败
    bool SetDateTime(const TDateTimeType &DateTime);

    // 获取装置工作状态
    // 输入：
    //   dsType: 状态类型
    // 输出：（无）
    // 返回：
    //   true = 置位   false = 无
    bool GetDevState( TCOMDevState dsType );

  protected:
    // 用信息点号探索信息点
    // 输入：
    //   uInf: 信息点
    // 输出：（无）
    // 返回：
    //   0: 失败   非0: 信息点描述指针
    const TCommInfoItem* GetInfoByInf(uint32_t uInf);

    // 用信息点号探索寄存器
    // 输入：
    //   uInf: 信息点
    // 输出：（无）
    // 返回：
    //   0:搜索失败  >0:寄存器地址
    uint32_t GetRegByInf(uint32_t uInf);

    // 用寄存器号探索通信信息点号
    // 输入：
    //   uRegNum: 寄存器号
    // 输出：（无）
    // 返回：
    //   0: 失败   非0: 信息点描述指针
    const TCommInfoItem* GetInfoByRegNum(uint32_t uRegNum);

    // 在指定的组中搜索寄存器
    // 输入：
    //   pgInfo： 信息组指针
    //   uRegNum: 寄存器号
    // 输出：（无）
    // 返回：
    //   0: 失败   非0: 信息点描述指针
    const TCommInfoItem* GetInfoByRegNum(const TCommInfoGroup* pgInfo,
                                                      uint32_t uRegNum);

    // 读操作记录
    //   pRegInfo: 寄存器描述
    // 返回：
    //   0: 无新事件   非0: 新事件指针
    const TCOMEventItem* FetchOpLog( const TDevRegInfoItem* pRegInfo,
                                     const TEventLogItem*   pEvtItem );

  private:
    // 初始化接口
    // 输入：（无）
    // 输出：（无）
    // 返回：（无）
    void InitIntf(void);
};
//=============================================================================
// 全局方法
//-----------------------------------------------------------------------------

//=============================================================================
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif
