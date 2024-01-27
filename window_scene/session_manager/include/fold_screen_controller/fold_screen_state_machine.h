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

#ifndef OHOS_ROSEN_FOLD_SCREEN_STATE_MACHINE_H
#define OHOS_ROSEN_FOLD_SCREEN_STATE_MACHINE_H

#include <deque>
#include <memory>
#include <mutex>
#include <sstream>

#include "refbase.h"
#include "ws_common.h"

namespace OHOS {
namespace Rosen {

enum class FoldScreenState : uint32_t {
    UNKNOWN,
    FOLDED,
    HALF_FOLDED,
    FULL
};

class TransitionCallback {
public:
    ~TransitionCallback() = default;

    virtual void OnTransitionEnter(FoldScreenState current, FoldScreenState next) { }

    virtual void OnTransitionExit(FoldScreenState previous, FoldScreenState current) { }
};

class FoldScreenStateMachine : public RefBase {
public:
    FoldScreenStateMachine();

    ~FoldScreenStateMachine();

    void TransitionTo(FoldScreenState state);

    void RegistrationTransitionCallback(const std::shared_ptr<TransitionCallback>& callback);

    void UnRegistrationTransitionCallback(const std::shared_ptr<TransitionCallback>& callback);

    FoldScreenState GetCurrentState() const;

    std::string GenStateMachineInfo() const;

private:
    std::deque<std::shared_ptr<TransitionCallback>> callbacks_;
    FoldScreenState currState_ = FoldScreenState::UNKNOWN;
    std::recursive_mutex mutex_;
};
} // namespace Rosen
} // namespace OHOS
#endif