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
#include <struct_multimodal.h>

#include "common/include/window_session_property.h"

namespace OHOS::MMI {
class PointerEvent;
} // namespace MMI

namespace OHOS::Rosen {

using MoveDragCallback = std::function<void(const SizeChangeReason&)>;

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
    MoveDragController(int32_t persistentId);
    ~MoveDragController() = default;

    void RegisterMoveDragCallback(const MoveDragCallback& callBack);
    void SetStartMoveFlag(bool flag);
    bool GetStartMoveFlag() const;
    bool GetStartDragFlag() const;
    WSRect GetTargetRect() const;
    void InitMoveDragProperty();
    void SetOriginalValue(int32_t pointerId, int32_t pointerType,
        int32_t pointerPosX, int32_t pointerPosY, const WSRect& winRect);
    void SetAspectRatio(float ratio);
    bool ConsumeMoveEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& originalRect);
    bool ConsumeDragEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& originalRect,
        const sptr<WindowSessionProperty> property, const SystemSessionConfig& sysConfig);
    void HandleMouseStyle(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& winRect);
    void ClacFirstMoveTargetRect(const WSRect& windowRect);

private:
    struct MoveDragProperty {
        int32_t pointerId_ = -1;
        int32_t pointerType_ = -1;
        int32_t originalPointerPosX_ = -1;
        int32_t originalPointerPosY_ = -1;
        WSRect originalRect_ = { 0, 0, 0, 0 };
        WSRect targetRect_ = { 0, 0, 0, 0 };

        bool isEmpty() const
        {
            return (pointerId_ == -1 && originalPointerPosX_ == -1 && originalPointerPosY_ == -1);
        }
    };

    struct MoveTempProperty {
        int32_t pointerId_ = -1;
        int32_t pointerType_ = -1;
        int32_t lastDownPointerPosX_ = -1;
        int32_t lastDownPointerPosY_ = -1;
        int32_t lastDownPointerWindowX_ = -1;
        int32_t lastDownPointerWindowY_ = -1;
        int32_t lastMovePointerPosX_ = -1;
        int32_t lastMovePointerPosY_ = -1;

        bool isEmpty() const
        {
            return (pointerId_ == -1 && lastDownPointerPosX_ == -1 && lastDownPointerPosY_ == -1);
        }
    };

    enum AxisType { UNDEFINED, X_AXIS, Y_AXIS };
    constexpr static float NEAR_ZERO = 0.001f;

    bool CalcMoveTargetRect(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& originalRect);
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
    void ProcessSessionRectChange(const SizeChangeReason& reason);
    void InitDecorValue(const sptr<WindowSessionProperty> property, const SystemSessionConfig& sysConfig);

    float GetVirtualPixelRatio() const;
    void UpdateDragType(int32_t startPointPosX, int32_t startPointPosY);
    bool IsPointInDragHotZone(int32_t startPointPosX, int32_t startPointPosY,
        int32_t sourceType, const WSRect& winRect);
    void CalculateStartRectExceptHotZone(float vpr, const WSRect& winRect);
    WSError UpdateMoveTempProperty(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    bool CheckDragEventLegal(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        const sptr<WindowSessionProperty> property);

    bool isStartMove_ = false;
    bool isStartDrag_ = false;
    bool isDecorEnable_ = true;
    float aspectRatio_ = 0.0f;
    float vpr_ = 1.0f;
    int32_t minTranX_ = INT32_MIN;
    int32_t minTranY_ = INT32_MIN;
    int32_t maxTranX_ = INT32_MAX;
    int32_t maxTranY_ = INT32_MAX;
    AreaType type_ = AreaType::UNDEFINED;
    AxisType mainMoveAxis_ = AxisType::UNDEFINED;
    WindowLimits limits_;
    MoveDragProperty moveDragProperty_;
    MoveDragCallback moveDragCallback_;
    int32_t persistentId_;

    enum class DragType : uint32_t {
        DRAG_UNDEFINED,
        DRAG_LEFT_OR_RIGHT,
        DRAG_BOTTOM_OR_TOP,
        DRAG_LEFT_TOP_CORNER,
        DRAG_RIGHT_TOP_CORNER,
    };
    const std::map<DragType, uint32_t> STYLEID_MAP = {
        {DragType::DRAG_UNDEFINED,        MMI::MOUSE_ICON::DEFAULT},
        {DragType::DRAG_BOTTOM_OR_TOP,    MMI::MOUSE_ICON::NORTH_SOUTH},
        {DragType::DRAG_LEFT_OR_RIGHT,    MMI::MOUSE_ICON::WEST_EAST},
        {DragType::DRAG_LEFT_TOP_CORNER,  MMI::MOUSE_ICON::NORTH_WEST_SOUTH_EAST},
        {DragType::DRAG_RIGHT_TOP_CORNER, MMI::MOUSE_ICON::NORTH_EAST_SOUTH_WEST}
    };
    Rect rectExceptFrame_ { 0, 0, 0, 0 };
    Rect rectExceptCorner_ { 0, 0, 0, 0 };
    uint32_t mouseStyleID_ = 0;
    DragType dragType_ = DragType::DRAG_UNDEFINED;
    MoveTempProperty moveTempProperty_;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_MOVE_DRAG_CONTROLLER_H
