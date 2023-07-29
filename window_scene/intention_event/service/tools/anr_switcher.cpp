/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <iostream>

#include "anr_manager.h"

static void ShowUsage()
{
    std::cout << "Supported commands:" << std::endl;
    std::cout << "ON   ------    turn on anr" << std::endl;
    std::cout << "OFF  ------    turn off anr" << std::endl;
}

int32_t main()
{
    ShowUsage();
    std::string option;
    while (true) {
        std::cin >> option;
        if (option == "ON") {
            OHOS::DelayedSingleton<OHOS::Rosen::ANRManager>::GetInstance()->SwitchAnr(true);
            std::cout << "Anr is on" << std::endl;
        } else if (option == "OFF") {
            OHOS::DelayedSingleton<OHOS::Rosen::ANRManager>::GetInstance()->SwitchAnr(false);
            std::cout << "Anr is off" << std::endl;
        } else {
            std::cout << "Unsupported command" << std::endl;
            ShowUsage();
        }
    }
    return 0;
}