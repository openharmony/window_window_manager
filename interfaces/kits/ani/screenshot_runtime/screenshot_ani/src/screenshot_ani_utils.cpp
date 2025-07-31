/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
 
#include <hitrace_meter.h>
 
#include "ani.h"
#include "screenshot_ani_utils.h"
#include "singleton_container.h"
#include "window_manager_hilog.h"
#include "dm_common.h"
#include "refbase.h"
 
namespace OHOS::Rosen {
ani_status ScreenshotAniUtils::GetStdString(ani_env *env, ani_string ani_str, std::string &result)
{
    ani_size strSize;
    ani_status ret = env->String_GetUTF8Size(ani_str, &strSize);
    if (ret != ANI_OK) {
        return ret;
    }
    std::vector<char> buffer(strSize + 1);
    char* utf8_buffer = buffer.data();
    ani_size bytes_written = 0;
    ret = env->String_GetUTF8(ani_str, utf8_buffer, strSize + 1, &bytes_written);
    if (ret != ANI_OK) {
        return ret;
    }
    utf8_buffer[bytes_written] = '\0';
    result = std::string(utf8_buffer);
    return ret;
}
 
ani_object ScreenshotAniUtils::CreateAniUndefined(ani_env* env)
{
    ani_ref aniRef;
    env->GetUndefined(&aniRef);
    return static_cast<ani_object>(aniRef);
}
 
ani_status ScreenshotAniUtils::GetAniString(ani_env* env, const std::string& str, ani_string* result)
{
    return env->String_NewUTF8(str.c_str(), static_cast<ani_size>(str.size()), result);
}
 
void ScreenshotAniUtils::ConvertScreenshot(ani_env *env, std::shared_ptr<Media::PixelMap> image, ani_object obj)
{
    return;
}
 
ani_status ScreenshotAniUtils::CallAniFunctionVoid(ani_env *env, const char* ns,
    const char* fn, const char* signature, ...)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI] begin");
    ani_status ret = ANI_OK;
    ani_namespace aniNamespace{};
    if ((ret = env->FindNamespace(ns, &aniNamespace)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]canot find ns %{public}d", ret);
        return ret;
    }
    ani_function func{};
    if ((ret = env->Namespace_FindFunction(aniNamespace, fn, signature, &func)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]canot find callBack %{public}d", ret);
        return ret;
    }
    if (func == nullptr) {
        TLOGI(WmsLogTag::DEFAULT, "[ANI] null func ani");
        return ret;
    }
    va_list args;
    va_start(args, signature);
    TLOGI(WmsLogTag::DEFAULT, "[ANI]CallAniFunctionVoid begin %{public}s", signature);
    ret = env->Function_Call_Void_V(func, args);
    va_end(args);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]canot run callBack %{public}d", ret);
        return ret;
    }
    return ret;
}
 
void ScreenshotAniUtils::GetScreenshotParam(ani_env *env, std::unique_ptr<Param> &param, ani_object options)
{
    if (param == nullptr) {
        TLOGI(WmsLogTag::DMS, "[ANI] null param");
        return;
    }
    ani_double displayId = 0;
    env->Object_GetPropertyByName_Double(options, "displayId", &displayId);
    param->option.displayId = static_cast<DisplayId>(displayId);
    ani_double rotation = 0;
    env->Object_GetPropertyByName_Double(options, "rotation", &rotation);
    param->option.rotation = static_cast<int32_t>(rotation);
    ani_boolean isNeedNotify = true;
    env->Object_GetPropertyByName_Boolean(options, "isNotificationNeeded", &isNeedNotify);
    param->option.isNeedNotify = static_cast<bool>(isNeedNotify);
    ani_boolean isCaptureFullOfScreen = false;
    env->Object_GetPropertyByName_Boolean(options, "isCaptureFullOfScreen", &isCaptureFullOfScreen);
    param->option.isCaptureFullOfScreen = static_cast<bool>(isCaptureFullOfScreen);
    GetScreenshotSize(env, param, options);
    GetScreenshotRect(env, param, options);
}
 
void ScreenshotAniUtils::GetScreenshotSize(ani_env *env, std::unique_ptr<Param> &param, ani_object options)
{
    ani_ref obj;
    env->Object_GetPropertyByName_Ref(options, "imageSize", &obj);
    ani_double width;
    ani_double height;
    env->Object_GetPropertyByName_Double(static_cast<ani_object>(obj), "width", &width);
    env->Object_GetPropertyByName_Double(static_cast<ani_object>(obj), "height", &height);
    param->option.size.width = static_cast<int32_t>(width);
    param->option.size.height = static_cast<int32_t>(height);
    TLOGI(WmsLogTag::DMS, "[ANI] get imageSize width = %{public}d, height = %{public}d",
        param->option.size.width, param->option.size.height);
}
 
void ScreenshotAniUtils::GetScreenshotRect(ani_env *env, std::unique_ptr<Param> &param, ani_object options)
{
    ani_ref obj;
    env->Object_GetPropertyByName_Ref(options, "screenRect", &obj);
    ani_double left = 0;
    ani_double top = 0;
    ani_double width = 0;
    ani_double height = 0;
    env->Object_GetPropertyByName_Double(static_cast<ani_object>(obj), "left", &left);
    env->Object_GetPropertyByName_Double(static_cast<ani_object>(obj), "top", &top);
    env->Object_GetPropertyByName_Double(static_cast<ani_object>(obj), "width", &width);
    env->Object_GetPropertyByName_Double(static_cast<ani_object>(obj), "height", &height);
    param->option.rect.left = static_cast<int32_t>(left);
    param->option.rect.top = static_cast<int32_t>(top);
    param->option.rect.width = static_cast<int32_t>(width);
    param->option.rect.height = static_cast<int32_t>(height);
}
}