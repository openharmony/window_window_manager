/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <cinttypes>

#include "include/core/SkCanvas.h"
#include "include/codec/SkCodec.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"

#include "vsync_station.h"
#include "window_life_cycle_interface.h"
#include "window_manager_hilog.h"
#include "window_option.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowInnerManager"};
}

WM_IMPLEMENT_SINGLE_INSTANCE(WindowInnerManager)

bool WindowInnerManager::DecodeImageFile(const char* filename, SkBitmap& bitmap)
{
    sk_sp<SkData> data(SkData::MakeFromFileName(filename));
    std::unique_ptr<SkCodec> codec = SkCodec::MakeFromData(std::move(data));
    if (codec == nullptr) {
        return false;
    }
    SkColorType colorType = kN32_SkColorType;
    SkImageInfo info = codec->getInfo().makeColorType(colorType);
    if (!bitmap.tryAllocPixels(info)) {
        return false;
    }
    return SkCodec::kSuccess == codec->getPixels(info, bitmap.getPixels(), bitmap.rowBytes());
}

void WindowInnerManager::DrawSurface(const sptr<Window>& window)
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
    if (!isDividerImageLoaded_) {
        isDividerImageLoaded_ = DecodeImageFile(splitIconPath_, dividerBitmap_);
    }
    if (!dividerBitmap_.isNull() && dividerBitmap_.width() != 0 && dividerBitmap_.height() != 0) {
        DrawBitmap(rsSurface, width, height);
    } else {
        DrawColor(rsSurface, width, height);
    }
}

void WindowInnerManager::DrawBitmap(std::shared_ptr<RSSurface>& rsSurface, uint32_t width, uint32_t height)
{
    std::unique_ptr<RSSurfaceFrame> frame = rsSurface->RequestFrame(width, height);
    if (frame == nullptr) {
        WLOGFE("DrawSurface frameptr is nullptr");
        return;
    }
    auto canvas = frame->GetCanvas();
    if (canvas == nullptr) {
        WLOGFE("DrawSurface canvas is nullptr");
        return;
    }
    canvas->clear(SK_ColorTRANSPARENT);
    SkPaint paint;
    SkMatrix matrix;
    SkRect rect;
    rect.set(0, 0, static_cast<int>(width), static_cast<int>(height));
    if (dividerBitmap_.width() != 0 && dividerBitmap_.height() != 0) {
        if (width > height) {
            // rotate when divider is horizontal
            matrix.setScale(static_cast<float>(height) / dividerBitmap_.width(),
                static_cast<float>(width) / dividerBitmap_.height());
            matrix.postRotate(-90.0f); // divider shader rotate -90.0
        } else {
            matrix.setScale(static_cast<float>(width) / dividerBitmap_.width(),
                static_cast<float>(height) / dividerBitmap_.height());
        }
    }
    paint.setShader(dividerBitmap_.makeShader(SkTileMode::kRepeat, SkTileMode::kRepeat));
    if (paint.getShader() == nullptr) {
        WLOGFE("DrawBitmap shader is nullptr");
        return;
    }
    paint.setShader(paint.getShader()->makeWithLocalMatrix(matrix));
    canvas->drawRect(rect, paint);
    frame->SetDamageRegion(0, 0, width, height);
    rsSurface->FlushFrame(frame);
}

void WindowInnerManager::DrawColor(std::shared_ptr<RSSurface>& rsSurface, uint32_t width, uint32_t height)
{
    std::unique_ptr<RSSurfaceFrame> frame = rsSurface->RequestFrame(width, height);
    if (frame == nullptr) {
        WLOGFE("DrawSurface frameptr is nullptr");
        return;
    }
    auto canvas = frame->GetCanvas();
    if (canvas == nullptr) {
        WLOGFE("DrawSurface canvas is nullptr");
        return;
    }
    canvas->clear(SK_ColorTRANSPARENT);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    const SkScalar skWidth = 20; // stroke width is 20.
    paint.setStrokeWidth(skWidth);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setColor(DIVIDER_HANDLE_COLOR);

    SkPoint rectPts[] = { {0, 0}, {static_cast<int>(width), static_cast<int>(height)} };
    SkRect rect;
    rect.set(rectPts[0], rectPts[1]);

    canvas->drawRect(rect, paint);
    frame->SetDamageRegion(0, 0, width, height);
    rsSurface->FlushFrame(frame);
    return;
}

sptr<Window> WindowInnerManager::GetDividerWindow(DisplayId displayId) const
{
    auto iter = dividerMap_.find(displayId);
    if (iter == dividerMap_.end()) {
        return nullptr;
    }
    return iter->second;
}

sptr<Window> WindowInnerManager::CreateWindow(DisplayId displayId, const WindowType& type, const Rect& rect)
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

void WindowInnerManager::CreateAndShowDivider(std::unique_ptr<WindowMessage> msg)
{
    auto window = CreateWindow(msg->displayId, WindowType::WINDOW_TYPE_DOCK_SLICE, msg->dividerRect);
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
    DrawSurface(window);
    WLOGFI("CreateAndShowDivider success");
}

void WindowInnerManager::HideAndDestroyDivider(std::unique_ptr<WindowMessage> msg)
{
    sptr<Window> window = GetDividerWindow(msg->displayId);
    if (window == nullptr) {
        WLOGFE("Window is nullptr");
        return;
    }
    WMError res = window->Hide();
    if (res != WMError::WM_OK) {
        WLOGFE("Hide window failed");
        return;
    }
    res = window->Destroy();
    if (res != WMError::WM_OK) {
        WLOGFE("Destroy window failed");
        return;
    }
    dividerMap_.erase(msg->displayId);
    WLOGFI("HideAndDestroyDivider success");
}

void WindowInnerManager::DestroyThread(std::unique_ptr<WindowMessage> msg)
{
    hasInitThread_ = false;
    needDestroyThread_ = true;
    isDividerImageLoaded_ = false;
    WLOGFI("DestroyThread success");
}

void WindowInnerManager::HandleMessage()
{
    WLOGFI("HandleMessage");
    std::vector<std::unique_ptr<WindowMessage>> handleMsgs;
    while (!needDestroyThread_) {
        // lock to store massages
        {
            std::unique_lock<std::mutex> lk(mutex_);
            conVar_.wait(lk, [this] { return ready_; });
            for (auto& iter: messages_) {
                handleMsgs.push_back(std::move(iter));
            }
            messages_.clear();
            ready_ = false;
        }
        // loop to handle massages
        for (auto& msg : handleMsgs) {
            if (!msg) {
                continue;
            }
            auto cmdType = msg->cmdType;
            using Func_t = void(WindowInnerManager::*)(std::unique_ptr<WindowMessage> winMsg);
            static const std::map<InnerWMCmd, Func_t> funcMap = {
                std::make_pair(INNER_WM_CREATE_DIVIDER,  &WindowInnerManager::CreateAndShowDivider),
                std::make_pair(INNER_WM_DESTROY_DIVIDER, &WindowInnerManager::HideAndDestroyDivider),
                std::make_pair(INNER_WM_DESTROY_THREAD,  &WindowInnerManager::DestroyThread)};
            auto it = funcMap.find(cmdType);
            if (it != funcMap.end()) {
                (this->*(it->second))(std::move(msg));
            }
        }
        handleMsgs.clear();
    }
}

void WindowInnerManager::SendMessage(InnerWMCmd cmdType, DisplayId displayId)
{
    std::unique_lock<std::mutex> lk(mutex_);
    if (!hasInitThread_) {
        WLOGFI("Inner window manager thread has not been created");
        return;
    }
    std::unique_ptr<WindowMessage> winMsg = std::make_unique<WindowMessage>();
    if (!winMsg) {
        WLOGFI("alloc winMsg failed");
        return;
    }
    winMsg->cmdType = cmdType;
    winMsg->displayId = displayId;
    WLOGFI("SendMessage : displayId = %{public}" PRIu64",  type = %{public}d",
        winMsg->displayId, static_cast<uint32_t>(cmdType));
    messages_.push_back(std::move(winMsg));
    ready_ = true;
    conVar_.notify_one();
}

void WindowInnerManager::Init()
{
    std::unique_lock<std::mutex> lk(mutex_);
    needDestroyThread_ = false;
    // create inner thread
    std::thread innerWMThread(&WindowInnerManager::HandleMessage, this);
    innerWMThread.detach();
    hasInitThread_ = true;
    VsyncStation::GetInstance().SetIsMainHandlerAvailable(false);
    WLOGFI("Inner window manager thread create success");
}
}
}