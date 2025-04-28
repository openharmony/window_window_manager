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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSAdapterTest : public Test {
public:
    static void SetUpTestCase()
    {
        rsUIDirector_ = RSUIDirector::Create();
        rsUIDirector_->Init(true, true);
        rsUIContext_ = rsUIDirector_->GetRSUIContext();
        struct RSSurfaceNodeConfig config;
        rsNode_ = RSSurfaceNode::Create(config, true, rsUIContext_);
    }

    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}

private:
    static std::shared_ptr<RSNode> rsNode_;
    static std::shared_ptr<RSUIContext> rsUIContext_;
    static std::shared_ptr<RSUIDirector> rsUIDirector_;
};

std::shared_ptr<RSNode> RSAdapterTest::rsNode_ = nullptr;
std::shared_ptr<RSUIContext> RSAdapterTest::rsUIContext_ = nullptr;
std::shared_ptr<RSUIDirector> RSAdapterTest::rsUIDirector_ = nullptr;

class MockRSTransactionAdapter : public RSTransactionAdapter {
public:
    explicit MockRSTransactionAdapter(const std::shared_ptr<RSUIContext>& rsUIContext)
        : RSTransactionAdapter(rsUIContext) {}
    MOCK_METHOD(void, Begin, (), (override));
    MOCK_METHOD(void, Commit, (uint64_t timestamp), (override));
};

class MockRSSyncTransactionAdapter : public RSSyncTransactionAdapter {
public:
    explicit MockRSSyncTransactionAdapter(const std::shared_ptr<RSUIContext>& rsUIContext)
        : RSSyncTransactionAdapter(rsUIContext) {}
    MOCK_METHOD(void, OpenSyncTransaction, (const std::shared_ptr<AppExecFwk::EventHandler>& handler), (override));
    MOCK_METHOD(void, CloseSyncTransaction, (const std::shared_ptr<AppExecFwk::EventHandler>& handler), (override));
};
    

/**
 * @tc.name: RSTransactionAdapterConstructor
 * @tc.desc: Verify RSTransactionAdapter constructed with RSUIContext.
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, RSTransactionAdapterConstructor, Function | SmallTest | Level1)
{
    RSTransactionAdapter adapter(rsUIContext_);
    if (rsUIContext_) {
        EXPECT_NE(adapter.rsTransHandler_, nullptr);
        EXPECT_EQ(adapter.rsTransProxy_, nullptr);
    } else {
        EXPECT_EQ(adapter.rsTransHandler_, nullptr);
        EXPECT_NE(adapter.rsTransProxy_, nullptr);
    }
}

/**
 * @tc.name: RSTransactionAdapterBegin
 * @tc.desc: Verify RSTransactionAdapter::Begin works.
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, RSTransactionAdapterBegin, Function | SmallTest | Level1)
{
    RSTransactionAdapter adapter(rsUIContext_);
    adapter.Begin();
    if (rsUIContext_) {
        EXPECT_EQ(RSTransactionAdapter::invokerType_, InvokerType::RS_TRANSACTION_HANDLER);
    } else {
        EXPECT_EQ(RSTransactionAdapter::invokerType_, InvokerType::RS_TRANSACTION_PROXY);
    }
}

/**
 * @tc.name: RSTransactionAdapterCommit
 * @tc.desc: Verify RSTransactionAdapter::Commit works.
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, RSTransactionAdapterCommit, Function | SmallTest | Level1)
{
    RSTransactionAdapter adapter(rsUIContext_);
    adapter.Commit();
    if (rsUIContext_) {
        EXPECT_EQ(RSTransactionAdapter::invokerType_, InvokerType::RS_TRANSACTION_HANDLER);
    } else {
        EXPECT_EQ(RSTransactionAdapter::invokerType_, InvokerType::RS_TRANSACTION_PROXY);
    }
}

/**
 * @tc.name: RSTransactionAdapterFlushImplicitTransaction
 * @tc.desc: Verify RSTransactionAdapter::FlushImplicitTransaction (member function).
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, RSTransactionAdapterFlushImplicitTransaction, Function | SmallTest | Level1)
{
    RSTransactionAdapter adapter(rsUIContext_);
    adapter.FlushImplicitTransaction();
    if (rsUIContext_) {
        EXPECT_EQ(RSTransactionAdapter::invokerType_, InvokerType::RS_TRANSACTION_HANDLER);
    } else {
        EXPECT_EQ(RSTransactionAdapter::invokerType_, InvokerType::RS_TRANSACTION_PROXY);
    }
}

/**
 * @tc.name: RSTransactionAdapterStaticFlushImplicitTransaction
 * @tc.desc: Verify static FlushImplicitTransaction with RSUIContext.
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, RSTransactionAdapterStaticFlushImplicitTransaction, Function | SmallTest | Level1)
{
    RSTransactionAdapter::FlushImplicitTransaction(rsUIContext_);
    if (rsUIContext_) {
        EXPECT_EQ(RSTransactionAdapter::invokerType_, InvokerType::RS_TRANSACTION_HANDLER);
    } else {
        EXPECT_EQ(RSTransactionAdapter::invokerType_, InvokerType::RS_TRANSACTION_PROXY);
    }
}

/**
 * @tc.name: AutoRSTransactionLifecycle
 * @tc.desc: Verify AutoRSTransaction lifecycle (Begin + Commit).
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, AutoRSTransactionLifecycle, Function | SmallTest | Level1)
{
    auto mockAdapter = std::make_shared<MockRSTransactionAdapter>(rsUIContext_);
    EXPECT_CALL(*mockAdapter, Begin())
        .Times(1);
    EXPECT_CALL(*mockAdapter, Commit(_))
        .Times(1);

    {
        AutoRSTransaction autoTransaction(mockAdapter, true);
    }
}

/**
 * @tc.name: RSSyncTransactionAdapterConstructor
 * @tc.desc: Verify RSSyncTransactionAdapter constructed with RSUIContext.
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, RSSyncTransactionAdapterConstructor, Function | SmallTest | Level1)
{
    RSSyncTransactionAdapter adapter(rsUIContext_);
    if (rsUIContext_) {
        EXPECT_NE(adapter.rsSyncTransHandler_, nullptr);
        EXPECT_EQ(adapter.rsSyncTransController_, nullptr);
    } else {
        EXPECT_EQ(adapter.rsSyncTransHandler_, nullptr);
        EXPECT_NE(adapter.rsSyncTransController_, nullptr);
    }
}

/**
 * @tc.name: RSSyncTransactionAdapterGetRSTransaction
 * @tc.desc: Verify RSSyncTransactionAdapter::GetRSTransaction.
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, RSSyncTransactionAdapterGetRSTransaction, Function | SmallTest | Level1)
{
    RSSyncTransactionAdapter adapter(rsUIContext_);
    adapter.GetRSTransaction();
    if (rsUIContext_) {
        EXPECT_EQ(RSSyncTransactionAdapter::invokerType_, InvokerType::RS_SYNC_TRANSACTION_HANDLER);
    } else {
        EXPECT_EQ(RSSyncTransactionAdapter::invokerType_, InvokerType::RS_SYNC_TRANSACTION_CONTROLLER);
    }
}

/**
 * @tc.name: RSSyncTransactionAdapterOpenTransaction
 * @tc.desc: Verify RSSyncTransactionAdapter::OpenSyncTransaction.
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, RSSyncTransactionAdapterOpenTransaction, Function | SmallTest | Level1)
{
    RSSyncTransactionAdapter adapter(rsUIContext_);
    adapter.OpenSyncTransaction();
    if (rsUIContext_) {
        EXPECT_EQ(RSSyncTransactionAdapter::invokerType_, InvokerType::RS_SYNC_TRANSACTION_HANDLER);
    } else {
        EXPECT_EQ(RSSyncTransactionAdapter::invokerType_, InvokerType::RS_SYNC_TRANSACTION_CONTROLLER);
    }
}

/**
 * @tc.name: RSSyncTransactionAdapterCloseTransaction
 * @tc.desc: Verify RSSyncTransactionAdapter::CloseSyncTransaction.
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, RSSyncTransactionAdapterCloseTransaction, Function | SmallTest | Level1)
{
    RSSyncTransactionAdapter adapter(rsUIContext_);
    adapter.CloseSyncTransaction();
    if (rsUIContext_) {
        EXPECT_EQ(RSSyncTransactionAdapter::invokerType_, InvokerType::RS_SYNC_TRANSACTION_HANDLER);
    } else {
        EXPECT_EQ(RSSyncTransactionAdapter::invokerType_, InvokerType::RS_SYNC_TRANSACTION_CONTROLLER);
    }
}

/**
 * @tc.name: RSSyncTransactionAdapterStaticGetRSTransaction
 * @tc.desc: Verify static GetRSTransaction with RSUIContext.
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, RSSyncTransactionAdapterStaticGetRSTransaction, Function | SmallTest | Level1)
{
    RSSyncTransactionAdapter::GetRSTransaction(rsUIContext_);
    if (rsUIContext_) {
        EXPECT_EQ(RSSyncTransactionAdapter::invokerType_, InvokerType::RS_SYNC_TRANSACTION_HANDLER);
    } else {
        EXPECT_EQ(RSSyncTransactionAdapter::invokerType_, InvokerType::RS_SYNC_TRANSACTION_CONTROLLER);
    }
}

/**
 * @tc.name: RSSyncTransactionAdapterStaticOpenSyncTransaction
 * @tc.desc: Verify static OpenSyncTransaction with RSUIContext.
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, RSSyncTransactionAdapterStaticOpenSyncTransaction, Function | SmallTest | Level1)
{
    RSSyncTransactionAdapter::OpenSyncTransaction(rsUIContext_);
    if (rsUIContext_) {
        EXPECT_EQ(RSSyncTransactionAdapter::invokerType_, InvokerType::RS_SYNC_TRANSACTION_HANDLER);
    } else {
        EXPECT_EQ(RSSyncTransactionAdapter::invokerType_, InvokerType::RS_SYNC_TRANSACTION_CONTROLLER);
    }
}

/**
 * @tc.name: RSSyncTransactionAdapterStaticCloseSyncTransaction
 * @tc.desc: Verify static CloseSyncTransaction with RSUIContext.
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, RSSyncTransactionAdapterStaticCloseSyncTransaction, Function | SmallTest | Level1)
{
    RSSyncTransactionAdapter::CloseSyncTransaction(rsUIContext_);
    if (rsUIContext_) {
        EXPECT_EQ(RSSyncTransactionAdapter::invokerType_, InvokerType::RS_SYNC_TRANSACTION_HANDLER);
    } else {
        EXPECT_EQ(RSSyncTransactionAdapter::invokerType_, InvokerType::RS_SYNC_TRANSACTION_CONTROLLER);
    }
}

/**
 * @tc.name: AutoRSSyncTransactionLifecycle
 * @tc.desc: Verify AutoRSSyncTransaction lifecycle (Open + Close).
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, AutoRSSyncTransactionLifecycle, Function | SmallTest | Level1)
{
    auto mockAdapter = std::make_shared<MockRSSyncTransactionAdapter>(rsUIContext_);
    EXPECT_CALL(*mockAdapter, OpenSyncTransaction(_))
        .Times(1);
    EXPECT_CALL(*mockAdapter, CloseSyncTransaction(_))
        .Times(1);

    {
        AutoRSSyncTransaction autoSyncTransaction(mockAdapter);
    }
}

/**
 * @tc.name: AllowRSMultiInstanceLifecycle
 * @tc.desc: Verify AllowRSMultiInstance lifecycle (skip check toggle).
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, AllowRSMultiInstanceLifecycle, Function | SmallTest | Level1)
{
    {
        AllowRSMultiInstance allowMulti(rsNode_);
        EXPECT_TRUE(rsNode_->isSkipCheckInMultiInstance_);
    }
    EXPECT_FALSE(rsNode_->isSkipCheckInMultiInstance_);
}

/**
 * @tc.name: RSAdapterUtilToStringMethods
 * @tc.desc: Verify RSAdapterUtil ToString methods.
 * @tc.type: FUNC
 */
HWTEST_F(RSAdapterTest, RSAdapterUtilToStringMethods, Function | SmallTest | Level1)
{
    auto nodeStr = RSAdapterUtil::RSNodeToStr(rsNode_);
    EXPECT_THAT(nodeStr, HasSubstr("RSNode"));

    auto ctxStr = RSAdapterUtil::RSUIContextToStr(rsUIContext_);
    EXPECT_THAT(ctxStr, HasSubstr("RSUIContext"));

    auto directorStr = RSAdapterUtil::RSUIDirectorToStr(rsUIDirector_);
    EXPECT_THAT(directorStr, HasSubstr("RSUIDirector"));
}
} // namespace Rosen
} // namespace OHOS
