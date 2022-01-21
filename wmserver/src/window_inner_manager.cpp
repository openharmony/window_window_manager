/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "window_inner_manager.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkImageInfo.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_surface_extractor.h"

#include "window_life_cycle_interface.h"
#include "window_manager_hilog.h"
#include "window_option.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowInnerManager"};
}

WM_IMPLEMENT_SINGLE_INSTANCE(WindowInnerManager)

void WindowInnerManager::DrawSurface(const sptr<Window>& window, uint32_t color)
{
    auto surfaceNode = window->GetSurfaceNode();
    auto winRect = window->GetRect();
    WLOGFI("SurfaceWindowDraw winRect, x : %{public}d, y : %{public}d, width: %{public}d, height: %{public}d",
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);

    auto width = winRect.width_;
    auto height = winRect.height_;

    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        WLOGFE("RSSurface is nullptr");
        return;
    }
#ifdef ACE_ENABLE_GL
    rsSurface->SetRenderContext(rc_);
#endif
    auto frame = rsSurface->RequestFrame(width, height);
    std::unique_ptr<RSSurfaceFrame> framePtr = std::move(frame);
    if (!framePtr) {
        WLOGFE("DrawSurface frameptr is nullptr");
        return;
    }
    auto canvas = framePtr->GetCanvas();
    if (!canvas) {
        WLOGFE("DrawSurface canvas is nullptr");
        return;
    }
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    const SkScalar skWidth = 20;
    paint.setStrokeWidth(skWidth);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setColor(color);

    SkPoint rectPts[] = { {0, 0}, {static_cast<int>(width), static_cast<int>(height)} };
    SkRect rect;
    rect.set(rectPts[0], rectPts[1]);

    canvas->drawRect(rect, paint);
    framePtr->SetDamageRegion(0, 0, width, height);
    auto framPtr1 = std::move(framePtr);
    rsSurface->FlushFrame(framPtr1);
}


sptr<Window> WindowInnerManager::GetDividerWindow(uint32_t displayId) const
{
    auto iter = dividerMap_.find(displayId);
    if (iter == dividerMap_.end()) {
        return nullptr;
    }
    return iter->second;
}

sptr<Window> WindowInnerManager::CreateWindow(uint32_t displayId, const WindowType& type, const Rect& rect)
{
    sptr<Window> window = GetDividerWindow(displayId);
    if (window == nullptr) {
        sptr<WindowOption> divWindowOp = new WindowOption();
        divWindowOp->SetWindowRect(rect);
        divWindowOp->SetWindowType(type);
        divWindowOp->SetFocusable(false);
        divWindowOp->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
        window = Window::Create("divider" + std::to_string(displayId), divWindowOp);
        if (window == nullptr) {
            WLOGFE("Window is nullptr");
            return nullptr;
        }
        dividerMap_.insert(std::make_pair(displayId, window));
        WLOGFI("CreateWindow success");
    }
    return window;
}

void WindowInnerManager::CreateAndShowDivider()
{
    auto window = CreateWindow(winMsg_->displayId, WindowType::WINDOW_TYPE_DOCK_SLICE, winMsg_->dividerRect);
    if (window == nullptr) {
        return;
    }

    WMError res = window->Show();
    if (res != WMError::WM_OK) {
        WLOGFE("Show window failed");
        return;
    }

#ifdef ACE_ENABLE_GL
    WLOGFI("Draw divider on gpu");
    // init render context
    static bool hasInitRC = false;
    if (!hasInitRC) {
        rc_ = RenderContextFactory::GetInstance().CreateEngine();
        if (rc_) {
            rc_->InitializeEglContext();
            hasInitRC = true;
        } else {
            WLOGFE("InitilizeEglContext failed");
            return;
        }
    }
#endif
    const uint32_t colorGrey = 0xff808080;
    DrawSurface(window, colorGrey);
    WLOGFI("CreateAndShowDivider success");
}

void WindowInnerManager::HideAndDestroyDivider()
{
    sptr<Window> window = GetDividerWindow(winMsg_->displayId);
    if (window == nullptr) {
        WLOGFE("Window is nullptr");
        return;
    }
    WMError res = window->Destroy();
    if (res != WMError::WM_OK) {
        WLOGFE("Destroy window failed");
        return;
    }
    dividerMap_.erase(winMsg_->displayId);
    WLOGFI("HideAndDestroyDivider success");
}

void WindowInnerManager::DestroyThread()
{
    hasInitThread_ = false;
    needDestroyThread_ = true;
    WLOGFI("DestroyThread success");
}

void WindowInnerManager::HandleMessage()
{
    WLOGFI("HandleMessage");
    while (!needDestroyThread_) {
        std::unique_lock<std::mutex> lk(mutex_);
        conVar_.wait(lk, [this] { return ready_; });

        auto cmdType = winMsg_->cmdType;
        using Func_t = void(WindowInnerManager::*)();
        static const std::map<InnerWMCmd, Func_t> funcMap = {
            std::make_pair(INNER_WM_CREATE_DIVIDER,  &WindowInnerManager::CreateAndShowDivider),
            std::make_pair(INNER_WM_DESTROY_DIVIDER, &WindowInnerManager::HideAndDestroyDivider),
            std::make_pair(INNER_WM_DESTROY_THREAD,  &WindowInnerManager::DestroyThread)
        };
        auto it = funcMap.find(cmdType);
        if (it != funcMap.end()) {
            (this->*(it->second))();
        }
        ready_ = false;
    }
}

void WindowInnerManager::SendMessage(InnerWMCmd cmdType, uint32_t displayId)
{
    if (!hasInitThread_) {
        WLOGFI("Inner window manager thread has not been created");
        return;
    }
    winMsg_->cmdType = cmdType;
    winMsg_->displayId = displayId;

    ready_ = true;
    conVar_.notify_one();

    WLOGFI("SendMessage : displayId = %{public}d,  type = %{public}d",
        winMsg_->displayId, static_cast<uint32_t>(cmdType));
}

void WindowInnerManager::SendMessage(InnerWMCmd cmdType, uint32_t displayId, const Rect& dividerRect)
{
    if (!hasInitThread_) {
        WLOGFI("Inner window manager thread has not been created");
        return;
    }
    winMsg_->cmdType = cmdType;
    winMsg_->displayId = displayId;
    winMsg_->dividerRect = dividerRect;

    ready_ = true;
    conVar_.notify_one();

    WLOGFI("SendMessage : displayId = %{public}d,  type = %{public}d" \
        " Rect = [%{public}d %{public}d %{public}d %{public}d]",
        winMsg_->displayId, static_cast<uint32_t>(cmdType),
        winMsg_->dividerRect.posX_, winMsg_->dividerRect.posY_,
        winMsg_->dividerRect.width_, winMsg_->dividerRect.height_);
}


void WindowInnerManager::Init()
{
    std::unique_lock<std::mutex> lk(mutex_);
    needDestroyThread_ = false;
    winMsg_ = std::make_unique<WindowMessage>();

    // create inner thread
    std::thread innerWMThread(&WindowInnerManager::HandleMessage, this);
    innerWMThread.detach();
    hasInitThread_ = true;
    WLOGFI("Inner window manager thread create success");
}
}
}