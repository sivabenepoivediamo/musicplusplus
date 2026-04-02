#ifndef MUSICPP_AUTOMATIONS_H
#define MUSICPP_AUTOMATIONS_H

/**
 * @file automations.h
 * @brief High-level automation utilities for voice-leading and degree-based operations
 *
 * Provides convenience wrappers to compute best-fit relative-mode alignments, transpositions,
 * modal selections, and degree-based automations for sequences of `position_vector`.
 *
 * These functions are thin helpers built on top of the distance and matrix utilities.
 *
 * @see matrix_distance.h
 */
#include "matrix_distance.h"

#include <cstdlib>
#include <stdexcept>

namespace musicpp {

/**
 * @brief Find best relative-mode row for a given degree using a criterion
 *
 * Performs a modal selection for the provided `degree` on `scale` using
 * `criterion`, computes the modal relative-mode matrix for those selections and
 * returns the best matching row to `reference` according to `complexity`.
 *
 * @param scale Scale as a position_vector
 * @param criterion interval_vector used as a selection criterion
 * @param degree Degree index within the modal selection
 * @param reference Reference position_vector used for distance calculation
 * @param complexity Complexity index used to select among ties (default 0)
 * @return Best matching ModalRelativeModeMatrixRow
 */
inline ModalRelativeModeMatrixRow degreeAutomation(position_vector& scale, interval_vector& criterion, int degree, position_vector& reference, int complexity = 0){
    ModalSelectionMatrix<position_vector> sel = modalSelection(scale, criterion, degree);
    ModalRelativeModeMatrix<position_vector> degrees = modal_relative_mode(sel);
    ModalRelativeModeMatrixDistance distances = calculateDistances(reference, degrees);
    ModalRelativeModeMatrixRow out = distances.getByComplexity(complexity);
    return out;
}

/**
 * @brief Compute best relative-mode match to voice-lead `target` to `reference`
 *
 * Aligns `target` with `reference` and computes relative-mode distances
 * returning the selected best row according to `complexity`.
 *
 * @param reference Reference position_vector
 * @param target Target position_vector to be voice-led
 * @param complexity Complexity index used for tie-breaking (default 0)
 * @return Best matching RelativeModeMatrixRow
 */
inline RelativeModeMatrixRow voiceLeadingAutomation(position_vector& reference, position_vector& target, int complexity = 0){
    int center = align(reference, target);
    RelativeModeMatrix positions = relative_mode_matrix(target, center);
    RelativeModeMatrixDistance distances = calculateDistances(reference, positions);
    RelativeModeMatrixRow out = distances.getByComplexity(complexity);
    return out;
}

/**
 * @brief Find best modal-interchange selection matching a set of notes
 *
 * Filters the modal matrix of `scale` to selections that contain `notes`,
 * computes distances and returns the best matching row for the given `complexity`.
 *
 * @param scale Input scale as position_vector
 * @param notes Vector of pitch classes (notes) used to filter modal selections
 * @param complexity Complexity index used to pick the result
 * @return Best matching ModalMatrixRow<position_vector>
 */
inline ModalMatrixRow<position_vector> modalInterchangeAutomation(position_vector& scale, const std::vector<int>& notes, int complexity){
    ModalMatrix<position_vector> modes = modalMatrix(scale);
    ModalMatrix<position_vector> filter = filterModalMatrix(modes, notes);
    ModalMatrixDistance<position_vector> distances = calculateDistances(scale, filter);
    ModalMatrixRow<position_vector> out = distances.getByComplexity(complexity);
    return out;
}

/**
 * @brief Find best transposition (modulation) matching a set of notes
 *
 * Builds the transposition matrix for `scale`, filters rows that contain
 * `notes`, computes distances and returns the best matching transposition row.
 *
 * @param scale Input scale as position_vector
 * @param notes Vector of pitch classes used to filter transpositions
 * @param complexity Complexity index for selecting among candidates
 * @return Best matching TranspositionMatrixRow
 */
inline TranspositionMatrixRow modulationAutomation(position_vector& scale, const std::vector<int>& notes, int complexity){
    TranspositionMatrix transpositions = transpositionMatrix(scale);
    TranspositionMatrix filter = filterTranspositionMatrix(transpositions, notes);
    TranspositionMatrixDistance distances = calculateDistances(scale, filter);
    TranspositionMatrixRow out = distances.getByComplexity(complexity);
    return out;
}
/**
 * @brief Helper function to normalize complexity vector to required size
 * @param complexities Input complexity vector (can be empty, smaller, larger, or single element)
 * @param requiredSize The size the vector needs to be
 * @return Normalized complexity vector of exactly requiredSize
 * @details If empty: fills with zeros
 *          If single element: repeats that element
 *          If smaller: cycles through elements until requiredSize
 *          If larger: truncates to requiredSize
 */
inline std::vector<int> normalizeComplexityVector(const std::vector<int>& complexities, size_t requiredSize) {
    std::vector<int> result;
    result.reserve(requiredSize);
    
    if (complexities.empty()) {
        // Fill with zeros
        result.assign(requiredSize, 0);
    } else if (complexities.size() == requiredSize) {
        // Already correct size
        result = complexities;
    } else if (complexities.size() < requiredSize) {
        // Repeat elements cyclically
        for (size_t i = 0; i < requiredSize; ++i) {
            result.push_back(complexities[i % complexities.size()]);
        }
    } else {
        // Truncate to required size
        result.assign(complexities.begin(), complexities.begin() + requiredSize);
    }
    
    return result;
}

/**
 * @brief Performs voice leading with custom reference positions
 * @param targets Vector of target position_vectors
 * @param references Vector of reference position_vectors (must match targets size)
 * @param complexities Vector of complexity values (will be normalized to match targets size)
 * @return Vector of position_vectors with voice leading applied
 * @details Each target is compared against its corresponding reference position
 */
inline std::vector<position_vector> voiceLeadingAutomationVectorReference(
    std::vector<position_vector>& targets,
    std::vector<position_vector>& references,
    const std::vector<int>& complexities = std::vector<int>())
{
    if (targets.size() != references.size()) {
        throw std::runtime_error("targets and references must have the same size");
    }
    
    std::vector<int> normalizedComplexities = normalizeComplexityVector(complexities, targets.size());
    
    std::vector<position_vector> result;
    result.reserve(targets.size());
    
    for (size_t i = 0; i < targets.size(); ++i) {
        RelativeModeMatrixRow selected = voiceLeadingAutomation(references[i], targets[i], normalizedComplexities[i]);
        result.push_back(selected.getVector());
    }
    
    return result;
}

/**
 * @brief Performs voice leading with custom reference positions
 * @param targets Vector of target position_vectors
 * @param references Reference position_vector
 * @param complexities Vector of complexity values (will be normalized to match targets size)
 * @return Vector of position_vectors with voice leading applied
 * @details Each target is compared against its corresponding reference position
 */
inline std::vector<position_vector> voiceLeadingAutomationReference(
    std::vector<position_vector>& targets,
    position_vector& reference,
    const std::vector<int>& complexities = std::vector<int>())
{
    std::vector<int> normalizedComplexities = normalizeComplexityVector(complexities, targets.size());
    
    std::vector<position_vector> result;
    result.reserve(targets.size());
    
    for (size_t i = 0; i < targets.size(); ++i) {
        RelativeModeMatrixRow selected = voiceLeadingAutomation(reference, targets[i], normalizedComplexities[i]);
        result.push_back(selected.getVector());
    }
    
    return result;
}

/**
 * @brief Performs sequential voice leading automation from start to end
 * @param targets Vector of target position_vectors (first element is kept as-is)
 * @param complexities Vector of complexity values (will be normalized to targets.size() - 1)
 * @return Vector of position_vectors with sequential voice leading applied
 * @throws runtime_error if targets vector is empty
 * @details First element is unchanged. Each subsequent element is found by comparing
 *          the relative_mode_matrix of targets[i] with the result from the previous step.
 */
inline std::vector<position_vector> forwardVoiceLeading(
    const std::vector<position_vector>& targets,
    const std::vector<int>& complexities = std::vector<int>())
{
    if (targets.empty()) {
        throw std::runtime_error("targets vector cannot be empty");
    }
    
    std::vector<position_vector> result;
    result.reserve(targets.size());
    
    // First element unchanged
    result.push_back(targets[0]);
    
    if (targets.size() == 1) {
        return result;
    }
    
    std::vector<int> normalizedComplexities = normalizeComplexityVector(complexities, targets.size() - 1);
    
    // Sequential processing
    for (size_t i = 1; i < targets.size(); ++i) {
        position_vector target = targets[i]; // Copy for relative_mode_matrix
        position_vector& reference = result[i - 1]; // Previous result
        RelativeModeMatrixRow selected = voiceLeadingAutomation(reference, target, normalizedComplexities[i-1]);
        result.push_back(selected.getVector());
    }
    
    return result;
}

/**
 * @brief Performs sequential voice leading automation from end to start
 * @param targets Vector of target position_vectors (last element is kept as-is)
 * @param complexities Vector of complexity values (will be normalized to targets.size() - 1)
 * @return Vector of position_vectors with sequential voice leading applied in reverse
 * @throws runtime_error if targets vector is empty
 * @details Last element is unchanged. Each previous element is found by comparing
 *          the relative_mode_matrix of targets[i] with the result from the next step.
 */
inline std::vector<position_vector> voiceLeadingAutomationSequentialBackward(
    const std::vector<position_vector>& targets,
    const std::vector<int>& complexities = std::vector<int>())
{
    if (targets.empty()) {
        throw std::runtime_error("targets vector cannot be empty");
    }
    
    std::vector<position_vector> result(targets.size());
    
    // Last element unchanged
    result[targets.size() - 1] = targets[targets.size() - 1];
    
    if (targets.size() == 1) {
        return result;
    }
    
    std::vector<int> normalizedComplexities = normalizeComplexityVector(complexities, targets.size() - 1);
    
    // Sequential processing backward
    for (int i = targets.size() - 2; i >= 0; --i) {
        position_vector target = targets[i]; // Copy for relative_mode_matrix
        position_vector& reference = result[i + 1]; // Next result
        RelativeModeMatrixRow selected = voiceLeadingAutomation(reference, target, normalizedComplexities[i]);
        result[i] = selected.getVector();
    }
    
    return result;
}

/**
 * @brief Performs degree automation with a single reference position
 * @param scale The scale to use for modal selection
 * @param criterion The interval criterion for modal selection
 * @param degrees Vector of degree values
 * @param reference Reference position_vector for distance calculation
 * @param complexities Vector of complexity values (will be normalized to match degrees size)
 * @return Vector of position_vectors with degree automation applied
 */
inline std::vector<position_vector> degreeAutomationReference(
    position_vector& scale,
    interval_vector& criterion,
    const std::vector<int>& degrees,
    position_vector& reference,
    const std::vector<int>& complexities = std::vector<int>())
{
    std::vector<int> normalizedComplexities = normalizeComplexityVector(complexities, degrees.size());
    
    std::vector<position_vector> result;
    result.reserve(degrees.size());
    
    for (size_t i = 0; i < degrees.size(); ++i) {
        ModalRelativeModeMatrixRow selected = degreeAutomation(
            scale, criterion, degrees[i], reference, normalizedComplexities[i]);
        result.push_back(selected.getVector());
    }
    
    return result;
}

/**
 * @brief Performs degree automation with individual reference positions
 * @param scale The scale to use for modal selection
 * @param criterion The interval criterion for modal selection
 * @param degrees Vector of degree values
 * @param references Vector of reference position_vectors (must match degrees size)
 * @param complexities Vector of complexity values (will be normalized to match degrees size)
 * @return Vector of position_vectors with degree automation applied
 * @details Each degree is compared against its corresponding reference position
 */
inline std::vector<position_vector> degreeAutomationVectorReference(
    position_vector& scale,
    interval_vector& criterion,
    const std::vector<int>& degrees,
    std::vector<position_vector>& references,
    const std::vector<int>& complexities = std::vector<int>())
{
    if (degrees.size() != references.size()) {
        throw std::runtime_error("degrees and references must have the same size");
    }
    
    std::vector<int> normalizedComplexities = normalizeComplexityVector(complexities, degrees.size());
    
    std::vector<position_vector> result;
    result.reserve(degrees.size());
    
    for (size_t i = 0; i < degrees.size(); ++i) {
        ModalRelativeModeMatrixRow selected = degreeAutomation(
            scale, criterion, degrees[i], references[i], normalizedComplexities[i]);
        result.push_back(selected.getVector());
    }
    
    return result;
}

/**
 * @brief Performs sequential degree automation from start to end
 * @param scale The scale to use for modal selection
 * @param criterion The interval criterion for modal selection
 * @param degrees Vector of degree values (first degree's result is used as initial reference)
 * @param initialReference Initial reference position for the first degree
 * @param complexities Vector of complexity values (will be normalized to degrees size)
 * @return Vector of position_vectors with sequential degree automation applied
 * @throws runtime_error if degrees vector is empty
 * @details First result is calculated using initialReference. Each subsequent result
 *          uses the previous result as its reference.
 */
inline std::vector<position_vector> forwardDegreeAutomation(
    position_vector& scale,
    interval_vector& criterion,
    const std::vector<int>& degrees,
    position_vector& initialReference,
    const std::vector<int>& complexities = std::vector<int>())
{
    if (degrees.empty()) {
        throw std::runtime_error("degrees vector cannot be empty");
    }
    
    std::vector<int> normalizedComplexities = normalizeComplexityVector(complexities, degrees.size());
    
    std::vector<position_vector> result;
    result.reserve(degrees.size());
    
    // First element uses initial reference
    ModalRelativeModeMatrixRow first = degreeAutomation(
        scale, criterion, degrees[0], initialReference, normalizedComplexities[0]);
    result.push_back(first.getVector());
    
    // Sequential processing - each uses previous result as reference
    for (size_t i = 1; i < degrees.size(); ++i) {
        position_vector& reference = result[i - 1];
        ModalRelativeModeMatrixRow selected = degreeAutomation(
            scale, criterion, degrees[i], reference, normalizedComplexities[i]);
        result.push_back(selected.getVector());
    }
    
    return result;
}

/**
 * @brief Performs sequential degree automation from end to start
 * @param scale The scale to use for modal selection
 * @param criterion The interval criterion for modal selection
 * @param degrees Vector of degree values (last degree's result is used as final reference)
 * @param finalReference Final reference position for the last degree
 * @param complexities Vector of complexity values (will be normalized to degrees size)
 * @return Vector of position_vectors with sequential degree automation applied in reverse
 * @throws runtime_error if degrees vector is empty
 * @details Last result is calculated using finalReference. Each previous result
 *          uses the next result as its reference.
 */
inline std::vector<position_vector> degreeAutomationSequentialBackward(
    position_vector& scale,
    interval_vector& criterion,
    const std::vector<int>& degrees,
    position_vector& finalReference,
    const std::vector<int>& complexities = std::vector<int>())
{
    if (degrees.empty()) {
        throw std::runtime_error("degrees vector cannot be empty");
    }
    
    std::vector<int> normalizedComplexities = normalizeComplexityVector(complexities, degrees.size());
    
    std::vector<position_vector> result(degrees.size());
    
    // Last element uses final reference
    ModalRelativeModeMatrixRow last = degreeAutomation(
        scale, criterion, degrees[degrees.size() - 1], finalReference, 
        normalizedComplexities[degrees.size() - 1]);
    result[degrees.size() - 1] = last.getVector();
    
    // Sequential processing backward - each uses next result as reference
    for (int i = degrees.size() - 2; i >= 0; --i) {
        position_vector& reference = result[i + 1];
        ModalRelativeModeMatrixRow selected = degreeAutomation(
            scale, criterion, degrees[i], reference, normalizedComplexities[i]);
        result[i] = selected.getVector();
    }
    
    return result;
}

/**
 * @brief Get the maximum consecutive interval in a scale representation
 *
 * Scans the integer vector representing scale degrees (absolute positions)
 * and returns the largest gap between consecutive elements. This helper is
 * used by `autoScale` to prefer candidate mappings that minimize the largest
 * step introduced by remapping pitch-classes into scale degrees.
 *
 * @param scale Vector of absolute positions
 * @return The maximum interval between consecutive entries in `scale`
 */

inline int getMaxInterval(std::vector<int>& scale) {
    if (scale.size() < 2) {
        return 0;
    }
    int maxInterval = 0;
    for (size_t i = 1; i < scale.size(); i++) {
        int interval = std::abs(scale[i] - scale[i - 1]);
        maxInterval = std::max(maxInterval, interval);
    }
    return maxInterval;
}


/**
 * @brief Auto-adjust a scale so it fits a set of absolute MIDI notes (pitch classes)
 *
 * For each input note the function finds the closest (by pitch-class) scale degree
 * that has not yet been used and assigns the note to that degree possibly changing
 * the degree's octave number to match the supplied note. Tie-breakers aim to
 * minimise the maximum interval in the resulting scale and to prefer positions
 * closer to the scale edges when equivalent.
 *
 * This is useful to adapt a given diatonic (or other) scale so that a set of
 * sounded notes is represented within the scale with minimal distortion.
 *
 * @param scale Input scale as a `position_vector` (will not be modified)
 * @param notes Vector of absolute MIDI-like note numbers (integers). Values are reduced to pitch-classes using the scale modulus.
 * @return A new `position_vector` with adjusted scale degrees matching the supplied notes when possible
 *
 * @note The returned `position_vector` preserves the input scale's modulus, user range and flags.
 */
inline position_vector autoScale(position_vector& scale, std::vector<int>& notes) {
    std::vector<int> scaleData = scale.data();
    int mod = scale.mod();
    
    std::vector<int> pitchClasses;
    for (int note : notes) {
        int pc = note % mod;
        if (pc < 0) {
            pc += mod;
        }
        pitchClasses.push_back(pc);
    }
    
    std::vector<bool> used(scaleData.size(), false);
    
    for (int pc : pitchClasses) {
        int closest = -1;
        int minDist = mod;
        int bestMaxInterval = 999;
        
        for (int i = 0; i < scaleData.size(); i++) {
            if (used[i]) continue;
            
            int scalePc = scaleData[i] % mod;
            int upDist = (pc - scalePc + mod) % mod;
            int downDist = (scalePc - pc + mod) % mod;
            int dist = std::min(upDist, downDist);
            
            if (dist < minDist) {
                minDist = dist;
                closest = i;

                std::vector<int> tempScale = scaleData;
                int octave = tempScale[i] / mod;
                tempScale[i] = octave * mod + pc;
                bestMaxInterval = getMaxInterval(tempScale);
            } else if (dist == minDist) {

                std::vector<int> tempScale = scaleData;
                int octave = tempScale[i] / mod;
                tempScale[i] = octave * mod + pc;
                int maxInterval = getMaxInterval(tempScale);
                
                if (maxInterval < bestMaxInterval) {
                    closest = i;
                    bestMaxInterval = maxInterval;
                } else if (maxInterval == bestMaxInterval && closest != -1) {
                    int toEdge = std::min(i, (int)scaleData.size() - 1 - i);
                    int closestToEdge = std::min(closest, (int)scaleData.size() - 1 - closest);
                    if (toEdge < closestToEdge) {
                        closest = i;
                        bestMaxInterval = maxInterval;
                    }
                }
            }
        }
        
        if (closest != -1) {

            int octave = scaleData[closest] / mod;
            scaleData[closest] = octave * mod + pc;
            used[closest] = true;
        }
    }

    return position_vector(scaleData, scale.mod(), scale.user_range(),
                         scale.auto_range_updates(), scale.use_user_range());
}

} // namespace musicpp

#endif // MUSICPP_AUTOMATIONS_H