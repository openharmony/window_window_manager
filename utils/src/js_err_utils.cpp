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

#include "js_err_utils.h"

#include <map>

namespace OHOS::Rosen {
constexpr const char* WM_ERROR_MSG_OK = "ok";
constexpr const char* WM_ERROR_MSG_DO_NOTHING = "do nothing";
constexpr const char* WM_ERROR_MSG_NO_MEM = "no mem";
constexpr const char* WM_ERROR_MSG_DESTROYED_OBJECT = "destroyed object";
constexpr const char* WM_ERROR_MSG_INVALID_WINDOW = "invalid window";
constexpr const char* WM_ERROR_MSG_INVALID_WINDOW_MODE_OR_SIZE = "invalid window mode or size";
constexpr const char* WM_ERROR_MSG_INVALID_OPERATION = "invalid operation";
constexpr const char* WM_ERROR_MSG_INVALID_PERMISSION = "invalid permission";
constexpr const char* WM_ERROR_MSG_NOT_SYSTEM_APP = "not system app";
constexpr const char* WM_ERROR_MSG_NO_REMOTE_ANIMATION = "no remote animation";
constexpr const char* WM_ERROR_MSG_INVALID_DISPLAY = "invalid display";
constexpr const char* WM_ERROR_MSG_INVALID_PARENT = "invalid parent";
constexpr const char* WM_ERROR_MSG_OPER_FULLSCREEN_FAILED = "oper fullscreen failed";
constexpr const char* WM_ERROR_MSG_REPEAT_OPERATION = "repeat operation";
constexpr const char* WM_ERROR_MSG_INVALID_SESSION = "invalid session";
constexpr const char* WM_ERROR_MSG_INVALID_CALLING = "invalid calling";
constexpr const char* WM_ERROR_MSG_DEVICE_NOT_SUPPORT = "device not support";
constexpr const char* WM_ERROR_MSG_NEED_REPORT_BASE = "need report base";
constexpr const char* WM_ERROR_MSG_NULLPTR = "nullptr";
constexpr const char* WM_ERROR_MSG_INVALID_TYPE = "invalid type";
constexpr const char* WM_ERROR_MSG_INVALID_PARAM = "invalid param";
constexpr const char* WM_ERROR_MSG_SAMGR = "samgr";
constexpr const char* WM_ERROR_MSG_IPC_FAILED = "ipc failed";
constexpr const char* WM_ERROR_MSG_NEED_REPORT_END = "need report end";
constexpr const char* WM_ERROR_MSG_START_ABILITY_FAILED = "start ability failed";
constexpr const char* WM_ERROR_MSG_PIP_DESTROY_FAILED = "pip destroy failed";
constexpr const char* WM_ERROR_MSG_PIP_STATE_ABNORMALLY = "pip state abnormally";
constexpr const char* WM_ERROR_MSG_PIP_CREATE_FAILED = "pip create failed";
constexpr const char* WM_ERROR_MSG_PIP_INTERNAL_ERROR = "pip internal error";
constexpr const char* WM_ERROR_MSG_PIP_REPEAT_OPERATION = "pip repeat operation";

static std::map<WMError, const char*> WM_ERROR_TO_ERROR_MSG_MAP {
    {WMError::WM_OK,                                    WM_ERROR_MSG_OK                          },
    {WMError::WM_DO_NOTHING,                            WM_ERROR_MSG_DO_NOTHING                  },
    {WMError::WM_ERROR_NO_MEM,                          WM_ERROR_MSG_NO_MEM                      },
    {WMError::WM_ERROR_DESTROYED_OBJECT,                WM_ERROR_MSG_DESTROYED_OBJECT            },
    {WMError::WM_ERROR_INVALID_WINDOW,                  WM_ERROR_MSG_INVALID_WINDOW              },
    {WMError::WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE,     WM_ERROR_MSG_INVALID_WINDOW_MODE_OR_SIZE },
    {WMError::WM_ERROR_INVALID_OPERATION,               WM_ERROR_MSG_INVALID_OPERATION           },
    {WMError::WM_ERROR_INVALID_PERMISSION,              WM_ERROR_MSG_INVALID_PERMISSION          },
    {WMError::WM_ERROR_NOT_SYSTEM_APP,                  WM_ERROR_MSG_NOT_SYSTEM_APP              },
    {WMError::WM_ERROR_NO_REMOTE_ANIMATION,             WM_ERROR_MSG_NO_REMOTE_ANIMATION         },
    {WMError::WM_ERROR_INVALID_DISPLAY,                 WM_ERROR_MSG_INVALID_DISPLAY             },
    {WMError::WM_ERROR_INVALID_PARENT,                  WM_ERROR_MSG_INVALID_PARENT              },
    {WMError::WM_ERROR_OPER_FULLSCREEN_FAILED,          WM_ERROR_MSG_OPER_FULLSCREEN_FAILED      },
    {WMError::WM_ERROR_REPEAT_OPERATION,                WM_ERROR_MSG_REPEAT_OPERATION            },
    {WMError::WM_ERROR_INVALID_SESSION,                 WM_ERROR_MSG_INVALID_SESSION             },
    {WMError::WM_ERROR_INVALID_CALLING,                 WM_ERROR_MSG_INVALID_CALLING             },
    {WMError::WM_ERROR_DEVICE_NOT_SUPPORT,              WM_ERROR_MSG_DEVICE_NOT_SUPPORT          },
    {WMError::WM_ERROR_NEED_REPORT_BASE,                WM_ERROR_MSG_NEED_REPORT_BASE            },
    {WMError::WM_ERROR_NULLPTR,                         WM_ERROR_MSG_NULLPTR                     },
    {WMError::WM_ERROR_INVALID_TYPE,                    WM_ERROR_MSG_INVALID_TYPE                },
    {WMError::WM_ERROR_INVALID_PARAM,                   WM_ERROR_MSG_INVALID_PARAM               },
    {WMError::WM_ERROR_SAMGR,                           WM_ERROR_MSG_SAMGR                       },
    {WMError::WM_ERROR_IPC_FAILED,                      WM_ERROR_MSG_IPC_FAILED                  },
    {WMError::WM_ERROR_NEED_REPORT_END,                 WM_ERROR_MSG_NEED_REPORT_END             },
    {WMError::WM_ERROR_START_ABILITY_FAILED,            WM_ERROR_MSG_START_ABILITY_FAILED        },
    {WMError::WM_ERROR_PIP_DESTROY_FAILED,              WM_ERROR_MSG_PIP_DESTROY_FAILED          },
    {WMError::WM_ERROR_PIP_STATE_ABNORMALLY,            WM_ERROR_MSG_PIP_STATE_ABNORMALLY        },
    {WMError::WM_ERROR_PIP_CREATE_FAILED,               WM_ERROR_MSG_PIP_CREATE_FAILED           },
    {WMError::WM_ERROR_PIP_INTERNAL_ERROR,              WM_ERROR_MSG_PIP_INTERNAL_ERROR          },
    {WMError::WM_ERROR_PIP_REPEAT_OPERATION,            WM_ERROR_MSG_PIP_REPEAT_OPERATION        },
};

constexpr const char* WM_ERROR_CODE_MSG_OK = "ok";
constexpr const char* WM_ERROR_CODE_MSG_NO_PERMISSION = "no permission";
constexpr const char* WM_ERROR_CODE_MSG_NOT_SYSTEM_APP = "not system app";
constexpr const char* WM_ERROR_CODE_MSG_INVALID_PARAM = "invalid param";
constexpr const char* WM_ERROR_CODE_MSG_DEVICE_NOT_SUPPORT = "device not support";
constexpr const char* WM_ERROR_CODE_MSG_REPEAT_OPERATION = "repeat operation";
constexpr const char* WM_ERROR_CODE_MSG_STATE_ABNORMALLY = "state abnormally";
constexpr const char* WM_ERROR_CODE_MSG_SYSTEM_ABNORMALLY = "system abnormally";
constexpr const char* WM_ERROR_CODE_MSG_INVALID_CALLING = "invalid calling";
constexpr const char* WM_ERROR_CODE_MSG_STAGE_ABNORMALLY = "stage abnormally";
constexpr const char* WM_ERROR_CODE_MSG_CONTEXT_ABNORMALLY = "context abnormally";
constexpr const char* WM_ERROR_CODE_MSG_START_ABILITY_FAILED = "start ability_failed";
constexpr const char* WM_ERROR_CODE_MSG_INVALID_DISPLAY = "invalid display";
constexpr const char* WM_ERROR_CODE_MSG_INVALID_PARENT = "invalid parent";
constexpr const char* WM_ERROR_CODE_MSG_OPER_FULLSCREEN_FAILED = "oper fullscreen failed";
constexpr const char* WM_ERROR_CODE_MSG_PIP_DESTROY_FAILED = "pip destroy failed";
constexpr const char* WM_ERROR_CODE_MSG_PIP_STATE_ABNORMALLY = "pip state abnormally";
constexpr const char* WM_ERROR_CODE_MSG_PIP_CREATE_FAILED = "pip create failed";
constexpr const char* WM_ERROR_CODE_MSG_PIP_INTERNAL_ERROR = "pip internal error";
constexpr const char* WM_ERROR_CODE_MSG_PIP_REPEAT_OPERATION = "pip repeat operation";

static std::map<WmErrorCode, const char*> WM_ERROR_CODE_TO_ERROR_MSG_MAP {
    {WmErrorCode::WM_OK,                              WM_ERROR_CODE_MSG_OK                     },
    {WmErrorCode::WM_ERROR_NO_PERMISSION,             WM_ERROR_CODE_MSG_NO_PERMISSION          },
    {WmErrorCode::WM_ERROR_NOT_SYSTEM_APP,            WM_ERROR_CODE_MSG_NOT_SYSTEM_APP         },
    {WmErrorCode::WM_ERROR_INVALID_PARAM,             WM_ERROR_CODE_MSG_INVALID_PARAM          },
    {WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT,        WM_ERROR_CODE_MSG_DEVICE_NOT_SUPPORT     },
    {WmErrorCode::WM_ERROR_REPEAT_OPERATION,          WM_ERROR_CODE_MSG_REPEAT_OPERATION       },
    {WmErrorCode::WM_ERROR_STATE_ABNORMALLY,          WM_ERROR_CODE_MSG_STATE_ABNORMALLY       },
    {WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY,         WM_ERROR_CODE_MSG_SYSTEM_ABNORMALLY      },
    {WmErrorCode::WM_ERROR_INVALID_CALLING,           WM_ERROR_CODE_MSG_INVALID_CALLING        },
    {WmErrorCode::WM_ERROR_STAGE_ABNORMALLY,          WM_ERROR_CODE_MSG_STAGE_ABNORMALLY       },
    {WmErrorCode::WM_ERROR_CONTEXT_ABNORMALLY,        WM_ERROR_CODE_MSG_CONTEXT_ABNORMALLY     },
    {WmErrorCode::WM_ERROR_START_ABILITY_FAILED,      WM_ERROR_CODE_MSG_START_ABILITY_FAILED   },
    {WmErrorCode::WM_ERROR_INVALID_DISPLAY,           WM_ERROR_CODE_MSG_INVALID_DISPLAY        },
    {WmErrorCode::WM_ERROR_INVALID_PARENT,            WM_ERROR_CODE_MSG_INVALID_PARENT         },
    {WmErrorCode::WM_ERROR_OPER_FULLSCREEN_FAILED,    WM_ERROR_CODE_MSG_OPER_FULLSCREEN_FAILED },
    {WmErrorCode::WM_ERROR_PIP_DESTROY_FAILED,        WM_ERROR_CODE_MSG_PIP_DESTROY_FAILED     },
    {WmErrorCode::WM_ERROR_PIP_STATE_ABNORMALLY,      WM_ERROR_CODE_MSG_PIP_STATE_ABNORMALLY   },
    {WmErrorCode::WM_ERROR_PIP_CREATE_FAILED,         WM_ERROR_CODE_MSG_PIP_CREATE_FAILED      },
    {WmErrorCode::WM_ERROR_PIP_INTERNAL_ERROR,        WM_ERROR_CODE_MSG_PIP_INTERNAL_ERROR     },
    {WmErrorCode::WM_ERROR_PIP_REPEAT_OPERATION,      WM_ERROR_CODE_MSG_PIP_REPEAT_OPERATION   },
};

constexpr const char* DM_ERROR_MSG_OK = "ok";
constexpr const char* DM_ERROR_MSG_INIT_DMS_PROXY_LOCKED = "init dms proxy locked";
constexpr const char* DM_ERROR_MSG_IPC_FAILED = "ipc failed";
constexpr const char* DM_ERROR_MSG_REMOTE_CREATE_FAILED = "remote create failed";
constexpr const char* DM_ERROR_MSG_NULLPTR = "nullptr";
constexpr const char* DM_ERROR_MSG_INVALID_PARAM = "invalid param";
constexpr const char* DM_ERROR_MSG_WRITE_INTERFACE_TOKEN_FAILED = "write interface token failed";
constexpr const char* DM_ERROR_MSG_DEATH_RECIPIENT = "death recipient";
constexpr const char* DM_ERROR_MSG_INVALID_MODE_ID = "invalid mode id";
constexpr const char* DM_ERROR_MSG_WRITE_DATA_FAILED = "write data failed";
constexpr const char* DM_ERROR_MSG_RENDER_SERVICE_FAILED = "render service failed";
constexpr const char* DM_ERROR_MSG_UNREGISTER_AGENT_FAILED = "unregister agent failed";
constexpr const char* DM_ERROR_MSG_INVALID_CALLING = "invalid calling";
constexpr const char* DM_ERROR_MSG_INVALID_PERMISSION = "invalid permission";
constexpr const char* DM_ERROR_MSG_NOT_SYSTEM_APP = "not system app";
constexpr const char* DM_ERROR_MSG_DEVICE_NOT_SUPPORT = "device not support";
constexpr const char* DM_ERROR_MSG_UNKNOWN = "unknown";

static std::map<DMError, const char*> DM_ERROR_TO_ERROR_MSG_MAP {
    {DMError::DM_OK,                                    DM_ERROR_MSG_OK                           },
    {DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED,           DM_ERROR_MSG_INIT_DMS_PROXY_LOCKED        },
    {DMError::DM_ERROR_IPC_FAILED,                      DM_ERROR_MSG_IPC_FAILED                   },
    {DMError::DM_ERROR_REMOTE_CREATE_FAILED,            DM_ERROR_MSG_REMOTE_CREATE_FAILED         },
    {DMError::DM_ERROR_NULLPTR,                         DM_ERROR_MSG_NULLPTR                      },
    {DMError::DM_ERROR_INVALID_PARAM,                   DM_ERROR_MSG_INVALID_PARAM                },
    {DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED,    DM_ERROR_MSG_WRITE_INTERFACE_TOKEN_FAILED },
    {DMError::DM_ERROR_DEATH_RECIPIENT,                 DM_ERROR_MSG_DEATH_RECIPIENT              },
    {DMError::DM_ERROR_INVALID_MODE_ID,                 DM_ERROR_MSG_INVALID_MODE_ID              },
    {DMError::DM_ERROR_WRITE_DATA_FAILED,               DM_ERROR_MSG_WRITE_DATA_FAILED            },
    {DMError::DM_ERROR_RENDER_SERVICE_FAILED,           DM_ERROR_MSG_RENDER_SERVICE_FAILED        },
    {DMError::DM_ERROR_UNREGISTER_AGENT_FAILED,         DM_ERROR_MSG_UNREGISTER_AGENT_FAILED      },
    {DMError::DM_ERROR_INVALID_CALLING,                 DM_ERROR_MSG_INVALID_CALLING              },
    {DMError::DM_ERROR_INVALID_PERMISSION,              DM_ERROR_MSG_INVALID_PERMISSION           },
    {DMError::DM_ERROR_NOT_SYSTEM_APP,                  DM_ERROR_MSG_NOT_SYSTEM_APP               },
    {DMError::DM_ERROR_DEVICE_NOT_SUPPORT,              DM_ERROR_MSG_DEVICE_NOT_SUPPORT           },
    {DMError::DM_ERROR_UNKNOWN,                         DM_ERROR_MSG_UNKNOWN                      },
};

constexpr const char* DM_ERROR_CODE_MSG_OK = "ok";
constexpr const char* DM_ERROR_CODE_MSG_NO_PERMISSION = "no permission";
constexpr const char* DM_ERROR_CODE_MSG_NOT_SYSTEM_APP = "not system app";
constexpr const char* DM_ERROR_CODE_MSG_INVALID_PARAM = "invalid param";
constexpr const char* DM_ERROR_CODE_MSG_DEVICE_NOT_SUPPORT = "device not support";
constexpr const char* DM_ERROR_CODE_MSG_INVALID_SCREEN = "invalid screen";
constexpr const char* DM_ERROR_CODE_MSG_INVALID_CALLING = "invalid calling";
constexpr const char* DM_ERROR_CODE_MSG_SYSTEM_INNORMAL = "system innormal";

static std::map<DmErrorCode, const char*> DM_ERROR_CODE_TO_ERROR_MSG_MAP {
    {DmErrorCode::DM_OK,                              DM_ERROR_CODE_MSG_OK                 },
    {DmErrorCode::DM_ERROR_NO_PERMISSION,             DM_ERROR_CODE_MSG_NO_PERMISSION      },
    {DmErrorCode::DM_ERROR_NOT_SYSTEM_APP,            DM_ERROR_CODE_MSG_NOT_SYSTEM_APP     },
    {DmErrorCode::DM_ERROR_INVALID_PARAM,             DM_ERROR_CODE_MSG_INVALID_PARAM      },
    {DmErrorCode::DM_ERROR_DEVICE_NOT_SUPPORT,        DM_ERROR_CODE_MSG_DEVICE_NOT_SUPPORT },
    {DmErrorCode::DM_ERROR_INVALID_SCREEN,            DM_ERROR_CODE_MSG_INVALID_SCREEN     },
    {DmErrorCode::DM_ERROR_INVALID_CALLING,           DM_ERROR_CODE_MSG_INVALID_CALLING    },
    {DmErrorCode::DM_ERROR_SYSTEM_INNORMAL,           DM_ERROR_CODE_MSG_SYSTEM_INNORMAL    },
};

template<class T>
napi_value JsErrUtils::CreateJsValue(napi_env env, const T& value)
{
    using ValueType = std::remove_cv_t<std::remove_reference_t<T>>;
    napi_value result = nullptr;
    if constexpr (std::is_same_v<ValueType, bool>) {
        napi_get_boolean(env, value, &result);
        return result;
    } else if constexpr (std::is_arithmetic_v<ValueType>) {
        return CreateJsNumber(env, value);
    } else if constexpr (std::is_same_v<ValueType, std::string>) {
        napi_create_string_utf8(env, value.c_str(), value.length(), &result);
        return result;
    } else if constexpr (std::is_enum_v<ValueType>) {
        return CreateJsNumber(env, static_cast<std::make_signed_t<ValueType>>(value));
    } else if constexpr (std::is_same_v<ValueType, const char*>) {
        (value != nullptr) ? napi_create_string_utf8(env, value, strlen(value), &result) :
            napi_get_undefined(env, &result);
        return result;
    }
}

std::string JsErrUtils::GetErrorMsg(const WMError& errorCode)
{
    return WM_ERROR_TO_ERROR_MSG_MAP.find(errorCode) == WM_ERROR_TO_ERROR_MSG_MAP.end() ?
        WM_ERROR_TO_ERROR_MSG_MAP.at(errorCode) : "";
}

std::string JsErrUtils::GetErrorMsg(const WmErrorCode& errorCode)
{
    return WM_ERROR_CODE_TO_ERROR_MSG_MAP.find(errorCode) == WM_ERROR_CODE_TO_ERROR_MSG_MAP.end() ?
        WM_ERROR_CODE_TO_ERROR_MSG_MAP.at(errorCode) : "";
}

std::string JsErrUtils::GetErrorMsg(const DMError& errorCode)
{
    return DM_ERROR_TO_ERROR_MSG_MAP.find(errorCode) == DM_ERROR_TO_ERROR_MSG_MAP.end() ?
        DM_ERROR_TO_ERROR_MSG_MAP.at(errorCode) : "";
}

std::string JsErrUtils::GetErrorMsg(const DmErrorCode& errorCode)
{
    return DM_ERROR_CODE_TO_ERROR_MSG_MAP.find(errorCode) == DM_ERROR_CODE_TO_ERROR_MSG_MAP.end() ?
        DM_ERROR_CODE_TO_ERROR_MSG_MAP.at(errorCode) : "";
}

napi_value JsErrUtils::CreateJsError(napi_env env, const WMError& errorCode, std::string msg)
{
    napi_value result = nullptr;
    napi_create_error(env, CreateJsValue(env, static_cast<int32_t>(errorCode)),
        CreateJsValue(env, msg == "" ? GetErrorMsg(errorCode) : msg), &result);
    return result;
}

napi_value JsErrUtils::CreateJsError(napi_env env, const WmErrorCode& errorCode, std::string msg)
{
    napi_value result = nullptr;
    napi_create_error(env, CreateJsValue(env, static_cast<int32_t>(errorCode)),
        CreateJsValue(env, msg == "" ? GetErrorMsg(errorCode) : msg), &result);
    return result;
}

napi_value JsErrUtils::CreateJsError(napi_env env, const DMError& errorCode, std::string msg)
{
    napi_value result = nullptr;
    napi_create_error(env, CreateJsValue(env, static_cast<int32_t>(errorCode)),
        CreateJsValue(env, msg == "" ? GetErrorMsg(errorCode) : msg), &result);
    return result;
}

napi_value JsErrUtils::CreateJsError(napi_env env, const DmErrorCode& errorCode, std::string msg)
{
    napi_value result = nullptr;
    napi_create_error(env, CreateJsValue(env, static_cast<int32_t>(errorCode)),
        CreateJsValue(env, msg == "" ? GetErrorMsg(errorCode) : msg), &result);
    return result;
}
} // namespace OHOS::Rosen
