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
#include "move_resampler.h"
#include "property/rs_properties_def.h"
#include "screen_manager.h"
#include "window.h"
#include "ws_common_inner.h"

namespace OHOS::MMI {
class PointerEvent;
} // namespace MMI

namespace OHOS::Rosen {
class SceneSession;

using NotifyWindowDragHotAreaFunc = std::function<void(DisplayId displayId, uint32_t type, SizeChangeReason reason)>;
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

/**
 * @brief Describes how the targetRect (in MoveDragProperty) was (or will be)
 *        updated in response to a event during dragging or moving.
 */
enum class TargetRectUpdateState {
    // No change was applied to the targetRect.
    UNCHANGED,

    // The targetRect will be updated later on the next vsync using resampled data.
    RESAMPLE_REQUIRED,

    // The targetRect has been updated immediately.
    UPDATED_DIRECTLY
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

    void SetStartMoveFlag(bool flag);
    void SetStartDragFlag(bool flag);
    bool GetStartMoveFlag() const;
    bool GetStartDragFlag() const;
    bool HasPointDown();
    void SetMovable(bool movable);
    bool GetMovable() const;
    void SetNotifyWindowPidChangeCallback(const NotifyWindowPidChangeCallback& callback);

    void SetTargetRect(const WSRect& rect);

    /**
     * @brief Gets the targetRect stored in MoveDragProperty.
     *
     * The targetRect represents the window's position and size during
     * drag-move or drag-resize operations.
     *
     * @param coordinate The coordinate space to convert the targetRect into.
     * @return WSRect The targetRect in the specified coordinate space.
     */
    WSRect GetTargetRect(TargetRectCoordinate coordinate) const;

    /**
     * @brief Maps the targetRect (originally defined relative to the start display)
     *        into the coordinate space of the specified display.
     *
     * @param displayId The display whose coordinate space the targetRect should be mapped to.
     * @return WSRect The targetRect expressed in the coordinate space of the given display.
     */
    WSRect GetTargetRectByDisplayId(DisplayId displayId) const;

    /**
     * @brief Map a rectangle from the start display's coordinate space
     *        into the coordinate space of the target display.
     *
     * The input rect (relativeStartRect) is expressed relative to the top-left
     * corner of the display where dragging started. This function converts it
     * into the coordinate system of targetDisplayId by applying the offset
     * difference between the two displays in the legacy global coordinate system.
     *
     * @param relativeStartRect The rect defined in the start display's coordinate space.
     * @param targetDisplayId   The display to which the rect should be mapped.
     * @return WSRect           The mapped rect in the target display's coordinate space.
     */
    WSRect MapRectFromStartToTarget(const WSRect& relativeStartRect, DisplayId targetDisplayId) const;

    /**
     * @brief Map a rectangle from the target display's coordinate space
     *        back into the coordinate space of the start display.
     *
     * The input rect (relativeTargetRect) is expressed relative to the top-left
     * corner of targetDisplayId. This function converts it into the coordinate
     * system of the start display using the display offset difference derived
     * from the legacy global coordinate system.
     *
     * @param relativeTargetRect The rect defined in the target display's coordinate space.
     * @param targetDisplayId    The display where the rect is currently defined.
     * @return WSRect            The mapped rect in the start display's coordinate space.
     */
    WSRect MapRectFromTargetToStart(const WSRect& relativeTargetRect, DisplayId targetDisplayId) const;

    /**
     * @brief Compute the resampled target rectangle at the given vsync timestamp.
     *
     * Performs a move-resample step using the provided vsync time and returns
     * the resulting update state.
     *
     * @param vsyncTimeUs Timestamp of the vsync event, in microseconds.
     * @return TargetRectUpdateState Resulting state after resample computation.
     */
    TargetRectUpdateState ComputeResampledTargetRectOnVsync(int64_t vsyncTimeUs);

    void InitMoveDragProperty();
    void SetOriginalMoveDragPos(int32_t pointerId, int32_t pointerType, int32_t pointerPosX,
                                int32_t pointerPosY, int32_t pointerWindowX, int32_t pointerWindowY,
                                const WSRect& winRect);

    /**
     * @brief Handles pointer events related to window movement.
     *
     * Processes pointer actions used to drag and move the window on screen.
     *
     * @param pointerEvent Pointer event to be processed.
     * @return true if the event is handled; false otherwise.
     */
    bool ConsumeMoveEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    /**
     * @brief Handles pointer events related to window resizing.
     *
     * Processes pointer actions used to drag window edges or corners to resize the window.
     *
     * @param pointerEvent Pointer event to be processed.
     * @return true if the event is handled; false otherwise.
     */
    bool ConsumeDragEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    void ModifyWindowCoordinates(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
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

    /**
     * @brief Get the Gravity based on the dragAreaType_.
     *
     * @return The corresponding Gravity value.
     */
    Gravity GetGravity() const;

    /**
     * @brief Get the Gravity based on the AreaType.
     *
     * @param type The AreaType indicating the hot area.
     * @return The corresponding Gravity value.
     */
    Gravity GetGravity(AreaType type) const;

    /**
     * @brief Restore the gravity of the surfaceNode to the pre-drag state.
     *
     * @param surfaceNode The RSSurfaceNode of the window.
     * @return true if the gravity is successfully restored; false otherwise.
     */
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
    void SetOriginalPositionZ(float originalPositionZ) { originalPositionZ_ = originalPositionZ; }
    float GetOriginalPositionZ() const { return originalPositionZ_; }

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
    void HandleStartMovingWithCoordinate(const MoveCoordinateProperty& property, bool isMovable = true);
    void SetSpecifyMoveStartDisplay(DisplayId displayId);
    void ClearSpecifyMoveStartDisplay();
    void StopMoving();
    void SetLastDragEndRect(const WSRect& rect) { lastDragEndRect_ = rect; }
    WSRect GetLastDragEndRect() const { return lastDragEndRect_; }

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

    /**
     * @brief Determine whether to block cross-display events.
     *
     * @param pointerEvent The pointer event to evaluate.
     * @return true if the event should be blocked; false otherwise.
     */
    bool ShouldBlockCrossDisplay(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const;

    /**
     * @brief Update targetRect directly using the offset from the move start.
     *
     * @param offsetX X offset from the move start.
     * @param offsetY Y offset from the move start.
     */
    void UpdateTargetRectWithOffset(int32_t offsetX, int32_t offsetY);

    /**
     * @brief Process a pointer event during window moving and update targetRect accordingly.
     *
    * @param pointerEvent The current pointer event.
    * @return TargetRectUpdateState The state indicating how targetRect was (or will be) updated.
     */
    TargetRectUpdateState UpdateTargetRectOnMoveEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    /**
     * @brief Process a pointer event during window dragging and update targetRect accordingly.
     *
    * @param pointerEvent The current pointer event.
    * @return TargetRectUpdateState The state indicating how targetRect was (or will be) updated.
     */
    TargetRectUpdateState UpdateTargetRectOnDragEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    bool EventDownInit(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
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
    WSError UpdateMoveTempProperty(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    /**
     * @brief Validate whether the given pointer event can be used to start or continue a move operation.
     *
     * @param pointerEvent Incoming pointer event.
     * @return true if the event is valid for the current move state; false otherwise.
     */
    bool IsValidMoveEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    /**
     * @brief Process a pointer event during window moving and update targetRect accordingly.
     *
     * @param pointerEvent The current pointer event.
     * @param reason       The reason for the size or position change.
     */
    void ProcessMoveRectUpdate(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, SizeChangeReason reason);

    /**
     * @brief Handles the moving event (pointer move).
     *
     * @param pointerEvent Pointer event representing the movement.
     * @return true always, since move events are consumed regardless of interruption state.
     */
    bool HandleMoving(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    /**
     * @brief Handles the move end event (pointer up/cancel/down).
     *
     * @param pointerEvent Pointer event representing the move end or cancellation.
     * @return true if the event was processed successfully; false if the move state was invalid.
     */
    bool HandleMoveEnd(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    /**
     * @brief Validate whether the given pointer event can be used to start or continue a drag operation.
     *
     * @param pointerEvent Incoming pointer event.
     * @return true if the event is valid for the current drag state; false otherwise.
     */
    bool IsValidDragEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    /**
     * @brief Handles the drag start event (pointer down).
     *
     * @param pointerEvent Pointer event representing the drag start.
     * @return true if the event was processed successfully; false if initialization failed.
     */
    bool HandleDragStart(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    /**
     * @brief Handles the dragging event (pointer move).
     *
     * @param pointerEvent Pointer event representing the drag movement.
     * @return true always, since drag-move events are consumed regardless of interruption state.
     */
    bool HandleDragging(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    /**
     * @brief Handles the drag end event (pointer up/cancel).
     *
     * @param pointerEvent Pointer event representing the drag end or cancellation.
     * @return true always, since drag-end events should be consumed regardless of state validity.
     */
    bool HandleDragEnd(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    void ResSchedReportData(int32_t type, bool onOffTag);
    void NotifyWindowInputPidChange(bool isServerPid);

    /**
     * @brief Compute the pointer offset relative to the move or drag start position
     *        in the legacy global coordinate system (unified coordinate system).
     *
     * The legacy global coordinate system uses the top-left corner of the
     * minimum bounding rectangle covering all displays as (0, 0).
     *
     * @param pointerEvent The current pointer event.
     * @return std::pair<int32_t, int32_t> Offset (deltaX, deltaY) from the move or drag start;
     *         returns {0, 0} if the pointer item or display offset is unavailable.
     */
    std::pair<int32_t, int32_t> ComputeOffsetFromStart(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    /**
     * @brief Calls a SceneSession method if available; returns default otherwise.
     *
     * Promotes `sceneSession_`. If valid, calls the given member function with
     * arguments; otherwise returns `defaultValue`.
     *
     * @tparam Ret          Return type
     * @tparam Func         Member function pointer type
     * @tparam Args         Function argument types
     * @param  func         Member function pointer
     * @param  defaultValue Value returned if session is null
     * @param  args         Arguments for the member function
     * @return Ret          Function result or `defaultValue`
     */
    template <typename Ret, typename Func, typename... Args>
    Ret CallWithSceneSession(Func func, Ret defaultValue, Args&& ...args) const;

    /**
     * @brief Calls a SceneSession method if available.
     *
     * Promotes `sceneSession_`. If valid, calls the given member function with
     * arguments.
     *
     * @tparam Func Member function pointer type
     * @tparam Args Function argument types
     * @param  func Member function pointer
     * @param  args Arguments for the member function
     */
    template <typename Func, typename... Args>
    void CallVoidFuncWithSceneSession(Func func, Args&& ...args) const;

    /**
     * @brief Gets the SessionProperty from the SceneSession.
     *
     * @return The SessionProperty; nullptr if session is null.
     */
    sptr<WindowSessionProperty> GetSessionProperty() const;

    /**
     * @brief Gets the global or window rect from the SceneSession.
     *
     * @return The global or window rect; empty rect if session is null.
     */
    WSRect GetGlobalOrWinRect() const;

    /**
     * @brief Gets the move rectangle for window drag from the SceneSession.
     *
     * @return The move rectangle; empty rect if session is null.
     */
    WSRect GetMoveRectForWindowDrag() const;

    /**
     * @brief Invoked after a move/drag operation updates the target rectangle.
     *
     * The callback provides the reason for the size or position change and the
     * resulting update state of targetRect (unchanged, resample-required, or updated directly).
     *
     * @param reason The reason for the size or position change.
     * @param state  The targetRect update state.
     */
    void OnMoveDragCallback(
        SizeChangeReason reason, TargetRectUpdateState state = TargetRectUpdateState::UPDATED_DIRECTLY);

    /**
     * @brief Determines whether resampling operations should be performed for the given event.
     *
     * @param pointerEvent MMI input event.
     * @return True if resampling is allowed; false otherwise.
     */
    bool ShouldResampleMoveEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const;

    /**
     * @brief Check whether the pointer event is an invalid mouse event.
     *
     * A mouse event is considered invalid if it is from a mouse source but
     * is not triggered by the left mouse button.
     *
     * @param pointerEvent The pointer event to check.
     * @return true if the mouse event is invalid; false otherwise.
     */
    bool IsInvalidMouseEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const;

    /**
     * @brief Synchronize internal state from the SceneSession.
     *
     * @return true if synchronization is successful; false otherwise.
     */
    bool SyncPropertiesFromSceneSession();

    // Weak reference to the owning SceneSession.
    wptr<SceneSession> sceneSession_ = nullptr;

    // Properties that remain constant during the lifetime of SceneSession.
    // They are initialized once in the constructor and never updated afterward.
    int32_t persistentId_ = INVALID_WINDOW_ID;
    WindowType winType_ = WindowType::APP_WINDOW_BASE;

    // Properties that may change during the lifetime of SceneSession.
    // These fields are updated via SyncPropertiesFromSceneSession().
    float aspectRatio_ = 0.0f;
    WindowLimits limits_;
    WindowDecoration decoration_;
    bool supportCrossDisplay_ = false;

    bool isStartMove_ = false;
    bool isStartDrag_ = false;
    bool isMovable_ = true;
    bool hasPointDown_ = false;
    bool isAdaptToDragScale_ = false;
    float vpr_ = 1.0f;
    int32_t minTranX_ = INT32_MIN;
    int32_t minTranY_ = INT32_MIN;
    int32_t maxTranX_ = INT32_MAX;
    int32_t maxTranY_ = INT32_MAX;
    AreaType type_ = AreaType::UNDEFINED;
    AreaType dragAreaType_ = AreaType::UNDEFINED;
    AxisType mainMoveAxis_ = AxisType::UNDEFINED;
    MoveDragProperty moveDragProperty_;

    enum class DragType : uint32_t {
        DRAG_UNDEFINED,
        DRAG_LEFT_OR_RIGHT,
        DRAG_BOTTOM_OR_TOP,
        DRAG_LEFT_TOP_CORNER,
        DRAG_RIGHT_TOP_CORNER,
    };
    Rect parentRect_ { 0, 0, 0, 0};
    MoveTempProperty moveTempProperty_;

    void UpdateHotAreaType(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void ProcessWindowDragHotAreaFunc(bool flag, SizeChangeReason reason);
    uint32_t windowDragHotAreaType_ = WINDOW_HOT_AREA_TYPE_UNDEFINED;
    NotifyWindowDragHotAreaFunc windowDragHotAreaFunc_;
    NotifyWindowPidChangeCallback pidChangeCallback_;

    std::optional<Gravity> preDragGravity_ = std::nullopt;

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

    /**
     * @brief Resampler used to compute vsync-aligned move positions.
     *
     * Move resampling improves drag smoothness by compensating for the mismatch
     * between MMI (input) event frequency and display vsync frequency. Even with
     * uniform pointer motion, each vsync interval may receive a different number
     * of MMI events. Since rendering only uses the latest event per frame, this
     * can cause visible jitter during window movement.
     *
     * Resampling computes a vsync-aligned position to produce smoother, more
     * stable motion.
     */
    MoveResampler moveResampler_;

    /**
     * @brief Whether move-resample is enabled for window moving.
     *
     * Note that even when this flag is true, resampling only
     * occurs if @ref ShouldResampleMoveEvent also returns true.
     */
    bool enableMoveResample_ = false;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_MOVE_DRAG_CONTROLLER_H
