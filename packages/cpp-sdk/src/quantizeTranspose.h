#ifndef QUANTIZE_TRANSPOSE_H
#define QUANTIZE_TRANSPOSE_H

#include "./positionVector.h"

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
int quantize(int note, const vector<int>& scale, bool left = true) {
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
 * @param inputScale The input scale represented as a PositionVector
 * @param outputscale The output scale represented as a PositionVector
 * @param inRoot The root note of the input scale (default 0)
 * @param outRoot The root note of the output scale (default 0)
 * @param notes Vector of notes to be transposed
 * @param outDegrees Output PositionVector that will contain the degree indices
 * @param outNotes Output PositionVector that will contain the transposed notes
 * @return Pair of PositionVectors: first contains degrees, second contains transposed notes
 * 
 * @details This function:
 *          1. Converts each input note to a pitch class relative to inRoot
 *          2. Quantizes the pitch class to the nearest degree in the input scale
 *          3. Maps the degree to the corresponding degree in the output scale
 *          4. Reconstructs the output note with the correct octave and outRoot
 *          5. Handles duplicate output notes by trying the opposite quantization direction
 *          6. Populates outDegrees and outNotes with the results
 *          7. Returns both PositionVectors as a pair
 */
pair<PositionVector, PositionVector> transpose(
    const PositionVector& inputScale,
    const PositionVector& outputscale,
    int inRoot,
    int outRoot,
    const vector<int>& notes,
    PositionVector& outDegrees,
    PositionVector& outNotes
) {
    const vector<int>& inScale = inputScale.getData();
    const vector<int>& outScale = outputscale.getData();
    int mod = inputScale.getMod();
    
    vector<int> degreesData;
    vector<int> notesData;
    int length = static_cast<int>(outScale.size());
    
    for (size_t i = 0; i < notes.size(); ++i) {
        int note = notes[i];
        
        // Calculate pitch class relative to input root
        int inPC = (note - inRoot) % mod;
        if (inPC < 0) inPC += mod;
        
        // Calculate octave displacement
        int octave = (note - inRoot) / mod;
        if (note - inRoot < 0 && (note - inRoot) % mod != 0) {
            octave--;
        }
        
        bool left = true;
        
        // Find the pitch class in the input scale
        auto it = find(inScale.begin(), inScale.end(), inPC);
        int index = -1;
        
        if (it != inScale.end()) {
            index = static_cast<int>(distance(inScale.begin(), it));
        } else {
            // Quantize to nearest scale degree
            inPC = quantize(inPC, inScale, left);
            it = find(inScale.begin(), inScale.end(), inPC);
            if (it != inScale.end()) {
                index = static_cast<int>(distance(inScale.begin(), it));
            }
        }
        
        if (index != -1) {
            int degree = index;
            int outPC = outScale[degree % length];
            int outNote = outPC + outRoot + octave * mod;
            
            // Handle duplicate notes by trying opposite quantization direction
            if (!notesData.empty() && notesData.back() == outNote) {
                if (i > 0 && notes[i] != notes[i - 1]) {
                    left = !left;
                    inPC = (notes[i] - inRoot) % mod;
                    if (inPC < 0) inPC += mod;
                    inPC = quantize(inPC, inScale, left);
                    
                    it = find(inScale.begin(), inScale.end(), inPC);
                    if (it != inScale.end()) {
                        index = static_cast<int>(distance(inScale.begin(), it));
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
    
    // Create PositionVectors with the same properties as inputScale
    outDegrees = PositionVector(degreesData, inputScale.size(), 
                                inputScale.getUserRange(), 
                                inputScale.getRangeUpdate(), 
                                inputScale.getUser());
    outNotes = PositionVector(notesData, inputScale.getMod(), 
                              inputScale.getUserRange(), 
                              inputScale.getRangeUpdate(), 
                              inputScale.getUser());
    
    return make_pair(outDegrees, outNotes);
}

#endif // QUANTIZE_TRANSPOSE_H
