#ifndef MUSICPP_QUANTIZE_TRANSPOSE_H
#define MUSICPP_QUANTIZE_TRANSPOSE_H

#include "position_vector.h"

#include <algorithm>
#include <utility>
#include <vector>

/**
 * @file quantize_transpose.h
 * @brief Functions for quantizing and transposing notes using scales
 * @author [not251]
 * @date 2025
 */

/**
 * @brief Quantizes a given note to the nearest value in the specified scale
 *
 * @param note The note to be quantized
 * @param scale Vector representing the scale
 * @param left If true, returns the lower neighbor; otherwise, the upper neighbor
 * @return The quantized note
 *
 * @details Returns the closest scale degree to the input note.
 *          When the note falls between two scale degrees:
 *          - If left=true: returns the lower neighbor
 *          - If left=false: returns the upper neighbor
 *          If the note is outside the scale range, returns the boundary value.
 */

namespace musicpp {

int quantize(int note, const std::vector<int>& scale, bool left = true) {
    int lower = -1;
    int upper = -1;

    for (size_t i = 0; i < scale.size(); ++i) {
        if (scale[i] <= note) {
            lower = scale[i];
        }
        if (scale[i] >= note) {
            upper = scale[i];
            break;
        }
    }

    if (lower == -1) return upper;
    if (upper == -1) return lower;

    return left ? lower : upper;
}

/**
 * @brief Quantizes and transposes notes from an input scale to an output scale
 *
 * @param inputScale The input scale represented as a position_vector
 * @param outputscale The output scale represented as a position_vector
 * @param inRoot The root note of the input scale (default 0)
 * @param outRoot The root note of the output scale (default 0)
 * @param notes Vector of notes to be transposed
 * @param outDegrees Output position_vector that will contain the degree indices
 * @param outNotes Output position_vector that will contain the transposed notes
 * @return Pair of position_vectors: first contains degrees, second contains transposed notes
 */
std::pair<position_vector, position_vector> transpose(
    const position_vector& inputScale,
    const position_vector& outputscale,
    int inRoot,
    int outRoot,
    const std::vector<int>& notes,
    position_vector& outDegrees,
    position_vector& outNotes)
{
    const std::vector<int>& inScale = inputScale.data();
    const std::vector<int>& outScale = outputscale.data();
    int mod = inputScale.mod();

    std::vector<int> degreesData;
    std::vector<int> notesData;
    int length = static_cast<int>(outScale.size());

    for (size_t i = 0; i < notes.size(); ++i) {
        int note = notes[i];

        int inPC = (note - inRoot) % mod;
        if (inPC < 0) inPC += mod;

        int octave = (note - inRoot) / mod;
        if (note - inRoot < 0 && (note - inRoot) % mod != 0) {
            octave--;
        }

        bool left = true;

        auto it = std::find(inScale.begin(), inScale.end(), inPC);
        int index = -1;

        if (it != inScale.end()) {
            index = static_cast<int>(std::distance(inScale.begin(), it));
        } else {
            inPC = quantize(inPC, inScale, left);
            it = std::find(inScale.begin(), inScale.end(), inPC);
            if (it != inScale.end()) {
                index = static_cast<int>(std::distance(inScale.begin(), it));
            }
        }

        if (index != -1) {
            int degree = index;
            int outPC = outScale[degree % length];
            int outNote = outPC + outRoot + octave * mod;

            if (!notesData.empty() && notesData.back() == outNote) {
                if (i > 0 && notes[i] != notes[i - 1]) {
                    left = !left;
                    inPC = (notes[i] - inRoot) % mod;
                    if (inPC < 0) inPC += mod;
                    inPC = quantize(inPC, inScale, left);

                    it = std::find(inScale.begin(), inScale.end(), inPC);
                    if (it != inScale.end()) {
                        index = static_cast<int>(std::distance(inScale.begin(), it));
                        degree = index;
                        outPC = outScale[degree % length];
                        outNote = outPC + outRoot + octave * mod;
                    }
                }
            }

            notesData.push_back(outNote);
            degreesData.push_back(degree);
        }
    }

    outDegrees = position_vector(degreesData,
                                 inputScale.size(),
                                 inputScale.user_range(),
                                 inputScale.auto_range_updates(),
                                 inputScale.use_user_range());
    outNotes = position_vector(notesData,
                               inputScale.mod(),
                               inputScale.user_range(),
                               inputScale.auto_range_updates(),
                               inputScale.use_user_range());

    return std::make_pair(outDegrees, outNotes);
}

} // namespace musicpp

#endif // MUSICPP_QUANTIZE_TRANSPOSE_H
