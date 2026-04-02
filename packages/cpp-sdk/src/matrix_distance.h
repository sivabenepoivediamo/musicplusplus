#ifndef MUSICPP_MATRIX_DISTANCE_H
#define MUSICPP_MATRIX_DISTANCE_H

#include "matrix.h"
#include "distances.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

/**
 * @file matrix_distance.h
 * @brief Classes and functions for calculating distances between vectors and matrices
 * @author [not251]
 * @date 2025
 * @details This file contains classes and functions to compute distances between a reference vector
 *          and all rows in various matrix types, storing the results with distance metrics and
 *          providing sorting capabilities.
 */

namespace musicpp {

/**
 * @brief Row wrapper for ModalMatrixDistance queries
 * @tparam T Vector type (interval_vector or position_vector)
 */
template<typename T>
class ModalMatrixRow {
private:
    T vector_;
    int index_;
    double distance_;

public:
    ModalMatrixRow(const T& vec, int idx, double dist)
        : vector_(vec), index_(idx), distance_(dist) {}
    
    // Construct from tuple
    ModalMatrixRow(const std::tuple<T, int, double>& t)
        : vector_(std::get<0>(t)), index_(std::get<1>(t)), distance_(std::get<2>(t)) {}
    
    // Accessors
    const T& getVector() const { return vector_; }
    int getIndex() const { return index_; }
    double getDistance() const { return distance_; }
    
    // Convenience - get mutable vector
    T& getVector() { return vector_; }
    
    // Convert back to tuple if needed
    std::tuple<T, int, double> toTuple() const {
        return std::make_tuple(vector_, index_, distance_);
    }
    
    // String representation
    std::string toString() const {
        std::ostringstream oss;
        oss << "(Mode = " << index_ 
            << ", Distance = " << distance_ 
            << ", Vector = " << vector_ << ")";
        return oss.str();
    }
    
    friend std::ostream& operator<<(std::ostream& os, const ModalMatrixRow& r) {
        return os << r.toString();
    }
};

// ==================== TRANSPOSITION MATRIX RESULT ====================

/**
 * @brief Row wrapper for TranspositionMatrixDistance queries
 */
class TranspositionMatrixRow {
private:
    position_vector vector_;
    int transposition_;
    double distance_;

public:
    TranspositionMatrixRow(const position_vector& vec, int trans, double dist)
        : vector_(vec), transposition_(trans), distance_(dist) {}
    
    // Construct from tuple
    explicit TranspositionMatrixRow(const std::tuple<position_vector, int, double>& t)
        : vector_(std::get<0>(t)), transposition_(std::get<1>(t)), distance_(std::get<2>(t)) {}
    
    // Accessors
    const position_vector& getVector() const { return vector_; }
    int getTransposition() const { return transposition_; }
    double getDistance() const { return distance_; }
    
    position_vector& getVector() { return vector_; }
    
    std::tuple<position_vector, int, double> toTuple() const {
        return std::make_tuple(vector_, transposition_, distance_);
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "(Transposition = " << transposition_ 
            << ", Distance = " << distance_ 
            << ", Vector = " << vector_ << ")";
        return oss.str();
    }
    
    friend std::ostream& operator<<(std::ostream& os, const TranspositionMatrixRow& r) {
        return os << r.toString();
    }
};

// ==================== RELATIVE MODE MATRIX RESULT ====================

/**
 * @brief Row wrapper for RelativeModeMatrixDistance queries
 */
class RelativeModeMatrixRow {
private:
    position_vector vector_;
    int relative_mode_offset_;
    double distance_;
    int center_;

public:
    RelativeModeMatrixRow(const position_vector& vec, int trans, double dist, int center)
        : vector_(vec), relative_mode_offset_(trans), distance_(dist), center_(center) {}
    
    // Construct from tuple + center
    RelativeModeMatrixRow(const std::tuple<position_vector, int, double>& t, int center)
        : vector_(std::get<0>(t)), relative_mode_offset_(std::get<1>(t)), distance_(std::get<2>(t)), center_(center) {}
    
    // Accessors
    const position_vector& getVector() const { return vector_; }
    int get_relative_mode_offset() const { return relative_mode_offset_; }
    double getDistance() const { return distance_; }
    int getCenter() const { return center_; }
    
    position_vector& getVector() { return vector_; }
    
    std::tuple<position_vector, int, double> toTuple() const {
        return std::make_tuple(vector_, relative_mode_offset_, distance_);
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "(Relative mode offset = " << relative_mode_offset_ 
            << ", Center = " << center_
            << ", Distance = " << distance_ 
            << ", Vector = " << vector_ << ")";
        return oss.str();
    }
    
    friend std::ostream& operator<<(std::ostream& os, const RelativeModeMatrixRow& r) {
        return os << r.toString();
    }
};

// ==================== MODAL SELECTION MATRIX RESULT ====================

/**
 * @brief Row wrapper for ModalSelectionMatrixDistance queries
 * @tparam T Vector type (interval_vector or position_vector)
 */
template<typename T>
class ModalSelectionMatrixRow {
private:
    T chord_;
    int modeIndex_;
    double distance_;

public:
    ModalSelectionMatrixRow(const T& chord, int mode, double dist)
        : chord_(chord), modeIndex_(mode), distance_(dist) {}
    
    // Construct from tuple
    explicit ModalSelectionMatrixRow(const std::tuple<T, int, double>& t)
        : chord_(std::get<0>(t)), modeIndex_(std::get<1>(t)), distance_(std::get<2>(t)) {}
    
    // Accessors
    const T& getChord() const { return chord_; }
    int getModeIndex() const { return modeIndex_; }
    double getDistance() const { return distance_; }
    
    T& getChord() { return chord_; }
    
    std::tuple<T, int, double> toTuple() const {
        return std::make_tuple(chord_, modeIndex_, distance_);
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "(Degree = " << modeIndex_ 
            << ", Distance = " << distance_ 
            << ", Vector = " << chord_ << ")";
        return oss.str();
    }
    
    friend std::ostream& operator<<(std::ostream& os, const ModalSelectionMatrixRow& r) {
        return os << r.toString();
    }
};

// ==================== MODAL RELATIVE MODE MATRIX RESULT ====================

/**
 * @brief Row wrapper for ModalRelativeModeMatrixDistance queries
 */
class ModalRelativeModeMatrixRow {
private:
    int modeIndex_;
    int relative_mode_index_;
    position_vector vector_;
    double distance_;

public:
    ModalRelativeModeMatrixRow(int mode, int trans, const position_vector& vec, double dist)
        : modeIndex_(mode), relative_mode_index_(trans), vector_(vec), distance_(dist) {}
    
    // Construct from tuple
    explicit ModalRelativeModeMatrixRow(const std::tuple<int, int, position_vector, double>& t)
        : modeIndex_(std::get<0>(t)), relative_mode_index_(std::get<1>(t)), 
          vector_(std::get<2>(t)), distance_(std::get<3>(t)) {}
    
    // Accessors
    int getModeIndex() const { return modeIndex_; }
    int get_relative_mode_index() const { return relative_mode_index_; }
    const position_vector& getVector() const { return vector_; }
    double getDistance() const { return distance_; }
    
    position_vector& getVector() { return vector_; }
    
    std::tuple<int, int, position_vector, double> toTuple() const {
        return std::make_tuple(modeIndex_, relative_mode_index_, vector_, distance_);
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "(Degree = " << modeIndex_ 
            << ", Relative mode = " << relative_mode_index_
            << ", Distance = " << distance_ 
            << ", Vector = " << vector_ << ")";
        return oss.str();
    }
    
    friend std::ostream& operator<<(std::ostream& os, const ModalRelativeModeMatrixRow& r) {
        return os << r.toString();
    }
};

// ==================== DISTANCE MATRIX CLASSES ====================

/**
 * @brief Class representing a modal matrix with distance metrics
 * @tparam T Type of the vector (interval_vector or position_vector)
 */
template<typename T>
class ModalMatrixDistance {
private:
    std::vector<std::tuple<T, int, double>> data_; // (vector, index, distance)

public:
    ModalMatrixDistance() = default;
    
    explicit ModalMatrixDistance(const std::vector<std::tuple<T, int, double>>& data) : data_(data) {}
    
    // Access methods
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    std::tuple<T, int, double>& operator[](size_t i) { return data_[i]; }
    const std::tuple<T, int, double>& operator[](size_t i) const { return data_[i]; }
    
    std::tuple<T, int, double>& at(size_t i) { return data_.at(i); }
    const std::tuple<T, int, double>& at(size_t i) const { return data_.at(i); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Get the underlying data
    const std::vector<std::tuple<T, int, double>>& getData() const { return data_; }
    
    // Sort by distance (ascending)
    void sortByDistance() {
        sort(data_.begin(), data_.end(), 
            [](const auto& a, const auto& b) {
                return std::get<2>(a) < std::get<2>(b);
            });
    }
    
    // Get only the vectors
    std::vector<T> getVectors() const {
        std::vector<T> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<0>(row));
        }
        return result;
    }
    
    // Get only the indices
    std::vector<int> getIndices() const {
        std::vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<1>(row));
        }
        return result;
    }
    
    // Get only the distances
    std::vector<double> getDistances() const {
        std::vector<double> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<2>(row));
        }
        return result;
    }
    
    // Get the closest match
    std::tuple<T, int, double> getClosest() const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot get closest from empty matrix");
        }
        return *min_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return std::get<2>(a) < std::get<2>(b);
            });
    }
    
    // Get the furthest match
    std::tuple<T, int, double> getFurthest() const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot get furthest from empty matrix");
        }
        return *max_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return std::get<2>(a) < std::get<2>(b);
            });
    }
    
    /**
     * @brief Get a match by complexity factor
     * @param complexity Complexity factor (0-100), where 0 = closest, 100 = farthest
     * @return ModalMatrixRow at the specified complexity level
     * @throws runtime_error if matrix is empty or complexity is out of range
     */
    ModalMatrixRow<T> getByComplexity(int complexity = 0) const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot get by complexity from empty matrix");
        }
        if (complexity < 0 || complexity > 100) {
            throw std::runtime_error("Complexity must be between 0 and 100");
        }
        
        // Map complexity to index: 0 -> 0, 100 -> size-1
        size_t index = static_cast<size_t>((complexity / 100.0) * (data_.size() - 1));
        return ModalMatrixRow<T>(data_[index]);
    }
};

/**
 * @brief Class representing a transposition matrix with distance metrics
 */
class TranspositionMatrixDistance {
private:
    std::vector<std::tuple<position_vector, int, double>> data_; // (vector, transposition, distance)

public:
    TranspositionMatrixDistance() = default;
    
    explicit TranspositionMatrixDistance(const std::vector<std::tuple<position_vector, int, double>>& data) : data_(data) {}
    
    // Access methods
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    std::tuple<position_vector, int, double>& operator[](size_t i) { return data_[i]; }
    const std::tuple<position_vector, int, double>& operator[](size_t i) const { return data_[i]; }
    
    std::tuple<position_vector, int, double>& at(size_t i) { return data_.at(i); }
    const std::tuple<position_vector, int, double>& at(size_t i) const { return data_.at(i); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Get the underlying data
    const std::vector<std::tuple<position_vector, int, double>>& getData() const { return data_; }
    
    // Sort by distance (ascending)
    void sortByDistance() {
        sort(data_.begin(), data_.end(), 
            [](const auto& a, const auto& b) {
                return std::get<2>(a) < std::get<2>(b);
            });
    }
    
    // Get only the vectors
    std::vector<position_vector> getVectors() const {
        std::vector<position_vector> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<0>(row));
        }
        return result;
    }
    
    // Get only the transposition indices
    std::vector<int> getTranspositions() const {
        std::vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<1>(row));
        }
        return result;
    }
    
    // Get only the distances
    std::vector<double> getDistances() const {
        std::vector<double> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<2>(row));
        }
        return result;
    }
    
   TranspositionMatrixRow getByComplexity(int complexity = 0) const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot get by complexity from empty matrix");
        }
        if (complexity < 0 || complexity > 100) {
            throw std::runtime_error("Complexity must be between 0 and 100");
        }
        
        size_t index = static_cast<size_t>((complexity / 100.0) * (data_.size() - 1));
        return TranspositionMatrixRow(data_[index]);
    }
    
    TranspositionMatrixRow getClosest() const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot get closest from empty matrix");
        }
        auto it = min_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return std::get<2>(a) < std::get<2>(b);
            });
        return TranspositionMatrixRow(*it);
    }
    
    TranspositionMatrixRow getFurthest() const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot get furthest from empty matrix");
        }
        auto it = max_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return std::get<2>(a) < std::get<2>(b);
            });
        return TranspositionMatrixRow(*it);
    }
};

/**
 * @brief Class representing a relative-mode matrix with distance metrics
 */
class RelativeModeMatrixDistance {
private:
    std::vector<std::tuple<position_vector, int, double>> data_; // (vector, relative_mode_offset, distance)
    int center_;

public:
    RelativeModeMatrixDistance() : center_(0) {}
    
    explicit RelativeModeMatrixDistance(const std::vector<std::tuple<position_vector, int, double>>& data, int center = 0) 
        : data_(data), center_(center) {}
    
    // Access methods
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    std::tuple<position_vector, int, double>& operator[](size_t i) { return data_[i]; }
    const std::tuple<position_vector, int, double>& operator[](size_t i) const { return data_[i]; }
    
    std::tuple<position_vector, int, double>& at(size_t i) { return data_.at(i); }
    const std::tuple<position_vector, int, double>& at(size_t i) const { return data_.at(i); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Get the underlying data
    const std::vector<std::tuple<position_vector, int, double>>& getData() const { return data_; }
    
    // Get the center
    int getCenter() const { return center_; }
    
    // Sort by distance (ascending)
    void sortByDistance() {
        sort(data_.begin(), data_.end(), 
            [](const auto& a, const auto& b) {
                return std::get<2>(a) < std::get<2>(b);
            });
    }
    
    // Get only the vectors
    std::vector<position_vector> getVectors() const {
        std::vector<position_vector> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<0>(row));
        }
        return result;
    }
    
    // Get only the relative-mode offsets (row indices in the matrix)
    std::vector<int> get_relative_mode_offsets() const {
        std::vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<1>(row));
        }
        return result;
    }
    
    // Get only the distances
    std::vector<double> getDistances() const {
        std::vector<double> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<2>(row));
        }
        return result;
    }
    
    // Get the closest match
    RelativeModeMatrixRow getByComplexity(int complexity = 0) const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot get by complexity from empty matrix");
        }
        if (complexity < 0 || complexity > 100) {
            throw std::runtime_error("Complexity must be between 0 and 100");
        }
        
        size_t index = static_cast<size_t>((complexity / 100.0) * (data_.size() - 1));
        return RelativeModeMatrixRow(data_[index], center_);
    }
    
    RelativeModeMatrixRow getClosest() const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot get closest from empty matrix");
        }
        auto it = min_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return std::get<2>(a) < std::get<2>(b);
            });
        return RelativeModeMatrixRow(*it, center_);
    }
    
    RelativeModeMatrixRow getFurthest() const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot get furthest from empty matrix");
        }
        auto it = max_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return std::get<2>(a) < std::get<2>(b);
            });
        return RelativeModeMatrixRow(*it, center_);
    }
};

/**
 * @brief Class representing a modal selection matrix with distance metrics
 * @tparam T Type of the vector (interval_vector or position_vector)
 */
template<typename T>
class ModalSelectionMatrixDistance {
private:
    std::vector<std::tuple<T, int, double>> data_; // (chord, mode_index, distance)

public:
    ModalSelectionMatrixDistance() = default;
    
    explicit ModalSelectionMatrixDistance(const std::vector<std::tuple<T, int, double>>& data) : data_(data) {}
    
    // Access methods
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    std::tuple<T, int, double>& operator[](size_t i) { return data_[i]; }
    const std::tuple<T, int, double>& operator[](size_t i) const { return data_[i]; }
    
    std::tuple<T, int, double>& at(size_t i) { return data_.at(i); }
    const std::tuple<T, int, double>& at(size_t i) const { return data_.at(i); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Get the underlying data
    const std::vector<std::tuple<T, int, double>>& getData() const { return data_; }
    
    // Sort by distance (ascending)
    void sortByDistance() {
        sort(data_.begin(), data_.end(), 
            [](const auto& a, const auto& b) {
                return std::get<2>(a) < std::get<2>(b);
            });
    }
    
    // Get only the chords
    std::vector<T> getChords() const {
        std::vector<T> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<0>(row));
        }
        return result;
    }
    
    // Get only the mode indices
    std::vector<int> getModeIndices() const {
        std::vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<1>(row));
        }
        return result;
    }
    
    // Get only the distances
    std::vector<double> getDistances() const {
        std::vector<double> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<2>(row));
        }
        return result;
    }
    
    ModalSelectionMatrixRow<T> getByComplexity(int complexity = 0) const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot get by complexity from empty matrix");
        }
        if (complexity < 0 || complexity > 100) {
            throw std::runtime_error("Complexity must be between 0 and 100");
        }
        
        size_t index = static_cast<size_t>((complexity / 100.0) * (data_.size() - 1));
        return ModalSelectionMatrixRow<T>(data_[index]);
    }
    
    ModalSelectionMatrixRow<T> getClosest() const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot get closest from empty matrix");
        }
        auto it = min_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return std::get<2>(a) < std::get<2>(b);
            });
        return ModalSelectionMatrixRow<T>(*it);
    }
    
    ModalSelectionMatrixRow<T> getFurthest() const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot get furthest from empty matrix");
        }
        auto it = max_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return std::get<2>(a) < std::get<2>(b);
            });
        return ModalSelectionMatrixRow<T>(*it);
    }
};


// ==================== DISTANCE CALCULATION FUNCTIONS ====================

/**
 * @brief Type alias for distance function pointer for position_vector
 */
using DistanceFuncPV = int (*)(position_vector, position_vector);

/**
 * @brief Type alias for distance function pointer for interval_vector
 */
using DistanceFuncIV = int (*)(interval_vector, interval_vector);

/**
 * @brief Calculates distances between a reference position_vector and a ModalMatrix
 * @param reference Reference position_vector to compare against
 * @param matrix Input ModalMatrix
 * @param distFunc Distance function to use
 * @param sort If true, sort results by distance (default: true)
 * @return ModalMatrixDistance with computed distances
 */
ModalMatrixDistance<position_vector> calculateDistances(
    const position_vector& reference,
    const ModalMatrix<position_vector>& matrix,
    DistanceFuncPV distFunc = manhattanDistance,
    bool sort = true)
{
    std::vector<std::tuple<position_vector, int, double>> result;
    result.reserve(matrix.size());
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const auto& [vec, idx] = matrix[i];
        double dist = distFunc(reference, vec);
        result.emplace_back(std::make_tuple(vec, idx, dist));
    }
    
    auto mmd = ModalMatrixDistance<position_vector>(result);
    if (sort) {
        mmd.sortByDistance();
    }
    return mmd;
}

/**
 * @brief Calculates distances between a reference interval_vector and a ModalMatrix
 * @param reference Reference interval_vector to compare against
 * @param matrix Input ModalMatrix
 * @param distFunc Distance function to use
 * @param sort If true, sort results by distance (default: true)
 * @return ModalMatrixDistance with computed distances
 */
ModalMatrixDistance<interval_vector> calculateDistances(
    const interval_vector& reference,
    const ModalMatrix<interval_vector>& matrix,
    DistanceFuncIV distFunc = manhattanDistance,
    bool sort = true)
{
    std::vector<std::tuple<interval_vector, int, double>> result;
    result.reserve(matrix.size());
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const auto& [vec, idx] = matrix[i];
        double dist = distFunc(reference, vec);
        result.emplace_back(std::make_tuple(vec, idx, dist));
    }
    
    auto mmd = ModalMatrixDistance<interval_vector>(result);
    if (sort) {
        mmd.sortByDistance();
    }
    return mmd;
}

/**
 * @brief Calculates distances between a reference position_vector and a TranspositionMatrix
 * @param reference Reference position_vector to compare against
 * @param matrix Input TranspositionMatrix
 * @param distFunc Distance function to use
 * @param sort If true, sort results by distance (default: true)
 * @return TranspositionMatrixDistance with computed distances
 */
TranspositionMatrixDistance calculateDistances(
    const position_vector& reference,
    const TranspositionMatrix& matrix,
    DistanceFuncPV distFunc = manhattanDistance,
    bool sort = true)
{
    std::vector<std::tuple<position_vector, int, double>> result;
    result.reserve(matrix.size());
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const auto& [vec, idx] = matrix[i];
        double dist = distFunc(reference, vec);
        result.emplace_back(std::make_tuple(vec, idx, dist));
    }
    
    auto tmd = TranspositionMatrixDistance(result);
    if (sort) {
        tmd.sortByDistance();
    }
    return tmd;
}

int align(position_vector reference, position_vector target){
  if (reference.size() == 0 || target.size() == 0) {
    throw std::invalid_argument("align: reference and target must be non-empty");
  }
  const int refRange = reference.effective_range();
  const int tgtRange = target.effective_range();
  if (refRange <= 0 || tgtRange <= 0) {
    throw std::invalid_argument("align: effective range must be positive");
  }
  int minV = reference[0];
  division_result referenceDiv = euclidean_division(reference[0], refRange);
  division_result targetDiv = euclidean_division(target[0], tgtRange);
  int diffOct = referenceDiv.quotient - targetDiv.remainder;
  int size = target.size();
  int i = diffOct * size;

  while (target[i] <= minV) {
    i++;
  }

  while (target[i] > minV) {
    i--;
  }

  return i;
}

/**
 * @brief Calculates distances between a reference position_vector and a RelativeModeMatrix
 * @param reference Reference position_vector to compare against
 * @param matrix Input RelativeModeMatrix
 * @param distFunc Distance function to use
 * @param sort If true, sort results by distance (default: true)
 * @return RelativeModeMatrixDistance with computed distances
 */
RelativeModeMatrixDistance calculateDistances(
    const position_vector& reference,
    const RelativeModeMatrix& matrix,
    DistanceFuncPV distFunc = manhattanDistance,
    bool sort = true)
{
    std::vector<std::tuple<position_vector, int, double>> result;
    result.reserve(matrix.size());
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const auto& [vec, idx] = matrix[i];
        double dist = distFunc(reference, vec);
        result.emplace_back(std::make_tuple(vec, idx, dist));
    }
    auto rmd = RelativeModeMatrixDistance(result, matrix.getCenter());
    if (sort) {
        rmd.sortByDistance();
    }
    return rmd;
}

/**
 * @brief Calculates distances between a reference position_vector and a ModalSelectionMatrix
 * @param reference Reference position_vector to compare against
 * @param matrix Input ModalSelectionMatrix
 * @param distFunc Distance function to use
 * @param sort If true, sort results by distance (default: true)
 * @return ModalSelectionMatrixDistance with computed distances
 */
ModalSelectionMatrixDistance<position_vector> calculateDistances(
    const position_vector& reference,
    const ModalSelectionMatrix<position_vector>& matrix,
    DistanceFuncPV distFunc = manhattanDistance,
    bool sort = true)
{
    std::vector<std::tuple<position_vector, int, double>> result;
    result.reserve(matrix.size());
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const auto& [vec, idx] = matrix[i];
        double dist = distFunc(reference, vec);
        result.emplace_back(std::make_tuple(vec, idx, dist));
    }
    
    auto mmd = ModalSelectionMatrixDistance<position_vector>(result);
    if (sort) {
        mmd.sortByDistance();
    }
    return mmd;
}

/**
 * @brief Calculates distances between a reference interval_vector and a ModalSelectionMatrix
 * @param reference Reference interval_vector to compare against
 * @param matrix Input ModalSelectionMatrix
 * @param distFunc Distance function to use
 * @param sort If true, sort results by distance (default: true)
 * @return ModalSelectionMatrixDistance with computed distances
 */
ModalSelectionMatrixDistance<interval_vector> calculateDistances(
    const interval_vector& reference,
    const ModalSelectionMatrix<interval_vector>& matrix,
    DistanceFuncIV distFunc = manhattanDistance,
    bool sort = true)
{
    std::vector<std::tuple<interval_vector, int, double>> result;
    result.reserve(matrix.size());
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const auto& [vec, idx] = matrix[i];
        double dist = distFunc(reference, vec);
        result.emplace_back(std::make_tuple(vec, idx, dist));
    }
    
    auto mmd = ModalSelectionMatrixDistance<interval_vector>(result);
    if (sort) {
        mmd.sortByDistance();
    }
    return mmd;
}

// ==================== MODAL ROTOTRANSLATION DISTANCE MATRIX CLASS ====================

/**
 * @brief Class representing distance metrics for a modal relative-mode matrix
 */
class ModalRelativeModeMatrixDistance {
private:
    // (mode_index, relative_mode_index, vector, distance)
    std::vector<std::tuple<int, int, position_vector, double>> data_;

public:
    ModalRelativeModeMatrixDistance() = default;
    
    explicit ModalRelativeModeMatrixDistance(
        const std::vector<std::tuple<int, int, position_vector, double>>& data) 
        : data_(data) {}
    
    // Access methods
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    std::tuple<int, int, position_vector, double>& operator[](size_t i) { return data_[i]; }
    const std::tuple<int, int, position_vector, double>& operator[](size_t i) const { return data_[i]; }
    
    std::tuple<int, int, position_vector, double>& at(size_t i) { return data_.at(i); }
    const std::tuple<int, int, position_vector, double>& at(size_t i) const { return data_.at(i); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Get the underlying data
    const std::vector<std::tuple<int, int, position_vector, double>>& getData() const { return data_; }
    
    // Sort by distance (ascending)
    void sortByDistance() {
        sort(data_.begin(), data_.end(), 
            [](const auto& a, const auto& b) {
                return std::get<3>(a) < std::get<3>(b);
            });
    }
    
    // Sort by mode index first, then distance
    void sortByMode() {
        sort(data_.begin(), data_.end(), 
            [](const auto& a, const auto& b) {
                if (std::get<0>(a) != std::get<0>(b)) {
                    return std::get<0>(a) < std::get<0>(b);
                }
                return std::get<3>(a) < std::get<3>(b);
            });
    }
    
    // Get only the vectors
    std::vector<position_vector> getVectors() const {
        std::vector<position_vector> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<2>(row));
        }
        return result;
    }
    
    // Get only the mode indices
    std::vector<int> getModeIndices() const {
        std::vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<0>(row));
        }
        return result;
    }
    
    // Get only the relative-mode indices (within each mode)
    std::vector<int> get_relative_mode_indices() const {
        std::vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<1>(row));
        }
        return result;
    }
    
    // Get only the distances
    std::vector<double> getDistances() const {
        std::vector<double> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(std::get<3>(row));
        }
        return result;
    }
    
    ModalRelativeModeMatrixRow getByComplexity(int complexity = 0) const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot get by complexity from empty matrix");
        }
        if (complexity < 0 || complexity > 100) {
            throw std::runtime_error("Complexity must be between 0 and 100");
        }
        
        size_t index = static_cast<size_t>((complexity / 100.0) * (data_.size() - 1));
        return ModalRelativeModeMatrixRow(data_[index]);
    }
    
    ModalRelativeModeMatrixRow getClosest() const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot get closest from empty matrix");
        }
        auto it = min_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return std::get<3>(a) < std::get<3>(b);
            });
        return ModalRelativeModeMatrixRow(*it);
    }
    
    ModalRelativeModeMatrixRow getFurthest() const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot get furthest from empty matrix");
        }
        auto it = max_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return std::get<3>(a) < std::get<3>(b);
            });
        return ModalRelativeModeMatrixRow(*it);
    }
    
};


/**
 * @brief Calculates distances between a reference vector and all vectors in a modal relative-mode matrix
 * @param reference Reference position_vector to compare against
 * @param matrix Input ModalRelativeModeMatrix
 * @param distFunc Distance function to use
 * @param sort If true, sort results by distance (default: true)
 * @return ModalRelativeModeMatrixDistance with computed distances
 * @details Computes the distance from the reference to every relative-mode variant
 *          in every mode, storing mode index, relative-mode index, vector, and distance.
 */
ModalRelativeModeMatrixDistance calculateDistances(
    const position_vector& reference,
    const ModalRelativeModeMatrix<position_vector>& matrix,
    DistanceFuncPV distFunc = manhattanDistance,
    bool sort = true)
{
    std::vector<std::tuple<int, int, position_vector, double>> result;
    result.reserve(matrix.getTotalVectorCount());
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const auto& [rtm, mode_idx] = matrix[i];
        
        for (size_t j = 0; j < rtm.size(); ++j) {
            const auto& [vec, trans_idx] = rtm[j];
            double dist = distFunc(reference, vec);
            result.emplace_back(std::make_tuple(mode_idx, trans_idx, vec, dist));
        }
    }
    
    auto mrmd = ModalRelativeModeMatrixDistance(result);
    if (sort) {
        mrmd.sortByDistance();
    }
    return mrmd;
}

// ==================== PRINT HELPERS ====================

// Helper to stringify a position_vector or interval_vector's data
template <typename Vec>
static std::string vecToString(const Vec& v) {
    std::ostringstream oss;
    oss << "[";
    const auto& data = v.data();
    for (size_t i = 0; i < data.size(); ++i) {
        if (i) oss << ", ";
        oss << data[i];
    }
    oss << "]";
    return oss.str();
}

// Print a ModalMatrixDistance<T>
template <typename T>
void printMatrixDistance(const ModalMatrixDistance<T>& mmd, std::ostream& out = std::cout) {
    out << std::left << std::setw(6) << "Row" << std::setw(20) << "Mode" << std::setw(30) << "Vector" << "Distance" << '\n';
    out << std::string(80, '-') << '\n';
    for (size_t i = 0; i < mmd.size(); ++i) {
        const auto& [vec, idx, dist] = mmd[i];
        out << std::setw(6) << i << std::setw(20) << idx << std::setw(30) << vecToString(vec) << dist << '\n';
    }
}

// Print a TranspositionMatrixDistance
inline void printMatrixDistance(const TranspositionMatrixDistance& tmd, std::ostream& out = std::cout) {
    out << std::left << std::setw(6) << "Row" << std::setw(16) << "Transposition" << std::setw(30) << "Vector" << "Distance" << '\n';
    out << std::string(80, '-') << '\n';
    for (size_t i = 0; i < tmd.size(); ++i) {
        const auto& [vec, idx, dist] = tmd[i];
        out << std::setw(6) << i << std::setw(16) << idx << std::setw(30) << vecToString(vec) << dist << '\n';
    }
}

// Print a RelativeModeMatrixDistance
inline void printMatrixDistance(const RelativeModeMatrixDistance& rmd, std::ostream& out = std::cout) {
    out << std::left << std::setw(6) << "Row" << std::setw(16) << "Rel.mode" << std::setw(30) << "Vector" << std::setw(10) << "Distance" << "Center" << '\n';
    out << std::string(80, '-') << '\n';
    for (size_t i = 0; i < rmd.size(); ++i) {
        const auto& [vec, idx, dist] = rmd[i];
        out << std::setw(6) << i << std::setw(16) << idx << std::setw(30) << vecToString(vec) << std::setw(10) << dist << rmd.getCenter() << '\n';
    }
}

// Print a ModalSelectionMatrixDistance<T>
template <typename T>
void printMatrixDistance(const ModalSelectionMatrixDistance<T>& mmd, std::ostream& out = std::cout) {
    out << std::left << std::setw(6) << "Row" << std::setw(16) << "Degree" << std::setw(30) << "Chord" << "Distance" << '\n';
    out << std::string(80, '-') << '\n';
    for (size_t i = 0; i < mmd.size(); ++i) {
        const auto& [chord, mode, dist] = mmd[i];
        out << std::setw(6) << i << std::setw(16) << mode << std::setw(30) << vecToString(chord) << dist << '\n';
    }
}

// Print a ModalRelativeModeMatrixDistance
inline void printMatrixDistance(const ModalRelativeModeMatrixDistance& mrmd, std::ostream& out = std::cout) {
    out << std::left 
        << std::setw(6) << "Row" 
        << std::setw(10) << "Degree" 
        << std::setw(12) << "Rel.mode" 
        << std::setw(30) << "Vector" 
        << "Distance" << '\n';
    out << std::string(80, '-') << '\n';
    
    for (size_t i = 0; i < mrmd.size(); ++i) {
        const auto& [mode, trans, vec, dist] = mrmd[i];
        
        std::ostringstream oss;
        oss << "[";
        const auto& data = vec.data();
        for (size_t k = 0; k < data.size(); ++k) {
            if (k) oss << ", ";
            oss << data[k];
        }
        oss << "]";
        
        out << std::setw(6) << i 
            << std::setw(10) << mode 
            << std::setw(12) << trans 
            << std::setw(30) << oss.str() 
            << dist << '\n';
    }
}

// -------------------- GENERIC TUPLE PRINT HELPERS --------------------

// Generic printer for std::tuple<T, int, double> where T has vecToString support.
// Example tuple: (position_vector, index, distance) or (interval_vector, mode, distance)
template <typename T>
inline std::string tuple_T_int_double_to_string(const std::tuple<T, int, double>& e) {
    std::ostringstream oss;
    const T& vec = std::get<0>(e);
    int idx = std::get<1>(e);
    double dist = std::get<2>(e);
    oss << "idx=" << idx << " vec=" << vecToString(vec) << " dist=" << dist;
    return oss.str();
}

template <typename T>
inline void print_tuple_T_int_double(const std::tuple<T, int, double>& e, std::ostream& out = std::cout) {
    out << tuple_T_int_double_to_string(e) << '\n';
}

// Convenience overloads that accept a ModalMatrixDistance / ModalSelectionMatrixDistance / TranspositionMatrixDistance / RelativeModeMatrixDistance element:
template <typename T>
inline void printMatrixRow(const ModalMatrixDistance<T>& mmd, size_t row, std::ostream& out = std::cout) {
    if (row >= mmd.size()) { out << "row out of range\n"; return; }
    print_tuple_T_int_double(mmd[row], out);
}

template <typename T>
inline void printSelectionRow(const ModalSelectionMatrixDistance<T>& mmd, size_t row, std::ostream& out = std::cout) {
    if (row >= mmd.size()) { out << "row out of range\n"; return; }
    print_tuple_T_int_double(mmd[row], out);
}

inline void printTranspositionRow(const TranspositionMatrixDistance& tmd, size_t row, std::ostream& out = std::cout) {
    if (row >= tmd.size()) { out << "row out of range\n"; return; }
    print_tuple_T_int_double(tmd[row], out); // T == position_vector
}

inline void print_relative_mode_row(const RelativeModeMatrixDistance& rmd, size_t row, std::ostream& out = std::cout) {
    if (row >= rmd.size()) { out << "row out of range\n"; return; }
    print_tuple_T_int_double(rmd[row], out); // T == position_vector
}

// -------------------- MODAL RELATIVE MODE (int,int,position_vector,double) HELPERS --------------------

// String/print helpers for std::tuple<int, int, position_vector, double>
inline std::string tuple_int_int_PV_double_to_string(const std::tuple<int, int, position_vector, double>& e) {
    std::ostringstream oss;
    int mode = std::get<0>(e);
    int trans = std::get<1>(e);
    const position_vector& vec = std::get<2>(e);
    double dist = std::get<3>(e);

    oss << "mode=" << mode
        << " rel_mode=" << trans
        << " vec=" << vecToString(vec)
        << " dist=" << dist;
    return oss.str();
}

inline void print_tuple_int_int_PV_double(const std::tuple<int, int, position_vector, double>& e,
                                          std::ostream& out = std::cout) {
    out << tuple_int_int_PV_double_to_string(e) << '\n';
}

// Convenience printer for a row of ModalRelativeModeMatrixDistance
inline void print_modal_relative_mode_row(const ModalRelativeModeMatrixDistance& mrmd, size_t row, std::ostream& out = std::cout) {
    if (row >= mrmd.size()) { out << "row out of range\n"; return; }
    print_tuple_int_int_PV_double(mrmd[row], out);
}


} // namespace musicpp

#endif // MUSICPP_MATRIX_DISTANCE_H