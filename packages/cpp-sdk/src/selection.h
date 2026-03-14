#ifndef SELECTION_H
#define SELECTION_H

#include "./vectors.h"

/**
 * @file selection.h
 * @brief Implementation of selection meta-operators for musical structures
 * @author [not251]
 * @date 2025
 */


    // ==================== POSITION VECTOR AS SOURCE ====================

    /**
     * @brief Position-based selection from Position Vectors
     * 
     * @param source Source position vector A
     * @param criterion Position criterion vector P
     * @param criterionRotation Rototranslation parameter for criterion vector (default 0)
     * @param voices Number of voices for output (default 0 = criterion length)
     * @return New PositionVector with selected elements
     * 
     * @details Extracts elements from the source vector at positions 
     *          specified by the criterion vector (optionally rototranslated).
     *          Uses cyclic access for both source and criterion vectors.
     *          Formula: p_{A_k} = ã_{p_k}
     * 
     * @note Musical applications: basso continuo realization, harmonic analysis,
     *       scale degree extraction
     */
    PositionVector select(const PositionVector& source, 
                         const PositionVector& criterion,
                         int criterionRotation = 0, int voices = 0) {
        // Apply rototranslation if needed
        vector<int> sourceData = source.getData();
        int criterionModulo = static_cast<int>(sourceData.size());
        PositionVector actualCriterion(criterion.getData(), criterionModulo);
        actualCriterion.setMod(source.size());
        PositionVector rotatedCriterion = (criterionRotation != 0) 
            ? actualCriterion.rotoTranslate(criterionRotation, voices)
            : actualCriterion;
        
        // Determine output length
        int outLength = (voices > 0) ? voices : static_cast<int>(rotatedCriterion.size());
        vector<int> result(outLength);
        
        // Use cyclic access for both vectors
        for (int k = 0; k < outLength; ++k) {
            result[k] = source[rotatedCriterion[k]];
        }
        
        return PositionVector(result, source.getMod(), source.getUserRange(), 
                            source.getRangeUpdate(), source.getUser());
    }

    /**
     * @brief Interval-based selection from Position Vectors
     * 
     * @param source Source position vector A
     * @param criterion Interval selection vector I (uses embedded offset)
     * @param criterionRotation Rotation parameter for interval vector (default 0)
     * @param voices Number of voices for output (default 0 = criterion length)
     * @return New PositionVector with selected elements
     * 
     * @details Extracts elements at positions determined by cumulative sums 
     *          of the interval vector (optionally rotated), starting from the
     *          interval vector's offset. Uses cyclic access for both vectors.
     *          Formula: i_{A_0} = ã_{offset}
     *                   i_{A_k} = ã_{(sum_{j=0}^{k-1} i_j + offset)}
     * 
     * @note Musical applications: harmony by intervals, voice leading analysis,
     *       chord construction following intervallic patterns
     */
    PositionVector select(const PositionVector& source,
                         const IntervalVector& criterion,
                         int criterionRotation = 0, int voices = 0) {
        // Apply rotation if needed
        IntervalVector actualCriterion = criterion;
        actualCriterion.setMod(source.size());
        IntervalVector rotatedCriterion = (criterionRotation != 0)
            ? actualCriterion.rotate(criterionRotation, voices)
            : actualCriterion;
        
        // Determine output length
        int outLength = (voices > 0) ? voices : static_cast<int>(rotatedCriterion.size());
        vector<int> result(outLength);
        
        int cumulativePosition = rotatedCriterion.getOffset();
        
        // Use cyclic access for both vectors
        for (int k = 0; k < outLength; ++k) {
            result[k] = source[cumulativePosition];
            cumulativePosition += rotatedCriterion[k];
        }
        
        return PositionVector(result, source.getMod(), source.getUserRange(),
                            source.getRangeUpdate(), source.getUser());
    }

    // ==================== INTERVAL VECTOR AS SOURCE ====================

    /**
     * @brief Interval-based selection from Interval Vectors
     * 
     * @param source Source interval vector A (uses embedded offset s_A)
     * @param indices Index selection vector I (uses embedded offset s_I)
     * @param criterionRotation Rotation parameter for index vector (default 0)
     * @param voices Number of voices for output (default 0 = criterion length)
     * @return New IntervalVector with selected intervals (output offset = s_A + sum_{j=0}^{s_I-1} ã_j)
     * 
     * @details Extracts consecutive intervals from the source vector,
     *          where each extracted interval spans a number of source intervals
     *          specified by the corresponding element in the index selection vector
     *          (optionally rotated). Uses cyclic access for both vectors and embedded offsets.
     * 
     *          Formula: s_out = s_A + sum_{j=0}^{s_I-1} ã_j (if s_I > 0)
     *                   i_k^A = sum_{j=0}^{i_k-1} ã_{(sum_{l=0}^{k-1} i_l + s_I + j)}
     * 
     * @note Musical applications: extended harmony construction (9th, 11th, 13th chords),
     *       compound interval formation
     */
    IntervalVector select(const IntervalVector& source,
                         const IntervalVector& indices,
                         int criterionRotation = 0, int voices = 0) {
        // Apply rotation if needed
        IntervalVector actualCriterion = indices;
        actualCriterion.setMod(source.size());
        IntervalVector rotatedCriterion = (criterionRotation != 0)
            ? actualCriterion.rotate(criterionRotation, voices)
            : actualCriterion;
        
        int sourceOffset = source.getOffset();
        int criterionOffset = rotatedCriterion.getOffset();
        
        // Calculate output offset: s_out = s_A + sum of first s_I elements
        int sOut = sourceOffset;
        for (int j = 0; j < criterionOffset; ++j) {
            sOut += source[j];
        }
        
        // Determine output length
        int outLength = (voices > 0) ? voices : static_cast<int>(rotatedCriterion.size());
        vector<int> result(outLength);
        
        int cumulativeIndex = criterionOffset;
        
        // Use cyclic access for both vectors
        for (int k = 0; k < outLength; ++k) {
            int sum = 0;
            int spanLength = rotatedCriterion[k];
            
            // Sum consecutive intervals using cyclic access
            for (int j = 0; j < spanLength; ++j) {
                sum += source[cumulativeIndex + j];
            }
            
            result[k] = sum;
            cumulativeIndex += spanLength;
        }
        
        return IntervalVector(result, sOut, source.getMod());
    }

    /**
     * @brief Position-based selection from Interval Vectors
     * 
     * @param source Source interval vector A (uses embedded offset s_A)
     * @param criterion Position selection vector P
     * @param criterionRotation Rototranslation parameter for position vector (default 0)
     * @param voices Number of voices for output (default 0 = criterion length)
     * @return New IntervalVector with extracted intervals (preserves source offset)
     * 
     * @details Extracts intervals between consecutive positions specified in 
     *          the selection vector (optionally rototranslated), with the final 
     *          interval wrapping around to complete the cycle. Uses cyclic access
     *          for both vectors and embedded offset from source.
     * 
     *          Formula: p_k^A = sum_{j=0}^{δ_k-1} ã_{(p_k + j)}
     *          where δ_k = p_{k+1} - p_k (with wraparound handling)
     * 
     * @note Musical applications: basso continuo from intervals, chord extraction,
     *       direct interval relationships
     */
    IntervalVector select(const IntervalVector& source,
                         const PositionVector& criterion,
                         int criterionRotation = 0, int voices = 0) {
        vector<int> sourceData = source.getData();
        int off = source.getOffset();
        int criterionModulo = static_cast<int>(sourceData.size());
        PositionVector actualCriterion = criterion;
        actualCriterion.setMod(source.size());
        PositionVector rotatedCriterion = (criterionRotation != 0) 
            ? actualCriterion.rotoTranslate(criterionRotation, voices)
            : actualCriterion;
        
        if (rotatedCriterion.size() == 0) {
            return IntervalVector({}, source.getOffset(), source.getMod());
        }


        int outLength = (voices > 0) ? voices : static_cast<int>(rotatedCriterion.size());
        vector<int> result(outLength);
        int n = static_cast<int>(source.size());
        

        for (int k = 0; k < outLength; ++k) {
            int p_k = rotatedCriterion[k];
            int p_next = rotatedCriterion[k + 1];
            

            int delta_k = p_next - p_k;
            if (delta_k <= 0) {
                delta_k += n; 
            }
            
            int sum = 0;
            for (int j = 0; j < delta_k; ++j) {
                sum += source[p_k + j];
            }
            
            result[k] = sum;
        }
        int sOut = off;
        if (criterion[0] >= 0) {
        for (int j = 0; j < criterion[0]; ++j) {
            sOut += source[j];
        }   
        }
        else {

            for (int j = criterion[0]; j < 0; ++j) {
                sOut -= source[j];
            }
        }
        return IntervalVector(result, sOut, source.getMod());
    }

#endif // SELECTION_H
