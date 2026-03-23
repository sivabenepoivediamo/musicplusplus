#ifndef MATRIX_DISTANCE_H
#define MATRIX_DISTANCE_H

#include "./matrix.h"
#include "./distances.h"

/**
 * @file matrixDistance.h
 * @brief Classes and functions for calculating distances between vectors and matrices
 * @author [not251]
 * @date 2025
 * @details This file contains classes and functions to compute distances between a reference vector
 *          and all rows in various matrix types, storing the results with distance metrics and
 *          providing sorting capabilities.
 */


/**
 * @brief Row wrapper for ModalMatrixDistance queries
 * @tparam T Vector type (IntervalVector or PositionVector)
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
    ModalMatrixRow(const tuple<T, int, double>& t)
        : vector_(get<0>(t)), index_(get<1>(t)), distance_(get<2>(t)) {}
    
    // Accessors
    const T& getVector() const { return vector_; }
    int getIndex() const { return index_; }
    double getDistance() const { return distance_; }
    
    // Convenience - get mutable vector
    T& getVector() { return vector_; }
    
    // Convert back to tuple if needed
    tuple<T, int, double> toTuple() const {
        return make_tuple(vector_, index_, distance_);
    }
    
    // String representation
    string toString() const {
        ostringstream oss;
        oss << "Mode = " << index_ 
            << ", Distance = " << distance_ 
            << ", Vector = " << vector_ << ")";
        return oss.str();
    }
    
    friend ostream& operator<<(ostream& os, const ModalMatrixRow& r) {
        return os << r.toString();
    }
};

// ==================== TRANSPOSITION MATRIX RESULT ====================

/**
 * @brief Row wrapper for TranspositionMatrixDistance queries
 */
class TranspositionMatrixRow {
private:
    PositionVector vector_;
    int transposition_;
    double distance_;

public:
    TranspositionMatrixRow(const PositionVector& vec, int trans, double dist)
        : vector_(vec), transposition_(trans), distance_(dist) {}
    
    // Construct from tuple
    explicit TranspositionMatrixRow(const tuple<PositionVector, int, double>& t)
        : vector_(get<0>(t)), transposition_(get<1>(t)), distance_(get<2>(t)) {}
    
    // Accessors
    const PositionVector& getVector() const { return vector_; }
    int getTransposition() const { return transposition_; }
    double getDistance() const { return distance_; }
    
    PositionVector& getVector() { return vector_; }
    
    tuple<PositionVector, int, double> toTuple() const {
        return make_tuple(vector_, transposition_, distance_);
    }
    
    string toString() const {
        ostringstream oss;
        oss << "Transposition = " << transposition_ 
            << ", Distance = " << distance_ 
            << ", Vector = " << vector_ << ")";
        return oss.str();
    }
    
    friend ostream& operator<<(ostream& os, const TranspositionMatrixRow& r) {
        return os << r.toString();
    }
};

// ==================== ROTOTRANSLATION MATRIX RESULT ====================

/**
 * @brief Row wrapper for RototranslationMatrixDistance queries
 */
class RototranslationMatrixRow {
private:
    PositionVector vector_;
    int translation_;
    double distance_;
    int center_;

public:
    RototranslationMatrixRow(const PositionVector& vec, int trans, double dist, int center)
        : vector_(vec), translation_(trans), distance_(dist), center_(center) {}
    
    // Construct from tuple + center
    RototranslationMatrixRow(const tuple<PositionVector, int, double>& t, int center)
        : vector_(get<0>(t)), translation_(get<1>(t)), distance_(get<2>(t)), center_(center) {}
    
    // Accessors
    const PositionVector& getVector() const { return vector_; }
    int getTranslation() const { return translation_; }
    double getDistance() const { return distance_; }
    int getCenter() const { return center_; }
    
    PositionVector& getVector() { return vector_; }
    
    tuple<PositionVector, int, double> toTuple() const {
        return make_tuple(vector_, translation_, distance_);
    }
    
    string toString() const {
        ostringstream oss;
        oss << "Position = " << translation_ 
            << ", Center = " << center_
            << ", Distance = " << distance_ 
            << ", Vector = " << vector_ << ")";
        return oss.str();
    }
    
    friend ostream& operator<<(ostream& os, const RototranslationMatrixRow& r) {
        return os << r.toString();
    }
};

// ==================== MODAL SELECTION MATRIX RESULT ====================

/**
 * @brief Row wrapper for ModalSelectionMatrixDistance queries
 * @tparam T Vector type (IntervalVector or PositionVector)
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
    explicit ModalSelectionMatrixRow(const tuple<T, int, double>& t)
        : chord_(get<0>(t)), modeIndex_(get<1>(t)), distance_(get<2>(t)) {}
    
    // Accessors
    const T& getChord() const { return chord_; }
    int getModeIndex() const { return modeIndex_; }
    double getDistance() const { return distance_; }
    
    T& getChord() { return chord_; }
    
    tuple<T, int, double> toTuple() const {
        return make_tuple(chord_, modeIndex_, distance_);
    }
    
    string toString() const {
        ostringstream oss;
        oss << "Degree = " << modeIndex_ 
            << ", Distance = " << distance_ 
            << ", Vector = " << chord_ << ")";
        return oss.str();
    }
    
    friend ostream& operator<<(ostream& os, const ModalSelectionMatrixRow& r) {
        return os << r.toString();
    }
};

// ==================== MODAL ROTOTRANSLATION MATRIX RESULT ====================

/**
 * @brief Row wrapper for ModalRototranslationMatrixDistance queries
 */
class ModalRototranslationMatrixRow {
private:
    int modeIndex_;
    int translationIndex_;
    PositionVector vector_;
    double distance_;

public:
    ModalRototranslationMatrixRow(int mode, int trans, const PositionVector& vec, double dist)
        : modeIndex_(mode), translationIndex_(trans), vector_(vec), distance_(dist) {}
    
    // Construct from tuple
    explicit ModalRototranslationMatrixRow(const tuple<int, int, PositionVector, double>& t)
        : modeIndex_(get<0>(t)), translationIndex_(get<1>(t)), 
          vector_(get<2>(t)), distance_(get<3>(t)) {}
    
    // Accessors
    int getModeIndex() const { return modeIndex_; }
    int getTranslationIndex() const { return translationIndex_; }
    const PositionVector& getVector() const { return vector_; }
    double getDistance() const { return distance_; }
    
    PositionVector& getVector() { return vector_; }
    
    tuple<int, int, PositionVector, double> toTuple() const {
        return make_tuple(modeIndex_, translationIndex_, vector_, distance_);
    }
    
    string toString() const {
        ostringstream oss;
        oss << "Degree = " << modeIndex_ 
            << ", Positions = " << translationIndex_
            << ", Distance = " << distance_ 
            << ", Vector = " << vector_ << ")";
        return oss.str();
    }
    
    friend ostream& operator<<(ostream& os, const ModalRototranslationMatrixRow& r) {
        return os << r.toString();
    }
};

// ==================== DISTANCE MATRIX CLASSES ====================

/**
 * @brief Class representing a modal matrix with distance metrics
 * @tparam T Type of the vector (IntervalVector or PositionVector)
 */
template<typename T>
class ModalMatrixDistance {
private:
    vector<tuple<T, int, double>> data_; // (vector, index, distance)

public:
    ModalMatrixDistance() = default;
    
    explicit ModalMatrixDistance(const vector<tuple<T, int, double>>& data) : data_(data) {}
    
    // Access methods
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    tuple<T, int, double>& operator[](size_t i) { return data_[i]; }
    const tuple<T, int, double>& operator[](size_t i) const { return data_[i]; }
    
    tuple<T, int, double>& at(size_t i) { return data_.at(i); }
    const tuple<T, int, double>& at(size_t i) const { return data_.at(i); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Get the underlying data
    const vector<tuple<T, int, double>>& getData() const { return data_; }
    
    // Sort by distance (ascending)
    void sortByDistance() {
        stable_sort(data_.begin(), data_.end(), 
            [](const auto& a, const auto& b) {
                return get<2>(a) < get<2>(b);
            });
    }
    
    // Get only the vectors
    vector<T> getVectors() const {
        vector<T> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<0>(row));
        }
        return result;
    }
    
    // Get only the indices
    vector<int> getIndices() const {
        vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<1>(row));
        }
        return result;
    }
    
    // Get only the distances
    vector<double> getDistances() const {
        vector<double> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<2>(row));
        }
        return result;
    }
    
    // Get the closest match
    tuple<T, int, double> getClosest() const {
        if (data_.empty()) {
            throw runtime_error("Cannot get closest from empty matrix");
        }
        return *min_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return get<2>(a) < get<2>(b);
            });
    }
    
    // Get the furthest match
    tuple<T, int, double> getFurthest() const {
        if (data_.empty()) {
            throw runtime_error("Cannot get furthest from empty matrix");
        }
        return *max_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return get<2>(a) < get<2>(b);
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
            throw runtime_error("Cannot get by complexity from empty matrix");
        }
        if (complexity < 0 || complexity > 100) {
            throw runtime_error("Complexity must be between 0 and 100");
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
    vector<tuple<PositionVector, int, double>> data_; // (vector, transposition, distance)

public:
    TranspositionMatrixDistance() = default;
    
    explicit TranspositionMatrixDistance(const vector<tuple<PositionVector, int, double>>& data) : data_(data) {}
    
    // Access methods
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    tuple<PositionVector, int, double>& operator[](size_t i) { return data_[i]; }
    const tuple<PositionVector, int, double>& operator[](size_t i) const { return data_[i]; }
    
    tuple<PositionVector, int, double>& at(size_t i) { return data_.at(i); }
    const tuple<PositionVector, int, double>& at(size_t i) const { return data_.at(i); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Get the underlying data
    const vector<tuple<PositionVector, int, double>>& getData() const { return data_; }
    
    // Sort by distance (ascending)
    void sortByDistance() {
        stable_sort(data_.begin(), data_.end(), 
            [](const auto& a, const auto& b) {
                return get<2>(a) < get<2>(b);
            });
    }
    
    // Get only the vectors
    vector<PositionVector> getVectors() const {
        vector<PositionVector> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<0>(row));
        }
        return result;
    }
    
    // Get only the transposition indices
    vector<int> getTranspositions() const {
        vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<1>(row));
        }
        return result;
    }
    
    // Get only the distances
    vector<double> getDistances() const {
        vector<double> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<2>(row));
        }
        return result;
    }
    
   TranspositionMatrixRow getByComplexity(int complexity = 0) const {
        if (data_.empty()) {
            throw runtime_error("Cannot get by complexity from empty matrix");
        }
        if (complexity < 0 || complexity > 100) {
            throw runtime_error("Complexity must be between 0 and 100");
        }
        
        size_t index = static_cast<size_t>((complexity / 100.0) * (data_.size() - 1));
        return TranspositionMatrixRow(data_[index]);
    }
    
    TranspositionMatrixRow getClosest() const {
        if (data_.empty()) {
            throw runtime_error("Cannot get closest from empty matrix");
        }
        auto it = min_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return get<2>(a) < get<2>(b);
            });
        return TranspositionMatrixRow(*it);
    }
    
    TranspositionMatrixRow getFurthest() const {
        if (data_.empty()) {
            throw runtime_error("Cannot get furthest from empty matrix");
        }
        auto it = max_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return get<2>(a) < get<2>(b);
            });
        return TranspositionMatrixRow(*it);
    }
};

/**
 * @brief Class representing a rototranslation matrix with distance metrics
 */
class RototranslationMatrixDistance {
private:
    vector<tuple<PositionVector, int, double>> data_; // (vector, translation, distance)
    int center_;

public:
    RototranslationMatrixDistance() : center_(0) {}
    
    explicit RototranslationMatrixDistance(const vector<tuple<PositionVector, int, double>>& data, int center = 0) 
        : data_(data), center_(center) {}
    
    // Access methods
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    tuple<PositionVector, int, double>& operator[](size_t i) { return data_[i]; }
    const tuple<PositionVector, int, double>& operator[](size_t i) const { return data_[i]; }
    
    tuple<PositionVector, int, double>& at(size_t i) { return data_.at(i); }
    const tuple<PositionVector, int, double>& at(size_t i) const { return data_.at(i); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Get the underlying data
    const vector<tuple<PositionVector, int, double>>& getData() const { return data_; }
    
    // Get the center
    int getCenter() const { return center_; }
    
    // Sort by distance (ascending)
    void sortByDistance() {
        stable_sort(data_.begin(), data_.end(), 
            [](const auto& a, const auto& b) {
                return get<2>(a) < get<2>(b);
            });
    }
    
    // Get only the vectors
    vector<PositionVector> getVectors() const {
        vector<PositionVector> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<0>(row));
        }
        return result;
    }
    
    // Get only the translation indices
    vector<int> getTranslations() const {
        vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<1>(row));
        }
        return result;
    }
    
    // Get only the distances
    vector<double> getDistances() const {
        vector<double> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<2>(row));
        }
        return result;
    }
    
    // Get the closest match
    RototranslationMatrixRow getByComplexity(int complexity = 0) const {
        if (data_.empty()) {
            throw runtime_error("Cannot get by complexity from empty matrix");
        }
        if (complexity < 0 || complexity > 100) {
            throw runtime_error("Complexity must be between 0 and 100");
        }
        
        size_t index = static_cast<size_t>((complexity / 100.0) * (data_.size() - 1));
        return RototranslationMatrixRow(data_[index], center_);
    }
    
    RototranslationMatrixRow getClosest() const {
        if (data_.empty()) {
            throw runtime_error("Cannot get closest from empty matrix");
        }
        auto it = min_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return get<2>(a) < get<2>(b);
            });
        return RototranslationMatrixRow(*it, center_);
    }
    
    RototranslationMatrixRow getFurthest() const {
        if (data_.empty()) {
            throw runtime_error("Cannot get furthest from empty matrix");
        }
        auto it = max_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return get<2>(a) < get<2>(b);
            });
        return RototranslationMatrixRow(*it, center_);
    }
};

/**
 * @brief Class representing a modal selection matrix with distance metrics
 * @tparam T Type of the vector (IntervalVector or PositionVector)
 */
template<typename T>
class ModalSelectionMatrixDistance {
private:
    vector<tuple<T, int, double>> data_; // (chord, mode_index, distance)

public:
    ModalSelectionMatrixDistance() = default;
    
    explicit ModalSelectionMatrixDistance(const vector<tuple<T, int, double>>& data) : data_(data) {}
    
    // Access methods
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    tuple<T, int, double>& operator[](size_t i) { return data_[i]; }
    const tuple<T, int, double>& operator[](size_t i) const { return data_[i]; }
    
    tuple<T, int, double>& at(size_t i) { return data_.at(i); }
    const tuple<T, int, double>& at(size_t i) const { return data_.at(i); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Get the underlying data
    const vector<tuple<T, int, double>>& getData() const { return data_; }
    
    // Sort by distance (ascending)
    void sortByDistance() {
        stable_sort(data_.begin(), data_.end(), 
            [](const auto& a, const auto& b) {
                return get<2>(a) < get<2>(b);
            });
    }
    
    // Get only the chords
    vector<T> getChords() const {
        vector<T> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<0>(row));
        }
        return result;
    }
    
    // Get only the mode indices
    vector<int> getModeIndices() const {
        vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<1>(row));
        }
        return result;
    }
    
    // Get only the distances
    vector<double> getDistances() const {
        vector<double> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<2>(row));
        }
        return result;
    }
    
    ModalSelectionMatrixRow<T> getByComplexity(int complexity = 0) const {
        if (data_.empty()) {
            throw runtime_error("Cannot get by complexity from empty matrix");
        }
        if (complexity < 0 || complexity > 100) {
            throw runtime_error("Complexity must be between 0 and 100");
        }
        
        size_t index = static_cast<size_t>((complexity / 100.0) * (data_.size() - 1));
        return ModalSelectionMatrixRow<T>(data_[index]);
    }
    
    ModalSelectionMatrixRow<T> getClosest() const {
        if (data_.empty()) {
            throw runtime_error("Cannot get closest from empty matrix");
        }
        auto it = min_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return get<2>(a) < get<2>(b);
            });
        return ModalSelectionMatrixRow<T>(*it);
    }
    
    ModalSelectionMatrixRow<T> getFurthest() const {
        if (data_.empty()) {
            throw runtime_error("Cannot get furthest from empty matrix");
        }
        auto it = max_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return get<2>(a) < get<2>(b);
            });
        return ModalSelectionMatrixRow<T>(*it);
    }
};


// ==================== DISTANCE CALCULATION FUNCTIONS ====================

/**
 * @brief Type alias for distance function pointer for PositionVector
 */
using DistanceFuncPV = int (*)(PositionVector, PositionVector);

/**
 * @brief Type alias for distance function pointer for IntervalVector
 */
using DistanceFuncIV = int (*)(IntervalVector, IntervalVector);

/**
 * @brief Calculates distances between a reference PositionVector and a ModalMatrix
 * @param reference Reference PositionVector to compare against
 * @param matrix Input ModalMatrix
 * @param distFunc Distance function to use
 * @param sort If true, sort results by distance (default: true)
 * @return ModalMatrixDistance with computed distances
 */
ModalMatrixDistance<PositionVector> calculateDistances(
    const PositionVector& reference,
    const ModalMatrix<PositionVector>& matrix,
    DistanceFuncPV distFunc = manhattanDistance,
    bool sort = true)
{
    vector<tuple<PositionVector, int, double>> result;
    result.reserve(matrix.size());
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const auto& [vec, idx] = matrix[i];
        double dist = distFunc(reference, vec);
        result.emplace_back(make_tuple(vec, idx, dist));
    }
    
    auto mmd = ModalMatrixDistance<PositionVector>(result);
    if (sort) {
        mmd.sortByDistance();
    }
    return mmd;
}

/**
 * @brief Calculates distances between a reference IntervalVector and a ModalMatrix
 * @param reference Reference IntervalVector to compare against
 * @param matrix Input ModalMatrix
 * @param distFunc Distance function to use
 * @param sort If true, sort results by distance (default: true)
 * @return ModalMatrixDistance with computed distances
 */
ModalMatrixDistance<IntervalVector> calculateDistances(
    const IntervalVector& reference,
    const ModalMatrix<IntervalVector>& matrix,
    DistanceFuncIV distFunc = manhattanDistance,
    bool sort = true)
{
    vector<tuple<IntervalVector, int, double>> result;
    result.reserve(matrix.size());
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const auto& [vec, idx] = matrix[i];
        double dist = distFunc(reference, vec);
        result.emplace_back(make_tuple(vec, idx, dist));
    }
    
    auto mmd = ModalMatrixDistance<IntervalVector>(result);
    if (sort) {
        mmd.sortByDistance();
    }
    return mmd;
}

/**
 * @brief Calculates distances between a reference PositionVector and a TranspositionMatrix
 * @param reference Reference PositionVector to compare against
 * @param matrix Input TranspositionMatrix
 * @param distFunc Distance function to use
 * @param sort If true, sort results by distance (default: true)
 * @return TranspositionMatrixDistance with computed distances
 */
TranspositionMatrixDistance calculateDistances(
    const PositionVector& reference,
    const TranspositionMatrix& matrix,
    DistanceFuncPV distFunc = manhattanDistance,
    bool sort = true)
{
    vector<tuple<PositionVector, int, double>> result;
    result.reserve(matrix.size());
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const auto& [vec, idx] = matrix[i];
        double dist = distFunc(reference, vec);
        result.emplace_back(make_tuple(vec, idx, dist));
    }
    
    auto tmd = TranspositionMatrixDistance(result);
    if (sort) {
        tmd.sortByDistance();
    }
    return tmd;
}

int align(PositionVector reference, PositionVector target){
  int minV = reference[0];
  DivisionResult referenceDiv = euclideanDivision(reference[0], reference.range);
  DivisionResult targetDiv = euclideanDivision(target[0], target.range);
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
 * @brief Calculates distances between a reference PositionVector and a RototranslationMatrix
 * @param reference Reference PositionVector to compare against
 * @param matrix Input RototranslationMatrix
 * @param distFunc Distance function to use
 * @param sort If true, sort results by distance (default: true)
 * @return RototranslationMatrixDistance with computed distances
 */
RototranslationMatrixDistance calculateDistances(
    const PositionVector& reference,
    const RototranslationMatrix& matrix,
    DistanceFuncPV distFunc = manhattanDistance,
    bool sort = true)
{
    vector<tuple<PositionVector, int, double>> result;
    result.reserve(matrix.size());
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const auto& [vec, idx] = matrix[i];
        double dist = distFunc(reference, vec);
        result.emplace_back(make_tuple(vec, idx, dist));
    }
    auto rmd = RototranslationMatrixDistance(result, matrix.getCenter());
    if (sort) {
        rmd.sortByDistance();
    }
    return rmd;
}

/**
 * @brief Calculates distances between a reference PositionVector and a ModalSelectionMatrix
 * @param reference Reference PositionVector to compare against
 * @param matrix Input ModalSelectionMatrix
 * @param distFunc Distance function to use
 * @param sort If true, sort results by distance (default: true)
 * @return ModalSelectionMatrixDistance with computed distances
 */
ModalSelectionMatrixDistance<PositionVector> calculateDistances(
    const PositionVector& reference,
    const ModalSelectionMatrix<PositionVector>& matrix,
    DistanceFuncPV distFunc = manhattanDistance,
    bool sort = true)
{
    vector<tuple<PositionVector, int, double>> result;
    result.reserve(matrix.size());
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const auto& [vec, idx] = matrix[i];
        double dist = distFunc(reference, vec);
        result.emplace_back(make_tuple(vec, idx, dist));
    }
    
    auto mmd = ModalSelectionMatrixDistance<PositionVector>(result);
    if (sort) {
        mmd.sortByDistance();
    }
    return mmd;
}

/**
 * @brief Calculates distances between a reference IntervalVector and a ModalSelectionMatrix
 * @param reference Reference IntervalVector to compare against
 * @param matrix Input ModalSelectionMatrix
 * @param distFunc Distance function to use
 * @param sort If true, sort results by distance (default: true)
 * @return ModalSelectionMatrixDistance with computed distances
 */
ModalSelectionMatrixDistance<IntervalVector> calculateDistances(
    const IntervalVector& reference,
    const ModalSelectionMatrix<IntervalVector>& matrix,
    DistanceFuncIV distFunc = manhattanDistance,
    bool sort = true)
{
    vector<tuple<IntervalVector, int, double>> result;
    result.reserve(matrix.size());
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const auto& [vec, idx] = matrix[i];
        double dist = distFunc(reference, vec);
        result.emplace_back(make_tuple(vec, idx, dist));
    }
    
    auto mmd = ModalSelectionMatrixDistance<IntervalVector>(result);
    if (sort) {
        mmd.sortByDistance();
    }
    return mmd;
}

// ==================== MODAL ROTOTRANSLATION DISTANCE MATRIX CLASS ====================

/**
 * @brief Class representing distance metrics for a modal rototranslation matrix
 */
class ModalRototranslationMatrixDistance {
private:
    // (mode_index, translation_index, vector, distance)
    vector<tuple<int, int, PositionVector, double>> data_;

public:
    ModalRototranslationMatrixDistance() = default;
    
    explicit ModalRototranslationMatrixDistance(
        const vector<tuple<int, int, PositionVector, double>>& data) 
        : data_(data) {}
    
    // Access methods
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    tuple<int, int, PositionVector, double>& operator[](size_t i) { return data_[i]; }
    const tuple<int, int, PositionVector, double>& operator[](size_t i) const { return data_[i]; }
    
    tuple<int, int, PositionVector, double>& at(size_t i) { return data_.at(i); }
    const tuple<int, int, PositionVector, double>& at(size_t i) const { return data_.at(i); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Get the underlying data
    const vector<tuple<int, int, PositionVector, double>>& getData() const { return data_; }
    
    // Sort by distance (ascending)
    void sortByDistance() {
        stable_sort(data_.begin(), data_.end(), 
            [](const auto& a, const auto& b) {
                return get<3>(a) < get<3>(b);
            });
    }
    
    // Sort by mode index first, then distance
    void sortByMode() {
        sort(data_.begin(), data_.end(), 
            [](const auto& a, const auto& b) {
                if (get<0>(a) != get<0>(b)) {
                    return get<0>(a) < get<0>(b);
                }
                return get<3>(a) < get<3>(b);
            });
    }
    
    // Get only the vectors
    vector<PositionVector> getVectors() const {
        vector<PositionVector> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<2>(row));
        }
        return result;
    }
    
    // Get only the mode indices
    vector<int> getModeIndices() const {
        vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<0>(row));
        }
        return result;
    }
    
    // Get only the translation indices
    vector<int> getTranslationIndices() const {
        vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<1>(row));
        }
        return result;
    }
    
    // Get only the distances
    vector<double> getDistances() const {
        vector<double> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(get<3>(row));
        }
        return result;
    }
    
    ModalRototranslationMatrixRow getByComplexity(int complexity = 0) const {
        if (data_.empty()) {
            throw runtime_error("Cannot get by complexity from empty matrix");
        }
        if (complexity < 0 || complexity > 100) {
            throw runtime_error("Complexity must be between 0 and 100");
        }
        
        size_t index = static_cast<size_t>((complexity / 100.0) * (data_.size() - 1));
        return ModalRototranslationMatrixRow(data_[index]);
    }
    
    ModalRototranslationMatrixRow getClosest() const {
        if (data_.empty()) {
            throw runtime_error("Cannot get closest from empty matrix");
        }
        auto it = min_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return get<3>(a) < get<3>(b);
            });
        return ModalRototranslationMatrixRow(*it);
    }
    
    ModalRototranslationMatrixRow getFurthest() const {
        if (data_.empty()) {
            throw runtime_error("Cannot get furthest from empty matrix");
        }
        auto it = max_element(data_.begin(), data_.end(),
            [](const auto& a, const auto& b) {
                return get<3>(a) < get<3>(b);
            });
        return ModalRototranslationMatrixRow(*it);
    }
    
};


/**
 * @brief Calculates distances between a reference vector and all vectors in a modal rototranslation matrix
 * @param reference Reference PositionVector to compare against
 * @param matrix Input ModalRototranslationMatrix
 * @param distFunc Distance function to use
 * @param sort If true, sort results by distance (default: true)
 * @return ModalRototranslationMatrixDistance with computed distances
 * @details Computes the distance from the reference to every rototranslated vector
 *          in every mode, storing mode index, translation index, vector, and distance.
 */
ModalRototranslationMatrixDistance calculateDistances(
    const PositionVector& reference,
    const ModalRototranslationMatrix<PositionVector>& matrix,
    DistanceFuncPV distFunc = manhattanDistance,
    bool sort = true)
{
    vector<tuple<int, int, PositionVector, double>> result;
    result.reserve(matrix.getTotalVectorCount());
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const auto& [rtm, mode_idx] = matrix[i];
        
        for (size_t j = 0; j < rtm.size(); ++j) {
            const auto& [vec, trans_idx] = rtm[j];
            double dist = distFunc(reference, vec);
            result.emplace_back(make_tuple(mode_idx, trans_idx, vec, dist));
        }
    }
    
    auto mrmd = ModalRototranslationMatrixDistance(result);
    if (sort) {
        mrmd.sortByDistance();
    }
    return mrmd;
}

// ==================== PRINT HELPERS ====================

// Helper to stringify a PositionVector or IntervalVector's data
template <typename Vec>
static std::string vecToString(const Vec& v) {
    std::ostringstream oss;
    oss << "[";
    const auto& data = v.getData();
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

// Print a RototranslationMatrixDistance
inline void printMatrixDistance(const RototranslationMatrixDistance& rmd, std::ostream& out = std::cout) {
    out << std::left << std::setw(6) << "Row" << std::setw(16) << "Position" << std::setw(30) << "Vector" << std::setw(10) << "Distance" << "Center" << '\n';
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

// Print a ModalRototranslationMatrixDistance
inline void printMatrixDistance(const ModalRototranslationMatrixDistance& mrmd, std::ostream& out = std::cout) {
    out << std::left 
        << std::setw(6) << "Row" 
        << std::setw(10) << "Degree" 
        << std::setw(12) << "Position" 
        << std::setw(30) << "Vector" 
        << "Distance" << '\n';
    out << std::string(80, '-') << '\n';
    
    for (size_t i = 0; i < mrmd.size(); ++i) {
        const auto& [mode, trans, vec, dist] = mrmd[i];
        
        std::ostringstream oss;
        oss << "[";
        const auto& data = vec.getData();
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

// Generic printer for tuple<T, int, double> where T has vecToString support.
// Example tuple: (PositionVector, index, distance) or (IntervalVector, mode, distance)
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

// Convenience overloads that accept a ModalMatrixDistance / ModalSelectionMatrixDistance / TranspositionMatrixDistance / RototranslationMatrixDistance element:
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
    print_tuple_T_int_double(tmd[row], out); // T == PositionVector
}

inline void printRototranslationRow(const RototranslationMatrixDistance& rmd, size_t row, std::ostream& out = std::cout) {
    if (row >= rmd.size()) { out << "row out of range\n"; return; }
    print_tuple_T_int_double(rmd[row], out); // T == PositionVector
}

// -------------------- MODAL ROTOTRANSLATION (int,int,PositionVector,double) HELPERS --------------------

// String/print helpers for tuple<int, int, PositionVector, double>
inline std::string tuple_int_int_PV_double_to_string(const std::tuple<int, int, PositionVector, double>& e) {
    std::ostringstream oss;
    int mode = std::get<0>(e);
    int trans = std::get<1>(e);
    const PositionVector& vec = std::get<2>(e);
    double dist = std::get<3>(e);

    oss << "mode=" << mode
        << " trans=" << trans
        << " vec=" << vecToString(vec)
        << " dist=" << dist;
    return oss.str();
}

inline void print_tuple_int_int_PV_double(const std::tuple<int, int, PositionVector, double>& e,
                                          std::ostream& out = std::cout) {
    out << tuple_int_int_PV_double_to_string(e) << '\n';
}

// Convenience printer for a row of ModalRototranslationMatrixDistance
inline void printModalRototranslationRow(const ModalRototranslationMatrixDistance& mrmd, size_t row, std::ostream& out = std::cout) {
    if (row >= mrmd.size()) { out << "row out of range\n"; return; }
    print_tuple_int_int_PV_double(mrmd[row], out);
}



#endif // MATRIX_DISTANCE_H
