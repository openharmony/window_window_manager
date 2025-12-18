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

#ifndef OHOS_ROSEN_WINDOW_SCENE_MOVE_RESAMPLER_H
#define OHOS_ROSEN_WINDOW_SCENE_MOVE_RESAMPLER_H

#include <cstdint>
#include <deque>
#include <sstream>
#include <string>

namespace OHOS {
namespace Rosen {
/**
 * @brief Represents a move event with timestamp and position.
 */
struct MoveEvent {
    int64_t timeUs = 0;
    int32_t posX = 0;
    int32_t posY = 0;

    std::string ToString() const
    {
        std::ostringstream oss;
        oss << timeUs << ": [" << posX << ", " << posY << "]";
        return oss.str();
    }
};

/**
 * @brief Parameters for One Euro Filter.
 */
struct FilterParam {
    double minCutoff;
    double beta;
    double dCutoff;
};

/**
 * @brief Recommended One Euro Filter parameters
 */
const FilterParam DEFAULT_FILTER_PARAM = { 0.8, 0.005, 1.0 };

/**
 * @brief One Euro Filter implementation for smoothing input signals.
 */
class OneEuroFilter {
public:
    /**
     * @brief Constructs a OneEuroFilter with specified parameters.
     *
     * @param filterParam Parameters for the filter.
     */
    explicit OneEuroFilter(const FilterParam& filterParam = DEFAULT_FILTER_PARAM) : filterParam_(filterParam) {}

    /**
     * @brief Applies the One Euro Filter to the input value.
     *        The filter adapts its smoothing rate based on the derivative magnitude.
     *
     * @param curTimeUs Current timestamp in microseconds.
     * @param curValue Current input value to be filtered.
     * @return The filtered output value.
     */
    double Filter(int64_t curTimeUs, double curValue);

    /**
     * @brief Resets the filter state.
     */
    void Reset();

private:
    /**
     * @brief Computes the smoothing factor α for a given cutoff frequency.
     *
     * @param cutoff Cutoff frequency.
     * @param dt Time difference since last update.
     * @return The smoothing factor α.
     */
    double Alpha(double cutoff, double dt) const;

    /**
     * @brief Applies a first-order low-pass filter.
     *
     * @param prev Previous filtered value.
     * @param curr Current input value.
     * @param alpha Smoothing factor.
     * @return The filtered output value.
     */
    double Lowpass(double prev, double curr, double alpha) const;

    FilterParam filterParam_;
    bool initialized = false;
    int64_t preTimeUs = 0;
    double preValue = 0;
    double preDeriv = 0;
};

/**
 * @brief Default maximum event retention time in microseconds.
 */
constexpr int64_t DEFAULT_MAX_EVENT_INTERVAL_US = 60 * 1000; // 60ms

/**
 * @brief Move resampler that buffers input events and produces smooth positions
 *        at requested timestamps via interpolation, extrapolation, and filtering.
 *
 * The resampler can generate positions for timestamps within or near the range
 * of buffered events, depending on available data.
 */
class MoveResampler {
public:
    /**
     * @brief Creates a MoveResampler with the given event retention time and filter parameters.
     *
     * @param maxEventIntervalUs Maximum age of events to keep, in microseconds.
     * @param filterParam        Parameters for the One Euro Filter.
     */
    explicit MoveResampler(int64_t maxEventIntervalUs = DEFAULT_MAX_EVENT_INTERVAL_US,
                           const FilterParam& filterParam = DEFAULT_FILTER_PARAM)
        : maxEventIntervalUs_(maxEventIntervalUs), filterX_(filterParam), filterY_(filterParam) {}

    /**
     * @brief Pushs a new input event to the buffer and discards outdated events.
     *
     * @param timeUs Timestamp of the event in microseconds.
     * @param posX X coordinate of the event.
     * @param posY Y coordinate of the event.
     */
    void PushEvent(int64_t timeUs, int32_t posX, int32_t posY);

    /**
     * @brief Returns a smoothed position at the given timestamp.
     *        Internally performs interpolation or extrapolation, then applies filtering.
     *
     * @param targetTimeUs The target timestamp to sample at (in microseconds).
     * @return The resampled MoveEvent at the target time.
     */
    MoveEvent ResampleAt(int64_t targetTimeUs);

    /**
     * @brief Resets the resampler state, clearing all buffered events and filter states.
     */
    void Reset();

private:
    /**
     * @brief Computes an unfiltered sample at the given timestamp using
     *        interpolation or extrapolation.
     *
     * @param targetTimeUs The target timestamp to sample at (in microseconds).
     * @return A pair of (posX, posY) representing the raw resampled position.
     */
    std::pair<double, double> ResampleRaw(int64_t targetTimeUs);

    /**
     * @brief Finds the index of the first event with timestamp >= targetTimeUs.
     */
    size_t FindSegmentIndex(int64_t targetTimeUs) const;

    /**
     * @brief Performs linear regression over the given event range and evaluates
     *        the fitted line at targetTimeUs.
     *
     * @param startIdx     Start index of the event range (inclusive).
     * @param endIdx       End index of the event range (inclusive).
     * @param targetTimeUs The timestamp to evaluate the fitted line at (in microseconds).
     * @return A pair of (posX, posY) representing the fitted position.
     */
    std::pair<double, double> LinearFitAt(size_t startIdx, size_t endIdx, int64_t targetTimeUs) const;

    /**
     * @brief Computes an interpolated position at targetTimeUs using nearby events.
     *
     * @param targetTimeUs The timestamp to interpolate at (in microseconds).
     * @return A pair of (posX, posY) representing the interpolated position.
     */
    std::pair<double, double> InterpolateLinear(int64_t targetTimeUs) const;

    /**
     * @brief Computes an extrapolated position at targetTimeUs using linear fitting.
     *
     * @param targetTimeUs The timestamp to extrapolate at (in microseconds).
     * @return A pair of (posX, posY) representing the extrapolated position.
     */
    std::pair<double, double> ExtrapolateFit(int64_t targetTimeUs) const;

    /**
     * @brief Removes events older than (currentTimeUs - maxEventIntervalUs_).
     *
     * @param currentTimeUs The current timestamp in microseconds.
     */
    void CleanupOldEvents(int64_t currentTimeUs);

    std::deque<MoveEvent> events_;
    int64_t maxEventIntervalUs_;
    OneEuroFilter filterX_;
    OneEuroFilter filterY_;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_WINDOW_SCENE_MOVE_RESAMPLER_H
