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

#include "abstract_screen.h"

#include "abstract_screen_controller.h"
#include "display_manager_service.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "AbstractScreenGroup"};
}

AbstractScreen::AbstractScreen(ScreenId dmsId, ScreenId rsId)
    : dmsId_(dmsId), rsId_(rsId)
{
}

AbstractScreen::~AbstractScreen()
{
}

sptr<SupportedScreenModes> AbstractScreen::GetActiveScreenMode() const
{
    if (activeIdx_ < 0 || activeIdx_ >= modes_.size()) {
        WLOGE("active mode index is wrong: %{public}d", activeIdx_);
        return nullptr;
    }
    return modes_[activeIdx_];
}

std::vector<sptr<SupportedScreenModes>> AbstractScreen::GetAbstractScreenModes() const
{
    return modes_;
}

sptr<AbstractScreenGroup> AbstractScreen::GetGroup() const
{
    return DisplayManagerService::GetInstance().GetAbstractScreenController()->GetAbstractScreenGroup(groupDmsId_);
}

const sptr<ScreenInfo> AbstractScreen::ConvertToScreenInfo() const
{
    sptr<ScreenInfo> info = new ScreenInfo();
    FillScreenInfo(info);
    return info;
}

void AbstractScreen::UpdateRSTree(std::shared_ptr<RSSurfaceNode>& surfaceNode, bool isAdd)
{
    if (rsDisplayNode_ == nullptr) {
        WLOGFE("rsDisplayNode_ is nullptr");
        return;
    }
    WLOGFI("AbstractScreen::UpdateRSTree");

    // add or remove window with transition animation, default duration 100ms
    static RSAnimationTimingProtocol timingProtocol(100);
    RSNode::Animate(timingProtocol, Rosen::RSAnimationTimingCurve::EASE_OUT, [=]() {
        if (isAdd) {
            rsDisplayNode_->AddChild(surfaceNode, -1);
        } else {
            rsDisplayNode_->RemoveChild(surfaceNode);
        }
    });
}

void AbstractScreen::InitRSDisplayNode(RSDisplayNodeConfig& config)
{
    std::shared_ptr<RSDisplayNode> rsDisplayNode = RSDisplayNode::Create(config);
    if (rsDisplayNode == nullptr) {
        WLOGE("fail to add child. create rsDisplayNode fail!");
        return;
    }
    rsDisplayNode_ = rsDisplayNode;
}

void AbstractScreen::FillScreenInfo(sptr<ScreenInfo> info) const
{
    info->id_ = dmsId_;
    uint32_t width = 0;
    uint32_t height = 0;
    if (activeIdx_ >= 0 && activeIdx_ < modes_.size()) {
        sptr<SupportedScreenModes> abstractScreenModes = modes_[activeIdx_];
        height = abstractScreenModes->height_;
        width = abstractScreenModes->width_;
    }
    info->virtualPixelRatio_ = virtualPixelRatio;
    info->virtualHeight_ = virtualPixelRatio * height;
    info->virtualWidth_ = virtualPixelRatio * width;
    info->parent_ = groupDmsId_;
    info->hasChild_ = DisplayManagerService::GetInstance().GetAbstractScreenController()->IsScreenGroup(dmsId_);
    info->modeId_ = activeIdx_;
    info->modes_ = modes_;
}

AbstractScreenGroup::AbstractScreenGroup(ScreenId dmsId, ScreenId rsId, ScreenCombination combination)
    : AbstractScreen(dmsId, rsId), combination_(combination)
{
    type_ = ScreenType::UNDEFINE;
}

AbstractScreenGroup::~AbstractScreenGroup()
{
    rsDisplayNode_ = nullptr;
    abstractScreenMap_.clear();
}

const sptr<ScreenGroupInfo> AbstractScreenGroup::ConvertToScreenGroupInfo() const
{
    sptr<ScreenGroupInfo> screenGroupInfo = new ScreenGroupInfo();
    FillScreenInfo(screenGroupInfo);
    screenGroupInfo->combination_ = combination_;
    for (auto iter = abstractScreenMap_.begin(); iter != abstractScreenMap_.end(); iter++) {
        screenGroupInfo->children_.push_back(iter->first);
    }
    auto positions = GetChildrenPosition();
    screenGroupInfo->position_.insert(screenGroupInfo->position_.end(), positions.begin(), positions.end());
    return screenGroupInfo;
}

bool AbstractScreenGroup::AddChild(sptr<AbstractScreen>& dmsScreen, Point& startPoint)
{
    if (dmsScreen == nullptr) {
        WLOGE("AddChild, dmsScreen is nullptr.");
        return false;
    }
    ScreenId screenId = dmsScreen->dmsId_;
    auto iter = abstractScreenMap_.find(screenId);
    if (iter != abstractScreenMap_.end()) {
        WLOGE("AddChild, abstractScreenMap_ has dmsScreen:%{public}" PRIu64"", screenId);
        return false;
    }

    struct RSDisplayNodeConfig config;
    switch (combination_) {
        case ScreenCombination::SCREEN_ALONE:
        case ScreenCombination::SCREEN_EXPAND:
            config = { dmsScreen->rsId_ };
            break;
        case ScreenCombination::SCREEN_MIRROR:
            WLOGE("The feature will be supported in the future");
            return false;
        default:
            WLOGE("fail to add child. invalid group combination:%{public}u", combination_);
            return false;
    }
    dmsScreen->InitRSDisplayNode(config);
    abstractScreenMap_.insert(std::make_pair(screenId, std::make_pair(dmsScreen, startPoint)));
    return true;
}

bool AbstractScreenGroup::AddChildren(std::vector<sptr<AbstractScreen>>& dmsScreens, std::vector<Point>& startPoints)
{
    size_t size = dmsScreens.size();
    if (size != startPoints.size()) {
        WLOGE("AddChildren, unequal size.");
        return false;
    }
    bool res = true;
    for (size_t i = 0; i < size; i++) {
        res &= AddChild(dmsScreens[i], startPoints[i]);
    }
    return res;
}

bool AbstractScreenGroup::RemoveChild(sptr<AbstractScreen>& dmsScreen)
{
    if (dmsScreen == nullptr) {
        WLOGE("RemoveChild, dmsScreen is nullptr.");
        return false;
    }
    ScreenId screenId = dmsScreen->dmsId_;
    dmsScreen->groupDmsId_ = SCREEN_ID_INVALID;
    bool res = abstractScreenMap_.erase(screenId);
    if (abstractScreenMap_.size() == 1) {
        combination_ = ScreenCombination::SCREEN_ALONE;
    }
    return res;
}

bool AbstractScreenGroup::HasChild(ScreenId childScreen) const
{
    auto iter = abstractScreenMap_.find(childScreen);
    return iter != abstractScreenMap_.end();
}

std::vector<sptr<AbstractScreen>> AbstractScreenGroup::GetChildren() const
{
    std::vector<sptr<AbstractScreen>> res;
    for (auto iter = abstractScreenMap_.begin(); iter != abstractScreenMap_.end(); iter++) {
        res.push_back(iter->second.first);
    }
    return res;
}

std::vector<Point> AbstractScreenGroup::GetChildrenPosition() const
{
    std::vector<Point> res;
    for (auto iter = abstractScreenMap_.begin(); iter != abstractScreenMap_.end(); iter++) {
        res.push_back(iter->second.second);
    }
    return res;
}

size_t AbstractScreenGroup::GetChildCount() const
{
    return abstractScreenMap_.size();
}
} // namespace OHOS::Rosen