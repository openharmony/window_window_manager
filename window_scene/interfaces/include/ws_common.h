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

#ifndef OHOS_ROSEN_WINDOW_SCENE_WS_COMMON_H
#define OHOS_ROSEN_WINDOW_SCENE_WS_COMMON_H

#include <inttypes.h>
#include <fstream>
#include <map>
#include <string>

#include "iremote_broker.h"

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
    { WSError::WS_OK, WSErrorCode::WS_OK },
    { WSError::WS_DO_NOTHING, WSErrorCode::WS_ERROR_STATE_ABNORMALLY },
    { WSError::WS_ERROR_INVALID_SESSION, WSErrorCode::WS_ERROR_STATE_ABNORMALLY },
    { WSError::WS_ERROR_IPC_FAILED, WSErrorCode::WS_ERROR_SYSTEM_ABNORMALLY },
    { WSError::WS_ERROR_NULLPTR, WSErrorCode::WS_ERROR_STATE_ABNORMALLY },
};

enum class SessionState : uint32_t {
    STATE_DISCONNECT = 0,
    STATE_CONNECT,
    STATE_FOREGROUND,
    STATE_ACTIVE,
    STATE_INACTIVE,
    STATE_BACKGROUND,
};

struct SessionInfo {
    std::string bundleName_ = "";
    std::string abilityName_ = "";
    sptr<IRemoteObject> callerToken_ = nullptr;
};

enum class SizeChangeReason : uint32_t {
    SHOW = 0,
    HIDE,
    MAXIMIZE,
    RECOVER,
    ROTATION,
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

    bool IsUninitializedRect() const
    {
        return (posX_ == 0 && posY_ == 0 && width_ == 0 && height_ == 0);
    }

    bool IsInsideOf(const WSRect& a) const
    {
        return (posX_ >= a.posX_ && posY_ >= a.posY_ && posX_ + width_ <= a.posX_ + a.width_ &&
                posY_ + height_ <= a.posY_ + a.height_);
    }
};

class WindowSceneJudgement final {
public:
    // judge whether window scene is enabled
    static inline bool IsWindowSceneEnabled()
    {
        static bool isWindowSceneEnabled = false;
        static bool initialized = false;
        if (!initialized) {
            InitWindowSceneWithConfigFile(isWindowSceneEnabled);
            initialized = true;
        }
        return isWindowSceneEnabled;
    }

private:
    // dealing with Windows type end of line "\r\n"
    static std::ifstream& SafeGetLine(std::ifstream& configFile, std::string& line)
    {
        std::getline(configFile, line);
        if (line.size() && line[line.size() - 1] == '\r') {
            line = line.substr(0, line.size() - 1);
        }
        return configFile;
    }

    static void InitWindowSceneWithConfigFile(bool& isWindowSceneEnabled)
    {
        std::ifstream configFile("/etc/windowscene.config");
        std::string line;
        if (configFile.is_open() && SafeGetLine(configFile, line) && line == "ENABLED") {
            isWindowSceneEnabled = true;
        }
        configFile.close();
    }
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_WS_COMMON_H
