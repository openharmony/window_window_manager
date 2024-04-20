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

#include "anomaly_detection.h"

#include "dfx_hisysevent.h"
#include "interfaces/include/ws_common.h"
#include "session_manager/include/scene_session_manager.h"
#include "window_helper.h"
#include "screen_session_manager/include/screen_session_manager_client.h"
#include "singleton_container.h"
#include "perform_reporter.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "AnomalyDetection" };
}

void AnomalyDetection::SceneZOrderCheckProcess()
{
    bool keyGuardFlag = false;
    uint32_t curZOrder = 0;
    auto func = [&curZOrder, &keyGuardFlag](sptr<SceneSession> session) {
        if ((session == nullptr) || (!SceneSessionManager::GetInstance().IsSessionVisible(session))) {
            return false;
        }
        // check zorder = 0
        if (session->GetZOrder() == 0) {
            TLOGE(WmsLogTag::WMS_FOCUS, "ZOrderCheck err, zorder 0");
            ReportZOrderException("check zorder 0", session);
        }
        // repetitive zorder
        if (session->GetZOrder() == curZOrder) {
            TLOGE(WmsLogTag::WMS_FOCUS, "ZOrderCheck err, repetitive zorder %{public}d", session->GetZOrder());
            ReportZOrderException("check repetitive zorder", session);
        }
        curZOrder = session->GetZOrder();
        // callingSession check for input method
        if (session->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
            uint32_t callingWindowId = session->GetSessionProperty()->GetCallingSessionId();
            const auto& callingSession =
                SceneSessionManager::GetInstance().GetSceneSession(static_cast<int32_t>(callingWindowId));
            if ((callingSession != nullptr) && (callingSession->GetZOrder() > session->GetZOrder())) {
                TLOGE(WmsLogTag::WMS_FOCUS, "ZOrderCheck err, callingSession: %{public}d curSession: %{public}d",
                    callingSession->GetZOrder(), session->GetZOrder());
                ReportZOrderException("check callingSession check for input", session);
            }
        }
        // subWindow/dialogWindow
        if (WindowHelper::IsSubWindow(session->GetWindowType()) ||
            session->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
            auto mainSession = session->GetParentSession();
            if ((mainSession != nullptr) && (session->GetZOrder() < mainSession->GetZOrder())) {
                TLOGE(WmsLogTag::WMS_FOCUS, "ZOrderCheck err, subSession %{public}d mainSession %{public}d",
                    session->GetZOrder(), mainSession->GetZOrder());
                ReportZOrderException("check subWindow and dialogWindow", session);
            }
        }
        if (session->GetWindowType() == WindowType::WINDOW_TYPE_KEYGUARD) {
            keyGuardFlag = true;
            return false;
        }
        if (keyGuardFlag && (!session->IsShowWhenLocked()) && (session->IsAppSession())) {
            TLOGE(WmsLogTag::WMS_FOCUS, "ZOrderCheck err %{public}d IsShowWhenLocked", session->GetZOrder());
            ReportZOrderException("check isShowWhenLocked", session);
        }
        return false;
    };
    SceneSessionManager::GetInstance().TraverseSessionTree(func, false);
}

void AnomalyDetection::FocusCheckProcess(int32_t focusedId, int32_t nextId)
{
    if (nextId == INVALID_SESSION_ID) {
        TLOGE(WmsLogTag::WMS_FOCUS, "FocusCheck err: invalid id, focusedId:%{public}d nextId:%{public}d",
            focusedId, nextId);
        ReportFocusException("invalid id", focusedId, nextId, nullptr);
    }
    bool focusSessionFlag = false;
    auto func = [&focusSessionFlag, focusedId, nextId](sptr<SceneSession> session) {
        if (session == nullptr) {
            return false;
        }
        if (session->IsFocused()) {
            focusSessionFlag = true;
            return false;
        }
        if (focusSessionFlag && session->GetBlockingFocus() &&
            SceneSessionManager::GetInstance().IsSessionVisible(session)) {
            TLOGE(WmsLogTag::WMS_FOCUS, "FocusCheck err: blockingFocus, sessionId:%{public}d",
                session->GetPersistentId());
            ReportFocusException("check blockingFocus", focusedId, nextId, session);
        }
        return false;
    };
    SceneSessionManager::GetInstance().TraverseSessionTree(func, false);
}

void AnomalyDetection::ReportZOrderException(const std::string& errorReason, sptr<SceneSession> session)
{
    if (session == nullptr) {
        return;
    }
    std::ostringstream oss;
    oss << " ZOrderCheck err " << errorReason;
    oss << " cur persistentId: " << session->GetPersistentId() << ",";
    oss << " windowType: " << static_cast<uint32_t>(session->GetWindowType()) << ",";
    oss << " cur ZOrder: " << session->GetZOrder() << ";";
    WindowInfoReporter::GetInstance().ReportWindowException(
        static_cast<int32_t>(WindowDFXHelperType::WINDOW_ZORDER_CHECK), getpid(), oss.str());
}

void AnomalyDetection::ReportFocusException(const std::string& errorReason, int32_t focusedId, int32_t nextId,
    sptr<SceneSession> session)
{
    std::ostringstream oss;
    oss << " FocusCheck err " << errorReason;
    if (session != nullptr) {
        oss << " cur persistentId: " << session->GetPersistentId() << ",";
        oss << " windowType: " << static_cast<uint32_t>(session->GetWindowType()) << ",";
    }
    oss << " focusedId: " << focusedId << ",";
    oss << " nextId: " << nextId << ";";
    WindowInfoReporter::GetInstance().ReportWindowException(
        static_cast<int32_t>(WindowDFXHelperType::WINDOW_FOCUS_CHECK), getpid(), oss.str());
}
} // namespace Rosen
} // namespace OHOS