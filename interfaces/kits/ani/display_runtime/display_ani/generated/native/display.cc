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

#include "display.h"

#define KOALA_INTEROP_MODULE DISPLAYNativeModule
#include "common-interop.h"
#include "callback-resource.h"
#include "SerializerBase.h"
#include "DeserializerBase.h"
#include <unordered_map>

#if KOALA_USE_PANDA_VM
KOALA_ETS_INTEROP_MODULE_CLASSPATH(KOALA_INTEROP_MODULE, KOALA_QUOTE(ETS_MODULE_CLASSPATH_PREFIX) KOALA_QUOTE(KOALA_INTEROP_MODULE));
#endif

CustomDeserializer * DeserializerBase::customDeserializers = nullptr;

typedef enum CallbackKind {
    Kind_EMPTY_Callback = -1,
} CallbackKind;

OH_NativePointer getManagedCallbackCaller(CallbackKind kind);
OH_NativePointer getManagedCallbackCallerSync(CallbackKind kind);

struct Counter {
    int count;
    void* data;
};

static int bufferResourceId = 0;
static std::unordered_map<int, Counter> refCounterMap;

int allocate_buffer(int len, void** mem) {
    char* data = new char[len];
    (*mem) = data;
    int id = ++bufferResourceId;
    refCounterMap[id] = Counter { 1, (void*)data };
    return id;
}

void releaseBuffer(int resourceId) {
    if (refCounterMap.find(resourceId) != refCounterMap.end()) {
        Counter& record = refCounterMap[resourceId];
        --record.count;
        if (record.count <= 0) {
            delete[] (char*)record.data;
        }
    }
}

void holdBuffer(int resourceId) {
    if (refCounterMap.find(resourceId) != refCounterMap.end()) {
        Counter& record = refCounterMap[resourceId];
        ++record.count;
    }
}

void impl_AllocateNativeBuffer(KInt len, KByte* ret, KByte* init) {
    void* mem;
    int resourceId = allocate_buffer(len, &mem);
    memcpy((KByte*)mem, init, len);
    SerializerBase ser { ret, 40 }; // todo check
    ser.writeInt32(resourceId);
    ser.writePointer((void*)&holdBuffer);
    ser.writePointer((void*)&releaseBuffer);
    ser.writePointer(mem);
    ser.writeInt64(len);

}
KOALA_INTEROP_V3(AllocateNativeBuffer, KInt, KByte*, KByte*);
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const OH_Int32& value)
{
    return INTEROP_RUNTIME_OBJECT;
}
template <>
inline void WriteToString(std::string* result, const Opt_Int32* value) {
    result->append("{.tag=");
    result->append(tagNameExact((OH_Tag)(value->tag)));
    result->append(", .value=");
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        WriteToString(result, value->value);
    } else {
        OH_Undefined undefined = { 0 };
        WriteToString(result, undefined);
    }
    result->append("}");
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Opt_Int32& value)
{
    return (value.tag != INTEROP_TAG_UNDEFINED) ? (INTEROP_RUNTIME_OBJECT) : (INTEROP_RUNTIME_UNDEFINED);
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const OH_Number& value)
{
    return INTEROP_RUNTIME_NUMBER;
}
template <>
inline void WriteToString(std::string* result, const Opt_Number* value) {
    result->append("{.tag=");
    result->append(tagNameExact((OH_Tag)(value->tag)));
    result->append(", .value=");
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        WriteToString(result, &value->value);
    } else {
        OH_Undefined undefined = { 0 };
        WriteToString(result, undefined);
    }
    result->append("}");
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Opt_Number& value)
{
    return (value.tag != INTEROP_TAG_UNDEFINED) ? (INTEROP_RUNTIME_OBJECT) : (INTEROP_RUNTIME_UNDEFINED);
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const OH_DISPLAY_Rect& value)
{
    return INTEROP_RUNTIME_OBJECT;
}
template <>
inline void WriteToString(std::string* result, const OH_DISPLAY_Rect* value) {
    result->append("{");
    // OH_Number left
    result->append(".left=");
    WriteToString(result, &value->left);
    // OH_Number top
    result->append(", ");
    result->append(".top=");
    WriteToString(result, &value->top);
    // OH_Number width
    result->append(", ");
    result->append(".width=");
    WriteToString(result, &value->width);
    // OH_Number height
    result->append(", ");
    result->append(".height=");
    WriteToString(result, &value->height);
    result->append("}");
}
template <>
inline void WriteToString(std::string* result, const Opt_Rect* value) {
    result->append("{.tag=");
    result->append(tagNameExact((OH_Tag)(value->tag)));
    result->append(", .value=");
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        WriteToString(result, &value->value);
    } else {
        OH_Undefined undefined = { 0 };
        WriteToString(result, undefined);
    }
    result->append("}");
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Opt_Rect& value)
{
    return (value.tag != INTEROP_TAG_UNDEFINED) ? (INTEROP_RUNTIME_OBJECT) : (INTEROP_RUNTIME_UNDEFINED);
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Array_Rect& value)
{
    return INTEROP_RUNTIME_OBJECT;
}

template <>
inline void WriteToString(std::string* result, const OH_DISPLAY_Rect* value);

inline void WriteToString(std::string* result, const Array_Rect* value) {
    int32_t count = value->length;
    result->append("{.array=allocArray<OH_DISPLAY_Rect, " + std::to_string(count) + ">({{");
    for (int i = 0; i < count; i++) {
        if (i > 0) result->append(", ");
        WriteToString(result, (const OH_DISPLAY_Rect*)&value->array[i]);
    }
    result->append("}})");
    result->append(", .length=");
    result->append(std::to_string(value->length));
    result->append("}");
}
template <>
inline void WriteToString(std::string* result, const Opt_Array_Rect* value) {
    result->append("{.tag=");
    result->append(tagNameExact((OH_Tag)(value->tag)));
    result->append(", .value=");
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        WriteToString(result, &value->value);
    } else {
        OH_Undefined undefined = { 0 };
        WriteToString(result, undefined);
    }
    result->append("}");
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Opt_Array_Rect& value)
{
    return (value.tag != INTEROP_TAG_UNDEFINED) ? (INTEROP_RUNTIME_OBJECT) : (INTEROP_RUNTIME_UNDEFINED);
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const OH_DISPLAY_WaterfallDisplayAreaRects& value)
{
    return INTEROP_RUNTIME_OBJECT;
}
template <>
inline void WriteToString(std::string* result, const OH_DISPLAY_WaterfallDisplayAreaRects* value) {
    result->append("{");
    // OH_DISPLAY_Rect left
    result->append(".left=");
    WriteToString(result, &value->left);
    // OH_DISPLAY_Rect right
    result->append(", ");
    result->append(".right=");
    WriteToString(result, &value->right);
    // OH_DISPLAY_Rect top
    result->append(", ");
    result->append(".top=");
    WriteToString(result, &value->top);
    // OH_DISPLAY_Rect bottom
    result->append(", ");
    result->append(".bottom=");
    WriteToString(result, &value->bottom);
    result->append("}");
}
template <>
inline void WriteToString(std::string* result, const Opt_WaterfallDisplayAreaRects* value) {
    result->append("{.tag=");
    result->append(tagNameExact((OH_Tag)(value->tag)));
    result->append(", .value=");
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        WriteToString(result, &value->value);
    } else {
        OH_Undefined undefined = { 0 };
        WriteToString(result, undefined);
    }
    result->append("}");
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Opt_WaterfallDisplayAreaRects& value)
{
    return (value.tag != INTEROP_TAG_UNDEFINED) ? (INTEROP_RUNTIME_OBJECT) : (INTEROP_RUNTIME_UNDEFINED);
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const OH_DISPLAY_Display& value)
{
    return INTEROP_RUNTIME_OBJECT;
}
template <>
inline void WriteToString(std::string* result, const OH_DISPLAY_Display value) {
    WriteToString(result, static_cast<InteropNativePointer>(value));
}
template <>
inline void WriteToString(std::string* result, const Opt_Display* value) {
    result->append("{.tag=");
    result->append(tagNameExact((OH_Tag)(value->tag)));
    result->append(", .value=");
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        WriteToString(result, value->value);
    } else {
        OH_Undefined undefined = { 0 };
        WriteToString(result, undefined);
    }
    result->append("}");
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Opt_Display& value)
{
    return (value.tag != INTEROP_TAG_UNDEFINED) ? (INTEROP_RUNTIME_OBJECT) : (INTEROP_RUNTIME_UNDEFINED);
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const OH_DISPLAY_FoldCreaseRegion& value)
{
    return INTEROP_RUNTIME_OBJECT;
}
template <>
inline void WriteToString(std::string* result, const OH_DISPLAY_FoldCreaseRegion* value) {
    result->append("{");
    // OH_Number displayId
    result->append(".displayId=");
    WriteToString(result, &value->displayId);
    // Array_Rect creaseRects
    result->append(", ");
    result->append(".creaseRects=");
    WriteToString(result, &value->creaseRects);
    result->append("}");
}
template <>
inline void WriteToString(std::string* result, const Opt_FoldCreaseRegion* value) {
    result->append("{.tag=");
    result->append(tagNameExact((OH_Tag)(value->tag)));
    result->append(", .value=");
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        WriteToString(result, &value->value);
    } else {
        OH_Undefined undefined = { 0 };
        WriteToString(result, undefined);
    }
    result->append("}");
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Opt_FoldCreaseRegion& value)
{
    return (value.tag != INTEROP_TAG_UNDEFINED) ? (INTEROP_RUNTIME_OBJECT) : (INTEROP_RUNTIME_UNDEFINED);
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const OH_DISPLAY_display_FoldStatus& value)
{
    return INTEROP_RUNTIME_NUMBER;
}
template <>
inline void WriteToString(std::string* result, const OH_DISPLAY_display_FoldStatus value) {
    result->append("OH_DISPLAY_display_FoldStatus(");
    WriteToString(result, (OH_Int32) value);
    result->append(")");
}
template <>
inline void WriteToString(std::string* result, const Opt_display_FoldStatus* value) {
    result->append("{.tag=");
    result->append(tagNameExact((OH_Tag)(value->tag)));
    result->append(", .value=");
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        WriteToString(result, value->value);
    } else {
        OH_Undefined undefined = { 0 };
        WriteToString(result, undefined);
    }
    result->append("}");
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Opt_display_FoldStatus& value)
{
    return (value.tag != INTEROP_TAG_UNDEFINED) ? (INTEROP_RUNTIME_OBJECT) : (INTEROP_RUNTIME_UNDEFINED);
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const OH_DISPLAY_display_FoldDisplayMode& value)
{
    return INTEROP_RUNTIME_NUMBER;
}
template <>
inline void WriteToString(std::string* result, const OH_DISPLAY_display_FoldDisplayMode value) {
    result->append("OH_DISPLAY_display_FoldDisplayMode(");
    WriteToString(result, (OH_Int32) value);
    result->append(")");
}
template <>
inline void WriteToString(std::string* result, const Opt_display_FoldDisplayMode* value) {
    result->append("{.tag=");
    result->append(tagNameExact((OH_Tag)(value->tag)));
    result->append(", .value=");
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        WriteToString(result, value->value);
    } else {
        OH_Undefined undefined = { 0 };
        WriteToString(result, undefined);
    }
    result->append("}");
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Opt_display_FoldDisplayMode& value)
{
    return (value.tag != INTEROP_TAG_UNDEFINED) ? (INTEROP_RUNTIME_OBJECT) : (INTEROP_RUNTIME_UNDEFINED);
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const OH_DISPLAY_CutoutInfo& value)
{
    return INTEROP_RUNTIME_OBJECT;
}
template <>
inline void WriteToString(std::string* result, const OH_DISPLAY_CutoutInfo* value) {
    result->append("{");
    // Array_Rect boundingRects
    result->append(".boundingRects=");
    WriteToString(result, &value->boundingRects);
    // OH_DISPLAY_WaterfallDisplayAreaRects waterfallDisplayAreaRects
    result->append(", ");
    result->append(".waterfallDisplayAreaRects=");
    WriteToString(result, &value->waterfallDisplayAreaRects);
    result->append("}");
}
template <>
inline void WriteToString(std::string* result, const Opt_CutoutInfo* value) {
    result->append("{.tag=");
    result->append(tagNameExact((OH_Tag)(value->tag)));
    result->append(", .value=");
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        WriteToString(result, &value->value);
    } else {
        OH_Undefined undefined = { 0 };
        WriteToString(result, undefined);
    }
    result->append("}");
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Opt_CutoutInfo& value)
{
    return (value.tag != INTEROP_TAG_UNDEFINED) ? (INTEROP_RUNTIME_OBJECT) : (INTEROP_RUNTIME_UNDEFINED);
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Array_Display& value)
{
    return INTEROP_RUNTIME_OBJECT;
}

template <>
inline void WriteToString(std::string* result, const OH_DISPLAY_Display value);

inline void WriteToString(std::string* result, const Array_Display* value) {
    int32_t count = value->length;
    result->append("{.array=allocArray<OH_DISPLAY_Display, " + std::to_string(count) + ">({{");
    for (int i = 0; i < count; i++) {
        if (i > 0) result->append(", ");
        WriteToString(result, value->array[i]);
    }
    result->append("}})");
    result->append(", .length=");
    result->append(std::to_string(value->length));
    result->append("}");
}
template <>
inline void WriteToString(std::string* result, const Opt_Array_Display* value) {
    result->append("{.tag=");
    result->append(tagNameExact((OH_Tag)(value->tag)));
    result->append(", .value=");
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        WriteToString(result, &value->value);
    } else {
        OH_Undefined undefined = { 0 };
        WriteToString(result, undefined);
    }
    result->append("}");
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Opt_Array_Display& value)
{
    return (value.tag != INTEROP_TAG_UNDEFINED) ? (INTEROP_RUNTIME_OBJECT) : (INTEROP_RUNTIME_UNDEFINED);
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const OH_String& value)
{
    return INTEROP_RUNTIME_STRING;
}
template <>
inline void WriteToString(std::string* result, const Opt_String* value) {
    result->append("{.tag=");
    result->append(tagNameExact((OH_Tag)(value->tag)));
    result->append(", .value=");
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        WriteToString(result, &value->value);
    } else {
        OH_Undefined undefined = { 0 };
        WriteToString(result, undefined);
    }
    result->append("}");
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Opt_String& value)
{
    return (value.tag != INTEROP_TAG_UNDEFINED) ? (INTEROP_RUNTIME_OBJECT) : (INTEROP_RUNTIME_UNDEFINED);
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const OH_Boolean& value)
{
    return INTEROP_RUNTIME_BOOLEAN;
}
template <>
inline void WriteToString(std::string* result, const Opt_Boolean* value) {
    result->append("{.tag=");
    result->append(tagNameExact((OH_Tag)(value->tag)));
    result->append(", .value=");
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        WriteToString(result, value->value);
    } else {
        OH_Undefined undefined = { 0 };
        WriteToString(result, undefined);
    }
    result->append("}");
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Opt_Boolean& value)
{
    return (value.tag != INTEROP_TAG_UNDEFINED) ? (INTEROP_RUNTIME_OBJECT) : (INTEROP_RUNTIME_UNDEFINED);
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const OH_DISPLAY_GlobalScope_ohos_display& value)
{
    return INTEROP_RUNTIME_OBJECT;
}
template <>
inline void WriteToString(std::string* result, const OH_DISPLAY_GlobalScope_ohos_display value) {
    WriteToString(result, static_cast<InteropNativePointer>(value));
}
template <>
inline void WriteToString(std::string* result, const Opt_GlobalScope_ohos_display* value) {
    result->append("{.tag=");
    result->append(tagNameExact((OH_Tag)(value->tag)));
    result->append(", .value=");
    if (value->tag != INTEROP_TAG_UNDEFINED) {
        WriteToString(result, value->value);
    } else {
        OH_Undefined undefined = { 0 };
        WriteToString(result, undefined);
    }
    result->append("}");
}
template <>
inline OH_DISPLAY_RuntimeType runtimeType(const Opt_GlobalScope_ohos_display& value)
{
    return (value.tag != INTEROP_TAG_UNDEFINED) ? (INTEROP_RUNTIME_OBJECT) : (INTEROP_RUNTIME_UNDEFINED);
}
class Serializer : public SerializerBase {
    public:
    Serializer(CallbackResourceHolder* resourceHolder = nullptr) : SerializerBase(resourceHolder) {
    }
    public:
    Serializer(uint8_t* data, OH_UInt32 dataLength = 0, CallbackResourceHolder* resourceHolder = nullptr) : SerializerBase(data, dataLength, resourceHolder) {
    }
    void writeRect(OH_DISPLAY_Rect value)
    {
        Serializer& valueSerializer = *this;
        const auto value_left = value.left;
        valueSerializer.writeNumber(value_left);
        const auto value_top = value.top;
        valueSerializer.writeNumber(value_top);
        const auto value_width = value.width;
        valueSerializer.writeNumber(value_width);
        const auto value_height = value.height;
        valueSerializer.writeNumber(value_height);
    }
    void writeWaterfallDisplayAreaRects(OH_DISPLAY_WaterfallDisplayAreaRects value)
    {
        Serializer& valueSerializer = *this;
        const auto value_left = value.left;
        valueSerializer.writeRect(value_left);
        const auto value_right = value.right;
        valueSerializer.writeRect(value_right);
        const auto value_top = value.top;
        valueSerializer.writeRect(value_top);
        const auto value_bottom = value.bottom;
        valueSerializer.writeRect(value_bottom);
    }
    void writeDisplay(OH_DISPLAY_Display value)
    {
        Serializer& valueSerializer = *this;
        valueSerializer.writePointer(value);
    }
    void writeFoldCreaseRegion(OH_DISPLAY_FoldCreaseRegion value)
    {
        Serializer& valueSerializer = *this;
        const auto value_displayId = value.displayId;
        valueSerializer.writeNumber(value_displayId);
        const auto value_creaseRects = value.creaseRects;
        valueSerializer.writeInt32(value_creaseRects.length);
        for (int i = 0; i < value_creaseRects.length; i++) {
            const OH_DISPLAY_Rect value_creaseRects_element = value_creaseRects.array[i];
            valueSerializer.writeRect(value_creaseRects_element);
        }
    }
    void writeCutoutInfo(OH_DISPLAY_CutoutInfo value)
    {
        Serializer& valueSerializer = *this;
        const auto value_boundingRects = value.boundingRects;
        valueSerializer.writeInt32(value_boundingRects.length);
        for (int i = 0; i < value_boundingRects.length; i++) {
            const OH_DISPLAY_Rect value_boundingRects_element = value_boundingRects.array[i];
            valueSerializer.writeRect(value_boundingRects_element);
        }
        const auto value_waterfallDisplayAreaRects = value.waterfallDisplayAreaRects;
        valueSerializer.writeWaterfallDisplayAreaRects(value_waterfallDisplayAreaRects);
    }
};

class Deserializer : public DeserializerBase {
    public:
    Deserializer(uint8_t* data, OH_Int32 length) : DeserializerBase(data, length) {
    }
    OH_DISPLAY_Rect readRect()
    {
        OH_DISPLAY_Rect value = {};
        Deserializer& valueDeserializer = *this;
        value.left = static_cast<OH_Number>(valueDeserializer.readNumber());
        value.top = static_cast<OH_Number>(valueDeserializer.readNumber());
        value.width = static_cast<OH_Number>(valueDeserializer.readNumber());
        value.height = static_cast<OH_Number>(valueDeserializer.readNumber());
        return value;
    }
    OH_DISPLAY_WaterfallDisplayAreaRects readWaterfallDisplayAreaRects()
    {
        OH_DISPLAY_WaterfallDisplayAreaRects value = {};
        Deserializer& valueDeserializer = *this;
        value.left = valueDeserializer.readRect();
        value.right = valueDeserializer.readRect();
        value.top = valueDeserializer.readRect();
        value.bottom = valueDeserializer.readRect();
        return value;
    }
    OH_DISPLAY_Display readDisplay()
    {
        Deserializer& valueDeserializer = *this;
        OH_NativePointer ptr = valueDeserializer.readPointer();
        return static_cast<OH_DISPLAY_Display>(ptr);
    }
    OH_DISPLAY_FoldCreaseRegion readFoldCreaseRegion()
    {
        OH_DISPLAY_FoldCreaseRegion value = {};
        Deserializer& valueDeserializer = *this;
        value.displayId = static_cast<OH_Number>(valueDeserializer.readNumber());
        const OH_Int32 creaseRects_buf_length = valueDeserializer.readInt32();
        Array_Rect creaseRects_buf = {};
        valueDeserializer.resizeArray<std::decay<decltype(creaseRects_buf)>::type,
        std::decay<decltype(*creaseRects_buf.array)>::type>(&creaseRects_buf, creaseRects_buf_length);
        for (int creaseRects_buf_i = 0; creaseRects_buf_i < creaseRects_buf_length; creaseRects_buf_i++) {
            creaseRects_buf.array[creaseRects_buf_i] = valueDeserializer.readRect();
        }
        value.creaseRects = creaseRects_buf;
        return value;
    }
    OH_DISPLAY_CutoutInfo readCutoutInfo()
    {
        OH_DISPLAY_CutoutInfo value = {};
        Deserializer& valueDeserializer = *this;
        const OH_Int32 boundingRects_buf_length = valueDeserializer.readInt32();
        Array_Rect boundingRects_buf = {};
        valueDeserializer.resizeArray<std::decay<decltype(boundingRects_buf)>::type,
        std::decay<decltype(*boundingRects_buf.array)>::type>(&boundingRects_buf, boundingRects_buf_length);
        for (int boundingRects_buf_i = 0; boundingRects_buf_i < boundingRects_buf_length; boundingRects_buf_i++) {
            boundingRects_buf.array[boundingRects_buf_i] = valueDeserializer.readRect();
        }
        value.boundingRects = boundingRects_buf;
        value.waterfallDisplayAreaRects = valueDeserializer.readWaterfallDisplayAreaRects();
        return value;
    }
};
OH_DISPLAY_DisplayHandle Display_constructImpl();
void Display_destructImpl(OH_DISPLAY_DisplayHandle thiz);
OH_DISPLAY_CutoutInfo Display_getCutoutInfoImpl(OH_NativePointer thisPtr);
OH_NativePointer GlobalScope_ohos_display_getFoldDisplayModeImpl();
OH_NativePointer GlobalScope_ohos_display_getDefaultDisplaySyncImpl();
OH_NativePointer GlobalScope_ohos_display_getFoldStatusImpl();
OH_DISPLAY_FoldCreaseRegion GlobalScope_ohos_display_getCurrentFoldCreaseRegionImpl();
OH_NativePointer GlobalScope_ohos_display_getDisplayByIdSyncImpl(const OH_Number* displayId);
OH_Boolean GlobalScope_ohos_display_isFoldableImpl();
OH_Number GlobalScope_ohos_display_onImpl(const OH_String* type);
OH_NativePointer GlobalScope_ohos_display_getAllDisplaysImpl();
const OH_DISPLAY_DisplayModifier* OH_DISPLAY_DisplayModifierImpl() {
    const static OH_DISPLAY_DisplayModifier instance = {
        &Display_constructImpl,
        &Display_destructImpl,
        &Display_getCutoutInfoImpl,
    };
    return &instance;
}
const OH_DISPLAY_Modifier* OH_DISPLAY_ModifierImpl() {
    const static OH_DISPLAY_Modifier instance = {
        &GlobalScope_ohos_display_getFoldDisplayModeImpl,
        &GlobalScope_ohos_display_getDefaultDisplaySyncImpl,
        &GlobalScope_ohos_display_getFoldStatusImpl,
        &GlobalScope_ohos_display_getCurrentFoldCreaseRegionImpl,
        &GlobalScope_ohos_display_getDisplayByIdSyncImpl,
        &GlobalScope_ohos_display_isFoldableImpl,
        &GlobalScope_ohos_display_onImpl,
        &GlobalScope_ohos_display_getAllDisplaysImpl,
    };
    return &instance;
}
const OH_DISPLAY_API* GetDISPLAYAPIImpl(int version) {
    const static OH_DISPLAY_API api = {
        1, // version
        &OH_DISPLAY_DisplayModifierImpl,
        &OH_DISPLAY_ModifierImpl,
    };
    if (version != api.version) return nullptr;
    return &api;
}

// Accessors

OH_NativePointer impl_Display_ctor() {
        return GetDISPLAYAPIImpl(DISPLAY_API_VERSION)->Display()->construct();
}
KOALA_INTEROP_0(Display_ctor, OH_NativePointer)
OH_NativePointer impl_Display_getFinalizer() {
        return (OH_NativePointer) GetDISPLAYAPIImpl(DISPLAY_API_VERSION)->Display()->destruct;
}
KOALA_INTEROP_0(Display_getFinalizer, OH_NativePointer)
KInteropReturnBuffer impl_Display_getCutoutInfo(OH_NativePointer thisPtr) {
        Serializer _retSerializer {};
        _retSerializer.writeCutoutInfo(GetDISPLAYAPIImpl(DISPLAY_API_VERSION)->Display()->getCutoutInfo(thisPtr));
        return _retSerializer.toReturnBuffer();
}
KOALA_INTEROP_1(Display_getCutoutInfo, KInteropReturnBuffer, OH_NativePointer)
void impl_GlobalScope_ohos_display_getFoldDisplayMode() {
        GetDISPLAYAPIImpl(DISPLAY_API_VERSION)->DISPLAY()->getFoldDisplayMode();
}
KOALA_INTEROP_V0(GlobalScope_ohos_display_getFoldDisplayMode)
OH_NativePointer impl_GlobalScope_ohos_display_getDefaultDisplaySync() {
        return GetDISPLAYAPIImpl(DISPLAY_API_VERSION)->DISPLAY()->getDefaultDisplaySync();
}
KOALA_INTEROP_0(GlobalScope_ohos_display_getDefaultDisplaySync, OH_NativePointer)
void impl_GlobalScope_ohos_display_getFoldStatus() {
        GetDISPLAYAPIImpl(DISPLAY_API_VERSION)->DISPLAY()->getFoldStatus();
}
KOALA_INTEROP_V0(GlobalScope_ohos_display_getFoldStatus)
KInteropReturnBuffer impl_GlobalScope_ohos_display_getCurrentFoldCreaseRegion() {
        Serializer _retSerializer {};
        _retSerializer.writeFoldCreaseRegion(GetDISPLAYAPIImpl(DISPLAY_API_VERSION)->DISPLAY()->getCurrentFoldCreaseRegion());
        return _retSerializer.toReturnBuffer();
}
KOALA_INTEROP_0(GlobalScope_ohos_display_getCurrentFoldCreaseRegion, KInteropReturnBuffer)
OH_NativePointer impl_GlobalScope_ohos_display_getDisplayByIdSync(KInteropNumber displayId) {
        return GetDISPLAYAPIImpl(DISPLAY_API_VERSION)->DISPLAY()->getDisplayByIdSync((const OH_Number*) (&displayId));
}
KOALA_INTEROP_1(GlobalScope_ohos_display_getDisplayByIdSync, OH_NativePointer, KInteropNumber)
OH_Boolean impl_GlobalScope_ohos_display_isFoldable() {
        return GetDISPLAYAPIImpl(DISPLAY_API_VERSION)->DISPLAY()->isFoldable();
}
KOALA_INTEROP_0(GlobalScope_ohos_display_isFoldable, OH_Boolean)
OH_Int32 impl_GlobalScope_ohos_display_on(const KStringPtr& type) {
        return GetDISPLAYAPIImpl(DISPLAY_API_VERSION)->DISPLAY()->on((const OH_String*) (&type)).i32;
}
KOALA_INTEROP_1(GlobalScope_ohos_display_on, OH_Int32, KStringPtr)
void impl_GlobalScope_ohos_display_getAllDisplays() {
        GetDISPLAYAPIImpl(DISPLAY_API_VERSION)->DISPLAY()->getAllDisplays();
}
KOALA_INTEROP_V0(GlobalScope_ohos_display_getAllDisplays)
void deserializeAndCallCallback(OH_Int32 kind, uint8_t* thisArray, OH_Int32 thisLength)
{
    printf("Unknown callback kind\n");
}
void deserializeAndCallCallbackSync(OH_DISPLAY_VMContext vmContext, OH_Int32 kind, uint8_t* thisArray, OH_Int32 thisLength)
{
    printf("Unknown callback kind\n");
}
OH_NativePointer getManagedCallbackCaller(CallbackKind kind)
{
    return nullptr;
}
OH_NativePointer getManagedCallbackCallerSync(CallbackKind kind)
{
    return nullptr;
}
const OH_AnyAPI* impls[16] = { 0 };


const OH_AnyAPI* GetAnyAPIImpl(int kind, int version) {
    switch (kind) {
        case OH_DISPLAY_API_KIND:
            return reinterpret_cast<const OH_AnyAPI*>(GetDISPLAYAPIImpl(version));
        default:
            return nullptr;
    }
}

extern "C" const OH_AnyAPI* GetAnyAPI(int kind, int version) {
    if (kind < 0 || kind > 15) return nullptr;
    if (!impls[kind]) {
        impls[kind] = GetAnyAPIImpl(kind, version);
    }
    return impls[kind];
}
