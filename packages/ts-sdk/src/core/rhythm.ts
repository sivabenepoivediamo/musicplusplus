import { BinaryVector } from "./binaryVector";
import {
  euclideanDivision,
  truncateTowardZero
} from "./internal";
import { IntervalVector } from "./intervalVector";
import { PositionVector } from "./positionVector";

function euclideanIntervals(steps: number, events: number): number[] {
  var result: number[] = [];
  if (events <= 0) {
    return result;
  }

  var division = euclideanDivision(steps, events);
  if (division.remainder === 0) {
    for (var index = 0; index < events; index += 1) {
      result.push(division.quotient);
    }
    return result;
  }

  var recursive = euclideanIntervals(events, division.remainder);
  for (var recursiveIndex = 0; recursiveIndex < division.remainder; recursiveIndex += 1) {
    for (var fillIndex = 0; fillIndex < recursive[recursiveIndex] - 1; fillIndex += 1) {
      result.push(division.quotient);
    }
    result.push(division.quotient + 1);
  }

  return result;
}

function cloughDouthettValues(steps: number, events: number): number[] {
  var result: number[] = [];
  for (var index = 0; index < events; index += 1) {
    result.push(Math.floor((index * steps) / events));
  }
  return result;
}

function deepRhythmValues(steps: number, events: number, multiplicity: number): number[] {
  var result: number[] = [];
  for (var index = 0; index < events; index += 1) {
    result.push((index * multiplicity) % steps);
  }
  result.sort(function (left, right) {
    return left - right;
  });
  return result;
}

function tihaiGenerator(steps: number, repetitions: number): [number, number] {
  var length = steps;
  while ((length % repetitions) !== 0) {
    length += 1;
  }
  var dams = length - steps;
  var bols = truncateTowardZero(length / repetitions) - dams;
  return [bols, dams];
}

function tihaiReader(bols: number, dams: number, repetitions: number): number[] {
  var result: number[] = [];
  var index = 0;
  for (index = 0; index < bols; index += 1) {
    result.push(1);
  }
  for (index = 0; index < dams; index += 1) {
    result.push(0);
  }
  for (index = 0; index < repetitions - 2; index += 1) {
    var repeatIndex = 0;
    for (repeatIndex = 0; repeatIndex < bols; repeatIndex += 1) {
      result.push(1);
    }
    for (repeatIndex = 0; repeatIndex < dams; repeatIndex += 1) {
      result.push(0);
    }
  }
  for (index = 0; index < bols; index += 1) {
    result.push(1);
  }
  return result;
}

function isAllZeros(values: readonly number[]): boolean {
  for (var index = 0; index < values.length; index += 1) {
    if (values[index] !== 0) {
      return false;
    }
  }
  return true;
}

function isAllOnes(values: readonly number[]): boolean {
  for (var index = 0; index < values.length; index += 1) {
    if (values[index] !== 1) {
      return false;
    }
  }
  return true;
}

function appendOnes(values: number[], targetSize: number): void {
  while (values.length < targetSize) {
    values.push(1);
  }
}

function cut(values: readonly number[], length: number): number[] {
  if (length >= values.length) {
    return values.slice() as number[];
  }
  return values.slice(0, length);
}

function tihaiPattern(steps: number, repetitions: number, pseudo: boolean): number[] {
  var result: number[] = [];
  var index = 0;
  if (steps <= 2) {
    for (index = 0; index < steps; index += 1) {
      result.push(1);
    }
    return result;
  }
  if (repetitions === 1) {
    for (index = 0; index < steps; index += 1) {
      result.push(1);
    }
    return result;
  }
  if (repetitions <= 0) {
    for (index = 0; index < steps; index += 1) {
      result.push(0);
    }
    return result;
  }

  var generated = tihaiGenerator(steps, repetitions);
  var pattern = tihaiReader(generated[0], generated[1], repetitions);

  if (((isAllZeros(pattern) && pseudo) || isAllOnes(pattern)) && pseudo) {
    var shorterPattern = tihaiPattern(steps - 1, repetitions, pseudo);
    appendOnes(shorterPattern, steps);
    return shorterPattern;
  }

  return cut(pattern, steps);
}

export function euclidean(steps: number, events: number, offset: number = 0): IntervalVector {
  return new IntervalVector(euclideanIntervals(steps, events), { mod: steps, offset: offset });
}

export function cloughDouthett(steps: number, events: number, offset: number = 0): PositionVector {
  return new PositionVector(cloughDouthettValues(steps, events), { mod: steps }).addScalar(offset);
}

export function deepRhythm(steps: number, events: number, multiplicity: number, offset: number = 0): PositionVector {
  return new PositionVector(deepRhythmValues(steps, events, multiplicity), { mod: steps }).addScalar(offset);
}

export function tihai(steps: number, repetitions: number, pseudo: boolean = false, offset: number = 0): BinaryVector {
  return new BinaryVector(tihaiPattern(steps, repetitions, pseudo), { mod: steps, offset: offset });
}
