#ifndef DISTANCES_H
#define DISTANCES_H

#include "./vectors.h"

/**
 * @file distances.h
 * @brief Functions for calculating distances and transformations between vectors
 * @author [not251]
 * @date 2025
 */

/**
 * @brief Normalizes a vector of integers to a probability distribution
 * @param in Input vector of integers
 * @return Vector of doubles representing the normalized probabilities
 * @throw invalid_argument if the sum of the input vector is zero
 */
vector<double> normalize(vector<int>& in) {
    double sum = accumulate(in.begin(), in.end(), 0.0);
    if (sum == 0) {
        throw invalid_argument("Sum of vector elements is zero, cannot normalize");
    }

    vector<double> out(in.size());
    transform(in.begin(), in.end(), in.begin(), [sum](int val) {
        return val / sum;
    });

    return out;
}

/**
 * @brief Computes the cumulative distribution function (CDF) from a probability density function (PDF)
 * @param pdf Input vector representing the PDF (should sum to 1)
 *  @return Vector representing the CDF
 */
vector<double> computeCDF(vector<double>& pdf) {
    vector<double> cdf(pdf.size());
    partial_sum(pdf.begin(), pdf.end(), cdf.begin());
    return cdf;
}
/**
 * @brief Calculates the Euclidean distance between two vectors of integers
 * @param v1 First input vector
 * @param v2 Second input vector
 * @return Euclidean distance as a double
 */
double euclideanDistance(vector<int> v1, vector<int> v2) {
    int length = min(v1.size(), v2.size());
    
    double out = 0.0;
    for (int i = 0; i < length; ++i) {
        double diff = v1[i] - v2[i];
        out += diff * diff;
    }
    
    return sqrt(out);
}
/**
 * @brief Calculates the Levenshtein edit distance between two vectors of integers
 * @param v1 First input vector
 * @param v2 Second input vector
 * @return Edit distance as an integer
 */
int editDistance(vector<int>& v1, vector<int>& v2) {
    int n = v1.size();
    int m = v2.size();
    vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));

    for (int i = 0; i <= n; ++i) dp[i][0] = i;
    for (int j = 0; j <= m; ++j) dp[0][j] = j;

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (v1[i - 1] == v2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
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
int hammingDistance(vector<int>& v1, vector<int>& v2) {
    int length = min(v1.size(), v2.size());
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
int manhattanDistance(vector<int>& v1, vector<int>& v2){
    int length = min(v1.size(), v2.size());
    int sum = 0;
    for (size_t i = 0; i < length; ++i){
        sum += abs(v1[i]-v2[i]);
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
int difference(vector<int>& v1, vector<int>& v2) {
    int length = min(v1.size(), v2.size());
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
double variationDistance(vector<int> a, vector<int> b) {
    if (a.empty() && b.empty()) return 0.0;
    if (a.empty() || b.empty())
        throw invalid_argument("Empty vector, distance calculation is impossible.");

    if (!is_sorted(a.begin(), a.end())) sort(a.begin(), a.end());
    if (!is_sorted(b.begin(), b.end())) sort(b.begin(), b.end());

    const size_t minLen = min(a.size(), b.size());
    const size_t maxLen = max(a.size(), b.size());
    double total = 0.0;

    for (size_t i = 0; i < minLen; ++i)
        total += abs(a[i] - b[i]);

    const vector<int>& longer  = (a.size() >= b.size()) ? a : b;
    const vector<int>& shorter = (a.size() >= b.size()) ? b : a;

    for (size_t i = minLen; i < maxLen; ++i) {
        int extra   = longer[i];
        int minDist = INT_MAX;
        int maxDist = INT_MIN;
        for (int x : shorter) {
            int d   = abs(extra - x);
            minDist = min(minDist, d);
            maxDist = max(maxDist, d);
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
vector<int> generalizedNeoRiemann(const vector<int>& input, int position, int shift) {
    vector<int> output = input;
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
vector<pair<int, pair<int, int>>> transformationSteps(const vector<int>& start, const vector<int>& end) {
    vector<pair<int, pair<int, int>>> steps;
    int startLength = start.size();
    int endLength = end.size();
    int minLength = min(startLength, endLength);
    int addedPosition = startLength;
    
    for (int i = 0; i < minLength; ++i) {
        int diff = end[i] - start[i];
        if (diff != 0) {
            steps.push_back({0, {i, diff}});
            vector<int> transformed = generalizedNeoRiemann(start, i, diff);
            vector<pair<int, pair<int, int>>> substeps = transformationSteps(transformed, end);
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
void printSteps(const vector<pair<int, pair<int, int>>>& steps) {
    for (const auto& step : steps) {
        int type = step.first;
        int position = step.second.first;
        int value = step.second.second;
        
        cout << "position: " << position << ", ";
        if (type == 0) {
            cout << "shift: " << value << endl;
        } else if (type == 1) {
            cout << "added: " << value << endl;
        } else if (type == 2) {
            cout << "removed: " << value << endl;
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
int weightedTransformationDistance(vector<int>& start, vector<int>& end) {
    vector<pair<int, pair<int, int>>> steps = transformationSteps(start, end);
    int distance = 0;
    for (const auto& step : steps) {
        int weight = abs(step.second.second);
        distance += weight;
    }
    return distance;
}

// Overloaded functions for PositionVector and IntervalVector

/**
 * @brief Overloaded distance functions for PositionVector and IntervalVector
 * @details These functions extract the underlying data vectors and call the corresponding distance functions.
 */

double euclideanDistance(PositionVector a, PositionVector b){
    return euclideanDistance(a.data, b.data);
}
int manhattanDistance(PositionVector a, PositionVector b){
    return manhattanDistance(a.data, b.data);
}

int editDistance(PositionVector a, PositionVector b){
    return editDistance(a.data, b.data);
}

int weightedTransformationDistance(PositionVector a, PositionVector b){
    return weightedTransformationDistance(a.data, b.data);
}

int difference(PositionVector a, PositionVector b){
    return difference(a.data, b.data);
}

int hammingDistance(PositionVector a, PositionVector b){
    return hammingDistance(a.data, b.data);
}

int difference(IntervalVector a, IntervalVector b){
    return difference(a.data, b.data);
}
int hammingDistance(IntervalVector a, IntervalVector b){
    return hammingDistance(a.data, b.data);
}
int manhattanDistance(IntervalVector a, IntervalVector b){
    return manhattanDistance(a.data, b.data);
}
double euclideanDistance(IntervalVector a, IntervalVector b){
    return euclideanDistance(a.data, b.data);
}
int editDistance(IntervalVector a, IntervalVector b){
    return editDistance(a.data, b.data);
}
int weightedTransformationDistance(IntervalVector a, IntervalVector b){
    return weightedTransformationDistance(a.data, b.data);
}
double variationDistance(PositionVector a, PositionVector b){
    return variationDistance(a.data, b.data);
}




#endif
