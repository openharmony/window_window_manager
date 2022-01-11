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

#include <refbase.h>
#include <screen_manager/screen_types.h>

namespace OHOS::Rosen {
struct Point {
    int32_t posX_;
    int32_t posY_;
};

enum class AbstractScreenType : uint32_t {
    SCREEN_ALONE,
    SCREEN_EXPAND,
    SCREEN_MIRROR,
};

class AbstractScreen : public RefBase {
public:
    AbstractScreen(ScreenId dmsId, ScreenId rsId);
    AbstractScreen() = delete;
    ~AbstractScreen();

    ScreenId dmsId_;
    ScreenId rsId_;
};

class AbstractScreenGroup : public AbstractScreen {
public:
    AbstractScreenGroup(ScreenId dmsId, ScreenId rsId);
    AbstractScreenGroup() = delete;
    ~AbstractScreenGroup();

    AbstractScreenType GetType() const;
    std::vector<sptr<AbstractScreen>> GetChildren() const;
    std::vector<Point> GetChildrenPosition() const;
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_ABSTRACT_SCREEN_H