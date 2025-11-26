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

#include "move_resampler.h"

#include <algorithm>
#include <cmath>

namespace OHOS {
namespace Rosen {
double OneEuroFilter::Filter(int64_t curTimeUs, double curValue)
{
    constexpr int64_t RESET_TIME_US = 50 * 1000; // 50ms

    // Reset state on first input, timestamp reversal, or large time jump
    if (!initialized || curTimeUs <= preTimeUs || (curTimeUs - preTimeUs) > RESET_TIME_US) {
        initialized = true;
        preValue = curValue;
        preTimeUs = curTimeUs;
        preDeriv = 0;
        return curValue;
    }

    // Calculate time delta in seconds
    double dt = (curTimeUs - preTimeUs) / 1e6; // us to s
    // Protect against extremely small time deltas leading to huge noise
    constexpr double MIN_DT = 1e-4;
    if (dt < MIN_DT) {
        dt = MIN_DT;
    }

    const auto& [minCutoff, beta, dCutoff] = filterParam_;

    // Use previously filtered value for derivative estimation to reduce noise
    double rawDeriv = (curValue - preValue) / dt;

    // First-stage low-pass filter for derivative
    double filteredDeriv = Lowpass(preDeriv, rawDeriv, Alpha(dCutoff, dt));

    // Adaptive cutoff adjusts smoothing intensity based on motion speed
    double adaptiveCutoff = minCutoff + beta * std::fabs(filteredDeriv);

    // Second-stage low-pass filter for the actual value
    double filteredValue = Lowpass(preValue, curValue, Alpha(adaptiveCutoff, dt));

    preValue = filteredValue;
    preDeriv = filteredDeriv;
    preTimeUs = curTimeUs;
    return filteredValue;
}

double OneEuroFilter::Alpha(double cutoff, double dt) const
{
    constexpr double MIN_VALUE = 1e-6;
    // Clamp values to avoid division by zero and degenerate smoothing
    cutoff = std::max(cutoff, MIN_VALUE);
    dt = std::max(dt, MIN_VALUE);

    const double tau = 1.0 / (2.0 * M_PI * cutoff);
    return 1.0 / (1.0 + tau / dt);
}

double OneEuroFilter::Lowpass(double prev, double curr, double alpha) const
{
    return alpha * curr + (1.0 - alpha) * prev;
}

void OneEuroFilter::Reset()
{
    initialized = false;
    preTimeUs = 0;
    preValue = 0;
    preDeriv = 0;
}

void MoveResampler::PushEvent(int64_t timeUs, int32_t posX, int32_t posY)
{
    events_.push_back({ timeUs, posX, posY });
    CleanupOldEvents(timeUs);
}

MoveEvent MoveResampler::ResampleAt(int64_t targetTimeUs)
{
    auto [rawX, rawY] = ResampleRaw(targetTimeUs);
    auto filteredX = filterX_.Filter(targetTimeUs, rawX);
    auto filteredY = filterY_.Filter(targetTimeUs, rawY);
    return {
        .timeUs = targetTimeUs,
        .posX = static_cast<int32_t>(std::round(filteredX)),
        .posY = static_cast<int32_t>(std::round(filteredY))
    };
}

void MoveResampler::Reset()
{
    events_.clear();
    filterX_.Reset();
    filterY_.Reset();
}

std::pair<double, double> MoveResampler::ResampleRaw(int64_t targetTimeUs)
{
    // Clear old events first
    CleanupOldEvents(targetTimeUs);

    if (events_.empty()) {
        return { 0.0, 0.0 };
    }

    // Only one event available → no sampling, direct value return;
    // Or if target time is older than the earliest event → clamp to first event.
    if (events_.size() == 1 || targetTimeUs <= events_.front().timeUs) {
        const auto& e = events_.front();
        return { static_cast<double>(e.posX), static_cast<double>(e.posY) };
    }

    // If target time is inside the recorded range → linear interpolation.
    if (targetTimeUs < events_.back().timeUs) {
        return InterpolateLinear(targetTimeUs);
    }

    // Otherwise: target time is newer than the latest event → extrapolation or fitting.
    return ExtrapolateFit(targetTimeUs);
}

size_t MoveResampler::FindSegmentIndex(int64_t targetTimeUs) const
{
    size_t left = 1;
    size_t right = events_.size() - 1;

    while (left < right) {
        size_t mid = (left + right) / 2;
        if (events_[mid].timeUs < targetTimeUs) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

std::pair<double, double> MoveResampler::LinearFitAt(size_t startIdx, size_t endIdx, int64_t targetTimeUs) const
{
    if (events_.empty()) {
        return { 0.0, 0.0 };
    }

    if (startIdx > endIdx || endIdx >= events_.size()) {
        const auto& e = events_.back();
        return {static_cast<double>(e.posX), static_cast<double>(e.posY)};
    }

    size_t n = endIdx - startIdx + 1;

    // First pass: compute means
    double sumT = 0.0;
    double sumX = 0.0;
    double sumY = 0.0;

    for (size_t i = startIdx; i <= endIdx; ++i) {
        const auto& e = events_[i];
        sumT += static_cast<double>(e.timeUs);
        sumX += static_cast<double>(e.posX);
        sumY += static_cast<double>(e.posY);
    }

    double meanT = sumT / n;
    double meanX = sumX / n;
    double meanY = sumY / n;

    // Second pass: least-squares mean-centered
    double sumTT = 0.0;  // Σ (t^2)
    double sumTdx = 0.0; // Σ (t * (x - meanX))
    double sumTdy = 0.0; // Σ (t * (y - meanY))

    for (size_t i = startIdx; i <= endIdx; ++i) {
        const auto& e = events_[i];
        double t = static_cast<double>(e.timeUs) - meanT;
        double dx = static_cast<double>(e.posX) - meanX;
        double dy = static_cast<double>(e.posY) - meanY;

        sumTT += t * t;
        sumTdx += t * dx;
        sumTdy += t * dy;
    }

    constexpr double EPS = 1e-12;
    double slopeX = 0.0;
    double slopeY = 0.0;

    if (std::abs(sumTT) >= EPS) {
        slopeX = sumTdx / sumTT;
        slopeY = sumTdy / sumTT;
    }

    // Evaluate at target time (mean-centered)
    double tQuery = static_cast<double>(targetTimeUs) - meanT;
    double x = meanX + slopeX * tQuery;
    double y = meanY + slopeY * tQuery;
    return { x, y };
}

std::pair<double, double> MoveResampler::InterpolateLinear(int64_t targetTimeUs) const
{
    if (events_.empty()) {
        return { 0.0, 0.0 };
    }

    if (events_.size() == 1) {
        const auto& e = events_.back();
        return { static_cast<double>(e.posX), static_cast<double>(e.posY) };
    }

    size_t idx = FindSegmentIndex(targetTimeUs);
    const MoveEvent& prev = events_[idx - 1];
    const MoveEvent& next = events_[idx];

    double span = static_cast<double>(next.timeUs - prev.timeUs);
    if (span <= 0.0) {
        return { static_cast<double>(next.posX), static_cast<double>(next.posY) };
    }

    // Smooth prev using LinearFitAt
    size_t prevStart = std::max(static_cast<size_t>(0), idx - 4); // 4: using up to last 4 points
    size_t prevEnd = idx - 1;

    auto [prevSmoothX, prevSmoothY] = LinearFitAt(prevStart, prevEnd, prev.timeUs);

    // Smooth next using LinearFitAt
    size_t nextStart = std::max(static_cast<size_t>(0), idx - 3); // 3: using up to last 3 points
    size_t nextEnd = idx;

    auto [nextSmoothX, nextSmoothY] = LinearFitAt(nextStart, nextEnd, next.timeUs);

    // Standard linear interpolation
    double ratio = (static_cast<double>(targetTimeUs - prev.timeUs) / span);
    double x = prevSmoothX + ratio * (nextSmoothX - prevSmoothX);
    double y = prevSmoothY + ratio * (nextSmoothY - prevSmoothY);
    return { x, y };
}

std::pair<double, double> MoveResampler::ExtrapolateFit(int64_t targetTimeUs) const
{
    if (events_.empty()) {
        return { 0.0, 0.0 };
    }
    size_t nEvents = events_.size();
    size_t startIdx = std::max(static_cast<size_t>(0), nEvents - 5); // 5: using up to last 5 points
    size_t endIdx = nEvents - 1;
    return LinearFitAt(startIdx, endIdx, targetTimeUs);
}

void MoveResampler::CleanupOldEvents(int64_t currentTimeUs)
{
    const int64_t threshold = currentTimeUs - maxEventIntervalUs_;
    while (!events_.empty() && events_.front().timeUs < threshold) {
        events_.pop_front();
    }
}
} // namespace Rosen
} // namespace OHOS
