#ifndef MUSICPP_MATRIX_H
#define MUSICPP_MATRIX_H

#include "chord.h"

#include <algorithm>
#include <iomanip>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

/**    
 * @file matrix.h
 * @brief Functions for generating musical matrices (modal, transposition, relative mode)
 * @author [not251]
 * @date 2025
 * @details This file contains functions to generate various musical matrices:
 * - Modal Matrix: Generates all rotations of an interval_vector or position_vector.
 * - Transposition Matrix: Generates all transpositions of a position_vector.
 * - Relative-mode matrix: All relative-mode extractions of a position_vector around a center.
 * - Modal Selection: Selects chords from a source vector based on modal criteria.
 * @note All operations respect cyclic properties and use Euclidean division where applicable.
 **/


namespace musicpp {

// Forward declarations for ostream operators
template<typename T> class ModalMatrix;
class TranspositionMatrix;
class RelativeModeMatrix;
template<typename T> class ModalSelectionMatrix;
template<typename T> class ModalRelativeModeMatrix;

// ==================== MATRIX CLASSES ====================

/**
 * @brief Class representing a modal matrix for interval_vectors or position_vectors
 * @tparam T Type of the vector (interval_vector or position_vector)
 */
template<typename T>
class ModalMatrix {
private:
    std::vector<std::pair<T, int>> data_;

public:
    ModalMatrix() = default;
    
    explicit ModalMatrix(const std::vector<std::pair<T, int>>& data) : data_(data) {}
    
    // Access methods
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    std::pair<T, int>& operator[](size_t i) { return data_[i]; }
    const std::pair<T, int>& operator[](size_t i) const { return data_[i]; }
    
    std::pair<T, int>& at(size_t i) { return data_.at(i); }
    const std::pair<T, int>& at(size_t i) const { return data_.at(i); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Get the underlying data
    const std::vector<std::pair<T, int>>& getData() const { return data_; }
    
    // Get only the vectors (without indices)
    std::vector<T> getVectors() const {
        std::vector<T> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(row.first);
        }
        return result;
    }
    
    // Get only the indices
    std::vector<int> getIndices() const {
        std::vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(row.second);
        }
        return result;
    }
    
    // Friend declaration for ostream operator
    friend std::ostream& operator<<(std::ostream& os, const ModalMatrix<T>& mm) {
        os << std::setw(6) << "Row" << " | " << std::setw(4) << "Mode" << " | Vector\n";
        os << std::string(60, '-') << "\n";
        for (size_t i = 0; i < mm.size(); ++i) {
            os << std::setw(6) << i + 1 << " | " << std::setw(4) << mm[i].second << " | " << mm[i].first << "\n";
        }
        return os;
    }
};

/**
 * @brief Class representing a transposition matrix for position_vectors
 */
class TranspositionMatrix {
private:
    std::vector<std::pair<position_vector, int>> data_;

public:
    TranspositionMatrix() = default;
    
    explicit TranspositionMatrix(const std::vector<std::pair<position_vector, int>>& data) : data_(data) {}
    
    // Access methods
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    std::pair<position_vector, int>& operator[](size_t i) { return data_[i]; }
    const std::pair<position_vector, int>& operator[](size_t i) const { return data_[i]; }
    
    std::pair<position_vector, int>& at(size_t i) { return data_.at(i); }
    const std::pair<position_vector, int>& at(size_t i) const { return data_.at(i); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Get the underlying data
    const std::vector<std::pair<position_vector, int>>& getData() const { return data_; }
    
    // Get only the vectors (without indices)
    std::vector<position_vector> getVectors() const {
        std::vector<position_vector> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(row.first);
        }
        return result;
    }
    
    // Get only the transposition indices
    std::vector<int> getTranspositions() const {
        std::vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(row.second);
        }
        return result;
    }
    
    // Friend declaration for ostream operator
    friend std::ostream& operator<<(std::ostream& os, const TranspositionMatrix& tm);
};

// ostream operator for TranspositionMatrix
inline std::ostream& operator<<(std::ostream& os, const TranspositionMatrix& tm) {
    os << std::setw(6) << "Row" << " | " << std::setw(4) << "Transposition" << " | Vector\n";
    os << std::string(60, '-') << "\n";
    for (size_t i = 0; i < tm.size(); ++i) {
        os << std::setw(6) << i << " | " << std::setw(4) << tm[i].second << " | " << tm[i].first << "\n";
    }
    return os;
}

/**
 * @brief Class representing a relative-mode matrix for position_vectors
 */
class RelativeModeMatrix {
private:
    std::vector<std::pair<position_vector, int>> data_;
    int center_;

public:
    RelativeModeMatrix() : center_(0) {}
    
    explicit RelativeModeMatrix(const std::vector<std::pair<position_vector, int>>& data, int center = 0) 
        : data_(data), center_(center) {}
    
    // Access methods
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    std::pair<position_vector, int>& operator[](size_t i) { return data_[i]; }
    const std::pair<position_vector, int>& operator[](size_t i) const { return data_[i]; }
    
    std::pair<position_vector, int>& at(size_t i) { return data_.at(i); }
    const std::pair<position_vector, int>& at(size_t i) const { return data_.at(i); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Get the underlying data
    const std::vector<std::pair<position_vector, int>>& getData() const { return data_; }
    
    // Get the center used for relative-mode rows
    int getCenter() const { return center_; }
    
    // Get only the vectors (without indices)
    std::vector<position_vector> getVectors() const {
        std::vector<position_vector> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(row.first);
        }
        return result;
    }
    
    // Get only the relative-mode offsets (row labels)
    std::vector<int> get_relative_mode_offsets() const {
        std::vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(row.second);
        }
        return result;
    }
    
    // Friend declaration for ostream operator
    friend std::ostream& operator<<(std::ostream& os, const RelativeModeMatrix& rtm);
};

// ostream operator for RelativeModeMatrix
inline std::ostream& operator<<(std::ostream& os, const RelativeModeMatrix& rtm) {
    os << std::setw(6) << "Row" << " | " << std::setw(4) << "Rel." << " | Vector\n";
    os << std::string(60, '-') << "\n";
    for (size_t i = 0; i < rtm.size(); ++i) {
        os << std::setw(6) << i << " | " << std::setw(4) << rtm[i].second << " | " << rtm[i].first << "\n";
    }
    return os;
}

/**
 * @brief Class representing a modal selection matrix
 * @tparam T Type of the vector (interval_vector or position_vector)
 */
template<typename T>
class ModalSelectionMatrix {
private:
    std::vector<std::pair<T, int>> data_;

public:
    ModalSelectionMatrix() = default;
    
    explicit ModalSelectionMatrix(const std::vector<std::pair<T, int>>& data) : data_(data) {}
    
    // Access methods
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    std::pair<T, int>& operator[](size_t i) { return data_[i]; }
    const std::pair<T, int>& operator[](size_t i) const { return data_[i]; }
    
    std::pair<T, int>& at(size_t i) { return data_.at(i); }
    const std::pair<T, int>& at(size_t i) const { return data_.at(i); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Get the underlying data
    const std::vector<std::pair<T, int>>& getData() const { return data_; }
    
    // Get only the chords (without indices)
    std::vector<T> getChords() const {
        std::vector<T> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(row.first);
        }
        return result;
    }
    
    // Get only the mode indices
    std::vector<int> getModeIndices() const {
        std::vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(row.second);
        }
        return result;
    }
    
    // Friend declaration for ostream operator
    friend std::ostream& operator<<(std::ostream& os, const ModalSelectionMatrix<T>& msm) {
        os << std::setw(6) << "Mode" << " | " << std::setw(4) << "Degree" << " | Vector\n";
        os << std::string(60, '-') << "\n";
        for (size_t i = 0; i < msm.size(); ++i) {
            os << std::setw(6) << i + 1 << " | " << std::setw(4) << msm[i].second << " | " << msm[i].first << "\n";
        }
        return os;
    }
};

// ==================== MODAL ROTOTRANSLATION MATRIX CLASS ====================

/**
 * @brief Class representing a modal selection where each row contains a relative-mode matrix
 * @tparam T Type of the vector (interval_vector or position_vector)
 */
template<typename T>
class ModalRelativeModeMatrix {
private:
    std::vector<std::pair<RelativeModeMatrix, int>> data_; // (relative-mode matrix, mode index)

public:
    ModalRelativeModeMatrix() = default;
    
    explicit ModalRelativeModeMatrix(const std::vector<std::pair<RelativeModeMatrix, int>>& data) 
        : data_(data) {}
    
    // Access methods
    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    
    std::pair<RelativeModeMatrix, int>& operator[](size_t i) { return data_[i]; }
    const std::pair<RelativeModeMatrix, int>& operator[](size_t i) const { return data_[i]; }
    
    std::pair<RelativeModeMatrix, int>& at(size_t i) { return data_.at(i); }
    const std::pair<RelativeModeMatrix, int>& at(size_t i) const { return data_.at(i); }
    
    // Iterator support
    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }
    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    
    // Get the underlying data
    const std::vector<std::pair<RelativeModeMatrix, int>>& getData() const { return data_; }
    
    // Get only the relative-mode matrices
    std::vector<RelativeModeMatrix> get_relative_mode_matrices() const {
        std::vector<RelativeModeMatrix> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(row.first);
        }
        return result;
    }
    
    // Get only the mode indices
    std::vector<int> getModeIndices() const {
        std::vector<int> result;
        result.reserve(data_.size());
        for (const auto& row : data_) {
            result.emplace_back(row.second);
        }
        return result;
    }
    
    // Get total number of relative-mode rows across all matrices
    size_t getTotalVectorCount() const {
        size_t count = 0;
        for (const auto& [matrix, _] : data_) {
            count += matrix.size();
        }
        return count;
    }
    
    // Friend declaration for ostream operator
    friend std::ostream& operator<<(std::ostream& os, const ModalRelativeModeMatrix<T>& mrtm) {
        for (size_t i = 0; i < mrtm.size(); ++i) {
            os << "Rotation " << i + 1 << " (degree " << mrtm[i].second << "):\n";
            os << mrtm[i].first;
            if (i < mrtm.size() - 1) os << "\n";
        }
        return os;
    }
};

// ==================== MATRIX GENERATION FUNCTIONS ====================

/**
 * @brief Generates the modal matrix of an interval_vector    
 * @param iv Input interval_vector
 * @return ModalMatrix containing parallel-mode rows and indices
 * @details Each row applies parallel_mode(i) to the input interval_vector.
 */ 
ModalMatrix<interval_vector> modalMatrix(interval_vector iv) {
    int n = iv.size();
    std::vector<std::pair<interval_vector, int>> matrix;
    matrix.reserve(n);
    
    for (int i = 0; i < n; ++i) {
        interval_vector rotated = iv.parallel_mode(i);
        matrix.emplace_back(std::make_pair(rotated, i));
    }
    
    return ModalMatrix<interval_vector>(matrix);
}

/**
 * @brief Generates the relative-mode matrix of a position_vector
 * @param in Input position_vector
 * @param center Center index for the family of relative-mode rows
 * @return RelativeModeMatrix containing relative-mode variants and indices
 * @details Each row is relative_mode(i) of the input for i from center−n through center+n.
 *         The stored index is i (relative-mode offset).
 *         The number of rows is determined by the size of the input vector.
 */
RelativeModeMatrix relative_mode_matrix(position_vector& in, int center) {
    std::vector<std::pair<position_vector, int>> matrix;
    int n = in.size();

    for (int i = center - n; i < center + n+1; i++) {
        position_vector row = in.relative_mode(i);
        matrix.emplace_back(std::make_pair(row, i));
    }
    return RelativeModeMatrix(matrix, center);
}

/**
 * @brief Generates the modal matrix of a position_vector
 * @param pv Input position_vector
 * @return ModalMatrix containing rotations and indices
 * @details Each row is a rotation of the input position_vector.
 *         The rotation index indicates the amount of rotation applied.
 *         The number of rows is determined by the size of the input vector.
 *         Internally converts the position_vector to an interval_vector for rotation,
 *         then back to position_vector.
 */
ModalMatrix<position_vector> modalMatrix(position_vector pv) {
    interval_vector iv = positions_to_intervals(pv);
    ModalMatrix<interval_vector> ivMatrix = modalMatrix(iv);
    
    std::vector<std::pair<position_vector, int>> pvMatrix;
    pvMatrix.reserve(ivMatrix.size());
    for (size_t i = 0; i < ivMatrix.size(); ++i) {
        position_vector posVec = intervals_to_positions(ivMatrix[i].first);
        pvMatrix.emplace_back(std::make_pair(posVec, ivMatrix[i].second));
    }

    return ModalMatrix<position_vector>(pvMatrix);
}

/**
 * @brief Generates the transposition matrix of a position_vector
 * @param pv Input position_vector
 * @return TranspositionMatrix containing transpositions and indices
 * @details Each row is a transposition of the input position_vector.
 *         The transposition index indicates the amount of transposition applied.
 *         The number of rows is determined by the modulo of the input vector.
 *         Internally uses modular arithmetic to ensure values wrap around the modulo.
 *         The resulting position_vectors are sorted in ascending order for consistency.
 */
TranspositionMatrix transpositionMatrix(position_vector pv) {
    int n = pv.mod();
    std::vector<std::pair<position_vector, int>> matrix;
    matrix.reserve(n);
    
    for (int i = 0; i < n; ++i) {
        position_vector transposed = (pv + i) % n;
        std::sort(transposed.data().begin(), transposed.data().end());
        matrix.emplace_back(std::make_pair(transposed, i));
    }
    
    return TranspositionMatrix(matrix);
}

/**
 * @brief Generates a selection from a source vector based on the modal matrix of the criterion
 * @param source Source interval_vector
 * @param criterion interval_vector defining the modal structure
 * @param degree Degree of selection (default 0)
 * @return ModalSelectionMatrix containing chords and rotation indices
 * @details For each mode defined by the criterion, generates a chord from the source
 *          starting at the specified degree. The rotation index indicates the mode used.
 *          The degree is adjusted based on the sum of intervals in the criterion.
 */
ModalSelectionMatrix<interval_vector> modalSelection(interval_vector source, interval_vector criterion, int degree = 0){
    ModalMatrix<interval_vector> modes = modalMatrix(criterion);
    int rows = modes.size();
    std::vector<std::pair<interval_vector, int>> selection;
    selection.reserve(rows);
    for (int i = 0; i < rows; ++i) {
        interval_vector candidate = chord(source, modes[i].first, degree);
        int sum = 0;
        for (int k = 0; k < i; ++k) {
            sum += criterion.data()[k];
        }
        division_result div = euclidean_division(degree - sum, source.size());
        int g = div.remainder;
        selection.emplace_back(std::make_pair(candidate, g));
    }
    return ModalSelectionMatrix<interval_vector>(selection);
}

/**
 * @brief Generates a selection from a source position_vector based on the modal matrix of the criterion
 * @param source Source position_vector
 * @param criterion interval_vector defining the modal structure
 * @param degree Degree of selection (default 0)
 * @return ModalSelectionMatrix containing chords and rotation indices
 * @details For each mode defined by the criterion, generates a chord from the source
 *          starting at the specified degree. The rotation index indicates the mode used.
 *          The degree is adjusted based on the sum of intervals in the criterion.
 * @note Converts the source position_vector to an interval_vector for chord generation,
 *       then back to position_vector for the result.
 */
ModalSelectionMatrix<position_vector> modalSelection(position_vector source, interval_vector criterion, int degree = 0){
    ModalMatrix<interval_vector> modes = modalMatrix(criterion);
    interval_vector ivSource = positions_to_intervals(source);
    int rows = modes.size();
    std::vector<std::pair<position_vector, int>> selection;
    selection.reserve(rows);
    for (int i = 0; i < rows; ++i) {
        interval_vector candidate = chord(ivSource, modes[i].first, degree);
        position_vector pc = intervals_to_positions(candidate);
        int sum = 0;
        for (int k = 0; k < i; ++k) {
            sum += criterion.data()[k];
        }
        division_result div = euclidean_division(degree - sum, source.size());
        int g = div.remainder;
        selection.emplace_back(std::make_pair(pc, g));
    }
    return ModalSelectionMatrix<position_vector>(selection);
}

// ==================== GENERATION FUNCTIONS ====================

/**
 * @brief Generates a modal relative-mode matrix from a modal selection
 * @param selection Input ModalSelectionMatrix
 * @return ModalRelativeModeMatrix with a relative-mode matrix for each selected chord
 * @details For each chord in the modal selection, generates a full relative_mode_matrix
 *          with center 0, preserving the mode index from the selection.
 */
ModalRelativeModeMatrix<position_vector> modal_relative_mode(
    const ModalSelectionMatrix<position_vector>& selection)
{
    std::vector<std::pair<RelativeModeMatrix, int>> result;
    result.reserve(selection.size());
    
    for (size_t i = 0; i < selection.size(); ++i) {
        const auto& [chord, mode_idx] = selection[i];
        position_vector pv = chord; // Make a copy since relative_mode_matrix takes non-const ref
        RelativeModeMatrix rtm = relative_mode_matrix(pv, 0);
        result.emplace_back(std::make_pair(rtm, mode_idx));
    }
    
    return ModalRelativeModeMatrix<position_vector>(result);
}

/**
 * @brief Filters a ModalMatrix<position_vector> to keep only rows containing all specified MIDI notes
 * @param matrix Input ModalMatrix<position_vector>
 * @param notes Vector of MIDI note numbers to check for
 * @return ModalMatrix<position_vector> with only rows containing all specified notes (mod checked)
 * @details Checks if each row's position_vector contains all notes in the notes vector,
 *          comparing modulo the position_vector's modulo value.
 */
ModalMatrix<position_vector> filterModalMatrix(
    const ModalMatrix<position_vector>& matrix, 
    const std::vector<int>& notes)
{
    if (notes.empty()) {
        return matrix; // No filtering if no notes specified
    }
    
    std::vector<std::pair<position_vector, int>> filtered;
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const position_vector& pv = matrix[i].first;
        int mode_idx = matrix[i].second;
        int mod = pv.mod();
        
        // Check if this row contains all required notes (modulo mod)
        bool contains_all = true;
        for (int note : notes) {
            int note_mod = ((note % mod) + mod) % mod; // Euclidean modulo
            
            // Check if note_mod exists in this position_vector
            bool found = false;
            for (int pos : pv.data()) {
                if (((pos % mod) + mod) % mod == note_mod) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                contains_all = false;
                break;
            }
        }
        
        if (contains_all) {
            filtered.emplace_back(std::make_pair(pv, mode_idx));
        }
    }
    
    return ModalMatrix<position_vector>(filtered);
}

/**
 * @brief Filters a TranspositionMatrix to keep only rows containing all specified MIDI notes
 * @param matrix Input TranspositionMatrix
 * @param notes Vector of MIDI note numbers to check for
 * @return TranspositionMatrix with only rows containing all specified notes (mod checked)
 * @details Checks if each row's position_vector contains all notes in the notes vector,
 *          comparing modulo the position_vector's modulo value.
 */
TranspositionMatrix filterTranspositionMatrix(
    const TranspositionMatrix& matrix, 
    const std::vector<int>& notes)
{
    if (notes.empty()) {
        return matrix; // No filtering if no notes specified
    }
    
    std::vector<std::pair<position_vector, int>> filtered;
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        const position_vector& pv = matrix[i].first;
        int trans_idx = matrix[i].second;
        int mod = pv.mod();
        
        // Check if this row contains all required notes (modulo mod)
        bool contains_all = true;
        for (int note : notes) {
            int note_mod = ((note % mod) + mod) % mod; // Euclidean modulo
            
            // Check if note_mod exists in this position_vector
            bool found = false;
            for (int pos : pv.data()) {
                if (((pos % mod) + mod) % mod == note_mod) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                contains_all = false;
                break;
            }
        }
        
        if (contains_all) {
            filtered.emplace_back(std::make_pair(pv, trans_idx));
        }
    }
    
    return TranspositionMatrix(filtered);
}

/**
 * @brief In-place filters a ModalMatrix<position_vector> to keep only rows containing all specified MIDI notes
 * @param matrix ModalMatrix<position_vector> to be modified
 * @param notes Vector of MIDI note numbers to check for
 * @details Modifies the input matrix in place, removing rows that don't contain all specified notes.
 */
void filterModalMatrixInPlace(
    ModalMatrix<position_vector>& matrix, 
    const std::vector<int>& notes)
{
    matrix = filterModalMatrix(matrix, notes);
}

/**
 * @brief In-place filters a TranspositionMatrix to keep only rows containing all specified MIDI notes
 * @param matrix TranspositionMatrix to be modified
 * @param notes Vector of MIDI note numbers to check for
 * @details Modifies the input matrix in place, removing rows that don't contain all specified notes.
 */
void filterTranspositionMatrixInPlace(
    TranspositionMatrix& matrix, 
    const std::vector<int>& notes)
{
    matrix = filterTranspositionMatrix(matrix, notes);
}


} // namespace musicpp

#endif // MUSICPP_MATRIX_H