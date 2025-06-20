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

#include "floating_ball_option.h"

namespace OHOS {
namespace Rosen {
FbOption::FbOption()
{
}

void FbOption::SetTemplate(const uint32_t& type)
{
    template_ = type;
}

void FbOption::SetTitle(const std::string& title)
{
    title_ = title;
}

void FbOption::SetContent(const std::string& content)
{
    content_ = content;
}

void FbOption::SetBackgroundColor(const std::string& color)
{
    backgroundColor_ = color;
}

void FbOption::SetIcon(const std::shared_ptr<Media::PixelMap>& icon)
{
    icon_ = icon;
}

uint32_t FbOption::GetTemplate() const
{
    return template_;
}

std::string FbOption::GetTitle() const
{
    return title_;
}

std::string FbOption::GetContent() const
{
    return content_;
}

std::string FbOption::GetBackgroundColor() const
{
    return backgroundColor_;
}

std::shared_ptr<Media::PixelMap> FbOption::GetIcon() const
{
    return icon_;
}

void FbOption::GetFbTemplateBaseInfo(FloatingBallTemplateBaseInfo& fbTemplateBaseInfo)
{
    fbTemplateBaseInfo.template_ = template_;
    fbTemplateBaseInfo.title_ = title_;
    fbTemplateBaseInfo.content_ = content_;
    fbTemplateBaseInfo.backgroundColor_ = backgroundColor_;
}

} // namespace Rosen
} // namespace OHOS