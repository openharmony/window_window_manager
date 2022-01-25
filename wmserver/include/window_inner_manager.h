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

#ifndef OHOS_ROSEN_WINDOW_INNER_MANAGER_H
#define OHOS_ROSEN_WINDOW_INNER_MANAGER_H

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#ifdef ACE_ENABLE_GL
#include "render_context/render_context.h"
#endif
#include "include/core/SkBitmap.h"
#include "single_instance.h"
#include "singleton_delegator.h"
#include "window.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
enum InnerWMCmd : uint32_t {
    INNER_WM_CREATE_DIVIDER,
    INNER_WM_DESTROY_DIVIDER,
    INNER_WM_DESTROY_THREAD,
};

struct WindowMessage {
    InnerWMCmd cmdType;
    uint32_t displayId;
    Rect dividerRect;
};

class WindowInnerManager {
WM_DECLARE_SINGLE_INSTANCE(WindowInnerManager);
public:
    void Init();
    void SendMessage(InnerWMCmd cmdType, uint32_t displayId = 0);
    void SendMessage(InnerWMCmd cmdType, uint32_t displayId, const Rect& rect);
    void HandleMessage();
private:
    static inline SingletonDelegator<WindowInnerManager> delegator;

    sptr<Window> CreateWindow(uint32_t displayId, const WindowType& type, const Rect& rect);
    void CreateAndShowDivider(std::unique_ptr<WindowMessage> msg);
    void HideAndDestroyDivider(std::unique_ptr<WindowMessage> msg);
    void DestroyThread(std::unique_ptr<WindowMessage> msg);
    void DrawSurface(const sptr<Window>& window);
    sptr<Window> GetDividerWindow(uint32_t displayId) const;
    bool DecodeImageFile(const char* filename, SkBitmap* bitmap);

    std::mutex mutex_;
    std::condition_variable conVar_;
    bool ready_ = false;
#ifdef ACE_ENABLE_GL
    RenderContext* rc_ = nullptr;
#endif
    std::map<uint32_t, sptr<Window>> dividerMap_;
    std::vector<std::unique_ptr<WindowMessage>> messages_;
    bool hasInitThread_ = false;
    bool needDestroyThread_ = false;
    bool isDeviderImageLoaded = false;
    const char *splitIconPath_ = "/etc/window/resources/bg_split_handle.png";
    SkBitmap deviderBitmap;
};
}
}
#endif // OHOS_ROSEN_WINDOW_INNER_MANAGER_H
