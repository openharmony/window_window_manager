/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_DISPLAY_UTILS_H
#define OHOS_DISPLAY_UTILS_H

#include <cstdint>
#include <memory>
#include <string>

#define FFI_EXPORT __attribute__((visibility("default")))

extern "C" {
typedef struct {
    int32_t code;
    int64_t len;
    void* data;
} RetStruct;

typedef struct {
    uint32_t foldDisplayMode;
    uint32_t physicalWidth;
    uint32_t physicalHeight;
} CDisplayPhysicalResolution;

typedef struct {
    int32_t left;
    int32_t top;
    uint32_t width;
    uint32_t height;
} CRect;

typedef struct {
    CRect left;
    CRect top;
    CRect right;
    CRect bottom;
} CWaterfallDisplayAreaRects;

typedef struct {
    CRect* boundingRects;
    int64_t number;
    CWaterfallDisplayAreaRects waterfallDisplayAreaRects;
} CCutoutInfo;

typedef struct {
    uint32_t displayId;
    int64_t number;
    CRect* creaseRects;
} CFoldCreaseRegion;
}

namespace OHOS {
namespace Rosen {
enum class DisplayRetCode : int32_t {
    RET_OK = 0,
    RET_ERR = -1,
    RET_FIND_EXISTS = 0xf0000000,
};
}
} // namespace OHOS

#endif // OHOS_DISPLAY_UTILS_H
