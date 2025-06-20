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
 
#ifndef OHOS_FB_WINDOW_INFO_H
#define OHOS_FB_WINDOW_INFO_H
#include <refbase.h>
#include <string>
 
namespace OHOS {
namespace Rosen {
class FbWindowInfo : virtual public RefBase {
public:
    explicit FbWindowInfo(uint32_t windowId) : windowId_(windowId){};
    virtual ~FbWindowInfo() = default;
 
    void SetWindowId(uint32_t windowId)
    {
        windowId_ = windowId;
    }
 
    uint32_t GetWindowId() {return windowId_;}
 
    static void Finalizer(napi_env env, void* data, void* hint)
    {
        std::unique_ptr<FbWindowInfo>(static_cast<FbWindowInfo*>(data));
    }
 
private:
    uint32_t windowId_ {};
};
}
}
#endif // OHOS_FB_WINDOW_INFO_H