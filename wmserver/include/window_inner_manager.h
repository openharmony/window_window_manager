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

#ifndef OHOS_ROSEN_WINDOW_INNER_MANAGER_H
#define OHOS_ROSEN_WINDOW_INNER_MANAGER_H

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "include/core/SkBitmap.h"
#ifdef ACE_ENABLE_GL
#include "render_context/render_context.h"
#endif
#include "transaction/rs_transaction.h"
#include "ui/rs_surface_extractor.h"
#include "wm_single_instance.h"
#include "singleton_delegator.h"
#include "window.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
enum class InnerWMCmd : uint32_t {
    INNER_WM_CREATE_DIVIDER,
    INNER_WM_DESTROY_DIVIDER,
    INNER_WM_DESTROY_THREAD,
};

struct WindowInnerMessage {
    InnerWMCmd cmdType;
    DisplayId displayId;
    Rect dividerRect;
};

class WindowInnerManager {
WM_DECLARE_SINGLE_INSTANCE(WindowInnerManager);
public:
    void Init();
    void SendMessage(InnerWMCmd cmdType, DisplayId displayId = 0);
private:
    static inline SingletonDelegator<WindowInnerManager> delegator;

    void HandleMessage();
    sptr<Window> CreateWindow(DisplayId displayId, const WindowType& type, const Rect& rect);
    void CreateAndShowDivider(std::unique_ptr<WindowInnerMessage> msg);
    void HideAndDestroyDivider(std::unique_ptr<WindowInnerMessage> msg);
    void DestroyThread(std::unique_ptr<WindowInnerMessage> msg);
    void DrawSurface(const sptr<Window>& window);
    void DrawColor(std::shared_ptr<RSSurface>& rsSurface, uint32_t width, uint32_t height);
    void DrawBitmap(std::shared_ptr<RSSurface>& rsSurface, uint32_t width, uint32_t height);
    sptr<Window> GetDividerWindow(DisplayId displayId) const;
    bool DecodeImageFile(const char* filename, SkBitmap& bitmap);

    std::mutex mutex_;
    std::condition_variable conVar_;
    bool ready_ = false;
#ifdef ACE_ENABLE_GL
    std::unique_ptr<RenderContext> renderContext_;
#endif
    std::map<uint32_t, sptr<Window>> dividerMap_;
    std::vector<std::unique_ptr<WindowInnerMessage>> messages_;
    bool hasInitThread_ = false;
    bool needDestroyThread_ = false;
    bool isDividerImageLoaded_ = false;
    const char *splitIconPath_ = "/etc/window/resources/bg_split_handle.png";
    uint32_t DIVIDER_HANDLE_COLOR = 0xff808080; // gray
    SkBitmap dividerBitmap_;
};
}
}
#endif // OHOS_ROSEN_WINDOW_INNER_MANAGER_H
