# AGENTS.md — Window Manager (OpenHarmony)

## 窗口动效：应用拉起应用 — 拉起入口与冷热启动分流

统一架构（window_scene）下，应用A 通过 startAbility 拉起应用B，WMS 侧主链路按序经过：

1. `window_scene/session/host/src/zidl/session_proxy.cpp` — SessionProxy::PendingSessionActivation，IPC code TRANS_ID_ACTIVE_PENDING_SESSION（Ability Manager Service → root/前台A 的 session），是拉起事件进入 WMS 的 IPC 入口；热启动/mission 前台化才走 ISceneSessionManagerLite 的 PendingSessionToForeground。
2. `window_scene/session/host/src/scene_session.cpp` — SceneSession::PendingSessionActivation：校验 PERMISSION_MANAGE_MISSION、标记 startMethod 为 START_CALL、提取 caller 信息，再经 NAPI 桥抛给 SceneBoard。排查"谁拉起谁"看 callerBundleName_/callerToken_；callerPersistentId_ 多数场景为 INVALID_SESSION_ID，不是可靠拉起者标识。
3. `window_scene/session_manager/src/scene_session_manager.cpp` — RequestSceneSessionActivationInner 做冷/热启动分流：session 已存在且已连接（典型：拉起后台B）直接 NotifySessionForeground；否则经 StartUIAbilityBySCB 让 Ability Manager Service 拉起进程（sceneFlag 恒为 ABILITY_CALL，带 XCollie 5 秒超时监控）。

常见误判：统一架构下 WMS 不执行 open/close 转场动画（SetWindowAnimationController 等接口为 no-op）；动画执行在关联仓 SceneBoard 的 SCBScenePanelAnimator。不要到分离架构目录（wmserver 的 RemoteAnimation/StartingWindow）里找统一架构的动效实现。全链路时序、日志/trace 关联表与最短定位决策树见文末"专项知识文档"。

## 窗口动效：应用拉起应用 — 启动窗、转场执行边界与首帧衔接

- `wm/src/window_scene_session_impl.cpp` — 客户端侧：应用主动移除启动窗 Window::NotifyRemoveStartingWindow（要求模块配置 enable.remove.starting.window）；热启动时 window->Show(reason, withAnimation) 直连前台化。
- `window_scene/session/host/src/session.cpp` — 服务端侧：Session::RemoveStartingWindow 遍历 ILifecycleListener 通知，是本仓启动窗移除链路终点；SetBufferAvailable 经 bufferAvailableChange 向 SCB 发首帧就绪事件（第二参数 startWindowInvisible 只做"内容已加载"通知，不代表启动窗已移除）；RETAIN_AND_INVISIBLE 时 SetLeashWindowAlpha 将 leash alpha 置 0。
- `window_scene/session_manager/include/zidl/scene_session_manager_interface.h` — 职责边界证据：SetWindowAnimationController/NotifyWindowTransition/GetWindowAnimationTargets 为 no-op 默认实现，统一架构 WMS 不执行转场动画、不组装远程动画 target。
- 边界：启动窗类型决策与启动页资源查找在服务端（StartWindowType 三值、四级查找）；启动窗 UI 绘制与退出动画执行在关联仓 arkui_ace_engine 的 WindowScene 组件；转场动画编排与执行在关联仓 window_scene_board（作用于 ArkUI 容器属性，非 RS leash）。在本仓与 scene_board 都找不到启动窗绘制代码属预期。

## 仓库概览

This repo is the window management subsystem of OpenHarmony. It provides core capabilities for window and display management, serving as the foundational subsystem for UI display. See [README.md](./README.md) for full architecture details.

## What is `window_manager`

`window_manager` is a `Client-Server` architecture subsystem. Key modules:
- `wm/` — Window Manager Client
- `dm/` — Display Manager Client
- `wmserver/` — Window Manager Server (separated arch)
- `dmserver/` — Display Manager Server
- `window_scene/` — Window Manager Server (unified arch)
- `interfaces/innerkits/` — Native APIs
- `interfaces/kits/` — JS/NAPI APIs
- `extension/` — Ability Component window integration
- `utils/` — Shared utilities

Two compile-time architectures are selected via `window_manager_use_sceneboard` (set externally by the OpenHarmony build system):

| Value | Architecture | Key module compiled |
|-------|-------------|---------------------|
| `false` | Separated | `wmserver/` |
| `true` | Unified | `window_scene/` |

## WorkFlow

### CodeStyle specification
You must follow the [CodeStyle](./docs/CodeStyle.md) specification when you write code.

Key rules:
- **C++ standard**: C++11; **Column limit**: 120; **Indent**: 4 spaces, no tabs
- **Namespace**: always `OHOS::Rosen`
- **Naming**: Classes `PascalCase`, methods `camelCase`, member vars `camelCase_`, constants `UPPER_SNAKE_CASE`, files `snake_case`
- **Include order**: corresponding header → stdlib → OH framework → `interfaces/` → other internal
- **Logging**: `TLOGD` / `TLOGI` / `TLOGW` / `TLOGE`
- **Error handling**: return `WMError`/`WmErrorCode`; early-return with `WLOGFE` log
- **Memory**: `sptr<T>` for IPC/singletons, `wptr<T>` for weak refs; prefer RAII
- Every source file must begin with the Apache 2.0 license header

### Testing specification
You must follow the [Testing](./docs/Testing.md) specification after you write testing code and keep testing suite pass.

### Git Commit Rules

- **User approval required**: Ask user before `git commit`. Use `git commit -s` after approval.
- **Angular format**: `type(scope): subject` (feat, fix, docs, style, refactor, test, chore)
- **Co-authored footer**: Append `Co-Authored-By: Agent` to every commit message.

Example:
```
feat(auth): add user login feature

Signed-off-by: Your Name <your.email@example.com>
Co-Authored-by: Agent
```

## 专项知识文档

深度专项知识位于 docs/knowledge/，处理对应场景前建议先读：

- [应用拉起应用动效：拉起链路、启动窗与转场动画](docs/knowledge/app-launch-app-animation.md) — 应用A拉起应用B 的窗口动效全链路：IPC 入口、caller 记录、冷热启动分流、启动窗决策与移除、SCB 转场动画、首帧衔接、焦点推进，含日志/trace 关联表、常见误判与最短定位决策树。
- [沉浸式窗口状态机](docs/knowledge/immersive-window-overview.md)
- [沉浸式系统栏](docs/knowledge/immersive-system-bars.md)
- [避让区布局与 rect 通知时序](docs/knowledge/immersive-avoid-area-layout-timing.md)
- [避让区基础](docs/knowledge/immersive-avoid-area.md)
- [沉浸式调试与测试](docs/knowledge/immersive-debugging-and-tests.md)
- [兼容性说明](docs/knowledge/compatible.md)
