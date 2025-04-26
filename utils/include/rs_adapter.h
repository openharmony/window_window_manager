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

#ifndef OHOS_ROSEN_RS_ADAPTER_H
#define OHOS_ROSEN_RS_ADAPTER_H

#include <transaction/rs_sync_transaction_controller.h>
#include <transaction/rs_sync_transaction_handler.h>
#include <transaction/rs_transaction.h>
#include <ui/rs_node.h>
#include <ui/rs_ui_context.h>
#include <ui/rs_ui_director.h>

namespace OHOS {
namespace Rosen {
class RSTransactionAdapter {
public:
    explicit RSTransactionAdapter(std::shared_ptr<RSUIContext> rsUIContext);
    explicit RSTransactionAdapter(std::shared_ptr<RSNode> rsNode);
    explicit RSTransactionAdapter(std::shared_ptr<RSUIDirector> rsUIDirector);

    std::shared_ptr<RSUIContext> GetRSUIContext() const;
    void Begin();
    void Commit(uint64_t timestamp = 0);
    void FlushImplicitTransaction(uint64_t timestamp = 0, const std::string& abilityName = "");

    static void FlushImplicitTransaction(
        const std::shared_ptr<RSUIContext>& rsUIContext, uint64_t timestamp = 0, const std::string& abilityName = "");
    static void FlushImplicitTransaction(
        const std::shared_ptr<RSNode>& rsNode, uint64_t timestamp = 0, const std::string& abilityName = "");
    static void FlushImplicitTransaction(
        const std::shared_ptr<RSUIDirector>& rsUIDirector, uint64_t timestamp = 0, const std::string& abilityName = "");

private:
    void InitByRSUIContext(const std::shared_ptr<RSUIContext>& rsUIContext);

    template<typename Func>
    void InvokeTransaction(Func&& func);

    std::shared_ptr<RSUIContext> rsUIContext_;
    RSTransactionProxy* rsTransProxy_;
    std::shared_ptr<RSTransactionHandler> rsTransHandler_;
};

class AutoRSTransaction {
public:
    explicit AutoRSTransaction(std::shared_ptr<RSNode> rsNode, bool enable = true);
    ~AutoRSTransaction();
    AutoRSTransaction(const AutoRSTransaction&) = delete;
    AutoRSTransaction& operator=(const AutoRSTransaction&) = delete;
    AutoRSTransaction(AutoRSTransaction&&) = delete;
    AutoRSTransaction& operator=(AutoRSTransaction&&) = delete;

private:
    std::unique_ptr<RSTransactionAdapter> rsTransAdapter_;
};

class RSSyncTransactionAdapter {
public:
    explicit RSSyncTransactionAdapter(std::shared_ptr<RSUIContext> rsUIContext);
    explicit RSSyncTransactionAdapter(std::shared_ptr<RSNode> rsNode);

    std::shared_ptr<RSTransaction> GetRSTransaction();
    void OpenSyncTransaction(std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr);
    void CloseSyncTransaction(std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr);

    static void OpenSyncTransaction(
        const std::shared_ptr<RSUIContext>& rsUIContext, std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr);
    static void OpenSyncTransaction(
        const std::shared_ptr<RSNode>& rsNode, std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr);
    static void CloseSyncTransaction(
        const std::shared_ptr<RSUIContext>& rsUIContext, std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr);
    static void CloseSyncTransaction(
        const std::shared_ptr<RSNode>& rsNode, std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr);

private:
    void InitByRSUIContext(const std::shared_ptr<RSUIContext>& rsUIContext);

    template<typename ReturnType, typename Func>
    ReturnType InvokeSyncTransaction(Func&& func);

    template<typename Func>
    static void InvokeSyncTransaction(const std::shared_ptr<RSUIContext>& rsUIContext, Func&& func);

    std::shared_ptr<RSUIContext> rsUIContext_;
    RSSyncTransactionController* rsSyncTransController_;
    std::shared_ptr<RSSyncTransactionHandler> rsSyncTransHandler_;
};

class AutoRSSyncTransaction {
public:
    explicit AutoRSSyncTransaction(std::shared_ptr<RSNode> rsNode,
                                   bool needFlushImplicitTransaction = true,
                                   std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr);
    ~AutoRSSyncTransaction();
    AutoRSSyncTransaction(const AutoRSSyncTransaction&) = delete;
    AutoRSSyncTransaction& operator=(const AutoRSSyncTransaction&) = delete;
    AutoRSSyncTransaction(AutoRSSyncTransaction&&) = delete;
    AutoRSSyncTransaction& operator=(AutoRSSyncTransaction&&) = delete;

private:
    std::unique_ptr<RSSyncTransactionAdapter> rsSyncTransAdapter_;
    std::shared_ptr<AppExecFwk::EventHandler> handler_;
};

class AllowRSMultiInstance {
public:
    explicit AllowRSMultiInstance(std::shared_ptr<RSNode> rsNode);
    ~AllowRSMultiInstance();
    AllowRSMultiInstance(const AllowRSMultiInstance&) = delete;
    AllowRSMultiInstance& operator=(const AllowRSMultiInstance&) = delete;
    AllowRSMultiInstance(AllowRSMultiInstance&&) = delete;
    AllowRSMultiInstance& operator=(AllowRSMultiInstance&&) = delete;

private:
    std::shared_ptr<RSNode> rsNode_;
};

class RSAdapterUtil {
public:
    static std::string RSUIContextToStr(const std::shared_ptr<RSUIContext>& rsUIContext);
    static std::string RSNodeToStr(const std::shared_ptr<RSNode>& rsNode);
    static std::string RSUIDirectorToStr(const std::shared_ptr<RSUIDirector>& rsUIDirector);
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_RS_ADAPTER_H