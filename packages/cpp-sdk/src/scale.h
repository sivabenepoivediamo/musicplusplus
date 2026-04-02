#ifndef MUSICPP_SCALE_H
#define MUSICPP_SCALE_H

#include "selection.h"

/**
 * @file scale.h
 * @brief Definition of the Scale class for musical scales
 * @author [not251]
 * @date 2025
 * @details This file contains the definition of the Scale class, which represents musical scales.
 *          The Scale class can be constructed from either a position_vector or an interval_vector,
 *          and supports various transformations such as transposition, mode selection, inversion,
 *          and mirroring. The class provides methods to retrieve the scale in both positional and
 *          intervallic forms, as well as access to its parameters and original generator.
 */

/**
 * @struct ScaleParams
 * @brief Structure to hold parameters for the Scale class
 * @details This structure encapsulates the parameters used to define and transform a musical scale.
 *         It includes the root note, mode, degree, inversion settings, and mirroring settings.
 */

namespace musicpp {

struct ScaleParams {
    int root;
    int mode;
    int degree;
    bool invert;
    int inversionAxis;
    bool mirror;
    int mirrorAxis;

    // Default constructor
    ScaleParams(int root = 0,
                int mode = 0,
                int degree = 0,
                bool invert = false,
                int inversionAxis = 0,
                bool mirror = false,
                int mirrorAxis = 0)
        : root(root), mode(mode), degree(degree),
          invert(invert), inversionAxis(inversionAxis),
          mirror(mirror), mirrorAxis(mirrorAxis) {}

    // Update with optional parameters - only updates what's provided
    ScaleParams& withRoot(int val) { root = val; return *this; }
    ScaleParams& withMode(int val) { mode = val; return *this; }
    ScaleParams& withDegree(int val) { degree = val; return *this; }
    ScaleParams& withInvert(bool val) { invert = val; return *this; }
    ScaleParams& withInversionAxis(int val) { inversionAxis = val; return *this; }
    ScaleParams& withMirror(bool val) { mirror = val; return *this; }
    ScaleParams& withMirrorAxis(int val) { mirrorAxis = val; return *this; }

    // Merge with another ScaleParams (other overwrites this where specified)
    void merge(const ScaleParams& other) {
        root = other.root;
        mode = other.mode;
        degree = other.degree;
        invert = other.invert;
        inversionAxis = other.inversionAxis;
        mirror = other.mirror;
        mirrorAxis = other.mirrorAxis;
    }
};

/**
 * @class Scale
 * @brief Class to represent a musical scale with various transformations
 * 
 * @details The Scale class encapsulates a musical scale that can be defined
 *          using either a position_vector or an interval_vector as its generator.
 *          It supports transformations such as transposition (root), mode selection,
 *          inversion, and mirroring. The class provides methods to retrieve the scale
 *          in both positional and intervallic forms, as well as access to its parameters
 *          and original generator.
 * 
 * @note All operations respect cyclic properties and use Euclidean division where applicable.
 */
class Scale {
private:
    interval_vector intervals;
    interval_vector generator;
    bool isFromPositions;
    ScaleParams params;

    // Apply transformations to the internal interval vector
    void applyTransformations() {
        intervals = generator;
        intervals.set_offset(params.root);
        intervals = intervals.rotate(params.mode);
        if (params.invert) {
            intervals = intervals.inversion(params.inversionAxis);
        }
        if (params.mirror) {
            intervals = intervals.singleMirror(params.mirrorAxis, true);
        }
    }

public:
    // Constructor from interval_vector
    Scale(interval_vector& generator, 
          int root = 0, 
          int mode = 0, 
          int degree = 0, 
          bool invert = false, 
          int inversionAxis = 0, 
          bool mirror = false, 
          int mirrorAxis = 0)
        : generator(generator),
          isFromPositions(false),
          params(root, mode, degree, invert, inversionAxis, mirror, mirrorAxis) {
        applyTransformations();
    }

    // Constructor from interval_vector with ScaleParams
    Scale(interval_vector& generator, const ScaleParams& params)
        : generator(generator),
          isFromPositions(false),
          params(params) {
        applyTransformations();
    }

    // Constructor from position_vector
    Scale(position_vector& generator, 
          int root = 0, 
          int mode = 0, 
          int degree = 0, 
          bool invert = false, 
          int inversionAxis = 0, 
          bool mirror = false, 
          int mirrorAxis = 0)
        : generator(positions_to_intervals(generator)),
          isFromPositions(true),
          params(root, mode, degree, invert, inversionAxis, mirror, mirrorAxis) {
        applyTransformations();
    }

    // Constructor from position_vector with ScaleParams
    Scale(position_vector& generator, const ScaleParams& params)
        : generator(positions_to_intervals(generator)),
          isFromPositions(true),
          params(params) {
        applyTransformations();
    }

    // Get as position_vector
    position_vector toPositions() const {
        return intervals_to_positions(intervals);
    }

    // Get as interval_vector
    interval_vector toIntervals() const {
        return intervals;
    }

    // Get original generator as interval_vector
    interval_vector getGenerator() const {
        return generator;
    }

    // Get original generator as position_vector (if applicable)
    position_vector getGeneratorAsPositions() const {
        return intervals_to_positions(generator);
    }

    // Get current parameters
    ScaleParams getParams() const { return params; }
  
    // Individual getters
    int getRoot() const { return params.root; }
    int getMode() const { return params.mode; }
    int getDegree() const { return params.degree; }
    bool getInvert() const { return params.invert; }
    int getInversionAxis() const { return params.inversionAxis; }
    bool getMirror() const { return params.mirror; }
    int getMirrorAxis() const { return params.mirrorAxis; }
    bool getIsFromPositions() const { return isFromPositions; }
    interval_vector getIntervals() const { return intervals; }

    // Individual setters
    void setRoot(int newRoot) { 
        params.root = newRoot; 
        applyTransformations();
    }
    
    void setMode(int newMode) { 
        params.mode = newMode; 
        applyTransformations();
    }

    void setDegree(int newDegree) {
        params.degree = newDegree;
        applyTransformations();
    }

    void setInvert(bool newInvert) {
        params.invert = newInvert;
        applyTransformations();
    }

    void setInversionAxis(int newAxis) {
        params.inversionAxis = newAxis;
        applyTransformations();
    }

    void setMirror(bool newMirror) {
        params.mirror = newMirror;
        applyTransformations();
    }

    void setMirrorAxis(int newAxis) {
        params.mirrorAxis = newAxis;
        applyTransformations();
    }

    void setGenerator(const interval_vector& newGenerator) {
        generator = newGenerator;
        isFromPositions = false;
        applyTransformations();
    }

    void setGenerator(const position_vector& newGenerator) {
        generator = positions_to_intervals(newGenerator);
        isFromPositions = true;
        applyTransformations();
    }

    // Set all parameters at once
    void setParams(const ScaleParams& newParams) {
        params = newParams;
        applyTransformations();
    }

    // Update parameters using builder pattern
    // Returns current params so you can chain updates
    ScaleParams& updateParams() {
        return params;
    }

    // Apply pending parameter updates (call after chaining updates)
    void applyUpdates() {
        applyTransformations();
    }
};

} // namespace musicpp

#endif // MUSICPP_SCALE_H