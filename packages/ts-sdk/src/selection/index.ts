import {
  IntervalVector,
  PositionVector
} from "../core";

export interface SelectionOptions {
  readonly rotation?: number;
  readonly voices?: number;
}

function resolveSelectionOptions(options?: SelectionOptions): Required<SelectionOptions> {
  return {
    rotation: options && typeof options.rotation === "number" ? options.rotation : 0,
    voices: options && typeof options.voices === "number" ? options.voices : 0
  };
}

export function select(source: PositionVector, criterion: PositionVector, options?: SelectionOptions): PositionVector;
export function select(source: PositionVector, criterion: IntervalVector, options?: SelectionOptions): PositionVector;
export function select(source: IntervalVector, criterion: PositionVector, options?: SelectionOptions): IntervalVector;
export function select(source: IntervalVector, criterion: IntervalVector, options?: SelectionOptions): IntervalVector;
export function select(
  source: PositionVector | IntervalVector,
  criterion: PositionVector | IntervalVector,
  options?: SelectionOptions
): PositionVector | IntervalVector {
  var resolved = resolveSelectionOptions(options);

  if (source instanceof PositionVector && criterion instanceof PositionVector) {
    var actualCriterion = new PositionVector(criterion.data, { mod: source.size });
    var rotatedCriterion = resolved.rotation !== 0 ? actualCriterion.rotoTranslate(resolved.rotation, resolved.voices) : actualCriterion;
    var outLength = resolved.voices > 0 ? resolved.voices : rotatedCriterion.size;
    var result: number[] = [];

    for (var index = 0; index < outLength; index += 1) {
      result.push(source.at(rotatedCriterion.at(index)));
    }

    return new PositionVector(result, {
      mod: source.mod,
      rangeUpdate: source.rangeUpdate,
      user: source.user,
      userRange: source.userRange
    });
  }

  if (source instanceof PositionVector && criterion instanceof IntervalVector) {
    var intervalCriterion = criterion.withMod(source.size);
    var rotatedIntervals = resolved.rotation !== 0 ? intervalCriterion.rotate(resolved.rotation, resolved.voices) : intervalCriterion;
    var positionOutLength = resolved.voices > 0 ? resolved.voices : rotatedIntervals.size;
    var positions: number[] = [];
    var cumulativePosition = rotatedIntervals.offset;

    for (var positionIndex = 0; positionIndex < positionOutLength; positionIndex += 1) {
      positions.push(source.at(cumulativePosition));
      cumulativePosition += rotatedIntervals.at(positionIndex);
    }

    return new PositionVector(positions, {
      mod: source.mod,
      rangeUpdate: source.rangeUpdate,
      user: source.user,
      userRange: source.userRange
    });
  }

  if (source instanceof IntervalVector && criterion instanceof IntervalVector) {
    var actualIndices = criterion.withMod(source.size);
    var rotatedIndexCriterion = resolved.rotation !== 0 ? actualIndices.rotate(resolved.rotation, resolved.voices) : actualIndices;
    var sourceOffset = source.offset;
    var criterionOffset = rotatedIndexCriterion.offset;
    var outputOffset = sourceOffset;
    var sourceIndex = 0;

    for (sourceIndex = 0; sourceIndex < criterionOffset; sourceIndex += 1) {
      outputOffset += source.at(sourceIndex);
    }

    var intervalOutLength = resolved.voices > 0 ? resolved.voices : rotatedIndexCriterion.size;
    var intervalResult: number[] = [];
    var cumulativeIndex = criterionOffset;

    for (sourceIndex = 0; sourceIndex < intervalOutLength; sourceIndex += 1) {
      var sum = 0;
      var spanLength = rotatedIndexCriterion.at(sourceIndex);
      for (var spanIndex = 0; spanIndex < spanLength; spanIndex += 1) {
        sum += source.at(cumulativeIndex + spanIndex);
      }
      intervalResult.push(sum);
      cumulativeIndex += spanLength;
    }

    return new IntervalVector(intervalResult, { mod: source.mod, offset: outputOffset });
  }

  var positionCriterion = criterion as PositionVector;
  var actualPositionCriterion = new PositionVector(positionCriterion.data, { mod: (source as IntervalVector).size });
  var rotatedPositionCriterion = resolved.rotation !== 0 ? actualPositionCriterion.rotoTranslate(resolved.rotation, resolved.voices) : actualPositionCriterion;

  if (rotatedPositionCriterion.size === 0) {
    return new IntervalVector([], { mod: (source as IntervalVector).mod, offset: (source as IntervalVector).offset });
  }

  var intervalSource = source as IntervalVector;
  var length = resolved.voices > 0 ? resolved.voices : rotatedPositionCriterion.size;
  var resultIntervals: number[] = [];
  var vectorSize = intervalSource.size;
  var intervalIndex = 0;

  for (intervalIndex = 0; intervalIndex < length; intervalIndex += 1) {
    var currentPosition = rotatedPositionCriterion.at(intervalIndex);
    var nextPosition = rotatedPositionCriterion.at(intervalIndex + 1);
    var delta = nextPosition - currentPosition;
    if (delta <= 0) {
      delta += vectorSize;
    }

    var intervalSum = 0;
    for (var deltaIndex = 0; deltaIndex < delta; deltaIndex += 1) {
      intervalSum += intervalSource.at(currentPosition + deltaIndex);
    }
    resultIntervals.push(intervalSum);
  }

  var selectedOffset = intervalSource.offset;
  if (positionCriterion.at(0) >= 0) {
    for (intervalIndex = 0; intervalIndex < positionCriterion.at(0); intervalIndex += 1) {
      selectedOffset += intervalSource.at(intervalIndex);
    }
  } else {
    for (intervalIndex = positionCriterion.at(0); intervalIndex < 0; intervalIndex += 1) {
      selectedOffset -= intervalSource.at(intervalIndex);
    }
  }

  return new IntervalVector(resultIntervals, { mod: intervalSource.mod, offset: selectedOffset });
}
