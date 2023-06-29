/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_IMPL_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_IMPL_H

#include "window_session_impl.h"

namespace OHOS {
namespace Rosen {
class WindowSceneSessionImpl : public WindowSessionImpl {
public:
    explicit WindowSceneSessionImpl(const sptr<WindowOption>& option);
    ~WindowSceneSessionImpl();
    WMError Create(const std::shared_ptr<AbilityRuntime::Context>& context,
        const sptr<Rosen::ISession>& iSession) override;
    WMError Show(uint32_t reason = 0, bool withAnimation = false) override;
    WMError Hide(uint32_t reason, bool withAnimation, bool isFromInnerkits) override;
    WMError Destroy(bool needClearListener) override;
    WSError SetActive(bool active) override;
    WMError DisableAppWindowDecor() override;
    bool IsDecorEnable() const override;
    WMError Minimize() override;
    WMError MaximizeFloating() override;
    WMError Maximize() override;
    WMError Recover() override;
    void StartMove() override;
    WMError Close() override;
    WindowMode GetMode() const override;
    WMError MoveTo(int32_t x, int32_t y) override;
    WMError Resize(uint32_t width, uint32_t height) override;
    WmErrorCode RaiseToAppTop() override;
    WSError HandleBackEvent() override;
    WMError SetAspectRatio(float ratio) override;
    WMError ResetAspectRatio() override;
    WMError SetGlobalMaximizeMode(MaximizeMode mode) override;
    MaximizeMode GetGlobalMaximizeMode() const override;
    WMError GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea) override;
    SystemBarProperty GetSystemBarPropertyByType(WindowType type) const override;
    WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) override;
    WMError SetLayoutFullScreen(bool status) override;
    WMError SetFullScreen(bool status) override;

    WMError SetBackgroundColor(const std::string& color) override;
    WMError SetTransparent(bool isTransparent) override;
    virtual WMError SetPrivacyMode(bool isPrivacyMode) override;
    virtual void SetSystemPrivacyMode(bool isSystemPrivacyMode) override;
    virtual WMError SetSnapshotSkip(bool isSkip) override;
    
    bool IsTransparent() const override;
    virtual bool IsPrivacyMode() const override;

    static void UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration);
    virtual void UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration) override;

    virtual WMError AddWindowFlag(WindowFlag flag) override;
    virtual WMError RemoveWindowFlag(WindowFlag flag) override;
    virtual WMError SetWindowFlags(uint32_t flags) override;
    virtual uint32_t GetWindowFlags() const override;

    // window effect
    virtual WMError SetCornerRadius(float cornerRadius) override;
    virtual WMError SetShadowRadius(float radius) override;
    virtual WMError SetShadowColor(std::string color) override;
    virtual WMError SetShadowOffsetX(float offsetX) override;
    virtual WMError SetShadowOffsetY(float offsetY) override;
    virtual WMError SetBlur(float radius) override;
    virtual WMError SetBackdropBlur(float radius) override;
    virtual WMError SetBackdropBlurStyle(WindowBlurStyle blurStyle) override;

protected:
    void DestroySubWindow();
    WMError CreateAndConnectSpecificSession();
    sptr<WindowSessionImpl> FindParentSessionByParentId(uint32_t parentId);
    sptr<WindowSessionImpl> FindMainWindowWithContext();
    void UpdateSubWindowStateAndNotify(uint64_t parentPersistentId, const WindowState& newState);
    void LimitCameraFloatWindowMininumSize(uint32_t& width, uint32_t& height);
    WMError NotifyWindowSessionProperty();
    WMError NotifyWindowNeedAvoid(bool status = false);
    WMError SetLayoutFullScreenByApiVersion(bool status);

private:
    bool IsValidSystemWindowType(const WindowType& type);
    WMError SetBackgroundColor(uint32_t color);
    uint32_t GetBackgroundColor() const;
    WMError CheckParmAndPermission();
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_IMPL_H
