export interface NoteInfo {
  readonly position: number;
  readonly degree: number;
  readonly octave: number;
  readonly noteValue: number;
}

export interface MelodyAnalysis {
  readonly chord: NoteInfo;
  readonly scale: NoteInfo;
  readonly chromatic: NoteInfo;
}

export type Analysis = MelodyAnalysis;

export type VectorModificationType = "chord" | "scale" | "chromatic";

export interface VectorModification {
  readonly type: VectorModificationType;
  readonly delta: number;
}

export interface TripleSelectResult {
  readonly results: readonly number[];
  readonly isOut: readonly boolean[];
}

export interface AppliedTripleSelectResult {
  readonly note: number;
  readonly selection: TripleSelectResult;
}

function invalidNote(): NoteInfo {
  return {
    degree: -666,
    noteValue: -666,
    octave: -666,
    position: -666
  };
}

function cloneNumberArray(values: readonly number[]): number[] {
  return values.slice();
}

function sortAscending(values: readonly number[]): number[] {
  return cloneNumberArray(values).sort(function (left, right) {
    return left - right;
  });
}

function sortDescending(values: readonly number[]): number[] {
  return cloneNumberArray(values).sort(function (left, right) {
    return right - left;
  });
}

function noteInfo(inputNote: number, value: readonly number[], mod: number): NoteInfo {
  var normalizedNote = inputNote % mod;
  var baseOctave = Math.floor(inputNote / mod);
  var degree = -1;

  for (var index = 0; index < value.length; index += 1) {
    if ((value[index] % mod) === normalizedNote) {
      degree = index;
      break;
    }
  }

  if (degree === -1) {
    return invalidNote();
  }

  var octaveAdjustment = Math.floor(value[degree] / mod);
  var octave = baseOctave + octaveAdjustment;

  return {
    degree: degree,
    noteValue: inputNote,
    octave: octave,
    position: (octave * value.length) + degree
  };
}

export function diminution(degree: number, length: number, up: boolean, left: boolean): number[] {
  if (length <= 0) {
    return [];
  }
  if (length === 1) {
    return [degree];
  }

  var result: number[] = new Array(length);
  var middle: number[] = [];
  var distance = length - 2;
  var firstNumber = up ? degree - distance : degree + distance;

  result[0] = degree;
  result[length - 1] = degree;

  for (var index = 1; index < length - 1; index += 1) {
    middle.push(up ? firstNumber + (index - 1) : firstNumber - (index - 1));
  }

  middle = left ? sortAscending(middle) : sortDescending(middle);
  for (index = 0; index < middle.length; index += 1) {
    result[index + 1] = middle[index];
  }

  return result;
}

export function run(degree: number, length: number, direction: boolean): number[] {
  var step = direction ? 1 : -1;
  var start = direction ? degree - (length - 1) : degree + (length - 1);
  var result: number[] = [];

  for (var index = 0; index < length; index += 1) {
    result.push(start + (index * step));
  }

  return result;
}

export function run2(start: number, end: number): number[] {
  var result: number[] = [];

  if (start <= end) {
    for (var value = start; value <= end; value += 1) {
      result.push(value);
    }
    return result;
  }

  for (value = start; value >= end; value -= 1) {
    result.push(value);
  }

  return result;
}

export const runBetween = run2;

export function normalizeNotes(notes: readonly number[], mod: number): number[] {
  var result: number[] = [];

  for (var index = 0; index < notes.length; index += 1) {
    var value = notes[index] % mod;
    result.push(value < 0 ? value + mod : value);
  }

  return result;
}

export function hierarchy(
  note: number,
  chord: readonly number[],
  scale: readonly number[],
  chromatic: readonly number[],
  mod: number
): MelodyAnalysis {
  var normalizedChord = normalizeNotes(chord, mod);

  return {
    chord: noteInfo(note, normalizedChord, mod),
    chromatic: noteInfo(note, chromatic, mod),
    scale: noteInfo(note, scale, mod)
  };
}

export function noteFromPosition(position: number, value: readonly number[], mod: number): number {
  var vectorLength = value.length;
  var octave = Math.floor(position / vectorLength);
  var degree = ((position % vectorLength) + vectorLength) % vectorLength;
  return value[degree] + (octave * mod);
}

export function tripleSelect(
  analysis: MelodyAnalysis,
  modifications: readonly VectorModification[],
  chord: readonly number[],
  scale: readonly number[],
  chromatic: readonly number[],
  mod: number
): TripleSelectResult {
  var results: number[] = [];
  var isOut: boolean[] = [];

  for (var index = 0; index < modifications.length; index += 1) {
    var modification = modifications[index];
    var basePosition = 0;
    var source = chord;

    if (modification.type === "chord") {
      basePosition = analysis.chord.position;
      source = chord;
    } else if (modification.type === "scale") {
      basePosition = analysis.scale.position;
      source = scale;
    } else if (modification.type === "chromatic") {
      basePosition = analysis.chromatic.position;
      source = chromatic;
    } else {
      throw new Error("Invalid modification type");
    }

    if (basePosition === -666) {
      results.push(0);
      isOut.push(true);
      continue;
    }

    results.push(noteFromPosition(basePosition + modification.delta, source, mod));
    isOut.push(false);
  }

  return {
    isOut: isOut,
    results: results
  };
}

export function parseModifications(
  deltas: readonly number[],
  types: readonly VectorModificationType[]
): VectorModification[] {
  var resolvedTypes = types.slice();

  if (resolvedTypes.length === 1) {
    resolvedTypes = [];
    for (var index = 0; index < deltas.length; index += 1) {
      resolvedTypes.push(types[0]);
    }
  }

  if (deltas.length !== resolvedTypes.length) {
    throw new Error("Deltas and types must have the same length.");
  }

  var modifications: VectorModification[] = [];
  for (index = 0; index < deltas.length; index += 1) {
    modifications.push({
      delta: deltas[index],
      type: resolvedTypes[index]
    });
  }

  return modifications;
}

export function ornamentLoop(values: readonly number[], length: number): number[] {
  if (length <= 0) {
    return [];
  }

  var result: number[] = [];
  var prefix = cloneNumberArray(values);

  while (result.length < length) {
    result = prefix.concat(result);
  }

  if (result.length > length) {
    result = result.slice(result.length - length);
  }

  return result;
}

export function ornamentTypesLoop(types: readonly VectorModificationType[], length: number): VectorModificationType[] {
  if (length <= 0) {
    return [];
  }

  var result: VectorModificationType[] = [];
  var prefix = types.slice();

  while (result.length < length) {
    result = prefix.concat(result);
  }

  if (result.length > length) {
    result = result.slice(result.length - length);
  }

  return result;
}

export function applyTripleSelect(
  notes: readonly number[],
  chord: readonly number[],
  scale: readonly number[],
  chromatic: readonly number[],
  mod: number,
  ornaments: readonly number[],
  types: readonly VectorModificationType[]
): AppliedTripleSelectResult[] {
  var expandedTypes = ornamentTypesLoop(types, ornaments.length);
  var modifications = parseModifications(ornaments, expandedTypes);
  var results: AppliedTripleSelectResult[] = [];

  for (var index = 0; index < notes.length; index += 1) {
    var analysis = hierarchy(notes[index], chord, scale, chromatic, mod);
    results.push({
      note: notes[index],
      selection: tripleSelect(analysis, modifications, chord, scale, chromatic, mod)
    });
  }

  return results;
}
