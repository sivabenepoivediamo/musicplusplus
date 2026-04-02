#ifndef Vector_H
#define Vector_H

#include "./vectors.h"

/**
 * @file Vectors.h
 * @brief Unified class maintaining synchronized position, interval, and binary representations
 * @author [not251]
 * @date 2025
 */

/**
 * @class Vectors
 * @brief Maintains three synchronized representations of a musical set
 * 
 * @details A Vectors contains three representations:
 * - Position Vector: absolute pitch positions
 * - Interval Vector: intervals between consecutive positions
 * - Binary Vector: presence/absence pattern in pitch space
 * 
 * All three representations are kept synchronized automatically.
 * Operations on any representation update all others.
 */
class Vectors {
public:
    PositionVector positions;
    IntervalVector intervals;
    BinaryVector binary;
    int mod;  ///< Global modulo for all representations
    
    // ==================== CONVERSION FUNCTIONS ====================
   
    /**
     * @brief Converts positions to intervals
     * @return IntervalVector derived from current positions
     */
    IntervalVector positionsToIntervals() const {
        if (positions.size() == 0) {
            return IntervalVector({}, 0, mod);
        }

        const size_t n = positions.size();
        vector<int> intervalData;
        intervalData.reserve(n);

        if (n == 1) {
            return IntervalVector(intervalData, positions[0], mod);
        }

        for (size_t i = 0; i + 1 < n; ++i) {
            intervalData.emplace_back(
                positions[static_cast<int>(i + 1)] - positions[static_cast<int>(i)]);
        }
        const int closureRaw =
            positions[0] - positions[static_cast<int>(n - 1)];
        const int closurePeriod = positions.getRange();
        if (closurePeriod > 0) {
            intervalData.emplace_back(euclideanDivision(closureRaw, closurePeriod).remainder);
        } else {
            intervalData.emplace_back(closureRaw);
        }

        return IntervalVector(intervalData, positions[0], mod);
    }
    
    /**
     * @brief Converts positions to binary representation
     * @return BinaryVector derived from current positions
     */
    BinaryVector positionsToBinary() const {
        if (positions.size() == 0) {
            return BinaryVector({}, 0, mod);
        }
        
        vector<int> posData = positions.getData();
        int range = positions.getRange();
        vector<int> binaryData(range, 0);
        
        // Normalize positions to range [0, range)
        int minPos = *min_element(posData.begin(), posData.end());
        for (int pos : posData) {
            int normalizedPos = pos - posData[0];
            DivisionResult div = euclideanDivision(normalizedPos, range);
            binaryData[div.remainder] = 1;
        }
        
        return BinaryVector(binaryData, minPos, range);
    }
    
    /**
     * @brief Converts intervals to positions
     * @return PositionVector derived from current intervals
     */
    PositionVector intervalsToPositions() const {
        vector<int> intervalData = intervals.getData();
        
        if (intervalData.empty()) {
            return PositionVector({intervals.getOffset()}, mod, 0, true, false);
        }
        
        // Calculate positions from intervals (starting from offset)
        vector<int> posData;
        posData.reserve(intervals.size());
        int currentPos = intervals.getOffset(); 
        posData.emplace_back(currentPos);
        
        for (size_t i = 0; i < intervalData.size() - 1; ++i) {  
            currentPos += intervalData[i];
            posData.emplace_back(currentPos);
        }
        
        return PositionVector(posData, mod, 0, true, false);
    }
    
    /**
     * @brief Converts binary to positions
     * @return PositionVector derived from current binary representation
     */
    PositionVector binaryToPositions() const {
        vector<int> binaryData = binary.getData();
        int offset = binary.getOffset();
        
        // Extract positions where binary has 1s
        vector<int> posData;
        for (size_t i = 0; i < binaryData.size(); ++i) {
            if (binaryData[i] == 1) {
                posData.emplace_back(static_cast<int>(i) + offset);
            }
        }
        
        if (posData.empty()) {
            return PositionVector({0}, mod, 0, true, false);
        }
        
        return PositionVector(posData, mod, 0, true, false);
    }
    
    // ==================== UPDATE FUNCTIONS ====================
    
    /**
     * @brief Updates intervals and binary from positions
     */
    void updateFromPositions() {
        intervals = positionsToIntervals();
        binary = positionsToBinary();
    }
    
    /**
     * @brief Updates positions and binary from intervals
     */
    void updateFromIntervals() {
        positions = intervalsToPositions();
        binary = positionsToBinary();
    }
    
    /**
     * @brief Updates positions and intervals from binary
     */
    void updateFromBinary() {
        positions = binaryToPositions();
        intervals = positionsToIntervals();
    }

public:
    // ==================== CONSTRUCTORS ====================
    
    /**
     * @brief Default constructor
     */
    Vectors(int modulo = 12) 
        : positions({0}, modulo, 0, true, false),
          intervals({}, 0, modulo),
          binary({1}, 0, modulo),
          mod(modulo) 
    {}
    
    /**
     * @brief Construct from PositionVector
     */
    Vectors(const PositionVector& pv) 
        : positions(pv),
          mod(pv.getMod())
    {
        updateFromPositions();
    }
    
    /**
     * @brief Construct from IntervalVector
     */
    Vectors(const IntervalVector& iv) 
        : intervals(iv),
          mod(iv.getMod())
    {
        updateFromIntervals();
    }
    
    /**
     * @brief Construct from BinaryVector
     */
    Vectors(const BinaryVector& bv) 
        : binary(bv),
          mod(bv.getMod())
    {
        updateFromBinary();
    }
    
    // ==================== GETTERS ====================
    
    const PositionVector& getPositions() const { return positions; }
    const IntervalVector& getIntervals() const { return intervals; }
    const BinaryVector& getBinary() const { return binary; }
    int getMod() const { return mod; }
    
    // ==================== POSITION OPERATIONS ====================
    
    /**
     * @brief Transpose positions
     */
    Vectors transpose(int amount) {
        Vectors result(*this);
        result.positions = positions + amount;
        result.updateFromPositions();
        return result;
    }
    
    /**
     * @brief Multiply positions by scalar
     */
    Vectors multiplyPositions(int scalar) {
        Vectors result(*this);
        result.positions = positions * scalar;
        result.updateFromPositions();
        return result;
    }
    
    Vectors negative(int axis = 10) {
        Vectors result(*this);
        result.positions = positions.negative(axis);
        result.updateFromPositions();
        return result;
    }
    
    /**
     * @brief Rotate position vector
     */
    Vectors rotatePositions(int amount) {
        Vectors result(*this);
        result.positions = positions.rotate(amount);
        result.updateFromPositions();
        return result;
    }
    
    /**
     * @brief Rotate position vector
     */
    Vectors rototranslatePositions(int amount, int length = 0) {
        Vectors result(*this);
        result.positions = positions.rotoTranslate(amount, length);
        result.updateFromPositions();
        return result;
    }

    /**
     * @brief Alias for roto-translation
     */
    Vectors inversion(int amount, int length = 0) {
        return rototranslatePositions(amount, length);
    }

    /**
     * @brief Invert positions around axis
     */
    Vectors invertPositions(int axisIndex, bool sortOutput = true) {
        Vectors result(*this);
        result.positions = positions.inversion(axisIndex, sortOutput);
        result.updateFromPositions();
        return result;
    }
    
    /**
     * @brief Complement of positions
     */
    Vectors complementPositions() {
        Vectors result(*this);
        result.positions = positions.complement();
        result.updateFromPositions();
        return result;
    }
    
    // ==================== INTERVAL OPERATIONS ====================
    
    /**
     * @brief Add to intervals
     */
    Vectors addToIntervals(int amount) {
        Vectors result(*this);
        result.intervals = intervals + amount;
        result.updateFromIntervals();
        return result;
    }
    
    /**
     * @brief Multiply intervals by scalar
     */
    Vectors multiplyIntervals(int scalar) {
        Vectors result(*this);
        result.intervals = intervals * scalar;
        result.updateFromIntervals();
        return result;
    }
    
    /**
     * @brief Rotate interval vector
     */
    Vectors rotateIntervals(int amount) {
        Vectors result(*this);
        result.intervals = intervals.rotate(amount);
        result.updateFromIntervals();
        return result;
    }
    
    /**
     * @brief Reverse (retrograde) intervals
     */
    Vectors reverseIntervals() {
        Vectors result(*this);
        result.intervals = intervals.reverse();
        result.updateFromIntervals();
        return result;
    }
    
    /**
     * @brief Negate intervals
     */
    Vectors invertIntervals(int axisIndex) {
        Vectors result(*this);
        result.intervals = intervals.inversion(axisIndex);
        result.updateFromIntervals();
        return result;
    }
        /**
     * @brief Alias for interval rotation
     */
    Vectors mode (int amount) {
        return rotateIntervals(amount);
    }



    // ==================== BINARY OPERATIONS ====================
    
    /**
     * @brief Rotate binary pattern
     */
    Vectors rotateBinary(int amount) {
        Vectors result(*this);
        result.binary = binary.rotate(amount);
        result.updateFromBinary();
        return result;
    }
    
    /**
     * @brief Complement binary pattern
     */
    Vectors complementBinary() {
        Vectors result(*this);
        result.binary = binary.complement();
        result.updateFromBinary();
        return result;
    }
    
    /**
     * @brief Multiply (space out) binary pattern
     */
    Vectors multiplyBinary(int scalar) {
        Vectors result(*this);
        result.binary = binary * scalar;
        result.mod = result.binary.getMod();
        result.updateFromBinary();
        return result;
    }
    
    /**
     * @brief Divide (compress) binary pattern
     */
    Vectors divideBinary(int divisor) {
        Vectors result(*this);
        result.binary = binary / divisor;
        result.mod = result.binary.getMod();
        result.updateFromBinary();
        return result;
    }
    
    /**
     * @brief OR with another Vectors
     */
    Vectors operator|(const Vectors& other) const {
        Vectors result(*this);
        result.binary = binary | other.binary;
        result.updateFromBinary();
        return result;
    }
    
    /**
     * @brief AND with another Vectors
     */
    Vectors operator&(const Vectors& other) const {
        Vectors result(*this);
        result.binary = binary & other.binary;
        result.updateFromBinary();
        return result;
    }
    
    /**
     * @brief XOR with another Vectors
     */
    Vectors operator^(const Vectors& other) const {
        Vectors result(*this);
        result.binary = binary ^ other.binary;
        result.updateFromBinary();
        return result;
    }
    
    // ==================== UTILITY METHODS ====================
    
    /**
     * @brief Print all representations
     */
    void printAll() const {
        cout << "=== Vectors (mod=" << mod << ") ===" << endl;
        cout << "Positions: " << positions << endl;
        cout << "Intervals: " << intervals << endl;
        cout << "Binary:    " << binary << endl;
        cout << "Pattern:   ";
        binary.printPattern();
    }
    
    /**
     * @brief Print positions only
     */
    void printPositions() const {
        cout << "Positions: " << positions << endl;
    }
    
    /**
     * @brief Print intervals only
     */
    void printIntervals() const {
        cout << "Intervals: " << intervals << endl;
    }
    
    /**
     * @brief Print binary only
     */
    void printBinary() const {
        cout << "Binary: " << binary << endl;
        binary.printPattern();
    }
    
    /**
     * @brief Compare equality
     */
    bool operator==(const Vectors& other) const {
        return positions == other.positions && 
               intervals == other.intervals && 
               binary == other.binary;
    }
    
    bool operator!=(const Vectors& other) const {
        return !(*this == other);
    }
    
    // ==================== STATIC FACTORY METHODS ====================
    
    /**
     * @brief Create from position data
     */
    static Vectors fromPositions(const vector<int>& data, int modulo = 12) {
        return Vectors(PositionVector(data, modulo, -1, true, false));
    }
    
    /**
     * @brief Create from interval data
     */
    static Vectors fromIntervals(const vector<int>& data, int modulo = 12) {
        return Vectors(IntervalVector(data, 0, modulo));
    }
    
    /**
     * @brief Create from binary data
     */
    static Vectors fromBinary(const vector<int>& data, int offset = 0, int modulo = 12) {
        return Vectors(BinaryVector(data, offset, modulo));
    }
    
    /**
     * @brief Create Euclidean rhythm as Vectors
     */
    static Vectors euclidean(int pulses, int steps) {
        return Vectors(BinaryVector::euclidean(pulses, steps));
    }
};


    
    

#endif // Vector_H