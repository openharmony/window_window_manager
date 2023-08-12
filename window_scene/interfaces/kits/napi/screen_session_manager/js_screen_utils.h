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

#ifndef OHOS_WINDOW_SCENE_JS_SCREEN_UTILS_H
#define OHOS_WINDOW_SCENE_JS_SCREEN_UTILS_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "session/screen/include/screen_property.h"

namespace OHOS::Rosen {
bool ConvertRRectFromJs(NativeEngine& engine, NativeObject* jsObject, RRect& bound);
class JsScreenUtils {
public:
    static NativeValue* CreateJsScreenProperty(NativeEngine& engine, const ScreenProperty& screenProperty);
    static NativeValue* CreateJsRRect(NativeEngine& engine, const RRect& rrect);
    static NativeValue* CreateJsScreenConnectChangeType(NativeEngine& engine);
};
} // namespace OHOS::Rosen

#endif // OHOS_WINDOW_SCENE_JS_SCREEN_UTILS_H
