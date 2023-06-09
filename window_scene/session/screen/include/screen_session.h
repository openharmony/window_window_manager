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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_H
#define OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_H

#include <vector>

#include <refbase.h>
#include <screen_manager/screen_types.h>
#include <ui/rs_display_node.h>

#include "screen_property.h"
#include "dm_common.h"
#include "display_info.h"
#include "screen.h"
#include "screen_info.h"
#include "screen_group.h"
#include "screen_group_info.h"

namespace OHOS::Rosen {
class IScreenChangeListener : public RefBase {
public:
    IScreenChangeListener() = default;
    virtual ~IScreenChangeListener() = default;

    virtual void OnConnect() = 0;
    virtual void OnDisconnect() = 0;
    virtual void OnPropertyChange(const ScreenProperty& newProperty) = 0;
};

enum class ScreenState : int32_t {
    INIT,
    CONNECTION,
    DISCONNECTION,
};

class ScreenSession : public RefBase {
public:
    explicit ScreenSession(ScreenId screenId, const ScreenProperty& property);
    ScreenSession();
    ScreenSession(const std::string& name, ScreenId smsId, ScreenId rsId);
    ~ScreenSession() = default;

    void RegisterScreenChangeListener(IScreenChangeListener* screenChangeListener);
    void UnregisterScreenChangeListener(IScreenChangeListener* screenChangeListener);

    sptr<DisplayInfo> ConvertToDisplayInfo();

    ScreenId GetScreenId();
    ScreenProperty GetScreenProperty() const;
    std::shared_ptr<RSDisplayNode> GetDisplayNode() const;

    sptr<SupportedScreenModes> GetActiveScreenMode() const;
    Rotation CalcRotation(Orientation orientation) const;
    sptr<ScreenInfo> ConvertToScreenInfo() const;
    void FillScreenInfo(sptr<ScreenInfo> info) const;
    bool SetOrientation(Orientation orientation);
    void InitRSDisplayNode(RSDisplayNodeConfig& config, Point& startPoint);
    DMError GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& colorGamuts);

    std::string name_;
    ScreenId screenId_;
    ScreenId rsId_;
    ScreenType type_ { ScreenType::REAL };
    float virtualPixelRatio_ = { 1.0 };
    Orientation orientation_ { Orientation::UNSPECIFIED };

    int32_t activeIdx_ { 0 };
    std::vector<sptr<SupportedScreenModes>> modes_ = {};
    Orientation screenRequestedOrientation_ { Orientation::UNSPECIFIED };
    Rotation rotation_ { Rotation::ROTATION_0 };

    bool isScreenGroup_ { false };
    ScreenId groupSmsId_ { SCREEN_ID_INVALID };
    ScreenId lastGroupSmsId_ { SCREEN_ID_INVALID };
    std::shared_ptr<RSDisplayNode> displayNode_;

    void Connect();
    void Disconnect();

private:
    ScreenProperty property_;
    
    ScreenState screenState_ { ScreenState::INIT };
    std::vector<IScreenChangeListener*> screenChangeListenerList_;
};

class ScreenSessionGroup : public ScreenSession {
public:
    ScreenSessionGroup(ScreenId smsId, ScreenId rsId, std::string name, ScreenCombination combination);
    ScreenSessionGroup() = delete;
    WM_DISALLOW_COPY_AND_MOVE(ScreenSessionGroup);
    ~ScreenSessionGroup();

    bool AddChild(sptr<ScreenSession>& smsScreen, Point& startPoint);
    bool AddChild(sptr<ScreenSession>& smsScreen, Point& startPoint, sptr<ScreenSession> defaultScreenSession);
    bool AddChildren(std::vector<sptr<ScreenSession>>& smsScreens, std::vector<Point>& startPoints);
    bool RemoveChild(sptr<ScreenSession>& smsScreen);
    bool HasChild(ScreenId childScreen) const;
    std::vector<sptr<ScreenSession>> GetChildren() const;
    std::vector<Point> GetChildrenPosition() const;
    Point GetChildPosition(ScreenId screenId) const;
    size_t GetChildCount() const;
    sptr<ScreenGroupInfo> ConvertToScreenGroupInfo() const;
    ScreenCombination GetScreenCombination() const;

    ScreenCombination combination_ { ScreenCombination::SCREEN_ALONE };
    ScreenId mirrorScreenId_ { SCREEN_ID_INVALID };

private:
    bool GetRSDisplayNodeConfig(sptr<ScreenSession>& dmsScreen, struct RSDisplayNodeConfig& config,
        sptr<ScreenSession> defaultScreenSession);

    std::map<ScreenId, std::pair<sptr<ScreenSession>, Point>> screenSessionMap_;
};

} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_H
