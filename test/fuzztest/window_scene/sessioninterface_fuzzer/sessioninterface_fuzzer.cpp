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

#include "sessioninterface_fuzzer.h"

#include <iremote_broker.h>

#include "ability_context_impl.h"
#include "session_manager.h"
#include "window_manager_hilog.h"
#include "window_scene_session_impl.h"
#include "zidl/session_ipc_interface_code.h"
#include "zidl/session_proxy.h"
#include "data_source.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionInterfaceFuzzTest"};
}

static std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext =
    std::make_shared<AbilityRuntime::AbilityContextImpl>();
static sptr<WindowSessionImpl> savedWindow;

std::pair<sptr<ISession>, sptr<IRemoteObject>> GetProxy()
{
    if (savedWindow) {
        savedWindow->Destroy();
        savedWindow = nullptr;
    }

    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SessionInterfaceFuzzTest");
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);

    sptr<WindowSessionImpl> window = new WindowSceneSessionImpl(option);
    WMError err = window->Create(abilityContext, nullptr);
    if (err != WMError::WM_OK) {
        WLOGFE("Failed to create window: %{public}d", static_cast<int>(err));
        return {nullptr, nullptr};
    }

    auto session = window->GetHostSession();
    if (!session) {
        WLOGFE("Session is nullptr");
        return {nullptr, nullptr};
    }

    sptr<SessionProxy> proxy = new SessionProxy(session->AsObject());
    savedWindow = window;

    WLOGFD("GetProxy success");

    return {proxy, session->AsObject()};
}

template<class T>
T* UnmarshallingDataTo(const uint8_t* data, size_t size)
{
    MessageParcel parcel;
    if (data) {
        parcel.WriteBuffer(data, size);
    }
    return T::Unmarshalling(parcel);
}

void IPCFuzzTest(const uint8_t* data, size_t size)
{
    auto [proxy, remoteObject] = GetProxy();
    if (!proxy || !remoteObject) {
        return;
    }

    DataSource source(data, size);
    uint32_t code = source.GetObject<uint32_t>();
    int flags = source.GetObject<int>();
    int waitTime = source.GetObject<int>();

    MessageParcel sendData;
    MessageParcel reply;
    MessageOption option(flags, waitTime);
    auto rawSize = source.size_ - source.pos_;
    auto buf = source.GetRaw(rawSize);
    if (buf) {
        sendData.WriteBuffer(buf, rawSize);
    }
    remoteObject->SendRequest(code, sendData, reply, option);
}

void IPCSpecificInterfaceFuzzTest1(sptr<IRemoteObject> proxy, MessageParcel& sendData, MessageParcel& reply,
    MessageOption& option)
{
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_CONNECT),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_FOREGROUND),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKGROUND),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_DISCONNECT),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_ACTIVE_STATUS),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SESSION_EVENT),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_RECT),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_TO_APP_TOP),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKPRESSED),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_MARK_PROCESSED),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_MAXIMIZE_MODE),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_MAXIMIZE_MODE),
        sendData, reply, option);
}

void IPCSpecificInterfaceFuzzTest2(sptr<IRemoteObject> proxy, MessageParcel& sendData, MessageParcel& reply,
    MessageOption& option)
{
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NEED_AVOID),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_AVOID_AREA),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_SESSION_PROPERTY),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_ASPECT_RATIO),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_WINDOW_ANIMATION_FLAG),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_CUSTOM_ANIMATION),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_ABOVE_TARGET),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_ACTIVE_PENDING_SESSION),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TERMINATE),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_EXCEPTION),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_ABILITY_RESULT),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_EXTENSION_DATA),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_REMOTE_READY),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_DIED),
        sendData, reply, option);
}

void IPCInterfaceFuzzTest(const uint8_t* data, size_t size)
{
    auto [proxy, remoteObject] = GetProxy();
    if (!proxy || !remoteObject) {
        return;
    }

    DataSource source(data, size);
    int flags = source.GetObject<int>();
    int waitTime = source.GetObject<int>();

    MessageParcel sendData;
    MessageParcel reply;
    MessageOption option(flags, waitTime);
    sendData.WriteInterfaceToken(proxy->GetDescriptor());
    auto rawSize = source.size_ - source.pos_;
    auto buf = source.GetRaw(rawSize);
    if (buf) {
        sendData.WriteBuffer(buf, rawSize);
    }
    IPCSpecificInterfaceFuzzTest1(remoteObject, sendData, reply, option);
    IPCSpecificInterfaceFuzzTest2(remoteObject, sendData, reply, option);
}

void ProxyInterfaceFuzzTestPart1(const uint8_t* data, size_t size)
{
    auto [proxy, remoteObject] = GetProxy();
    if (!proxy || !remoteObject) {
        return;
    }

    DataSource source(data, size);

    sptr<WindowSessionProperty> property = UnmarshallingDataTo<WindowSessionProperty>(data, size);
    proxy->Foreground(property);
    proxy->SetSessionProperty(property);
    proxy->Background();
    proxy->RaiseToAppTop();

    bool active = source.GetObject<bool>();
    proxy->UpdateActiveStatus(active);

    SessionEvent event = source.GetObject<SessionEvent>();
    proxy->OnSessionEvent(event);

    WSRect rect = source.GetObject<WSRect>();
    SizeChangeReason reason = source.GetObject<SizeChangeReason>();
    proxy->UpdateSessionRect(rect, reason);

    bool needMoveToBackground = source.GetObject<bool>();
    proxy->RequestSessionBack(needMoveToBackground);
}

void ProxyInterfaceFuzzTestPart2(const uint8_t* data, size_t size)
{
    auto [proxy, remoteObject] = GetProxy();
    if (!proxy || !remoteObject) {
        return;
    }

    DataSource source(data, size);

    int32_t eventId = source.GetObject<int32_t>();
    proxy->MarkProcessed(eventId);

    MaximizeMode mode = source.GetObject<MaximizeMode>();
    proxy->SetGlobalMaximizeMode(mode);
    proxy->GetGlobalMaximizeMode(mode);

    bool status = source.GetObject<bool>();
    proxy->OnNeedAvoid(status);

    AvoidAreaType type = source.GetObject<AvoidAreaType>();
    proxy->GetAvoidAreaByType(type);

    float ratio = source.GetObject<float>();
    proxy->SetAspectRatio(ratio);

    bool needDefaultAnimationFlag = source.GetObject<bool>();
    proxy->UpdateWindowAnimationFlag(needDefaultAnimationFlag);

    bool isAdd = source.GetObject<bool>();
    proxy->UpdateWindowSceneAfterCustomAnimation(isAdd);
}

void ProxyInterfaceFuzzTestPart3(const uint8_t* data, size_t size)
{
    auto [proxy, remoteObject] = GetProxy();
    if (!proxy || !remoteObject) {
        return;
    }

    DataSource source(data, size);

    int32_t subWindowId = source.GetObject<int32_t>();
    proxy->RaiseAboveTarget(subWindowId);

    sptr<AAFwk::SessionInfo> sessionInfo = UnmarshallingDataTo<AAFwk::SessionInfo>(data, size);
    proxy->PendingSessionActivation(sessionInfo);
    proxy->TerminateSession(sessionInfo);
    proxy->NotifySessionException(sessionInfo);

    uint32_t resultCode = source.GetObject<uint32_t>();
    sptr<AAFwk::Want> want = UnmarshallingDataTo<AAFwk::Want>(data, size);
    proxy->TransferAbilityResult(resultCode, *want);

    sptr<AAFwk::WantParams> wantParams = UnmarshallingDataTo<AAFwk::WantParams>(data, size);
    proxy->TransferExtensionData(*wantParams);
    proxy->NotifyRemoteReady();
    proxy->NotifyExtensionDied();
}

void ProxyInterfaceFuzzTest(const uint8_t* data, size_t size)
{
    ProxyInterfaceFuzzTestPart1(data, size);
    ProxyInterfaceFuzzTestPart2(data, size);
    ProxyInterfaceFuzzTestPart3(data, size);
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::IPCFuzzTest(data, size);
    OHOS::Rosen::IPCInterfaceFuzzTest(data, size);
    OHOS::Rosen::ProxyInterfaceFuzzTest(data, size);
    return 0;
}
