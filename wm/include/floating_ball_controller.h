/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#ifndef OHOS_FLOATING_BALL_CONTROLLER_H
#define OHOS_FLOATING_BALL_CONTROLLER_H
 
#include <refbase.h>
#include <ability_context.h>
#include "window.h"
#include "wm_common.h"
#include "floating_ball_option.h"
#include "floating_ball_interface.h"
 
namespace OHOS {
namespace Rosen {
using namespace Ace;
class FloatingBallController : public RefBase {
public:
    FloatingBallController(const sptr<Window>& mainWindow, const uint32_t& mainWindowId, void* context);
    virtual ~FloatingBallController();
    WMError StartFloatingBall(sptr<FbOption>& option);
    WMError StopFloatingBall();
    WMError UpdateFloatingBall(sptr<FbOption>& option);
    WMError RestoreMainWindow(const std::shared_ptr<AAFwk::Want>& want);
    WMError DestroyFloatingBallWindow();
    sptr<Window> GetFbWindow() const;
    FbWindowState GetControllerState() const;
    WMError StopFloatingBallFromClient();
 
    WMError RegisterFbLifecycle(const sptr<IFbLifeCycle>& listener);
    WMError RegisterFbClickObserver(const sptr<IFbClickObserver>& listener);
    WMError UnRegisterFbLifecycle(const sptr<IFbLifeCycle>& listener);
    WMError UnRegisterFbClickObserver(const sptr<IFbClickObserver>& listener);
 
    void OnFloatingBallClick();
 
    WMError GetFloatingBallWindowInfo(uint32_t& windowId);

    void UpdateMainWindow(const sptr<Window>& mainWindow);
 
private:
    WMError CreateFloatingBallWindow(const sptr<FbOption>& option);
    WMError StartFloatingBallInner(const sptr<FbOption>& option);
 
    template<typename T> WMError RegisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener);
    template<typename T> WMError UnRegisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener);
 
    void OnFloatingBallStart();
    void OnFloatingBallStop();
 
    wptr<FloatingBallController> weakRef_ = nullptr;
    sptr<Window> window_ = nullptr;
    sptr<Window> mainWindow_ = nullptr;
    uint32_t mainWindowId_ = 0;
    FbWindowState curState_ = FbWindowState::STATE_UNDEFINED;
    void* contextPtr_ = nullptr;
    bool stopFromClient_ = false;
 
    std::vector<sptr<IFbLifeCycle>> fbLifeCycleListeners_;
    std::vector<sptr<IFbClickObserver>> fbClickObservers_;
 
    std::mutex controllerMutex_;
 
    class WindowLifeCycleListener : public IWindowLifeCycle {
    public:
        void AfterDestroyed() override;
    };
    sptr<IWindowLifeCycle> mainWindowLifeCycleListener_ = nullptr;
    std::mutex listenerMutex_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_FLOATING_BALL_CONTROLLER_H