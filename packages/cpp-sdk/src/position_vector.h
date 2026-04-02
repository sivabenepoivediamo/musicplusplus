#ifndef MUSICPP_POSITION_VECTOR_H
#define MUSICPP_POSITION_VECTOR_H

#include "math_util.h"

namespace musicpp {

/**
 * @file position_vector.h
 * @brief Definition of the position_vector class for cyclic positional vectors
 * @author [not251]
 * @date 2025
 */

/**
 * @class position_vector
 * @brief Class to represent a positional vector with cyclic behavior
 * 
 * @details A position_vector is a container of integers that supports:
 * - Cyclic access to elements (indices automatically wrap around)
 * - Automatic scaling for range extensions
 * - Vector and scalar mathematical operations
 * - Specialized geometric operations (rotation, inversion, complement)
 * - Flexible control of the range used for calculations
 * 
 * The vector maintains a modulus (mod) that defines the base cyclic period.
 * The range can be calculated automatically based on data or set manually.
 * 
 * @note All division operations use Euclidean division to
 *       ensure consistent results with negative numbers.
 */
class position_vector {
private:
    std::vector<int> data_;
    int mod_;
    int user_range_;
    int range_;
    bool range_update_;
    bool user_;

private:
    /**
     * @brief Calculates the range needed to contain all values
     * 
     * @details Determines the minimum necessary range based on the data span
     *          (difference between maximum and minimum) and the current modulus.
     *          The range will be a multiple of the modulus sufficient to contain all values.
     * 
     * @return Calculated range as a multiple of the modulus
     * 
     * @note If the vector is empty, returns simply the reference modulus
     */
    int rangeCalculation() const {
        int modulo = user_ ? user_range_ : mod_;
        
        // If there's no data, the range is simply the modulus
        if (data_.empty()) {
            return modulo;
        }

        // Calculate the data span
        int maxValue = *std::max_element(data_.begin(), data_.end());
        int minValue = *std::min_element(data_.begin(), data_.end());
        int span = maxValue - minValue;
        
        // Determine how many multiples of the modulus are necessary
        division_result result = euclidean_division(span, modulo);
        
        return modulo * (result.quotient + 1);
    }

    /**
     * @brief Helper to initialize the range appropriately
     * 
     * @details Determines the initial range value based on:
     *          - If rangeUpdate is active, dynamically calculates the range
     *          - Otherwise uses userRange or mod based on the user flag
     * 
     * @return Initial range value
     */
    int initializeRange() const {
        if (range_update_) {
            return rangeCalculation();
        }
        return user_ ? user_range_ : mod_;
    }

    /**
     * @brief Updates the range if automatic updating is active
     * 
     * @details Centralizes the range update logic used
     *          by all methods that modify the data.
     */
    void updateRangeIfNeeded() {
        if (range_update_) {
            range_ = rangeCalculation();
        }
    }

public:
    /**
     * @brief Default constructor
     * 
     * @details Creates a position_vector with:
     *          - data = {0}
     *          - mod = 12
     *          - userRange = 12
     *          - rangeUpdate = true
     *          - user = false
     */
    position_vector()
        : data_({0}),
          mod_(12),
          user_range_(12),
          range_(0),
          range_update_(true),
          user_(false)
    {
        range_ = initializeRange();
    }

    /**
     * @brief Parameterized constructor
     * 
     * @param data Initial integer vector
     * @param mod Base modulus (cyclic period), default 12
     * @param userRange Custom range, if 0 or negative uses mod, default 0
     * @param rangeUpdate Flag for automatic range updating, default true
     * @param user Flag to use userRange instead of mod, default false
     * 
     * @note If userRange is 0 or negative, it's automatically set equal to mod
     */
    position_vector(const std::vector<int>& values,
                   int mod_in = 12,
                   int user_range_in = 0,
                   bool range_update_in = true,
                   bool user_in = false)
        : data_(values),
          mod_(mod_in),
          user_range_(user_range_in > 0 ? user_range_in : mod_in),
          range_(0),
          range_update_(range_update_in),
          user_(user_in)
    {
        range_ = initializeRange();
    }

    // ==================== SCALAR OPERATORS ====================

    /**
     * @brief Adds a scalar to all elements
     * 
     * @param scalar Value to add
     * @return New position_vector with the added values
     */
    position_vector operator+(int scalar) const {
        std::vector<int> result(data_.size());
        for (size_t i = 0; i < data_.size(); ++i) {
            result[i] = data_[i] + scalar;
        }
        return position_vector(result, mod_, user_range_, range_update_, user_);
    }

    /**
     * @brief Subtracts a scalar from all elements
     * 
     * @param scalar Value to subtract
     * @return New position_vector with the subtracted values
     */
    position_vector operator-(int scalar) const {
        std::vector<int> result(data_.size());
        for (size_t i = 0; i < data_.size(); ++i) {
            result[i] = data_[i] - scalar;
        }
        return position_vector(result, mod_, user_range_, range_update_, user_);
    }

    /**
     * @brief Multiplies all elements by a scalar
     * 
     * @param scalar Multiplication factor
     * @return New position_vector with the multiplied values
     */
    position_vector operator*(int scalar) const {
        std::vector<int> result(data_.size());
        for (size_t i = 0; i < data_.size(); ++i) {
            result[i] = data_[i] * scalar;
        }
        return position_vector(result, mod_, user_range_, range_update_, user_);
    }

    /**
     * @brief Divides all elements by a scalar (Euclidean division)
     * 
     * @param divisor Divisor
     * @return New position_vector with the quotients
     * @throw std::invalid_argument If divisor is 0
     * 
     * @note Uses Euclidean division to guarantee consistent results
     */
    position_vector operator/(int divisor) const {
        if (divisor == 0) {
            throw std::invalid_argument("Division by zero");
        }
        
        std::vector<int> result(data_.size());
        for (size_t i = 0; i < data_.size(); ++i) {
            division_result div = euclidean_division(data_[i], divisor);
            result[i] = div.quotient;
        }
        return position_vector(result, mod_, user_range_, range_update_, user_);
    }

    /**
     * @brief Calculates the remainder of Euclidean division for all elements
     * 
     * @param divisor Divisor
     * @return New position_vector with the remainders
     * @throw std::invalid_argument If divisor is 0
     */
    position_vector operator%(int divisor) const {
        if (divisor == 0) {
            throw std::invalid_argument("Division by zero");
        }
        
        std::vector<int> result(data_.size());
        for (size_t i = 0; i < data_.size(); ++i) {
            division_result div = euclidean_division(data_[i], divisor);
            result[i] = div.remainder;
        }
        return position_vector(result, mod_, user_range_, range_update_, user_);
    }

    // ==================== VECTOR OPERATORS ====================

    /**
     * @brief Component-wise addition with another position_vector
     * 
     * @param other position_vector to add
     * @return New position_vector result of the addition
     * 
     * @note Uses componentwiseSum without looping
     */
    position_vector operator+(const position_vector& other) const {
        return componentwiseSum(other.data_, false);
    }

    /**
     * @brief Component-wise subtraction with another position_vector
     * 
     * @param other position_vector to subtract
     * @return New position_vector result of the subtraction
     * 
     * @note Uses componentwiseSubtraction without looping
     */
    position_vector operator-(const position_vector& other) const {
        return componentwiseSubtraction(other.data_, false);
    }

    /**
     * @brief Component-wise product with another position_vector
     * 
     * @param other position_vector to multiply
     * @return New position_vector result of the product
     * 
     * @note Uses componentwiseProduct with looping
     */
    position_vector operator*(const position_vector& other) const {
        return componentwiseProduct(other.data_, true);
    }

    /**
     * @brief Component-wise division with another position_vector
     * 
     * @param other position_vector divisor
     * @return New position_vector with the quotients
     * @throw std::invalid_argument If other contains zeros
     * 
     * @note Uses componentwiseDivision with looping and Euclidean division
     */
    position_vector operator/(const position_vector& other) const {
        return componentwiseDivision(other.data_, true);
    }

    /**
     * @brief Component-wise modulo with another position_vector
     * 
     * @param other position_vector divisor
     * @return New position_vector with the remainders
     * @throw std::invalid_argument If other contains zeros
     * 
     * @note Uses componentwiseModulo with looping and Euclidean division
     */
    position_vector operator%(const position_vector& other) const {
        return componentwiseModulo(other.data_, true);
    }

    // ==================== OPERATORS WITH VECTOR<INT> ====================

    /**
     * @brief Component-wise addition with a std::vector<int>
     * 
     * @param other Vector to add
     * @return New position_vector result of the addition
     */
    position_vector operator+(const std::vector<int>& other) const {
        return componentwiseSum(other, false);
    }

    /**
     * @brief Component-wise subtraction with a std::vector<int>
     * 
     * @param other Vector to subtract
     * @return New position_vector result of the subtraction
     */
    position_vector operator-(const std::vector<int>& other) const {
        return componentwiseSubtraction(other, false);
    }

    /**
     * @brief Component-wise product with a std::vector<int>
     * 
     * @param other Vector to multiply
     * @return New position_vector result of the product
     */
    position_vector operator*(const std::vector<int>& other) const {
        return componentwiseProduct(other, true);
    }

    /**
     * @brief Component-wise division with a std::vector<int>
     * 
     * @param other Vector divisor
     * @return New position_vector with the quotients
     * @throw std::invalid_argument If other contains zeros
     */
    position_vector operator/(const std::vector<int>& other) const {
        return componentwiseDivision(other, true);
    }

    /**
     * @brief Component-wise modulo with a std::vector<int>
     * 
     * @param other Vector divisor
     * @return New position_vector with the remainders
     * @throw std::invalid_argument If other contains zeros
     */
    position_vector operator%(const std::vector<int>& other) const {
        return componentwiseModulo(other, true);
    }

    // ==================== COMPOUND ASSIGNMENT OPERATORS ====================

    /**
     * @brief Adds and assigns a scalar
     * 
     * @param scalar Value to add
     * @return Reference to this modified object
     */
    position_vector& operator+=(int scalar) {
        for (int& elem : data_) {
            elem += scalar;
        }
        updateRangeIfNeeded();
        return *this;
    }

    /**
     * @brief Subtracts and assigns a scalar
     * 
     * @param scalar Value to subtract
     * @return Reference to this modified object
     */
    position_vector& operator-=(int scalar) {
        for (int& elem : data_) {
            elem -= scalar;
        }
        updateRangeIfNeeded();
        return *this;
    }

    /**
     * @brief Multiplies and assigns a scalar
     * 
     * @param scalar Multiplication factor
     * @return Reference to this modified object
     */
    position_vector& operator*=(int scalar) {
        for (int& elem : data_) {
            elem *= scalar;
        }
        updateRangeIfNeeded();
        return *this;
    }

    /**
     * @brief Divides and assigns a scalar
     * 
     * @param divisor Divisor
     * @return Reference to this modified object
     * @throw std::invalid_argument If divisor is 0
     */
    position_vector& operator/=(int divisor) {
        if (divisor == 0) {
            throw std::invalid_argument("Division by zero");
        }
        
        for (int& elem : data_) {
            division_result div = euclidean_division(elem, divisor);
            elem = div.quotient;
        }
        updateRangeIfNeeded();
        return *this;
    }

    /**
     * @brief Calculates modulo and assigns
     * 
     * @param divisor Divisor
     * @return Reference to this modified object
     * @throw std::invalid_argument If divisor is 0
     */
    position_vector& operator%=(int divisor) {
        if (divisor == 0) {
            throw std::invalid_argument("Division by zero");
        }
        
        for (int& elem : data_) {
            division_result div = euclidean_division(elem, divisor);
            elem = div.remainder;
        }
        updateRangeIfNeeded();
        return *this;
    }

    /**
     * @brief Adds and assigns another position_vector
     * 
     * @param other position_vector to add
     * @return Reference to this modified object
     */
    position_vector& operator+=(const position_vector& other) {
        *this = *this + other;
        return *this;
    }

    /**
     * @brief Subtracts and assigns another position_vector
     * 
     * @param other position_vector to subtract
     * @return Reference to this modified object
     */
    position_vector& operator-=(const position_vector& other) {
        *this = *this - other;
        return *this;
    }

    /**
     * @brief Multiplies and assigns another position_vector
     * 
     * @param other position_vector to multiply
     * @return Reference to this modified object
     */
    position_vector& operator*=(const position_vector& other) {
        *this = *this * other;
        return *this;
    }

    /**
     * @brief Divides and assigns another position_vector
     * 
     * @param other position_vector divisor
     * @return Reference to this modified object
     * @throw std::invalid_argument If other contains zeros
     */
    position_vector& operator/=(const position_vector& other) {
        *this = *this / other;
        return *this;
    }

    /**
     * @brief Calculates modulo and assigns with another position_vector
     * 
     * @param other position_vector divisor
     * @return Reference to this modified object
     * @throw std::invalid_argument If other contains zeros
     */
    position_vector& operator%=(const position_vector& other) {
        *this = *this % other;
        return *this;
    }

    /**
     * @brief Adds and assigns a std::vector<int>
     * 
     * @param other Vector to add
     * @return Reference to this modified object
     */
    position_vector& operator+=(const std::vector<int>& other) {
        *this = *this + other;
        return *this;
    }

    /**
     * @brief Subtracts and assigns a std::vector<int>
     * 
     * @param other Vector to subtract
     * @return Reference to this modified object
     */
    position_vector& operator-=(const std::vector<int>& other) {
        *this = *this - other;
        return *this;
    }

    /**
     * @brief Multiplies and assigns a std::vector<int>
     * 
     * @param other Vector to multiply
     * @return Reference to this modified object
     */
    position_vector& operator*=(const std::vector<int>& other) {
        *this = *this * other;
        return *this;
    }

    /**
     * @brief Divides and assigns a std::vector<int>
     * 
     * @param other Vector divisor
     * @return Reference to this modified object
     * @throw std::invalid_argument If other contains zeros
     */
    position_vector& operator/=(const std::vector<int>& other) {
        *this = *this / other;
        return *this;
    }

    /**
     * @brief Calculates modulo and assigns with a std::vector<int>
     * 
     * @param other Vector divisor
     * @return Reference to this modified object
     * @throw std::invalid_argument If other contains zeros
     */
    position_vector& operator%=(const std::vector<int>& other) {
        *this = *this % other;
        return *this;
    }

    // ==================== ACCESS AND COMPARISON OPERATORS ====================

    /**
     * @brief Cyclic access to elements
     * 
     * @param index Index (can be negative or greater than size)
     * @return Value at the specified index with cyclic wraparound
     * 
     * @details Negative indices access elements from the end,
     *          indices >= size wrap cyclically.
     *          Each complete cycle adds/subtracts the effective range.
     */
    int operator[](int index) const {
        return element(index);
    }

    /**
     * @brief Equality operator
     * 
     * @param other position_vector to compare
     * @return true if the vectors are equal, false otherwise
     * 
     * @note Compares data, mod, userRange and user (not range or rangeUpdate)
     */
    bool operator==(const position_vector& other) const {
        return data_ == other.data_ && mod_ == other.mod_ &&
               user_range_ == other.user_range_ && user_ == other.user_;
    }

    /**
     * @brief Inequality operator
     * 
     * @param other position_vector to compare
     * @return true if the vectors are different, false otherwise
     */
    bool operator!=(const position_vector& other) const {
        return !(*this == other);
    }

    // ==================== FRIEND OPERATORS ====================

    /**
     * @brief Scalar-vector addition (friend)
     * 
     * @param scalar Scalar on the left
     * @param pv position_vector on the right
     * @return New position_vector result of the addition
     */
    friend position_vector operator+(int scalar, const position_vector& pv) {
        return pv + scalar;
    }

    /**
     * @brief Scalar-vector subtraction (friend)
     * 
     * @param scalar Scalar on the left (minuend)
     * @param pv position_vector on the right (subtrahend)
     * @return New position_vector with scalar - elements
     */
    friend position_vector operator-(int scalar, const position_vector& pv) {
        return pv - scalar;
    }

    /**
     * @brief Scalar-vector multiplication (friend)
     * 
     * @param scalar Scalar on the left
     * @param pv position_vector on the right
     * @return New position_vector result of the multiplication
     */
    friend position_vector operator*(int scalar, const position_vector& pv) {
        return pv * scalar;
    }

    /**
     * @brief Output stream operator
     * 
     * @param os Output stream
     * @param pv position_vector to print
     * @return Reference to the stream
     * 
     * @details Format: [elem1, elem2, ..., elemN]
     */
    friend std::ostream& operator<<(std::ostream& os, const position_vector& pv) {
        os << "[";
        for (size_t i = 0; i < pv.data_.size(); ++i) {
            os << pv.data_[i];
            if (i < pv.data_.size() - 1) os << ", ";
        }
        os << "]";
        return os;
    }

    // ==================== MAIN METHODS ====================

    /**
     * @brief Cyclic access to elements with wraparound
     * 
     * @param index Access index (can be any integer)
     * @return Value at the index with cyclic behavior
     * 
     * @details Access is cyclic: negative indices access from the end,
     *          indices >= size wrap. Each complete cycle adds/subtracts
     *          the effective range to the returned value.
     * 
     * @note If the vector is empty, returns 0
     */
    int element(int index) const {
        int size = static_cast<int>(data_.size());
        
        if (size == 0) {
            return 0;
        }
        
        division_result div = euclidean_division(index, size);
        int cycles = (index - div.remainder) / size;
        int effectiveRange = effective_range();
        
        return data_[div.remainder] + std::abs(effectiveRange) * cycles;
    }

    // ==================== ACCESSORS ====================

    const std::vector<int>& data() const { return data_; }
    std::vector<int>& data() { return data_; }

    int mod() const { return mod_; }
    int user_range() const { return user_range_; }
    /** Effective range used in cyclic indexing and complement. */
    int effective_range() const { return range_; }
    bool auto_range_updates() const { return range_update_; }
    bool use_user_range() const { return user_; }

    size_t size() const { return data_.size(); }

    void set_mod(int new_mod) {
        mod_ = new_mod;
        updateRangeIfNeeded();
    }

    void set_user_range(int new_user_range) {
        user_range_ = new_user_range;
        updateRangeIfNeeded();
    }

    void set_auto_range_updates(bool enabled) {
        range_update_ = enabled;
        updateRangeIfNeeded();
    }

    void set_use_user_range(bool use) {
        user_ = use;
        updateRangeIfNeeded();
    }

    /** Disables automatic range updates and fixes the effective range. */
    void set_effective_range(int new_range) {
        range_ = new_range;
        range_update_ = false;
    }

    // ==================== STATIC METHODS ====================

    /**
     * @brief Adapts a set of vectors to the least common multiple of their moduli
     * 
     * @param vectors Vector of position_vector to adapt
     * @return New vector of position_vector with uniform moduli
     * 
     * @details Scales all vectors so they have the same modulus (the LCM of all moduli).
     *          All data values, userRange and range are scaled proportionally.
     * 
     * @note If all vectors already have the same modulus, returns an unchanged copy.
     * @note If the input vector is empty, returns an empty vector.
     */
    static std::vector<position_vector> adapt_to_lcm(const std::vector<position_vector>& vectors) {
        if (vectors.empty()) {
            return {};
        }

        std::set<int> unique_moduli;
        for (const auto& pv : vectors) {
            unique_moduli.insert(pv.mod_);
        }

        if (unique_moduli.size() == 1) {
            return vectors;
        }

        std::vector<int> moduli_list(unique_moduli.begin(), unique_moduli.end());
        const int lcm_val = lcm(moduli_list);

        std::vector<position_vector> adapted_vectors;
        adapted_vectors.reserve(vectors.size());

        for (const position_vector& pv : vectors) {
            const int scale_factor = lcm_val / pv.mod_;

            std::vector<int> scaled_data(pv.data_.size());
            for (size_t i = 0; i < pv.data_.size(); ++i) {
                scaled_data[i] = pv.data_[i] * scale_factor;
            }

            const int scaled_user_range = pv.user_range_ * scale_factor;
            const int scaled_range = pv.range_ * scale_factor;

            position_vector adapted_pv(scaled_data, lcm_val, scaled_user_range, false, pv.user_);
            adapted_pv.set_effective_range(scaled_range);
            adapted_vectors.emplace_back(adapted_pv);
        }

        return adapted_vectors;
    }

    // ==================== TRANSFORMATION METHODS ====================

    /**
     * @brief Rotates the vector elements
     * 
     * @param rotationAmount Rotation amount (positive or negative)
     * @return New position_vector with rotated elements
     * 
     * @details Rotation circularly shifts the elements.
     *          Values are adjusted based on cycles through the vector.
     * 
     * @note If the vector is empty, returns itself
     */
    position_vector rotate(int rotationAmount) const {
        if (data_.empty()) {
            return *this;
        }

        std::vector<int> rotatedData(data_.size());
        int absRotation = std::abs(rotationAmount);
        int size = static_cast<int>(data_.size());
        
        for (int i = 0; i < size; ++i) {
            int newPosition = (i + absRotation) % size;
            rotatedData[newPosition] = (*this)[i];
        }
        
        return position_vector(rotatedData, mod_, user_range_, range_update_, user_);
    }

    /**
     * @brief Relative mode: extracts a section of the vector with cyclic access
     * 
     * @param startOffset Starting offset (can be negative or > size)
     * @param length Length of the section to extract (0 = use current size)
     * @return New position_vector with the extracted section
     * 
     * @details Extracts 'length' elements starting from startOffset.
     *          Access is cyclic, so startOffset can be any value.
     */
    position_vector relative_mode(int start_offset, int length = 0) const {
        int outLength = (length == 0) ? static_cast<int>(data_.size()) : std::abs(length);
        
        std::vector<int> newData(outLength);
        for (int i = 0; i < outLength; i++) {
            newData[i] = (*this)[start_offset + i];
        }
        
        return position_vector(newData, mod_, user_range_, range_update_, user_);
    }

    /**
     * @brief Calculates the complement of the vector with respect to the range
     * 
     * @return New position_vector containing elements not present in the original
     * 
     * @details Normalizes the vector to its minimum value, identifies all values
     *          in the interval [0, range) that are not present, and denormalizes them.
     * 
     * @note If the vector is empty, returns the entire universe [0, range)
     */
    position_vector complement() const {
        int effectiveRange = effective_range();
        
        if (data_.empty()) {
            std::vector<int> universe(effectiveRange);
            for (int i = 0; i < effectiveRange; ++i) {
                universe[i] = i;
            }
            return position_vector(universe, mod_, user_range_, range_update_, user_);
        }

        int minValue = *std::min_element(data_.begin(), data_.end());
        position_vector normalized = *this - minValue;
        std::set<int> normalizedSet(normalized.data_.begin(), normalized.data_.end());
        
        std::vector<int> complementData;
        for (int i = 0; i < effectiveRange; ++i) {
            if (normalizedSet.find(i) == normalizedSet.end()) {
                complementData.emplace_back(i);
            }
        }

        return position_vector(complementData, mod_, user_range_, range_update_, user_) + minValue;
    }

    /**
     * @brief Inversion with respect to a specified axis
     * 
     * @param axisIndex Index of the element to use as axis (with wraparound)
     * @param sortOutput If true, sorts the result
     * @return New position_vector inverted with respect to the axis
     * 
     * @details Inverts all elements with respect to the value of the element at axisIndex.
     *          Formula: result = 2 * axis_value - original_value
     * 
     * @note If the vector is empty, returns itself
     */
    position_vector inversion(int axisIndex, bool sortOutput = true) const {
        if (data_.empty()) {
            return *this;
        }

        std::vector<int> invertedData(data_.size());
        int size = static_cast<int>(data_.size());
        
        // Normalize the axis index
        int normalizedAxisIndex = euclidean_division(axisIndex, size).remainder;
        int axisValue = data_[normalizedAxisIndex];
        
        // Apply inversion to all elements
        for (size_t i = 0; i < data_.size(); ++i) {
            invertedData[i] = 2 * axisValue - data_[i];
        }
        if(sortOutput) {
            std::sort(invertedData.begin(), invertedData.end());
        }
        return position_vector(invertedData, mod_, user_range_, range_update_, user_);
    }

    /**
     * @brief Calculates the musical negative of the vector
     * 
     * @param axis Reference point for negation (default 10)
     * @param standard If true, uses standard transformation (default true)
     * @param sortResult If true, sorts the result (default true)
     * @return New position_vector negated
     * 
     * @details Complex musical negation operation that:
     *          - In standard mode: multiplies by 2, negates with respect to (axis*2-1), divides by 2
     *          - In non-standard mode: negates directly with respect to axis
     *          - Applies relative_mode(-1) to the result
     */
    position_vector negative(int axis = 10, bool standard = true, bool sortResult = true) const {
        position_vector result = *this;
        int adjustedPosition = axis;
        
        // Step 1: Doubling (if requested)
        if (standard) {
            for (int& elem : result.data_) {
                elem *= 2;
            }
            adjustedPosition = (axis * 2) - 1;
        }
        
        // Step 2: Subtraction of the reference position
        for (int& elem : result.data_) {
            elem -= adjustedPosition;
        }
        
        // Step 3: Negation
        for (int& elem : result.data_) {
            elem *= -1;
        }
        
        // Step 4: Addition of the reference position
        for (int& elem : result.data_) {
            elem += adjustedPosition;
        }
        
        // Step 5: Division by 2 (if requested)
        if (standard) {
            for (int& elem : result.data_) {
                elem /= 2;
            }
        }
        
        // Step 6: Sorting (if requested)
        if (sortResult) {
            std::sort(result.data_.begin(), result.data_.end());
        }
        
        // Step 7: Final roto-translation
        result = result.relative_mode(-1);
        
        return result;
    }

    // ==================== COMPONENT-WISE OPERATIONS ====================

    /**
     * @brief Component-wise addition with a std::vector<int>
     * 
     * @param other Vector to add
     * @param useLooping If true, uses cyclic wraparound; if false, extends with non-added elements
     * @return New position_vector result of the addition
     * 
     * @details With useLooping=true: resulting length is max(size1, size2),
     *          and elements wrap cyclically.
     *          With useLooping=false: adds up to min(size1, size2),
     *          then appends remaining unmodified elements.
     */
    position_vector componentwiseSum(const std::vector<int>& other, bool useLooping = false) const {
        if (other.empty()) return *this;
        if (data_.empty()) return position_vector(other, mod_, user_range_, range_update_, user_);
        
        std::vector<int> result;
        
        if (useLooping) {
            size_t maxLength = std::max(data_.size(), other.size());
            result.reserve(maxLength);
            
            for (size_t i = 0; i < maxLength; ++i) {
                int component1 = data_[i % data_.size()];
                int component2 = other[i % other.size()];
                result.emplace_back(component1 + component2);
            }
        } else {
            size_t minLength = std::min(data_.size(), other.size());
            result.reserve(std::max(data_.size(), other.size()));
            
            for (size_t i = 0; i < minLength; ++i) {
                result.emplace_back(data_[i] + other[i]);
            }
            
            for (size_t i = minLength; i < data_.size(); ++i) {
                result.emplace_back(data_[i]);
            }
            
            for (size_t i = minLength; i < other.size(); ++i) {
                result.emplace_back(other[i]);
            }
        }
        
        return position_vector(result, mod_, user_range_, range_update_, user_);
    }

    /**
     * @brief Component-wise subtraction with a std::vector<int>
     * 
     * @param other Vector to subtract
     * @param useLooping If true, uses cyclic wraparound; if false, extends with non-subtracted elements
     * @return New position_vector result of the subtraction
     * 
     * @details Behavior analogous to componentwiseSum but with subtraction
     */
    position_vector componentwiseSubtraction(const std::vector<int>& other, bool useLooping = false) const {
        if (other.empty()) return *this;
        if (data_.empty()) return position_vector(other, mod_, user_range_, range_update_, user_);
        
        std::vector<int> result;
        
        if (useLooping) {
            size_t maxLength = std::max(data_.size(), other.size());
            result.reserve(maxLength);
            
            for (size_t i = 0; i < maxLength; ++i) {
                int component1 = data_[i % data_.size()];
                int component2 = other[i % other.size()];
                result.emplace_back(component1 - component2);
            }
        } else {
            size_t minLength = std::min(data_.size(), other.size());
            result.reserve(std::max(data_.size(), other.size()));
            
            for (size_t i = 0; i < minLength; ++i) {
                result.emplace_back(data_[i] - other[i]);
            }
            
            for (size_t i = minLength; i < data_.size(); ++i) {
                result.emplace_back(data_[i]);
            }
            
            for (size_t i = minLength; i < other.size(); ++i) {
                result.emplace_back(other[i]);
            }
        }
        
        return position_vector(result, mod_, user_range_, range_update_, user_);
    }

    /**
     * @brief Component-wise product with a std::vector<int>
     * 
     * @param other Vector to multiply
     * @param useLooping If true (default), uses cyclic wraparound
     * @return New position_vector result of the product
     * 
     * @note If other is empty, returns an empty vector.
     *       If data is empty, returns itself.
     */
    position_vector componentwiseProduct(const std::vector<int>& other, bool useLooping = true) const {
        if (other.empty()) return position_vector({}, mod_, user_range_, range_update_, user_);
        if (data_.empty()) return *this;
        
        std::vector<int> result;
        
        if (useLooping) {
            size_t maxLength = std::max(data_.size(), other.size());
            result.reserve(maxLength);
            
            for (size_t i = 0; i < maxLength; ++i) {
                int component1 = data_[i % data_.size()];
                int component2 = other[i % other.size()];
                result.emplace_back(component1 * component2);
            }
        } else {
            size_t minLength = std::min(data_.size(), other.size());
            result.reserve(std::max(data_.size(), other.size()));
            
            for (size_t i = 0; i < minLength; ++i) {
                result.emplace_back(data_[i] * other[i]);
            }
            
            for (size_t i = minLength; i < data_.size(); ++i) {
                result.emplace_back(data_[i]);
            }
            
            for (size_t i = minLength; i < other.size(); ++i) {
                result.emplace_back(other[i]);
            }
        }
        
        return position_vector(result, mod_, user_range_, range_update_, user_);
    }

    /**
     * @brief Euclidean component-wise division with a std::vector<int>
     * 
     * @param other Vector divisor
     * @param useLooping If true (default), uses cyclic wraparound
     * @return New position_vector with the quotients
     * @throw std::invalid_argument If other is empty or contains zeros
     * 
     * @note Uses Euclidean division for consistent results
     */
    position_vector componentwiseDivision(const std::vector<int>& other, bool useLooping = true) const {
        if (other.empty()) {
            throw std::invalid_argument("Cannot divide by empty vector");
        }
        if (data_.empty()) return *this;
        
        for (int val : other) {
            if (val == 0) {
                throw std::invalid_argument("Division by zero in componentwise division");
            }
        }
        
        std::vector<int> result;
        
        if (useLooping) {
            size_t maxLength = std::max(data_.size(), other.size());
            result.reserve(maxLength);
            
            for (size_t i = 0; i < maxLength; ++i) {
                int dividend = data_[i % data_.size()];
                int divisor = other[i % other.size()];
                division_result div = euclidean_division(dividend, divisor);
                result.emplace_back(div.quotient);
            }
        } else {
            size_t minLength = std::min(data_.size(), other.size());
            result.reserve(std::max(data_.size(), other.size()));
            
            for (size_t i = 0; i < minLength; ++i) {
                division_result div = euclidean_division(data_[i], other[i]);
                result.emplace_back(div.quotient);
            }
            
            for (size_t i = minLength; i < data_.size(); ++i) {
                result.emplace_back(data_[i]);
            }
            
            for (size_t i = minLength; i < other.size(); ++i) {
                result.emplace_back(other[i]);
            }
        }
        
        return position_vector(result, mod_, user_range_, range_update_, user_);
    }

    /**
     * @brief Euclidean component-wise modulo with a std::vector<int>
     * 
     * @param other Vector divisor
     * @param useLooping If true (default), uses cyclic wraparound
     * @return New position_vector with the remainders
     * @throw std::invalid_argument If other is empty or contains zeros
     * 
     * @note Uses Euclidean division for consistent results
     */
    position_vector componentwiseModulo(const std::vector<int>& other, bool useLooping = true) const {
        if (other.empty()) {
            throw std::invalid_argument("Cannot compute modulo with empty vector");
        }
        if (data_.empty()) return *this;
        
        for (int val : other) {
            if (val == 0) {
                throw std::invalid_argument("Division by zero in componentwise modulo");
            }
        }
        
        std::vector<int> result;
        
        if (useLooping) {
            size_t maxLength = std::max(data_.size(), other.size());
            result.reserve(maxLength);
            
            for (size_t i = 0; i < maxLength; ++i) {
                int dividend = data_[i % data_.size()];
                int divisor = other[i % other.size()];
                division_result div = euclidean_division(dividend, divisor);
                result.emplace_back(div.remainder);
            }
        } else {
            size_t minLength = std::min(data_.size(), other.size());
            result.reserve(std::max(data_.size(), other.size()));
            
            for (size_t i = 0; i < minLength; ++i) {
                division_result div = euclidean_division(data_[i], other[i]);
                result.emplace_back(div.remainder);
            }
            
            for (size_t i = minLength; i < data_.size(); ++i) {
                result.emplace_back(data_[i]);
            }
            
            for (size_t i = minLength; i < other.size(); ++i) {
                result.emplace_back(other[i]);
            }
        }
        
        return position_vector(result, mod_, user_range_, range_update_, user_);
    }

    // ==================== UTILITY METHODS ====================

    /**
     * @brief Alias for operator+(int)
     * @param scalar Value to add
     * @return New position_vector with the added values
     */
    position_vector scalarSum(int scalar) const {
        return *this + scalar;
    }

    /**
     * @brief Alias for operator*(int)
     * @param scalar Multiplication factor
     * @return New position_vector with the multiplied values
     */
    position_vector scalarProduct(int scalar) const {
        return *this * scalar;
    }

    /**
     * @brief Concatenates this vector with another
     * 
     * @param other position_vector to concatenate
     * @return New position_vector with all elements
     * 
     * @details The result contains first all elements of this vector,
     *          then all elements of other. Maintains the properties of this vector.
     */
    position_vector concatenate(const position_vector& other) const {
        std::vector<int> result = data_;
        result.insert(result.end(), other.data_.begin(), other.data_.end());
        return position_vector(result, mod_, user_range_, range_update_, user_);
    }

    /**
     * @brief Repeats the vector a specified number of times
     * 
     * @param times Number of repetitions
     * @return New position_vector with the repeated content
     * 
     * @note If times <= 0, returns an empty vector
     */
    position_vector repeat(int times) const {
        if (times <= 0) return position_vector({}, mod_, user_range_, range_update_, user_);
        
        std::vector<int> result;
        result.reserve(data_.size() * times);
        
        for (int t = 0; t < times; ++t) {
            for (size_t i = 0; i < data_.size(); ++i) {
                result.emplace_back(data_[i]);
            }
        }
        
        return position_vector(result, mod_, user_range_, range_update_, user_);
    }

    /**
     * @brief Resizes the vector to a specified range
     * 
     * @param start Starting index (inclusive)
     * @param end Ending index (inclusive)
     * @return New position_vector with the resized content
     * 
     * @details If start <= end: ascending range from start to end
     *          If start > end: descending range from start to end (backwards)
     * 
     * @note If the vector is empty, returns itself
     */
    position_vector resize(int start, int end) const {
        if (data_.empty()) {
            return *this;
        }
        
        std::vector<int> resizedData;
        
        if (start <= end) {
            // Ascending range: from start to end inclusive
            int length = end - start + 1;
            resizedData.reserve(length);
            
            for (int i = start; i <= end; ++i) {
                resizedData.emplace_back((*this)[i]);
            }
        } else {
            // Descending range: from start to end inclusive (backwards)
            int length = start - end + 1;
            resizedData.reserve(length);
            
            for (int i = start; i >= end; --i) {
                resizedData.emplace_back((*this)[i]);
            }
        }
        
        return position_vector(resizedData, mod_, user_range_, range_update_, user_);
    }

    // ==================== DEBUG/OUTPUT METHODS ====================

    /**
     * @brief Prints the vector data
     * 
     * @details Output: "Data: [elem1, elem2, ..., elemN]"
     */
    void print_data() const {
        std::cout << "Data: [";
        for (size_t i = 0; i < data_.size(); ++i) {
            std::cout << data_[i];
            if (i < data_.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }

    /**
     * @brief Prints range information
     * 
     * @details Shows range, rangeUpdate, userRange, user, and the effective range source
     */
    void print_range_info() const {
        std::cout << "Range: " << range_ << std::endl;
        std::cout << "Range Update: " << (range_update_ ? "ON" : "OFF") << std::endl;
        std::cout << "User Range: " << user_range_ << std::endl;
        std::cout << "Use User Range: " << (user_ ? "ON" : "OFF") << std::endl;
        std::cout << "Effective Range Source: " << (user_ ? "user_range" : "mod") << std::endl;
    }

    /**
     * @brief Prints the modulus
     * 
     * @details Output: "Mod: [value]"
     */
    void print_mod() const {
        std::cout << "Mod: " << mod_ << std::endl;
    }

    /**
     * @brief Prints all position_vector information
     * 
     * @details Prints data, mod, range information and size
     */
    void print_all() const {
        std::cout << "=== position_vector Info ===" << std::endl;
        print_data();
        print_mod();
        print_range_info();
        std::cout << "Size: " << data_.size() << std::endl;
    }
};

} // namespace musicpp

#endif // MUSICPP_POSITION_VECTOR_H