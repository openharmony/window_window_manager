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

#include <optional>
#include <mutex>

#include <refbase.h>
#include <struct_multimodal.h>

#include "common/include/window_session_property.h"
#include "property/rs_properties_def.h"
#include "screen_manager.h"
#include "window.h"
#include "ws_common_inner.h"

namespace OHOS::MMI {
class PointerEvent;
} // namespace MMI

namespace OHOS::Rosen {
class SceneSession;

using MoveDragCallback = std::function<void(const SizeChangeReason)>;

using NotifyWindowDragHotAreaFunc = std::function<void(DisplayId displayId, uint32_t type,
    SizeChangeReason reason)>;

using NotifyWindowPidChangeCallback = std::function<void(int32_t windowId, bool startMoving)>;

const uint32_t WINDOW_HOT_AREA_TYPE_UNDEFINED = 0;
const int32_t POSITIVE_CORRELATION = 1;
const int32_t NEGATIVE_CORRELATION = -1;

enum class MoveDirection : uint32_t {
    UNKNOWN,
    LEFT_TO_RIGHT,
    RIGHT_TO_LEFT,
    UP_TO_BOTTOM,
    BOTTOM_TO_UP,
};

class MoveDragController : public ScreenManager::IScreenListener {
public:
    MoveDragController(wptr<SceneSession> sceneSession);
    ~MoveDragController() = default;

    /*
     * Cross Display Move Drag
     */
    enum class TargetRectCoordinate {
        RELATED_TO_START_DISPLAY,
        RELATED_TO_END_DISPLAY,
        GLOBAL
    };

    void RegisterMoveDragCallback(const MoveDragCallback& callBack);
    void SetStartMoveFlag(bool flag);
    void SetStartDragFlag(bool flag);
    bool GetStartMoveFlag() const;
    bool GetStartDragFlag() const;
    bool HasPointDown();
    void SetMovable(bool movable);
    bool GetMovable() const;
    void SetNotifyWindowPidChangeCallback(const NotifyWindowPidChangeCallback& callback);
    WSRect GetTargetRect(TargetRectCoordinate coordinate) const;
    WSRect GetTargetRectByDisplayId(DisplayId displayId) const;
    void SetTargetRect(const WSRect& rect);
    WSRect GetOriginalRect() const;
    void InitMoveDragProperty();
    void SetOriginalMoveDragPos(int32_t pointerId, int32_t pointerType, int32_t pointerPosX,
                                int32_t pointerPosY, int32_t pointerWindowX, int32_t pointerWindowY,
                                const WSRect& winRect);
    void SetAspectRatio(float ratio);
    bool ConsumeMoveEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& originalRect);
    bool ConsumeDragEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& originalRect,
        const sptr<WindowSessionProperty> property, const SystemSessionConfig& sysConfig);
    void ModifyWindowCoordinatesWhenMoveEnd(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void CalcFirstMoveTargetRect(const WSRect& windowRect, bool useWindowRect);
    WSRect GetFullScreenToFloatingRect(const WSRect& originalRect, const WSRect& windowRect);
    int32_t GetOriginalPointerPosX();
    int32_t GetOriginalPointerPosY();
    int32_t GetPointerType() const;
    void SetWindowDragHotAreaFunc(const NotifyWindowDragHotAreaFunc& func);
    void UpdateGravityWhenDrag(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode);
    void UpdateSubWindowGravityWhenFollow(const sptr<MoveDragController>& followedController,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode);
    void OnLostFocus();
    AreaType GetAreaType() const { return type_; };
    void SetScale(float scalex, float scaley);
    void SetParentRect(const Rect& parentRect);
    Gravity GetGravity() const;
    Gravity GetGravity(AreaType type) const;
    bool RestoreToPreDragGravity(const std::shared_ptr<RSSurfaceNode>& surfaceNode);

    /*
     * Cross Display Move Drag
     */
    uint64_t GetMoveDragStartDisplayId() const;
    uint64_t GetMoveDragEndDisplayId() const;
    uint64_t GetInitParentNodeId() const;
    std::set<uint64_t> GetDisplayIdsDuringMoveDrag();
    std::set<uint64_t> GetNewAddedDisplayIdsDuringMoveDrag();
    void InitCrossDisplayProperty(DisplayId displayId, uint64_t parentNodeId);
    WSRect GetScreenRectById(DisplayId displayId);
    DisplayId GetMoveInputBarStartDisplayId();
    void ResetCrossMoveDragProperty();
    void MoveDragInterrupted(bool resetPosition = true);
    void SetMoveAvailableArea(const DMRect& area);
    void UpdateMoveAvailableArea(DisplayId targetDisplayId);
    void SetCurrentScreenProperty(DisplayId targetDisplayId);
    void SetMoveInputBarStartDisplayId(DisplayId displayId);
    void SetInputBarCrossAttr(MoveDirection moveDirection, DisplayId targetDisplayId);
    void SetOriginalDisplayOffset(int32_t offsetX, int32_t offsetY);
    void SetOriginalPositionZ(float originalPositionZ) { originalPositionZ_ = originalPositionZ; }
    float GetOriginalPositionZ() const { return originalPositionZ_; }
    bool IsSupportWindowDragCrossDisplay();

    /*
     * Monitor screen connection status
     */
    void OnConnect(ScreenId screenId) override;
    void OnDisconnect(ScreenId screenId) override;
    void OnChange(ScreenId screenId) override;

    /*
     * PC Window Layout
     */
    struct MoveCoordinateProperty {
        int32_t pointerWindowX = 0;
        int32_t pointerWindowY = 0;
        int32_t pointerPosX = 0;
        int32_t pointerPosY = 0;
        DisplayId displayId = DISPLAY_ID_INVALID;
        WSRect winRect = { 0, 0, 0, 0 };
    };
    void HandleStartMovingWithCoordinate(const MoveCoordinateProperty& property);
    void SetSpecifyMoveStartDisplay(DisplayId displayId);
    void ClearSpecifyMoveStartDisplay();
    WSRect GetTargetDisplayRectRelatedToStartDisplay(WSRect rect, DisplayId displayId) const;
    void StopMoving();
    void SetLastDragEndRect(const WSRect& rect) { lastDragEndRect_ = rect; }
    WSRect GetLastDragEndRect() const { return lastDragEndRect_; }
    void SetWindowDecoration(const WindowDecoration& decoration) { decoration_ = decoration; }

private:
    struct MoveDragProperty {
        int32_t pointerId_ = -1;
        int32_t pointerType_ = -1;
        int32_t originalPointerPosX_ = -1;
        int32_t originalPointerPosY_ = -1;
        // the x coordinate of the pointer related to the active window
        int32_t originalPointerWindowX_ = -1;
        // the y coordinate of the pointer related to the active window
        int32_t originalPointerWindowY_ = -1;
        // the x coordinate scale of the active window
        float scaleX_ = 1.0f;
        // the y coordinate scale of the active window
        float scaleY_ = 1.0f;
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

    struct ScreenSizeProperty {
        uint32_t currentDisplayStartX = 0;
        uint32_t currentDisplayStartY = 0;
        int32_t currentDisplayLeft = 0;
        int32_t currentDisplayTop = 0;
        int32_t width = 0;
        int32_t height = 0;

        bool IsEmpty() const
        {
            return (currentDisplayStartX == 0 && currentDisplayStartY == 0 && currentDisplayLeft == 0 &&
                    currentDisplayTop == 0 && width == 0 && height == 0);
        }

        void Reset()
        {
            currentDisplayStartX = 0;
            currentDisplayStartY = 0;
            currentDisplayLeft = 0;
            currentDisplayTop = 0;
            width = 0;
            height = 0;
        }

        std::string ToString() const
        {
            if (IsEmpty()) {
                return "empty";
            }

            std::ostringstream ss;
            ss << "currentDisplayStartX: " << currentDisplayStartX << ","
               << "currentDisplayStartY: " << currentDisplayStartY << ","
               << "currentDisplayLeft: " << currentDisplayLeft << ","
               << "currentDisplayTop: " << currentDisplayTop << ","
               << "width: " << width << "," << "height: " << height;
            return ss.str();
        }
    };
    enum AxisType { UNDEFINED, X_AXIS, Y_AXIS };
    constexpr static float NEAR_ZERO = 0.001f;

    bool CalcMoveTargetRect(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& originalRect);
    void CalcDragTargetRect(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, SizeChangeReason reason);
    bool EventDownInit(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& originalRect,
        const sptr<WindowSessionProperty> property, const SystemSessionConfig& sysConfig);
    bool CalcMoveInputBarRect(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& originalRect);
    void AdjustTargetPositionByAvailableArea(int32_t& moveDragFinalX, int32_t& moveDragFinalY);
    MoveDirection CalcMoveDirection(DisplayId lastDisplayId, DisplayId currentDisplayId);

    void InitializeMoveDragPropertyNotValid(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
                                            const WSRect& originalRect);
    bool CheckAndInitializeMoveDragProperty(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
                                            const WSRect& originalRect);
    void HandleLeftToRightCross(DisplayId targetDisplayId, int32_t pointerDisplayX, int32_t pointerDisplayY,
            int32_t& moveDragFinalX, int32_t& moveDragFinalY);
    void HandleRightToLeftCross(DisplayId targetDisplayId, int32_t pointerDisplayX, int32_t pointerDisplayY,
            int32_t& moveDragFinalX, int32_t& moveDragFinalY);
    void HandleUpToBottomCross(DisplayId targetDisplayId, int32_t pointerDisplayX, int32_t pointerDisplayY,
            int32_t& moveDragFinalX, int32_t& moveDragFinalY);
    void HandleBottomToUpCross(DisplayId targetDisplayId, int32_t pointerDisplayX, int32_t pointerDisplayY,
            int32_t& moveDragFinalX, int32_t& moveDragFinalY);
    void CalcMoveForSameDisplay(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
                                int32_t& moveDragFinalX, int32_t& moveDragFinalY);
    AreaType GetAreaType(int32_t pointWinX, int32_t pointWinY, int32_t sourceType, const WSRect& rect);
    WSRect CalcFreeformTargetRect(AreaType type, int32_t tranX, int32_t tranY, WSRect originalRect);
    WSRect CalcFixedAspectRatioTargetRect(AreaType type, int32_t tranX, int32_t tranY, float aspectRatio,
        WSRect originalRect);
    void CalcFreeformTranslateLimits(AreaType type);
    void CalcFixedAspectRatioTranslateLimits(AreaType type);
    void FixTranslateByLimits(int32_t& tranX, int32_t& tranY);
    bool InitMainAxis(AreaType type, int32_t tranX, int32_t tranY);
    void ConvertXYByAspectRatio(int32_t& tx, int32_t& ty, float aspectRatio);
    int32_t ConvertByAreaType(int32_t tran) const;
    void ProcessSessionRectChange(SizeChangeReason reason);
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

    /*
     * Cross Display Move Drag
     */
    std::pair<int32_t, int32_t> CalcUnifiedTranslate(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    wptr<SceneSession> sceneSession_ = nullptr;
    bool isStartMove_ = false;
    bool isStartDrag_ = false;
    bool isMovable_ = true;
    bool isDecorEnable_ = true;
    bool hasPointDown_ = false;
    bool isAdaptToDragScale_ = false;
    float aspectRatio_ = 0.0f;
    float vpr_ = 1.0f;
    int32_t minTranX_ = INT32_MIN;
    int32_t minTranY_ = INT32_MIN;
    int32_t maxTranX_ = INT32_MAX;
    int32_t maxTranY_ = INT32_MAX;
    AreaType type_ = AreaType::UNDEFINED;
    AreaType dragAreaType_ = AreaType::UNDEFINED;
    AxisType mainMoveAxis_ = AxisType::UNDEFINED;
    WindowLimits limits_;
    WindowDecoration decoration_;
    MoveDragProperty moveDragProperty_;
    MoveDragCallback moveDragCallback_;
    int32_t persistentId_ = INVALID_WINDOW_ID;
    WindowType winType_ = WindowType::APP_WINDOW_BASE;

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
    Rect parentRect_ { 0, 0, 0, 0};
    uint32_t mouseStyleID_ = 0;
    DragType dragType_ = DragType::DRAG_UNDEFINED;
    MoveTempProperty moveTempProperty_;

    void UpdateHotAreaType(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void ProcessWindowDragHotAreaFunc(bool flag, SizeChangeReason reason);
    uint32_t windowDragHotAreaType_ = WINDOW_HOT_AREA_TYPE_UNDEFINED;
    NotifyWindowDragHotAreaFunc windowDragHotAreaFunc_;
    NotifyWindowPidChangeCallback pidChangeCallback_;

    std::optional<Gravity> preDragGravity_ = std::nullopt;
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

    /*
     * Cross Display Move Drag
     */
    bool moveDragIsInterrupted_ = false;
    DisplayId moveDragStartDisplayId_ = DISPLAY_ID_INVALID;
    DisplayId moveDragEndDisplayId_ = DISPLAY_ID_INVALID;
    uint64_t initParentNodeId_ = -1ULL;
    DisplayId hotAreaDisplayId_ = 0;
    int32_t originalDisplayOffsetX_ = 0;
    int32_t originalDisplayOffsetY_ = 0;
    float originalPositionZ_ = 0.0f;
    std::mutex displayIdSetDuringMoveDragMutex_;
    std::set<uint64_t> displayIdSetDuringMoveDrag_;
    DMRect moveAvailableArea_ = {0, 0, 0, 0};
    DisplayId moveInputBarStartDisplayId_ = DISPLAY_ID_INVALID;
    ScreenSizeProperty screenSizeProperty_;
    // Above guarded by displayIdSetDuringMoveDragMutex_
    std::mutex specifyMoveStartMutex_;
    DisplayId specifyMoveStartDisplayId_ = DISPLAY_ID_INVALID;
    bool isSpecifyMoveStart_ = false;
    bool isAdaptToProportionalScale_ = false;
    // Above guarded by specifyMoveStartMutex_

    /*
     * PC Window Layout
     */
    WSRect lastDragEndRect_ = { 0, 0, 0, 0 };
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_MOVE_DRAG_CONTROLLER_H
