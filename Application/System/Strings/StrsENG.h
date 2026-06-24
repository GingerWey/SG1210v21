//-----------------------------------------------------------------------------
/*
 File       : StrsENG.h
 Version    : V3.0
 By         : 卫荣平

 Description :英文的字符表

 Date       : 2016.06.14
*/
//-----------------------------------------------------------------------------
#ifndef DEV_STRSENG_H
#define DEV_STRSENG_H

//=============================================================================
// 通用的字符串
//-----------------------------------------------------------------------------
// 装置异常
//-----------------------------------------------------------------------------
#define szEngCriticalFW    "Devive FE Fault"
#define szEngCriticalHW    "Devive HW Fault"
#define szEngCriticalFT    "Devive FT Fault"
#define szEngCriticalDZ    "Devive SD Fault"
#define szEngCriticalUKN   "Devive UKN Fault"
#define szEngCritical      "Try to repair.\nIf show this again,\nContact to factory."

#define szEngCriticalFmt      "Repairing..."
#define szEngCriticalRst      "    Need Reset。Device\nmust be set RECONFIG!"
#define szEngCriticalOk       "    Repair OK! Device\nmust be set RECONFIG!"
#define szEngCriticalFailed   "    Repair FAILED!。Please retry\nor contact to factory\nfor help."
//-----------------------------------------------------------------------------
// GUI界面上的字串
//-----------------------------------------------------------------------------
// 主菜单
// Menu_Form
//-----------------------------------------------------------------------------
#define szEngMeasAdjuest      "AC. Adjust"         //"模拟量修正"
//-----------------------------------------------------------------------------
// 恢复默认值
#define szEngMenuRstSetting   "Load DEFAULT to\n[Block No.%u],\nSettings will be\nlost! Coninute?"
#define szEngMenuRstSetCfgOk  "Load DEFAULT to\n[Block No.%u],\nSuccesseful!"
#define szEngMenuRstSetCfgErr "Load DEFAULT to\n[Block No.%u],\nFailed!"

#define szEngMenuRstDevCfg    "Load default CONFIG,\nConfig will be\nlost!\nConinute?"
#define szEngMenuRstDevCfgOk  "Load default CONFIG,\nSuccessful!"
#define szEngMenuRstDevCfgErr "Load default CONFIG,\nFailed!."
//-----------------------------------------------------------------------------
// 菜单内提示信息
#define szEngFuncLocked       "Function needs\nSuper Password."   //"功能被锁定,\n需要超级口令."
#define szEngFuncFailed       "Function Locked."                  //"功能锁定"
#define szEngFuncUnanth       "Function Locked!\nCan't continue." // "功能未被授权!\n操作不能继续."

// 清除事件记录 确认
#define szEngClrEventLogCfm   "ALL [Event Logs]\nwill be DELETED,\nContinue?"
#define szEngClrAlarmLogCfm   "ALL [Alarm Logs]\nwill be DELETED,\nContinue?"
#define szEngClrFaultLogCfm   "ALL [Fault Logs]\nwill be DELETED,\nContinue?"
#define szEngClrEventLogOk    "ALL [Event logs]\nhave been\ndeleted\nsuccessefully."
#define szEngClrAlarmLogOk    "ALL [Alarm logs]\nhave been\ndeleted\nsuccessefully."
#define szEngClrFaultLogOk    "ALL [Fault logs]\nhave been\ndeleted\nsuccessefully."
//-----------------------------------------------------------------------------
#define szEngClrWaveLogCfm    "ALL [Wave Logs]\nwill be DELETED,\nContinue?"
#define szEngClrWaveLogOk     "ALL [Wave logs]\nhave been\ndeleted\nsuccessefully."
#define szEngClrWaveLogFail   "Delete\nALL [Wave logs]\nFailed."
//-----------------------------------------------------------------------------
#define szEngClrMeteringCfm   "ALL [Metering]\nwill to be zero,\nContinue?"
#define szEngClrMeteringOk    "ALL [Metering]\nhave been\nreset\nsuccessefully."
#define szEngClrMeteringFail  "Reset\nALL [Metering]\nFailed."
//-----------------------------------------------------------------------------
#define szEngDeviceResetCfm   "Deivce will be RESET?\nContinue?"
//-----------------------------------------------------------------------------
// 用户登录窗体
// Login_Form
//-----------------------------------------------------------------------------
#define szEngLoginCap1        "Password:"
#define szEngLoginCap2        "Administrator Password:"
#define szEngLoginCap3        "Super password:"
#define szEngLoginApply       "Application code:"

#define szEngLoginChkErrCap   "Incorrect\npassword"
#define szEngLoginChkErrTxt   szEngLoginChkErrCap "!\nPlease try again."

#define szEngSetPSWDCap1      "Set " szEngLoginCap1
#define szEngSetPSWDCap2      "Set " szEngLoginCap2

#define szEngSetPSWDCFM       "\nwill be\nmodified.\nSAVE or NOT?"
#define szEngSetPSWDCFM1      szEngLoginCap1 szEngSetPSWDCFM
#define szEngSetPSWDCFM2      szEngLoginCap2 szEngSetPSWDCFM

#define szEngSetPSWDOk        "\nmodification \nsuccessful!"
#define szEngSetPSWDOk1       szEngLoginCap1 szEngSetPSWDOk
#define szEngSetPSWDOk2       szEngLoginCap2 szEngSetPSWDOk
////-----------------------------------------------------------------------------
//// 切换运行区窗体
//// SetSwitch_Form
////-----------------------------------------------------------------------------
//#define szEngSSDestIsCur      "Invalid selection\noperation failed!"

//#define szEngSSSwtAtvHldOk    "Switched\n[Setting Block]\nto No: %u,\nSuccessful!"
//#define szEngSSSwtAtvHldErr   "Switched\n[Setting Block]\nto No: %u,\nFailed!"
//#define szEngSSSwtEdtHldErr   "Switched Block\nfrom No:%u to No:%u,\nFailed!"

//#define szEngSSSwtCpyHldOk    "Copy\n[Setting Block]\nfrom No:%u to No:%u,\nSuccessful!"
//#define szEngSSSwtCpyHldErr   "Copy\n[Setting Block]\nfrom No:%u to No:%u,\nFailed!"
//-----------------------------------------------------------------------------
// 校准提示
#define szEngACCalibHint      "AC Input:\n"   \
                              "U=57.735V\n"   \
                              "I= 5.000A\n"   \
                              "$= 0.000`"
//#define szEngDCCalibHint0     "DC Input:\n"   \
//                              "Step: 0\n"     \
//                              "V = 10.000V\n"
//#define szEngDCCalibHint1     "DC Input:\n"   \
//                              "Step: 1\n"     \
//                              "V = 30.000V\n"
#define szEngCalibCap         "Calibration "
#define szEngCalibOk          szEngCalibCap " succssful!"
#define szEngCalibSomeOk      szEngCalibCap " Completed.\nBut some channels failed!"
#define szEngCalibFailed      szEngCalibCap " failed."

#define szEngMeaAdjFailed     szEngMeasAdjuest " failed!\nPlease try again."
//=============================================================================
// 信息点表中的字串
//-----------------------------------------------------------------------------
// 设备参数
#define szEngEnumChanged      "%s\nhas been changed!"
#define szEngEnumChgRst       szEngEnumChanged "\nDevice will be\nRESET."

////-----------------------------------------------------------------------------
//// 直流配置
//#define szEngDCCfgName01      "Min"
//#define szEngDCCfgName02      "Max"
//#define szEngDCCfgName03      "Type"

//#define szEngDCCfgName(n)     "DC "#n":"
////-----------------------------------------------------------------------------
//// 网口
//#define szEngNICNo(x)         "ETH No."#x":"
//#define szEngSocketNo(x)      "Socket "#x":"

//#define szEngNICCfgName01     "IP address "
//#define szEngNICAddr(n)       szEngNICCfgName01#n
//#define szEngNICCfgName02     "Subnet mask"
//#define szEngNetMask(n)       szEngNICCfgName02#n
//#define szEngNICCfgName03     "NetGate addr."
//#define szEngNetGate(n)       szEngNICCfgName03#n
//#define szEngNICCfgName04     "MAC address"
//#define szEngNetMAC(n)        szEngNICCfgName04#n
//#define szEngNICCfgName05     "Peer address"
//#define szEngSktAddr(n)       szEngNICCfgName05#n
//#define szEngNICCfgName06     "Mode"        // "模式"
//#define szEngNICCfgName07     "Port"        // "端口"
//#define szEngNICCfgName08     "Protocol"    // "规约"
////-----------------------------------------------------------------------------
//// GOOSE
//#define szENGGOOSEPUBINTV(n)  "Publisher "#n" Interval "
//#define szENGGOOSEPUBMAC(n)   "Publisher "#n" Peer Address "

#define szENGGOOSESUBAPPID(n) "Subscriber "#n" APPID "
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 串口
#define szEngUartName(n)      "COM"#n":"    // "串口"#n":"

#define szEngUARTCfgName01    "Config"      // "配置"
#define szEngUARTCfgName02    "Address"     // "地址"
#define szEngUARTCfgName03    "Baudrate"    // "速率"
#define szEngUARTCfgName04    "Parity"      // "校验"
#define szEngUARTCfgName05    "Protocol"    // "规约"
//-----------------------------------------------------------------------------
#define szEngEventName00      "Device shutdown" // "装置掉电"
#define szEngEventName01      "Device stratup"  // "装置上电"
#define szEngEventName02      szEngEventName01 " PWR"
#define szEngEventName03      szEngEventName01 " WDG"
#define szEngEventName04      szEngEventName01 " SFT"
#define szEngEventName05      "Set time"
#define szEngEventName06      "Signal relays return"

//#define szEngRemoteOp         "Remote "
//#define szEngRemoteCtrl       "Remote "
//#define szEngEventName07      szEngRemoteOp "\n" szEngEventName05
//#define szEngEventName08      szEngRemoteOp " upload config"
//#define szEngEventName09      szEngRemoteCtrl szEngEventName06

//#define szEngRelay(n)         "DO No."#n
//#define szEngEventName10      szEngRemoteCtrl szEngRelay(1)
//#define szEngEventName11      szEngRemoteCtrl szEngRelay(2)
//#define szEngEventName12      szEngRemoteCtrl szEngRelay(3)
//#define szEngEventName13      szEngRemoteCtrl szEngRelay(4)
//#define szEngEventName14      szEngRemoteCtrl szEngRelay(5)
//#define szEngEventName15      szEngRemoteCtrl szEngRelay(6)
//#define szEngEventName16      szEngRemoteCtrl szEngRelay(7)
//#define szEngEventName17      szEngRemoteCtrl szEngRelay(8)
//#define szEngEventName18      szEngRemoteCtrl szEngRelay(9)
//#define szEngEventName19      szEngRemoteCtrl szEngRelay(10)
////-----------------------------------------------------------------------------
//// GOOSE事件
////-----------------------------------------------------------------------------
//#define szEngGSEvtSubBrkOffName(n) "GOOSE Sub. No:"#n
////-----------------------------------------------------------------------------
//// 开入别名
////-----------------------------------------------------------------------------
//// 开入默认名称
//#define szEngDIName(n)        "DI No."#n

//#define szEngDITestName(n)    "DI No."#n": test"

//// 用于编辑“开入别名”
//#define szEngDIAlias(n)       "DI"#n" Function"
////-----------------------------------------------------------------------------
////  开出状态
//#define szEngEnRlyState       "Startup Relay State"
//#define szEngRlyState(n)      "DO No."#n"(CK"#n") State"
//#define szEngRlyXHState(n, m) "DO No."#n"(XH"#m") State"

//#define szEngRlyTest(n)       "DO No."#n"(CK"#n") test"
//#define szEngRlyXHTest(n, m)  "DO No."#n"(XH"#m") test"
////-----------------------------------------------------------------------------
//// 谐波分量
//#define szEngHARM_THD         " THD"
//#define szEngHARMName01       "Sd.1 Curr." szEngHARM_THD
//#define szEngHARMName02       "Sd.1 Volt." szEngHARM_THD
//#define szEngHARMName03       "Sd.2 Curr." szEngHARM_THD
//#define szEngHARMName04       "Sd.2 Volt." szEngHARM_THD

//#define szEngHARM01(n)        #n" THD"
//#define szEngHARM02(n)        #n" total"
//#define szEngHARM03(n)        #n" THD odd"
//#define szEngHARM04(n)        #n" total odd"
//#define szEngHARM05(n)        #n" THD even"
//#define szEngHARM06(n)        #n" total even"
//#define szEngHARM07(n)        #n" harmonic content"
//#define szEngHARM08(n)        #n" harmonic ratio"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif //__STRSENG_H

