#ifndef MUSICPP_DISTANCES_H
#define MUSICPP_DISTANCES_H

#include "vectors.h"

#include <algorithm>
#include <cmath>
#include <climits>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

/**
 * @file distances.h
 * @brief Functions for calculating distances and transformations between vectors
 * @author [not251]
 * @date 2025
 */

/**
 * @brief Normalizes a vector of integers to a probability distribution
 * @param in Input vector of integers (read-only; not modified)
 * @return Vector of doubles representing the normalized probabilities
 * @throw invalid_argument if the sum of the input vector is zero
 */

namespace musicpp {

inline std::vector<double> normalize(const std::vector<int>& in) {
    double sum = std::accumulate(in.begin(), in.end(), 0.0);
    if (sum == 0) {
        throw std::invalid_argument("Sum of vector elements is zero, cannot normalize");
    }

    std::vector<double> out(in.size());
    std::transform(in.begin(), in.end(), out.begin(), [sum](int val) {
        return static_cast<double>(val) / sum;
    });

    return out;
}

/**
 * @brief Computes the cumulative distribution function (CDF) from a probability density function (PDF)
 * @param pdf Input vector representing the PDF (should sum to 1)
 *  @return Vector representing the CDF
 */
inline std::vector<double> computeCDF(std::vector<double>& pdf) {
    std::vector<double> cdf(pdf.size());
    std::partial_sum(pdf.begin(), pdf.end(), cdf.begin());
    return cdf;
}
/**
 * @brief Calculates the Euclidean distance between two vectors of integers
 * @param v1 First input vector
 * @param v2 Second input vector
 * @return Euclidean distance as a double
 */
inline double euclideanDistance(std::vector<int> v1, std::vector<int> v2) {
    int length = std::min(v1.size(), v2.size());
    
    double out = 0.0;
    for (int i = 0; i < length; ++i) {
        double diff = v1[i] - v2[i];
        out += diff * diff;
    }
    
    return std::sqrt(out);
}
/**
 * @brief Calculates the Levenshtein edit distance between two vectors of integers
 * @param v1 First input vector
 * @param v2 Second input vector
 * @return Edit distance as an integer
 */
inline int editDistance(std::vector<int>& v1, std::vector<int>& v2) {
    int n = v1.size();
    int m = v2.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));

    for (int i = 0; i <= n; ++i) dp[i][0] = i;
    for (int j = 0; j <= m; ++j) dp[0][j] = j;

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (v1[i - 1] == v2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
            }
        }
    }
    return dp[n][m];
}

/**
 * @brief Calculates the Hamming distance between two vectors of integers
 * @param v1 First input vector
 * @param v2 Second input vector
 * @return Hamming distance as an integer
 * @details The Hamming distance is the number of positions at which the corresponding elements are different.
 *          If the vectors are of different lengths, the comparison is done up to the length of the shorter vector.
 */
inline int hammingDistance(std::vector<int>& v1, std::vector<int>& v2) {
    int length = std::min(v1.size(), v2.size());
    int distance = 0;
    for (size_t i = 0; i < length; ++i) {
        if (v1[i] != v2[i]) {
            distance++;
        }
    }
    return distance;
}
/**
 * @brief Calculates the Manhattan (L1) distance between two vectors of integers
 * @param v1 First input vector
 * @param v2 Second input vector
 * @return Manhattan distance as an integer
 * @details The Manhattan distance is the sum of the absolute differences of their corresponding elements.
 *          If the vectors are of different lengths, the comparison is done up to the length of the shorter vector.
 */
inline int manhattanDistance(std::vector<int>& v1, std::vector<int>& v2){
    int length = std::min(v1.size(), v2.size());
    int sum = 0;
    for (size_t i = 0; i < length; ++i){
        sum += std::abs(v1[i]-v2[i]);
    }
    return sum;
}
/**
 * @brief Calculates the difference between two vectors of integers
 * @param v1 First input vector
 * @param v2 Second input vector
 * @return Difference as an integer (sum of element-wise differences)
 * @details The difference is calculated as the sum of (v1[i] - v2[i]) for each corresponding element.
 *          If the vectors are of different lengths, the comparison is done up to the length of the shorter vector.
 */
inline int difference(std::vector<int>& v1, std::vector<int>& v2) {
    int length = std::min(v1.size(), v2.size());
    int diff = 0;
    for (size_t i = 0; i < length; ++i) {
        diff += v1[i] - v2[i];
    }
    return diff;
}

/**
 * @brief Computes the total distance between two integer vectors.
 *
 * The vectors do not need to be sorted: if either is unsorted, a local
 * sorted copy is made without modifying the originals.
 *
 * Distance is computed in two parts:
 * - **Common part** (indices 0..minLen-1): element-wise absolute difference.
 * - **Extra elements** (tail of the longer vector): for each extra element,
 *   the average of its minimum and maximum distance from all elements
 *   of the shorter vector is added to the total.
 *
 * @param a First integer vector.
 * @param b Second integer vector.
 * @return  Total distance as a @c double.
 *
 * @throws std::invalid_argument If exactly one of the two vectors is empty.
 */
inline double variationDistance(std::vector<int> a, std::vector<int> b) {
    if (a.empty() && b.empty()) return 0.0;
    if (a.empty() || b.empty())
        throw std::invalid_argument("Empty vector, distance calculation is impossible.");

    if (!std::is_sorted(a.begin(), a.end())) std::sort(a.begin(), a.end());
    if (!std::is_sorted(b.begin(), b.end())) std::sort(b.begin(), b.end());

    const size_t minLen = std::min(a.size(), b.size());
    const size_t maxLen = std::max(a.size(), b.size());
    double total = 0.0;

    for (size_t i = 0; i < minLen; ++i)
        total += std::abs(a[i] - b[i]);

    const std::vector<int>& longer  = (a.size() >= b.size()) ? a : b;
    const std::vector<int>& shorter = (a.size() >= b.size()) ? b : a;

    for (size_t i = minLen; i < maxLen; ++i) {
        int extra   = longer[i];
        int minDist = INT_MAX;
        int maxDist = INT_MIN;
        for (int x : shorter) {
            int d   = std::abs(extra - x);
            minDist = std::min(minDist, d);
            maxDist = std::max(maxDist, d);
        }
        total += (minDist + maxDist) / 2.0;
    }

    return total;
}


/**
 * @brief Applies a generalized Neo-Riemannian transformation to a vector of integers
 * @param input Input vector of integers
 * @param position Position in the vector to apply the transformation
 * @param shift Amount to shift the element at the specified position
 * @return New vector with the transformation applied
 * @details The transformation modifies the element at the specified position by adding the shift value.
 */
inline std::vector<int> generalizedNeoRiemann(const std::vector<int>& input, int position, int shift) {
    std::vector<int> output = input;
    if (position >= 0 && position < static_cast<int>(input.size())) {
        output[position] += shift;
    }
    return output;
}

/**
 * @brief Computes the sequence of transformation steps to convert one vector into another
 * @param start Starting vector
 * @param end Target vector
 * @return Vector of transformation steps, each represented as a pair:
 *        - First element: type of operation (0 = shift, 1 = add, 2 = remove)
 *       - Second element: pair of (position, value)
 * @details The function identifies the minimal set of operations needed to transform the start vector into the end vector.
 *          It handles element shifts, additions, and removals.
 */
inline std::vector<std::pair<int, std::pair<int, int>>> transformationSteps(const std::vector<int>& start, const std::vector<int>& end) {
    std::vector<std::pair<int, std::pair<int, int>>> steps;
    int startLength = start.size();
    int endLength = end.size();
    int minLength = std::min(startLength, endLength);
    int addedPosition = startLength;
    
    for (int i = 0; i < minLength; ++i) {
        int diff = end[i] - start[i];
        if (diff != 0) {
            steps.push_back({0, {i, diff}});
            std::vector<int> transformed = generalizedNeoRiemann(start, i, diff);
            std::vector<std::pair<int, std::pair<int, int>>> substeps = transformationSteps(transformed, end);
            steps.insert(steps.end(), substeps.begin(), substeps.end());
            return steps;
        }
    }
    
    if (endLength > startLength) {
        for (int i = minLength; i < endLength; ++i) {
            steps.push_back({1, {addedPosition, end[i]}});
            addedPosition++;
        }
    }
    
    if (endLength < startLength) {
        for (int i = minLength; i < startLength; ++i) {
            steps.push_back({2, {i, start[i]}});
        }
    }
    
    return steps;
}
// Print transformation steps
inline void printSteps(const std::vector<std::pair<int, std::pair<int, int>>>& steps) {
    for (const auto& step : steps) {
        int type = step.first;
        int position = step.second.first;
        int value = step.second.second;
        
        std::cout << "position: " << position << ", ";
        if (type == 0) {
            std::cout << "shift: " << value << std::endl;
        } else if (type == 1) {
            std::cout << "added: " << value << std::endl;
        } else if (type == 2) {
            std::cout << "removed: " << value << std::endl;
        }
    }
}

/**
 * @brief Calculates the weighted transformation distance between two vectors of integers
 * @param start Starting vector
 * @param end Target vector
 * @return Weighted transformation distance as an integer
 * @details The distance is calculated as the sum of the absolute values of the shifts applied during the transformation.
 */
inline int weightedTransformationDistance(std::vector<int>& start, std::vector<int>& end) {
    std::vector<std::pair<int, std::pair<int, int>>> steps = transformationSteps(start, end);
    int distance = 0;
    for (const auto& step : steps) {
        int weight = std::abs(step.second.second);
        distance += weight;
    }
    return distance;
}

// Overloaded functions for position_vector and interval_vector

/**
 * @brief Overloaded distance functions for position_vector and interval_vector
 * @details These functions extract the underlying data vectors and call the corresponding distance functions.
 */

inline double euclideanDistance(position_vector a, position_vector b){
    return euclideanDistance(a.data(), b.data());
}
inline int manhattanDistance(position_vector a, position_vector b){
    return manhattanDistance(a.data(), b.data());
}

inline int editDistance(position_vector a, position_vector b){
    return editDistance(a.data(), b.data());
}

inline int weightedTransformationDistance(position_vector a, position_vector b){
    return weightedTransformationDistance(a.data(), b.data());
}

inline int difference(position_vector a, position_vector b){
    return difference(a.data(), b.data());
}

inline int hammingDistance(position_vector a, position_vector b){
    return hammingDistance(a.data(), b.data());
}

inline int difference(interval_vector a, interval_vector b){
    return difference(a.data(), b.data());
}
inline int hammingDistance(interval_vector a, interval_vector b){
    return hammingDistance(a.data(), b.data());
}
inline int manhattanDistance(interval_vector a, interval_vector b){
    return manhattanDistance(a.data(), b.data());
}
inline double euclideanDistance(interval_vector a, interval_vector b){
    return euclideanDistance(a.data(), b.data());
}
inline int editDistance(interval_vector a, interval_vector b){
    return editDistance(a.data(), b.data());
}
inline int weightedTransformationDistance(interval_vector a, interval_vector b){
    return weightedTransformationDistance(a.data(), b.data());
}
inline double variationDistance(position_vector a, position_vector b){
    return variationDistance(a.data(), b.data());
}




} // namespace musicpp

#endif
