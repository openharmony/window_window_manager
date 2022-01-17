/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_JS_WINDOW_UTILS_H
#define OHOS_JS_WINDOW_UTILS_H
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "window.h"
#include "window_option.h"
#include "wm_common.h"
namespace OHOS {
namespace Rosen {
namespace {
    constexpr size_t ARGC_ONE = 1;
    constexpr size_t ARGC_TWO = 2;
    constexpr size_t ARGC_THREE = 3;
    constexpr int32_t INDEX_ONE = 1;
    constexpr int32_t INDEX_TWO = 2;
    constexpr int32_t INDEX_THREE = 3;
    constexpr int32_t RGB_LENGTH = 7;
    constexpr int32_t RGBA_LENGTH = 8;
}

    NativeValue* GetRectAndConvertToJsValue(NativeEngine& engine, const Rect rect);
    NativeValue* CreateJsWindowPropertiesObject(NativeEngine& engine, sptr<Window>& window);
    bool SetSystemBarPropertiesFromJs(NativeEngine& engine, NativeObject* jsObject,
        std::map<WindowType, SystemBarProperty>& properties, sptr<Window>& window);
    bool GetSystemBarStatus(std::map<WindowType, SystemBarProperty>& systemBarProperties,
        NativeEngine& engine, NativeCallbackInfo& info, sptr<Window>& window);
    NativeValue* CreateJsSystemBarRegionTintArrayObject(NativeEngine& engine,
        const SystemBarProps& props);
    NativeValue* ChangeAvoidAreaToJsValue(NativeEngine& engine, const AvoidArea& avoidArea);
}
}
#endif