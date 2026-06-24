/*

    Dev_Clock

    系时系统
    由外部Timer每毫秒进行一次Tick操作
    由参数可取回日期,时间

    2005.3.5
    卫荣平
    
    version 1.0 创建

    2005.4.24
    version 1.1 修改DecodeDate和EncodeDate,从0开始计算日期
    
    2020.10.28
    ver1.2
    移植到GPT30
  
  2020.11.24
  v1.3
  移植到GP301
*/
//-----------------------------------------------------------------------------
#include "DevClock.h"
//=============================================================================
// 局部宏
//-----------------------------------------------------------------------------
// 每天的毫秒数
#define MilSec_Per_Day    86400000L
//=============================================================================
// 局部数据
//-----------------------------------------------------------------------------
// 正常年份每个月的天数
constexpr uint8_t DayOfMonthTable[13] = {0, 31, 28, 31, 30, 31, 30, 
                                            31, 31, 30, 31, 30, 31 } ;

//=============================================================================
// 全局数据
//-----------------------------------------------------------------------------
// 全局一个时钟
TDevClock      DevClock;
//=============================================================================
// 本地方法
//-----------------------------------------------------------------------------

//=============================================================================
// 全局方法
//-----------------------------------------------------------------------------
// 取每个月份的天数(非闰年)
// 输入:
//   uMonth: 月份  1~12
// 输出:
//   返回: 0:不正确的月份 >0:当月天数(非闰年)
uint32_t DEVCLK_DaysOfMonth(uint32_t uMonth)
{
 
  if( uMonth < 13 )
    return DayOfMonthTable[uMonth];
    
  return 0;  
}
//-----------------------------------------------------------------------------
// 测试指定年份是否闰年
// 输入:
//   uYear: 年
// 返回:  0:正常年   1:闰年
uint32_t DEVCLK_IsLeapyear(uint32_t uYear)
{

  return ((uYear % 4 == 0) && (uYear % 100 != 0)) || (uYear % 400 == 0);
}
//-----------------------------------------------------------------------------
// 清理TSysClock
// 将时间恢复到2000-1-1 0:0'0"0
// 输入:
//   pClock: 系统时间结构
// 输出:
//   无
void DEVCLK_Init(TDevClock* pClock)
{
  pClock->FDays  = Days2020Since2000;  // 2020/1/1
  pClock->FMilSecs = 0;
}
//-----------------------------------------------------------------------------
// 每毫秒一次的时间节拍
// 由外部时钟中断激发
// 输入:
//   pClock: 系统时间结构
// 输出:
//   无
void DEVCLK_Tick(TDevClock* pClock)
{
  
  // 毫秒数加一
  pClock->FMilSecs++;
  
  // 达到一天(24小时 x 3600秒 x 1000毫秒)
  if( pClock->FMilSecs >= MilSec_Per_Day )
    {                    
    // 时间清零
    pClock->FMilSecs = 0;
    // 日期加一
    pClock->FDays++;
    }
}
//-----------------------------------------------------------------------------
// 检查日期分量
// 输入:
//   y: 年
//   m: 月
//   d: 日
// 输出:
//   返回: 0: 成功 1:年错  2:月错  4:日错 --- 可以组合
int DEVCLK_CheckDatePart(uint32_t uYear, uint32_t uMonth, uint32_t uDays)
{

  int wResult = 0;
  
  // 最大计数178年
  if( uYear < ClockYearBegin || uYear > ClockYearBegin + 178 )
    wResult |= 1;
  
  // 月从1~12
  if( uMonth < 1 || uMonth > 12 )
    wResult |= 2;

  // 日期1~31
  if( uDays < 1 )
    wResult |= 4;
  else
    {
    // 二月
    if( (uMonth == 2) )
      {
      // 闰年
      if( DEVCLK_IsLeapyear(uYear) )
        {
        // 有29天
        if( uDays > 29 )
          wResult |= 4;
        }
      // 平常年
      else
        // 28天
        if( uDays > 28 )
          wResult |= 4;
      }
    else if( uMonth >= 1 || uMonth <= 12 )
      if( uDays > DEVCLK_DaysOfMonth(uMonth) )
        wResult |= 4;
    }
    
  // 返回结果
  return wResult;
}
//-----------------------------------------------------------------------------
// 取当前日期的年月日
// 输入:
//   pClock: 系统时间结构
// 输出:
//    y: 年
//    m: 月
//    d: 日
void DEVCLK_DecodeDate( const TDevClock *pClock,
                         uint32_t *puYear,          // 年
                         uint32_t *puMonth,         // 月
                         uint32_t *puDayOfMonth )   // 日
{

  uint32_t uDaysSum = 0,
           uMonth   = 1,
           uYear    = ClockYearBegin;

  // 计算当前天数中经历的年份
  for( ; ; uYear++ )
    {
    uint32_t uDaysOfYear;
    // 从2000年起，逐年计算天数，从总天数中减去
    if( DEVCLK_IsLeapyear(uYear) )
      {
      // 当前年是闰年
      uDaysOfYear = 366;
      }
    else
      {
      // 当前年不是闰年
      uDaysOfYear = 365;
      }
    
    if( uDaysSum + uDaysOfYear <= pClock->FDays )
      uDaysSum += uDaysOfYear;
    else
      // 不够该年天数，日期就在在当年，乘余天数即为日数
      break;
    }

  // 由天数计算月份
  for( ; uMonth <= 12; uMonth++ )
    {
    uint32_t uDayOfMonth;

    // 闰年的二月
    if( (uMonth == 2) && DEVCLK_IsLeapyear(uYear) )
      uDayOfMonth = 29;
    else
      uDayOfMonth = DEVCLK_DaysOfMonth(uMonth);

    // 若超过该月天数，则减掉月天数，继续
    if( uDaysSum + uDayOfMonth <= pClock->FDays)
      uDaysSum += uDayOfMonth;
    else
      // 不够该月天数，日期就在在这月份上，剩余天数即为日数
      break;
    }

  // 
  *puYear  = uYear;
  *puMonth = uMonth;
  *puDayOfMonth = 1 + (pClock->FDays - uDaysSum);
}
//-----------------------------------------------------------------------------
// 设置日期的年月日
// 输入:
//   pClock: 系统时间结构
//    y: 年
//    m: 月
//    d: 日
// 输出:
//    无
void DEVCLK_EncodeDate( TDevClock *pClock,
                         uint32_t  uYear,      // 年
                         uint32_t  uMonth,     // 月
                         uint32_t  uDayOfMonth )    // 日

{

  //

  // 检查有效性
  if( DEVCLK_CheckDatePart( uYear, uMonth, uDayOfMonth ) )
    return;
    
  // 从初始年份开始，每过一年加上当年天数
  uint32_t uDays = 0;
  for( uint32_t uIdx = ClockYearBegin; uIdx < uYear; uIdx++ )
    {
    // 区别闰年和平常年
    if( DEVCLK_IsLeapyear(uIdx) )
      uDays += 366;
    else
      uDays += 365;
    }

  // 加上当处经过的月份上的天数
  for( uint32_t uIdx = 1; uIdx < uMonth; uIdx++ )
    {
    if( (uIdx == 2) && DEVCLK_IsLeapyear(uYear) )
      uDays += 29;
    else
      uDays += DEVCLK_DaysOfMonth(uIdx);
    }

  // 加上当前经过的天数(减1是为从0开始计算)
  pClock->FDays = uDays + uDayOfMonth - 1;
}
//-----------------------------------------------------------------------------
// 检查时间分量
// 输入:
//   h: 时
//   m: 分
//   s: 秒
//   u: 毫秒
// 输出:
//   返回: 0: 成功 1:时错  2:分错  4:秒错 8:毫秒错 --- 可以组合
int DEVCLK_CheckTimePart(uint32_t uHours, uint32_t uMinute, uint32_t uSeconds, uint32_t uMilSec)
{

  int wResult = 0;

  // 小时: 0~23
  if( uHours > 23u )
    wResult |= 1;
    
  // 分: 0~59
  if( uMinute > 59u )
    wResult |= 2;
  
  // 秒: 0~59
  if( uSeconds > 59u )
    wResult |= 4;
  
  // 毫秒: 0~999
  if( uMilSec > 999u )
    wResult |= 8;
  
  // 返回结果
  return wResult;
}
//-----------------------------------------------------------------------------
// 取当前时间的时、分、秒、毫秒
// 输入:
//   pClock: 日期时间结构
// 输出:
//    h: 时
//    m: 分
//    s: 秒
//    u: 毫秒
void DEVCLK_DecodeTime( const TDevClock *pClock,
                         uint32_t *puHours,    // 时
                         uint32_t *puMinute,   // 分
                         uint32_t *puSecond,   // 秒
                         uint32_t *puMilSec )  // 毫秒
{

  uint32_t tm = pClock->FMilSecs / 60000u, // 以分钟为单位,简化计算
           ts = pClock->FMilSecs % 60000u; // 分钟以下的毫秒数
  *puHours  = tm / 60;
  *puMinute = tm % 60;
  *puSecond = ts / 1000;
  *puMilSec = ts % 1000;
}
//-----------------------------------------------------------------------------
// 设置时间的时、分、秒、毫秒
// 输入:
//   pClock: 日期时间结构
//    h: 时
//    m: 分
//    s: 秒
//    u: 毫秒
// 输出:
//   无
void DEVCLK_EncodeTime( TDevClock *pClock,
                         uint32_t  uHours, 
                         uint32_t  uMinute, 
                         uint32_t  uSeconds, 
                         uint32_t  uMilSec )
{
  // 以毫秒为单位
  pClock->FMilSecs = (uint32_t)((uint32_t)(uHours) * 3600u + uMinute * 60 + uSeconds) * 1000u + uMilSec;
}                  
//-----------------------------------------------------------------------------
// 时间校正
// 输入:
//         pClock: 日期时间结构
//   iDiffMilSecs: 时间差, 以ms为单位 >0时向后调整 <0时向前调整
// 输出:
//   无
// 返回:
//   无
void DEVCLK_TimeCorrect(TDevClock* pClock, int iDiffMilSecs)
{
  
  if( iDiffMilSecs > 0 )
  {
    // 向后溢出
    if( pClock->FMilSecs + iDiffMilSecs >= MilSec_Per_Day )
    {
      // 计算累加到后一天的时间
      pClock->FMilSecs = pClock->FMilSecs + iDiffMilSecs - MilSec_Per_Day;
      pClock->FDays++;
    }
    else
    {
      pClock->FMilSecs += iDiffMilSecs;
    }
  }
  else if( iDiffMilSecs < 0 )
  {
    // 向前溢出
    if( pClock->FMilSecs + iDiffMilSecs >= MilSec_Per_Day )
    {
      // 向前一天
      pClock->FMilSecs = pClock->FMilSecs + iDiffMilSecs + MilSec_Per_Day;
      pClock->FDays--;
    }
    else
      pClock->FMilSecs += iDiffMilSecs;
  }
}
//-----------------------------------------------------------------------------
// 时间校正
// 按分量校正
// 输入:
//  date: 日期
//  time: 时间
//    iDiffDays: 时间差, 以ms为单位 >0时向后调整 <0时向前调整
// 输出:
//   无
// 返回:
//   无
void DEVCLK_TimeCorrect2(uint32_t *date, uint32_t *time, int iDiffDays)
{

  if( iDiffDays > 0 )
  {
    // 向后溢出
    if( *time + iDiffDays >= MilSec_Per_Day )
    {
      // 计算累加到后一天的时间
      *time = *time + iDiffDays - MilSec_Per_Day;
      (*date)++;
    }
    else
    {
      *time += iDiffDays;
    }
  }
  else if( iDiffDays < 0 )
  {
    // 向前溢出
    if( *time + iDiffDays >= MilSec_Per_Day )
    {
      // 向前一天
      *time = *time + iDiffDays + MilSec_Per_Day;
      (*date)--;
    }
    else
      *time += iDiffDays;
  }
}
//-----------------------------------------------------------------------------
// 填写日期到数据区
// 输入:
//   pClock: 日期时间结构
//  pucBuf: 数据区指针
// 输出:
//   填写的长度  
int DEVCLK_FillDate2Buf(const TDevClock* pClock, uint8_t* pucBuf)
{

  uint32_t y, m, d;
  
  // 解码日期
  DEVCLK_DecodeDate( pClock, &y, &m, &d );
  
  // 填写数据区
  pucBuf[0] = y - ClockYearBegin;
  pucBuf[1] = m;
  pucBuf[2] = d;
  
  return 3;
}
//-----------------------------------------------------------------------------
// 填写时间到数据区
// 输入:
//   pClock: 日期时间结构
//  pucBuf: 数据区指针
// 输出:
//   填写的长度  
int FillTime2Buf(TDevClock* pClock, uint8_t* pucBuf)
{

  uint32_t h, m, s, u;
  
  // 时间解码
  DEVCLK_DecodeTime( pClock, &h, &m, &s, &u );
  
  // 填写数据区
  pucBuf[0] = h;
  pucBuf[1] = m;
  pucBuf[2] = s;
  pucBuf[3] = u / 0x100;
  pucBuf[4] = u % 0x100;
  
  return 5;
}
//-----------------------------------------------------------------------------
// 取日期串
// 输入:
//   pClock: 系统时间结构
//   pucBuf: 数据区  格式:YYMMDD,如:050305=2005.3.5
// 输出:
//   返回: 1:成功  0:失败  
int DEVCLK_DateString(const TDevClock* pClock, uint8_t* pucBuf)
{

  uint32_t y, m, d;

  if( !pucBuf )
    return 0;

  // 取年、月、日
  DEVCLK_DecodeDate( pClock, &y, &m, &d );

  // 填写结果: YYMMDD
  y -= ClockYearBegin;
  pucBuf[0] = y / 10 + 48;
  pucBuf[1] = y % 10 + 48;
  pucBuf[2] = m / 10 + 48;
  pucBuf[3] = m % 10 + 48;
  pucBuf[4] = d / 10 + 48;
  pucBuf[5] = d % 10 + 48;
  pucBuf[6] = 0;

  return 1;
}
//-----------------------------------------------------------------------------
// 取时间串
// 输入:
//   pClock: 日期时间结构
// 输出:
//  pucBuf: 数据区  格式:HHMMSS,如:142213=14:22"13'
// 返回: 1:成功  0:失败  
int DEVCLK_TimeString(const TDevClock* pClock, uint8_t* pucBuf)
{

  uint32_t h, m, s, u;

  if( !pucBuf )
    return 0;

  // 取时、分、秒、毫秒
  DEVCLK_DecodeTime( pClock, &h, &m, &s, &u );

  // 填写结果： HHMMSS
  pucBuf[0] = h / 10 + 48;
  pucBuf[1] = h % 10 + 48;
  pucBuf[2] = m / 10 + 48;
  pucBuf[3] = m % 10 + 48;
  pucBuf[4] = s / 10 + 48;
  pucBuf[5] = s % 10 + 48;
  pucBuf[6] = 0;

  return 1;
}
//-----------------------------------------------------------------------------
// 取日期和时间串
// 输入:
//   pClock: 系统时间结构
// 输出:
//  pucBuf: 数据区  格式:YYMMDDHHNNSS -- "050310181103" = 2005.3.5 18:11'3"
// 返回: 1: 成功  0:失败
uint32_t DEVCLK_DateTimeString(const TDevClock* pClock, uint8_t* pucBuf)
{

  if( !pucBuf )
    return 0;

  // 填写日期
  DEVCLK_DateString( pClock, pucBuf );

  // 填写时间
  DEVCLK_TimeString( pClock, pucBuf + 6 );

  return 1;
}
//-----------------------------------------------------------------------------
// 用日期串设置日期
// 输入:
//   pClock: 系统时间结构
//  pucBuf: 数据区  格式:YYMMDD,如:050305=2005.3.5
// 返回: 无
void DEVCLK_SetDateByString(TDevClock* pClock, uint8_t *pucBuf)
{

   uint32_t y, m, d;

   // 分解年、月、日
   y = (pucBuf[0] - 48) * 10 + (pucBuf[1] - 48) + ClockYearBegin;
   m = (pucBuf[2] - 48) * 10 + (pucBuf[3] - 48);
   d = (pucBuf[4] - 48) * 10 + (pucBuf[5] - 48);

   // 日期编码
   DEVCLK_EncodeDate( pClock, y, m, d );
}
//-----------------------------------------------------------------------------
// 用时间串设置时间
// 输入:
//   pClock: 日期时间结构
//  pucBuf: 数据区  格式:HHMMSS,如:142213=14:22"13'
// 输出:
// 返回: 无
void DEVCLK_SetTimeByString(TDevClock* pClock, uint8_t *pucBuf)
{

  uint32_t h, m, s;

  // 分解时、分、秒
  h = (pucBuf[0] - 48) * 10 + (pucBuf[1] - 48);
  m = (pucBuf[2] - 48) * 10 + (pucBuf[3] - 48);
  s = (pucBuf[4] - 48) * 10 + (pucBuf[5] - 48);

  // 以毫秒为单位
  pClock->FMilSecs = h * 3600000u + m * 60000 + s * 1000;
}
//-----------------------------------------------------------------------------
// 当前日期是星期几？
// 输入:
//   pClock: 日期时间结构
//   pucBuf: 数据区指针
// 输出:
//   返回: 1~7: Sunday Monday ...
uint32_t DEVCLK_DayOfWeek(TDevClock* pClock)
{

  // 2000-1-1是周六
  return (pClock->FDays + 5) % 7 + 1;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 读取时间
// 输入:
//   pClock: 日期时间结构
// 输出：
//   pTime:  数据区  格式:HHMMSS,如:142213=14:22"13'
// 返回: 无
void DEVCLK_GetDateTime(const TDevClock* pClock, TDateTimeType* pTime)
{
  
  uint32_t y, m, d, h, min, sec, ms;
  
  DEVCLK_DecodeDate( pClock, &y, &m, &d );  
  DEVCLK_DecodeTime( pClock, &h, &min, &sec, &ms );

  pTime->Year  = y;
  pTime->Month = m;
  pTime->Day   = d;
  pTime->WeekDay = (pClock->FDays - 1) % 7;
  pTime->Hours   = h;
  pTime->Minutes = min;
  pTime->Seconds = sec;
  pTime->MilSeconds = ms;
}
//-----------------------------------------------------------------------------
// 设置时间
// 输入:
//   pClock: 日期时间结构
//   pTime:  数据区
// 返回: 无
void DEVCLK_SetDateTime(TDevClock* pClock, const TDateTimeType* pTime)
{
  
  DEVCLK_EncodeDate( pClock, 
                     pTime->Year, 
                     pTime->Month, 
                     pTime->Day );  
  DEVCLK_EncodeTime( pClock, 
                     pTime->Hours, 
                     pTime->Minutes, 
                     pTime->Seconds, 
                     pTime->MilSeconds );
}
//-----------------------------------------------------------------------------
// 填写事件摘要中的时间区域
// 输入:
//   pClock: 日期时间结构
// 输出：
//   pSummary:  数据区
// 返回: 无
void DEVCLK_FillEventSummary(const TDevClock* pClock, TEventLogSummary* pSummary)
{
  
  uint32_t y, m, d, h, min, sec, ms;
  
  DEVCLK_DecodeDate( pClock, &y, &m, &d );
  DEVCLK_DecodeTime( pClock, &h, &min, &sec, &ms );

  pSummary->Time.Year  = y;
  pSummary->Time.Month = m;
  pSummary->Time.Day   = d;
  pSummary->Time.Hours = h;
  pSummary->Time.Minutes = min;
  pSummary->Time.Seconds = sec;
  pSummary->Time.milSecs = ms;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 检查日期时间
// 输入:
//   pTime: 日期时间
// 输出:
//   返回: 0: 成功;  !=0: 失败
int DEVCLK_CheckDateTime( TDateTimeType* pTime )
{

  if( nullptr == pTime )
    return 2048;
  
  //---------------------------
  int iResult = 0;

  // 小时: 0~23
  if( 23 < pTime->Hours )
    iResult |= 1;
    
  // 分: 0~59
  if( 59 < pTime->Minutes )
    iResult |= 2;
  
  // 秒: 0~59
  if( 59 < pTime->Seconds )
    iResult |= 4;
  
  // 毫秒: 0~999
  if( 999 < pTime->MilSeconds )
    iResult |= 8;
  
  // 最大计数178年
  if( ClockYearBegin > pTime->Year || ClockYearBegin + 178 < pTime->Year )
    iResult |= 16;
  
  // 日期1~31
  if( 1 > pTime->Day )
    iResult |= 64;
  else
    {
    // 二月
    if( 2 == pTime->Month )
      {
      // 闰年
      if( 0 != DEVCLK_IsLeapyear(pTime->Year) )
        {
        // 有29天
        if( 29 < pTime->Day )
          iResult |= 64;
        }
      // 平常年
      else
        // 28天
        if( 28 < pTime->Day )
          iResult |= 64;
      }
    else if( 1 <= pTime->Month && 12 >= pTime->Month )
      {
      // 月从1~12
      if( DEVCLK_DaysOfMonth(pTime->Month) < pTime->Day )
        iResult |= 64;
      }
    else
      iResult |= 32;
    }

  return iResult;
}
//-----------------------------------------------------------------------------
