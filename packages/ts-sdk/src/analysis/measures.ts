import {
  positionsToBinary,
  positionsToIntervals,
  PositionVector,
  type BinaryVector
} from "../core";
import {
  type AksakClassification,
  type GeneratedResult,
  type MeasureSummary,
  type OccurrenceCount
} from "./types";
import {
  arraysEqual,
  containsNumber,
  cppPrecision,
  maxNumber,
  minNumber,
  occurrenceCountForDistance
} from "./shared";

export function differences(values: readonly number[]): number[] {
  var result: number[] = [];
  if (values.length < 2) {
    return result;
  }

  for (var index = 1; index < values.length; index += 1) {
    result.push(values[index] - values[index - 1]);
  }

  return result;
}

export function geodesicDistance(left: number, right: number, mod: number): number {
  var distance = ((right - left) + mod) % mod;
  if (distance > (mod / 2)) {
    distance = mod - distance;
  }
  return distance;
}

export function geodesicDistances(value: PositionVector): number[] {
  var result: number[] = [];
  var data = value.toArray();

  for (var leftIndex = 0; leftIndex < data.length; leftIndex += 1) {
    for (var rightIndex = leftIndex + 1; rightIndex < data.length; rightIndex += 1) {
      result.push(geodesicDistance(data[leftIndex], data[rightIndex], value.mod));
    }
  }

  return result;
}

export function isEuclidean(value: PositionVector, mod: number): boolean {
  var intervals = positionsToIntervals(value).toArray();
  var rotated = intervals.slice();
  var baseline = intervals.slice();
  var length = rotated.length;

  if (length === 0) {
    return false;
  }

  rotated[0] += 1;
  rotated[length - 1] -= 1;

  for (var index = 0; index < length; index += 1) {
    if (arraysEqual(rotated, baseline)) {
      return true;
    }
    rotated.push(rotated.shift() as number);
  }

  return false;
}

export function calculateOccurrences(values: readonly number[]): OccurrenceCount[] {
  var counts: Record<string, number> = {};
  var index = 0;

  for (index = 0; index < values.length; index += 1) {
    var key = String(values[index]);
    counts[key] = (counts[key] || 0) + 1;
  }

  var keys = Object.keys(counts).map(function (value) {
    return Number(value);
  }).sort(function (left, right) {
    return left - right;
  });

  var result: OccurrenceCount[] = [];
  for (index = 0; index < keys.length; index += 1) {
    result.push({
      count: counts[String(keys[index])],
      distance: keys[index]
    });
  }

  return result;
}

export function isWinogradDeep(occurrences: readonly OccurrenceCount[], size: number): boolean {
  var seen: number[] = [];

  for (var distance = 1; distance < size; distance += 1) {
    var count = occurrenceCountForDistance(occurrences, distance);
    if (count === null || seen.indexOf(count) !== -1) {
      return false;
    }
    seen.push(count);
  }

  return true;
}

export function isErdosDeep(occurrences: readonly OccurrenceCount[]): boolean {
  var seen: number[] = [];

  for (var index = 0; index < occurrences.length; index += 1) {
    if (seen.indexOf(occurrences[index].count) !== -1) {
      return false;
    }
    seen.push(occurrences[index].count);
  }

  return true;
}

export function calculateRegressionEvenness(rhythm: readonly number[], totalTimeUnits: number): number {
  var numberOfNotes = rhythm.length;
  var idealInterval = totalTimeUnits / numberOfNotes;
  var regressionEvenness = 0;

  for (var index = 0; index < numberOfNotes; index += 1) {
    regressionEvenness += Math.abs(rhythm[index] - (index * idealInterval));
  }

  return regressionEvenness;
}

export function calculateRhythmicOddity(value: PositionVector): number {
  var notes = value.toArray();
  var rhythmicOddity = 0;

  for (var leftIndex = 0; leftIndex < notes.length; leftIndex += 1) {
    for (var rightIndex = leftIndex + 1; rightIndex < notes.length; rightIndex += 1) {
      var firstDistance = ((notes[rightIndex] - notes[leftIndex]) + value.mod) % value.mod;
      var secondDistance = value.mod - firstDistance;
      if (firstDistance === secondDistance) {
        rhythmicOddity += 1;
      }
    }
  }

  return rhythmicOddity;
}

export function computeTransitionComplexity(value: PositionVector, mod: number): number {
  var binary = positionsToBinary(value).toArray();
  var complexity = 0;

  if (binary.length === 0) {
    return complexity;
  }

  for (var index = 1; index < binary.length; index += 1) {
    if (binary[index] !== binary[index - 1]) {
      complexity += 1;
    }
  }

  return complexity;
}

export function computeEntropy(value: PositionVector): number {
  var binary = positionsToBinary(value).toArray();
  var frequencies: Record<string, number> = {};
  var entropy = 0;

  if (binary.length === 0) {
    return 0;
  }

  for (var index = 0; index < binary.length; index += 1) {
    var key = String(binary[index]);
    frequencies[key] = (frequencies[key] || 0) + 1;
  }

  var keys = Object.keys(frequencies);
  for (index = 0; index < keys.length; index += 1) {
    var probability = frequencies[keys[index]] / binary.length;
    entropy -= probability * (Math.log(probability) / Math.log(2));
  }

  return entropy;
}

export function computeLongestSubsequence(value: PositionVector): number {
  var binary = positionsToBinary(value).toArray();
  var longest = 0;
  var currentRun = 0;

  if (binary.length === 0) {
    return 0;
  }

  longest = 1;
  currentRun = 1;

  for (var index = 1; index < binary.length; index += 1) {
    if (binary[index] === binary[index - 1]) {
      currentRun += 1;
      if (currentRun > longest) {
        longest = currentRun;
      }
    } else {
      currentRun = 1;
    }
  }

  return longest;
}

export function calculateDistributionSpectra(value: PositionVector): Array<readonly number[]> {
  var normalizedScale = value.toArray();
  var spectra: number[][] = [];
  var size = normalizedScale.length;
  var leftIndex = 0;
  var rightIndex = 0;

  if (size < 2) {
    return [];
  }

  for (leftIndex = 0; leftIndex < size - 1; leftIndex += 1) {
    spectra.push([]);
  }

  for (leftIndex = 0; leftIndex < size; leftIndex += 1) {
    for (rightIndex = 1; rightIndex < size; rightIndex += 1) {
      var specificInterval = ((normalizedScale[(leftIndex + rightIndex) % size] - normalizedScale[leftIndex]) + value.mod) % value.mod;
      if (!containsNumber(spectra[rightIndex - 1], specificInterval)) {
        spectra[rightIndex - 1].push(specificInterval);
      }
    }
  }

  for (leftIndex = 0; leftIndex < spectra.length; leftIndex += 1) {
    spectra[leftIndex].sort(function (left, right) {
      return left - right;
    });
  }

  return spectra;
}

export function calculateSpectrumWidths(spectra: ReadonlyArray<readonly number[]>): number[] {
  var widths: number[] = [];

  for (var index = 0; index < spectra.length; index += 1) {
    if (spectra[index].length === 0) {
      widths.push(0);
    } else {
      widths.push(maxNumber(spectra[index]) - minNumber(spectra[index]));
    }
  }

  return widths;
}

export function calculateSpectrumVariation(widths: readonly number[], numberOfTones: number): number {
  var sum = 0;

  for (var index = 0; index < widths.length; index += 1) {
    sum += widths[index];
  }

  return sum / numberOfTones;
}

export function findRotationalSymmetryAxes(scale: PositionVector): number[] {
  var normalizedScale = scale.toArray();
  var axes: number[] = [];
  var size = normalizedScale.length;

  for (var interval = 1; interval < scale.mod; interval += 1) {
    var transposedScale: number[] = new Array(size);
    for (var index = 0; index < size; index += 1) {
      transposedScale[index] = (normalizedScale[index] + interval) % scale.mod;
    }
    transposedScale.sort(function (left, right) {
      return left - right;
    });
    if (arraysEqual(transposedScale, normalizedScale)) {
      axes.push(interval);
    }
  }

  return axes;
}

export function findReflectiveSymmetryAxes(scale: PositionVector): number[] {
  var normalizedScale = scale.toArray();
  var axes: number[] = [];
  var size = normalizedScale.length;

  for (var doubledAxis = 0; doubledAxis < (2 * scale.mod); doubledAxis += 1) {
    var axis = doubledAxis / 2;
    var isSymmetric = true;

    for (var index = 0; index < size; index += 1) {
      var reflected = (2 * axis) - normalizedScale[index];
      var reflectedNote = ((reflected + (10 * scale.mod)) % scale.mod);
      if (!containsNumber(normalizedScale, reflectedNote)) {
        isSymmetric = false;
        break;
      }
    }

    if (isSymmetric) {
      axes.push(axis);
    }
  }

  return axes;
}

export function isPrime(value: number): boolean {
  if (value <= 1) {
    return false;
  }
  if (value === 2) {
    return true;
  }
  if ((value % 2) === 0) {
    return false;
  }

  for (var divisor = 3; divisor <= Math.sqrt(value); divisor += 2) {
    if ((value % divisor) === 0) {
      return false;
    }
  }

  return true;
}

export function classifyAksakRhythm(mod: number): AksakClassification {
  if (isPrime(mod)) {
    return "authentic aksak";
  }
  if ((mod % 2) !== 0) {
    return "quasi-aksak";
  }
  return "pseudo-aksak";
}

export function isPalindrome(scale: PositionVector): boolean {
  return findReflectiveSymmetryAxes(scale).indexOf(0) !== -1;
}

export function isChiral(scale: PositionVector): boolean {
  var normalizedScale = scale.toArray();
  var mirroredScale = normalizedScale.slice();
  var size = normalizedScale.length;

  for (var index = 0; index < mirroredScale.length; index += 1) {
    mirroredScale[index] = (scale.mod - mirroredScale[index]) % scale.mod;
  }

  mirroredScale.sort(function (left, right) {
    return left - right;
  });

  if (arraysEqual(normalizedScale, mirroredScale)) {
    return false;
  }

  for (var interval = 1; interval < scale.mod; interval += 1) {
    var transposed: number[] = new Array(size);
    for (index = 0; index < size; index += 1) {
      transposed[index] = (mirroredScale[index] + interval) % scale.mod;
    }
    transposed.sort(function (left, right) {
      return left - right;
    });
    if (arraysEqual(transposed, normalizedScale)) {
      return false;
    }
  }

  return true;
}

export function isBalanced(scale: PositionVector): boolean {
  var xSum = 0;
  var ySum = 0;
  var angleStep = (2 * Math.PI) / scale.mod;
  var data = scale.toArray();

  for (var index = 0; index < data.length; index += 1) {
    var angle = data[index] * angleStep;
    xSum += Math.cos(angle);
    ySum += Math.sin(angle);
  }

  return Math.abs(xSum) < 1e-6 && Math.abs(ySum) < 1e-6;
}

export function generate(m: number, k: number, n: number): number[] {
  var sequence: number[] = [];

  for (var index = 0; index < k; index += 1) {
    sequence.push((index * m) % n);
  }

  return sequence;
}

export function isGenerated(values: readonly number[], n: number): GeneratedResult {
  var size = values.length;

  for (var multiplier = 1; multiplier < n; multiplier += 1) {
    var generated = generate(multiplier, size, n).slice().sort(function (left, right) {
      return left - right;
    });
    if (arraysEqual(generated, values)) {
      return {
        found: true,
        multiplier: multiplier
      };
    }
  }

  return {
    found: false,
    multiplier: -1
  };
}

export function measureSummary(value: PositionVector): MeasureSummary {
  var positions = value.toArray();
  var intervals = positionsToIntervals(value).toArray();
  var onsets = positionsToBinary(value);
  var differencesData = differences(positions);
  var geodesicData = geodesicDistances(value);
  var occurrences = calculateOccurrences(geodesicData);
  var spectra = calculateDistributionSpectra(value);
  var widths = calculateSpectrumWidths(spectra);
  var rotationalAxes = findRotationalSymmetryAxes(value);
  var reflectiveAxes = findReflectiveSymmetryAxes(value);
  var modulo = value.range;
  var generated = isGenerated(positions, modulo);

  return {
    aksakClassification: classifyAksakRhythm(modulo),
    balanced: isBalanced(value),
    chiral: isChiral(value),
    differences: differencesData,
    distributionSpectra: spectra,
    erdosDeep: isErdosDeep(occurrences),
    generated: generated,
    geodesicDistances: geodesicData,
    intervals: intervals,
    isEuclidean: isEuclidean(value, modulo),
    longestSubsequence: computeLongestSubsequence(value),
    modulo: modulo,
    occurrences: occurrences,
    onsets: onsets.toJSON(),
    palindrome: isPalindrome(value),
    positions: value.toJSON(),
    reflectiveSymmetryAxes: reflectiveAxes,
    regressionEvenness: cppPrecision(calculateRegressionEvenness(positions, modulo)),
    rhythmicOddity: calculateRhythmicOddity(value),
    rotationalSymmetryAxes: rotationalAxes,
    spectrumVariation: cppPrecision(calculateSpectrumVariation(widths, value.size)),
    spectrumWidths: widths,
    transitionComplexity: computeTransitionComplexity(value, modulo),
    winogradDeep: isWinogradDeep(occurrences, value.size),
    entropy: cppPrecision(computeEntropy(value))
  };
}
