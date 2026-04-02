#ifndef MUSICPP_VECTORS_H
#define MUSICPP_VECTORS_H

#include "position_vector.h"
#include "interval_vector.h"
#include "binary_vector.h"

namespace musicpp {

inline interval_vector positions_to_intervals(const position_vector& positions) {
    const int m = positions.mod();
    if (positions.size() == 0) {
        return interval_vector({}, 0, m);
    }
    const std::vector<int>& posData = positions.data();
    const size_t n = posData.size();
    std::vector<int> intervalData;
    intervalData.reserve(n);

    if (n == 1) {
        return interval_vector(intervalData, positions[0], m);
    }

    for (size_t i = 0; i + 1 < n; ++i) {
        intervalData.emplace_back(posData[i + 1] - posData[i]);
    }
    const int closureRaw = posData[0] - posData[static_cast<size_t>(n - 1)];
    const int periodMod = positions.mod();
    if (periodMod > 0) {
        intervalData.emplace_back(euclidean_division(closureRaw, periodMod).remainder);
    } else {
        intervalData.emplace_back(closureRaw);
    }

    return interval_vector(intervalData, positions[0], m);
}

inline position_vector intervals_to_positions(const interval_vector& intervals) {
    const int m = intervals.mod();
    std::vector<int> intervalData = intervals.data();

    if (intervalData.empty()) {
        return position_vector({0}, m, 0, true, false);
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

inline binary_vector positions_to_binary(position_vector& positions) {
    if (positions.size() == 0) {
        return binary_vector({}, 0, positions.mod());
    }

    std::vector<int> posData = positions.data();
    const int range = positions.effective_range();
    std::vector<int> binaryData(static_cast<size_t>(range), 0);

    const int minPos = *std::min_element(posData.begin(), posData.end());
    for (int pos : posData) {
        const int normalizedPos = pos - posData[0];
        const division_result div = euclidean_division(normalizedPos, range);
        binaryData[static_cast<size_t>(div.remainder)] = 1;
    }

    return binary_vector(binaryData, minPos, range);
}

/**
 * @class vector_set
 * @brief Synchronized position, interval, and binary representations of a musical set.
 */
class vector_set {
private:
    position_vector positions_;
    interval_vector intervals_;
    binary_vector binary_;
    int mod_;

public:
    vector_set()
        : positions_({0}, 12, 0, true, false),
          intervals_({}, 0, 12),
          binary_({1}, 0, 12),
          mod_(12) {}

    explicit vector_set(int modulo)
        : positions_({0}, modulo, 0, true, false),
          intervals_({}, 0, modulo),
          binary_({1}, 0, modulo),
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

    explicit vector_set(const binary_vector& bv)
        : binary_(bv),
          mod_(bv.mod()) {
        update_from_binary();
    }

    const position_vector& positions() const { return positions_; }
    position_vector& positions() { return positions_; }

    const interval_vector& intervals() const { return intervals_; }
    interval_vector& intervals() { return intervals_; }

    const binary_vector& binary() const { return binary_; }
    binary_vector& binary() { return binary_; }

    int mod() const { return mod_; }
    void set_mod(int m) { mod_ = m; }

    void update_from_positions() {
        intervals_ = positions_to_intervals(positions_);
        binary_ = positions_to_binary(positions_);
    }

    void update_from_intervals() {
        positions_ = intervals_to_positions(intervals_);
        binary_ = positions_to_binary(positions_);
    }

    void update_from_binary() {
        positions_ = binary_to_positions();
        intervals_ = positions_to_intervals(positions_);
    }

private:
    position_vector binary_to_positions() const {
        const std::vector<int>& binaryData = binary_.data();
        const int offset = binary_.offset();

        std::vector<int> posData;
        for (size_t i = 0; i < binaryData.size(); ++i) {
            if (binaryData[i] == 1) {
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

    vector_set rotate_positions(int amount) {
        vector_set result = *this;
        result.positions_ = positions_.rotate(amount);
        result.update_from_positions();
        return result;
    }

    vector_set roto_translate_positions(int amount, int length = 0) {
        vector_set result = *this;
        result.positions_ = positions_.roto_translate(amount, length);
        result.update_from_positions();
        return result;
    }

    vector_set inversion(int amount, int length = 0) {
        return roto_translate_positions(amount, length);
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

    vector_set rotate_intervals(int amount) {
        vector_set result = *this;
        result.intervals_ = intervals_.rotate(amount);
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

    vector_set mode(int amount) { return rotate_intervals(amount); }

    vector_set rotate_binary(int amount) {
        vector_set result = *this;
        result.binary_ = binary_.rotate(amount);
        result.update_from_binary();
        return result;
    }

    vector_set complement_binary() {
        vector_set result = *this;
        result.binary_ = binary_.complement();
        result.update_from_binary();
        return result;
    }

    vector_set multiply_binary(int scalar) {
        vector_set result = *this;
        result.binary_ = binary_ * scalar;
        result.mod_ = result.binary_.mod();
        result.update_from_binary();
        return result;
    }

    vector_set divide_binary(int divisor) {
        vector_set result = *this;
        result.binary_ = binary_ / divisor;
        result.mod_ = result.binary_.mod();
        result.update_from_binary();
        return result;
    }

    vector_set operator|(const vector_set& other) const {
        vector_set result = *this;
        result.binary_ = binary_ | other.binary_;
        result.update_from_binary();
        return result;
    }

    vector_set operator&(const vector_set& other) const {
        vector_set result = *this;
        result.binary_ = binary_ & other.binary_;
        result.update_from_binary();
        return result;
    }

    vector_set operator^(const vector_set& other) const {
        vector_set result = *this;
        result.binary_ = binary_ ^ other.binary_;
        result.update_from_binary();
        return result;
    }

    void print_all() const {
        std::cout << "=== vector_set (mod=" << mod_ << ") ===" << std::endl;
        std::cout << "Positions: " << positions_ << std::endl;
        std::cout << "Intervals: " << intervals_ << std::endl;
        std::cout << "Binary:    " << binary_ << std::endl;
        std::cout << "Pattern:   ";
        binary_.printPattern();
    }

    bool operator==(const vector_set& other) const {
        return positions_ == other.positions_ && intervals_ == other.intervals_ && binary_ == other.binary_;
    }

    bool operator!=(const vector_set& other) const { return !(*this == other); }

    static vector_set from_positions(const std::vector<int>& data, int modulo = 12) {
        return vector_set(position_vector(data, modulo, -1, true, false));
    }

    static vector_set from_intervals(const std::vector<int>& data, int modulo = 12) {
        return vector_set(interval_vector(data, 0, modulo));
    }

    static vector_set from_binary(const std::vector<int>& data, int offset = 0, int modulo = 12) {
        return vector_set(binary_vector(data, offset, modulo));
    }

    static vector_set euclidean(int pulses, int steps) {
        return vector_set(binary_vector::euclidean(pulses, steps));
    }
};

} // namespace musicpp

#endif // MUSICPP_VECTORS_H
