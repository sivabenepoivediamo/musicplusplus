import { PositionVector } from "../core";
import type { QuantizeTransposeOptions, QuantizeTransposeResult } from "./types";
import { truncateTowardZero } from "./shared";

export function quantize(note: number, scale: readonly number[], left: boolean = true): number {
  var lower = -1;
  var upper = -1;

  for (var index = 0; index < scale.length; index += 1) {
    if (scale[index] <= note) {
      lower = scale[index];
    }
    if (scale[index] >= note) {
      upper = scale[index];
      break;
    }
  }

  if (lower === -1) {
    return upper;
  }
  if (upper === -1) {
    return lower;
  }

  return left ? lower : upper;
}

export function transposeNotes(
  inputScale: PositionVector,
  outputScale: PositionVector,
  notes: readonly number[],
  options: QuantizeTransposeOptions = {}
): QuantizeTransposeResult {
  var inScale = inputScale.toArray();
  var outScale = outputScale.toArray();
  var mod = inputScale.mod;
  var inRoot = typeof options.inRoot === "number" ? options.inRoot : 0;
  var outRoot = typeof options.outRoot === "number" ? options.outRoot : 0;
  var degreesData: number[] = [];
  var notesData: number[] = [];
  var length = outScale.length;

  for (var index = 0; index < notes.length; index += 1) {
    var note = notes[index];
    var inPitchClass = (note - inRoot) % mod;
    if (inPitchClass < 0) {
      inPitchClass += mod;
    }

    var octave = truncateTowardZero((note - inRoot) / mod);
    if (((note - inRoot) < 0) && (((note - inRoot) % mod) !== 0)) {
      octave -= 1;
    }

    var useLeft = true;
    var degreeIndex = inScale.indexOf(inPitchClass);
    if (degreeIndex === -1) {
      inPitchClass = quantize(inPitchClass, inScale, useLeft);
      degreeIndex = inScale.indexOf(inPitchClass);
    }

    if (degreeIndex !== -1) {
      var degree = degreeIndex;
      var outPitchClass = outScale[degree % length];
      var outNote = outPitchClass + outRoot + (octave * mod);

      if (notesData.length > 0 && notesData[notesData.length - 1] === outNote) {
        if (index > 0 && notes[index] !== notes[index - 1]) {
          useLeft = !useLeft;
          inPitchClass = (notes[index] - inRoot) % mod;
          if (inPitchClass < 0) {
            inPitchClass += mod;
          }
          inPitchClass = quantize(inPitchClass, inScale, useLeft);
          degreeIndex = inScale.indexOf(inPitchClass);
          if (degreeIndex !== -1) {
            degree = degreeIndex;
            outPitchClass = outScale[degree % length];
            outNote = outPitchClass + outRoot + (octave * mod);
          }
        }
      }

      notesData.push(outNote);
      degreesData.push(degree);
    }
  }

  return {
    degrees: new PositionVector(degreesData, {
      mod: inputScale.size,
      rangeUpdate: inputScale.rangeUpdate,
      user: inputScale.user,
      userRange: inputScale.userRange
    }),
    notes: new PositionVector(notesData, {
      mod: inputScale.mod,
      rangeUpdate: inputScale.rangeUpdate,
      user: inputScale.user,
      userRange: inputScale.userRange
    })
  };
}
