/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WM_ANIMATION_COMMON_H
#define OHOS_ROSEN_WM_ANIMATION_COMMON_H

#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

#include <parcel.h>

namespace OHOS {
namespace Rosen {
/**
* @brief Enumerates window transition type.
*/
enum class WindowTransitionType : uint32_t {
    /**
     * window destroy.
     */
    DESTROY = 0,
    
    /**
     * end type.
     */
    END,
};

/**
* @brief Enumerates window animation curve type.
*/
enum class WindowAnimationCurve : uint32_t {
    /**
     * animation curve type linear.
     */
    LINEAR = 0,

    /**
     * animation curve type interpolation_spring.
     */
    INTERPOLATION_SPRING = 1,

    /**
     * animation curve type cubic bezier curve.
     */
    CUBIC_BEZIER = 2,
};

/**
 * Describes the window animation type
 */
enum class AnimationType : uint32_t {
    /**
     * window animation type fade in out
     */
    FADE_IN_OUT = 0,
    /**
     * window animation type fade in
     */
    FADE_IN = 1,
    /**
     * end type
     */
    END,
};

const uint32_t ANIMATION_PARAM_SIZE = 4;
const uint32_t ANIMATION_MAX_DURATION = 3000;
const uint32_t FLOAT_VALUE_LENGTH = 2;

// LCOV_EXCL_START
struct WindowAnimationProperty : public Parcelable {
    float targetScale = 0.0f;

    bool Marshalling(Parcel& parcel) const override
    {
        if (!parcel.WriteFloat(targetScale)) {
            return false;
        }
        return true;
    }

    static WindowAnimationProperty* Unmarshalling(Parcel& parcel)
    {
        WindowAnimationProperty* animationProperty = new WindowAnimationProperty();
        if (!parcel.ReadFloat(animationProperty->targetScale)) {
            delete animationProperty;
            return nullptr;
        }
        return animationProperty;
    }
};
// LCOV_EXCL_STOP

/**
* @brief Window transition animation configuration.
*/
struct WindowAnimationOption : public Parcelable {
    WindowAnimationCurve curve = WindowAnimationCurve::LINEAR;
    uint32_t duration = 0;
    std::array<float, ANIMATION_PARAM_SIZE> params;

    // LCOV_EXCL_START
    bool Marshalling(Parcel& parcel) const override
    {
        if (!(parcel.WriteUint32(static_cast<uint32_t>(curve)) && parcel.WriteUint32(duration))) {
            return false;
        }
        if (params.size() > ANIMATION_PARAM_SIZE) {
            return false;
        }
        for (const auto& param: params) {
            if (!parcel.WriteFloat(param)) {
                return false;
            }
        }
        return true;
    }
    // LCOV_EXCL_STOP

    static WindowAnimationOption* Unmarshalling(Parcel& parcel)
    {
        WindowAnimationOption* windowAnimationConfig = new WindowAnimationOption();
        uint32_t curve = 0;
        if (!parcel.ReadUint32(curve)) {
            delete windowAnimationConfig;
            return nullptr;
        }
        windowAnimationConfig->curve = static_cast<WindowAnimationCurve>(curve);
        if (!parcel.ReadUint32(windowAnimationConfig->duration)) {
            delete windowAnimationConfig;
            return nullptr;
        }
        if (windowAnimationConfig->params.size() > ANIMATION_PARAM_SIZE) {
            delete windowAnimationConfig;
            return nullptr;
        }
        for (auto& param: windowAnimationConfig->params) {
            if (!parcel.ReadFloat(param)) {
                delete windowAnimationConfig;
                return nullptr;
            }
        }
        return windowAnimationConfig;
    }

    // LCOV_EXCL_START
    std::string ToString() const
    {
        std::ostringstream oss;
        oss << "curve: " << std::to_string(static_cast<int32_t>(curve)) << ", duration: " << \
            std::to_string(static_cast<int32_t>(duration)) << ", params: [ ";
        for (auto param: params) {
            oss << std::fixed << std::setprecision(FLOAT_VALUE_LENGTH) << param << ", ";
        }
        oss << "]";
        return oss.str();
    }
    // LCOV_EXCL_STOP
};

/**
 * The animation configuration of start scene animation
 */
struct StartAnimationOptions : public Parcelable {
    /**
     * The type of window animation
     */
    AnimationType animationType = AnimationType::FADE_IN_OUT;

    // LCOV_EXCL_START
    bool Marshalling(Parcel& parcel) const override
    {
        if (!parcel.WriteUint32(static_cast<uint32_t>(animationType))) {
            return false;
        }
        return true;
    }
    // LCOV_EXCL_STOP

    static StartAnimationOptions* Unmarshalling(Parcel& parcel)
    {
        StartAnimationOptions* startAnimationOptions = new StartAnimationOptions();
        uint32_t animationType = 0;
        if (!parcel.ReadUint32(animationType)) {
            delete startAnimationOptions;
            return nullptr;
        }
        startAnimationOptions->animationType = static_cast<AnimationType>(animationType);
        return startAnimationOptions;
    }
};

/**
 * The animation configuration of SA start scene animation
 */
struct StartAnimationSystemOptions : public Parcelable {
    /**
     * The type of window animation
     */
    AnimationType animationType = AnimationType::FADE_IN_OUT;
    /**
     * The config of start secne animation
     */
    std::shared_ptr<WindowAnimationOption> animationConfig = nullptr;

    // LCOV_EXCL_START
    bool Marshalling(Parcel& parcel) const override
    {
        if (!parcel.WriteUint32(static_cast<uint32_t>(animationType))) {
            return false;
        }
        if (!parcel.WriteParcelable(animationConfig.get())) {
            return false;
        }
        return true;
    }
    // LCOV_EXCL_STOP

    static StartAnimationSystemOptions* Unmarshalling(Parcel& parcel)
    {
        StartAnimationSystemOptions* startAnimationSystemOptions = new StartAnimationSystemOptions();
        uint32_t animationType = 0;
        if (!parcel.ReadUint32(animationType)) {
            delete startAnimationSystemOptions;
            return nullptr;
        }
        startAnimationSystemOptions->animationType = static_cast<AnimationType>(animationType);
        startAnimationSystemOptions->animationConfig =
            std::shared_ptr<WindowAnimationOption>(parcel.ReadParcelable<WindowAnimationOption>());
        return startAnimationSystemOptions;
    }
};

/**
 * The animation params of window create
 */
struct WindowCreateParams : public Parcelable {
    /**
     * The animation configuration of start scene animation
     */
    std::shared_ptr<StartAnimationOptions> animationParams = nullptr;
    /**
     * The animation configuration of SA start scene animation
     */
    std::shared_ptr<StartAnimationSystemOptions> animationSystemParams = nullptr;

    // LCOV_EXCL_START
    bool Marshalling(Parcel& parcel) const override
    {
        if (!parcel.WriteParcelable(animationParams.get())) {
            return false;
        }
        if (!parcel.WriteParcelable(animationSystemParams.get())) {
            return false;
        }
        return true;
    }
    // LCOV_EXCL_STOP

    static WindowCreateParams* Unmarshalling(Parcel& parcel)
    {
        WindowCreateParams* windowCreateParams = new WindowCreateParams();
        windowCreateParams->animationParams =
            std::shared_ptr<StartAnimationOptions>(parcel.ReadParcelable<StartAnimationOptions>());
        windowCreateParams->animationSystemParams =
            std::shared_ptr<StartAnimationSystemOptions>(parcel.ReadParcelable<StartAnimationSystemOptions>());
        return windowCreateParams;
    }
};

/**
* @brief Transition animation configuration.
*/
struct TransitionAnimation : public Parcelable {
    WindowAnimationOption config;
    float opacity = 1.0f;

    // LCOV_EXCL_START
    bool Marshalling(Parcel& parcel) const override
    {
        if (!(parcel.WriteFloat(opacity) && parcel.WriteParcelable(&config))) {
            return false;
        }
        return true;
    }
    // LCOV_EXCL_STOP

    static TransitionAnimation* Unmarshalling(Parcel& parcel)
    {
        TransitionAnimation* transitionAnimation = new TransitionAnimation();
        if (!parcel.ReadFloat(transitionAnimation->opacity)) {
            delete transitionAnimation;
            return nullptr;
        }
        std::shared_ptr<WindowAnimationOption> animationConfig =
            std::shared_ptr<WindowAnimationOption>(parcel.ReadParcelable<WindowAnimationOption>());
        if (animationConfig == nullptr) {
            delete transitionAnimation;
            return nullptr;
        }
        transitionAnimation->config = *animationConfig;
        return transitionAnimation;
    }
};
}
}
#endif // OHOS_ROSEN_WM_ANIMATION_COMMON_H
