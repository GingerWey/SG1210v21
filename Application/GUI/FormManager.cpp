//-----------------------------------------------------------------------------
/*
 File        : FormManager.cpp
 Version     : V1.10
 By          : 银网科技
 Description : GUI主控

 Date       : 2026.6.8
*/
//-----------------------------------------------------------------------------
#include "FormManager.h"

#include "GWinTypes.h"
#include "GUIMessage.h"

#include <cstdio>
#include <cstdint>

using namespace std;
//=============================================================================
// 本地宏
//-----------------------------------------------------------------------------

//=============================================================================
// 数据结构
//-----------------------------------------------------------------------------

//=============================================================================
// class VFormManager
//-----------------------------------------------------------------------------
VFormManager& VFormManager::Instance()
{
    static VFormManager inst;
    return inst;
}

VFormManager::VFormManager()
{
    // 可在此处做必要初始化
}

VFormManager::~VFormManager()
{
    // 清理
}

void VFormManager::RegisterForm(uint32_t id, const GWinForm* form)
{
#if defined(_MSC_VER)
    lock_guard<std::mutex> lk(m_mutex);
#else
    if (m_mutex)
        osMutexWait(m_mutex, osWaitForever);
#endif
    if (form)
        m_forms[id] = form;

#ifndef _MSC_VER
    if (m_mutex)
        osMutexRelease(m_mutex);
#endif
}

void VFormManager::destroyCurrentForm(const void* para)
{
    if (m_currentForm && m_currentForm->pClose) {
        m_currentForm->pClose(para);
    }
    m_currentForm = nullptr;
    m_currentFormId = 0;
}

void VFormManager::createAndShowForm(uint32_t id, const void* para)
{
    auto it = m_forms.find(id);
    if (it == m_forms.end())
        return;
    const GWinForm* form = it->second;
    // 初始化 -> 显示
    if (form->pInit)
        form->pInit(para);
    if (form->pShow)
        form->pShow(para);
    m_currentForm = form;
    m_currentFormId = id;
    FGUIState.pCurForm = form;
    FGUIState.uCurFormID = id;
}

void VFormManager::SwitchForm(uint32_t id, const void* para)
{
#if defined(_MSC_VER)
    lock_guard<std::mutex> lk(m_mutex);
#else
    if (m_mutex)
        osMutexWait(m_mutex, osWaitForever);
#endif

    // 如果目标与当前相同，仍按策略销毁重建
    destroyCurrentForm(para);
    createAndShowForm(id, para);

#ifndef _MSC_VER
    if (FGUIState.mutexGUI)
        osMutexRelease(FGUIState.mutexGUI);
#endif
}

void VFormManager::SendMessage(uint32_t msgIdent, uint32_t param, uint32_t data)
{
    // 构造 GM_MESSAGE（若存在）并投递给当前窗体的消息处理函数
    if (!m_currentForm || !m_currentForm->pMsg)
        return;
    GM_MESSAGE msg = { 0 };
    msg.MsgId  = msgIdent;
    msg.Param  = param;
    msg.Data.v = data;
    m_currentForm->pMsg(&msg);
}

void VFormManager::ShowFatalMessage(const char* msg)
{
    // 简单实现：打印并尝试显示当前窗体以外的致命信息窗体
    // 实际工程应切换到错误画面或弹出对话
    if (msg) {
        // 在仿真/主机上先输出
        printf("FATAL: %s\n", msg);
    }
    // TODO: 切换到专门的致命错误窗体（若有注册）
}

void VFormManager::Init()
{

#ifndef _MSC_VER
#if osCMSIS >= 0x20000U
    FGUIState.mutexGUI = osMutexNew(nullptr);
#else
    FGUIState.mutexGUI = osMutexCreate(nullptr);
#endif
#endif
}

void VFormManager::Run()
{
    // 简化：如果存在一个启动窗体（如 WID_SplashForm），则切换到它
    // 否则不做事。实际应是个循环或被 GUI 任务驱动。
    if (m_forms.count(WID_SplashForm)) {
        SwitchForm(WID_SplashForm, nullptr);
    }
}

void VFormManager::ModeChanged()
{
    //// 更新全局状态并通知当前窗体（如需要）
    //if (m_currentForm && m_currentForm->pMsg) {
    //    GM_MESSAGE msg;
    //    msg.msgId = 0x80000000; // 自定义模式改变消息
    //    msg.wParam = FGUIState.ucMode;
    //    msg.lParam = 0;
    //    m_currentForm->pMsg(&msg);
    //}
}

void VFormManager::KeyEvent(uint32_t key, uint32_t pressedCnt)
{
    // 转发按键事件给当前窗体
    if (m_currentForm && m_currentForm->pMsg) {

            uint32_t uMsgId;
        if (0 == key)
            uMsgId = 0;
        else if (0 == pressedCnt)
            uMsgId = GM_KEYUP;
        else if (1 == pressedCnt)
            uMsgId = GM_KEYDOWN;
        else
            uMsgId = GM_KEYPRESS;

    if (0 != uMsgId) {
            GM_MESSAGE msg;
            msg.MsgId  = uMsgId;
            msg.Param  = key;
            msg.Data.v = pressedCnt;
            m_currentForm->pMsg(&msg);
        }
    }
}
