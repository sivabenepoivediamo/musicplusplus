#ifndef MUSICPP_RHYTHM_GEN_H
#define MUSICPP_RHYTHM_GEN_H

#include "vectors.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>
/**
 * @file rhythm_gen.h
 * @brief Functions for generating common rhythmic patterns and structures
 * @author [not251]
 * @date 2025
 * @details This file contains functions to generate various rhythmic patterns: 
 * - Euclidean rhythms
 * - Clough-Douthett patterns
 * - Deep rhythms
 * - Tihai patterns
 */

/**
 * @brief Generates a Euclidean rhythm pattern
 * @param steps Total number of steps in the pattern
 * @param events Number of onsets (pulses) in the pattern
 * @return Vector<int> representing the Euclidean rhythm as intervals
 * 
 */

namespace musicpp {

std::vector<int> euclidean(int steps, int events) {
    if (steps <= 0 || events <= 0) {
        throw std::invalid_argument("euclidean: steps and events must be positive");
    }
    std::vector<int> out;
    out.reserve(events);
    division_result div = euclidean_division(steps, events);
    if (div.remainder == 0) {
            for (int i = 0; i < events; i++) {
            out.emplace_back(div.quotient);
        }
    } else {
        int a = div.remainder;
        std::vector<int> x = euclidean(events, a);
        for (int i = 0; i < a; i++) {
            for (int j = 0; j < x[i] - 1; j++) {
                out.emplace_back(div.quotient);
            }
            out.emplace_back(div.quotient + 1);
        }
    }

    return out;
}

/**
 * @brief Generates a Clough-Douthett rhythm pattern
 * @param steps Total number of steps in the pattern
 * @param events Number of onsets (pulses) in the pattern
 * @return Vector<int> representing the Clough-Douthett rhythm as positions
 * 
 */
std::vector<int> CloughDouthett(int steps, int events) {
    std::vector<int> out;
    out.reserve(events);
    for (int i = 0; i < events; i++) {
        out.emplace_back(static_cast<int>(std::floor(i * steps / static_cast<double>(events))));
    }
    return out;
}

/**
 * @brief Generates a deep rhythm pattern
 * @param steps Total number of steps in the pattern
 * @param events Number of onsets (pulses) in the pattern
 * @param multiplicity Multiplicity factor for onset distribution
 * @return Vector<int> representing the deep rhythm as positions
 * @details The multiplicity factor determines how onsets are spaced within the total steps.
 */
std::vector<int> deepRhythm(int steps, int events, int multiplicity) {
    std::vector<int> out;
    out.reserve(events);
    for (int i = 0; i < events; i++) {
        out.emplace_back((i * multiplicity) % steps);
    }
    std::sort(out.begin(), out.end());

    return out;
}

/**
 * @brief Generates a Clough-Douthett position_vector
 * @param steps Total number of steps in the pattern
 * @param events Number of onsets (pulses) in the pattern
 * @param offset Offset to apply to all positions
 * @return position_vector representing the Clough-Douthett rhythm
 * @details The offset shifts all positions by the specified amount.
 */
position_vector CloughDouthettVector(int steps, int events, int offset) {
    std::vector<int> data = CloughDouthett(steps, events);
    position_vector pv = position_vector(data, steps);
    pv = pv + offset;
    return pv;
}

/**
 * @brief Generates a Euclidean interval_vector
 * @param steps Total number of steps in the pattern
 *  @param events Number of onsets (pulses) in the pattern
 * @param offset Offset to apply to the interval vector
 * @return interval_vector representing the Euclidean rhythm
 * @details The offset shifts the starting point of the interval vector.
 * 
 */
interval_vector euclidean (int steps, int events, int offset) {
    std::vector<int> data = euclidean(steps, events);
    return interval_vector(data, offset, steps);
}

/**
 * @brief Generates a deep rhythm position_vector
 * @param steps Total number of steps in the pattern
 * @param events Number of onsets (pulses) in the pattern
 * @param multiplicity Multiplicity factor for onset distribution
 * @param offset Offset to apply to all positions
 * @return position_vector representing the deep rhythm
 * @details The multiplicity factor determines how onsets are spaced within the total steps.
 */
position_vector deepRhythm(int steps, int events, int multiplicity, int offset) {
    std::vector<int> data = deepRhythm(steps, events, multiplicity);
    position_vector pv(data, steps);
    pv = pv + offset;
    return pv;
}

/**
 * @brief Calculates the length of a rhythmic phrase
 * @param e Phrase endpoint
 * @param c Number of full cycles (times the phrase crosses its start)
 * @param n Number of steps per cycle
 * @param s Phrase starting point
 * @param l Subdivision density
 * @return Total length of the phrase
 * @details The formula used is: length = (e + c * n - s) * l
 */
int phraseLength(int e, int c, int n, int s, int l) {
    int length = (e + c * n - s) * l;
    return length;
}

/**
 * @brief Generates parameters for a Tihai pattern
 * @param steps Total number of steps in the pattern
 * @param repetitions Number of repetitions of the pattern
 * @return Pair<int, int> representing (bols, dams)
 * @details The function calculates the number of bols (onsets) and dams (silences)
 *          needed to construct a Tihai pattern that fits within the specified steps
 *         and repetitions.
 *         The bols and dams are calculated to ensure the pattern can be evenly repeated.
 *         The length is adjusted to be divisible by the number of repetitions.
 *        The resulting bols and dams can be used to construct the Tihai pattern.
 *       The bols represent the number of onsets, while the dams represent the number of silences.
 * 
 */
std::pair<int, int> tihaiGenerator(int steps, int repetitions) {
    int length = steps;
    while (length % repetitions != 0) {
        length++;
    }
    const int dams = length - steps;
    const int bols = length / repetitions - dams;
    return {bols, dams};
}

/**
 * @brief Generates a Tihai rhythm pattern
 * @param steps Total number of steps in the pattern
 * @param repetitions Number of repetitions of the pattern
 * @param pseudo If true, adjusts pattern to avoid all-silence or all-onset cases
 * @return Vector<int> representing the Tihai rhythm as positions
 * @details The function constructs a Tihai pattern based on the specified parameters.
 *          It uses the tihaiGenerator to determine the number of bols (onsets) and dams (silences).
 *          The resulting pattern consists of a sequence of bols followed by dams, repeated as specified.
 *          If the resulting pattern is all silences or all onsets and pseudo is true,
 *          it generates a shorter Tihai pattern and pads it with onsets to maintain the original length.
 */
std::vector<int> tihaiReader(int b, int d, int m, int steps) {
    std::vector<int> out;
    out.reserve(steps);
    
    for (int i = 0; i < b; i++) out.emplace_back(1);
    for (int i = 0; i < d; i++) out.emplace_back(0);
    
    for (int i = 0; i < m - 2; i++) {
        for (int j = 0; j < b; j++) out.emplace_back(1);
        for (int j = 0; j < d; j++) out.emplace_back(0);
    }
    
    for (int i = 0; i < b; i++) out.emplace_back(1);
    
    return out;
}

/**
 * @brief Checks if a vector contains only zeros
 * @param vec Input vector to check
 * @return true if all elements are zero, false otherwise
 */
bool isAllZeros(const std::vector<int>& vec) {
    return std::all_of(vec.begin(), vec.end(), [](int x) { return x == 0; });
}

/**
 * @brief Checks if a vector contains only ones
 * @param vec Input vector to check
 * @return true if all elements are one, false otherwise
 */
bool isAllOnes(const std::vector<int>& vec) {
    return std::all_of(vec.begin(), vec.end(), [](int x) { return x == 1; });
}
/**
 * @brief Appends ones to a vector until it reaches the target size
 * @param vec Input vector to modify    
 * @param targetSize Desired size of the vector
 * @details The function adds 1s to the end of the vector until its size matches targetSize.
 *         If the vector is already at or above the target size, no changes are made.
 * 
 */
void appendOnes(std::vector<int>& vec, int targetSize) {
    while (static_cast<int>(vec.size()) < targetSize) {
        vec.emplace_back(1);
    }
}
/**
 * @brief Cuts a vector to a specified length
 * @param vec Input vector to cut
 * @param length Desired length of the output vector
 * @return New vector containing the first 'length' elements of the input vector
 * @details If the specified length is greater than or equal to the input vector's size,
 *          the original vector is returned unchanged.
 */

std::vector<int> cut(const std::vector<int>& vec, int length) {
    if (length >= static_cast<int>(vec.size())) {
        return vec;
    }
    return std::vector<int>(vec.begin(), vec.begin() + length);
}

/**
 * @brief Generates a Tihai rhythm pattern
 * @param steps Total number of steps in the pattern
 * @param repetitions Number of repetitions of the pattern
 * @param pseudo If true, recursively adjusts pattern to avoid all-silence or all-onset cases
 * @return Vector<int> representing the Tihai rhythm as positions
 */
std::vector<int> tihai(int steps, int repetitions, bool pseudo) {
    if (steps <= 2) {
        return std::vector<int>(steps, 1);
    }
    if (repetitions == 1) {
        return std::vector<int>(steps, 1);
    } else if (repetitions <= 0) {
        return std::vector<int>(steps, 0);
    } else {
        auto [bols, dams] = tihaiGenerator(steps, repetitions);
        std::vector<int> pattern = tihaiReader(bols, dams, repetitions, steps);
        
        if (((isAllZeros(pattern) && pseudo) || isAllOnes(pattern)) && pseudo) {
            std::vector<int> shorterPattern = tihai(steps - 1, repetitions, pseudo);
            appendOnes(shorterPattern, steps);
            return shorterPattern;
        } else {
            return cut(pattern, steps);
        }
    }
}

/**
 * @brief Generates a Tihai rhythm as an onset_vector
 * @param steps Total number of steps in the pattern
 * @param repetitions Number of repetitions of the pattern
 * @param pseudo If true, recursively adjusts pattern to avoid all-silence or all-onset cases
 * @param offset Offset to apply to the onset_vector
 * @return onset_vector representing the Tihai rhythm
 */
onset_vector tihai(int steps, int repetitions, bool pseudo, int offset) {
    std::vector<int> pattern = tihai(steps, repetitions, pseudo);
    return onset_vector(pattern, offset, steps);
}

} // namespace musicpp

#endif // MUSICPP_RHYTHM_GEN_H

