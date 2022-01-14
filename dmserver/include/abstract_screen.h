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
#include <refbase.h>
#include <screen_manager/screen_types.h>

#include "screen.h"

namespace OHOS::Rosen {
constexpr static ScreenId SCREEN_ID_INVALID = INVALID_SCREEN_ID;

enum class ScreenCombination : uint32_t {
    SCREEN_ALONE,
    SCREEN_EXPAND,
    SCREEN_MIRROR,
};

enum class ScreenType : uint32_t {
    UNDEFINE,
    REAL,
    VIRTUAL
};

struct AbstractScreenInfo : public RefBase {
    int32_t width_;
    int32_t height_;
    uint32_t freshRate_;
};

class AbstractScreenGroup;
class AbstractScreen : public RefBase {
public:
    AbstractScreen(ScreenId dmsId, ScreenId rsId);
    AbstractScreen() = delete;
    ~AbstractScreen();

    sptr<AbstractScreenGroup> GetGroup() const;

    ScreenId dmsId_;
    ScreenId rsId_;
    ScreenId groupDmsId_;
    ScreenType type_ { ScreenType::REAL };
    int32_t activeIdx_;
    std::vector<sptr<AbstractScreenInfo>> infos_ = {};
};

class AbstractScreenGroup : public AbstractScreen {
public:
    AbstractScreenGroup(ScreenId dmsId, ScreenId rsId);
    AbstractScreenGroup() = delete;
    ~AbstractScreenGroup();

    void AddChild(ScreenCombination combination, sptr<AbstractScreen>& dmsScreen, Point& startPoint);
    void AddChild(ScreenCombination combination,
        std::vector<sptr<AbstractScreen>>& dmsScreens,
        std::vector<Point>& startPoints);
    std::vector<sptr<AbstractScreen>> GetChildren() const;
    std::vector<Point> GetChildrenPosition() const;

    ScreenCombination combination_ { ScreenCombination::SCREEN_ALONE };
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_ABSTRACT_SCREEN_H