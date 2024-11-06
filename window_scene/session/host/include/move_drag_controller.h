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

#include <mutex>

#include <refbase.h>
#include <struct_multimodal.h>

#include "common/include/window_session_property.h"
#include "property/rs_properties_def.h"
#include "window.h"
#include "screen_manager.h"
#include "ws_common_inner.h"

namespace OHOS::MMI {
class PointerEvent;
} // namespace MMI

namespace OHOS::Rosen {

using MoveDragCallback = std::function<void(const SizeChangeReason)>;

using NotifyWindowDragHotAreaFunc = std::function<void(DisplayId displayId, uint32_t type,
    const SizeChangeReason reason)>;

using NotifyWindowPidChangeCallback = std::function<void(int32_t windowId, bool startMoving)>;

const uint32_t WINDOW_HOT_AREA_TYPE_UNDEFINED = 0;

class MoveDragController : public ScreenManager::IScreenListener {
public:
    MoveDragController(int32_t persistentId, bool isSystemWindow = false);
    ~MoveDragController() = default;

    /**
     * Cross Display Move Drag
     */
    enum class TargetRectCoordinate {
        RELATED_TO_START_DISPLAY,
        RELATED_TO_END_DISPLAY,
        GLOBAL
    };

    void RegisterMoveDragCallback(const MoveDragCallback& callBack);
    void SetStartMoveFlag(bool flag);
    bool GetStartMoveFlag() const;
    bool GetStartDragFlag() const;
    void SetAsSystemWindow(bool isSystemWindow);
    bool IsSystemWindow() const;
    bool HasPointDown();
    void SetMovable(bool movable);
    bool GetMovable() const;
    void SetNotifyWindowPidChangeCallback(const NotifyWindowPidChangeCallback& callback);
    WSRect GetTargetRect(TargetRectCoordinate coordinate = TargetRectCoordinate::RELATED_TO_START_DISPLAY) const;
    void InitMoveDragProperty();
    void SetOriginalValue(int32_t pointerId, int32_t pointerType,
        int32_t pointerPosX, int32_t pointerPosY, const WSRect& winRect);
    void SetAspectRatio(float ratio);
    bool ConsumeMoveEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& originalRect);
    bool ConsumeDragEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& originalRect,
        const sptr<WindowSessionProperty> property, const SystemSessionConfig& sysConfig);
    void CalcFirstMoveTargetRect(const WSRect& windowRect, bool isFullToFloating);
    WSRect GetFullScreenToFloatingRect(const WSRect& originalRect, const WSRect& windowRect);
    int32_t GetOriginalPointerPosX();
    int32_t GetOriginalPointerPosY();
    void SetWindowDragHotAreaFunc(const NotifyWindowDragHotAreaFunc& func);
    void UpdateGravityWhenDrag(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode);
    void OnLostFocus();
    void SetIsPcWindow(bool isPcWindow);

    /**
     * Cross Display Move Drag
     */
    uint64_t GetMoveDragStartDisplayId() const;
    uint64_t GetMoveDragEndDisplayId() const;
    uint64_t GetInitParentNodeId() const;
    std::set<uint64_t> GetDisplayIdsDuringMoveDrag();
    std::set<uint64_t> GetNewAddedDisplayIdsDuringMoveDrag();
    void InitCrossDisplayProperty(DisplayId displayId, uint64_t parentNodeId);
    WSRect GetScreenRectById(DisplayId displayId);
    void MoveDragInterrupt();
    void ResetCrossMoveDragProperty();

    /**
     * Monitor screen connection status
     */
    void OnConnect(ScreenId screenId) override;
    void OnDisconnect(ScreenId screenId) override;
    void OnChange(ScreenId screenId) override;

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
    void CalcDragTargetRect(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
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
    void ProcessSessionRectChange(const SizeChangeReason reason);
    void InitDecorValue(const sptr<WindowSessionProperty> property, const SystemSessionConfig& sysConfig);

    float GetVirtualPixelRatio() const;
    void UpdateDragType(int32_t startPointPosX, int32_t startPointPosY);
    bool IsPointInDragHotZone(int32_t startPointPosX, int32_t startPointPosY,
        int32_t sourceType, const WSRect& winRect);
    void CalculateStartRectExceptHotZone(float vpr, const WSRect& winRect);
    WSError UpdateMoveTempProperty(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    bool CheckDragEventLegal(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        const sptr<WindowSessionProperty> property);
    void ResSchedReportData(int32_t type, bool onOffTag);
    void NotifyWindowInputPidChange(bool isServerPid);

    /**
     * Cross Display Move Drag
     */
    std::pair<int32_t, int32_t> CalcUnifiedTranslate(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    bool isStartMove_ = false;
    bool isStartDrag_ = false;
    bool isMovable_ = true;
    bool isDecorEnable_ = true;
    bool hasPointDown_ = false;
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
    bool isPcWindow_ = false;

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

    void UpdateHotAreaType(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void ProcessWindowDragHotAreaFunc(bool flag, const SizeChangeReason reason);
    uint32_t windowDragHotAreaType_ = WINDOW_HOT_AREA_TYPE_UNDEFINED;
    NotifyWindowDragHotAreaFunc windowDragHotAreaFunc_;
    NotifyWindowPidChangeCallback pidChangeCallback_;

    const std::map<AreaType, Gravity> GRAVITY_MAP = {
        {AreaType::LEFT,          Gravity::TOP_RIGHT},
        {AreaType::TOP,           Gravity::BOTTOM_LEFT},
        {AreaType::RIGHT,         Gravity::TOP_LEFT},
        {AreaType::BOTTOM,        Gravity::TOP_LEFT},
        {AreaType::LEFT_TOP,      Gravity::BOTTOM_RIGHT},
        {AreaType::RIGHT_TOP,     Gravity::BOTTOM_LEFT},
        {AreaType::RIGHT_BOTTOM,  Gravity::TOP_LEFT},
        {AreaType::LEFT_BOTTOM,   Gravity::TOP_RIGHT}
    };

    /**
     * Cross Display Move Drag
     */
    bool isSystemWindow_ = false;
    bool moveDragIsInterrupted_ = false;
    DisplayId moveDragStartDisplayId_ = DISPLAY_ID_INVALID;
    DisplayId moveDragEndDisplayId_ = DISPLAY_ID_INVALID;
    uint64_t initParentNodeId_ = -1ULL;
    DisplayId hotAreaDisplayId_ = 0;
    int32_t originalDisplayOffsetX_ = 0;
    int32_t originalDisplayOffsetY_ = 0;
    std::mutex displayIdSetDuringMoveDragMutex_;
    std::set<uint64_t> displayIdSetDuringMoveDrag_;
    // Above guarded by displayIdSetDuringMoveDragMutex_
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_MOVE_DRAG_CONTROLLER_H
