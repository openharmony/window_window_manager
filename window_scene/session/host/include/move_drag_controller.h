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

#ifndef OHOS_ROSEN_WINDOW_SCENE_MOVE_DRAG_CONTROLLER_H
#define OHOS_ROSEN_WINDOW_SCENE_MOVE_DRAG_CONTROLLER_H

#include <refbase.h>

#include "common/include/window_session_property.h"

namespace OHOS::MMI {
class PointerEvent;
} // namespace MMI

namespace OHOS::Rosen {

using NotifyVsyncHandleFunc = std::function<void(void)>;

enum class AreaType : uint32_t {
    UNDEFINED = 0,
    LEFT = 1 << 0,
    TOP = 1 << 1,
    RIGHT = 1 << 2,
    BOTTOM = 1 << 3,
    LEFT_TOP = LEFT | TOP,
    RIGHT_TOP = RIGHT | TOP,
    RIGHT_BOTTOM = RIGHT | BOTTOM,
    LEFT_BOTTOM = LEFT | BOTTOM,
};

class MoveDragController : public RefBase {
public:
    struct MoveDragProperty {
        int32_t pointerId_;
        int32_t originalPointerPosX_;
        int32_t originalPointerPosY_;
        WSRect originalRect_;
        WSRect targetRect_;

        bool isEmpty() const
        {
            return (pointerId_ == -1 && originalPointerPosX_ == -1 && originalPointerPosY_ == -1);
        }
    };

    MoveDragController();
    ~MoveDragController();

    void SetVsyncHandleListenser(const NotifyVsyncHandleFunc& func);
    void SetStartMoveFlag(bool flag);
    bool GetStartMoveFlag() const;
    WSRect GetTargetRect() const;
    void InitMoveDragProperty();
    void SetAspectRatio(float ratio);
    WSError ConsumeMoveEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& originalRect);
    bool ConsumeDragEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& originalRect,
        const sptr<WindowSessionProperty> property, const SystemSessionConfig& sysConfig);

private:
    enum AxisType { UNDEFINED, X_AXIS, Y_AXIS };
    constexpr static float NEAR_ZERO = 0.001f;

    void CalcMoveTargetRect(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& originalRect);
    bool EventDownInit(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& originalRect,
        const sptr<WindowSessionProperty> property, const SystemSessionConfig& sysConfig);
    AreaType GetAreaType(int32_t pointWinX, int32_t pointWinY, int32_t sourceType, const WSRect& rect);
    WSRect CalcFreeformTargetRect(AreaType type, int32_t tranX, int32_t tranY, WSRect originalRect);
    WSRect CalcFixedAspectRatioTargetRect(AreaType type, int32_t tranX, int32_t tranY, float aspectRatio,
        WSRect originalRect);
    void CalcFreeformTranslateLimits(AreaType type);
    void CalcFixedAspectRatioTranslateLimits(AreaType type, AxisType axis);
    void FixTranslateByLimits(int32_t& tranX, int32_t& tranY);
    bool InitMainAxis(AreaType type, int32_t tranX, int32_t tranY);
    void ConvertXYByAspectRatio(int32_t& tx, int32_t& ty, float aspectRatio);
    void RequestVsync(void);
    void RemoveVsync();
    void OnReceiveVsync(int64_t timeStamp);
    void InitDecorValue(const sptr<WindowSessionProperty> property, const SystemSessionConfig& sysConfig);

    bool isStartMove_ = false;
    bool isStartDrag_ = false;
    bool isDecorEnable_ = true;
    float aspectRatio_ = 0.0f;
    float vpr_ = 1.0f;
    int32_t minTranX_ = INT32_MIN;
    int32_t minTranY_ = INT32_MIN;
    int32_t maxTranX_ = INT32_MAX;
    int32_t maxTranY_ = INT32_MAX;
    int32_t maxFloatingWindowSize_ = 1920;  // 1920: default max size of floating window
    AreaType type_ = AreaType::UNDEFINED;
    AxisType mainMoveAxis_ = AxisType::UNDEFINED;
    WindowLimits limits_;
    MoveDragProperty moveDragProperty_ = { -1, -1, -1, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
    std::shared_ptr<VsyncCallback> vsyncCallback_ = std::make_shared<VsyncCallback>(VsyncCallback());
    NotifyVsyncHandleFunc vsyncHandleFunc_;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_MOVE_DRAG_CONTROLLER_H
