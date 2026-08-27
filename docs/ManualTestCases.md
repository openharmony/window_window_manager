# AllUsersWindowManager 手工测试用例

## 测试范围与约定

**被测特性**：车机多前台用户场景下的窗口客户端聚合查询与聚合监听

**userId约定**：主驾=100，副驾=101，后排/新增用户依次+1

**测试方法**：Risk-Based Testing + State Transition + EP/BVA + Concurrency + Fault Injection

**前置公共条件**：
- `persist.dms.concurrentuser=true`
- 调用方进程为 u0 系统服务（如 AccessibilityService）
- 所有调用均通过 `AllUsersWindowManager::GetInstance()` 接口

---

## P0 — 冒烟测试（阻塞发布）

### P0-1 双用户在线 — 聚合查询基本功能

**车机场景**：主驾和副驾同时使用各自的窗口界面，Accessibility 需要同时感知两个用户空间的窗口状态

**前置条件**：userId=100、101 已上线，各拥有可见窗口（100有2个窗口，101有1个窗口）

**操作步骤**：
1. 调用 `GetActiveUserIds()` 记录返回值
2. 调用 `GetVisibilityWindowInfo(infos)` 记录返回值和 infos 内容
3. 调用 `GetAccessibilityWindowInfo(infos)` 记录返回值和 infos 内容
4. 检查日志中 "GetVisibilityWindowInfo users: 2" 和 "GetAccessibilityWindowInfo users: 2"

**预期结果**：
- Step 1: 返回 `{100, 101}`
- Step 2: 返回 WM_OK，infos 包含3条记录（100×2 + 101×1），每条 WindowVisibilityInfo 可区分所属用户
- Step 3: 返回 WM_OK，包含两个用户的 AccessibilityWindowInfo
- 无崩溃、无卡死

---

### P0-2 聚合监听 — 焦点与可见性回调接收

**车机场景**：Accessibility 注册一个全局监听器，主驾和副驾的窗口焦点/可见性变化都应触发同一个回调

**前置条件**：userId=100、101 已上线

**操作步骤**：
1. 注册全局监听器：
   ```
   RegisterFocusChangedListener(myFocusListener)
   RegisterVisibilityChangedListener(myVisListener)
   ```
2. userId=100 的窗口获得焦点 → 检查 myFocusListener 回调
3. userId=101 的窗口获得焦点 → 检查 myFocusListener 回调
4. userId=100 的窗口可见性变化 → 检查 myVisListener 回调
5. userId=101 的窗口可见性变化 → 检查 myVisListener 回调
6. 反注册：
   ```
   UnregisterFocusChangedListener(myFocusListener)
   UnregisterVisibilityChangedListener(myVisListener)
   ```
7. 再次触发焦点/可见性变化 → 确认不再收到回调

**预期结果**：
- Step 2-3: myFocusListener 两次回调均触发，FocusChangeInfo 中窗口信息正确
- Step 4-5: myVisListener 两次回调均触发
- Step 7: 不再收到任何回调

---

### P0-3 单用户模式降级 — 回归不破坏

**车机场景**：非车机设备或 `persist.dms.concurrentuser=false`，AUWM 所有接口应退化为单实例行为，与原 WindowManager 完全一致

**前置条件**：`persist.dms.concurrentuser=false`，userId=100 单用户在线

**操作步骤**：
1. 调用 `GetVisibilityWindowInfo(infos)` → 检查日志出现 "single instance mode"
2. 调用 `RegisterFocusChangedListener(listener)` → 检查日志出现 "single instance mode"
3. 对比直接调用 `WindowManager::GetInstance().GetVisibilityWindowInfo(infos2)` 的结果
4. 触发焦点变化 → 对比回调行为与直接注册 `WindowManager::GetInstance().RegisterFocusChangedListener(listener2)` 一致

**预期结果**：
- Step 1-2: 日志出现 "single instance mode"
- Step 3: infos 与 infos2 完全一致
- Step 4: 回调行为一致
- 原有单用户场景不受任何影响

---

## P1 — 核心场景验证

### P1-1 用户添加 — 监听器自动传播到新用户实例

**车机场景**：后排乘客上车（userId=102上线），Accessibility 已注册的全局监听器应自动注册到新用户空间，无需手动重新注册

**前置条件**：userId=100、101在线，Accessibility已注册 myFocusListener 和 myVisListener

**操作步骤**：
1. userId=102 上线（触发 OnWMSConnectionChanged → UpdateActiveUserIds(102, true) → OnUserAdded(102)）
2. 等待回调链完成，检查日志中 "Register global focus listener for user 102" 和 "Register global visibility listener for user 102"
3. 调用 `GetActiveUserIds()` → 确认包含 {100, 101, 102}
4. userId=102 创建窗口并获得焦点 → 检查 myFocusListener 是否收到 userId=102 的焦点回调
5. userId=102 窗口可见性变化 → 检查 myVisListener 是否收到回调
6. 调用 `GetVisibilityWindowInfo(infos)` → 确认包含3个用户的窗口数据

**预期结果**：
- Step 2: OnUserAdded 从 focusListeners_ 拷贝并注册到 WindowManager(102)
- Step 4-5: 全局监听器收到 userId=102 的回调
- Step 6: infos 包含3个用户的所有窗口

---

### P1-2 用户删除 — 下线用户数据从聚合结果中移除

**车机场景**：后排乘客下车（userId=102下线），聚合查询和回调应不再包含该用户的数据

**前置条件**：userId=100、101、102在线，Accessibility已注册全局监听器

**操作步骤**：
1. userId=102 下线（OnWMSConnectionChanged isConnected=false）
2. 等待 UpdateActiveUserIds(102, false) 完成 → OnUserRemoved(102)
3. 调用 `GetActiveUserIds()` → 确认返回 {100, 101}，不含102
4. 调用 `GetVisibilityWindowInfo(infos)` → 确认不含 userId=102 的窗口数据
5. userId=102 空间内窗口事件 → 确认 myFocusListener/myVisListener 不再收到102的回调

**预期结果**：
- Step 3: 返回 {100, 101}
- Step 4: infos 只包含100和101的窗口
- Step 5: 不再收到102的回调
- OnUserRemoved 当前只打日志（TODO），服务端自行管理 agent 生命周期

---

### P1-3 用户下线后重新上线 — 监听器恢复

**车机场景**：副驾短暂离车后返回（101下线再上线），全局监听器应重新自动注册到新的 WindowManager 实例

**前置条件**：userId=100、101在线，Accessibility已注册全局监听器

**操作步骤**：
1. userId=101 下线 → GetActiveUserIds 返回 {100}
2. 调用 `GetVisibilityWindowInfo` → 只包含100的窗口
3. userId=101 再次上线（触发 OnUserAdded(101)）
4. 检查日志：全局监听器重新注册到 WindowManager(101)
5. userId=101 创建窗口触发事件 → 验证 myFocusListener/myVisListener 回调恢复
6. 调用 `GetVisibilityWindowInfo` → 包含100和101的窗口

**预期结果**：
- Step 1-2: 数据和回调只包含100
- Step 4-5: 监听器自动恢复，回调正常触发
- Step 6: 聚合查询恢复完整

---

### P1-4 用户切换 — fromUserId 机制验证

**车机场景**：主驾用户从100切换到120（如访客模式），旧用户100自动下线，新用户120自动上线

**前置条件**：userId=100在线，Accessibility已注册全局监听器

**操作步骤**：
1. 触发用户切换：OnWMSConnectionChanged(userId=120, isConnected=true, fromUserId=100)
2. 验证：UpdateActiveUserIds(120, true) → activeUserIds_ 加入120
3. 验证：UpdateActiveUserIds(100, false) → activeUserIds_ 移除100（fromUserId机制）
4. 调用 `GetActiveUserIds()` → 返回 {120}
5. 全局监听器收到：userId=100 断连通知 + userId=120 连接通知
6. OnUserAdded(120) → 全局监听器注册到 WindowManager(120)
7. userId=120 窗口事件 → myFocusListener 正常收到回调

**预期结果**：
- Step 4: activeUserIds = {120}，不含100
- Step 5: OnWMSConnectionChangedCallback 先回调 fromUserId=100 断连，再回调 userId=120 连接
- Step 6-7: 监听器自动传播到新用户

---

### P1-5 SceneBoard 异常死亡 — 缓存清空与自愈恢复

**车机场景**：车机系统异常崩溃重启，所有 WMS 连接断开后恢复，聚合功能应自动恢复

**前置条件**：userId=100、101在线，activeUserIdsInitialized_=true，Accessibility已注册全局监听器

**操作步骤**：
1. Kill SceneBoard 进程 → FoundationDeathRecipient::OnRemoteDied 触发
2. OnFoundationDied 执行：清空所有proxy、清空 activeUserIds_、activeUserIdsInitialized_=false
3. 调用 `GetActiveUserIds()` → 返回空集合（IPC不可用，initialized=false）
4. 调用 `GetVisibilityWindowInfo(infos)` → WM_OK，infos 为空
5. 等待 SceneBoard 重启恢复 → SMSRecoverListener 触发 RecoverSessionManagerService
6. WMS 重连：OnWMSConnectionChanged(userId=100, true) 和 (userId=101, true)
7. UpdateActiveUserIds 重新添加 → OnUserAdded(100) 和 OnUserAdded(101) 重新触发
8. 验证全局监听器重新注册到100和101的 WindowManager 实例
9. 调用 `GetActiveUserIds()` → 返回 {100, 101}
10. 焦点/可见性回调恢复正常

**预期结果**：
- Step 3-4: 返回空集合，不崩溃
- Step 7-8: OnUserAdded 重新触发，全局监听器自动恢复注册
- Step 9-10: 聚合查询和回调完全恢复
- **关键**：OnFoundationDied 清空 SessionManager 缓存但不清空 AllUsersWindowManager 的 focusListeners_/visibilityListeners_，恢复时监听器集合仍在，OnUserAdded 可以重新注册

---

### P1-6 同一用户多屏幕 — 去重不重复查询

**车机场景**：主驾在主屏和副屏同时显示（同一 userId=100 在两个屏幕），聚合查询不应重复

**前置条件**：userId=100 在 screenId=0 和 screenId=1 上都有 WMS 连接

**操作步骤**：
1. OnWMSConnectionChanged(userId=100, screenId=0, true) → UpdateActiveUserIds(100, true)
2. OnWMSConnectionChanged(userId=100, screenId=1, true) → UpdateActiveUserIds(100, true)（重复 insert）
3. 调用 `GetActiveUserIds()` → unordered_set 去重，返回 {100}（不是 {100, 100}）
4. 调用 `GetVisibilityWindowInfo(infos)` → 只查询 WindowManager(100) 一次

**预期结果**：
- activeUserIds_ = {100}（不是 {100, 100}）
- 聚合查询只对 userId=100 查询一次，不重复

---

## P2 — 边界与异常路径

### P2-1 注册/反注册 null 监听器

**操作步骤**：
```
RegisterFocusChangedListener(nullptr) → 预期 WM_ERROR_NULLPTR
UnregisterFocusChangedListener(nullptr) → 预期 WM_ERROR_NULLPTR
RegisterVisibilityChangedListener(nullptr) → 预期 WM_ERROR_NULLPTR
UnregisterVisibilityChangedListener(nullptr) → 预期 WM_ERROR_NULLPTR
```

**预期结果**：全部返回 WM_ERROR_NULLPTR，不崩溃，不影响已有监听器

---

### P2-2 重复注册同一监听器 — 幂等性

**操作步骤**：
1. `RegisterFocusChangedListener(listener)` → 返回 WM_OK
2. `RegisterFocusChangedListener(listener)`（同一对象再次注册）→ 返回 WM_OK
3. focusListeners_ 中只有一个 listener（unordered_set 去重）
4. 触发焦点变化 → 回调只触发一次（不重复）

**预期结果**：第二次注册无害，回调不重复

---

### P2-3 反注册未注册的监听器 — 幂等性

**操作步骤**：
1. 创建 listenerA 但不注册
2. 注册 listenerB → 正常工作
3. `UnregisterFocusChangedListener(listenerA)` → 返回 WM_OK（不存在于集合中，erase 无操作）
4. listenerB 正常收到回调，不受影响

**预期结果**：反注册不存在的监听器无害，不误删其他监听器

---

### P2-4 注册监听器时部分用户 IPC 失败

**车机场景**：userId=101 的 SCB 正在重启，注册全局监听器时101失败但100成功

**前置条件**：userId=100正常在线，userId=101的WMS连接异常（SCB重启中）

**操作步骤**：
1. `RegisterFocusChangedListener(myFocusListener)`
2. focusListeners_ 先插入 listener → 然后遍历 activeUserIds IPC
3. userId=100 RegisterFocusChangedListener 成功
4. userId=101 RegisterFocusChangedListener 失败（IPC 错误 WM_ERROR_IPC_FAILED）
5. 返回值为101的错误码
6. 等待101的SCB恢复 → ActiveFaultAgentReregister 自动重试注册

**预期结果**：
- Step 4: 日志 "RegisterFocusChangedListener failed for userId 101"
- Step 5: 返回非 WM_OK
- Step 6: SCB恢复后 Agent 自动重试注册，监听器最终对101也生效

---

### P2-5 GetActiveUserIds IPC 失败 — 返回空集合不崩溃

**车机场景**：MockSMS SA 不可用时查询应安全降级

**操作步骤**：
1. Kill MockSMS SA 进程
2. 调用 `GetActiveUserIds()` → InitializeActiveUserIds IPC失败 → 返回空集合
3. 调用 `GetVisibilityWindowInfo(infos)` → 遍历空活跃用户列表 → WM_OK，infos 为空
4. 恢复 MockSMS SA
5. 再次调用 `GetActiveUserIds()` → 重试成功，返回正确列表

**预期结果**：
- Step 2-3: 返回空集合/空 infos，不崩溃
- Step 5: 自动重试成功，数据恢复

---

### P2-6 实例上限边界 — 超过 MAX_INSTANCE_NUM(20) 降级

**车机场景**：极端多用户场景（20+用户空间同时在线）

**操作步骤**：
1. 快速上线22个用户（userId=100~121）
2. 前21个用户正常创建 WindowManager 实例（`> MAX_INSTANCE_NUM` 允许 size=21）
3. 第22个用户降级返回默认实例 → 日志 "Can not create a new instance that limited by MAX_INSTANCE_NUM"
4. 调用 `GetVisibilityWindowInfo` → 超限用户使用默认实例查询，数据可能重复

**预期结果**：
- 不崩溃、不内存泄漏
- 超限用户降级到默认实例，功能不中断（数据可能有重叠）

---

### P2-7 并发多线程调用 — 不死锁不丢数据

**操作步骤**：
1. 创建10个线程并发调用 `GetActiveUserIds()`
2. 同时有用户上线事件（UpdateActiveUserIds）
3. 所有线程返回值最终一致
4. 不死锁、不崩溃

**预期结果**：并发安全，最终一致性

---

### P2-8 系统冷启动 — 无用户在线时的初始状态

**操作步骤**：
1. 系统冷启动，无任何 WMS 连接
2. `GetActiveUserIds()` → 返回空集合（或 IPC 返回空列表）
3. `GetVisibilityWindowInfo(infos)` → WM_OK，infos 为空
4. `RegisterFocusChangedListener(listener)` → WM_OK，listener 加入 focusListeners_，遍历空用户列表（harmless）
5. userId=100 上线 → OnUserAdded(100) → listener 自动注册到 WindowManager(100)
6. 100的焦点事件 → 回调正常触发

**预期结果**：冷启动安全，监听器先记录后传播

---

## P3 — 压力与长稳

### P3-1 快速用户上下线循环

**操作步骤**：
1. userId=101 上线 → 下线 → 上线 → 下线，循环50次
2. 每次 activeUserIds 正确反映当前状态
3. WindowManager 实例不无限增长（RemoveInstanceByUserId 正常工作）
4. 监听器每次上线自动恢复注册

**预期结果**：不死锁、不内存泄漏、状态始终正确

---

### P3-2 多服务并发注册监听器

**操作步骤**：
1. AccessibilityService、InputMethodService 等3个服务并发注册各自的 FocusChangedListener
2. 3个监听器均加入 focusListeners_
3. 每个监听器对所有活跃用户的 WM 注册
4. 触发焦点变化 → 3个监听器都收到回调

**预期结果**：多服务并发注册安全，各监听器独立工作

---

### P3-3 长稳运行24小时

**操作步骤**：
1. 注册10个全局监听器
2. 3个用户持续在线
3. 定期触发焦点/可见性事件
4. 24小时后检查所有监听器仍正常、focusListeners_ 大小未异常增长

**预期结果**：无内存泄漏、无监听器丢失

---

## 优先级总结

| 级别 | 数量 | 必须通过条件 |
|-----|------|------------|
| P0 | 3 | 全部通过才可发布 |
| P1 | 6 | 至少通过 P1-1~P1-5 才可合入主干 |
| P2 | 8 | 建议全部执行 |
| P3 | 3 | 可选，长稳在发版前执行 |

**建议执行顺序**：P0-1 → P0-2 → P0-3 → P1-1 → P1-2 → P1-3 → P1-4 → P1-5 → P1-6 → P2全 → P3-1 → P3-2 → P3-3