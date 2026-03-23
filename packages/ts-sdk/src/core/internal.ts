import type { DivisionResult } from "./types";

interface NumericArrayLike {
  readonly toArray: () => number[];
}

export function truncateTowardZero(value: number): number {
  return value < 0 ? Math.ceil(value) : Math.floor(value);
}

export function cppNumber(value: number): number {
  if (!isFinite(value)) {
    return value;
  }
  return Number(value.toPrecision(15));
}

export function cloneNumberArray(values: readonly number[]): number[] {
  return values.map(function (value) {
    return value === 0 ? 0 : value;
  });
}

export function absoluteInteger(value: number): number {
  return value < 0 ? -value : value;
}

export function sortAscending(values: readonly number[]): number[] {
  return cloneNumberArray(values).sort(function (left, right) {
    return left - right;
  });
}

export function arraysEqual(left: readonly number[], right: readonly number[]): boolean {
  if (left.length !== right.length) {
    return false;
  }

  for (var index = 0; index < left.length; index += 1) {
    if (left[index] !== right[index]) {
      return false;
    }
  }

  return true;
}

export function uniquePositive(values: readonly number[]): number[] {
  var result: number[] = [];
  for (var index = 0; index < values.length; index += 1) {
    if (values[index] > 0 && result.indexOf(values[index]) === -1) {
      result.push(values[index]);
    }
  }
  return result.sort(function (left, right) {
    return left - right;
  });
}

export function maxNumber(values: readonly number[]): number {
  if (values.length === 0) {
    return 0;
  }

  var current = values[0];
  for (var index = 1; index < values.length; index += 1) {
    if (values[index] > current) {
      current = values[index];
    }
  }
  return current;
}

export function minNumber(values: readonly number[]): number {
  if (values.length === 0) {
    return 0;
  }

  var current = values[0];
  for (var index = 1; index < values.length; index += 1) {
    if (values[index] < current) {
      current = values[index];
    }
  }
  return current;
}

export function extractData(input: readonly number[] | NumericArrayLike): number[] {
  if ("toArray" in input) {
    return input.toArray();
  }
  return cloneNumberArray(input);
}

export function componentwiseOperation(
  left: readonly number[],
  right: readonly number[],
  useLooping: boolean,
  combine: (leftValue: number, rightValue: number) => number
): number[] {
  if (right.length === 0) {
    return cloneNumberArray(left);
  }
  if (left.length === 0) {
    return cloneNumberArray(right);
  }

  var result: number[] = [];
  var index = 0;

  if (useLooping) {
    var maxLength = left.length > right.length ? left.length : right.length;
    result.length = maxLength;
    for (index = 0; index < maxLength; index += 1) {
      result[index] = combine(left[index % left.length], right[index % right.length]);
    }
    return result;
  }

  var minLength = left.length < right.length ? left.length : right.length;
  for (index = 0; index < minLength; index += 1) {
    result.push(combine(left[index], right[index]));
  }
  for (index = minLength; index < left.length; index += 1) {
    result.push(left[index]);
  }
  for (index = minLength; index < right.length; index += 1) {
    result.push(right[index]);
  }
  return result;
}

export function validateBinaryData(values: readonly number[]): void {
  for (var index = 0; index < values.length; index += 1) {
    if (values[index] !== 0 && values[index] !== 1) {
      throw new Error("BinaryVector data must contain only 0s and 1s");
    }
  }
}

export function euclideanDivision(dividend: number, divisor: number): DivisionResult {
  if (divisor === 0) {
    throw new Error("Division by zero");
  }

  if (divisor < 0) {
    dividend = -dividend;
    divisor = -divisor;
  }

  var quotient = truncateTowardZero(dividend / divisor);
  var remainder = dividend - quotient * divisor;

  if (remainder < 0) {
    return {
      quotient: quotient - 1,
      remainder: remainder + divisor
    };
  }

  return {
    quotient: quotient,
    remainder: remainder
  };
}

export function gcd(left: number, right: number): number {
  if (right === 0) {
    return absoluteInteger(left);
  }
  return gcd(right, left % right);
}

export function lcm(values: readonly number[]): number {
  if (values.length === 0) {
    return 1;
  }
  if (values.length === 1) {
    return absoluteInteger(values[0]);
  }

  var result = absoluteInteger(values[0]);
  for (var index = 1; index < values.length; index += 1) {
    var divisor = gcd(result, values[index]);
    result = (result * absoluteInteger(values[index])) / divisor;
  }

  return result;
}
