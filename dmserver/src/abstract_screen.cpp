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
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "AbstractScreen"};
}
AbstractScreen::AbstractScreen(ScreenId dmsId, ScreenId rsId)
    : dmsId_(dmsId), rsId_(rsId)
{
}

AbstractScreen::~AbstractScreen()
{
}

sptr<AbstractScreenInfo> AbstractScreen::GetActiveScreenInfo() const
{
    if (activeIdx_ < 0 && activeIdx_ >= infos_.size()) {
        WLOGE("active mode index is wrong: %{public}d", activeIdx_);
        return nullptr;
    }
    return infos_[activeIdx_];
}

sptr<AbstractScreenGroup> AbstractScreen::GetGroup() const
{
    return DisplayManagerService::GetInstance().GetAbstractScreenController()->GetAbstractScreenGroup(groupDmsId_);
}

AbstractScreenGroup::AbstractScreenGroup(ScreenId dmsId, ScreenId rsId) : AbstractScreen(dmsId, rsId)
{
    type_ = ScreenType::UNDEFINE;
}

AbstractScreenGroup::~AbstractScreenGroup()
{
    rsDisplayNode_ = nullptr;
    children_.clear();
}

bool AbstractScreenGroup::AddChild(ScreenCombination type, sptr<AbstractScreen>& dmsScreen, Point& startPoint)
{ 
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
    std::shared_ptr<RSDisplayNode> rsDisplayNode = RSDisplayNode::Create(config);
    if (rsDisplayNode == nullptr) {
        WLOGE("fail to add child. create rsDisplayNode fail!");
        return false;
    }
    children_.push_back(dmsScreen);
    dmsScreen->rsDisplayNode_ = rsDisplayNode;
    return true;
}

bool AbstractScreenGroup::AddChild(ScreenCombination type,
    std::vector<sptr<AbstractScreen>>& dmsScreens,
    std::vector<Point>& startPoints)
{
    return true;
}

std::vector<sptr<AbstractScreen>> AbstractScreenGroup::GetChildren() const
{
    std::vector<sptr<AbstractScreen>> tmp;
    return tmp;
}

std::vector<Point> AbstractScreenGroup::GetChildrenPosition() const
{
    std::vector<Point> tmp;
    return tmp;
}
} // namespace OHOS::Rosen