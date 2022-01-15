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

namespace OHOS::Rosen {
AbstractScreen::AbstractScreen(ScreenId dmsId, ScreenId rsId)
    : dmsId_(dmsId), rsId_(rsId)
{
}

AbstractScreen::~AbstractScreen()
{
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
}

void AbstractScreenGroup::AddChild(ScreenCombination type, sptr<AbstractScreen>& dmsScreen, Point& startPoint)
{
}

void AbstractScreenGroup::AddChild(ScreenCombination type,
    std::vector<sptr<AbstractScreen>>& dmsScreens,
    std::vector<Point>& startPoints)
{
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