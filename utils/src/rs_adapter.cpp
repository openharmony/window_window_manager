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
void RSTransactionAdapter::InitByRSUIContext(const std::shared_ptr<RSUIContext>& rsUIContext)
{
    rsUIContext_ = rsUIContext;
    if (rsUIContext_) {
        rsTransHandler_ = rsUIContext_->GetRSTransaction();
    }
    if (!rsTransHandler_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "rsTransHandler is null, fallback to rsTransProxy: %{public}s",
              RSAdapterUtil::RSUIContextToStr(rsUIContext_).c_str());
        rsTransProxy_ = RSTransactionProxy::GetInstance();
    }
}

RSTransactionAdapter::RSTransactionAdapter(std::shared_ptr<RSUIContext> rsUIContext)
{
    InitByRSUIContext(rsUIContext);
}

RSTransactionAdapter::RSTransactionAdapter(std::shared_ptr<RSNode> rsNode)
{
    InitByRSUIContext(rsNode ? rsNode->GetRSUIContext() : nullptr);
}

RSTransactionAdapter::RSTransactionAdapter(std::shared_ptr<RSUIDirector> rsUIDirector)
{
    InitByRSUIContext(rsUIDirector ? rsUIDirector->GetRSUIContext() : nullptr);
}

std::shared_ptr<RSUIContext> RSTransactionAdapter::GetRSUIContext() const
{
    TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "%{public}s", RSAdapterUtil::RSUIContextToStr(rsUIContext_).c_str());
    return rsUIContext_;
}

template<typename Func>
void RSTransactionAdapter::InvokeTransaction(Func&& func)
{
    if (rsTransHandler_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsTransHandler");
        func(rsTransHandler_);
    } else if (rsTransProxy_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Fallback to rsTransProxy");
        func(rsTransProxy_);
    } else {
        TLOGE(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Both rsTransHandler and rsTransProxy are null");
    }
}

void RSTransactionAdapter::Begin()
{
    InvokeTransaction([](auto& invoker) { invoker->Begin(); });
}

void RSTransactionAdapter::Commit(uint64_t timestamp)
{
    InvokeTransaction([timestamp](auto& invoker) { invoker->Commit(timestamp); });
}

void RSTransactionAdapter::FlushImplicitTransaction(uint64_t timestamp, const std::string& abilityName)
{
    InvokeTransaction([timestamp, &abilityName](auto& invoker) {
        invoker->FlushImplicitTransaction(timestamp, abilityName);
    });
}

void RSTransactionAdapter::FlushImplicitTransaction(
    const std::shared_ptr<RSUIContext>& rsUIContext, uint64_t timestamp, const std::string& abilityName)
{
    auto rsTransHandler = rsUIContext ? rsUIContext->GetRSTransaction() : nullptr;
    if (rsTransHandler) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsTransHandler");
        rsTransHandler->FlushImplicitTransaction(timestamp, abilityName);
    } else if (auto rsTransProxy = RSTransactionProxy::GetInstance()) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Fallback to rsTransProxy");
        rsTransProxy->FlushImplicitTransaction(timestamp, abilityName);
    } else {
        TLOGE(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Both rsTransHandler and rsTransProxy are null");
    }
}

void RSTransactionAdapter::FlushImplicitTransaction(
    const std::shared_ptr<RSNode>& rsNode, uint64_t timestamp, const std::string& abilityName)
{
    TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "%{public}s", RSAdapterUtil::RSNodeToStr(rsNode).c_str());
    FlushImplicitTransaction(rsNode ? rsNode->GetRSUIContext() : nullptr, timestamp, abilityName);
}

void RSTransactionAdapter::FlushImplicitTransaction(
    const std::shared_ptr<RSUIDirector>& rsUIDirector, uint64_t timestamp, const std::string& abilityName)
{
    TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "%{public}s", RSAdapterUtil::RSUIDirectorToStr(rsUIDirector).c_str());
    FlushImplicitTransaction(rsUIDirector ? rsUIDirector->GetRSUIContext() : nullptr, timestamp, abilityName);
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

void RSSyncTransactionAdapter::InitByRSUIContext(const std::shared_ptr<RSUIContext>& rsUIContext)
{
    rsUIContext_ = rsUIContext;
    if (rsUIContext_) {
        rsSyncTransHandler_ = rsUIContext_->GetSyncTransactionHandler();
    }
    if (!rsSyncTransHandler_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "rsSyncTransHandler is null, fallback to rsSyncTransController: %{public}s",
              RSAdapterUtil::RSUIContextToStr(rsUIContext).c_str());
        rsSyncTransController_ = RSSyncTransactionController::GetInstance();
    }
}

RSSyncTransactionAdapter::RSSyncTransactionAdapter(std::shared_ptr<RSUIContext> rsUIContext)
{
    InitByRSUIContext(rsUIContext);
}

RSSyncTransactionAdapter::RSSyncTransactionAdapter(std::shared_ptr<RSNode> rsNode)
{
    InitByRSUIContext(rsNode ? rsNode->GetRSUIContext() : nullptr);
}

template<typename ReturnType, typename Func>
ReturnType RSSyncTransactionAdapter::InvokeSyncTransaction(Func&& func)
{
    if (rsSyncTransHandler_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsSyncTransHandler");
        return func(rsSyncTransHandler_);
    } else if (rsSyncTransController_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Fallback to rsSyncTransController");
        return func(rsSyncTransController_);
    } else {
        TLOGE(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Both rsSyncTransHandler and rsSyncTransController are null");
        if constexpr (!std::is_void_v<ReturnType>) {
            return nullptr;
        }
    }
}

std::shared_ptr<RSTransaction> RSSyncTransactionAdapter::GetRSTransaction()
{
    return InvokeSyncTransaction<std::shared_ptr<RSTransaction>>([](auto& invoker) {
        return invoker->GetRSTransaction();
    });
}

void RSSyncTransactionAdapter::OpenSyncTransaction(std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    InvokeSyncTransaction<void>([handler](auto& invoker) {
        invoker->OpenSyncTransaction(handler);
    });
}

void RSSyncTransactionAdapter::CloseSyncTransaction(std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    InvokeSyncTransaction<void>([handler](auto& invoker) {
        invoker->CloseSyncTransaction(handler);
    });
}

template<typename Func>
void RSSyncTransactionAdapter::InvokeSyncTransaction(const std::shared_ptr<RSUIContext>& rsUIContext, Func&& func)
{
    auto rsSyncTransHandler = rsUIContext ? rsUIContext->GetSyncTransactionHandler() : nullptr;
    if (rsSyncTransHandler) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Use rsSyncTransHandler");
        func(rsSyncTransHandler);
    } else if (auto rsSyncTransController = RSSyncTransactionController::GetInstance()) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Fallback to rsSyncTransController");
        func(rsSyncTransController);
    } else {
        TLOGE(WmsLogTag::WMS_RS_MULTI_INSTANCE, "Both rsSyncTransHandler and rsSyncTransController are null");
    }
}

void RSSyncTransactionAdapter::OpenSyncTransaction(
    const std::shared_ptr<RSUIContext>& rsUIContext, std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    InvokeSyncTransaction(rsUIContext, [handler](auto& invoker) {
        invoker->OpenSyncTransaction(handler);
    });
}

void RSSyncTransactionAdapter::OpenSyncTransaction(
    const std::shared_ptr<RSNode>& rsNode, std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "%{public}s", RSAdapterUtil::RSNodeToStr(rsNode).c_str());
    OpenSyncTransaction(rsNode->GetRSUIContext(), handler);
}

void RSSyncTransactionAdapter::CloseSyncTransaction(
    const std::shared_ptr<RSUIContext>& rsUIContext, std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    InvokeSyncTransaction(rsUIContext, [handler](auto& invoker) {
        invoker->CloseSyncTransaction(handler);
    });
}

void RSSyncTransactionAdapter::CloseSyncTransaction(
    const std::shared_ptr<RSNode>& rsNode, std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE, "%{public}s", RSAdapterUtil::RSNodeToStr(rsNode).c_str());
    CloseSyncTransaction(rsNode->GetRSUIContext(), handler);
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
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "Skip check in multi-instance: %{public}s", RSAdapterUtil::RSNodeToStr(rsNode_).c_str());
        rsNode_->SetSkipCheckInMultiInstance(true);
    }
}

AllowRSMultiInstance::~AllowRSMultiInstance()
{
    if (rsNode_) {
        TLOGD(WmsLogTag::WMS_RS_MULTI_INSTANCE,
              "Reopen check in multi-instance: %{public}s", RSAdapterUtil::RSNodeToStr(rsNode_).c_str());
        rsNode_->SetSkipCheckInMultiInstance(false);
    }
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

std::string RSAdapterUtil::RSNodeToStr(const std::shared_ptr<RSNode>& rsNode)
{
    if (!rsNode) {
        return "RSNode is null";
    }
    std::ostringstream oss;
    oss << "RSNode [id: " << rsNode->GetId() << "], "
        << RSUIContextToStr(rsNode->GetRSUIContext());
    return oss.str();
}

std::string RSAdapterUtil::RSUIDirectorToStr(const std::shared_ptr<RSUIDirector>& rsUIDirector)
{
    if (!rsUIDirector) {
        return "RSUIDirector is null";
    }
    std::ostringstream oss;
    oss << "RSUIDirector's " << RSUIContextToStr(rsUIDirector->GetRSUIContext());
    return oss.str();
}
} // namespace Rosen
} // namespace OHOS