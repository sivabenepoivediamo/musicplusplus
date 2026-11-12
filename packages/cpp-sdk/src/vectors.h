#ifndef MUSICPP_VECTORS_H
#define MUSICPP_VECTORS_H

#include "position_vector.h"
#include "interval_vector.h"
#include "onset_vector.h"

namespace musicpp {

inline interval_vector positions_to_intervals(const position_vector& positions) {
    const int m = positions.mod();
    if (positions.size() == 0) {
        return interval_vector({}, 0, m);
    }
    const size_t n = positions.size();
    std::vector<int> intervalData;
    intervalData.reserve(n);

    if (n == 1) {
        return interval_vector(intervalData, positions[0], m);
    }

    for (size_t i = 0; i + 1 < n; ++i) {
        intervalData.emplace_back(
            positions[static_cast<int>(i + 1)] - positions[static_cast<int>(i)]);
    }
    const int closureRaw =
        positions[0] - positions[static_cast<int>(n - 1)];
    const int closurePeriod = positions.effective_range();
    if (closurePeriod > 0) {
        intervalData.emplace_back(euclidean_division(closureRaw, closurePeriod).remainder);
    } else {
        intervalData.emplace_back(closureRaw);
    }

    return interval_vector(intervalData, positions[0], m);
}

/**
 * @brief Reconstructs pitch positions from an interval string and starting offset.
 *
 * @note When @p intervals has no interval steps (empty `data()`), the result is a
 *       single-element `position_vector` holding only `intervals.offset()`. This is
 *       not symmetric with `positions_to_intervals` on a one-note scale (which yields
 *       an empty interval list); round-trips across empty/singleton cases may change
 *       element counts by design.
 */
inline position_vector intervals_to_positions(const interval_vector& intervals) {
    const int m = intervals.mod();
    std::vector<int> intervalData = intervals.data();

    if (intervalData.empty()) {
        return position_vector({intervals.offset()}, m, 0, true, false);
    }

    std::vector<int> posData;
    posData.reserve(intervalData.size());
    int currentPos = intervals.offset();
    posData.emplace_back(currentPos);

    for (size_t i = 0; i + 1 < intervalData.size(); ++i) {
        currentPos += intervalData[i];
        posData.emplace_back(currentPos);
    }

    return position_vector(posData, m, 0, true, false);
}

/**
 * @brief Converts pitch positions to a binary onset pattern over one cycle.
 * @details Onset indices use each pitch relative to the minimum pitch in @p positions
 *          (not the first stored element), so unsorted position lists map correctly.
 */
inline onset_vector positions_to_onset(position_vector& positions) {
    if (positions.size() == 0) {
        return onset_vector({}, 0, positions.mod());
    }

    std::vector<int> posData = positions.data();
    const int range = positions.effective_range();
    if (range <= 0) {
        return onset_vector({}, 0, positions.mod());
    }

    std::vector<int> onsetData(static_cast<size_t>(range), 0);

    const int minPos = *std::min_element(posData.begin(), posData.end());
    for (int pos : posData) {
        const int normalizedPos = pos - minPos;
        const division_result div = euclidean_division(normalizedPos, range);
        onsetData[static_cast<size_t>(div.remainder)] = 1;
    }

    return onset_vector(onsetData, minPos, range);
}

/**
 * @class vector_set
 * @brief Synchronized position, interval, and onset-vector representations of a musical set.
 */
class vector_set {
private:
    position_vector positions_;
    interval_vector intervals_;
    onset_vector onset_;
    int mod_;

public:
    vector_set()
        : positions_({0}, 12, 0, true, false),
          intervals_({}, 0, 12),
          onset_({1}, 0, 12),
          mod_(12) {}

    explicit vector_set(int modulo)
        : positions_({0}, modulo, 0, true, false),
          intervals_({}, 0, modulo),
          onset_({1}, 0, modulo),
          mod_(modulo) {}

    explicit vector_set(const position_vector& pv)
        : positions_(pv),
          mod_(pv.mod()) {
        update_from_positions();
    }

    explicit vector_set(const interval_vector& iv)
        : intervals_(iv),
          mod_(iv.mod()) {
        update_from_intervals();
    }

    explicit vector_set(const onset_vector& ov)
        : onset_(ov),
          mod_(ov.mod()) {
        update_from_onset();
    }

    const position_vector& positions() const { return positions_; }
    position_vector& positions() { return positions_; }

    const interval_vector& intervals() const { return intervals_; }
    interval_vector& intervals() { return intervals_; }

    const onset_vector& onset() const { return onset_; }
    onset_vector& onset() { return onset_; }

    int mod() const { return mod_; }

    /**
     * @brief Sets the musical modulus for this set and all three representations.
     * @details Updates `mod_` and propagates the same modulus to positions, intervals,
     *          and onset so the trio stays consistent.
     */
    void set_mod(int m) {
        mod_ = m;
        positions_.set_mod(m);
        intervals_.set_mod(m);
        onset_.set_mod(m);
    }

    void update_from_positions() {
        intervals_ = positions_to_intervals(positions_);
        onset_ = positions_to_onset(positions_);
    }

    void update_from_intervals() {
        positions_ = intervals_to_positions(intervals_);
        onset_ = positions_to_onset(positions_);
    }

    void update_from_onset() {
        positions_ = onset_to_positions();
        intervals_ = positions_to_intervals(positions_);
    }

private:
    position_vector onset_to_positions() const {
        const std::vector<int>& onsetData = onset_.data();
        const int offset = onset_.offset();

        std::vector<int> posData;
        for (size_t i = 0; i < onsetData.size(); ++i) {
            if (onsetData[i] == 1) {
                posData.emplace_back(static_cast<int>(i) + offset);
            }
        }

        if (posData.empty()) {
            return position_vector({0}, mod_, 0, true, false);
        }

        return position_vector(posData, mod_, 0, true, false);
    }

public:
    vector_set transpose(int amount) {
        vector_set result = *this;
        result.positions_ = positions_ + amount;
        result.update_from_positions();
        return result;
    }

    vector_set multiply_positions(int scalar) {
        vector_set result = *this;
        result.positions_ = positions_ * scalar;
        result.update_from_positions();
        return result;
    }

    vector_set negative(int axis = 10) {
        vector_set result = *this;
        result.positions_ = positions_.negative(axis);
        result.update_from_positions();
        return result;
    }

    vector_set rotate(int amount) {
        vector_set result = *this;
        result.positions_ = positions_.rotate(amount);
        result.update_from_positions();
        return result;
    }

    vector_set relative_mode(int amount, int length = 0) {
        vector_set result = *this;
        result.positions_ = positions_.relative_mode(amount, length);
        result.update_from_positions();
        return result;
    }

    vector_set inversion(int amount, int length = 0) {
        return relative_mode(amount, length);
    }

    vector_set invert_positions(int axisIndex, bool sortOutput = true) {
        vector_set result = *this;
        result.positions_ = positions_.inversion(axisIndex, sortOutput);
        result.update_from_positions();
        return result;
    }

    vector_set complement_positions() {
        vector_set result = *this;
        result.positions_ = positions_.complement();
        result.update_from_positions();
        return result;
    }

    vector_set add_to_intervals(int amount) {
        vector_set result = *this;
        result.intervals_ = intervals_ + amount;
        result.update_from_intervals();
        return result;
    }

    vector_set multiply_intervals(int scalar) {
        vector_set result = *this;
        result.intervals_ = intervals_ * scalar;
        result.update_from_intervals();
        return result;
    }

    vector_set parallel_mode(int amount) {
        vector_set result = *this;
        result.intervals_ = intervals_.parallel_mode(amount);
        result.update_from_intervals();
        return result;
    }

    vector_set reverse_intervals() {
        vector_set result = *this;
        result.intervals_ = intervals_.reverse();
        result.update_from_intervals();
        return result;
    }

    vector_set invert_intervals(int axisIndex) {
        vector_set result = *this;
        result.intervals_ = intervals_.inversion(axisIndex);
        result.update_from_intervals();
        return result;
    }

    vector_set mode(int amount) { return parallel_mode(amount); }

    vector_set rotate_onset(int amount) {
        vector_set result = *this;
        result.onset_ = onset_.rotate(amount);
        result.update_from_onset();
        return result;
    }

    vector_set complement_onset() {
        vector_set result = *this;
        result.onset_ = onset_.complement();
        result.update_from_onset();
        return result;
    }

    vector_set multiply_onset(int scalar) {
        vector_set result = *this;
        result.onset_ = onset_ * scalar;
        result.mod_ = result.onset_.mod();
        result.update_from_onset();
        return result;
    }

    vector_set divide_onset(int divisor) {
        vector_set result = *this;
        result.onset_ = onset_ / divisor;
        result.mod_ = result.onset_.mod();
        result.update_from_onset();
        return result;
    }

    vector_set operator|(const vector_set& other) const {
        vector_set result = *this;
        result.onset_ = onset_ | other.onset_;
        result.update_from_onset();
        return result;
    }

    vector_set operator&(const vector_set& other) const {
        vector_set result = *this;
        result.onset_ = onset_ & other.onset_;
        result.update_from_onset();
        return result;
    }

    vector_set operator^(const vector_set& other) const {
        vector_set result = *this;
        result.onset_ = onset_ ^ other.onset_;
        result.update_from_onset();
        return result;
    }

    void print_all() const {
        std::cout << "=== vector_set (mod=" << mod_ << ") ===" << std::endl;
        std::cout << "Positions: " << positions_ << std::endl;
        std::cout << "Intervals: " << intervals_ << std::endl;
        std::cout << "Onset:     " << onset_ << std::endl;
        std::cout << "Pattern:   ";
        onset_.printPattern();
    }

    bool operator==(const vector_set& other) const {
        return positions_ == other.positions_ && intervals_ == other.intervals_ && onset_ == other.onset_;
    }

    bool operator!=(const vector_set& other) const { return !(*this == other); }

    static vector_set from_positions(const std::vector<int>& data, int modulo = 12) {
        return vector_set(position_vector(data, modulo, -1, true, false));
    }

    static vector_set from_intervals(const std::vector<int>& data, int modulo = 12) {
        return vector_set(interval_vector(data, 0, modulo));
    }

    static vector_set from_onset(const std::vector<int>& data, int offset = 0, int modulo = 12) {
        return vector_set(onset_vector(data, offset, modulo));
    }

    static vector_set euclidean(int pulses, int steps) {
        return vector_set(onset_vector::euclidean(pulses, steps));
    }
};

} // namespace musicpp

#endif // MUSICPP_VECTORS_H
