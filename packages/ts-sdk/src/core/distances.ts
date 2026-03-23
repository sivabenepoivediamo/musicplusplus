import {
  absoluteInteger,
  cloneNumberArray,
  sortAscending
} from "./internal";
import { IntervalVector } from "./intervalVector";
import { PositionVector } from "./positionVector";

type DistanceInput = readonly number[] | PositionVector | IntervalVector;

function transformationSteps(start: readonly number[], end: readonly number[]): Array<[number, [number, number]]> {
  var steps: Array<[number, [number, number]]> = [];
  var minLength = start.length < end.length ? start.length : end.length;
  var index = 0;

  for (index = 0; index < minLength; index += 1) {
    var differenceValue = end[index] - start[index];
    if (differenceValue !== 0) {
      steps.push([0, [index, differenceValue]]);
      var transformed = cloneNumberArray(start);
      transformed[index] = transformed[index] + differenceValue;
      return steps.concat(transformationSteps(transformed, end));
    }
  }

  if (end.length > start.length) {
    for (index = minLength; index < end.length; index += 1) {
      steps.push([1, [start.length + (index - minLength), end[index]]]);
    }
  }

  if (end.length < start.length) {
    for (index = minLength; index < start.length; index += 1) {
      steps.push([2, [index, start[index]]]);
    }
  }

  return steps;
}

function distanceSource(input: DistanceInput): number[] {
  if (input instanceof PositionVector || input instanceof IntervalVector) {
    return input.toArray();
  }
  return cloneNumberArray(input);
}

export function euclideanDistance(left: DistanceInput, right: DistanceInput): number {
  var leftData = distanceSource(left);
  var rightData = distanceSource(right);
  var length = leftData.length < rightData.length ? leftData.length : rightData.length;
  var total = 0;

  for (var index = 0; index < length; index += 1) {
    var differenceValue = leftData[index] - rightData[index];
    total += differenceValue * differenceValue;
  }

  return Math.sqrt(total);
}

export function editDistance(left: DistanceInput, right: DistanceInput): number {
  var leftData = distanceSource(left);
  var rightData = distanceSource(right);
  var rows = leftData.length + 1;
  var columns = rightData.length + 1;
  var dp: number[][] = [];
  var row = 0;
  var column = 0;

  for (row = 0; row < rows; row += 1) {
    dp[row] = [];
    dp[row][0] = row;
  }
  for (column = 0; column < columns; column += 1) {
    dp[0][column] = column;
  }

  for (row = 1; row < rows; row += 1) {
    for (column = 1; column < columns; column += 1) {
      if (leftData[row - 1] === rightData[column - 1]) {
        dp[row][column] = dp[row - 1][column - 1];
      } else {
        var insertion = dp[row][column - 1];
        var deletion = dp[row - 1][column];
        var substitution = dp[row - 1][column - 1];
        var minimum = insertion < deletion ? insertion : deletion;
        minimum = minimum < substitution ? minimum : substitution;
        dp[row][column] = 1 + minimum;
      }
    }
  }

  return dp[rows - 1][columns - 1];
}

export function hammingDistance(left: DistanceInput, right: DistanceInput): number {
  var leftData = distanceSource(left);
  var rightData = distanceSource(right);
  var length = leftData.length < rightData.length ? leftData.length : rightData.length;
  var distance = 0;

  for (var index = 0; index < length; index += 1) {
    if (leftData[index] !== rightData[index]) {
      distance += 1;
    }
  }

  return distance;
}

export function manhattanDistance(left: DistanceInput, right: DistanceInput): number {
  var leftData = distanceSource(left);
  var rightData = distanceSource(right);
  var length = leftData.length < rightData.length ? leftData.length : rightData.length;
  var sum = 0;

  for (var index = 0; index < length; index += 1) {
    sum += absoluteInteger(leftData[index] - rightData[index]);
  }

  return sum;
}

export function difference(left: DistanceInput, right: DistanceInput): number {
  var leftData = distanceSource(left);
  var rightData = distanceSource(right);
  var length = leftData.length < rightData.length ? leftData.length : rightData.length;
  var total = 0;

  for (var index = 0; index < length; index += 1) {
    total += leftData[index] - rightData[index];
  }

  return total;
}

export function variationDistance(left: readonly number[] | PositionVector, right: readonly number[] | PositionVector): number {
  var leftData = left instanceof PositionVector ? left.toArray() : cloneNumberArray(left);
  var rightData = right instanceof PositionVector ? right.toArray() : cloneNumberArray(right);

  if (leftData.length === 0 && rightData.length === 0) {
    return 0;
  }
  if (leftData.length === 0 || rightData.length === 0) {
    throw new Error("Empty vector, distance calculation is impossible.");
  }

  var sortedLeft = sortAscending(leftData);
  var sortedRight = sortAscending(rightData);
  var minLength = sortedLeft.length < sortedRight.length ? sortedLeft.length : sortedRight.length;
  var maxLength = sortedLeft.length > sortedRight.length ? sortedLeft.length : sortedRight.length;
  var total = 0;
  var index = 0;

  for (index = 0; index < minLength; index += 1) {
    total += absoluteInteger(sortedLeft[index] - sortedRight[index]);
  }

  var longer = sortedLeft.length >= sortedRight.length ? sortedLeft : sortedRight;
  var shorter = sortedLeft.length >= sortedRight.length ? sortedRight : sortedLeft;

  for (index = minLength; index < maxLength; index += 1) {
    var extra = longer[index];
    var minimum = Number.POSITIVE_INFINITY;
    var maximum = Number.NEGATIVE_INFINITY;
    for (var shortIndex = 0; shortIndex < shorter.length; shortIndex += 1) {
      var currentDistance = absoluteInteger(extra - shorter[shortIndex]);
      if (currentDistance < minimum) {
        minimum = currentDistance;
      }
      if (currentDistance > maximum) {
        maximum = currentDistance;
      }
    }
    total += (minimum + maximum) / 2;
  }

  return total;
}

export function weightedTransformationDistance(left: DistanceInput, right: DistanceInput): number {
  var leftData = distanceSource(left);
  var rightData = distanceSource(right);
  var steps = transformationSteps(leftData, rightData);
  var distance = 0;

  for (var index = 0; index < steps.length; index += 1) {
    distance += absoluteInteger(steps[index][1][1]);
  }

  return distance;
}
