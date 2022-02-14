/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_DMSERVER_ABSTRACT_SCREEN_H
#define FOUNDATION_DMSERVER_ABSTRACT_SCREEN_H


#include <vector>
#include <map>
#include <refbase.h>
#include <screen_manager/screen_types.h>
#include <ui/rs_display_node.h>
#include <ui/rs_surface_node.h>

#include "screen.h"
#include "screen_group.h"
#include "screen_group_info.h"
#include "screen_info.h"

namespace OHOS::Rosen {
enum class ScreenType : uint32_t {
    UNDEFINE,
    REAL,
    VIRTUAL
};

class AbstractScreenGroup;
class AbstractScreenController;
class AbstractScreen : public RefBase {
public:
    AbstractScreen(ScreenId dmsId, ScreenId rsId);
    AbstractScreen() = delete;
    ~AbstractScreen();
    sptr<SupportedScreenModes> GetActiveScreenMode() const;
    std::vector<sptr<SupportedScreenModes>> GetAbstractScreenModes() const;
    sptr<AbstractScreenGroup> GetGroup() const;
    const sptr<ScreenInfo> ConvertToScreenInfo() const;
    void RequestRotation(Rotation rotation);
    Rotation GetRotation() const;

    void UpdateRSTree(std::shared_ptr<RSSurfaceNode>& surfaceNode, bool isAdd);
    void InitRSDisplayNode(RSDisplayNodeConfig& config);

    // colorspace, gamut
    DMError GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& colorGamuts);
    DMError GetScreenColorGamut(ScreenColorGamut& colorGamut);
    DMError SetScreenColorGamut(int32_t colorGamutIdx);
    DMError GetScreenGamutMap(ScreenGamutMap& gamutMap);
    DMError SetScreenGamutMap(ScreenGamutMap gamutMap);
    DMError SetScreenColorTransform();

    ScreenId dmsId_;
    ScreenId rsId_;
    bool canHasChild_ { false };
    std::shared_ptr<RSDisplayNode> rsDisplayNode_;
    RSDisplayNodeConfig rSDisplayNodeConfig_;
    ScreenId groupDmsId_ {INVALID_SCREEN_ID};
    ScreenType type_ { ScreenType::REAL };
    int32_t activeIdx_;
    float virtualPixelRatio = { 1.0 };
    std::vector<sptr<SupportedScreenModes>> modes_ = {};
    Rotation rotation_ { Rotation::ROTATION_0 };
protected:
    void FillScreenInfo(sptr<ScreenInfo>) const;
    sptr<AbstractScreenController>& screenController_;
};

class AbstractScreenGroup : public AbstractScreen {
public:
    AbstractScreenGroup(ScreenId dmsId, ScreenId rsId, ScreenCombination combination);
    AbstractScreenGroup() = delete;
    ~AbstractScreenGroup();

    bool AddChild(sptr<AbstractScreen>& dmsScreen, Point& startPoint);
    bool AddChildren(std::vector<sptr<AbstractScreen>>& dmsScreens, std::vector<Point>& startPoints);
    bool RemoveChild(sptr<AbstractScreen>& dmsScreen);
    bool HasChild(ScreenId childScreen) const;
    std::vector<sptr<AbstractScreen>> GetChildren() const;
    std::vector<Point> GetChildrenPosition() const;
    size_t GetChildCount() const;
    const sptr<ScreenGroupInfo> ConvertToScreenGroupInfo() const;

    ScreenCombination combination_ { ScreenCombination::SCREEN_ALONE };
    ScreenId mirrorScreenId_ {INVALID_SCREEN_ID};
private:
    std::map<ScreenId, std::pair<sptr<AbstractScreen>, Point>> abstractScreenMap_;
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_ABSTRACT_SCREEN_H