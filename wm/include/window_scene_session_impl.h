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

const std::map<OHOS::AppExecFwk::DisplayOrientation, Orientation> ABILITY_TO_SESSION_ORIENTATION_MAP {
    {OHOS::AppExecFwk::DisplayOrientation::UNSPECIFIED,                         Orientation::UNSPECIFIED},
    {OHOS::AppExecFwk::DisplayOrientation::LANDSCAPE,                           Orientation::HORIZONTAL},
    {OHOS::AppExecFwk::DisplayOrientation::PORTRAIT,                            Orientation::VERTICAL},
    {OHOS::AppExecFwk::DisplayOrientation::FOLLOWRECENT,                        Orientation::LOCKED},
    {OHOS::AppExecFwk::DisplayOrientation::LANDSCAPE_INVERTED,                  Orientation::REVERSE_HORIZONTAL},
    {OHOS::AppExecFwk::DisplayOrientation::PORTRAIT_INVERTED,                   Orientation::REVERSE_VERTICAL},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION,                       Orientation::SENSOR},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_LANDSCAPE,             Orientation::SENSOR_HORIZONTAL},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_PORTRAIT,              Orientation::SENSOR_VERTICAL},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_RESTRICTED,            Orientation::AUTO_ROTATION_RESTRICTED},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED,
        Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_PORTRAIT_RESTRICTED,
        Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED},
    {OHOS::AppExecFwk::DisplayOrientation::LOCKED,                              Orientation::LOCKED},
};

class WindowSceneSessionImpl : public WindowSessionImpl {
public:
    explicit WindowSceneSessionImpl(const sptr<WindowOption>& option);
    ~WindowSceneSessionImpl();
    WMError Create(const std::shared_ptr<AbilityRuntime::Context>& context,
        const sptr<Rosen::ISession>& iSession) override;
    WMError Show(uint32_t reason = 0, bool withAnimation = false) override;
    WMError Hide(uint32_t reason, bool withAnimation, bool isFromInnerkits) override;
    WMError Destroy(bool needNotifyServer, bool needClearListener = true) override;
    void PreProcessCreate();
    void SetDefaultProperty();
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
    WmErrorCode RaiseAboveTarget(int32_t subWindowId) override;
    WSError HandleBackEvent() override;
    void PerformBack() override;
    WMError SetAspectRatio(float ratio) override;
    WMError ResetAspectRatio() override;
    WMError SetGlobalMaximizeMode(MaximizeMode mode) override;
    MaximizeMode GetGlobalMaximizeMode() const override;
    WMError GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea) override;
    SystemBarProperty GetSystemBarPropertyByType(WindowType type) const override;
    WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) override;
    WMError SetLayoutFullScreen(bool status) override;
    WMError SetFullScreen(bool status) override;
    WMError BindDialogTarget(sptr<IRemoteObject> targetToken) override;
    static void UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration);
    static sptr<Window> GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    static sptr<Window> GetTopWindowWithId(uint32_t mainWinId);
    virtual void UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration) override;

    WMError NotifyMemoryLevel(int32_t level) override;

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
    virtual WMError SetWindowMode(WindowMode mode) override;

    virtual WMError SetTransparent(bool isTransparent) override;
    virtual WMError SetTurnScreenOn(bool turnScreenOn) override;
    virtual WMError SetKeepScreenOn(bool keepScreenOn) override;
    virtual WMError SetPrivacyMode(bool isPrivacyMode) override;
    virtual void SetSystemPrivacyMode(bool isSystemPrivacyMode) override;
    virtual WMError SetSnapshotSkip(bool isSkip) override;
    virtual std::shared_ptr<Media::PixelMap> Snapshot() override;
    WMError SetTouchHotAreas(const std::vector<Rect>& rects) override;

    virtual bool IsTransparent() const override;
    virtual bool IsTurnScreenOn() const override;
    virtual bool IsKeepScreenOn() const override;
    virtual bool IsPrivacyMode() const override;
    virtual bool IsLayoutFullScreen() const override;
    virtual bool IsFullScreen() const override;

    WMError RegisterAnimationTransitionController(const sptr<IAnimationTransitionController>& listener) override;
    void SetNeedDefaultAnimation(bool needDefaultAnimation) override;
    WMError SetTransform(const Transform& trans) override;
    const Transform& GetTransform() const override;
    WMError UpdateSurfaceNodeAfterCustomAnimation(bool isAdd) override;
    WMError SetAlpha(float alpha) override;
    void DumpSessionElementInfo(const std::vector<std::string>& params) override;
    WSError UpdateWindowMode(WindowMode mode) override;

protected:
    void DestroySubWindow();
    WMError CreateAndConnectSpecificSession();
    sptr<WindowSessionImpl> FindParentSessionByParentId(uint32_t parentId);
    sptr<WindowSessionImpl> FindMainWindowWithContext();
    void UpdateSubWindowStateAndNotify(int32_t parentPersistentId, const WindowState& newState);
    void LimitCameraFloatWindowMininumSize(uint32_t& width, uint32_t& height);
    void UpdateFloatingWindowSizeBySizeLimits(uint32_t& width, uint32_t& height) const;
    WMError NotifyWindowSessionProperty();
    WMError NotifyWindowNeedAvoid(bool status = false);
    WMError SetLayoutFullScreenByApiVersion(bool status) override;
    void UpdateWindowSizeLimits();
    WindowLimits GetSystemSizeLimits(uint32_t displayWidth, uint32_t displayHeight, float vpr);
    void GetConfigurationFromAbilityInfo();

private:
    bool IsValidSystemWindowType(const WindowType& type);
    WMError CheckParmAndPermission();
    static uint32_t maxFloatingWindowSize_;
    void TransformSurfaceNode(const Transform& trans);
    void AdjustWindowAnimationFlag(bool withAnimation = false);
    WMError UpdateAnimationFlagProperty(bool withAnimation);
    WMError UpdateWindowModeImmediately(WindowMode mode);
    uint32_t UpdateConfigVal(uint32_t minVal, uint32_t maxVal, uint32_t configVal, uint32_t defaultVal, float vpr);
    void UpdateWindowState();

    bool enableDefaultAnimation_ = true;
    sptr<IAnimationTransitionController> animationTransitionController_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_IMPL_H
