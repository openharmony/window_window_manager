/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
 
#ifndef OHOS_FB_OPTION_H
#define OHOS_FB_OPTION_H
#include <refbase.h>
#include <string>
#include "floating_ball_template_info.h"
namespace OHOS {
namespace Rosen {
class FbOption : virtual public RefBase {
public:
    explicit FbOption();
    virtual ~FbOption() = default;
 
    void SetTemplate(const uint32_t& type);
    void SetTitle(const std::string& title);
    void SetContent(const std::string& content);
    void SetIcon(const std::shared_ptr<Media::PixelMap>& icon);
 
    uint32_t GetTemplate() const;
    std::string GetTitle() const;
    std::string GetContent() const;
    std::shared_ptr<Media::PixelMap> GetIcon() const;
 
    void GetFbTemplateBaseInfo(FloatingBallTemplateBaseInfo& fbTemplateBaseInfo);
private:
    uint32_t template_ {};
    std::string title_ {};
    std::string content_ {};
    std::shared_ptr<Media::PixelMap> icon_ {};
};
}
}
#endif //OHOS_FB_OPTION_H