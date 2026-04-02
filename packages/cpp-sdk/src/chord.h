#ifndef MUSICPP_CHORD_H
#define MUSICPP_CHORD_H

#include "selection.h"

/**
 * @file chord.h
 * @brief Definition of the Chord class for generating chords from scales
 * @author [not251]
 * @date 2025
 * @details This file contains the definition of the Chord class, which generates chords
 *          based on a scale and a set of degrees or intervals. The class supports various
 *          transformations including shifting, parallel/relative mode, inversion, negation,
 *          and mirroring.
 */

/**
 * @struct ChordParams
 * @brief Structure to hold parameters for the Chord class
 * @details This structure encapsulates the parameters used to define and transform a chord.
 */

namespace musicpp {

struct ChordParams {
    int shift;
    /** Step applied to the criterion: relative_mode for position criterion, parallel_mode for interval criterion. */
    int criterion_mode;
    int preVoices;
    /** Final relative-mode offset applied to the chord result. */
    int relative_mode;
    bool invert;
    int axis;
    bool negativeOrMirror;
    int negativeOrMirrorPos;

    // Default constructor
    ChordParams(int shift = 0,
                int criterion_mode = 0,
                int preVoices = 0,
                int relative_mode = 0,
                bool invert = false,
                int axis = 0,
                bool negativeOrMirror = false,
                int negativeOrMirrorPos = 10)
        : shift(shift),
          criterion_mode(criterion_mode),
          preVoices(preVoices),
          relative_mode(relative_mode),
          invert(invert),
          axis(axis),
          negativeOrMirror(negativeOrMirror),
          negativeOrMirrorPos(negativeOrMirrorPos) {}

    // Builder pattern methods
    ChordParams& withShift(int val) { shift = val; return *this; }
    ChordParams& with_criterion_mode(int val) { criterion_mode = val; return *this; }
    ChordParams& withPreVoices(int val) { preVoices = val; return *this; }
    ChordParams& with_relative_mode(int val) { relative_mode = val; return *this; }
    ChordParams& withInvert(bool val) { invert = val; return *this; }
    ChordParams& withAxis(int val) { axis = val; return *this; }
    ChordParams& withNegativeOrMirror(bool val) { negativeOrMirror = val; return *this; }
    ChordParams& withNegativeOrMirrorPos(int val) { negativeOrMirrorPos = val; return *this; }
};

/**
 * @class Chord
 * @brief Class to represent a musical chord generated from a scale
 * 
 * @details The Chord class generates chords from a scale (position_vector or interval_vector)
 *          and a selection criterion (degrees as position_vector or intervals as interval_vector).
 *          It supports various transformations and provides the result in both positional
 *          and intervallic forms.
 */
class Chord {
private:
    enum CriterionType { POSITION_CRITERION, INTERVAL_CRITERION };
    enum ScaleType { POSITION_SCALE, INTERVAL_SCALE };
    
    position_vector scalePositions;
    interval_vector scaleIntervals;
    position_vector criterionPositions;
    interval_vector criterionIntervals;
    
    ScaleType scaleType;
    CriterionType criterionType;
    ChordParams params;
    
    position_vector resultPositions;
    interval_vector resultIntervals;
    bool isResultPositions;

    // Apply transformations and generate the chord
    void generate() {
        if (scaleType == POSITION_SCALE && criterionType == POSITION_CRITERION) {
            generatePosPos();
        } else if (scaleType == POSITION_SCALE && criterionType == INTERVAL_CRITERION) {
            generatePosInt();
        } else if (scaleType == INTERVAL_SCALE && criterionType == POSITION_CRITERION) {
            generateIntPos();
        } else {
            generateIntInt();
        }
    }

    void generatePosPos() {
        position_vector offsetDegrees = criterionPositions + params.shift;
        resultPositions = select(scalePositions, offsetDegrees, params.criterion_mode, params.preVoices);
        if (params.invert) {
            resultPositions = resultPositions.inversion(params.axis, true);
        }
        if (params.negativeOrMirror) {
            resultPositions = resultPositions.negative(params.negativeOrMirrorPos);
        }
        resultPositions = resultPositions.relative_mode(params.relative_mode);
        isResultPositions = true;
    }

    void generatePosInt() {
        interval_vector offsetIntervals = criterionIntervals;
        offsetIntervals.set_offset(params.shift);
        resultPositions = select(scalePositions, offsetIntervals, params.criterion_mode, params.preVoices);
        if (params.invert) {
            resultPositions = resultPositions.inversion(params.axis, true);
        }
        if (params.negativeOrMirror) {
            resultPositions = resultPositions.negative(params.negativeOrMirrorPos);
        }
        resultPositions = resultPositions.relative_mode(params.relative_mode);
        isResultPositions = true;
    }

    void generateIntPos() {
        position_vector tempScalePos = intervals_to_positions(scaleIntervals);
        position_vector offsetDegrees = criterionPositions + params.shift;
        interval_vector tempResult = select(scaleIntervals, offsetDegrees, params.criterion_mode, params.preVoices);
        resultIntervals = tempResult;
        if (params.invert) {
            resultIntervals = resultIntervals.inversion(params.axis);
        }
        if (params.negativeOrMirror) {
            resultIntervals = resultIntervals.singleMirror(params.negativeOrMirrorPos, true);
        }
        resultIntervals = resultIntervals.relative_mode(params.relative_mode);
        isResultPositions = false;
    }

    void generateIntInt() {
        interval_vector offsetIntervals = criterionIntervals;
        int off = criterionIntervals.offset();
        offsetIntervals.set_offset(params.shift + off);
        interval_vector tempResult = select(scaleIntervals, offsetIntervals, params.criterion_mode, params.preVoices);
        resultIntervals = tempResult;
        if (params.invert) {
            resultIntervals = resultIntervals.inversion(params.axis);
        }
        if (params.negativeOrMirror) {
            resultIntervals = resultIntervals.singleMirror(params.negativeOrMirrorPos, true);
        }
        resultIntervals = resultIntervals.relative_mode(params.relative_mode);
        isResultPositions = false;
    }

public:
    // Constructor: position_vector scale + position_vector criterion
    Chord(position_vector& scale, position_vector& degrees, const ChordParams& params = ChordParams())
        : scalePositions(scale), criterionPositions(degrees),
          scaleType(POSITION_SCALE), criterionType(POSITION_CRITERION),
          params(params), isResultPositions(true) {
        generate();
    }

    // Constructor: position_vector scale + interval_vector criterion
    Chord(position_vector& scale, interval_vector& intervals, const ChordParams& params = ChordParams())
        : scalePositions(scale), criterionIntervals(intervals),
          scaleType(POSITION_SCALE), criterionType(INTERVAL_CRITERION),
          params(params), isResultPositions(true) {
        generate();
    }

    // Constructor: interval_vector scale + position_vector criterion
    Chord(interval_vector& scale, position_vector& degrees, const ChordParams& params = ChordParams())
        : scaleIntervals(scale), criterionPositions(degrees),
          scaleType(INTERVAL_SCALE), criterionType(POSITION_CRITERION),
          params(params), isResultPositions(false) {
        generate();
    }

    // Constructor: interval_vector scale + interval_vector criterion
    Chord(interval_vector& scale, interval_vector& intervals, const ChordParams& params = ChordParams())
        : scaleIntervals(scale), criterionIntervals(intervals),
          scaleType(INTERVAL_SCALE), criterionType(INTERVAL_CRITERION),
          params(params), isResultPositions(false) {
        generate();
    }

    // Get result as position_vector
    position_vector toPositions() const {
        if (isResultPositions) {
            return resultPositions;
        } else {
            return intervals_to_positions(resultIntervals);
        }
    }

    // Get result as interval_vector
    interval_vector toIntervals() const {
        if (isResultPositions) {
            return positions_to_intervals(resultPositions);
        } else {
            return resultIntervals;
        }
    }

    // Get current parameters
    ChordParams getParams() const { return params; }

    // Individual getters
    int getShift() const { return params.shift; }
    int get_criterion_mode() const { return params.criterion_mode; }
    int getPreVoices() const { return params.preVoices; }
    int get_relative_mode() const { return params.relative_mode; }
    bool getInvert() const { return params.invert; }
    int getAxis() const { return params.axis; }
    bool getNegativeOrMirror() const { return params.negativeOrMirror; }
    int getNegativeOrMirrorPos() const { return params.negativeOrMirrorPos; }

    // Individual setters
    void setShift(int val) {
        params.shift = val;
        generate();
    }

    void set_criterion_mode(int val) {
        params.criterion_mode = val;
        generate();
    }

    void setPreVoices(int val) {
        params.preVoices = val;
        generate();
    }

    void set_relative_mode(int val) {
        params.relative_mode = val;
        generate();
    }

    void setInvert(bool val) {
        params.invert = val;
        generate();
    }

    void setAxis(int val) {
        params.axis = val;
        generate();
    }

    void setNegativeOrMirror(bool val) {
        params.negativeOrMirror = val;
        generate();
    }

    void setNegativeOrMirrorPos(int val) {
        params.negativeOrMirrorPos = val;
        generate();
    }

    // Set all parameters at once
    void setParams(const ChordParams& newParams) {
        params = newParams;
        generate();
    }

    // Update scale
    void setScale(const position_vector& newScale) {
        scalePositions = newScale;
        scaleType = POSITION_SCALE;
        generate();
    }

    void setScale(const interval_vector& newScale) {
        scaleIntervals = newScale;
        scaleType = INTERVAL_SCALE;
        generate();
    }

    // Update criterion
    void setCriterion(const position_vector& newCriterion) {
        criterionPositions = newCriterion;
        criterionType = POSITION_CRITERION;
        generate();
    }

    void setCriterion(const interval_vector& newCriterion) {
        criterionIntervals = newCriterion;
        criterionType = INTERVAL_CRITERION;
        generate();
    }

    // Builder pattern support
    ChordParams& updateParams() {
        return params;
    }

    void applyUpdates() {
        generate();
    }
};

/**
 * @file chord.h
 * @brief Functions for generating chords from scales and intervals
 * @author [not251]
 * @date 2025
 * @details This file contains functions to generate chords based on:
 * - A scale represented as a position_vector or interval_vector
 * - A set of degrees (as position_vector) or intervals (as interval_vector)
 * 
 * The functions support various transformations including:
 * - Shifting degrees/intervals
 * - Criterion stepping (relative mode for degrees, parallel mode for interval patterns)
 * - Predefining the number of voices
 * - Inversion around an axis
 * - Negation around a position (for position_vector)
 * - Mirroring around a position (for interval_vector)
 * 
 * The output is either a position_vector or interval_vector representing the resulting chord.
 * All operations respect cyclic properties and use Euclidean division where applicable.
 */

/**
 * @brief Generates a chord from a scale and degrees using position_vectors
 * @param scale position_vector representing the scale
 * @param degrees position_vector representing the degrees to select
 * @param shift Integer to shift the degrees
 * @param criterion_relative_mode Integer relative-mode step for the position criterion
 * @param preVoices Integer to predefine the number of voices in the output
 * @param relative_mode Integer relative-mode offset applied to the result
 * @param invert Boolean to apply inversion around an axis
 * @param axis Integer axis for inversion
 * @param negative Boolean to apply negation around a position
 * @param negativePos Integer position for negation
 * @return position_vector representing the generated chord
 * 
 */

inline position_vector chord(position_vector& scale, position_vector& degrees, int shift = 0, int criterion_relative_mode = 0, int preVoices = 0, int relative_mode = 0, bool invert = false, int axis = 0, bool negative = false, int negativePos = 10) {
    position_vector offsetDegrees = degrees + shift;
    position_vector result = select(scale, offsetDegrees, criterion_relative_mode, preVoices);
    result = (invert) ? result.inversion(axis, true) : result;
    result = (negative) ? result.negative(negativePos) : result;
    return result.relative_mode(relative_mode);
};

/**
 * @brief Generates a chord from a scale and intervals using interval_vectors
 * @param scale interval_vector representing the scale
 * @param intervals interval_vector representing the intervals to select
 * @param shift Integer to shift the intervals
 * @param criterion_parallel_mode Integer parallel-mode step for the interval criterion
 * @param preVoices Integer to predefine the number of voices in the output
 * @param relative_mode Integer relative-mode offset applied to the result
 * @param invert Boolean to apply inversion around an axis
 * @param axis Integer axis for inversion
 * @param negative Boolean to apply negation around a position
 * @param negativePos Integer position for negation
 * @return position_vector representing the generated chord
 * 
 */
inline position_vector chord(position_vector& scale, interval_vector& intervals, int shift = 0, int criterion_parallel_mode = 0, int preVoices = 0, int relative_mode = 0, bool invert = false, int axis = 0, bool negative = false, int negativePos = 10){
    interval_vector offsetIntervals = intervals;
    offsetIntervals.set_offset(shift);
    position_vector result = select(scale, offsetIntervals, criterion_parallel_mode, preVoices);
    result = (invert) ? result.inversion(axis, true) : result;
    result = (negative) ? result.negative(negativePos) : result;
    return result.relative_mode(relative_mode);
};

/**
 * @brief Generates a chord from a scale and degrees using interval_vectors and position_vectors
 * @param scale interval_vector representing the scale
 * @param degrees position_vector representing the degrees to select
 * @param shift Integer to shift the degrees
 * @param criterion_relative_mode Integer relative-mode step for the position criterion
 * @param preVoices Integer to predefine the number of voices in the output
 * @param relative_mode Integer relative-mode offset applied to the result
 * @param invert Boolean to apply inversion around an axis
 * @param axis Integer axis for inversion
 * @param mirror Boolean to apply mirroring around a position
 * @param mirrorPos Integer position for mirroring
 * @return interval_vector representing the generated chord
 * 
 */
inline interval_vector chord(interval_vector& scale, position_vector& degrees, int shift = 0, int criterion_relative_mode = 0, int preVoices = 0, int relative_mode = 0, bool invert = false, int axis = 0, bool mirror = false, int mirrorPos = 10) {
    position_vector offsetDegrees = degrees + shift;
    interval_vector result = select(scale, offsetDegrees, criterion_relative_mode, preVoices);
    result = (invert) ? result.inversion(axis) : result;
    result = (mirror) ? result.singleMirror(mirrorPos, true) : result;
    result = result.relative_mode(relative_mode);
    return result;
};

/**
 * @brief Generates a chord from a scale and intervals using interval_vectors
 * @param scale interval_vector representing the scale
 * @param intervals interval_vector representing the intervals to select
 * @param shift Integer to shift the intervals
 * @param criterion_parallel_mode Integer parallel-mode step for the interval criterion
 * @param preVoices Integer to predefine the number of voices in the output
 * @param relative_mode Integer relative-mode offset applied to the result
 * @param invert Boolean to apply inversion around an axis
 * @param axis Integer axis for inversion
 * @param mirror Boolean to apply mirroring around a position
 * @param mirrorPos Integer position for mirroring
 * @return interval_vector representing the generated chord
 * 
 */
inline interval_vector chord(interval_vector& scale, interval_vector& intervals, int shift = 0, int criterion_parallel_mode = 0, int preVoices = 0, int relative_mode = 0, bool invert = false, int axis = 0, bool mirror = false, int mirrorPos = 10) {
    interval_vector offsetIntervals = intervals;
    int off = intervals.offset();
    offsetIntervals.set_offset(shift + off);
    interval_vector result = select(scale, offsetIntervals, criterion_parallel_mode, preVoices);
    result = (invert) ? result.inversion(axis) : result;
    result = (mirror) ? result.singleMirror(mirrorPos, true) : result;
    result = result.relative_mode(relative_mode);
    return result;
};
} // namespace musicpp

#endif // MUSICPP_CHORD_H
