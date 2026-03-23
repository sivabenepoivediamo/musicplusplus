import { euclideanDivision } from "./internal";
import { PositionVector } from "./positionVector";

export function align(reference: PositionVector, target: PositionVector): number {
  var minimumValue = reference.at(0);
  var referenceDivision = euclideanDivision(reference.at(0), reference.range);
  var targetDivision = euclideanDivision(target.at(0), target.range);
  var differenceInOctaves = referenceDivision.quotient - targetDivision.remainder;
  var index = differenceInOctaves * target.size;

  while (target.at(index) <= minimumValue) {
    index += 1;
  }
  while (target.at(index) > minimumValue) {
    index -= 1;
  }

  return index;
}

export function pitchClass(note: number, mod: number): number {
  return euclideanDivision(note, mod).remainder;
}

export function containsAllNotesModulo(vector: PositionVector, notes: readonly number[]): boolean {
  var mod = vector.mod;
  var data = vector.toArray();
  var noteIndex = 0;
  var positionIndex = 0;

  for (noteIndex = 0; noteIndex < notes.length; noteIndex += 1) {
    var normalizedNote = pitchClass(notes[noteIndex], mod);
    var found = false;
    for (positionIndex = 0; positionIndex < data.length; positionIndex += 1) {
      if (pitchClass(data[positionIndex], mod) === normalizedNote) {
        found = true;
        break;
      }
    }
    if (!found) {
      return false;
    }
  }

  return true;
}

export function getMaxInterval(scale: readonly number[]): number {
  var maximum = 0;
  for (var index = 1; index < scale.length; index += 1) {
    var interval = scale[index] - scale[index - 1];
    if (interval > maximum) {
      maximum = interval;
    }
  }
  return maximum;
}
