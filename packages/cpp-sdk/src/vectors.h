#ifndef VECTORS_H
#define VECTORS_H

#include "./positionVector.h"
#include "./intervalVector.h"
#include "./binaryVector.h"

/**
 * @file Vectors.h
 * @brief Unified class maintaining synchronized position, interval, and binary representations
 * @author [not251]
 * @date 2025
 */
    // ==================== CONVERSION FUNCTIONS ====================
    
    /**
     * @brief Converts positions to intervals
     * @return IntervalVector derived from current positions
     */
    IntervalVector positionsToIntervals(PositionVector positions) {
        int mod = positions.getMod();
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
     * @brief Converts intervals to positions
     * @return PositionVector derived from current intervals
     */
    PositionVector intervalsToPositions(IntervalVector intervals) {
        int mod = intervals.getMod();
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

    BinaryVector positionsToBinary(PositionVector& positions) {
        if (positions.size() == 0) {
            return BinaryVector({}, 0, positions.mod);
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
#endif // VECTORS_H