//-----------------------------------------------------------------------------
/*
 File        : FormManager.h
 Version     : V1.10
 By          : 银网科技
 Description : GUI主控

 Date       : 2026.6.8
*/
//-----------------------------------------------------------------------------
#ifndef GUI_FormManager_H
#define GUI_FormManager_H

#include "GUICntr.h"
#include "GWinTypes.h"

#include <cstdint>
#include <unordered_map>

#if defined(_MSC_VER)
#include <mutex>
#else
#include <cmsis_os.h>
#endif

//=============================================================================
#if defined(__cplusplus)
extern "C" { // Make sure we have C-declarations in C++ programs
#endif
//=============================================================================
// 全局宏
//-----------------------------------------------------------------------------

//=============================================================================
// 数据结构
//-----------------------------------------------------------------------------
class VFormManager {
public:
    static VFormManager& Instance();

    // 注册窗体（程序启动时由各窗体模块调用）
    void RegisterForm(uint32_t id, const GWinForm* form);

    // 切换窗体（按注释：销毁当前，创建新窗体）
    void SwitchForm(uint32_t id, const void* para);

    // 向当前窗体发送消息（或全局投递）
    void SendMessage(uint32_t msgIdent, uint32_t param, uint32_t data);

    // 显示致命错误（阻塞/显示错误界面）
    void ShowFatalMessage(const char* msg);

    // GUI 启动/主循环入口（简化）
    void Init();
    void Run(); // GUICenter 的实现点

    // 模式/按键事件处理钩子
    void ModeChanged();
    void KeyEvent(uint32_t key, uint32_t pressedCnt);

private:
    VFormManager();
    ~VFormManager();

    void destroyCurrentForm(const void* para);
    void createAndShowForm(uint32_t id, const void* para);

    const GWinForm* m_currentForm   = nullptr;
    uint32_t        m_currentFormId = 0;
    std::unordered_map<uint32_t, const GWinForm*> m_forms;

#if defined(_MSC_VER)
    std::mutex m_mutex;
#else
    osMutexId  m_mutex;
#endif
}; //=============================================================================
#if defined(__cplusplus)
}
#endif
//=============================================================================
#endif // GUI_FormManager_H
