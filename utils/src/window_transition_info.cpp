/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include "window_transition_info.h"

namespace OHOS {
namespace Rosen {
WindowTransitionInfo::WindowTransitionInfo(sptr<AAFwk::AbilityTransitionInfo> info)
{
    bundleName_ = info->bundleName_;
    abilityName_ = info->abilityName_;
    mode_ = static_cast<WindowMode>(info->mode_);
    abilityToken_ = info->abilityToken_;
    displayId_ = info->displayId_;
    isShowWhenLocked_ = info->isShowWhenLocked_;
}

void WindowTransitionInfo::SetBundleName(std::string name)
{
    bundleName_ = name;
}

std::string WindowTransitionInfo::GetBundleName()
{
    return bundleName_;
}

void WindowTransitionInfo::SetAbilityName(std::string name)
{
    abilityName_ = name;
}

std::string WindowTransitionInfo::GetAbilityName()
{
    return abilityName_;
}

void WindowTransitionInfo::SetWindowMode(WindowMode mode)
{
    mode_ = mode;
}

WindowMode WindowTransitionInfo::GetWindowMode()
{
    return mode_;
}

void WindowTransitionInfo::SetWindowRect(Rect rect)
{
    windowRect_ = rect;
}

Rect WindowTransitionInfo::GetWindowRect()
{
    return windowRect_;
}

void WindowTransitionInfo::SetAbilityToken(sptr<IRemoteObject> abilityToken)
{
    abilityToken_ = abilityToken;
}

sptr<IRemoteObject> WindowTransitionInfo::GetAbilityToken()
{
    return abilityToken_;
}

void WindowTransitionInfo::SetDisplayId(DisplayId displayId)
{
    displayId_ = displayId;
}

DisplayId WindowTransitionInfo::GetDisplayId()
{
    return displayId_;
}

void WindowTransitionInfo::SetWindowType(WindowType windowType)
{
    windowType_ = windowType;
}

WindowType WindowTransitionInfo::GetWindowType()
{
    return windowType_;
}

void WindowTransitionInfo::SetShowFlagWhenLocked(bool isShow)
{
    isShowWhenLocked_ = isShow;
}

bool WindowTransitionInfo::GetShowFlagWhenLocked()
{
    return isShowWhenLocked_;
}

void WindowTransitionInfo::SetTransitionReason(TransitionReason reason)
{
    reason_ = reason;
}

TransitionReason WindowTransitionInfo::GetTransitionReason()
{
    return reason_;
}

bool WindowTransitionInfo::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteString(bundleName_) || !parcel.WriteString(abilityName_)) {
        return false;
    }

    if (!parcel.WriteUint32(static_cast<uint32_t>(mode_))) {
        return false;
    }

    if (!(parcel.WriteInt32(windowRect_.posX_) && parcel.WriteInt32(windowRect_.posY_) &&
        parcel.WriteUint32(windowRect_.width_) && parcel.WriteUint32(windowRect_.height_))) {
        return false;
    }

    if (!abilityToken_) {
        if (!parcel.WriteBool(false)) {
            return false;
        }
    } else {
        if (!parcel.WriteBool(true) || !parcel.WriteObject(abilityToken_)) {
            return false;
        }
    }

    if (!parcel.WriteUint64(displayId_)) {
        return false;
    }

    if (!parcel.WriteUint32(static_cast<uint32_t>(windowType_))) {
        return false;
    }

    if (!parcel.WriteBool(isShowWhenLocked_)) {
        return false;
    }

    if (!parcel.WriteUint32(static_cast<uint32_t>(reason_))) {
        return false;
    }
    return true;
}

WindowTransitionInfo* WindowTransitionInfo::Unmarshalling(Parcel& parcel)
{
    auto windowTransitionInfo = new(std::nothrow) WindowTransitionInfo();
    if (windowTransitionInfo == nullptr) {
        return nullptr;
    }
    windowTransitionInfo->bundleName_ = parcel.ReadString();
    windowTransitionInfo->abilityName_ = parcel.ReadString();
    windowTransitionInfo->mode_ = static_cast<WindowMode>(parcel.ReadUint32());
    windowTransitionInfo->windowRect_.posX_ = parcel.ReadInt32();
    windowTransitionInfo->windowRect_.posY_  = parcel.ReadInt32();
    windowTransitionInfo->windowRect_.width_  = parcel.ReadUint32();
    windowTransitionInfo->windowRect_.height_  = parcel.ReadUint32();
    if (parcel.ReadBool()) {
        windowTransitionInfo->abilityToken_ = parcel.ReadObject<IRemoteObject>();
    }
    windowTransitionInfo->displayId_ = parcel.ReadUint64();
    windowTransitionInfo->windowType_ = static_cast<WindowType>(parcel.ReadUint32());
    windowTransitionInfo->isShowWhenLocked_ = parcel.ReadBool();
    windowTransitionInfo->reason_ = static_cast<TransitionReason>(parcel.ReadUint32());
    return windowTransitionInfo;
}
} // Rosen
} // OHOS
