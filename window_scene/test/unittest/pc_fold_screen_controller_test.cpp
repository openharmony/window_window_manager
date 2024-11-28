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

#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <chrono>
#include "session/host/include/pc_fold_screen_controller.h"
#include "session/host/include/main_session.h"
#include "wm_math.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

namespace {
// screen const
constexpr int32_t SCREEN_WIDTH = 2472;
constexpr int32_t SCREEN_HEIGHT = 3296;
constexpr int32_t SCREEN_HEIGHT_HALF = 1648;
constexpr int32_t CREASE_HEIGHT = 48;
constexpr int32_t TOP_AVOID_HEIGHT = 56;
constexpr int32_t BOT_AVOID_HEIGHT = 112;

// screen rects
const WSRect DISPLAY_RECT = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
const WSRect DEFAULT_DISPLAY_RECT = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT_HALF };
const WSRect VIRTUAL_DISPLAY_RECT = { 0, SCREEN_HEIGHT_HALF, SCREEN_WIDTH, SCREEN_HEIGHT_HALF };
const WSRect FOLD_CREASE_RECT = { 0, SCREEN_HEIGHT_HALF - CREASE_HEIGHT / 2, SCREEN_WIDTH, CREASE_HEIGHT };
const WSRect ZERO_RECT = { 0, 0, 0, 0 };
const WSRect DEFAULT_FULLSCREEN_RECT = { 0, TOP_AVOID_HEIGHT, SCREEN_WIDTH,
                                         SCREEN_HEIGHT_HALF - CREASE_HEIGHT / 2 - TOP_AVOID_HEIGHT};
const WSRect VIRTUAL_FULLSCREEN_RECT = { 0, SCREEN_HEIGHT_HALF + CREASE_HEIGHT / 2, SCREEN_WIDTH,
                                         SCREEN_HEIGHT_HALF - CREASE_HEIGHT / 2 - BOT_AVOID_HEIGHT };

// test rects
const WSRect B_RECT = { 100, 100, 400, 400 };
const WSRect B_ACROSS_RECT = { 400, SCREEN_HEIGHT_HALF - 300, 400, 400 };
const WSRect B_TOP_RECT = { 400, -100, 400, 400 };
const WSRect C_RECT = { 100, SCREEN_HEIGHT_HALF + 100, 400, 400 };
const WSRect C_ACROSS_RECT = { 400, SCREEN_HEIGHT_HALF - 100, 400, 400 };
const WSRect C_BOT_RECT = { 400, SCREEN_HEIGHT - 100, 400, 400 };

// arrange rule
constexpr int32_t RULE_TRANS_X = 48; // dp
constexpr int32_t MIN_DECOR_HEIGHT = 37; // dp
constexpr int32_t MAX_DECOR_HEIGHT = 112; // dp

// velocity test
constexpr int32_t MOVING_RECORDS_SIZE_LIMIT = 5;
constexpr int32_t MOVING_RECORDS_TIME_LIMIT_IN_NS = 100000;
const WSRectF B_VELOCITY = { 0.0f, 1.0f, 0.0f, 0.0f };
const WSRectF B_VELOCITY_SLOW = { 0.0f, 0.1f, 0.0f, 0.0f };
const WSRectF B_VELOCITY_SKEW = { 0.5f, 0.5f, 0.0f, 0.0f };
const WSRectF C_VELOCITY = { 0.0f, -1.0f, 0.0f, 0.0f };
const WSRectF C_VELOCITY_SLOW = { 0.0f, -0.1f, 0.0f, 0.0f };
const WSRectF C_VELOCITY_SKEW = { 0.5f, -0.5f, 0.0f, 0.0f };
} // namespace

class PcFoldScreenManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static void SetHalfFolded();
    static void SetExpanded();

    static PcFoldScreenManager& manager_;
};

PcFoldScreenManager& PcFoldScreenManagerTest::manager_ = PcFoldScreenManager::GetInstance();

void PcFoldScreenManagerTest::SetUpTestCase()
{
}

void PcFoldScreenManagerTest::TearDownTestCase()
{
}

void PcFoldScreenManagerTest::SetUp()
{
    SetExpanded();
}

void PcFoldScreenManagerTest::TearDown()
{
}

void PcFoldScreenManagerTest::SetHalfFolded()
{
    manager_.UpdateFoldScreenStatus(DEFAULT_SCREEN_ID, SuperFoldStatus::HALF_FOLDED,
        DEFAULT_DISPLAY_RECT, VIRTUAL_DISPLAY_RECT, FOLD_CREASE_RECT);
}

void PcFoldScreenManagerTest::SetExpanded()
{
    manager_.UpdateFoldScreenStatus(DEFAULT_SCREEN_ID, SuperFoldStatus::EXPANDED,
        DISPLAY_RECT, ZERO_RECT, ZERO_RECT);
}

class PcFoldScreenControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static void SetHalfFolded();
    static void SetExpanded();

    static PcFoldScreenManager& manager_;
    sptr<MainSession> mainSession_;
    sptr<PcFoldScreenController> controller_;
};

PcFoldScreenManager& PcFoldScreenControllerTest::manager_ = PcFoldScreenManager::GetInstance();

void PcFoldScreenControllerTest::SetUpTestCase()
{
}

void PcFoldScreenControllerTest::TearDownTestCase()
{
}

void PcFoldScreenControllerTest::SetUp()
{
    // set up main session
    SessionInfo info;
    info.abilityName_ = "MainSession";
    info.bundleName_ = "MainSession";
    mainSession_ = sptr<MainSession>::MakeSptr(info, nullptr);

    ASSERT_NE(mainSession_, nullptr);
    ASSERT_NE(mainSession_->pcFoldScreenController_, nullptr);
    controller_ = mainSession_->pcFoldScreenController_;

    SetExpanded();
}

void PcFoldScreenControllerTest::TearDown()
{
    controller_ = nullptr;
    mainSession_ = nullptr;
}

void PcFoldScreenControllerTest::SetHalfFolded()
{
    manager_.UpdateFoldScreenStatus(DEFAULT_SCREEN_ID, SuperFoldStatus::HALF_FOLDED,
        DEFAULT_DISPLAY_RECT, VIRTUAL_DISPLAY_RECT, FOLD_CREASE_RECT);
}

void PcFoldScreenControllerTest::SetExpanded()
{
    manager_.UpdateFoldScreenStatus(DEFAULT_SCREEN_ID, SuperFoldStatus::EXPANDED,
        DISPLAY_RECT, ZERO_RECT, ZERO_RECT);
}

/**
 * @tc.name: SetDisplayInfo
 * @tc.desc: test function : SetDisplayInfo
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenManagerTest, SetDisplayInfo, Function | SmallTest | Level1)
{
    SetExpanded();
    int64_t otherDisplayId = 100;
    manager_.SetDisplayInfo(DEFAULT_SCREEN_ID, SuperFoldStatus::EXPANDED);
    // update display id
    manager_.SetDisplayInfo(otherDisplayId, SuperFoldStatus::EXPANDED);
    EXPECT_EQ(otherDisplayId, manager_.displayId_);
    EXPECT_EQ(SuperFoldStatus::EXPANDED, manager_.screenFoldStatus_);
    // update fold status
    manager_.SetDisplayInfo(otherDisplayId, SuperFoldStatus::HALF_FOLDED);
    EXPECT_EQ(SuperFoldStatus::HALF_FOLDED, manager_.screenFoldStatus_);
}

namespace {
/**
 * @tc.name: IsHalfFolded
 * @tc.desc: test function : IsHalfFolded
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenManagerTest, IsHalfFolded, Function | SmallTest | Level1)
{
    SetHalfFolded();
    EXPECT_TRUE(manager_.IsHalfFolded(DEFAULT_SCREEN_ID));
    SetExpanded();
    EXPECT_TRUE(!manager_.IsHalfFolded(DEFAULT_SCREEN_ID));
}

/**
 * @tc.name: SetDisplayRects
 * @tc.desc: test function : SetDisplayRects, GetDisplayRects
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenManagerTest, SetDisplayRects, Function | SmallTest | Level1)
{
    manager_.SetDisplayRects(DISPLAY_RECT, ZERO_RECT, ZERO_RECT);
    const auto& [rect1, rect2, rect3] = manager_.GetDisplayRects();
    EXPECT_EQ(DISPLAY_RECT, rect1);
    EXPECT_EQ(ZERO_RECT, rect2);
    EXPECT_EQ(ZERO_RECT, rect3);
    manager_.SetDisplayRects(DEFAULT_DISPLAY_RECT, VIRTUAL_DISPLAY_RECT, FOLD_CREASE_RECT);
    const auto& [rect4, rect5, rect6] = manager_.GetDisplayRects();
    EXPECT_EQ(DEFAULT_DISPLAY_RECT, rect4);
    EXPECT_EQ(VIRTUAL_DISPLAY_RECT, rect5);
    EXPECT_EQ(FOLD_CREASE_RECT, rect6);
}

/**
 * @tc.name: CalculateScreenSide
 * @tc.desc: test function : CalculateScreenSide
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenManagerTest, CalculateScreenSide, Function | SmallTest | Level1)
{
    SetHalfFolded();
    EXPECT_EQ(manager_.CalculateScreenSide(B_ACROSS_RECT), ScreenSide::FOLD_B);
    EXPECT_EQ(manager_.CalculateScreenSide(C_ACROSS_RECT), ScreenSide::FOLD_C);
}

/**
 * @tc.name: ResetArrangeRule
 * @tc.desc: test function : ResetArrangeRule, including overload functions
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenManagerTest, ResetArrangeRule, Function | SmallTest | Level2)
{
    SetHalfFolded();
    // param: rect
    manager_.defaultArrangedRect_ = B_RECT;
    manager_.virtualArrangedRect_ = C_RECT;
    manager_.ResetArrangeRule(B_ACROSS_RECT);
    EXPECT_EQ(ZERO_RECT, manager_.defaultArrangedRect_);
    EXPECT_NE(ZERO_RECT, manager_.virtualArrangedRect_);
    manager_.defaultArrangedRect_ = B_RECT;
    manager_.virtualArrangedRect_ = C_RECT;
    manager_.ResetArrangeRule(C_ACROSS_RECT);
    EXPECT_NE(ZERO_RECT, manager_.defaultArrangedRect_);
    EXPECT_EQ(ZERO_RECT, manager_.virtualArrangedRect_);

    // param: void
    manager_.defaultArrangedRect_ = B_RECT;
    manager_.virtualArrangedRect_ = C_RECT;
    manager_.ResetArrangeRule();
    EXPECT_EQ(ZERO_RECT, manager_.defaultArrangedRect_);
    EXPECT_EQ(ZERO_RECT, manager_.virtualArrangedRect_);

    // param: side
    manager_.defaultArrangedRect_ = B_RECT;
    manager_.virtualArrangedRect_ = C_RECT;
    manager_.ResetArrangeRule(ScreenSide::EXPAND);
    EXPECT_NE(ZERO_RECT, manager_.defaultArrangedRect_);
    EXPECT_NE(ZERO_RECT, manager_.virtualArrangedRect_);
    manager_.defaultArrangedRect_ = B_RECT;
    manager_.virtualArrangedRect_ = C_RECT;
    manager_.ResetArrangeRule(ScreenSide::FOLD_B);
    EXPECT_EQ(ZERO_RECT, manager_.defaultArrangedRect_);
    EXPECT_NE(ZERO_RECT, manager_.virtualArrangedRect_);
    manager_.defaultArrangedRect_ = B_RECT;
    manager_.virtualArrangedRect_ = C_RECT;
    manager_.ResetArrangeRule(ScreenSide::FOLD_C);
    EXPECT_NE(ZERO_RECT, manager_.defaultArrangedRect_);
    EXPECT_EQ(ZERO_RECT, manager_.virtualArrangedRect_);
}

/**
 * @tc.name: ResizeToFullScreen
 * @tc.desc: test function : ResizeToFullScreen
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenManagerTest, ResizeToFullScreen, Function | SmallTest | Level2)
{
    SetHalfFolded();
    WSRect rect = B_RECT;
    manager_.ResizeToFullScreen(rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT);
    EXPECT_EQ(rect, DEFAULT_FULLSCREEN_RECT);
    rect = C_RECT;
    manager_.ResizeToFullScreen(rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT);
    EXPECT_EQ(rect, VIRTUAL_FULLSCREEN_RECT);
}

/**
 * @tc.name: NeedDoThrowSlip
 * @tc.desc: test function : NeedDoThrowSlip
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenManagerTest, NeedDoThrowSlip, Function | SmallTest | Level1)
{
    // side B
    EXPECT_TRUE(manager_.NeedDoThrowSlip(ScreenSide::FOLD_B, B_VELOCITY));
    EXPECT_FALSE(manager_.NeedDoThrowSlip(ScreenSide::FOLD_B, B_VELOCITY_SLOW));
    EXPECT_FALSE(manager_.NeedDoThrowSlip(ScreenSide::FOLD_B, B_VELOCITY_SKEW));
    // side C
    EXPECT_TRUE(manager_.NeedDoThrowSlip(ScreenSide::FOLD_C, C_VELOCITY));
    EXPECT_FALSE(manager_.NeedDoThrowSlip(ScreenSide::FOLD_C, C_VELOCITY_SLOW));
    EXPECT_FALSE(manager_.NeedDoThrowSlip(ScreenSide::FOLD_C, C_VELOCITY_SKEW));
}

/**
 * @tc.name: ThrowSlipToOppositeSide
 * @tc.desc: test function : ThrowSlipToOppositeSide
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenManagerTest, ThrowSlipToOppositeSide, Function | SmallTest | Level1)
{
    WSRect rect = B_RECT;
    EXPECT_FALSE(manager_.ThrowSlipToOppositeSide(ScreenSide::EXPAND, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT, 0));
    EXPECT_TRUE(manager_.ThrowSlipToOppositeSide(ScreenSide::FOLD_B, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT, 0));
    rect = C_RECT;
    EXPECT_TRUE(manager_.ThrowSlipToOppositeSide(ScreenSide::FOLD_C, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT, 0));
}

/**
 * @tc.name: MappingRectInScreenSide
 * @tc.desc: test function : MappingRectInScreenSide
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenManagerTest, MappingRectInScreenSide, Function | SmallTest | Level1)
{
    // half folded
    SetHalfFolded();
    WSRect rect = B_ACROSS_RECT;
    WSRect rectMapped = rect;
    manager_.MappingRectInScreenSide(ScreenSide::FOLD_B, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT);
    rectMapped.posY_ = FOLD_CREASE_RECT.posY_ - 400;
    EXPECT_EQ(rect, rectMapped);

    rect = B_TOP_RECT;
    rectMapped = rect;
    manager_.MappingRectInScreenSide(ScreenSide::FOLD_B, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT);
    rectMapped.posY_ = TOP_AVOID_HEIGHT;
    EXPECT_EQ(rect, rectMapped);

    rect = C_ACROSS_RECT;
    rectMapped = rect;
    manager_.MappingRectInScreenSide(ScreenSide::FOLD_C, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT);
    rectMapped.posY_ = FOLD_CREASE_RECT.posY_ + FOLD_CREASE_RECT.height_;
    EXPECT_EQ(rect, rectMapped);

    rect = C_BOT_RECT;
    rectMapped = rect;
    manager_.MappingRectInScreenSide(ScreenSide::FOLD_C, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT);
    rectMapped.posY_ = SCREEN_HEIGHT - BOT_AVOID_HEIGHT - MIN_DECOR_HEIGHT * manager_.vpr_;
    EXPECT_EQ(rect, rectMapped);

    // flattened
    SetExpanded();
    rect = B_ACROSS_RECT;
    rectMapped = rect;
    manager_.MappingRectInScreenSide(ScreenSide::EXPAND, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT);
    EXPECT_EQ(rect, rectMapped);
}

/**
 * @tc.name: MappingRectInScreenSideWithArrangeRule1
 * @tc.desc: test function : MappingRectInScreenSideWithArrangeRule + ApplyInitArrangeRule
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenManagerTest, MappingRectInScreenSideWithArrangeRule1, Function | SmallTest | Level1)
{
    int32_t decorHeight = (MIN_DECOR_HEIGHT + MAX_DECOR_HEIGHT) / 2;

    SetHalfFolded();
    float vpr = manager_.vpr_;
    WSRect rect = B_RECT;

    // init arrange rule
    manager_.defaultArrangedRect_ = ZERO_RECT;
    rect = B_RECT;
    manager_.MappingRectInScreenSideWithArrangeRule(ScreenSide::FOLD_B, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT,
        decorHeight);
    const int32_t centerBX = (DEFAULT_DISPLAY_RECT.width_ - B_RECT.width_) / 2;
    const int32_t centerBY = TOP_AVOID_HEIGHT + (FOLD_CREASE_RECT.posY_ - TOP_AVOID_HEIGHT - B_RECT.height_) / 2;
    EXPECT_EQ(rect, (WSRect { centerBX, centerBY, B_RECT.width_, B_RECT.height_ }));
    EXPECT_EQ(manager_.defaultArrangedRect_,
        (WSRect { centerBX, centerBY, RULE_TRANS_X * vpr, decorHeight * vpr }));

    manager_.virtualArrangedRect_ = ZERO_RECT;
    rect = C_RECT;
    manager_.MappingRectInScreenSideWithArrangeRule(ScreenSide::FOLD_C, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT,
        decorHeight);
    const int32_t centerCX = (VIRTUAL_DISPLAY_RECT.width_ - C_RECT.width_) / 2;
    const int32_t centerCY = FOLD_CREASE_RECT.posY_ + FOLD_CREASE_RECT.height_ +
        (SCREEN_HEIGHT - FOLD_CREASE_RECT.posY_ - FOLD_CREASE_RECT.height_ - BOT_AVOID_HEIGHT - C_RECT.height_) / 2;
    EXPECT_EQ(rect, (WSRect { centerCX, centerCY, C_RECT.width_, C_RECT.height_ }));
    EXPECT_EQ(manager_.virtualArrangedRect_,
        (WSRect { centerCX, centerCY, RULE_TRANS_X * vpr, decorHeight * vpr }));
}

/**
 * @tc.name: MappingRectInScreenSideWithArrangeRule2
 * @tc.desc: test function : MappingRectInScreenSideWithArrangeRule + ApplyArrangeRule, normal arrange rule
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenManagerTest, MappingRectInScreenSideWithArrangeRule2, Function | SmallTest | Level1)
{
    int32_t decorHeight = (MIN_DECOR_HEIGHT + MAX_DECOR_HEIGHT) / 2;

    SetHalfFolded();
    float vpr = manager_.vpr_;
    WSRect rect = B_RECT;

    // normal arrange rule
    manager_.defaultArrangedRect_ = WSRect { 100, 100, RULE_TRANS_X * vpr, 100 * vpr };
    rect = B_RECT;
    manager_.MappingRectInScreenSideWithArrangeRule(ScreenSide::FOLD_B, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT,
        decorHeight);
    EXPECT_EQ(rect, (WSRect { 100 + RULE_TRANS_X * vpr, 100 + 100 * vpr, B_RECT.width_, B_RECT.height_ }));
    EXPECT_EQ(manager_.defaultArrangedRect_,
        (WSRect { 100 + RULE_TRANS_X * vpr, 100 + 100 * vpr, RULE_TRANS_X * vpr, decorHeight * vpr }));

    manager_.virtualArrangedRect_ = WSRect { 100, SCREEN_HEIGHT_HALF + 100, RULE_TRANS_X * vpr, 100 * vpr };
    rect = C_RECT;
    manager_.MappingRectInScreenSideWithArrangeRule(ScreenSide::FOLD_C, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT,
        decorHeight);
    EXPECT_EQ(rect,
        (WSRect { 100 + RULE_TRANS_X * vpr, SCREEN_HEIGHT_HALF + 100 + 100 * vpr, C_RECT.width_, C_RECT.height_ }));
    EXPECT_EQ(manager_.virtualArrangedRect_,
        (WSRect { 100 + RULE_TRANS_X * vpr, SCREEN_HEIGHT_HALF + 100 + 100 * vpr,
                RULE_TRANS_X * vpr, decorHeight * vpr }));
}

/**
 * @tc.name: MappingRectInScreenSideWithArrangeRule3
 * @tc.desc: test function : MappingRectInScreenSideWithArrangeRule + ApplyArrangeRule, special arrange rule
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenManagerTest, MappingRectInScreenSideWithArrangeRule3, Function | SmallTest | Level1)
{
    int32_t decorHeight = (MIN_DECOR_HEIGHT + MAX_DECOR_HEIGHT) / 2;

    SetHalfFolded();
    float vpr = manager_.vpr_;
    WSRect rect = B_RECT;
    int32_t virtualPosY = FOLD_CREASE_RECT.posY_ + FOLD_CREASE_RECT.height_;

    // new column rule
    manager_.defaultArrangedRect_ = WSRect { 100, SCREEN_HEIGHT_HALF - 200, RULE_TRANS_X * vpr, 100 * vpr };
    rect = B_RECT;
    manager_.MappingRectInScreenSideWithArrangeRule(ScreenSide::FOLD_B, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT,
        decorHeight);
    EXPECT_EQ(rect, (WSRect { 100 + RULE_TRANS_X * vpr, TOP_AVOID_HEIGHT, B_RECT.width_, B_RECT.height_ }));
    EXPECT_EQ(manager_.defaultArrangedRect_,
        (WSRect { 100 + RULE_TRANS_X * vpr, TOP_AVOID_HEIGHT, RULE_TRANS_X * vpr, decorHeight * vpr }));

    manager_.virtualArrangedRect_ = WSRect { 100, SCREEN_HEIGHT - 200, RULE_TRANS_X * vpr, 100 * vpr };
    rect = C_RECT;
    manager_.MappingRectInScreenSideWithArrangeRule(ScreenSide::FOLD_C, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT,
        decorHeight);
    EXPECT_EQ(rect,
        (WSRect { 100 + RULE_TRANS_X * vpr, virtualPosY, C_RECT.width_, C_RECT.height_ }));
    EXPECT_EQ(manager_.virtualArrangedRect_,
        (WSRect { 100 + RULE_TRANS_X * vpr, virtualPosY, RULE_TRANS_X * vpr, decorHeight * vpr }));

    // reset rule
    manager_.defaultArrangedRect_ = WSRect { SCREEN_WIDTH - 200, 100, RULE_TRANS_X * vpr, 100 * vpr };
    rect = B_RECT;
    manager_.MappingRectInScreenSideWithArrangeRule(ScreenSide::FOLD_B, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT,
        decorHeight);
    EXPECT_EQ(rect, (WSRect { 0, TOP_AVOID_HEIGHT, B_RECT.width_, B_RECT.height_ }));
    EXPECT_EQ(manager_.defaultArrangedRect_,
        (WSRect { 0, TOP_AVOID_HEIGHT, RULE_TRANS_X * vpr, decorHeight * vpr }));

    manager_.virtualArrangedRect_ = WSRect { SCREEN_WIDTH - 200, 100, RULE_TRANS_X * vpr, 100 * vpr };
    rect = C_RECT;
    manager_.MappingRectInScreenSideWithArrangeRule(ScreenSide::FOLD_C, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT,
        decorHeight);
    EXPECT_EQ(rect, (WSRect { 0, virtualPosY, C_RECT.width_, C_RECT.height_ }));
    EXPECT_EQ(manager_.virtualArrangedRect_,
        (WSRect { 0, virtualPosY, RULE_TRANS_X * vpr, decorHeight * vpr }));
}

/**
 * @tc.name: RegisterFoldScreenStatusChangeCallback
 * @tc.desc: test function : RegisterFoldScreenStatusChangeCallback, UnregisterFoldScreenStatusChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenManagerTest, RegisterFoldScreenStatusChangeCallback, Function | SmallTest | Level2)
{
    auto& callbacks = manager_.foldScreenStatusChangeCallbacks_;
    callbacks.clear();
    EXPECT_EQ(callbacks.size(), 0);
    int32_t persistentId = 100;
    auto func = std::make_shared<FoldScreenStatusChangeCallback>(
        [](DisplayId displayId, SuperFoldStatus status, SuperFoldStatus prevStatus) {}
    );
    manager_.RegisterFoldScreenStatusChangeCallback(persistentId, std::weak_ptr<FoldScreenStatusChangeCallback>(func));
    EXPECT_NE(callbacks.find(persistentId), callbacks.end());
    manager_.UnregisterFoldScreenStatusChangeCallback(persistentId);
    EXPECT_EQ(callbacks.find(persistentId), callbacks.end());
}

/**
 * @tc.name: ExecuteFoldScreenStatusChangeCallbacks
 * @tc.desc: test function : ExecuteFoldScreenStatusChangeCallbacks
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenManagerTest, ExecuteFoldScreenStatusChangeCallbacks, Function | SmallTest | Level2)
{
    auto& callbacks = manager_.foldScreenStatusChangeCallbacks_;
    callbacks.clear();
    EXPECT_EQ(callbacks.size(), 0);
    int32_t persistentId = 100;
    DisplayId testDisplayId = 0;
    SuperFoldStatus testStatus = SuperFoldStatus::UNKNOWN;
    SuperFoldStatus testPrevStatus = SuperFoldStatus::UNKNOWN;
    auto func = std::make_shared<FoldScreenStatusChangeCallback>(
        [&testDisplayId, &testStatus, &testPrevStatus](DisplayId displayId,
            SuperFoldStatus status, SuperFoldStatus prevStatus) {
            testDisplayId = displayId;
            testStatus = status;
            testPrevStatus = prevStatus;
        }
    );
    manager_.RegisterFoldScreenStatusChangeCallback(persistentId, std::weak_ptr<FoldScreenStatusChangeCallback>(func));
    EXPECT_NE(callbacks.find(persistentId), callbacks.end());
    manager_.ExecuteFoldScreenStatusChangeCallbacks(100, SuperFoldStatus::EXPANDED, SuperFoldStatus::HALF_FOLDED);
    EXPECT_EQ(testDisplayId, 100);
    EXPECT_EQ(testStatus, SuperFoldStatus::EXPANDED);
    EXPECT_EQ(testPrevStatus, SuperFoldStatus::HALF_FOLDED);
}

/**
 * @tc.name: GetVpr
 * @tc.desc: test function : GetVpr
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenManagerTest, GetVpr, Function | SmallTest | Level2)
{
    manager_.vpr_ = 1.0;
    EXPECT_TRUE(MathHelper::NearZero(manager_.GetVpr() - 1.0));
}

/**
 * @tc.name: RecordStartMoveRect
 * @tc.desc: test function : RecordStartMoveRect, IsStartFullScreen
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenControllerTest, RecordStartMoveRect, Function | SmallTest | Level2)
{
    WSRect rect = { 100, 100, 200, 200 };
    controller_->RecordStartMoveRect(rect, true);
    EXPECT_EQ(controller_->startMoveRect_, rect);
    EXPECT_EQ(controller_->IsStartFullScreen(), true);
}

/**
 * @tc.name: RecordMoveRects
 * @tc.desc: test function : RecordMoveRects
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenControllerTest, RecordMoveRects, Function | SmallTest | Level2)
{
    WSRect rect = ZERO_RECT;
    for (int i = 0; i < MOVING_RECORDS_SIZE_LIMIT; ++i) {
        controller_->RecordMoveRects(rect);
        EXPECT_EQ(i + 1, controller_->movingRectRecords_.size());
    }
    controller_->RecordMoveRects(rect);
    EXPECT_EQ(MOVING_RECORDS_SIZE_LIMIT, controller_->movingRectRecords_.size());
    usleep(2 * MOVING_RECORDS_TIME_LIMIT_IN_NS);
    controller_->RecordMoveRects(rect);
    EXPECT_EQ(1, controller_->movingRectRecords_.size());
}

/**
 * @tc.name: ThrowSlip1
 * @tc.desc: test function : ThrowSlip in B
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenControllerTest, ThrowSlip1, Function | SmallTest | Level1)
{
    SetHalfFolded();
    WSRect rect0 = B_RECT;
    WSRect rect = ZERO_RECT;

    // throw
    controller_->RecordStartMoveRect(rect0, false);
    controller_->RecordMoveRects(rect0);
    usleep(10000);
    rect = rect0;
    rect.posY_ += 400;
    controller_->RecordMoveRects(rect);
    EXPECT_TRUE(controller_->ThrowSlip(DEFAULT_SCREEN_ID, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT));

    // throw failed
    controller_->movingRectRecords_.clear();
    controller_->RecordStartMoveRect(rect0, false);
    controller_->RecordMoveRects(rect0);
    usleep(10000);
    rect = rect0;
    rect.posY_ += 1;
    controller_->RecordMoveRects(rect);
    EXPECT_FALSE(controller_->ThrowSlip(DEFAULT_SCREEN_ID, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT));
}

/**
 * @tc.name: ThrowSlip2
 * @tc.desc: test function : ThrowSlip in C
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenControllerTest, ThrowSlip2, Function | SmallTest | Level1)
{
    SetHalfFolded();
    WSRect rect0 = { 100, SCREEN_HEIGHT - 500, 400, 400 };
    WSRect rect = ZERO_RECT;

    // throw
    controller_->RecordStartMoveRect(rect0, false);
    controller_->RecordMoveRects(rect0);
    usleep(10000);
    rect = rect0;
    rect.posY_ -= 400;
    controller_->RecordMoveRects(rect);
    EXPECT_TRUE(controller_->ThrowSlip(DEFAULT_SCREEN_ID, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT));

    // throw failed
    controller_->movingRectRecords_.clear();
    controller_->RecordStartMoveRect(rect0, false);
    controller_->RecordMoveRects(rect0);
    usleep(10000);
    rect = rect0;
    rect.posY_ -= 1;
    controller_->RecordMoveRects(rect);
    EXPECT_FALSE(controller_->ThrowSlip(DEFAULT_SCREEN_ID, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT));
}

/**
 * @tc.name: ThrowSlip3
 * @tc.desc: test function : ThrowSlip when flattened
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenControllerTest, ThrowSlip3, Function | SmallTest | Level1)
{
    SetExpanded();
    WSRect rect0 = B_RECT;
    WSRect rect = ZERO_RECT;

    // throw failed
    controller_->RecordStartMoveRect(rect0, false);
    controller_->RecordMoveRects(rect0);
    usleep(10000);
    rect = rect0;
    rect.posY_ += 400;
    controller_->RecordMoveRects(rect);
    EXPECT_FALSE(controller_->ThrowSlip(DEFAULT_SCREEN_ID, rect, TOP_AVOID_HEIGHT, BOT_AVOID_HEIGHT));
}

/**
 * @tc.name: UpdateFullScreenWaterfallMode
 * @tc.desc: test function : UpdateFullScreenWaterfallMode, IsFullScreenWaterfallMode
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenControllerTest, UpdateFullScreenWaterfallMode, Function | SmallTest | Level2)
{
    controller_->UpdateFullScreenWaterfallMode(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(controller_->IsFullScreenWaterfallMode());
    controller_->UpdateFullScreenWaterfallMode(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(controller_->IsFullScreenWaterfallMode());
}

/**
 * @tc.name: RegisterFullScreenWaterfallModeChangeCallback
 * @tc.desc: test function : RegisterFullScreenWaterfallModeChangeCallback
 * @tc.desc: test function : UnregisterFullScreenWaterfallModeChangeCallback
 * @tc.desc: test function : ExecuteFullScreenWaterfallModeChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenControllerTest, RegisterFullScreenWaterfallModeChangeCallback, Function | SmallTest | Level2)
{
    // register and unregister
    auto func1 = [](bool isWaterfallMode) {};
    controller_->RegisterFullScreenWaterfallModeChangeCallback(std::move(func1));
    ASSERT_NE(controller_->fullScreenWaterfallModeChangeCallback_, nullptr);
    controller_->UnregisterFullScreenWaterfallModeChangeCallback();
    ASSERT_EQ(controller_->fullScreenWaterfallModeChangeCallback_, nullptr);

    // register and execute
    controller_->isFullScreenWaterfallMode_ = true;
    bool testWaterfallMode = false;
    auto func2 = [&testWaterfallMode](bool isWaterfallMode) {
        testWaterfallMode = isWaterfallMode;
    };
    controller_->RegisterFullScreenWaterfallModeChangeCallback(std::move(func2));
    ASSERT_NE(controller_->fullScreenWaterfallModeChangeCallback_, nullptr);
    EXPECT_TRUE(testWaterfallMode);
    controller_->isFullScreenWaterfallMode_ = false;
    controller_->ExecuteFullScreenWaterfallModeChangeCallback();
    EXPECT_FALSE(testWaterfallMode);
}

/**
 * @tc.name: GetPersistentId
 * @tc.desc: test function : GetPersistentId
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenControllerTest, GetPersistentId, Function | SmallTest | Level3)
{
    controller_->persistentId_ = 100;
    EXPECT_EQ(mainSession_->pcFoldScreenController_->GetPersistentId(), 100);
}

/**
 * @tc.name: GetTitleHeight
 * @tc.desc: test function : GetTitleHeight
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenControllerTest, GetTitleHeight, Function | SmallTest | Level2)
{
    // min
    mainSession_->customDecorHeight_ = MIN_DECOR_HEIGHT - 10;
    EXPECT_EQ(controller_->GetTitleHeight(), MIN_DECOR_HEIGHT);
    // normal
    const int32_t height = (MIN_DECOR_HEIGHT + MAX_DECOR_HEIGHT) / 2;
    mainSession_->customDecorHeight_ = height;
    EXPECT_EQ(controller_->GetTitleHeight(), height);
    // max
    mainSession_->customDecorHeight_ = MAX_DECOR_HEIGHT + 10;
    EXPECT_EQ(controller_->GetTitleHeight(), MAX_DECOR_HEIGHT);
}

/**
 * @tc.name: CalculateMovingVelocity
 * @tc.desc: test function : CalculateMovingVelocity
 * @tc.type: FUNC
 */
HWTEST_F(PcFoldScreenControllerTest, CalculateMovingVelocity, Function | SmallTest | Level1)
{
    SetHalfFolded();
    WSRect rect0 = { 100, SCREEN_HEIGHT - 500, 400, 400 };
    WSRect rect = ZERO_RECT;
    int32_t dt = 50;
    float err = 0.05f;

    // throw
    controller_->RecordStartMoveRect(rect0, false);
    controller_->RecordMoveRects(rect0);
    auto t0 = std::chrono::high_resolution_clock::now();
    usleep(dt * 1000);
    rect = rect0;
    rect.posX_ += dt;
    rect.posY_ += dt;
    rect.width_ += dt;
    rect.height_ += dt;
    controller_->RecordMoveRects(rect);
    auto t1 = std::chrono::high_resolution_clock::now();
    float ratio = float(TimeHelper::GetDuration(t0, t1)) / float(dt);
    WSRectF velocity = controller_->CalculateMovingVelocity();
    EXPECT_LE(std::abs(velocity.posX_ - ratio), err);
    EXPECT_LE(std::abs(velocity.posY_ - ratio), err);
    EXPECT_LE(std::abs(velocity.width_ - ratio), err);
    EXPECT_LE(std::abs(velocity.height_ - ratio), err);
}
}
}
}