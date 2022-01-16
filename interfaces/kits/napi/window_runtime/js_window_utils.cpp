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
#include "js_window_utils.h"
#include <iomanip>
#include <regex>
#include <sstream>
#include "js_runtime_utils.h"
#include "window_manager_hilog.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "JsWindowUtils"};
}

static NativeValue* GetRectAndConvertToJsValue(NativeEngine& engine, const Rect rect)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert rect to jsObject");
        return nullptr;
    }
    object->SetProperty("left", CreateJsValue(engine, rect.posX_));
    object->SetProperty("top", CreateJsValue(engine, rect.posY_));
    object->SetProperty("width", CreateJsValue(engine, rect.width_));
    object->SetProperty("height", CreateJsValue(engine, rect.height_));
    return objValue;
}

NativeValue* CreateJsWindowPropertiesObject(NativeEngine& engine, sptr<Window>& window)
{
    WLOGFI("JsWindow::CreateJsWindowPropertiesObject is called");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert windowProperties to jsObject");
        return nullptr;
    }

    Rect rect = window->GetRect();
    NativeValue* rectObj = GetRectAndConvertToJsValue(engine, rect);
    if (rectObj == nullptr) {
        WLOGFE("GetRect failed!");
    }
    object->SetProperty("windowRect", rectObj);
    object->SetProperty("type", CreateJsValue(engine, window->GetType()));
    return objValue;
}
static std::string GetHexColor(uint32_t color)
{
    std::stringstream ioss;
    std::string temp;
    ioss << std::setiosflags(std::ios::uppercase) << std::hex << color;
    ioss >> temp;
    int count = RGBA_LENGTH - temp.length();
    std::string tmpColor(count, '0');
    tmpColor += temp;
    std::string finalColor("#");
    finalColor += tmpColor;
    return finalColor;
}

static NativeValue* CreateJsSystemBarRegionTintObject(NativeEngine& engine,
    WindowType type, const SystemBarProperty& prop)
{
    WLOGFI("JsWindow::CreateJsSystemBarRegionTintObject is called");

    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert SystemBarProperty to jsObject");
        return nullptr;
    }
    object->SetProperty("type", CreateJsValue(engine, static_cast<uint32_t>(type)));
    object->SetProperty("isEnable", CreateJsValue(engine, prop.enable_));
    std::string bkgColor = GetHexColor(prop.backgroundColor_);
    WLOGFI("JsWindow::CreateJsSystemBarRegionTintObject backgroundColir: %{public}s", bkgColor.c_str());
    object->SetProperty("backgroundColor", CreateJsValue(engine, bkgColor));
    std::string contentColor = GetHexColor(prop.contentColor_);
    WLOGFI("JsWindow::CreateJsSystemBarRegionTintObject contentColor: %{public}s", contentColor.c_str());
    object->SetProperty("contentColor", CreateJsValue(engine, contentColor));
    Rect rect = {0, 0, 0, 0}; // to fix on next version
    object->SetProperty("region", GetRectAndConvertToJsValue(engine, rect));
    return objValue;
}

NativeValue* CreateJsSystemBarRegionTintArrayObject(NativeEngine& engine,
    const SystemBarProps& props)
{
    WLOGFI("JsWindow::CreateJsSystemBarRegionTintArrayObject is called");
    if (props.empty()) {
        return nullptr;
    }
    NativeValue* objValue = engine.CreateArray(props.size());
    NativeArray* array = ConvertNativeValueTo<NativeArray>(objValue);
    if (array == nullptr) {
        WLOGFE("Failed to convert SystemBarPropertys to jsArrayObject");
        return nullptr;
    }
    uint32_t index = 0;
    for (size_t i = 0; i < props.size(); i++) {
        array->SetElement(index++, CreateJsSystemBarRegionTintObject(engine, props[i].first, props[i].second));
    }
    return objValue;
}

bool GetSystemBarStatus(std::map<WindowType, SystemBarProperty>& systemBarProperties,
                        NativeEngine& engine, NativeCallbackInfo& info, sptr<Window>& window)
{
    NativeArray* nativeArray = ConvertNativeValueTo<NativeArray>(info.argv[0]);
    if (nativeArray == nullptr) {
        WLOGFE("Failed to convert parameter to SystemBarArray");
        return false;
    }
    uint32_t size = nativeArray->GetLength();
    auto statusProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    auto navProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    statusProperty.enable_ = false;
    navProperty.enable_ = false;
    systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR] = statusProperty;
    systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = navProperty;
    for (uint32_t i = 0; i < size; i++) {
        std::string name;
        if (!ConvertFromJsValue(engine, nativeArray->GetElement(i), name)) {
            WLOGFE("Failed to convert parameter to SystemBarName");
            return false;
        }
        if (name.compare("status") == 0) {
            systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = true;
        } else if (name.compare("navigation") == 0) {
            systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enable_ = true;
        }
    }
    return true;
}

static uint32_t GetColorFromJs(NativeEngine& engine, NativeObject* jsObject,
    const char* name, uint32_t defaultColor)
{
    NativeValue* jsColor = jsObject->GetProperty(name);
    if (jsColor->TypeOf() != NATIVE_UNDEFINED) {
        std::string colorStr;
        if (!ConvertFromJsValue(engine, jsColor, colorStr)) {
            WLOGFE("Failed to convert parameter to color");
            return defaultColor;
        }
        std::regex pattern("^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{8})$");
        if (!std::regex_match(colorStr, pattern)) {
            WLOGFE("invalid color input");
            return defaultColor;
        }
        WLOGFI("origin color: %{public}s", colorStr.c_str());
        if (colorStr.length() == RGB_LENGTH) {
            colorStr += "FF"; // RGB + A
        }
        std::string color = colorStr.substr(1);
        std::stringstream ss;
        uint32_t hexColor;
        ss << std::hex << color;
        ss >> hexColor;
        WLOGFI("color: %{public}s, Final Color is %{public}x", color.c_str(), hexColor);
        return hexColor;
    }
    return defaultColor;
}

bool SetSystemBarPropertiesFromJs(NativeEngine& engine, NativeObject* jsObject,
    std::map<WindowType, SystemBarProperty>& properties, sptr<Window>& window)
{
    auto statusProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    auto navProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    properties[WindowType::WINDOW_TYPE_STATUS_BAR] = statusProperty;
    properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = navProperty;
    properties[WindowType::WINDOW_TYPE_STATUS_BAR].backgroundColor_ = GetColorFromJs(engine,
        jsObject, "statusBarColor", statusProperty.backgroundColor_);
    properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].backgroundColor_ = GetColorFromJs(engine,
        jsObject, "navigationBarColor", navProperty.backgroundColor_);
    NativeValue* jsStatusContentColor = jsObject->GetProperty("statusBarContenColor");
    NativeValue* jsStatusIcon = jsObject->GetProperty("isStatusBarLightIcon");
    if (jsStatusContentColor->TypeOf() != NATIVE_UNDEFINED) {
        properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ =  GetColorFromJs(engine,
            jsObject, "statusBarContenColor", statusProperty.contentColor_);
    } else if (jsStatusIcon->TypeOf() != NATIVE_UNDEFINED) {
        bool isStatusBarLightIcon;
        if (!ConvertFromJsValue(engine, jsStatusIcon, isStatusBarLightIcon)) {
            WLOGFE("Failed to convert parameter to isStatusBarLightIcon");
            return false;
        }
        if (isStatusBarLightIcon) {
            properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ = SYSTEM_COLOR_WHITE;
        } else {
            properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ = SYSTEM_COLOR_BLACK;
        }
    }
    NativeValue* jsNavigationContentColor = jsObject->GetProperty("navigationBarContenColor");
    NativeValue* jsNavigationIcon = jsObject->GetProperty("isNavigationBarLightIcon");
    if (jsNavigationContentColor->TypeOf() != NATIVE_UNDEFINED) {
        properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ = GetColorFromJs(engine,
            jsObject, "navigationBarContenColor", navProperty.contentColor_);
    } else if (jsNavigationIcon->TypeOf() != NATIVE_UNDEFINED) {
        bool isNavigationBarLightIcon;
        if (!ConvertFromJsValue(engine, jsNavigationIcon, isNavigationBarLightIcon)) {
            WLOGFE("Failed to convert parameter to isNavigationBarLightIcon");
            return false;
        }
        if (isNavigationBarLightIcon) {
            properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ = SYSTEM_COLOR_WHITE;
        } else {
            properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ = SYSTEM_COLOR_BLACK;
        }
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS