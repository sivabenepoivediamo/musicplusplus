import { BinaryVector } from "./binaryVector";
import { IntervalVector } from "./intervalVector";
import {
  euclideanDivision,
  minNumber
} from "./internal";
import { PositionVector } from "./positionVector";

export function positionsToIntervals(positions: PositionVector): IntervalVector {
  if (positions.size === 0) {
    return new IntervalVector([], { mod: positions.mod, offset: 0 });
  }

  var positionData = positions.toArray();
  var intervalData: number[] = [];

  if (positionData.length > 1) {
    for (var index = 0; index < positionData.length; index += 1) {
      intervalData.push(positions.at(index + 1) - positions.at(index));
    }
  }

  return new IntervalVector(intervalData, { mod: positions.mod, offset: positions.at(0) });
}

export function intervalsToPositions(intervals: IntervalVector): PositionVector {
  if (intervals.size === 0) {
    return new PositionVector([0], { mod: intervals.mod });
  }

  var intervalData = intervals.toArray();
  var positions: number[] = [];
  var currentPosition = intervals.offset;

  positions.push(currentPosition);
  for (var index = 0; index < intervalData.length - 1; index += 1) {
    currentPosition += intervalData[index];
    positions.push(currentPosition);
  }

  return new PositionVector(positions, { mod: intervals.mod });
}

export function binaryToPositions(binary: BinaryVector): PositionVector {
  var positionData: number[] = [];
  for (var index = 0; index < binary.size; index += 1) {
    if (binary.at(index) === 1) {
      positionData.push(index + binary.offset);
    }
  }

  if (positionData.length === 0) {
    return new PositionVector([0], { mod: binary.mod });
  }

  return new PositionVector(positionData, { mod: binary.mod });
}

export function positionsToBinary(positions: PositionVector): BinaryVector {
  if (positions.size === 0) {
    return new BinaryVector([], { mod: positions.mod, offset: 0 });
  }

  var positionData = positions.toArray();
  var range = positions.range;
  var binaryData: number[] = [];
  var index = 0;

  for (index = 0; index < range; index += 1) {
    binaryData.push(0);
  }

  var minimum = minNumber(positionData);
  for (index = 0; index < positionData.length; index += 1) {
    var normalizedPosition = positionData[index] - positionData[0];
    binaryData[euclideanDivision(normalizedPosition, range).remainder] = 1;
  }

  return new BinaryVector(binaryData, { mod: range, offset: minimum });
}
