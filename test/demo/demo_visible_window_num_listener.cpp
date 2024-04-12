/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <iostream>
#include <refbase.h>
#include <unistd.h>

#include "window.h"
#include "wm_common.h"
#include "window_option.h"
#include "window_manager.h"

#include "future.h"

namespace OHOS {
namespace Rosen {
static constexpr long WAIT_TIME = 20000;

class VisibleWindowNumChangedListenerFuture : public IVisibleWindowNumChangedListener {
public:
    void OnVisibleWindowNumChange(const std::vector<VisibleWindowNumInfo>& visibleWindowNumInfo) override
    {
        future_.SetValue(visibleWindowNumInfo);
    };
    RunnableFuture<std::vector<VisibleWindowNumInfo>> future_;
};
}
}

using namespace OHOS;
using namespace OHOS::Rosen;

int main(int argc, char *argv[])
{
    std::cout << "===========================Start===========================" << std::endl;

    std::cout << "RegisterVisibleWindowNumChangedListener" << std::endl;
    sptr<VisibleWindowNumChangedListenerFuture> listener = new VisibleWindowNumChangedListenerFuture();
    if (listener == nullptr) {
        return 0;
    }
    WindowManager::GetInstance().RegisterVisibleWindowNumChangedListener(listener);

    std::cout << "You can open a window in during 20s" << std::endl;
    std::vector<VisibleWindowNumInfo> visibleWindowNumInfo;
    visibleWindowNumInfo = listener->future_.GetResult(WAIT_TIME);
    for (const auto& num : visibleWindowNumInfo) {
        std::cout << "callback displayId = " << num.displayId <<
            "callback visibleWindowNum = " << num.visibleWindowNum << std::endl;
    }

    listener->future_.Reset(visibleWindowNumInfo);
    std::cout << "You can close the window, hide it in during 20s" << std::endl;
    visibleWindowNumInfo = listener->future_.GetResult(WAIT_TIME);
    for (const auto& num : visibleWindowNumInfo) {
        std::cout << "callback displayId = " << num.displayId <<
            "callback visibleWindowNum = " << num.visibleWindowNum << std::endl;
    }

    WindowManager::GetInstance().UnregisterVisibleWindowNumChangedListener(listener);
    std::cout << "============================End============================" << std::endl;
    return 0;
}