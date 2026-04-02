#ifndef BINARYVECTOR_H
#define BINARYVECTOR_H

#include "./mathUtil.h"

using namespace std;

/**
 * @file BinaryVector.h
 * @brief Definition of BinaryVector class for binary rhythm patterns
 * @author [not251]
 * @date 2025
 */

/**
 * @class BinaryVector
 * @brief Class for representing binary vectors (0s and 1s) with rhythmic operations
 * 
 * @details A BinaryVector represents rhythmic patterns as sequences of 0s and 1s.
 * It supports:
 * - Offset-based transposition
 * - Scaling through multiplication and division (stretching/warping)
 * - Cyclic rotation
 * - Complement and inversion operations
 * - Logical operations (OR, AND, XOR, NOR, NAND, XNOR)
 * - Automatic modulo adaptation via LCM
 */
class BinaryVector {
public:
    vector<int> data;        ///< Binary data (0s and 1s)
    int offset;              ///< Offset for transposition
    int mod;                 ///< Modulo base (period)

    /**
     * @brief Validates that data contains only 0s and 1s
     * @throw invalid_argument if data contains invalid values
     */
    void validateBinaryData() const {
        for (int val : data) {
            if (val != 0 && val != 1) {
                throw invalid_argument("BinaryVector data must contain only 0s and 1s");
            }
        }
    }


    /**
     * @brief Default constructor
     */
    BinaryVector() 
        : data({1, 0, 0, 0}), 
          offset(0),
          mod(4)
    {}

    /**
     * @brief Parameterized constructor
     * @param data Binary vector data
     * @param offset Initial offset value
     * @param mod Modulo base (default 4)
     * @throw invalid_argument if data contains non-binary values
     */
    BinaryVector(const vector<int>& data, 
                 int offset = 0,
                 int mod = 4)
        : data(data), 
          offset(offset),
          mod(mod)
    {
        validateBinaryData();
    }

    // ==================== GETTERS ====================

    const vector<int>& getData() const { return data; }
    int getOffset() const { return offset; }
    int getMod() const { return mod; }
    size_t size() const { return data.size(); }

    // ==================== SETTERS ====================

    void setOffset(int newOffset) { 
        offset = newOffset;
    }

    void setMod(int newMod) { 
        mod = newMod;
    }

    // ==================== SCALAR OPERATIONS ====================

    /**
     * @brief Multiply (space out) the pattern by a scalar
     * @param scalar Spacing factor
     * @return New BinaryVector with elements spaced by zeros
     * @details Elements are spaced out by inserting (scalar-1) zeros between each element
     */
    BinaryVector operator*(int scalar) const { // v: vettore in ingresso, k: scalare positivo
    if (scalar <= 0) {
        throw invalid_argument("scalar must be positive"); // controllo che k sia positivo
    }
    
    int n = data.size(); // lunghezza del vettore in ingresso
    vector<int> out(scalar * n, 0); // inizializzo un vettore di zeri lungo kn
    
    for (int i = 0; i < scalar * n; i++) { // per ogni intero da 0 a kn
        if ((i + 1) % scalar == 1) { // se l'indice successivo è congruente a 1 modulo k
            int index = i / scalar; // inizializzo l'indice di selezione a i/k
            out[i] = data[index]; // metto alla i-esima posizione del vettore in uscita l'elemento alla i/k-esima posizione del vettore in ingresso
        }
    }
    
    return BinaryVector(out, offset, mod); // restituisco il vettore 'moltiplicato'
}


 /**
 * @brief Divide (compress spacing) the pattern by a scalar
 * @param divisor Compression factor
 * @return New BinaryVector with spacing between pulses compressed
 * @details Compresses the spaces between 1s by removing zeros proportionally.
 * For each gap between pulses, keeps only 1/divisor of the zeros (rounded down).
 * This is the inverse operation of multiplication.
 */
BinaryVector operator/(int scalar) const { // v: vettore in ingresso, k: scalare positivo
    int n = data.size(); // lunghezza del vettore in ingresso

    if (scalar <= 0) {
        throw invalid_argument("k must be positive"); // controllo che k sia positivo
    }
    
    if (scalar > n) {
        throw invalid_argument("k must be less than or equal to vector size"); // controllo che k sia minore o uguale a n
    }
    
    
    if (n % scalar != 0) {
        throw invalid_argument("Vector size must be divisible by k"); // controllo che n sia divisibile per k
    }
    
    vector<int> out(n / scalar); // inizializzo un vettore di zeri lungo n/k
    for (int i = 0; i < n / scalar; i++) { // per ogni intero da 0 a n/k
        int index = i * scalar; // inizializzo l'indice di selezione a ik
        out[i] = data[index]; // metto alla i-esima posizione del vettore in uscita l'elemento alla ik-esima posizione del vettore in ingresso
    }
    return BinaryVector(out, offset, mod); // restituisco il vettore 'diviso'
}

    BinaryVector& operator*=(int scalar) {
        *this = *this * scalar;
        return *this;
    }

    BinaryVector& operator/=(int divisor) {
        *this = *this / divisor;
        return *this;
    }
// old, deprecated
BinaryVector divide(int divisor) const {
    if (divisor <= 0) {
        throw invalid_argument("Divisor must be positive");
    }
    
    if (divisor == 1) {
        return *this;
    }
    
    vector<int> compressed;
    int consecutiveZeros = 0;
    
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] == 1) {
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
    while (compressed.size() < data.size()) {
        compressed.emplace_back(0);
    }
    
    // Truncate if somehow longer (shouldn't happen, but safety check)
    if (compressed.size() > data.size()) {
        compressed.resize(data.size());
    }
    
    return BinaryVector(compressed, offset, mod);
}


    // deprecated
    BinaryVector stretch(int scalar) const {
        if (scalar <= 0) {
            throw invalid_argument("Scalar must be positive for multiplication");
        }

        vector<int> result;
        result.reserve(data.size() * scalar);

        for (int val : data) {
            result.emplace_back(val);
            for (int i = 1; i < scalar; ++i) {
                result.emplace_back(0);
            }
        }

        return BinaryVector(result, offset, mod * scalar);
    }
    
    // ==================== COMPONENTWISE LOGICAL OPERATIONS ====================

    /**
     * @brief Componentwise OR with optional looping
     * @param other Vector to OR with
     * @param useLooping If true, use cyclic wraparound; if false, extend with unprocessed elements
     * @return New BinaryVector with OR operation applied
     */
    BinaryVector componentwiseOr(const vector<int>& other, bool useLooping = false) const {
        if (other.empty()) return *this;
        if (data.empty()) return BinaryVector(other, offset, mod);
        
        vector<int> result;
        
        if (useLooping) {
            size_t maxLength = max(data.size(), other.size());
            result.reserve(maxLength);
            
            for (size_t i = 0; i < maxLength; ++i) {
                int val1 = data[i % data.size()];
                int val2 = other[i % other.size()];
                result.emplace_back(val1 | val2);
            }
        } else {
            size_t minLength = min(data.size(), other.size());
            result.reserve(max(data.size(), other.size()));
            
            for (size_t i = 0; i < minLength; ++i) {
                result.emplace_back(data[i] | other[i]);
            }
            
            for (size_t i = minLength; i < data.size(); ++i) {
                result.emplace_back(data[i]);
            }
            
            for (size_t i = minLength; i < other.size(); ++i) {
                result.emplace_back(other[i]);
            }
        }
        
        return BinaryVector(result, offset, mod);
    }

    /**
     * @brief Componentwise AND with optional looping
     * @param other Vector to AND with
     * @param useLooping If true, use cyclic wraparound; if false, extend with unprocessed elements
     * @return New BinaryVector with AND operation applied
     */
    BinaryVector componentwiseAnd(const vector<int>& other, bool useLooping = false) const {
        if (other.empty()) return BinaryVector({}, offset, mod);
        if (data.empty()) return *this;
        
        vector<int> result;
        
        if (useLooping) {
            size_t maxLength = max(data.size(), other.size());
            result.reserve(maxLength);
            
            for (size_t i = 0; i < maxLength; ++i) {
                int val1 = data[i % data.size()];
                int val2 = other[i % other.size()];
                result.emplace_back(val1 & val2);
            }
        } else {
            size_t minLength = min(data.size(), other.size());
            result.reserve(max(data.size(), other.size()));
            
            for (size_t i = 0; i < minLength; ++i) {
                result.emplace_back(data[i] & other[i]);
            }
            
            for (size_t i = minLength; i < data.size(); ++i) {
                result.emplace_back(data[i]);
            }
            
            for (size_t i = minLength; i < other.size(); ++i) {
                result.emplace_back(other[i]);
            }
        }
        
        return BinaryVector(result, offset, mod);
    }

    /**
     * @brief Componentwise XOR with optional looping
     * @param other Vector to XOR with
     * @param useLooping If true, use cyclic wraparound; if false, extend with unprocessed elements
     * @return New BinaryVector with XOR operation applied
     */
    BinaryVector componentwiseXor(const vector<int>& other, bool useLooping = false) const {
        if (other.empty()) return *this;
        if (data.empty()) return BinaryVector(other, offset, mod);
        
        vector<int> result;
        
        if (useLooping) {
            size_t maxLength = max(data.size(), other.size());
            result.reserve(maxLength);
            
            for (size_t i = 0; i < maxLength; ++i) {
                int val1 = data[i % data.size()];
                int val2 = other[i % other.size()];
                result.emplace_back(val1 ^ val2);
            }
        } else {
            size_t minLength = min(data.size(), other.size());
            result.reserve(max(data.size(), other.size()));
            
            for (size_t i = 0; i < minLength; ++i) {
                result.emplace_back(data[i] ^ other[i]);
            }
            
            for (size_t i = minLength; i < data.size(); ++i) {
                result.emplace_back(data[i]);
            }
            
            for (size_t i = minLength; i < other.size(); ++i) {
                result.emplace_back(other[i]);
            }
        }
        
        return BinaryVector(result, offset, mod);
    }

    // ==================== LOGICAL OPERATIONS (LCM-ADAPTED) ====================

    /**
     * @brief Bitwise OR (union) of two patterns with LCM adaptation
     * @param other BinaryVector to OR with
     * @return New BinaryVector with 1 where either pattern has 1
     * @details Combines patterns - pulse occurs if either source has a pulse
     */
    BinaryVector operator|(const BinaryVector& other) const {
        vector<BinaryVector> adapted = adaptToLCM({*this, other});
        return adapted[0].componentwiseOr(adapted[1].data, false);
    }

    /**
     * @brief Bitwise AND (intersection) of two patterns with LCM adaptation
     * @param other BinaryVector to AND with
     * @return New BinaryVector with 1 only where both patterns have 1
     * @details Creates sparse pattern - pulse only where both sources pulse
     */
    BinaryVector operator&(const BinaryVector& other) const {
        vector<BinaryVector> adapted = adaptToLCM({*this, other});
        return adapted[0].componentwiseAnd(adapted[1].data, false);
    }

    /**
     * @brief Bitwise XOR (symmetric difference) of two patterns with LCM adaptation
     * @param other BinaryVector to XOR with
     * @return New BinaryVector with 1 where exactly one pattern has 1
     * @details Creates counter-rhythm - pulse where patterns don't coincide
     */
    BinaryVector operator^(const BinaryVector& other) const {
        vector<BinaryVector> adapted = adaptToLCM({*this, other});
        return adapted[0].componentwiseXor(adapted[1].data, false);
    }

    /**
     * @brief Bitwise NOR (negated union) of two patterns
     * @param other BinaryVector to NOR with
     * @return New BinaryVector with 1 where neither pattern has 1
     * @details Creates silence pattern - pulse only in gaps of both sources
     */
    BinaryVector nor(const BinaryVector& other) const {
        return ~(*this | other);
    }

    /**
     * @brief Bitwise NAND (negated intersection) of two patterns
     * @param other BinaryVector to NAND with
     * @return New BinaryVector with 0 only where both patterns have 1
     * @details Pulse everywhere except where both sources coincide
     */
    BinaryVector nand(const BinaryVector& other) const {
        return ~(*this & other);
    }

    /**
     * @brief Bitwise XNOR (equivalence) of two patterns
     * @param other BinaryVector to compare with
     * @return New BinaryVector with 1 where both patterns match
     * @details Pulse where patterns are the same (both 0 or both 1)
     */
    BinaryVector xnor(const BinaryVector& other) const {
        return ~(*this ^ other);
    }

    /**
     * @brief Bitwise NOT (complement) - unary operator
     * @return New BinaryVector with all bits flipped
     * @details Alias for complement() method
     */
    BinaryVector operator~() const {
        return complement();
    }

    // ==================== LOGICAL ASSIGNMENT OPERATORS ====================

    BinaryVector& operator|=(const BinaryVector& other) {
        *this = *this | other;
        return *this;
    }

    BinaryVector& operator&=(const BinaryVector& other) {
        *this = *this & other;
        return *this;
    }

    BinaryVector& operator^=(const BinaryVector& other) {
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
        if (data.empty()) return 0;
        int size = static_cast<int>(data.size());
        DivisionResult div = euclideanDivision(index, size);
        return data[div.remainder];
    }

    bool operator==(const BinaryVector& other) const {
        return data == other.data && offset == other.offset && mod == other.mod;
    }

    bool operator!=(const BinaryVector& other) const {
        return !(*this == other);
    }

    // ==================== FRIEND OPERATORS ====================

    friend BinaryVector operator*(int scalar, const BinaryVector& bv) {
        return bv * scalar;
    }

    friend ostream& operator<<(ostream& os, const BinaryVector& bv) {
        os << "[";
        for (size_t i = 0; i < bv.data.size(); ++i) {
            os << bv.data[i];
            if (i < bv.data.size() - 1) os << ", ";
        }
        os << "] (offset: " << bv.offset << ")";
        return os;
    }

    // ==================== STATIC METHODS ====================

    /**
     * @brief Adapt vectors to LCM by spacing elements
     * @param vectors Vector of BinaryVectors to adapt
     * @return Adapted vectors with uniform modulo
     * @details Elements are spaced by inserting zeros between them
     */
    static vector<BinaryVector> adaptToLCM(const vector<BinaryVector>& vectors) {
        if (vectors.empty()) {
            return vector<BinaryVector>();
        }

        set<int> uniqueModulos;
        for (const auto& bv : vectors) {
            uniqueModulos.insert(bv.mod);
        }

        if (uniqueModulos.size() == 1) {
            return vectors;
        }

        vector<int> modulosList(uniqueModulos.begin(), uniqueModulos.end());
        int lcm = LCM(modulosList);

        vector<BinaryVector> adaptedVectors;
        adaptedVectors.reserve(vectors.size());

        for (const BinaryVector& bv : vectors) {
            int scaleFactor = lcm / bv.mod;
            
            // Space out elements by inserting zeros
            vector<int> spacedData;
            spacedData.reserve(bv.data.size() * scaleFactor);

            for (int val : bv.data) {
                spacedData.emplace_back(val);
                for (int i = 1; i < scaleFactor; ++i) {
                    spacedData.emplace_back(0);
                }
            }

            BinaryVector adaptedBV(spacedData, bv.offset, lcm);
            adaptedVectors.emplace_back(adaptedBV);
        }

        return adaptedVectors;
    }

    /**
     * @brief Generate Euclidean rhythm
     * @param pulses Number of pulses (onsets)
     * @param steps Total number of steps
     * @return BinaryVector containing the Euclidean rhythm
     * @details Uses Bjorklund's algorithm to distribute pulses evenly
     */
    static BinaryVector euclidean(int pulses, int steps) {
        if (pulses <= 0 || steps <= 0 || pulses > steps) {
            throw invalid_argument("Invalid Euclidean parameters");
        }

        vector<vector<int>> groups;
        
        // Initialize with pulses and silences
        for (int i = 0; i < pulses; ++i) {
            groups.push_back({1});
        }
        for (int i = 0; i < steps - pulses; ++i) {
            groups.push_back({0});
        }

        // Bjorklund's algorithm
        while (groups.size() > 1) {
            int minSize = min(pulses, steps - pulses);
            
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
        vector<int> result;
        for (const auto& group : groups) {
            result.insert(result.end(), group.begin(), group.end());
        }

        return BinaryVector(result, 0, static_cast<int>(result.size()));
    }

    // ==================== TRANSFORMATION METHODS ====================

    /**
     * @brief Rotate the pattern cyclically
     * @param rotationAmount Amount to rotate (positive or negative)
     * @return New BinaryVector with rotated pattern
     */
    BinaryVector rotate(int rotationAmount) const {
        if (data.empty()) {
            return *this;
        }

        int size = static_cast<int>(data.size());
        DivisionResult div = euclideanDivision(rotationAmount, size);
        int normalizedRotation = div.remainder;

        vector<int> rotatedData(size);
        for (int i = 0; i < size; ++i) {
            rotatedData[i] = data[(i + normalizedRotation) % size];
        }

        return BinaryVector(rotatedData, offset, mod);
    }

    /**
     * @brief Calculate complement (flip all bits)
     * @return New BinaryVector with inverted bits
     */
    BinaryVector complement() const {
        vector<int> complementData(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            complementData[i] = 1 - data[i];
        }
        return BinaryVector(complementData, offset, mod);
    }

    /**
     * @brief Inversion around an axis
     * @param axisIndex Index of the axis element
     * @return New BinaryVector with pattern inverted around axis
     */
    BinaryVector inversion(int axisIndex) const {
        if (data.empty()) {
            return *this;
        }

        int size = static_cast<int>(data.size());
        DivisionResult div = euclideanDivision(axisIndex, size);
        int normalizedAxis = div.remainder;

        vector<int> invertedData(size);
        for (int i = 0; i < size; ++i) {
            int distance = i - normalizedAxis;
            int mirrorIndex = normalizedAxis - distance;
            DivisionResult mirrorDiv = euclideanDivision(mirrorIndex, size);
            invertedData[i] = data[mirrorDiv.remainder];
        }

        return BinaryVector(invertedData, offset, mod);
    }

    /**
     * @brief Transpose the pattern using offset
     * @param transpositionAmount Amount to transpose
     * @return New BinaryVector with updated offset
     */
    BinaryVector transpose(int transpositionAmount) const {
        return BinaryVector(data, offset + transpositionAmount, mod);
    }

    // ==================== UTILITY METHODS ====================

    /**
     * @brief Concatenate with another BinaryVector
     * @param other BinaryVector to append
     * @return New BinaryVector with concatenated data
     */
    BinaryVector concatenate(const BinaryVector& other) const {
        vector<int> result = data;
        result.insert(result.end(), other.data.begin(), other.data.end());
        return BinaryVector(result, offset, mod);
    }

    /**
     * @brief Repeat the pattern multiple times
     * @param times Number of repetitions
     * @return New BinaryVector with repeated pattern
     */
    BinaryVector repeat(int times) const {
        if (times <= 0) {
            return BinaryVector({}, offset, mod);
        }

        vector<int> result;
        result.reserve(data.size() * times);

        for (int t = 0; t < times; ++t) {
            result.insert(result.end(), data.begin(), data.end());
        }

        return BinaryVector(result, offset, mod);
    }

    /**
     * @brief Count the number of active pulses (1s)
     * @return Number of 1s in the pattern
     */
    int countPulses() const {
        return count(data.begin(), data.end(), 1);
    }

    /**
     * @brief Get density (ratio of 1s to total length)
     * @return Density value between 0 and 1
     */
    double density() const {
        if (data.empty()) return 0.0;
        return static_cast<double>(countPulses()) / data.size();
    }

    /**
     * @brief Extract indices where pulses occur
     * @return Vector of indices containing 1s
     */
    vector<int> getPulseIndices() const {
        vector<int> indices;
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i] == 1) {
                indices.emplace_back(static_cast<int>(i));
            }
        }
        return indices;
    }

    /**
     * @brief Calculate inter-onset intervals (time between pulses)
     * @return Vector of intervals between consecutive pulses
     */
    vector<int> getInterOnsetIntervals() const {
        vector<int> indices = getPulseIndices();
        if (indices.size() < 2) {
            return vector<int>();
        }

        vector<int> intervals;
        for (size_t i = 1; i < indices.size(); ++i) {
            intervals.emplace_back(indices[i] - indices[i-1]);
        }
        
        // Add wraparound interval
        intervals.emplace_back(static_cast<int>(data.size()) - indices.back() + indices[0]);
        
        return intervals;
    }

    // ==================== DEBUG/OUTPUT ====================

    void printData() const {
        cout << "Data: [";
        for (size_t i = 0; i < data.size(); ++i) {
            cout << data[i];
            if (i < data.size() - 1) cout << ", ";
        }
        cout << "]" << endl;
    }

    void printInfo() const {
        cout << "=== BinaryVector Info ===" << endl;
        printData();
        cout << "Offset: " << offset << endl;
        cout << "Mod: " << mod << endl;
        cout << "Size: " << data.size() << endl;
        cout << "Pulses: " << countPulses() << endl;
        cout << "Density: " << density() << endl;
    }

    void printPattern() const {
        for (size_t i = 0; i < data.size(); ++i) {
            cout << (data[i] == 1 ? "X" : ".");
        }
        cout << endl;
    }
};

#endif // BINARYVECTOR_H
