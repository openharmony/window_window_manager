/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
class WaterMarkFlagChangedListenerFuture : public IWaterMarkFlagChangedListener {
public:
    void OnWaterMarkFlagUpdate(bool showWaterMark) override
    {
        future_.SetValue(showWaterMark);
    };
    RunnableFuture<bool> future_;
    static constexpr long WAIT_TIME = 20000;
};
}
}

using namespace OHOS;
using namespace OHOS::Rosen;

int main(int argc, char *argv[])
{
    std::cout << "===========================Start===========================" << std::endl;

    std::cout << "RegisterWaterMarkFlagChangedListener" << std::endl;
    sptr<WaterMarkFlagChangedListenerFuture> listener = new WaterMarkFlagChangedListenerFuture();
    if (listener == nullptr) {
        return 0;
    }
    WindowManager::GetInstance().RegisterWaterMarkFlagChangedListener(listener);

    std::cout << "You can open a window with water mark flag in during 20s" << std::endl;
    auto result = listener->future_.GetResult(WaterMarkFlagChangedListenerFuture::WAIT_TIME);
    std::cout << "callback result = " << result << std::endl;

    listener->future_.Reset(result);
    std::cout << "You can close the window, hide it, or change the flag in during 20s" << std::endl;
    result = listener->future_.GetResult(WaterMarkFlagChangedListenerFuture::WAIT_TIME);
    std::cout << "callback result = " << result << std::endl;

    WindowManager::GetInstance().UnregisterWaterMarkFlagChangedListener(listener);
    std::cout << "============================End============================" << std::endl;
    return 0;
}