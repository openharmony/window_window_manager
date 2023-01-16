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
#ifndef OHOS_ROSEN_WINDOW_SCENE_COMMON_H
#define OHOS_ROSEN_WINDOW_SCENE_COMMON_H

#include <string>
#include <map>

namespace OHOS::Rosen {
namespace {
    constexpr uint32_t INVALID_SESSION_ID = 0;
}

enum class WSError : int32_t {
    WS_OK = 0,
    WS_DO_NOTHING,
    WS_ERROR_IPC_FAILED,
    WS_ERROR_NULLPTR,
    WS_ERROR_INVALID_SESSION,
};

enum class WSErrorCode : int32_t {
    WS_OK = 0,
    WS_ERROR_NO_PERMISSION = 201,
    WS_ERROR_INVALID_PARAM = 401,
    WS_ERROR_DEVICE_NOT_SUPPORT = 801,
    WS_ERROR_REPEAT_OPERATION = 1300001,
    WS_ERROR_STATE_ABNORMALLY = 1300002,
    WS_ERROR_SYSTEM_ABNORMALLY = 1300003,
    WS_ERROR_INVALID_CALLING = 1300004,
    WS_ERROR_STAGE_ABNORMALLY = 1300005,
    WS_ERROR_CONTEXT_ABNORMALLY = 1300006,
};

const std::map<WSError, WSErrorCode> WS_JS_TO_ERROR_CODE_MAP {
    {WSError::WS_OK,                                   WSErrorCode::WS_OK                       },
    {WSError::WS_DO_NOTHING,                           WSErrorCode::WS_ERROR_STATE_ABNORMALLY   },
    {WSError::WS_ERROR_INVALID_SESSION,                WSErrorCode::WS_ERROR_STATE_ABNORMALLY   },
    {WSError::WS_ERROR_IPC_FAILED,                     WSErrorCode::WS_ERROR_SYSTEM_ABNORMALLY  },
    {WSError::WS_ERROR_NULLPTR,                        WSErrorCode::WS_ERROR_STATE_ABNORMALLY   },
};

enum class SessionState : uint32_t {
    STATE_DISCONNECT = 0,
    STATE_CONNECT,
    STATE_FOREGROUND,
    STATE_ACTIVE,
    STATE_INACTIVE,
    STATE_BACKGROUND,
};

struct SceneAbilityInfo {
    std::string bundleName_ = "";
    std::string abilityName_ = "";
};

enum class SessionSizeChangeReason : uint32_t {
    SHOW = 0,
    HIDE,
    MAXIMIZE,
    MINIMIZE,
    RECOVER,
    ROTATION,
    RESIZE,
    MOVE,
};

struct WSRect {
    int32_t posX_;
    int32_t posY_;
    uint32_t width_;
    uint32_t height_;

    bool operator==(const WSRect& a) const
    {
        return (posX_ == a.posX_ && posY_ == a.posY_ && width_ == a.width_ && height_ == a.height_);
    }

    bool operator!=(const WSRect& a) const
    {
        return !this->operator==(a);
    }

    bool isUninitializedRect() const
    {
        return (posX_ == 0 && posY_ == 0 && width_ == 0 && height_ == 0);
    }

    bool IsInsideOf(const WSRect& a) const
    {
        return (posX_ >= a.posX_ && posY_ >= a.posY_ &&
            posX_ + width_ <= a.posX_ + a.width_ && posY_ + height_ <= a.posY_ + a.height_);
    }
};
}
#endif // OHOS_ROSEN_WINDOW_SCENE_COMMON_H

