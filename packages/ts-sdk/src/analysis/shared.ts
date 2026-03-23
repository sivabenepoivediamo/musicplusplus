import type { OccurrenceCount } from "./types";

export function cloneNumberArray(values: readonly number[]): number[] {
  return values.slice();
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

export function truncateTowardZero(value: number): number {
  return value < 0 ? Math.ceil(value) : Math.floor(value);
}

export function cppPrecision(value: number): number {
  if (!isFinite(value)) {
    return value;
  }
  return Number(value.toPrecision(15));
}

export function occurrenceCountForDistance(occurrences: readonly OccurrenceCount[], distance: number): number | null {
  for (var index = 0; index < occurrences.length; index += 1) {
    if (occurrences[index].distance === distance) {
      return occurrences[index].count;
    }
  }

  return null;
}

export function containsNumber(values: readonly number[], target: number): boolean {
  return values.indexOf(target) !== -1;
}
