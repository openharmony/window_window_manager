/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_PIP_OPTION_H
#define OHOS_PIP_OPTION_H
#include <refbase.h>
#include <string>
#include "xcomponent_controller.h"

namespace OHOS {
namespace Rosen {
using namespace Ace;
class PipOption : virtual public RefBase {
public:
    explicit PipOption();
    virtual ~PipOption() = default;
    void SetContext(void* contextPtr);
    void SetNavigationId(const std::string& navigationId);
    void SetPipTemplate(uint32_t templateType);
    void SetContentSize(uint32_t width, uint32_t height);
    void SetXComponentController(std::shared_ptr<XComponentController> xComponentController);
    void SetControlGroup(std::vector<std::uint32_t> controlGroup);
    void* GetContext() const;
    std::string GetNavigationId() const;
    uint32_t GetPipTemplate();
    std::vector<std::uint32_t> GetControlGroup();
    void GetContentSize(uint32_t& width, uint32_t& height);
    std::shared_ptr<XComponentController> GetXComponentController();
private:
    void* contextPtr_ = nullptr;
    uint32_t templateType_  = 0;
    std::string navigationId_ = "";
    uint32_t contentWidth_ = 0;
    uint32_t contentHeight_ = 0;
    std::shared_ptr<XComponentController> xComponentController_;
    std::vector<std::uint32_t> controlGroup_;
};
}
}
#endif //OHOS_PIP_OPTION_H
