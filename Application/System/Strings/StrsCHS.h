//-----------------------------------------------------------------------------
/*
 File       : StrsCHS.h
 Version    : V3.0
 By         : 卫荣平

 Description :简体中文的字符表

 Date       : 2016.06.14
*/
//-----------------------------------------------------------------------------
#ifndef DEV_STRSCHS_H
#define DEV_STRSCHS_H

//=============================================================================
// 通用的字符串
//-----------------------------------------------------------------------------
// 装置异常
//-----------------------------------------------------------------------------
//#define szCriticalFW          "装置软件故障"
//#define szCriticalHW          "装置硬件故障"
//#define szCriticalFT          "装置类型错误"
//#define szCriticalDZ          "装置定值错误"
//#define szCriticalUKN         "装置未知错误"
//#define szCritical            "出错了！"

//#define szCriticalFmt         "正在偿试修复..."
//#define szCriticalRst         "    需要重启。重启后须重新设置\n定值的参数。"
//#define szCriticalOk          "    修复成功。重启后须重新设置\n定值的参数。"
//#define szCriticalFailed      "    修复失败。请重试或\n返厂维修。"
//-----------------------------------------------------------------------------
// GUI界面上的字串
//-----------------------------------------------------------------------------
// 主菜单
//-----------------------------------------------------------------------------
//#define szMeasAdjuest        "模拟量修正"
//-----------------------------------------------------------------------------
//#define szMenuFuncType       "功能类型：%s\nDevice Function"
//#define szMenuFrimware       "固件版本%s：%u.%u.%u.%u\nFirmware Version"
//#define szMenuFirmCRC        "固件校验：%04X-%04X\nFirmware CRC32"
//#define szMenuBootFirm       "启动固件：%u.%u.%u.%u[%04X-%04X]\nBoot Infomation"
//#define szMenuHardware       "硬件信息：%u.%u[%04X-%04X]\nHardware Version & Ident"
//#define szSupportInfo        "技术支持：%s\nSupport: %s"
//-----------------------------------------------------------------------------
//// 恢复默认值
//#define szMenuRstSetting     "恢复第%u区默认定值，\n将丢失设置！\n是否继续恢复？"
//#define szMenuRstSetCfgOk    "恢复第%u区默认定值，\n成功完成。"
//#define szMenuRstSetCfgErr   "恢复第%u区默认定值，\n保存失败。"

//#define szMenuRstDevCfg      "恢复默认设备参数，\n将丢失设置！\n是否继续恢复？"
//#define szMenuRstDevCfgOk    "恢复默认设备参数，\n成功完成。"
//#define szMenuRstDevCfgErr   "恢复默认设备参数，\n保存失败。"
//-----------------------------------------------------------------------------
// 菜单内提示信息
#define szFuncLocked        "功能被锁定，\n需要超级口令。"
#define szFuncFailed        "功能锁定"
#define szFuncUnanth        "功能未被授权！\n操作不能继续。"

// 清除事件记录 确认
#define szClrEventLogCfm    "是否要清除：\n全部【事件记录】？"
#define szClrAlarmLogCfm    "是否要清除：\n全部【告警记录】？"
#define szClrFaultLogCfm    "是否要清除：\n全部【事故记录】？"
#define szClrEventLogOk     "成功清除：\n全部【事件记录】。"
#define szClrAlarmLogOk     "成功清除：\n全部【告警记录】。"
#define szClrFaultLogOk     "成功清除：\n全部【事故记录】。"
//-----------------------------------------------------------------------------
#define szClrWaveLogCfm     "是否要清除：\n全部【故障波形记录】？"
#define szClrWaveLogOk      "成功清除：\n全部【故障波形记录】。"
#define szClrWaveLogFail    "清除[波形记录]: \n失败，请重试。"
//-----------------------------------------------------------------------------
#define szClrMeteringCfm    "是否要清零：\n全部[电度数据]?"
#define szClrMeteringOk     "成功清零\n全部[电度数据]。"
#define szClrMeteringFail   "[电度数据]清零: \n失败，请重试。"
//-----------------------------------------------------------------------------
#define szDeviceResetCfm    "是否要：\n复位本装置?"
//-----------------------------------------------------------------------------
// 用户登录窗体
// Login_Form
//-----------------------------------------------------------------------------
#define szLoginCap1         "操作口令:"
#define szLoginCap2         "管理口令:"
#define szLoginCap3         "超级口令:"

#define szLoginChkErrCap    "口令错误"
#define szLoginChkErrTxt    szLoginChkErrCap"！\n请检查后重试。"

#define szSetPSWDCap1       "修改" szLoginCap1
#define szSetPSWDCap2       "修改" szLoginCap2

#define szSetPSWDCFM        "将被修改。\n请牢记新口令。\n是否继续？"
#define szSetPSWDCFM1       szLoginCap1 szSetPSWDCFM
#define szSetPSWDCFM2       szLoginCap2 szSetPSWDCFM

#define szSetPSWDOk         "\n修改成功！"
#define szSetPSWDOk1        szLoginCap1 szSetPSWDOk
#define szSetPSWDOk2        szLoginCap2 szSetPSWDOk
////-----------------------------------------------------------------------------
//// 切换运行区窗体
//// SetSwitch_Form
////-----------------------------------------------------------------------------
//#define szSSDestIsCur      "目标区是当前区。\n操作不能完成！"

//#define szSSSwtAtvHldOk    "切换运行区到：\n第 %u 区。\n切换成功！"
//#define szSSSwtAtvHldErr   "切换运行区到：\n第 %u 区。\n切换失败！"
//#define szSSSwtEdtHldErr   "切换编辑区到：\n【第%u区】至【第%u区】\n切换失败！"

//#define szSSSwtCpyHldOk    "复制保护定值和压板：\n【第%u区】至【第%u区】。\n操作成功！"
//#define szSSSwtCpyHldErr   "复制保护定值和压板：\n【第%u区】至【第%u区】。\n操作失败！"
////-----------------------------------------------------------------------------
// 校准提示
//#define szACCalibHint      "交流输入：\n"   \
//                           "  U = 57.735V\n"    \
//                           "  I  =  5.000A\n"    \
//                           "  $ =  0.000`"
//#define szDCCalibHint0     "直流输入：\n"   \
//                           "  Step: 1\n"      \
//                           "    V = 10.000V\n"
//#define szDCCalibHint1     "直流输入：\n"   \
//                           "  Step: 2\n"      \
//                           "    V = 30.000V\n"

//#define szCalibCap         "校准操作"
//#define szCalibOk          szCalibCap "成功。"
//#define szCalibSomeOk      szCalibCap "完成。\n有部分通道未能校准！"
//#define szCalibFailed      szCalibCap "失败。"

//#define szMeaAdjFailed     szMeasAdjuest "失败！\n需检查信号源、\n标定值是否正确？"
//=============================================================================
// 信息点表中的字串
//-----------------------------------------------------------------------------
#define szEnumChanged      "%s\n修改成功！"
#define szEnumChgRst       szEnumChanged "\n装置将重启。"

////-----------------------------------------------------------------------------
//// 直流配置
//#define szDCCfgName01      "最小"
//#define szDCCfgName02      "最大"
//#define szDCCfgName03      "类型"

//#define szDCCfgName(n)     "直流"#n":"
////-----------------------------------------------------------------------------
//// 网口
//#define szNICNo(x)         "网口"#x":"
//#define szSocketNo(x)      "联接"#x":"

//#define szNICCfgName01     "IP地址"
//#define szNICAddr(n)       szNICCfgName01#n
//#define szNICCfgName02     "子网掩码"
//#define szNetMask(n)       szNICCfgName02#n
//#define szNICCfgName03     "网关地址"
//#define szNetGate(n)       szNICCfgName03#n
//#define szNICCfgName04     "物理地址"
//#define szNetMAC(n)        szNICCfgName04#n
//#define szNICCfgName05     "对端地址"
//#define szSktAddr(n)       szNICCfgName05#n
//#define szNICCfgName06     "模式"
//#define szNICCfgName07     "端口"
//#define szNICCfgName08     "规约"
//-----------------------------------------------------------------------------
//// GOOSE
//#define szGOOSEPUBAPPID    "发布器APPID"
//#define szGOOSEPUBVLAN     "发布器VLAN ID"
//#define szGOOSEPUBVLANPRIO "发布器VLAN优先级"
//#define szGOOSEPUBINTV(n)  "发布器重发间隔["#n"]"
//#define szGOOSEPUBMAC(n)   "发布器对端地址["#n"]"

//#define szGOOSESUBAPPID(n) "订阅["#n"] APPID"
//-----------------------------------------------------------------------------
// 串口
#define szUartName(n)      "串口"#n":"

#define szUARTCfgName01    "配置"
#define szUARTCfgName02    "地址"
#define szUARTCfgName03    "速率"
#define szUARTCfgName04    "校验"
#define szUARTCfgName05    "规约"
//-----------------------------------------------------------------------------
#define szEventName00      "装置掉电"
#define szEventName01      "装置上电"
#define szEventName02      szEventName01 " PWR"
#define szEventName03      szEventName01 " WDG"
#define szEventName04      szEventName01 " SFT"
#define szEventName05      "修改日期时间"
//#define szEventName06      "信号继电器复归"

#define szRemoteOp         "远程"
#define szRemoteCtrl       "遥控"
#define szEventName07      szRemoteOp szEventName05
#define szEventName08      szRemoteOp "上传配置数据"
#define szEventName09      szRemoteCtrl szEventName06

//#define szRelay(n)         "出口"#n
//#define szEventName10      szRemoteCtrl szRelay(1)
//#define szEventName11      szRemoteCtrl szRelay(2)
//#define szEventName12      szRemoteCtrl szRelay(3)
//#define szEventName13      szRemoteCtrl szRelay(4)
//#define szEventName14      szRemoteCtrl szRelay(5)
//#define szEventName15      szRemoteCtrl szRelay(6)
//#define szEventName16      szRemoteCtrl szRelay(7)
//#define szEventName17      szRemoteCtrl szRelay(8)
//#define szEventName18      szRemoteCtrl szRelay(9)
//#define szEventName19      szRemoteCtrl szRelay(10)
//-----------------------------------------------------------------------------
// GOOSE事件
//-----------------------------------------------------------------------------
#define szGSEvtSubBrkOffName(n) "GOOSE订阅"#n
//-----------------------------------------------------------------------------
// 开入别名
//-----------------------------------------------------------------------------
//// 开入默认名称
//#define szDIName(n)        "开入"#n

//#define szDITestName(n)    "开入"#n": 测试"

//// 用于编辑“开入别名”
//#define szDIAlias(n)       "开入"#n"功能"
//-----------------------------------------------------------------------------
////  开出状态
//#define szEnRlyState       "启动继状态"
//#define szRlyState(n)      "开出"#n": CK"#n"状态"
//#define szRlyXHState(n, m) "开出"#n": XH"#m"状态"

//#define szRlyTest(n)       "开出"#n": CK"#n"测试"
//#define szRlyXHTest(n, m)  "开出"#n": XH"#m"测试"
//-----------------------------------------------------------------------------
//// 谐波分量
//#define szHARM_THD         "谐波总含量"
//#define szHARMName01       "1侧电流" szHARM_THD
//#define szHARMName02       "1侧电压" szHARM_THD
//#define szHARMName03       "2侧电流" szHARM_THD
//#define szHARMName04       "2侧电压" szHARM_THD

//#define szHARM01(n)        #n"谐波总含有率"
//#define szHARM02(n)        #n"谐波总含量"
//#define szHARM03(n)        #n"奇次谐波含有率"
//#define szHARM04(n)        #n"奇次谐波含量"
//#define szHARM05(n)        #n"偶次谐波含有率"
//#define szHARM06(n)        #n"偶次谐波含量"
//#define szHARM07(n)        #n"谐波含量"
//#define szHARM08(n)        #n"谐波含有率"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif //__STRSCHS_H

