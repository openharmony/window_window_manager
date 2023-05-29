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

#ifndef OHOS_ROSEN_WINDOW_SESSION_PROPERTY_H
#define OHOS_ROSEN_WINDOW_SESSION_PROPERTY_H

#include <refbase.h>
#include <string>
#include <unordered_map>
#include <parcel.h>
#include "interfaces/include/ws_common.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class WindowSessionProperty : public Parcelable {
public:
    WindowSessionProperty() = default;
    ~WindowSessionProperty() = default;

    void SetWindowName(const std::string& name);
    void SetSessionInfo(const SessionInfo& info);
    void SetRequestRect(const struct Rect& rect);
    void SetWindowRect(const struct Rect& rect);
    void SetFocusable(bool isFocusable);
    void SetTouchable(bool isTouchable);
    void SetDisplayId(uint64_t displayId);
    void SetWindowType(WindowType type);
    void SetParentId(uint32_t parentId);
    void SetPersistentId(uint64_t persistentId);
    void SetParentPersistentId(uint64_t persistentId);

    const std::string& GetWindowName() const;
    const SessionInfo& GetSessionInfo() const;
    Rect GetWindowRect() const;
    Rect GetRequestRect() const;
    WindowType GetWindowType() const;
    bool GetFocusable() const;
    bool GetTouchable() const;
    uint32_t GetParentId() const;
    uint64_t GetDisplayId() const;
    uint64_t GetPersistentId() const;
    uint64_t GetParentPersistentId() const;

    bool Marshalling(Parcel& parcel) const override;
    static WindowSessionProperty* Unmarshalling(Parcel& parcel);
private:
    std::string windowName_;
    SessionInfo sessionInfo_;
    Rect requestRect_ { 0, 0, 0, 0 }; // window rect requested by the client (without decoration size)
    Rect windowRect_ { 0, 0, 0, 0 }; // actual window rect
    WindowType type_ { WindowType::WINDOW_TYPE_APP_MAIN_WINDOW }; // type main window
    bool focusable_ { true };
    bool touchable_ { true };
    uint64_t displayId_ = 0;
    uint32_t parentId_ = INVALID_SESSION_ID; // parentId of sceneSession, which is low 32 bite of parentPersistentId_
    uint64_t persistentId_ = INVALID_SESSION_ID;
    uint64_t parentPersistentId_ = INVALID_SESSION_ID;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_PROPERTY_H
