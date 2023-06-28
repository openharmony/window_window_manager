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

#include "session_manager.h"
#include "session_manager_service_proxy.h"
#include <unistd.h>
#include <iostream>
#include <memory>

using namespace OHOS;
using namespace OHOS::Rosen;

int main(int argc, char *argv[])
{
    SessionManager sessionManager;

    auto remoteObject = sessionManager.GetRemoteObject();
    if (!remoteObject) {
        std::cout << "remoteObject is nullptr." << std::endl;
    }

    auto proxy = std::make_unique<SessionManagerServiceProxy>(remoteObject);

    if (!proxy) {
        std::cout << "proxy is nullptr." << std::endl;
    } else {
        int ret = proxy->GetValueById(1);
        std::cout << "ret value: " << ret << std::endl;
    }

    return 0;
}