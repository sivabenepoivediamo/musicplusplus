import {
  align,
  getMaxInterval,
  IntervalVector,
  PositionVector
} from "../core";
import {
  calculateDistances,
  filterModalMatrix,
  filterTranspositionMatrix,
  getByComplexity,
  modalMatrix,
  modalRototranslation,
  modalSelection,
  ModalDistanceRow,
  ModalRototranslationDistanceRow,
  RototranslationDistanceRow,
  rototranslationMatrix,
  TranspositionDistanceRow,
  transpositionMatrix
} from "../matrix";

export function normalizeComplexityVector(complexities: readonly number[], requiredSize: number): number[] {
  var result: number[] = [];
  var index = 0;

  if (complexities.length === 0) {
    for (index = 0; index < requiredSize; index += 1) {
      result.push(0);
    }
    return result;
  }

  if (complexities.length === requiredSize) {
    return complexities.slice() as number[];
  }

  if (complexities.length < requiredSize) {
    for (index = 0; index < requiredSize; index += 1) {
      result.push(complexities[index % complexities.length]);
    }
    return result;
  }

  return complexities.slice(0, requiredSize) as number[];
}

export function degreeAutomation(
  scale: PositionVector,
  criterion: IntervalVector,
  degree: number,
  reference: PositionVector,
  complexity: number = 0
): ModalRototranslationDistanceRow {
  var selection = modalSelection(scale, criterion, degree) as readonly { readonly modeIndex: number; readonly vector: PositionVector }[];
  var degrees = modalRototranslation(selection);
  var distances = calculateDistances(reference, degrees) as readonly ModalRototranslationDistanceRow[];
  return getByComplexity(distances, complexity);
}

export function voiceLeadingAutomation(
  reference: PositionVector,
  target: PositionVector,
  complexity: number = 0
): RototranslationDistanceRow {
  var center = calculateCenter(reference, target, 0);
  var positions = rototranslationMatrix(target, center);
  var distances = calculateDistances(reference, positions) as readonly RototranslationDistanceRow[];
  return getByComplexity(distances, complexity);
}

function calculateCenter(reference: PositionVector, target: PositionVector, fallbackCenter: number): number {
  try {
    return requireAlignedCenter(reference, target);
  } catch (_error) {
    return fallbackCenter;
  }
}

function requireAlignedCenter(reference: PositionVector, target: PositionVector): number {
  return align(reference, target);
}

export function modalInterchangeAutomation(scale: PositionVector, notes: readonly number[], complexity: number): ModalDistanceRow<PositionVector> {
  var modes = modalMatrix(scale) as readonly ModalDistanceRow<PositionVector>[];
  var filtered = filterModalMatrix(modes, notes);
  var distances = calculateDistances(scale, filtered) as readonly ModalDistanceRow<PositionVector>[];
  return getByComplexity(distances, complexity);
}

export function modulationAutomation(scale: PositionVector, notes: readonly number[], complexity: number): TranspositionDistanceRow {
  var transpositions = transpositionMatrix(scale);
  var filtered = filterTranspositionMatrix(transpositions, notes);
  var distances = calculateDistances(scale, filtered) as readonly TranspositionDistanceRow[];
  return getByComplexity(distances, complexity);
}

export function voiceLeadingAutomationVectorReference(
  targets: readonly PositionVector[],
  references: readonly PositionVector[],
  complexities: readonly number[] = []
): readonly PositionVector[] {
  if (targets.length !== references.length) {
    throw new Error("targets and references must have the same size");
  }

  var normalized = normalizeComplexityVector(complexities, targets.length);
  var result: PositionVector[] = [];
  for (var index = 0; index < targets.length; index += 1) {
    result.push(voiceLeadingAutomation(targets[index], references[index], normalized[index]).vector);
  }
  return result;
}

export function voiceLeadingAutomationReference(
  targets: readonly PositionVector[],
  reference: PositionVector,
  complexities: readonly number[] = []
): readonly PositionVector[] {
  var normalized = normalizeComplexityVector(complexities, targets.length);
  var result: PositionVector[] = [];
  for (var index = 0; index < targets.length; index += 1) {
    result.push(voiceLeadingAutomation(reference, targets[index], normalized[index]).vector);
  }
  return result;
}

export function forwardVoiceLeading(
  targets: readonly PositionVector[],
  complexities: readonly number[] = []
): readonly PositionVector[] {
  if (targets.length === 0) {
    throw new Error("targets vector cannot be empty");
  }

  var result: PositionVector[] = [targets[0]];
  if (targets.length === 1) {
    return result;
  }

  var normalized = normalizeComplexityVector(complexities, targets.length - 1);
  for (var index = 1; index < targets.length; index += 1) {
    result.push(voiceLeadingAutomation(result[index - 1], targets[index], normalized[index - 1]).vector);
  }
  return result;
}

export function voiceLeadingAutomationSequentialBackward(
  targets: readonly PositionVector[],
  complexities: readonly number[] = []
): readonly PositionVector[] {
  if (targets.length === 0) {
    throw new Error("targets vector cannot be empty");
  }

  var result: PositionVector[] = new Array(targets.length);
  result[targets.length - 1] = targets[targets.length - 1];
  if (targets.length === 1) {
    return result;
  }

  var normalized = normalizeComplexityVector(complexities, targets.length - 1);
  for (var index = targets.length - 2; index >= 0; index -= 1) {
    result[index] = voiceLeadingAutomation(result[index + 1], targets[index], normalized[index]).vector;
  }
  return result;
}

export function degreeAutomationReference(
  scale: PositionVector,
  criterion: IntervalVector,
  degrees: readonly number[],
  reference: PositionVector,
  complexities: readonly number[] = []
): readonly PositionVector[] {
  var normalized = normalizeComplexityVector(complexities, degrees.length);
  var result: PositionVector[] = [];
  for (var index = 0; index < degrees.length; index += 1) {
    result.push(degreeAutomation(scale, criterion, degrees[index], reference, normalized[index]).vector);
  }
  return result;
}

export function degreeAutomationVectorReference(
  scale: PositionVector,
  criterion: IntervalVector,
  degrees: readonly number[],
  references: readonly PositionVector[],
  complexities: readonly number[] = []
): readonly PositionVector[] {
  if (degrees.length !== references.length) {
    throw new Error("degrees and references must have the same size");
  }

  var normalized = normalizeComplexityVector(complexities, degrees.length);
  var result: PositionVector[] = [];
  for (var index = 0; index < degrees.length; index += 1) {
    result.push(degreeAutomation(scale, criterion, degrees[index], references[index], normalized[index]).vector);
  }
  return result;
}

export function forwardDegreeAutomation(
  scale: PositionVector,
  criterion: IntervalVector,
  degrees: readonly number[],
  initialReference: PositionVector,
  complexities: readonly number[] = []
): readonly PositionVector[] {
  if (degrees.length === 0) {
    throw new Error("degrees vector cannot be empty");
  }

  var normalized = normalizeComplexityVector(complexities, degrees.length);
  var result: PositionVector[] = [];
  result.push(degreeAutomation(scale, criterion, degrees[0], initialReference, normalized[0]).vector);

  for (var index = 1; index < degrees.length; index += 1) {
    result.push(degreeAutomation(scale, criterion, degrees[index], result[index - 1], normalized[index]).vector);
  }

  return result;
}

export function degreeAutomationSequentialBackward(
  scale: PositionVector,
  criterion: IntervalVector,
  degrees: readonly number[],
  finalReference: PositionVector,
  complexities: readonly number[] = []
): readonly PositionVector[] {
  if (degrees.length === 0) {
    throw new Error("degrees vector cannot be empty");
  }

  var normalized = normalizeComplexityVector(complexities, degrees.length);
  var result: PositionVector[] = new Array(degrees.length);
  result[degrees.length - 1] = degreeAutomation(
    scale,
    criterion,
    degrees[degrees.length - 1],
    finalReference,
    normalized[degrees.length - 1]
  ).vector;

  for (var index = degrees.length - 2; index >= 0; index -= 1) {
    result[index] = degreeAutomation(scale, criterion, degrees[index], result[index + 1], normalized[index]).vector;
  }

  return result;
}

export function autoScale(scale: PositionVector, notes: readonly number[]): PositionVector {
  var scaleData = scale.toArray();
  var pitchClasses: number[] = [];
  var used: boolean[] = [];
  var noteIndex = 0;

  for (noteIndex = 0; noteIndex < notes.length; noteIndex += 1) {
    pitchClasses.push(notes[noteIndex] % scale.mod);
  }

  for (noteIndex = 0; noteIndex < scaleData.length; noteIndex += 1) {
    used.push(false);
  }

  for (noteIndex = 0; noteIndex < pitchClasses.length; noteIndex += 1) {
    var closest = -1;
    var minimumDistance = scale.mod;
    var bestMaximumInterval = 999;

    for (var scaleIndex = 0; scaleIndex < scaleData.length; scaleIndex += 1) {
      if (used[scaleIndex]) {
        continue;
      }

      var scalePitchClass = scaleData[scaleIndex] % scale.mod;
      var upwardDistance = (pitchClasses[noteIndex] - scalePitchClass + scale.mod) % scale.mod;
      var downwardDistance = (scalePitchClass - pitchClasses[noteIndex] + scale.mod) % scale.mod;
      var distance = upwardDistance < downwardDistance ? upwardDistance : downwardDistance;

      if (distance < minimumDistance) {
        minimumDistance = distance;
        closest = scaleIndex;
        var tempScale = scaleData.slice();
        tempScale[scaleIndex] = Math.floor(tempScale[scaleIndex] / scale.mod) * scale.mod + pitchClasses[noteIndex];
        bestMaximumInterval = getMaxInterval(tempScale);
      } else if (distance === minimumDistance) {
        var alternativeScale = scaleData.slice();
        alternativeScale[scaleIndex] = Math.floor(alternativeScale[scaleIndex] / scale.mod) * scale.mod + pitchClasses[noteIndex];
        var maximumInterval = getMaxInterval(alternativeScale);
        if (maximumInterval < bestMaximumInterval) {
          closest = scaleIndex;
          bestMaximumInterval = maximumInterval;
        } else if (maximumInterval === bestMaximumInterval && closest !== -1) {
          var distanceToEdge = Math.min(scaleIndex, scaleData.length - 1 - scaleIndex);
          var closestDistanceToEdge = Math.min(closest, scaleData.length - 1 - closest);
          if (distanceToEdge < closestDistanceToEdge) {
            closest = scaleIndex;
            bestMaximumInterval = maximumInterval;
          }
        }
      }
    }

    if (closest !== -1) {
      scaleData[closest] = Math.floor(scaleData[closest] / scale.mod) * scale.mod + pitchClasses[noteIndex];
      used[closest] = true;
    }
  }

  return new PositionVector(scaleData, {
    mod: scale.mod,
    rangeUpdate: scale.rangeUpdate,
    user: scale.user,
    userRange: scale.userRange
  });
}
