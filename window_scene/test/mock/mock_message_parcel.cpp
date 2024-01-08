/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#include "mock_message_parcel.h"
#include "iremote_object.h"
#include "message_parcel.h"
#include "window_manager_hilog.h"
namespace OHOS {
namespace {
bool g_setWriteInt32ErrorFlag = false;
bool g_setWriteInt64ErrorFlag = false;
bool g_setWriteParcelableErrorFlag = false;
bool g_setWriteInterfaceTokenErrorFlag = false;
bool g_setReadInt32ErrorFlag = false;
bool g_setReadInt64ErrorFlag = false;
#ifdef ENABLE_MOCK_WRITE_STRING
const static std::string ERROR_FLAG = "error";
#endif
constexpr int32_t ERROR_INT = -1;
#ifdef ENABLE_MOCK_WRITE_STRING_VECTOR
constexpr int32_t ERROR_SIZE = 1;
#endif

} // namespace
namespace Rosen {
void MockMessageParcel::ClearAllErrorFlag()
{
    g_setWriteInt32ErrorFlag = false;
    g_setWriteInt64ErrorFlag = false;
    g_setWriteParcelableErrorFlag = false;
    g_setWriteInterfaceTokenErrorFlag = false;
    g_setReadInt32ErrorFlag = false;
    g_setReadInt64ErrorFlag = false;
}

void MockMessageParcel::SetWriteInt32ErrorFlag(bool flag)
{
    g_setWriteInt32ErrorFlag = flag;
}

void MockMessageParcel::SetWriteInt64ErrorFlag(bool flag)
{
    g_setWriteInt64ErrorFlag = flag;
}

void MockMessageParcel::SetWriteParcelableErrorFlag(bool flag)
{
    g_setWriteParcelableErrorFlag = flag;
}

void MockMessageParcel::SetWriteInterfaceTokenErrorFlag(bool flag)
{
    g_setWriteInterfaceTokenErrorFlag = flag;
}

void MockMessageParcel::SetReadInt32ErrorFlag(bool flag)
{
    g_setReadInt32ErrorFlag = flag;
}

void MockMessageParcel::SetReadInt64ErrorFlag(bool flag)
{
    g_setReadInt64ErrorFlag = flag;
}
}

bool MessageParcel::WriteInterfaceToken(std::u16string name)
{
    if (g_setWriteInterfaceTokenErrorFlag) {
        return false;
    }
    return true;
}

bool Parcel::WriteParcelable(const Parcelable* object)
{
    (void)object;
    if (g_setWriteParcelableErrorFlag) {
        return false;
    }
    return true;
}

#ifdef ENABLE_MOCK_WRITE_STRING
bool Parcel::WriteString(const std::string& value)
{
    (void)value;
    if (value == ERROR_FLAG) {
        return false;
    }
    return true;
}
#endif

bool Parcel::WriteInt32(int32_t value)
{
    (void)value;
    if (g_setWriteInt32ErrorFlag || value == ERROR_INT) {
        return false;
    }
    return true;
}

bool Parcel::WriteInt64(int64_t value)
{
    (void)value;
    if (g_setWriteInt64ErrorFlag || value == ERROR_INT) {
        return false;
    }
    return true;
}

#ifdef ENABLE_MOCK_READ_INT32
bool Parcel::ReadInt32(int32_t &value)
{
    if (g_setReadInt32ErrorFlag) {
        return false;
    }
    return true;
}
#endif

#ifdef ENABLE_MOCK_READ_INT64
bool Parcel::ReadInt64(int64_t &value)
{
    if (g_setReadInt64ErrorFlag) {
        return false;
    }
    return true;
}
#endif

#ifdef ENABLE_MOCK_WRITE_STRING_VECTOR
bool Parcel::WriteStringVector(const std::vector<std::string>& val)
{
    (void)val;
    if (val.size() == ERROR_SIZE) {
        return false;
    }
    return true;
}
#endif
} // namespace OHOS
