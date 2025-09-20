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

#include "ani_extension_window_config.h"
#include <string>
#include "ani_window_utils.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

ani_enum_item GetAniModalityType(ani_env* env, ModalityType enumObj)
{
    ani_enum enumType;
    if(ANI_OK != env->FindEnum("L@ohos/window/window/ModalityType;", &enumType)) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] ModalityType not found");
    }
    ani_enum_item enumItem;
    env->Enum_GetEnumItemByIndex(enumType, static_cast<ani_int>(enumObj), &enumItem);

    return enumItem;
}

ani_object CreatAniSubWindowOptions(ani_env* env, const SubWindowOptions& subWindowOptions)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    ani_class aniClass;
    ani_status ret = env->FindClass("L@ohos/window/window/SubWindowOptionsInternal;", &aniClass);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] class not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniCtor);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] ctor not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_object aniOptions;
    ret = env->Object_New(aniClass, aniCtor, &aniOptions);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] fail to new obj");
        return AniWindowUtils::CreateAniUndefined(env);
    }

    ani_string title;
    AniWindowUtils::GetAniString(env, subWindowOptions.title, &title);
    AniWindowUtils::CallAniMethodVoid(env, aniOptions, aniClass, "<set>title", nullptr, title);
    AniWindowUtils::CallAniMethodVoid(env, aniOptions, aniClass, "<set>decorEnabled", nullptr,
        static_cast<ani_boolean>(subWindowOptions.decorEnabled));
    AniWindowUtils::CallAniMethodVoid(env, aniOptions, aniClass, "<set>isModal", nullptr,
        AniWindowUtils::CreateOptionalBool(env, static_cast<ani_boolean>(subWindowOptions.isModal)));
    AniWindowUtils::CallAniMethodVoid(env, aniOptions, aniClass, "<set>isTopmost", nullptr,
        AniWindowUtils::CreateOptionalBool(env, static_cast<ani_boolean>(subWindowOptions.isTopmost)));
    AniWindowUtils::CallAniMethodVoid(env, aniOptions, aniClass, "<set>zLevel", nullptr,
        AniWindowUtils::CreateOptionalInt(env, static_cast<ani_int>(subWindowOptions.zLevel)));
    AniWindowUtils::CallAniMethodVoid(env, aniOptions, aniClass, "<set>maximizeSupported", nullptr,
        AniWindowUtils::CreateOptionalBool(env, static_cast<ani_boolean>(subWindowOptions.maximizeSupported)));
    AniWindowUtils::CallAniMethodVoid(env, aniOptions, aniClass, "<set>modalityType", nullptr,
        GetAniModalityType(env, subWindowOptions.modalityType));

    return aniOptions;
}

ani_object CreatAniSystemWindowOptions(ani_env* env, const SystemWindowOptions& systemWindowOptions)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    ani_class aniClass;
    ani_status ret = env->FindClass("L@ohos/window/window/SystemWindowOptionsInternal;", &aniClass);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] class not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniCtor);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] ctor not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_object aniOptions;
    ret = env->Object_New(aniClass, aniCtor, &aniOptions);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] fail to new obj");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    
    ApiWindowType type = ApiWindowType::TYPE_BASE;
    if (systemWindowOptions.windowType >= static_cast<int32_t>(ApiWindowType::TYPE_BASE) &&
        systemWindowOptions.windowType < static_cast<int32_t>(ApiWindowType::TYPE_END)) {
        type = static_cast<ApiWindowType>(systemWindowOptions.windowType);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] invalid windowType: %{public}d", systemWindowOptions.windowType);
    }
    std::string typeName = "TYPE_APP";
    if (API_TO_ANI_STRING_TYPE_MAP.count(type) != 0) {
        typeName = API_TO_ANI_STRING_TYPE_MAP.at(type);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] invalid ApiWindowType: %{public}d", static_cast<int32_t>(type));
    }

    ani_enum enumType;
    if(ANI_OK != env->FindEnum("L@ohos/window/window/WindowType;", &enumType)) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] WindowType not found");
    }
    ani_enum_item enumItem;
    env->Enum_GetEnumItemByName(enumType, typeName.c_str(), &enumItem);
    AniWindowUtils::CallAniMethodVoid(env, aniOptions, aniClass, "<set>windowType", nullptr, enumItem);
    
    return aniOptions;
}

ani_enum_item GetAniExtensionWindowAttribute(ani_env* env, ExtensionWindowAttribute enumObj)
{
    ani_enum enumType;
    if(ANI_OK != env->FindEnum("L@ohos/window/window/ExtensionWindowAttribute;", &enumType)) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] ExtensionWindowAttribute not found");
    }
    ani_enum_item enumItem;
    env->Enum_GetEnumItemByIndex(enumType, static_cast<ani_int>(enumObj), &enumItem);

    return enumItem;
}

ani_object CreateAniExtensionWindowConfig(ani_env* env,
    const std::shared_ptr<ExtensionWindowConfig>& extensionWindowConfig)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    ani_class aniClass;
    ani_status ret = env->FindClass("L@ohos/window/window/ExtensionWindowConfigInternal;", &aniClass);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] class not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniCtor);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] ctor not found");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    ani_object aniConfig;
    ret = env->Object_New(aniClass, aniCtor, &aniConfig);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] fail to new obj");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    if (extensionWindowConfig != nullptr) {
        ani_string windowName;
        AniWindowUtils::GetAniString(env, extensionWindowConfig->windowName, &windowName);
        AniWindowUtils::CallAniMethodVoid(env, aniConfig, aniClass, "<set>windowName", nullptr, windowName);
        AniWindowUtils::CallAniMethodVoid(env, aniConfig, aniClass, "<set>windowAttribute", nullptr,
            GetAniExtensionWindowAttribute(env, extensionWindowConfig->windowAttribute));
        AniWindowUtils::CallAniMethodVoid(env, aniConfig, aniClass, "<set>windowRect", nullptr,
            AniWindowUtils::CreateAniRect(env, extensionWindowConfig->windowRect));
        AniWindowUtils::CallAniMethodVoid(env, aniConfig, aniClass, "<set>subWindowOptions", nullptr,
            CreatAniSubWindowOptions(env, extensionWindowConfig->subWindowOptions));
        AniWindowUtils::CallAniMethodVoid(env, aniConfig, aniClass, "<set>systemWindowOptions", nullptr,
            CreatAniSystemWindowOptions(env, extensionWindowConfig->systemWindowOptions));
    }

    return aniConfig;
}
} // namespace Rosen
} // namespace OHOS