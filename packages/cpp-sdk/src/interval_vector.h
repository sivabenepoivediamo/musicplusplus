#ifndef MUSICPP_INTERVAL_VECTOR_H
#define MUSICPP_INTERVAL_VECTOR_H

#include "math_util.h"

namespace musicpp {


/**
 * @file interval_vector.h
 * @brief Definition of the interval_vector class for interval vectors
 * @author [not251]
 * @date 2025
 */

/**
 * @class interval_vector
 * @brief Class to represent a vector of intervals with cyclic access
 * 
 * @details An interval_vector is a container of intervals (integers) that supports:
 * - Cyclic access to elements
 * - Rotation and inversion
 * - Scalar and vector mathematical operations
 * - Offset for translations
 * - Modulo for cyclic operations
 * 
 * Intervals are typically used in musical contexts to represent
 * distances between notes, but the class is generic and usable in other contexts.
 */
class interval_vector {
private:
    std::vector<int> data_;
    int offset_;
    int mod_;

public:


    // ==================== CONSTRUCTORS ====================

    /**
     * @brief Default constructor
     * 
     * @details Creates an interval_vector with:
     *          - data = {0}
     *          - offset = 0
     *          - mod = 12
     */
    interval_vector() 
        : data_({0}), offset_(0), mod_(12) {}

    /**
     * @brief Parameterized constructor
     * 
     * @param in Initial interval vector
     * @param newOffset Initial offset, default 0
     * @param newMod Modulo, default 12
     */
    interval_vector(const std::vector<int>& in, int newOffset = 0, int newMod = 12)
        : data_(in), offset_(newOffset), mod_(newMod) {}

    // ==================== SCALAR OPERATORS ====================

    /**
     * @brief Adds a scalar to all elements
     * 
     * @param scalar Value to add
     * @return New interval_vector with summed values
     */
    interval_vector operator+(int scalar) const {
        std::vector<int> result(data_.size());
        for (size_t i = 0; i < data_.size(); ++i) {
            result[i] = data_[i] + scalar;
        }
        return interval_vector(result, offset_, mod_);
    }

    /**
     * @brief Subtracts a scalar from all elements
     * 
     * @param scalar Value to subtract
     * @return New interval_vector with subtracted values
     */
    interval_vector operator-(int scalar) const {
        std::vector<int> result(data_.size());
        for (size_t i = 0; i < data_.size(); ++i) {
            result[i] = data_[i] - scalar;
        }
        return interval_vector(result, offset_, mod_);
    }

    /**
     * @brief Multiplies all elements by a scalar
     * 
     * @param scalar Multiplication factor
     * @return New interval_vector with multiplied values
     */
    interval_vector operator*(int scalar) const {
        std::vector<int> result(data_.size());
        for (size_t i = 0; i < data_.size(); ++i) {
            result[i] = data_[i] * scalar;
        }
        return interval_vector(result, offset_, mod_);
    }

    /**
     * @brief Divides all elements by a scalar (Euclidean division)
     * 
     * @param divisor Divisor
     * @return New interval_vector with quotients
     * @throw std::invalid_argument If divisor is 0
     */
    interval_vector operator/(int divisor) const {
        if (divisor == 0) {
            throw std::invalid_argument("Division by zero");
        }
        
        std::vector<int> result(data_.size());
        for (size_t i = 0; i < data_.size(); ++i) {
            division_result div = euclidean_division(data_[i], divisor);
            result[i] = div.quotient;
        }
        return interval_vector(result, offset_, mod_);
    }

    /**
     * @brief Calculates the remainder of Euclidean division for all elements
     * 
     * @param divisor Divisor
     * @return New interval_vector with remainders
     * @throw std::invalid_argument If divisor is 0
     */
    interval_vector operator%(int divisor) const {
        if (divisor == 0) {
            throw std::invalid_argument("Division by zero");
        }
        
        std::vector<int> result(data_.size());
        for (size_t i = 0; i < data_.size(); ++i) {
            division_result div = euclidean_division(data_[i], divisor);
            result[i] = div.remainder;
        }
        return interval_vector(result, offset_, mod_);
    }

    // ==================== VECTOR OPERATORS ====================

    /**
     * @brief Component-wise addition with another interval_vector
     * 
     * @param other interval_vector to add
     * @return New interval_vector result of the addition
     * 
     * @note Uses componentwiseSum without looping
     */
    interval_vector operator+(const interval_vector& other) const {
        return componentwiseSum(other.data_, false);
    }

    /**
     * @brief Component-wise subtraction with another interval_vector
     * 
     * @param other interval_vector to subtract
     * @return New interval_vector result of the subtraction
     * 
     * @note Uses componentwiseSubtraction without looping
     */
    interval_vector operator-(const interval_vector& other) const {
        return componentwiseSubtraction(other.data_, false);
    }

    /**
     * @brief Component-wise product with another interval_vector
     * 
     * @param other interval_vector to multiply
     * @return New interval_vector result of the product
     * 
     * @note Uses componentwiseProduct with looping
     */
    interval_vector operator*(const interval_vector& other) const {
        return componentwiseProduct(other.data_, true);
    }

    /**
     * @brief Component-wise division with another interval_vector
     * 
     * @param other interval_vector divisor
     * @return New interval_vector with quotients
     * @throw std::invalid_argument If other contains zeros
     * 
     * @note Uses componentwiseDivision with looping and Euclidean division
     */
    interval_vector operator/(const interval_vector& other) const {
        return componentwiseDivision(other.data_, true);
    }

    /**
     * @brief Component-wise modulo with another interval_vector
     * 
     * @param other interval_vector divisor
     * @return New interval_vector with remainders
     * @throw std::invalid_argument If other contains zeros
     * 
     * @note Uses componentwiseModulo with looping and Euclidean division
     */
    interval_vector operator%(const interval_vector& other) const {
        return componentwiseModulo(other.data_, true);
    }

    // ==================== OPERATORS WITH VECTOR<INT> ====================

    /**
     * @brief Component-wise addition with a std::vector<int>
     * 
     * @param other Vector to add
     * @return New interval_vector result of the addition
     */
    interval_vector operator+(const std::vector<int>& other) const {
        return componentwiseSum(other, false);
    }

    /**
     * @brief Component-wise subtraction with a std::vector<int>
     * 
     * @param other Vector to subtract
     * @return New interval_vector result of the subtraction
     */
    interval_vector operator-(const std::vector<int>& other) const {
        return componentwiseSubtraction(other, false);
    }

    /**
     * @brief Component-wise product with a std::vector<int>
     * 
     * @param other Vector to multiply
     * @return New interval_vector result of the product
     */
    interval_vector operator*(const std::vector<int>& other) const {
        return componentwiseProduct(other, true);
    }

    /**
     * @brief Component-wise division with a std::vector<int>
     * 
     * @param other Vector divisor
     * @return New interval_vector with quotients
     * @throw std::invalid_argument If other contains zeros
     */
    interval_vector operator/(const std::vector<int>& other) const {
        return componentwiseDivision(other, true);
    }

    /**
     * @brief Component-wise modulo with a std::vector<int>
     * 
     * @param other Vector divisor
     * @return New interval_vector with remainders
     * @throw std::invalid_argument If other contains zeros
     */
    interval_vector operator%(const std::vector<int>& other) const {
        return componentwiseModulo(other, true);
    }

    // ==================== COMPOUND ASSIGNMENT OPERATORS ====================

    /**
     * @brief Add and assign a scalar
     * 
     * @param scalar Value to add
     * @return Reference to this modified object
     */
    interval_vector& operator+=(int scalar) {
        for (int& elem : data_) {
            elem += scalar;
        }
        return *this;
    }

    /**
     * @brief Subtract and assign a scalar
     * 
     * @param scalar Value to subtract
     * @return Reference to this modified object
     */
    interval_vector& operator-=(int scalar) {
        for (int& elem : data_) {
            elem -= scalar;
        }
        return *this;
    }

    /**
     * @brief Multiply and assign a scalar
     * 
     * @param scalar Multiplication factor
     * @return Reference to this modified object
     */
    interval_vector& operator*=(int scalar) {
        for (int& elem : data_) {
            elem *= scalar;
        }
        return *this;
    }

    /**
     * @brief Divide and assign a scalar
     * 
     * @param divisor Divisor
     * @return Reference to this modified object
     * @throw std::invalid_argument If divisor is 0
     */
    interval_vector& operator/=(int divisor) {
        if (divisor == 0) {
            throw std::invalid_argument("Division by zero");
        }
        
        for (int& elem : data_) {
            division_result div = euclidean_division(elem, divisor);
            elem = div.quotient;
        }
        return *this;
    }

    /**
     * @brief Calculate modulo and assign
     * 
     * @param divisor Divisor
     * @return Reference to this modified object
     * @throw std::invalid_argument If divisor is 0
     */
    interval_vector& operator%=(int divisor) {
        if (divisor == 0) {
            throw std::invalid_argument("Division by zero");
        }
        
        for (int& elem : data_) {
            division_result div = euclidean_division(elem, divisor);
            elem = div.remainder;
        }
        return *this;
    }

    /**
     * @brief Add and assign another interval_vector
     * 
     * @param other interval_vector to add
     * @return Reference to this modified object
     */
    interval_vector& operator+=(const interval_vector& other) {
        *this = *this + other;
        return *this;
    }

    /**
     * @brief Subtract and assign another interval_vector
     * 
     * @param other interval_vector to subtract
     * @return Reference to this modified object
     */
    interval_vector& operator-=(const interval_vector& other) {
        *this = *this - other;
        return *this;
    }

    /**
     * @brief Multiply and assign another interval_vector
     * 
     * @param other interval_vector to multiply
     * @return Reference to this modified object
     */
    interval_vector& operator*=(const interval_vector& other) {
        *this = *this * other;
        return *this;
    }

    /**
     * @brief Divide and assign another interval_vector
     * 
     * @param other interval_vector divisor
     * @return Reference to this modified object
     * @throw std::invalid_argument If other contains zeros
     */
    interval_vector& operator/=(const interval_vector& other) {
        *this = *this / other;
        return *this;
    }

    /**
     * @brief Calculate modulo and assign with another interval_vector
     * 
     * @param other interval_vector divisor
     * @return Reference to this modified object
     * @throw std::invalid_argument If other contains zeros
     */
    interval_vector& operator%=(const interval_vector& other) {
        *this = *this % other;
        return *this;
    }

    /**
     * @brief Add and assign a std::vector<int>
     * 
     * @param other Vector to add
     * @return Reference to this modified object
     */
    interval_vector& operator+=(const std::vector<int>& other) {
        *this = *this + other;
        return *this;
    }

    /**
     * @brief Subtract and assign a std::vector<int>
     * 
     * @param other Vector to subtract
     * @return Reference to this modified object
     */
    interval_vector& operator-=(const std::vector<int>& other) {
        *this = *this - other;
        return *this;
    }

    /**
     * @brief Multiply and assign a std::vector<int>
     * 
     * @param other Vector to multiply
     * @return Reference to this modified object
     */
    interval_vector& operator*=(const std::vector<int>& other) {
        *this = *this * other;
        return *this;
    }

    /**
     * @brief Divide and assign a std::vector<int>
     * 
     * @param other Vector divisor
     * @return Reference to this modified object
     * @throw std::invalid_argument If other contains zeros
     */
    interval_vector& operator/=(const std::vector<int>& other) {
        *this = *this / other;
        return *this;
    }

    /**
     * @brief Calculate modulo and assign with a std::vector<int>
     * 
     * @param other Vector divisor
     * @return Reference to this modified object
     * @throw std::invalid_argument If other contains zeros
     */
    interval_vector& operator%=(const std::vector<int>& other) {
        *this = *this % other;
        return *this;
    }

    // ==================== ACCESS AND COMPARISON OPERATORS ====================

    /**
     * @brief Cyclic access to elements
     * 
     * @param index Index (can be negative or greater than size)
     * @return Value at specified index with cyclic wraparound
     */
    int operator[](int index) const {
        return element(index);
    }

    /**
     * @brief Equality operator
     * 
     * @param other interval_vector to compare
     * @return true if vectors are equal, false otherwise
     */
    bool operator==(const interval_vector& other) const {
        return data_ == other.data_ && offset_ == other.offset_ && mod_ == other.mod_;
    }

    /**
     * @brief Inequality operator
     * 
     * @param other interval_vector to compare
     * @return true if vectors are different, false otherwise
     */
    bool operator!=(const interval_vector& other) const {
        return !(*this == other);
    }

    // ==================== FRIEND OPERATORS ====================

    /**
     * @brief Scalar-vector addition (friend)
     * 
     * @param scalar Scalar on the left
     * @param iv interval_vector on the right
     * @return New interval_vector result of the addition
     */
    friend interval_vector operator+(int scalar, const interval_vector& iv) {
        return iv + scalar;
    }

    /**
     * @brief Scalar-vector subtraction (friend)
     * 
     * @param scalar Scalar on the left (minuend)
     * @param iv interval_vector on the right (subtrahend)
     * @return New interval_vector with scalar - elements
     */
    friend interval_vector operator-(int scalar, const interval_vector& iv) {
        return iv - scalar;
    }

    /**
     * @brief Scalar-vector multiplication (friend)
     * 
     * @param scalar Scalar on the left
     * @param iv interval_vector on the right
     * @return New interval_vector result of the multiplication
     */
    friend interval_vector operator*(int scalar, const interval_vector& iv) {
        return iv * scalar;
    }

    /**
     * @brief Output stream operator
     * 
     * @param os Output stream
     * @param iv interval_vector to print
     * @return Reference to the stream
     * 
     * @details Format: [elem1, elem2, ..., elemN]
     */
    friend std::ostream& operator<<(std::ostream& os, const interval_vector& iv) {
    os << "[";
    for (size_t i = 0; i < iv.data_.size(); ++i) {
        os << iv.data_[i];
        if (i < iv.data_.size() - 1) os << ", ";
    }
    os << "] (offset: " << iv.offset_ << ")";
    return os;
}

    // ==================== MAIN METHODS ====================

    /**
     * @brief Cyclic access to elements
     * 
     * @param i Access index (can be any integer)
     * @return Value at index with cyclic behavior
     * 
     * @note If the vector is empty, returns 0
     */
    int element(int i) const {
        if (data_.empty()) {
            return 0;
        }
        
        int size = static_cast<int>(data_.size());
        division_result div = euclidean_division(i, size);
        return data_[div.remainder];
    }

    /**
     * @brief Rotates the vector elements
     * 
     * @param r Rotation amount
     * @param n Result length (0 = use current size)
     * @return New interval_vector with rotated elements
     * 
     * @details Extracts n elements starting from index r with cyclic access
     */
    interval_vector rotate(int r, int n = 0) const {
        n = std::abs(n);
        if (n == 0) n = static_cast<int>(data_.size());
        
        std::vector<int> out(n);
        for (int i = 0; i < n; i++) {
            out[i] = element(r + i);
        }
        return interval_vector(out, offset_, mod_);
    }

    /**
     * @brief Rototranslation: rotation combined with offset adjustment
     * 
     * @param r Rotation amount
     * @param n Result length (0 = use current size)
     * @return New interval_vector with rotated elements and adjusted offset
     * 
     * @details Extracts n elements starting from index r with cyclic access.
     *          Adjusts the offset by summing the intervals that are skipped
     *          during the rotation. 
     */ 

interval_vector roto_translate(int r, int n = 0) const {
    n = std::abs(n);
    int dataSize = static_cast<int>(data_.size());
    if (n == 0) n = dataSize;
    
    std::vector<int> out(n);
    for (int i = 0; i < n; i++) {
        out[i] = element(r + i);
    }
    
    int sum = 0;
    
    if (std::abs(r) < dataSize) {

        if (r >= 0) {
            for (int i = 0; i < r; i++) {
                sum += element(i);
            }
        } else {
            for (int i = 0; i < -r; i++) {
                sum -= element(dataSize - 1 - i);
            }
        }
    } else {

        division_result div = euclidean_division(r, dataSize);
        
        if (r >= 0) {
            for (int i = 0; i < dataSize; i++) {
                int mult = (i < div.remainder) ? (div.quotient + 1) : div.quotient;
                sum += element(i) * mult;
            }
        } else {
            int thresh = dataSize + div.remainder; 
            for (int i = 0; i < dataSize; i++) {
                int mult = (i >= thresh) ? (div.quotient - 1) : div.quotient;
                sum += element(i) * mult;
            }
        }
    }
    
    return interval_vector(out, offset_ + sum, mod_);
}

    /**
     * @brief Reverses the order of elements (retrograde)
     * 
     * @return New interval_vector with elements in reverse order
     */
    interval_vector reverse() const {
        std::vector<int> out(data_.size());
        for (size_t i = 0; i < data_.size(); i++) {
            out[i] = data_[(data_.size() - 1) - i];
        }
        return interval_vector(out, offset_, mod_);
    }

    /**
     * @brief Retrograde (alias for reverse)
     * 
     * @return New interval_vector with elements in reverse order
     */
    interval_vector retrograde() const {
        return reverse();
    }

    /**
     * @brief Inversion: reflects the sequence around an axis
     * 
     * @param axisIndex Position of the reflection axis (0 = start, size = end)
     * @return New interval_vector with elements reflected around the axis
     * 
     * @details Reflects the order of elements around a position.
     *          Elements before the axis are reversed in order,
     *          and elements after the axis are also reversed in order.
     *          
     *          Example with [2, 2, 1]:
     *          - axisIndex=0: [2, 2, 1] (no elements before axis)
     *          - axisIndex=1: [2, 2, 1] (1 element before, reversed with itself)
     *          - axisIndex=2: [1, 2, 2] (2 elements before are swapped)
     */
    interval_vector inversion(int axisIndex = 0) const {
        if (data_.empty()) {
            return *this;
        }

        int size = static_cast<int>(data_.size());
        division_result div = euclidean_division(axisIndex, size + 1);
        int normalizedAxis = div.remainder;
        
        std::vector<int> result = data_;
        
        // Reverse elements before the axis
        for (int i = 0; i < normalizedAxis / 2; ++i) {
            std::swap(result[i], result[normalizedAxis - 1 - i]);
        }
        
        // Reverse elements after the axis
        int start = normalizedAxis;
        int end = size;
        int len = end - start;
        for (int i = 0; i < len / 2; ++i) {
            std::swap(result[start + i], result[end - 1 - i]);
        }
        
        return interval_vector(result, offset_, mod_);
    }

    /**
     * @brief Negation of intervals
     * 
     * @return New interval_vector with all intervals negated
     */
    interval_vector negate() const {
        std::vector<int> result(data_.size());
        for (size_t i = 0; i < data_.size(); ++i) {
            result[i] = -data_[i];
        }
        return interval_vector(result, -offset_, mod_);
    }

    /**
     * @brief Normalizes intervals with respect to a modulo
     * 
     * @param modulo Modulo for normalization (if 0, uses mod)
     * @return New normalized interval_vector
     */
    interval_vector normalize(int modulo = 0) const {
        if (modulo == 0) modulo = mod_;
        if (modulo == 0) return *this;
        
        std::vector<int> result(data_.size());
        for (size_t i = 0; i < data_.size(); ++i) {
            division_result div = euclidean_division(data_[i], modulo);
            result[i] = div.remainder;
        }
        return interval_vector(result, offset_, mod_);
    }

    const std::vector<int>& data() const { return data_; }
    std::vector<int>& data() { return data_; }

    int offset() const { return offset_; }

    int mod() const { return mod_; }

    size_t size() const { return data_.size(); }

    bool empty() const { return data_.empty(); }

    void set_offset(int new_offset) { offset_ = new_offset; }

    void set_mod(int new_mod) { mod_ = new_mod; }

    void set_data(const std::vector<int>& new_data) { data_ = new_data; }

    // ==================== COMPONENT-WISE OPERATIONS ====================

    /**
     * @brief Component-wise addition with a std::vector<int>
     * 
     * @param other Vector to add
     * @param useLooping If true, uses cyclic wraparound; if false, extends with non-added elements
     * @return New interval_vector result of the addition
     * 
     * @details With useLooping=true: resulting length is std::max(size1, size2),
     *          and elements wrap cyclically.
     *          With useLooping=false: adds up to std::min(size1, size2),
     *          then appends remaining unmodified elements.
     */
    interval_vector componentwiseSum(const std::vector<int>& other, bool useLooping = false) const {
        if (other.empty()) return *this;
        if (data_.empty()) return interval_vector(other, offset_, mod_);
        
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
        
        return interval_vector(result, offset_, mod_);
    }

    /**
     * @brief Component-wise subtraction with a std::vector<int>
     * 
     * @param other Vector to subtract
     * @param useLooping If true, uses cyclic wraparound; if false, extends with non-subtracted elements
     * @return New interval_vector result of the subtraction
     */
    interval_vector componentwiseSubtraction(const std::vector<int>& other, bool useLooping = false) const {
        if (other.empty()) return *this;
        if (data_.empty()) return interval_vector(other, offset_, mod_);
        
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
        
        return interval_vector(result, offset_, mod_);
    }

    /**
     * @brief Component-wise product with a std::vector<int>
     * 
     * @param other Vector to multiply
     * @param useLooping If true (default), uses cyclic wraparound
     * @return New interval_vector result of the product
     */
    interval_vector componentwiseProduct(const std::vector<int>& other, bool useLooping = true) const {
        if (other.empty()) return interval_vector({}, offset_, mod_);
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
        
        return interval_vector(result, offset_, mod_);
    }

    /**
     * @brief Euclidean component-wise division with a std::vector<int>
     * 
     * @param other Vector divisor
     * @param useLooping If true (default), uses cyclic wraparound
     * @return New interval_vector with quotients
     * @throw std::invalid_argument If other is empty or contains zeros
     */
    interval_vector componentwiseDivision(const std::vector<int>& other, bool useLooping = true) const {
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
        
        return interval_vector(result, offset_, mod_);
    }

    /**
     * @brief Euclidean component-wise modulo with a std::vector<int>
     * 
     * @param other Vector divisor
     * @param useLooping If true (default), uses cyclic wraparound
     * @return New interval_vector with remainders
     * @throw std::invalid_argument If other is empty or contains zeros
     */
    interval_vector componentwiseModulo(const std::vector<int>& other, bool useLooping = true) const {
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
        
        return interval_vector(result, offset_, mod_);
    }

    // ==================== STATIC METHODS ====================

    /**
     * @brief Adapts a set of vectors to the least common multiple of their modulos
     * 
     * @param vectors Vector of interval_vectors to adapt
     * @return New vector of interval_vectors with uniform modulos
     * 
     * @details Scales all vectors so they have the same modulo (the LCM of all modulos).
     *          All data values and offsets are scaled proportionally.
     * 
     * @note If all vectors already have the same modulo, returns an unmodified copy.
     * @note If the input vector is empty, returns an empty vector.
     * @note Vectors with mod = 0 are ignored in the LCM calculation
     */
    static std::vector<interval_vector> adapt_to_lcm(const std::vector<interval_vector>& vectors) {
        if (vectors.empty()) {
            return std::vector<interval_vector>();
        }
        
        // Collect all unique modulos (exclude 0)
        std::set<int> uniqueModulos;
        for (const auto& iv : vectors) {
            if (iv.mod_ != 0) {
                uniqueModulos.insert(iv.mod_);
            }
        }
        
        // If no valid modulos or only one modulo, return copy
        if (uniqueModulos.empty() || uniqueModulos.size() == 1) {
            return vectors;
        }
        
        // Calculate LCM
        std::vector<int> modulosList(uniqueModulos.begin(), uniqueModulos.end());
        const int lcm_val = lcm(modulosList);
        
        // Adapt each vector
        std::vector<interval_vector> adaptedVectors;
        adaptedVectors.reserve(vectors.size());
        
        for (const interval_vector& iv : vectors) {
            if (iv.mod_ == 0) {
                // If mod is 0, keep the vector unchanged
                adaptedVectors.emplace_back(iv);
                continue;
            }
            
            int scaleFactor = lcm_val / iv.mod_;
            
            // Scale the data
            std::vector<int> scaledData(iv.data_.size());
            for (size_t i = 0; i < iv.data_.size(); ++i) {
                scaledData[i] = iv.data_[i] * scaleFactor;
            }
            
            // Scale the offset
            int scaledOffset = iv.offset_ * scaleFactor;
            
            adaptedVectors.emplace_back(interval_vector(scaledData, scaledOffset, lcm_val));
        }
        
        return adaptedVectors;
    }

    // ==================== UTILITY METHODS ====================

    /**
     * @brief Concatenates this vector with another
     * 
     * @param other interval_vector to concatenate
     * @return New interval_vector with all elements
     */
    interval_vector concatenate(const interval_vector& other) const {
        std::vector<int> result = data_;
        result.insert(result.end(), other.data_.begin(), other.data_.end());
        return interval_vector(result, offset_, mod_);
    }

    /**
     * @brief Repeats the vector a specified number of times
     * 
     * @param times Number of repetitions
     * @return New interval_vector with repeated content
     */
    interval_vector repeat(int times) const {
        if (times <= 0) return interval_vector({}, offset_, mod_);
        
        std::vector<int> result;
        result.reserve(data_.size() * times);
        
        for (int t = 0; t < times; ++t) {
            result.insert(result.end(), data_.begin(), data_.end());
        }
        return interval_vector(result, offset_, mod_);
    }

    // ==================== MIRRORING METHODS ====================

    /**
     * @brief Individually reflects elements to the left or right of a position
     * 
     * @param position Position around which reflection occurs
     * @param left If true, reflects elements to the left; if false, to the right
     * @return New interval_vector with reflected elements
     * 
     * @details If left=true: reflects elements up to position inward.
     *          If left=false: reflects elements from position to end outward.
     *          Creates a reflected pattern around a position in one direction.
     * 
     * @note If position is out of range [0, size], returns an unmodified copy
     */
    interval_vector singleMirror(int position, bool left) const {
        std::vector<int> out = data_;
        int length = static_cast<int>(out.size());

        if (position < 0 || position > length) {
            return *this;
        }

        if (left) {
            for (int i = 0; i < position / 2; i++) {
                std::swap(out[i], out[position - 1 - i]);
            }
        } else {
            int end = position + (length - position) / 2;
            for (int i = position; i < end; i++) {
                std::swap(out[i], out[length - 1 - (i - position)]);
            }
        }

        return interval_vector(out, offset_, mod_);
    }

    /**
     * @brief Reflects in two directions around a central position
     * 
     * @param position Central position around which reflections occur
     * @return New interval_vector with symmetrically reflected elements
     * 
     * @details First reflects elements up to position inward.
     *          Then reflects elements after position outward.
     *          Creates a symmetric pattern around the position in both directions.
     * 
     * @note If position is out of range [0, size], returns an unmodified copy
     */
    interval_vector doubleMirror(int position) const {
        std::vector<int> out = data_;
        int length = static_cast<int>(out.size());

        if (position < 0 || position > length) {
            return *this;
        }

        // Mirror the left part (up to position)
        for (int i = 0; i < position / 2; i++) {
            std::swap(out[i], out[position - 1 - i]);
        }

        // Mirror the right part (from position to end)
        int end = position + (length - position) / 2;
        for (int i = position; i < end; i++) {
            std::swap(out[i], out[length - 1 - (i - position)]);
        }

        return interval_vector(out, offset_, mod_);
    }

    /**
     * @brief Reflects elements from the opposite side of a position
     * 
     * @param position Central position around which reflection occurs
     * @param left If true, reflects elements from left to right; if false, from right to left
     * @return New interval_vector with elements reflected from the opposite side
     * 
     * @details If left=true: reflects elements from the left section (up to pos) toward the right end.
     *          If left=false: reflects elements from the right section (from pos onward) toward the start.
     *          Creates a reflection across the position from one side to the other.
     * 
     * @note If position is out of range, returns an unmodified copy
     */
    interval_vector crossMirror(int position, bool left) const {
        std::vector<int> out = data_;
        int n = static_cast<int>(data_.size());

        if (left) {
            for (int i = 0; i < position && i < n; i++) {
                out[n - 1 - i] = data_[i];
            }
        } else {
            for (int i = position; i < n; i++) {
                out[i - position] = data_[n - 1 - (i - position)];
            }
        }

        return interval_vector(out, offset_, mod_);
    }


    // ==================== DEBUG/OUTPUT METHODS ====================

    /**
     * @brief Prints the vector data
     * 
     * @details Output: "Data: [elem1, elem2, ..., elemN]"
     */
    void printData() const {
        std::cout << "Data: [";
        for (size_t i = 0; i < data_.size(); ++i) {
            std::cout << data_[i];
            if (i < data_.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }

    /**
     * @brief Prints the offset
     * 
     * @details Output: "Offset: [value]"
     */
    void printOffset() const {
        std::cout << "Offset: " << offset_ << std::endl;
    }

    /**
     * @brief Prints the modulo
     * 
     * @details Output: "Mod: [value]"
     */
    void printMod() const {
        std::cout << "Mod: " << mod_ << std::endl;
    }

    /**
     * @brief Prints all interval_vector information
     * 
     * @details Prints data, offset, modulo, and size
     */
    void printAll() const {
        std::cout << "=== interval_vector Info ===" << std::endl;
        printData();
        printOffset();
        printMod();
        std::cout << "Size: " << data_.size() << std::endl;
    }
};

} // namespace musicpp

#endif // MUSICPP_INTERVAL_VECTOR_H
