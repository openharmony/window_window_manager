/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OH_DISPLAY_H
#define OH_DISPLAY_H

#ifndef _INTEROP_TYPES_H_
#define _INTEROP_TYPES_H_

#include <stdint.h>

#define INTEROP_FATAL(msg, ...) do { fprintf(stderr, msg "\n", ##__VA_ARGS__); abort(); } while (0)

typedef enum InteropTag
{
  INTEROP_TAG_UNDEFINED = 101,
  INTEROP_TAG_INT32 = 102,
  INTEROP_TAG_FLOAT32 = 103,
  INTEROP_TAG_STRING = 104,
  INTEROP_TAG_LENGTH = 105,
  INTEROP_TAG_RESOURCE = 106,
  INTEROP_TAG_OBJECT = 107,
} InteropTag;

typedef enum InteropRuntimeType
{
  INTEROP_RUNTIME_UNEXPECTED = -1,
  INTEROP_RUNTIME_NUMBER = 1,
  INTEROP_RUNTIME_STRING = 2,
  INTEROP_RUNTIME_OBJECT = 3,
  INTEROP_RUNTIME_BOOLEAN = 4,
  INTEROP_RUNTIME_UNDEFINED = 5,
  INTEROP_RUNTIME_BIGINT = 6,
  INTEROP_RUNTIME_FUNCTION = 7,
  INTEROP_RUNTIME_SYMBOL = 8,
  INTEROP_RUNTIME_MATERIALIZED = 9,
} InteropRuntimeType;

typedef float InteropFloat32;
typedef double InteropFloat64;
typedef int32_t InteropInt32;
typedef unsigned int InteropUInt32; // TODO: update unsigned int
typedef int64_t InteropInt64;
typedef uint64_t InteropUInt64;
typedef int8_t InteropInt8;
typedef uint8_t InteropUInt8;
typedef int64_t InteropDate;
typedef int8_t InteropBoolean;
typedef const char* InteropCharPtr;
typedef void* InteropNativePointer;

struct _InteropVMContext;
typedef struct _InteropVMContext* InteropVMContext;
struct _InteropPipelineContext;
typedef struct _InteropPipelineContext* InteropPipelineContext;
struct _InteropVMObject;
typedef struct _InteropVMObject* InteropVMObject;
struct _InteropNode;
typedef struct _InteropNode* InteropNodeHandle;
typedef struct InteropDeferred {
    void* handler;
    void* context;
    void (*resolve)(struct InteropDeferred* thiz, uint8_t* data, int32_t length);
    void (*reject)(struct InteropDeferred* thiz, const char* message);
} InteropDeferred;

// Binary layout of InteropString must match that of KStringPtrImpl.
typedef struct InteropString {
  const char* chars;
  InteropInt32 length;
} InteropString;

typedef struct InteropEmpty {
  InteropInt32 dummy; // Empty structs are forbidden in C.
} InteropEmpty;

typedef struct InteropNumber {
  InteropInt8 tag;
  union {
    InteropFloat32 f32;
    InteropInt32 i32;
  };
} InteropNumber;

// Binary layout of InteropLength must match that of KLength.
typedef struct InteropLength
{
  InteropInt8 type;
  InteropFloat32 value;
  InteropInt32 unit;
  InteropInt32 resource;
} InteropLength;

typedef struct InteropCustomObject {
  char kind[20];
  InteropInt32 id;
  // Data of custom object.
  union {
    InteropInt32 ints[4];
    InteropFloat32 floats[4];
    void* pointers[4];
    InteropString string;
  };
} InteropCustomObject;

typedef struct InteropUndefined {
  InteropInt32 dummy; // Empty structs are forbidden in C.
} InteropUndefined;

typedef struct InteropVoid {
  InteropInt32 dummy; // Empty structs are forbidden in C.
} InteropVoid;

typedef struct InteropFunction {
  InteropInt32 id;
} InteropFunction;
typedef InteropFunction InteropCallback;
typedef InteropFunction InteropErrorCallback;

typedef struct InteropMaterialized {
  InteropNativePointer ptr;
} InteropMaterialized;

typedef struct InteropCallbackResource {
  InteropInt32 resourceId;
  void (*hold)(InteropInt32 resourceId);
  void (*release)(InteropInt32 resourceId);
} InteropCallbackResource;

typedef struct InteropBuffer {
  InteropCallbackResource resource;
  InteropNativePointer data;
  InteropInt64 length;
} InteropBuffer;

#endif // _INTEROP_TYPES_H_


#define DISPLAY_API_VERSION 1

#include <stdint.h>

/* clang-format off */

#ifdef __cplusplus
extern "C" {
#endif

typedef InteropTag OH_Tag;
typedef InteropRuntimeType OH_DISPLAY_RuntimeType;

typedef InteropFloat32 OH_Float32;
typedef InteropFloat64 OH_Float64;
typedef InteropInt32 OH_Int32;
typedef InteropUInt32 OH_UInt32;
typedef InteropInt64 OH_Int64;
typedef InteropUInt64 OH_UInt64;
typedef InteropInt8 OH_Int8;
typedef InteropBoolean OH_Boolean;
typedef InteropCharPtr OH_CharPtr;
typedef InteropNativePointer OH_NativePointer;
typedef InteropString OH_String;
typedef InteropCallbackResource OH_DISPLAY_CallbackResource;
typedef InteropNumber OH_Number;
typedef InteropMaterialized OH_Materialized;
typedef InteropCustomObject OH_CustomObject;
typedef InteropUndefined OH_Undefined;
// typedef InteropAPIKind OH_APIKind;
typedef InteropVMContext OH_DISPLAY_VMContext;
typedef InteropBuffer OH_Buffer;
typedef InteropLength OH_Length;
typedef InteropFunction OH_Function;

typedef enum OH_APIKind {
    OH_DISPLAY_API_KIND = 100
} OH_APIKind;

typedef struct OH_AnyAPI {
    OH_Int32 version;
} OH_AnyAPI;

typedef struct Array_Rect Array_Rect;
typedef struct Opt_Array_Rect Opt_Array_Rect;
typedef struct OH_DISPLAY_Display OH_DISPLAY_Display;
typedef struct Opt_Display Opt_Display;
typedef struct OH_DISPLAY_FoldCreaseRegion OH_DISPLAY_FoldCreaseRegion;
typedef struct Opt_FoldCreaseRegion Opt_FoldCreaseRegion;
typedef struct DISPLAY_GlobalScope_ohos_displayPeer DISPLAY_GlobalScope_ohos_displayPeer;
typedef struct DISPLAY_GlobalScope_ohos_displayPeer* OH_DISPLAY_GlobalScope_ohos_display;
typedef struct Opt_GlobalScope_ohos_display Opt_GlobalScope_ohos_display;
typedef struct OH_DISPLAY_Rect OH_DISPLAY_Rect;
typedef struct Opt_Rect Opt_Rect;
typedef enum OH_DISPLAY_display_FoldStatus {
    OH_DISPLAY_DISPLAY_FOLD_STATUS_FOLD_STATUS_UNKNOWN = 0,
    OH_DISPLAY_DISPLAY_FOLD_STATUS_FOLD_STATUS_EXPANDED = 1,
    OH_DISPLAY_DISPLAY_FOLD_STATUS_FOLD_STATUS_FOLDED = 2,
    OH_DISPLAY_DISPLAY_FOLD_STATUS_FOLD_STATUS_HALF_FOLDED = 3,
} OH_DISPLAY_display_FoldStatus;
typedef struct Opt_display_FoldStatus {
    OH_Tag tag;
    OH_DISPLAY_display_FoldStatus value;
} Opt_display_FoldStatus;
typedef enum OH_DISPLAY_display_FoldDisplayMode {
    OH_DISPLAY_DISPLAY_FOLD_DISPLAY_MODE_FOLD_DISPLAY_MODE_UNKNOWN = 0,
    OH_DISPLAY_DISPLAY_FOLD_DISPLAY_MODE_FOLD_DISPLAY_MODE_FULL = 1,
    OH_DISPLAY_DISPLAY_FOLD_DISPLAY_MODE_FOLD_DISPLAY_MODE_MAIN = 2,
    OH_DISPLAY_DISPLAY_FOLD_DISPLAY_MODE_FOLD_DISPLAY_MODE_SUB = 3,
    OH_DISPLAY_DISPLAY_FOLD_DISPLAY_MODE_FOLD_DISPLAY_MODE_COORDINATION = 4,
} OH_DISPLAY_display_FoldDisplayMode;
typedef struct Opt_display_FoldDisplayMode {
    OH_Tag tag;
    OH_DISPLAY_display_FoldDisplayMode value;
} Opt_display_FoldDisplayMode;
typedef struct Opt_Int32 {
    OH_Tag tag;
    OH_Int32 value;
} Opt_Int32;
typedef struct Array_Rect {
    OH_DISPLAY_Rect* array;
    OH_Int32 length;
} Array_Rect;
typedef struct Opt_Array_Rect {
    OH_Tag tag;
    Array_Rect value;
} Opt_Array_Rect;
typedef struct Opt_Number {
    OH_Tag tag;
    OH_Number value;
} Opt_Number;
typedef struct OH_DISPLAY_Display {
    void *handle;
} OH_DISPLAY_Display;
typedef struct Opt_Display {
    OH_Tag tag;
    OH_DISPLAY_Display value;
} Opt_Display;
typedef struct OH_DISPLAY_FoldCreaseRegion {
    OH_Number displayId;
    Array_Rect creaseRects;
} OH_DISPLAY_FoldCreaseRegion;
typedef struct Opt_FoldCreaseRegion {
    OH_Tag tag;
    OH_DISPLAY_FoldCreaseRegion value;
} Opt_FoldCreaseRegion;
typedef struct Opt_Boolean {
    OH_Tag tag;
    OH_Boolean value;
} Opt_Boolean;
typedef struct Opt_GlobalScope_ohos_display {
    OH_Tag tag;
    OH_DISPLAY_GlobalScope_ohos_display value;
} Opt_GlobalScope_ohos_display;
typedef struct OH_DISPLAY_Rect {
    OH_Number left;
    OH_Number top;
    OH_Number width;
    OH_Number height;
} OH_DISPLAY_Rect;
typedef struct Opt_Rect {
    OH_Tag tag;
    OH_DISPLAY_Rect value;
} Opt_Rect;
struct OH_DISPLAY_GlobalScope_ohos_displayHandleOpaque;
typedef struct OH_DISPLAY_GlobalScope_ohos_displayHandleOpaque* OH_DISPLAY_GlobalScope_ohos_displayHandle;
typedef struct OH_DISPLAY_Modifier {
    OH_NativePointer (*getFoldDisplayMode)();
    OH_DISPLAY_Display (*getDefaultDisplaySync)();
    OH_NativePointer (*getFoldStatus)();
    OH_DISPLAY_FoldCreaseRegion (*getCurrentFoldCreaseRegion)();
    OH_DISPLAY_Display (*getDisplayByIdSync)(const OH_Number* displayId);
    OH_Boolean (*isFoldable)();
} OH_DISPLAY_Modifier;
typedef struct OH_DISPLAY_API {
    OH_Int32 version;
    const OH_DISPLAY_Modifier* (*DISPLAY)();
} OH_DISPLAY_API;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif // OH_DISPLAY_H
/* clang-format on */