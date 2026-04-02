#ifndef MUSICPP_ONSET_VECTOR_H
#define MUSICPP_ONSET_VECTOR_H

#include "math_util.h"

namespace musicpp {


/**
 * @file onset_vector.h
 * @brief Definition of onset_vector for cyclic onset (0/1) rhythm patterns
 * @author [not251]
 * @date 2025
 */

/**
 * @class onset_vector
 * @brief Onset vector: 0/1 pattern over a cyclic grid (rhythmic onsets and rests)
 * 
 * @details An onset_vector encodes where onsets (1) and rests (0) fall on each step of a cycle.
 * It supports:
 * - Offset-based transposition
 * - Scaling through multiplication and division (stretching/warping)
 * - Cyclic rotation
 * - Complement and inversion operations
 * - Logical operations (OR, AND, XOR, NOR, NAND, XNOR)
 * - Automatic modulo adaptation via LCM
 */
class onset_vector {
private:
    std::vector<int> data_;
    int offset_;
    int mod_;

public:

    /**
     * @brief Validates that data contains only 0s and 1s
     * @throw std::invalid_argument if data contains invalid values
     */
    void validate_onset_pattern() const {
        for (int val : data_) {
            if (val != 0 && val != 1) {
                throw std::invalid_argument("onset_vector pattern must contain only 0s and 1s");
            }
        }
    }


    /**
     * @brief Default constructor
     */
    onset_vector() 
        : data_({1, 0, 0, 0}), 
          offset_(0),
          mod_(4)
    {}

    /**
     * @brief Parameterized constructor
     * @param data Binary vector data
     * @param offset Initial offset value
     * @param mod Modulo base (default 4)
     * @throw std::invalid_argument if data contains values other than 0 or 1
     */
    onset_vector(const std::vector<int>& values,
                 int offset_in = 0,
                 int mod_in = 4)
        : data_(values), 
          offset_(offset_in),
          mod_(mod_in)
    {
        validate_onset_pattern();
    }

    // ==================== GETTERS ====================

    const std::vector<int>& data() const { return data_; }
    std::vector<int>& data() { return data_; }
    int offset() const { return offset_; }
    int mod() const { return mod_; }
    size_t size() const { return data_.size(); }

    void set_offset(int new_offset) { offset_ = new_offset; }
    void set_mod(int new_mod) { mod_ = new_mod; }

    // ==================== SCALAR OPERATIONS ====================

    /**
     * @brief Multiply (space out) the pattern by a scalar
     * @param scalar Spacing factor
     * @return New onset_vector with elements spaced by zeros
     * @details Elements are spaced out by inserting (scalar-1) zeros between each element
     */
    onset_vector operator*(int scalar) const {
    if (scalar <= 0) {
        throw std::invalid_argument("scalar must be positive");
    }

    int n = static_cast<int>(data_.size());
    std::vector<int> out(scalar * n, 0);

    for (int i = 0; i < scalar * n; i++) {
        if ((i + 1) % scalar == 1) {
            int index = i / scalar;
            out[i] = data_[index];
        }
    }

    return onset_vector(out, offset_, mod_);
}


 /**
 * @brief Divide (compress spacing) the pattern by a scalar
 * @param divisor Compression factor
 * @return New onset_vector with spacing between pulses compressed
 * @details Compresses the spaces between 1s by removing zeros proportionally.
 * For each gap between pulses, keeps only 1/divisor of the zeros (rounded down).
 * This is the inverse operation of multiplication.
 */
onset_vector operator/(int scalar) const {
    int n = static_cast<int>(data_.size());

    if (scalar <= 0) {
        throw std::invalid_argument("k must be positive");
    }

    if (scalar > n) {
        throw std::invalid_argument("k must be less than or equal to vector size");
    }

    if (n % scalar != 0) {
        throw std::invalid_argument("Vector size must be divisible by k");
    }

    std::vector<int> out(n / scalar);
    for (int i = 0; i < n / scalar; i++) {
        int index = i * scalar;
        out[i] = data_[index];
    }
    return onset_vector(out, offset_, mod_);
}

    onset_vector& operator*=(int scalar) {
        *this = *this * scalar;
        return *this;
    }

    onset_vector& operator/=(int divisor) {
        *this = *this / divisor;
        return *this;
    }
// old, deprecated
onset_vector divide(int divisor) const {
    if (divisor <= 0) {
        throw std::invalid_argument("Divisor must be positive");
    }
    
    if (divisor == 1) {
        return *this;
    }
    
    std::vector<int> compressed;
    int consecutiveZeros = 0;
    
    for (size_t i = 0; i < data_.size(); ++i) {
        if (data_[i] == 1) {
            // Output compressed zeros before this pulse
            int compressedZeroCount = consecutiveZeros / divisor;
            for (int j = 0; j < compressedZeroCount; ++j) {
                compressed.emplace_back(0);
            }
            // Output the pulse
            compressed.emplace_back(1);
            consecutiveZeros = 0;
        } else {
            consecutiveZeros++;
        }
    }
    
    // Handle trailing zeros
    int compressedTrailingZeros = consecutiveZeros / divisor;
    for (int j = 0; j < compressedTrailingZeros; ++j) {
        compressed.emplace_back(0);
    }
    
    // Pad with zeros to maintain original length
    while (compressed.size() < data_.size()) {
        compressed.emplace_back(0);
    }
    
    // Truncate if somehow longer (shouldn't happen, but safety check)
    if (compressed.size() > data_.size()) {
        compressed.resize(data_.size());
    }
    
    return onset_vector(compressed, offset_, mod_);
}


    // deprecated
    onset_vector stretch(int scalar) const {
        if (scalar <= 0) {
            throw std::invalid_argument("Scalar must be positive for multiplication");
        }

        std::vector<int> result;
        result.reserve(data_.size() * scalar);

        for (int val : data_) {
            result.emplace_back(val);
            for (int i = 1; i < scalar; ++i) {
                result.emplace_back(0);
            }
        }

        return onset_vector(result, offset_, mod_ * scalar);
    }
    
    // ==================== COMPONENTWISE LOGICAL OPERATIONS ====================

    /**
     * @brief Componentwise OR with optional looping
     * @param other Vector to OR with
     * @param useLooping If true, use cyclic wraparound; if false, extend with unprocessed elements
     * @return New onset_vector with OR operation applied
     */
    onset_vector componentwiseOr(const std::vector<int>& other, bool useLooping = false) const {
        if (other.empty()) return *this;
        if (data_.empty()) return onset_vector(other, offset_, mod_);
        
        std::vector<int> result;
        
        if (useLooping) {
            size_t maxLength = std::max(data_.size(), other.size());
            result.reserve(maxLength);
            
            for (size_t i = 0; i < maxLength; ++i) {
                int val1 = data_[i % data_.size()];
                int val2 = other[i % other.size()];
                result.emplace_back(val1 | val2);
            }
        } else {
            size_t minLength = std::min(data_.size(), other.size());
            result.reserve(std::max(data_.size(), other.size()));
            
            for (size_t i = 0; i < minLength; ++i) {
                result.emplace_back(data_[i] | other[i]);
            }
            
            for (size_t i = minLength; i < data_.size(); ++i) {
                result.emplace_back(data_[i]);
            }
            
            for (size_t i = minLength; i < other.size(); ++i) {
                result.emplace_back(other[i]);
            }
        }
        
        return onset_vector(result, offset_, mod_);
    }

    /**
     * @brief Componentwise AND with optional looping
     * @param other Vector to AND with
     * @param useLooping If true, use cyclic wraparound; if false, extend with unprocessed elements
     * @return New onset_vector with AND operation applied
     */
    onset_vector componentwiseAnd(const std::vector<int>& other, bool useLooping = false) const {
        if (other.empty()) return onset_vector({}, offset_, mod_);
        if (data_.empty()) return *this;
        
        std::vector<int> result;
        
        if (useLooping) {
            size_t maxLength = std::max(data_.size(), other.size());
            result.reserve(maxLength);
            
            for (size_t i = 0; i < maxLength; ++i) {
                int val1 = data_[i % data_.size()];
                int val2 = other[i % other.size()];
                result.emplace_back(val1 & val2);
            }
        } else {
            size_t minLength = std::min(data_.size(), other.size());
            result.reserve(std::max(data_.size(), other.size()));
            
            for (size_t i = 0; i < minLength; ++i) {
                result.emplace_back(data_[i] & other[i]);
            }
            
            for (size_t i = minLength; i < data_.size(); ++i) {
                result.emplace_back(data_[i]);
            }
            
            for (size_t i = minLength; i < other.size(); ++i) {
                result.emplace_back(other[i]);
            }
        }
        
        return onset_vector(result, offset_, mod_);
    }

    /**
     * @brief Componentwise XOR with optional looping
     * @param other Vector to XOR with
     * @param useLooping If true, use cyclic wraparound; if false, extend with unprocessed elements
     * @return New onset_vector with XOR operation applied
     */
    onset_vector componentwiseXor(const std::vector<int>& other, bool useLooping = false) const {
        if (other.empty()) return *this;
        if (data_.empty()) return onset_vector(other, offset_, mod_);
        
        std::vector<int> result;
        
        if (useLooping) {
            size_t maxLength = std::max(data_.size(), other.size());
            result.reserve(maxLength);
            
            for (size_t i = 0; i < maxLength; ++i) {
                int val1 = data_[i % data_.size()];
                int val2 = other[i % other.size()];
                result.emplace_back(val1 ^ val2);
            }
        } else {
            size_t minLength = std::min(data_.size(), other.size());
            result.reserve(std::max(data_.size(), other.size()));
            
            for (size_t i = 0; i < minLength; ++i) {
                result.emplace_back(data_[i] ^ other[i]);
            }
            
            for (size_t i = minLength; i < data_.size(); ++i) {
                result.emplace_back(data_[i]);
            }
            
            for (size_t i = minLength; i < other.size(); ++i) {
                result.emplace_back(other[i]);
            }
        }
        
        return onset_vector(result, offset_, mod_);
    }

    // ==================== LOGICAL OPERATIONS (LCM-ADAPTED) ====================

    /**
     * @brief Bitwise OR (union) of two patterns with LCM adaptation
     * @param other onset_vector to OR with
     * @return New onset_vector with 1 where either pattern has 1
     * @details Combines patterns - pulse occurs if either source has a pulse
     */
    onset_vector operator|(const onset_vector& other) const {
        std::vector<onset_vector> adapted = adapt_to_lcm({*this, other});
        return adapted[0].componentwiseOr(adapted[1].data_, false);
    }

    /**
     * @brief Bitwise AND (intersection) of two patterns with LCM adaptation
     * @param other onset_vector to AND with
     * @return New onset_vector with 1 only where both patterns have 1
     * @details Creates sparse pattern - pulse only where both sources pulse
     */
    onset_vector operator&(const onset_vector& other) const {
        std::vector<onset_vector> adapted = adapt_to_lcm({*this, other});
        return adapted[0].componentwiseAnd(adapted[1].data_, false);
    }

    /**
     * @brief Bitwise XOR (symmetric difference) of two patterns with LCM adaptation
     * @param other onset_vector to XOR with
     * @return New onset_vector with 1 where exactly one pattern has 1
     * @details Creates counter-rhythm - pulse where patterns don't coincide
     */
    onset_vector operator^(const onset_vector& other) const {
        std::vector<onset_vector> adapted = adapt_to_lcm({*this, other});
        return adapted[0].componentwiseXor(adapted[1].data_, false);
    }

    /**
     * @brief Bitwise NOR (negated union) of two patterns
     * @param other onset_vector to NOR with
     * @return New onset_vector with 1 where neither pattern has 1
     * @details Creates silence pattern - pulse only in gaps of both sources
     */
    onset_vector nor(const onset_vector& other) const {
        return ~(*this | other);
    }

    /**
     * @brief Bitwise NAND (negated intersection) of two patterns
     * @param other onset_vector to NAND with
     * @return New onset_vector with 0 only where both patterns have 1
     * @details Pulse everywhere except where both sources coincide
     */
    onset_vector nand(const onset_vector& other) const {
        return ~(*this & other);
    }

    /**
     * @brief Bitwise XNOR (equivalence) of two patterns
     * @param other onset_vector to compare with
     * @return New onset_vector with 1 where both patterns match
     * @details Pulse where patterns are the same (both 0 or both 1)
     */
    onset_vector xnor(const onset_vector& other) const {
        return ~(*this ^ other);
    }

    /**
     * @brief Bitwise NOT (complement) - unary operator
     * @return New onset_vector with all bits flipped
     * @details Alias for complement() method
     */
    onset_vector operator~() const {
        return complement();
    }

    // ==================== LOGICAL ASSIGNMENT OPERATORS ====================

    onset_vector& operator|=(const onset_vector& other) {
        *this = *this | other;
        return *this;
    }

    onset_vector& operator&=(const onset_vector& other) {
        *this = *this & other;
        return *this;
    }

    onset_vector& operator^=(const onset_vector& other) {
        *this = *this ^ other;
        return *this;
    }

    // ==================== ACCESS AND COMPARISON ====================

    /**
     * @brief Cyclic access to elements
     * @param index Index to access (can be any integer)
     * @return Value at index with cyclic behavior
     */
    int operator[](int index) const {
        if (data_.empty()) return 0;
        int size = static_cast<int>(data_.size());
        division_result div = euclidean_division(index, size);
        return data_[div.remainder];
    }

    bool operator==(const onset_vector& other) const {
        return data_ == other.data_ && offset_ == other.offset_ && mod_ == other.mod_;
    }

    bool operator!=(const onset_vector& other) const {
        return !(*this == other);
    }

    // ==================== FRIEND OPERATORS ====================

    friend onset_vector operator*(int scalar, const onset_vector& bv) {
        return bv * scalar;
    }

    friend std::ostream& operator<<(std::ostream& os, const onset_vector& bv) {
        os << "[";
        for (size_t i = 0; i < bv.data_.size(); ++i) {
            os << bv.data_[i];
            if (i < bv.data_.size() - 1) os << ", ";
        }
        os << "] (offset: " << bv.offset_ << ")";
        return os;
    }

    // ==================== STATIC METHODS ====================

    /**
     * @brief Adapt vectors to LCM by spacing elements
     * @param vectors Vector of onset_vectors to adapt
     * @return Adapted vectors with uniform modulo
     * @details Elements are spaced by inserting zeros between them
     */
    static std::vector<onset_vector> adapt_to_lcm(const std::vector<onset_vector>& vectors) {
        if (vectors.empty()) {
            return std::vector<onset_vector>();
        }

        std::set<int> uniqueModulos;
        for (const auto& bv : vectors) {
            uniqueModulos.insert(bv.mod_);
        }

        if (uniqueModulos.size() == 1) {
            return vectors;
        }

        std::vector<int> modulosList(uniqueModulos.begin(), uniqueModulos.end());
        const int lcm_val = lcm(modulosList);

        std::vector<onset_vector> adaptedVectors;
        adaptedVectors.reserve(vectors.size());

        for (const onset_vector& bv : vectors) {
            int scaleFactor = lcm_val / bv.mod_;
            
            // Space out elements by inserting zeros
            std::vector<int> spacedData;
            spacedData.reserve(bv.data_.size() * scaleFactor);

            for (int val : bv.data_) {
                spacedData.emplace_back(val);
                for (int i = 1; i < scaleFactor; ++i) {
                    spacedData.emplace_back(0);
                }
            }

            onset_vector adaptedBV(spacedData, bv.offset_, lcm_val);
            adaptedVectors.emplace_back(adaptedBV);
        }

        return adaptedVectors;
    }

    /**
     * @brief Generate Euclidean rhythm
     * @param pulses Number of pulses (onsets)
     * @param steps Total number of steps
     * @return onset_vector containing the Euclidean rhythm
     * @details Uses Bjorklund's algorithm to distribute pulses evenly
     */
    static onset_vector euclidean(int pulses, int steps) {
        if (pulses <= 0 || steps <= 0 || pulses > steps) {
            throw std::invalid_argument("Invalid Euclidean parameters");
        }

        std::vector<std::vector<int>> groups;
        
        // Initialize with pulses and silences
        for (int i = 0; i < pulses; ++i) {
            groups.push_back({1});
        }
        for (int i = 0; i < steps - pulses; ++i) {
            groups.push_back({0});
        }

        // Bjorklund's algorithm
        while (groups.size() > 1) {
            int minSize = std::min(pulses, steps - pulses);
            
            // Combine first minSize groups with last minSize groups
            for (int i = 0; i < minSize; ++i) {
                groups[i].insert(groups[i].end(), 
                                groups[groups.size() - 1].begin(), 
                                groups[groups.size() - 1].end());
                groups.pop_back();
            }
            
            pulses = minSize;
            steps = static_cast<int>(groups.size());
        }

        // Flatten result
        std::vector<int> result;
        for (const auto& group : groups) {
            result.insert(result.end(), group.begin(), group.end());
        }

        // `steps` was overwritten in the loop; period is the pattern length.
        return onset_vector(result, 0, static_cast<int>(result.size()));
    }

    // ==================== TRANSFORMATION METHODS ====================

    /**
     * @brief Rotate the pattern cyclically
     * @param rotationAmount Amount to rotate (positive or negative)
     * @return New onset_vector with rotated pattern
     */
    onset_vector rotate(int rotationAmount) const {
        if (data_.empty()) {
            return *this;
        }

        int size = static_cast<int>(data_.size());
        division_result div = euclidean_division(rotationAmount, size);
        int normalizedRotation = div.remainder;

        std::vector<int> rotatedData(size);
        for (int i = 0; i < size; ++i) {
            rotatedData[i] = data_[(i + normalizedRotation) % size];
        }

        return onset_vector(rotatedData, offset_, mod_);
    }

    /**
     * @brief Calculate complement (flip all bits)
     * @return New onset_vector with inverted bits
     */
    onset_vector complement() const {
        std::vector<int> complementData(data_.size());
        for (size_t i = 0; i < data_.size(); ++i) {
            complementData[i] = 1 - data_[i];
        }
        return onset_vector(complementData, offset_, mod_);
    }

    /**
     * @brief Inversion around an axis
     * @param axisIndex Index of the axis element
     * @return New onset_vector with pattern inverted around axis
     */
    onset_vector inversion(int axisIndex) const {
        if (data_.empty()) {
            return *this;
        }

        int size = static_cast<int>(data_.size());
        division_result div = euclidean_division(axisIndex, size);
        int normalizedAxis = div.remainder;

        std::vector<int> invertedData(size);
        for (int i = 0; i < size; ++i) {
            int distance = i - normalizedAxis;
            int mirrorIndex = normalizedAxis - distance;
            division_result mirrorDiv = euclidean_division(mirrorIndex, size);
            invertedData[i] = data_[mirrorDiv.remainder];
        }

        return onset_vector(invertedData, offset_, mod_);
    }

    /**
     * @brief Transpose the pattern using offset
     * @param transpositionAmount Amount to transpose
     * @return New onset_vector with updated offset
     */
    onset_vector transpose(int transpositionAmount) const {
        return onset_vector(data_, offset_ + transpositionAmount, mod_);
    }

    // ==================== UTILITY METHODS ====================

    /**
     * @brief Concatenate with another onset_vector
     * @param other onset_vector to append
     * @return New onset_vector with concatenated data
     */
    onset_vector concatenate(const onset_vector& other) const {
        std::vector<int> result = data_;
        result.insert(result.end(), other.data_.begin(), other.data_.end());
        return onset_vector(result, offset_, mod_);
    }

    /**
     * @brief Repeat the pattern multiple times
     * @param times Number of repetitions
     * @return New onset_vector with repeated pattern
     */
    onset_vector repeat(int times) const {
        if (times <= 0) {
            return onset_vector({}, offset_, mod_);
        }

        std::vector<int> result;
        result.reserve(data_.size() * times);

        for (int t = 0; t < times; ++t) {
            result.insert(result.end(), data_.begin(), data_.end());
        }

        return onset_vector(result, offset_, mod_);
    }

    /**
     * @brief Count the number of active pulses (1s)
     * @return Number of 1s in the pattern
     */
    int countPulses() const {
        return static_cast<int>(std::count(data_.begin(), data_.end(), 1));
    }

    /**
     * @brief Get density (ratio of 1s to total length)
     * @return Density value between 0 and 1
     */
    double density() const {
        if (data_.empty()) return 0.0;
        return static_cast<double>(countPulses()) / data_.size();
    }

    /**
     * @brief Extract indices where pulses occur
     * @return Vector of indices containing 1s
     */
    std::vector<int> getPulseIndices() const {
        std::vector<int> indices;
        for (size_t i = 0; i < data_.size(); ++i) {
            if (data_[i] == 1) {
                indices.emplace_back(static_cast<int>(i));
            }
        }
        return indices;
    }

    /**
     * @brief Calculate inter-onset intervals (time between pulses)
     * @return Vector of intervals between consecutive pulses
     */
    std::vector<int> getInterOnsetIntervals() const {
        std::vector<int> indices = getPulseIndices();
        if (indices.size() < 2) {
            return std::vector<int>();
        }

        std::vector<int> intervals;
        for (size_t i = 1; i < indices.size(); ++i) {
            intervals.emplace_back(indices[i] - indices[i-1]);
        }
        
        // Add wraparound interval
        intervals.emplace_back(static_cast<int>(data_.size()) - indices.back() + indices[0]);
        
        return intervals;
    }

    // ==================== DEBUG/OUTPUT ====================

    void printData() const {
        std::cout << "Data: [";
        for (size_t i = 0; i < data_.size(); ++i) {
            std::cout << data_[i];
            if (i < data_.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }

    void printInfo() const {
        std::cout << "=== onset_vector ===" << std::endl;
        printData();
        std::cout << "Offset: " << offset_ << std::endl;
        std::cout << "Mod: " << mod_ << std::endl;
        std::cout << "Size: " << data_.size() << std::endl;
        std::cout << "Pulses: " << countPulses() << std::endl;
        std::cout << "Density: " << density() << std::endl;
    }

    void printPattern() const {
        for (size_t i = 0; i < data_.size(); ++i) {
            std::cout << (data_[i] == 1 ? "X" : ".");
        }
        std::cout << std::endl;
    }
};

} // namespace musicpp

#endif // MUSICPP_ONSET_VECTOR_H
