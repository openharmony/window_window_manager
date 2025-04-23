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

#include "rs_adapter.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
RSTransactionAdapter::RSTransactionAdapter(std::shared_ptr<RSNode> rsNode)
{
    if (rsNode) {
        rsUIContext_ = rsNode->GetRSUIContext();
    }
    if (rsUIContext_) {
        rsTransHandler_ = rsUIContext_->GetRSTransaction();
    }
    if (!rsTransHandler_) {
        TLOGW(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "rsTransHandler is null, use rsTransProxy: %{public}s",
              RSAdapterUtil::RSNodeToStr(rsNode).c_str());
        rsTransProxy_ = RSTransactionProxy::GetInstance();
    }

}

RSTransactionAdapter::RSTransactionAdapter(std::shared_ptr<RSUIDirector> rsUIDirector)
{
    if (rsUIDirector) {
        rsUIContext_ = rsUIDirector->GetRSUIContext();
    }
    if (rsUIContext_) {
        rsTransHandler_ = rsUIContext_->GetRSTransaction();
    }
    if (!rsTransHandler_) {
        TLOGW(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "rsTransHandler is null, use rsTransProxy: %{public}s",
              RSAdapterUtil::RSUIDirectorToStr(rsUIDirector).c_str());
        rsTransProxy_ = RSTransactionProxy::GetInstance();
    }
}

RSTransactionAdapter::RSTransactionAdapter(std::shared_ptr<RSUIContext> rsUIContext) : rsUIContext_(rsUIContext)
{
    if (rsUIContext_) {
        rsTransHandler_ = rsUIContext_->GetRSTransaction();
    }
    if (!rsTransHandler_) {
        TLOGW(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "rsTransHandler is null, use rsTransProxy: %{public}s",
              RSAdapterUtil::RSUIContextToStr(rsUIContext).c_str());
        rsTransProxy_ = RSTransactionProxy::GetInstance();
    }
}

std::shared_ptr<RSUIContext> RSTransactionAdapter::GetRSUIContext() const
{
    TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE,
          "%{public}s", RSAdapterUtil::RSUIContextToStr(rsUIContext_).c_str());
    return rsUIContext_;
}

void RSTransactionAdapter::Begin()
{
    if (rsTransHandler_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsTransHandler");
        rsTransHandler_->Begin();
    } else if (rsTransProxy_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsTransProxy");
        rsTransProxy_->Begin();
    } else {
        TLOGE(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Both rsTransHandler and rsTransProxy are null");
    }
}

void RSTransactionAdapter::Commit(uint64_t timestamp)
{
    if (rsTransHandler_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsTransHandler");
        rsTransHandler_->Commit(timestamp);
    } else if (rsTransProxy_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsTransProxy");
        rsTransProxy_->Commit(timestamp);
    } else {
        TLOGE(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Both rsTransHandler and rsTransProxy are null");
    }
}

void RSTransactionAdapter::FlushImplicitTransaction(uint64_t timestamp, const std::string& abilityName)
{
    if (rsTransHandler_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsTransHandler");
        rsTransHandler_->FlushImplicitTransaction(timestamp, abilityName);
    } else if (rsTransProxy_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsTransProxy");
        rsTransProxy_->FlushImplicitTransaction(timestamp, abilityName);
    } else {
        TLOGE(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Both rsTransHandler and rsTransProxy are null");
    }
}

void RSTransactionAdapter::FlushImplicitTransaction(
    const std::shared_ptr<RSNode>& rsNode, uint64_t timestamp, const std::string& abilityName)
{
    auto rsUIContext = rsNode ? rsNode->GetRSUIContext() : nullptr;
    if (!rsUIContext) {
        TLOGW(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "rsUIContext is null, %{public}s", RSAdapterUtil::RSNodeToStr(rsNode).c_str());
    }
    FlushImplicitTransaction(rsUIContext, timestamp, abilityName);
}

void RSTransactionAdapter::FlushImplicitTransaction(
    const std::shared_ptr<RSUIDirector>& rsUIDirector, uint64_t timestamp, const std::string& abilityName)
{
    auto rsUIContext = rsUIDirector ? rsUIDirector->GetRSUIContext() : nullptr;
    if (!rsUIContext) {
        TLOGW(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "rsUIContext is null, %{public}s", RSAdapterUtil::RSUIDirectorToStr(rsUIDirector).c_str());
    }
    FlushImplicitTransaction(rsUIContext, timestamp, abilityName);
}

void RSTransactionAdapter::FlushImplicitTransaction(
    const std::shared_ptr<RSUIContext>& rsUIContext, uint64_t timestamp, const std::string& abilityName)
{
    auto rsTransHandler = rsUIContext ? rsUIContext->GetRSTransaction() : nullptr;
    if (rsTransHandler) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsTransHandler");
        rsTransHandler->FlushImplicitTransaction(timestamp, abilityName);
    } else if (auto rsTransProxy = RSTransactionProxy::GetInstance()) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsTransProxy");
        rsTransProxy->FlushImplicitTransaction(timestamp, abilityName);
    } else {
        TLOGE(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Both rsTransHandler and rsTransProxy are null");
    }
}

AutoRSTransaction::AutoRSTransaction(std::shared_ptr<RSNode> rsNode, bool enable)
{
    if (enable) {
        rsTransAdapter_ = std::make_unique<RSTransactionAdapter>(rsNode);
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "Begin transaction: %{public}s", RSAdapterUtil::RSNodeToStr(rsNode).c_str());
        rsTransAdapter_->Begin();
    }
}

AutoRSTransaction::~AutoRSTransaction()
{
    if (rsTransAdapter_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Commit transaction");
        rsTransAdapter_->Commit();
    }
}

RSSyncTransactionAdapter::RSSyncTransactionAdapter(std::shared_ptr<RSNode> rsNode)
{
    if (rsNode) {
        rsUIContext_ = rsNode->GetRSUIContext();
    }
    if (rsUIContext_) {
        rsSyncTransHandler_ = rsUIContext_->GetSyncTransactionHandler();
    }
    if (!rsSyncTransHandler_) {
        TLOGW(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "rsSyncTransHandler is null, use rsSyncTransController: %{public}s",
              RSAdapterUtil::RSNodeToStr(rsNode).c_str());
        rsSyncTransController_ = RSSyncTransactionController::GetInstance();
    }
}

RSSyncTransactionAdapter::RSSyncTransactionAdapter(std::shared_ptr<RSUIContext> rsUIContext) : rsUIContext_(rsUIContext)
{
    if (rsUIContext_) {
        rsSyncTransHandler_ = rsUIContext_->GetSyncTransactionHandler();
    }
    if (!rsSyncTransHandler_) {
        TLOGW(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "rsSyncTransHandler is null, use rsSyncTransController: %{public}s",
              RSAdapterUtil::RSUIContextToStr(rsUIContext).c_str());
        rsSyncTransController_ = RSSyncTransactionController::GetInstance();
    }
}

std::shared_ptr<RSTransaction> RSSyncTransactionAdapter::GetRSTransaction() const
{
    if (rsSyncTransHandler_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsSyncTransHandler");
        return rsSyncTransHandler_->GetRSTransaction();
    }
    if (rsSyncTransController_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsSyncTransController");
        return rsSyncTransController_->GetRSTransaction();
    }
    TLOGE(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Both rsSyncTransHandler and rsSyncTransController are null");
    return nullptr;
}

void RSSyncTransactionAdapter::OpenSyncTransaction(std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    if (rsSyncTransHandler_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsSyncTransHandler");
        rsSyncTransHandler_->OpenSyncTransaction(handler);
    } else if (rsSyncTransController_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsSyncTransController");
        rsSyncTransController_->OpenSyncTransaction(handler);
    } else {
        TLOGE(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Both rsSyncTransHandler and rsSyncTransController are null");
    }
}

void RSSyncTransactionAdapter::CloseSyncTransaction(std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    if (rsSyncTransHandler_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsSyncTransHandler");
        rsSyncTransHandler_->CloseSyncTransaction(handler);
    } else if (rsSyncTransController_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsSyncTransController");
        rsSyncTransController_->CloseSyncTransaction(handler);
    } else {
        TLOGE(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Both rsSyncTransHandler and rsSyncTransController are null");
    }
}

void RSSyncTransactionAdapter::OpenSyncTransaction(
    const std::shared_ptr<RSNode>& rsNode, std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    auto rsUIContext = rsNode ? rsNode->GetRSUIContext() : nullptr;
    if (!rsUIContext) {
        TLOGW(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "rsUIContext is null, %{public}s", RSAdapterUtil::RSNodeToStr(rsNode).c_str());
    }
    OpenSyncTransaction(rsUIContext, handler);
}

void RSSyncTransactionAdapter::OpenSyncTransaction(
    const std::shared_ptr<RSUIContext>& rsUIContext, std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    auto rsSyncTransHandler = rsUIContext ? rsUIContext->GetSyncTransactionHandler() : nullptr;
    if (rsSyncTransHandler) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsSyncTransHandler");
        rsSyncTransHandler->OpenSyncTransaction(handler);
    } else if (auto rsSyncTransController = RSSyncTransactionController::GetInstance()) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsSyncTransController");
        rsSyncTransController->OpenSyncTransaction(handler);
    } else {
        TLOGE(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Both rsSyncTransHandler and rsSyncTransController are null");
    }
}

void RSSyncTransactionAdapter::CloseSyncTransaction(
    const std::shared_ptr<RSNode>& rsNode, std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    auto rsUIContext = rsNode ? rsNode->GetRSUIContext() : nullptr;
    if (!rsUIContext) {
        TLOGW(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "rsUIContext is null, %{public}s", RSAdapterUtil::RSNodeToStr(rsNode).c_str());
    }
    CloseSyncTransaction(rsUIContext, handler);
}

void RSSyncTransactionAdapter::CloseSyncTransaction(
    const std::shared_ptr<RSUIContext>& rsUIContext, std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    auto rsSyncTransHandler = rsUIContext ? rsUIContext->GetSyncTransactionHandler() : nullptr;
    if (rsSyncTransHandler) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsSyncTransHandler");
        rsSyncTransHandler->CloseSyncTransaction(handler);
    } else if (auto rsSyncTransController = RSSyncTransactionController::GetInstance()) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsSyncTransController");
        rsSyncTransController->CloseSyncTransaction(handler);
    } else {
        TLOGE(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Both rsSyncTransHandler and rsSyncTransController are null");
    }
}

AutoRSSyncTransaction::AutoRSSyncTransaction(
    std::shared_ptr<RSNode> rsNode,
    bool needFlushImplicitTransaction,
    std::shared_ptr<AppExecFwk::EventHandler> handler) : handler_(handler)
{
    if (needFlushImplicitTransaction) {
        RSTransactionAdapter::FlushImplicitTransaction(rsNode);
    }
    rsSyncTransAdapter_ = std::make_unique<RSSyncTransactionAdapter>(rsNode);
    TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE,
          "Open sync transaction: %{public}s", RSAdapterUtil::RSNodeToStr(rsNode).c_str());
    rsSyncTransAdapter_->OpenSyncTransaction(handler);
}

AutoRSSyncTransaction::~AutoRSSyncTransaction()
{
    if (rsSyncTransAdapter_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Close sync transaction");
        rsSyncTransAdapter_->CloseSyncTransaction(handler_);
    }
}

AllowRSMultiInstance::AllowRSMultiInstance(std::shared_ptr<RSNode> rsNode) : rsNode_(rsNode)
{
    if (rsNode_) {
        TLOGW(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "Skip check in multi-instance: %{public}s", RSAdapterUtil::RSNodeToStr(rsNode_).c_str());
        rsNode_->SetSkipCheckInMultiInstance(true);
    }
}

AllowRSMultiInstance::~AllowRSMultiInstance()
{
    if (rsNode_) {
        TLOGW(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "Reopen check in multi-instance: %{public}s", RSAdapterUtil::RSNodeToStr(rsNode_).c_str());
        rsNode_->SetSkipCheckInMultiInstance(false);
    }
}

std::string RSAdapterUtil::RSNodeToStr(const std::shared_ptr<RSNode>& rsNode)
{
    if (!rsNode) {
        return "RSNode is null";
    }
    std::ostringstream oss;
    oss << "RSNode [id: " << rsNode->GetId() << "]";
    auto rsUIContext = rsNode->GetRSUIContext();
    if (rsUIContext) {
        oss << ", RSUIContext [token: " << rsUIContext->GetToken()
            << ", tid: "
            << static_cast<int32_t>(rsUIContext->GetToken() >> 32) // 32: tid's offset position in the token
            << "]";
    } else {
        oss << ", RSUIContext is null";
    }
    return oss.str();
}

std::string RSAdapterUtil::RSUIContextToStr(const std::shared_ptr<RSUIContext>& rsUIContext)
{
    if (!rsUIContext) {
        return "RSUIContext is null";
    }
    std::ostringstream oss;
    oss << "RSUIContext [token: " << rsUIContext->GetToken()
        << ", tid: "
        << static_cast<int32_t>(rsUIContext->GetToken() >> 32) // 32: tid's offset position in the token
        << "]";
    return oss.str();
}

std::string RSAdapterUtil::RSUIDirectorToStr(const std::shared_ptr<RSUIDirector>& rsUIDirector)
{
    if (!rsUIDirector) {
        return "RSUIDirector is null";
    }
    std::ostringstream oss;
    if (auto rsUIContext = rsUIDirector->GetRSUIContext()) {
        oss << "RSUIDirector's RSUIContext [token: " << rsUIContext->GetToken()
            << ", tid: "
            << static_cast<int32_t>(rsUIContext->GetToken() >> 32) // 32: tid's offset position in the token
            << "]";
    } else {
        oss << ", RSUIContext is null";
    }
    return oss.str();
}
} // namespace Rosen
} // namespace OHOS