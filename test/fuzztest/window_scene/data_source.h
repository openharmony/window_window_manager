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

#ifndef TEST_FUZZTEST_DATA_SOURCE_H
#define TEST_FUZZTEST_DATA_SOURCE_H

#include <string>
#include <vector>
#include <cstdint>
#include <securec.h>

namespace OHOS::Rosen {
class DataSource  {
public:
    DataSource(const std::uint8_t* data, size_t size) : data_(data), size_(size), pos_(0) {}

    template<class T>
    T GetObject()
    {
        T object{};
        auto objSize = sizeof(object);
        if (!data_ || (objSize > size_ - pos_)) {
            return object;
        }
        auto ret = memcpy_s(&object, objSize, data_ + pos_, objSize);
        if (ret != EOK) {
            return {};
        }
        pos_ += objSize;
        return object;
    }

    const std::uint8_t* GetRaw(size_t rawSize)
    {
        if (!data_ || (rawSize > size_ - pos_)) {
            return nullptr;
        }
        auto current = pos_;
        pos_ += rawSize;
        return data_ + current;
    }

    std::string GetString()
    {
        size_t num = GetObject<uint8_t>();
        std::string str;
        str.resize(num);
        auto rawSize = sizeof(std::string::value_type) * num;
        auto buf =  GetRaw(rawSize);
        if (buf) {
            memcpy_s(str.data(), rawSize, buf, rawSize);
        }
        return str;
    }

    const std::uint8_t* data_;
    size_t size_;
    size_t pos_;
};
}

#endif
