#ifndef MUSICPP_SELECTION_H
#define MUSICPP_SELECTION_H

#include "vectors.h"

namespace musicpp {

inline position_vector select(const position_vector& source,
                              const position_vector& criterion,
                              int criterion_relative_mode = 0,
                              int voices = 0) {
    std::vector<int> sourceData = source.data();
    const int criterionModulo = static_cast<int>(sourceData.size());
    position_vector actualCriterion(criterion.data(), criterionModulo);
    actualCriterion.set_mod(static_cast<int>(source.size()));
    const position_vector steppedCriterion = (criterion_relative_mode != 0)
        ? actualCriterion.relative_mode(criterion_relative_mode, voices)
        : actualCriterion;

    const int outLength = (voices > 0) ? voices : static_cast<int>(steppedCriterion.size());
    std::vector<int> result(static_cast<size_t>(outLength));

    for (int k = 0; k < outLength; ++k) {
        result[static_cast<size_t>(k)] = source[steppedCriterion[k]];
    }

    return position_vector(result, source.mod(), source.user_range(), source.auto_range_updates(),
                           source.use_user_range());
}

inline position_vector select(const position_vector& source,
                              const interval_vector& criterion,
                              int criterion_parallel_mode = 0,
                              int voices = 0) {
    interval_vector actualCriterion = criterion;
    actualCriterion.set_mod(static_cast<int>(source.size()));
    const interval_vector steppedCriterion = (criterion_parallel_mode != 0)
        ? actualCriterion.parallel_mode(criterion_parallel_mode, voices)
        : actualCriterion;

    const int outLength = (voices > 0) ? voices : static_cast<int>(steppedCriterion.size());
    std::vector<int> result(static_cast<size_t>(outLength));

    int cumulativePosition = steppedCriterion.offset();

    for (int k = 0; k < outLength; ++k) {
        result[static_cast<size_t>(k)] = source[cumulativePosition];
        cumulativePosition += steppedCriterion[k];
    }

    return position_vector(result, source.mod(), source.user_range(), source.auto_range_updates(),
                           source.use_user_range());
}

inline interval_vector select(const interval_vector& source,
                              const interval_vector& indices,
                              int criterion_parallel_mode = 0,
                              int voices = 0) {
    interval_vector actualCriterion = indices;
    actualCriterion.set_mod(static_cast<int>(source.size()));
    const interval_vector steppedCriterion = (criterion_parallel_mode != 0)
        ? actualCriterion.parallel_mode(criterion_parallel_mode, voices)
        : actualCriterion;

    const int sourceOffset = source.offset();
    const int criterionOffset = steppedCriterion.offset();

    int sOut = sourceOffset;
    for (int j = 0; j < criterionOffset; ++j) {
        sOut += source[j];
    }

    const int outLength = (voices > 0) ? voices : static_cast<int>(steppedCriterion.size());
    std::vector<int> result(static_cast<size_t>(outLength));

    int cumulativeIndex = criterionOffset;

    for (int k = 0; k < outLength; ++k) {
        int sum = 0;
        const int spanLength = steppedCriterion[k];

        for (int j = 0; j < spanLength; ++j) {
            sum += source[cumulativeIndex + j];
        }

        result[static_cast<size_t>(k)] = sum;
        cumulativeIndex += spanLength;
    }

    return interval_vector(result, sOut, source.mod());
}

inline interval_vector select(const interval_vector& source,
                              const position_vector& criterion,
                              int criterion_relative_mode = 0,
                              int voices = 0) {
    const std::vector<int> sourceData = source.data();
    const int off = source.offset();
    position_vector actualCriterion = criterion;
    actualCriterion.set_mod(static_cast<int>(source.size()));
    const position_vector steppedCriterion = (criterion_relative_mode != 0)
        ? actualCriterion.relative_mode(criterion_relative_mode, voices)
        : actualCriterion;

    if (steppedCriterion.size() == 0) {
        return interval_vector({}, source.offset(), source.mod());
    }

    const int outLength = (voices > 0) ? voices : static_cast<int>(steppedCriterion.size());
    std::vector<int> result(static_cast<size_t>(outLength));
    const int n = static_cast<int>(source.size());

    for (int k = 0; k < outLength; ++k) {
        int p_k = steppedCriterion[k];
        int p_next = steppedCriterion[k + 1];

        int delta_k = p_next - p_k;
        if (delta_k <= 0) {
            delta_k += n;
        }

        int sum = 0;
        for (int j = 0; j < delta_k; ++j) {
            sum += source[p_k + j];
        }

        result[static_cast<size_t>(k)] = sum;
    }

    int sOut = off;
    if (criterion[0] >= 0) {
        for (int j = 0; j < criterion[0]; ++j) {
            sOut += source[j];
        }
    } else {
        for (int j = criterion[0]; j < 0; ++j) {
            sOut -= source[j];
        }
    }
    return interval_vector(result, sOut, source.mod());
}

} // namespace musicpp

#endif // MUSICPP_SELECTION_H
