# GForm 表单系统技术文档

> 版本：V1.02  
> 日期：2026.06.24  
> 作者：Wey. Silver Grid

---

## 1. 背景

SG1210 固件原有**两套不完整的 GUI Form 管理系统**：

| 系统 | 文件 | 问题 |
|------|------|------|
| GUICntr | `GUICntr.cpp/h` | 栈式管理，表单硬编码在 `GFormsList[]`，`MAX_FORMS=8`，扩展性差 |
| FormManager | `FormManager.cpp/h` | 半成品单例，`unordered_map` 动态注册，无栈无 Tick，引用不存在的字段 |

**GForm 统一系统**替换上述两者，提供干净的 C++20 namespace 风格 API。

---

## 2. 架构设计

```
┌─────────────────────────────────────────────────┐
│  调用层                                          │
│  WinMain.cpp  ──→  gform::KeyEvent()             │
│  GUIMain.cpp  ──→  gform::Init/Tick/PushForm()   │
│  各 Form.cpp  ──→  gform::PushForm/PopForm/SendMsg│
│                ──→  FormRegistrar (静态自注册)     │
├─────────────────────────────────────────────────┤
│  引擎层                                          │
│  gform::                                         │
│  ├─ Init()         导航栈 + 消息队列初始化         │
│  ├─ RegisterForm() 动态注册 (32槽，保留静态注册)    │
│  ├─ PushForm()     压栈 → pInit → pShow          │
│  ├─ ReplaceForm()  弹出旧 → 压入新               │
│  ├─ PopForm()      弹出 → 恢复前一个              │
│  ├─ SendMsg()      向栈顶表单发送消息              │
│  ├─ PostMsg()      延迟消息 (下个 Tick 投递)       │
│  ├─ Tick()         10ms 定时 + 消息队列排空        │
│  └─ KeyEvent()     按键翻译 → SendMsg             │
├─────────────────────────────────────────────────┤
│  平台层                                          │
│  GFormPlatform.h                                 │
│  ├─ platform::Lock   (std::mutex / osMutexId)     │
│  └─ ScopedLock       RAII 自动加锁/解锁           │
├─────────────────────────────────────────────────┤
│  注册层                                          │
│  GFormRegistrar.h  (RAII 静态构造 → RegisterForm) │
└─────────────────────────────────────────────────┘

  ~~ GUICntr.cpp 适配层 ~~  已不再被任何活跃代码调用
```

### 核心设计决策

| 决策 | 理由 |
|------|------|
| **namespace 而非 singleton** | 无构造/析构顺序问题，语义最简 |
| **固定数组而非动态容器** | 无堆分配，适合嵌入式 Cortex-M4 |
| **保留 GWinForm 4函数指针** | 现有表单无需修改结构体声明 |
| **PlatformLock 抽象** | 业务逻辑零 `#ifdef __vmSIMULATOR__` |
| **FormRegistrar RAII** | 静态初始化自动注册，增删 .cpp 自动生效 |
| **栈式导航 (push/replace/pop)** | 适合嵌入式 UI 层级结构 |
| **Init() 不清空注册表** | 静态初始化期注册的表单在 Init() 后依然有效 |

---

## 3. 文件变更清单

### 3.1 新建文件

| 文件 | 用途 | 状态 |
|------|------|------|
| `Application/GUI/GForm.h` | 公共 API + WID_ 常量定义 | ✅ |
| `Application/GUI/GForm.cpp` | 引擎实现 — 注册表(32槽), 调用栈(16层), 消息队列, Tick, 导航 | ✅ |
| `Application/GUI/GFormPlatform.h` | 平台互斥锁抽象 + RAII ScopedLock | ✅ |
| `Application/GUI/GFormRegistrar.h` | FormRegistrar 类 (include GWinTypes.h, 非 forward decl) | ✅ |

### 3.2 修改文件

| 文件 | 变更内容 | 状态 |
|------|----------|------|
| **Phase 1 — 基础架构** | | |
| `GUICntr.cpp` | 重写为适配层，`GUI*` 函数转发到 `gform::`；`s_formTable[]` 含哨兵条目；`#include` 精简 | ✅ |
| `GUIMessage.h` | 新增 `GM_FORM_ACTIVATED`(18) / `GM_FORM_DEACTIVATED`(19) | ✅ |
| `Sim/SG1210Sim.vcxproj` | 项目重命名；添加 GForm.cpp；输出 SG1210D→SG1210Sim；修复 post-build | ✅ |
| `Sim/SG1210Sim.vcxproj.filters` | 过滤列表添加 GForm.cpp → GUI\Core | ✅ |
| `Sim/SG1210Sim.sln` | 项目引用 SimulationTrial → SG1210Sim | ✅ |
| **Phase 2 — 表单自注册** | | |
| `GPSplashForm.cpp` | 添加 `#include "GFormRegistrar.h"` + `FormRegistrar kRegSplash(WID_SplashForm, ...)` | ✅ |
| `GPMenuForm.cpp` | 同上 `kRegMenu(WID_MenuForm, ...)` | ✅ |
| `GPMainForm.cpp` | 同上 `kRegMain(WID_MainForm, ...)` | ✅ |
| `GPEventBrowserForm.cpp` | 同上 `kRegEvents(WID_EventListForm, ...)` | ✅ |
| `GFormRegistrar.h` | 修复：`#include "GWinTypes.h"` 替代 `struct GWinForm;` forward decl (typedef struct 冲突) | ✅ |
| `GForm.cpp` | 修复：`Init()` 不再清空注册表（保留 FormRegistrar 静态初始化期注册） | ✅ |
| **Phase 3 — 调用点迁移** | | |
| `GUIMain.cpp` | `GUIStart()+GUICenter()` → `gform::Init()`+emWin init+`gform::PushForm()`+`gform::Tick()` 循环 | ✅ |
| `WinMain.cpp` | `#include "GUICntr.h"` → `#include "GForm.h"`；`GUIKeyEnevt()` → `gform::KeyEvent()` | ✅ |
| `GForm.h` | 新增 WID_ 常量（`#ifndef WID_FORMBEGIN` 防护），调用方无需 include GUICntr.h | ✅ |
| 4个 Form.cpp | `#include "GUICntr.h"` → `#include "GForm.h"`；`GUIFormOpen()`→`gform::PushForm()`；`GUIFormClose()`→`gform::PopForm()`；`GUISendMessage()`→`gform::SendMsg()` | ✅ |
| **Phase 5 — CSG 图像编解码注册** | | |
| `GUIPicture.cpp` | 新增 `#include "CSGDecoder.h"` + `case ID_CSG:` 解码分支 | ✅ |
| `GPSplashForm.cpp` | 新增 `#include "Graphics/x128.h"` + 居中绘制 `picx128csg` | ✅ |

### 3.3 不再使用的文件（待删除）

| 文件 | 状态 | 原因 |
|------|------|------|
| `Application/GUI/GUICntr.cpp` | 🔴 冗余 | 适配层已无活跃调用方，仅保留 `KEYDB_OnChanged`/`IRKBD_Received` 等真机函数 |
| `Application/GUI/GUICntr.h` | 🔴 冗余 | WID_ 常量已迁入 GForm.h，所有 include 已替换 |
| `Application/GUI/FormManager.cpp` | 🔴 废弃 | 半成品，已被 GForm 完全替代 |
| `Application/GUI/FormManager.h` | 🔴 废弃 | 同上 |

### 3.4 未受影响的文件

- `GWinTypes.h` — `GWinForm` 结构体定义保持不变
- `GUIMessage.h` — 仅追加2个消息宏，其余不变
- `GUI.h`, `GUIConf.h`, `LCDConf.h`, `GUIBitMap.h` 等 emWin 层
- `PictureRes.cpp`, `x128.cpp` — 图形资源
- 所有非 GUI 模块 (Device/, System/, Mics/, Drivers/, Middlewares/)

---

## 4. API 参考

### 4.1 类型定义

```cpp
using FormId = uint16_t;

enum class FormTransition : uint8_t {
    kPush,      // 压栈并显示
    kReplace,   // 弹出当前 → 压入并显示
    kPop        // 弹出当前 → 恢复前一个
};

struct FormRecord {
    const GWinForm* callbacks;   // Init/Show/Close/Msg 四个回调
    const char*     name;        // 调试名称 (UTF-8)
    uint16_t        flags;       // 动画/过渡标志位 (预留)
};

// ID 常量
constexpr uint16_t WID_SplashForm     = 100;
constexpr uint16_t WID_MenuForm       = 101;
constexpr uint16_t WID_MainForm       = 102;
constexpr uint16_t WID_EventListForm  = 103;
// ... 等 (定义在 GForm.h，与旧 GUICntr.h 兼容)
constexpr uint16_t kFormIdInvalid = 0xFFFF;

// 容量限制
constexpr size_t kMaxForms = 32;   // 注册表容量
constexpr size_t kMaxStack = 16;   // 导航栈深度
```

### 4.2 生命周期

```cpp
void gform::Init();          // 初始化导航栈和消息队列 (不清空注册表)
void gform::Tick();          // 10ms Tick — 排空延迟消息 + 投递 GM_TIMER_TICK
size_t gform::Run();         // 返回当前栈深度
```

### 4.3 注册

```cpp
void gform::RegisterForm(FormId id, const GWinForm* form, const char* name);
const FormRecord* gform::FindForm(FormId id);
void gform::UnregisterForm(FormId id);

// RAII 自动注册 — 在 Form.cpp 文件作用域使用
#include "GFormRegistrar.h"
static const gform::FormRegistrar kReg(WID_SPLASH, &FSplashForm, "Splash");
// FormRegistrar 构造函数在 main() 前调用 RegisterForm()
// gform::Init() 不清空注册表，故注册在 Init() 后依然有效
```

### 4.4 导航

```cpp
void gform::PushForm(FormId id, const void* para = nullptr);
void gform::ReplaceForm(FormId id, const void* para = nullptr);
void gform::PopForm();                          // 忽略参数，弹出后恢复前一个
void gform::CloseCurrentForm();
FormId gform::GetCurrentFormId();
```

### 4.5 消息

```cpp
void gform::SendMsg(uint16_t msgId, uint16_t param = 0, int32_t value = 0);
void gform::SendMsgPtr(uint16_t msgId, uint16_t param, const void* data);
void gform::PostMsg(uint16_t msgId, uint16_t param = 0, int32_t value = 0);
void gform::BroadcastMsg(uint16_t msgId, uint16_t param = 0, int32_t value = 0);
void gform::KeyEvent(uint32_t key, uint32_t pressedCnt);
```

> **命名**: `SendMsg` 而非 `SendMessage`；`PostMsg` 而非 `PostMessage`——避免 `<Windows.h>` 的 `SendMessageA/W`、`PostMessageA/W` 宏冲突。

### 4.6 栈检查

```cpp
size_t gform::StackDepth();
bool gform::IsStackEmpty();
FormId gform::GetStackForm(size_t depth);   // 0=栈顶
bool gform::IsFormOnStack(FormId id);
```

---

## 5. 迁移路线

### Phase 1 — 基础架构 ✅ (2026.06.24)

- 创建 GForm.h / GForm.cpp / GFormPlatform.h / GFormRegistrar.h
- GUICntr.cpp 变为适配层，所有 GUI* 函数转发到 gform::
- SG1210Sim.vcxproj 添加 GForm.cpp，项目重命名
- 模拟器编译通过并正常运行

### Phase 2 — 表单自注册 ✅ (2026.06.24)

- 4个活跃表单 .cpp 添加 FormRegistrar 静态注册器
- GFormRegistrar.h 修复：include GWinTypes.h（替代 forward decl）
- GForm::Init() 修复：不清空注册表（保留静态初始化期注册）
- GUICntr::s_formTable[] 清空为哨兵

### Phase 3 — 调用点迁移 ✅ (2026.06.24)

- GUIMain.cpp: GUIStart()+GUICenter() → gform::Init()+gform::Tick()
- WinMain.cpp: GUIKeyEnevt() → gform::KeyEvent()
- GForm.h: 新增 WID_ 常量
- 4个 Form.cpp: #include → GForm.h, API → gform::
- GUICntr.h 不再被任何活跃 sim 源文件 include

### Phase 4 (待执行) — 清理

- [ ] 删除 `GUICntr.cpp/h`（确认真机代码中 KEYDB_OnChanged 等迁移完毕）
- [ ] 删除 `FormManager.cpp/h`
- [ ] 旧文件 GB2312 → UTF-8 BOM 批量转换
- [ ] Keil MDK 真机编译验证
- [ ] 非活跃表单（GPConfigForm, GPGuageForm 等）迁移到 FormRegistrar

### Phase 5 — CSG 图像编解码注册 ✅ (2026.06.24)

- GUIPicture.cpp: `GUI_DrawPicture()` 新增 `case ID_CSG:` → `CSGDecoder::DecodePicture()` → RGBA 像素绘制
- GPSplashForm.cpp: 启动画面居中绘制 `picx128csg` (128×128 CSG 测试图像)

---

## 6. 当前文件依赖关系

```
WinMain.cpp  ──→  GForm.h ──→  GWinTypes.h + GUIMessage.h
GUIMain.cpp  ──→  GForm.h ──→  GFormPlatform.h (mutex)
                     │
GPSplashForm.cpp ──→ GFormRegistrar.h ──→ GWinTypes.h
GPMenuForm.cpp    ──→ GFormRegistrar.h
GPMainForm.cpp    ──→ GFormRegistrar.h
GPEventBrowser.cpp──→ GFormRegistrar.h
                     │
               (静态构造 → RegisterForm → 注册表)

GUICntr.cpp/h ........ 不再被任何活跃 sim 源文件 include (冗余)
FormManager.cpp/h .... 半成品 (废弃)
```

---

## 7. 新旧对比

| 特性 | GUICntr (旧) | FormManager (旧) | GForm (新) |
|------|-------------|-------------------|------------|
| 注册方式 | 硬编码 `GFormsList[]` | `unordered_map` 动态注册 | 固定数组 + FormRegistrar RAII |
| 表单上限 | 8 | 无限制 (堆分配) | 32 (可调) |
| 导航模式 | 仅 Push | 仅 Switch (销毁重建) | Push / Replace / Pop |
| 消息队列 | 无 | 无 | PostMsg 延迟队列 (16槽) |
| 平台适配 | `#ifdef` 遍布业务逻辑 | 同左 | PlatformLock 单头文件抽象 |
| 内存分配 | 静态 | 堆 (`unordered_map`) | 静态 (`std::array`) |
| Tick 机制 | 空闲超时 + 定时器 | 缺失 | 消息队列排空 + GM_TIMER_TICK |
| API 风格 | C 函数 + 全局状态 | 单例类 | namespace + 匿名空间内部状态 |
| 编码 | GB2312 | UTF-8 | UTF-8 with BOM |
| Windows 兼容 | 直接使用 SendMessage | 同左 | SendMsg/PostMsg 避免宏冲突 |
| 静态初始化安全 | N/A | N/A | FormRegistrar 在 main() 前注册，Init() 保留 |

---

## 8. CSG 图像编解码集成

### 8.1 背景

CSG (Compact Scalable Graphic) 是银网科技自研的图像压缩格式（v1.5），编解码器位于 `Middlewares/emWin/csgimage/`。解码器在模拟器构建中已编译，但此前未注册到 emWin 的图像绘制管线。

### 8.2 集成方式

通过 `GUI_DrawPicture()` 调度函数接入，无需修改 emWin 内核：

```cpp
// GUIPicture.cpp — GUI_DrawPicture()
void GUI_DrawPicture(const TGUIPicture *pPic, int x0, int y0) {
    switch (pPic->Type) {
        case ID_BITMAP: LCDX_Bitmap_Draw(...);  break;   // 已有
        case ID_CSG:    /* CSG decode + draw */  break;   // 新增
        case ID_JPG:    GUI_JPEG_Draw(...);      break;   // 已有
    }
}
```

### 8.3 数据流

```
TGUIPicture (Type=ID_CSG, pData=CSG bytes, Size=N)
  │
  ▼  GUI_DrawPicture(&pic, x0, y0)
  │
  ▼  case ID_CSG:
CSGDecoder::DecodePicture(data, len)
  │  ① 解析 CSGPicture header (16B): magic "Vx", width, height, colorMode, CRN, CAS
  │  ② CRC16 校验
  │  ③ 根据 CAS 选择解压器: RLE / MiniLZ77 / Huffman / DEFLATE(MiniLZ77+Huffman)
  │  ④ 调色板索引 → CRM 原始色深数据
  │  ⑤ CRM → RGBA 转换 + 透明色处理 (CRI=0 或 palette[0] 匹配)
  ▼
DecoderResult { .pixels=RGBA[W*H*4], .width, .height, .colorMode, ... }
  │
  ▼  逐像素遍历:
  │    if (alpha > 0) { GUI_SetColor(r,g,b); GUI_DrawPixel(x0+x, y0+y); }
  ▼
LCD 显示
```

### 8.4 修改文件

| 文件 | 变更 |
|------|------|
| `Middlewares/emWin/user/GUIPicture.cpp` | 新增 `#include "CSGDecoder.h"` + `case ID_CSG:` 分支 |
| `Application/GUI/GPSplashForm.cpp` | 新增 `#include "Graphics/x128.h"` + `GUI_DrawPicture(&picx128csg, ...)` 测试绘制 |

### 8.5 支持的 CSG 格式

| 参数 | 支持范围 |
|------|----------|
| 色深 (ColorMode) | RGB565, BGR565, RGB666, RGB888, RGB8888 |
| 调色板 (CRN) | 0 (真彩), 2, 4, 8, 16, 32, 64, 128 |
| 压缩 (CAS) | None, RLE, MiniLZ77, Huffman, DEFLATE |
| 压缩等级 (CAL) | 0–7 |
| 分辨率 | 10×10 到 256×256 |
| 图集 (Atlas) | 支持多图 CSG 文件 |

### 8.6 使用示例

```cpp
#include "GUIPicture.h"
#include "Graphics/x128.h"   // 声明 extern const TGUIPicture picx128csg;

// 在表单 _Show() 或 _UpdateGraph() 中：
GUI_DrawPicture(&picx128csg, x, y);
```

---

## 9. 踩坑记录

### 9.1 `struct` vs `typedef struct` 前向声明冲突

**问题**: `GWinForm` 定义为 `typedef struct tagGWinForm { ... } GWinForm;`，使用 `struct GWinForm;` 前向声明会导致 MSVC 报 `C2371: 重定义；不同的基类型`。

**修复**: 在 `GForm.h` 和 `GFormRegistrar.h` 中直接 `#include "GWinTypes.h"`，不使用前向声明。

### 9.2 Windows API 宏冲突

**问题**: `<Windows.h>` 将 `SendMessage` 宏展开为 `SendMessageA`，导致 `gform::SendMessage()` → `gform::SendMessageA()`，编译失败。`PostMessage` 同理。

**修复**: 重命名为 `gform::SendMsg()` / `gform::PostMsg()`。

### 9.3 静态初始化顺序

**问题**: `FormRegistrar` 在 `main()` 前构造调用 `RegisterForm()`，但 `gform::Init()` (在 `main()` 后调用) 原本会清零注册表。

**修复**: `Init()` 不再清零注册表，仅重置导航栈和消息队列。

### 9.4 空数组编译错误

**问题**: `s_formTable[] = {}` 在 MSVC 中产生零大小数组，报 `C2466: 不能分配常量大小为 0 的数组`。

**修复**: 添加哨兵 `{ 0, nullptr, nullptr }` + `RegisterAllForms()` 中 null 检查。

### 9.5 GB2312/UTF-8 编码与 C4819 警告

**问题**: 旧文件使用 GB2312 编码，MSVC 代码页 936 报 C4819 警告。`sed` 可能破坏编码导致编译错误。

**修复**: 新文件统一使用 UTF-8 with BOM。旧文件编码转换留待 Phase 4。

### 9.6 VS 项目 post-build 路径错误

**问题**: Post-build 步骤 `copy ShivaVG.dll exe\` 目标目录 `exe\` 不存在。

**修复**: 改为 `copy ShivaVG.dll Build\`。
