export type SlonimskySequence = readonly number[];

function cloneNumberArray(values: readonly number[]): number[] {
  return values.slice();
}

export function intervals(sequence: SlonimskySequence): number[] {
  var result: number[] = [];

  for (var index = 0; index < sequence.length - 1; index += 1) {
    result.push(sequence[index + 1] - sequence[index]);
  }

  return result;
}

export function minInterval(sequence: SlonimskySequence): number {
  var sequenceIntervals = intervals(sequence);
  var current = sequenceIntervals[0];

  for (var index = 1; index < sequenceIntervals.length; index += 1) {
    if (sequenceIntervals[index] < current) {
      current = sequenceIntervals[index];
    }
  }

  return current;
}

export function infrapolationPermutation(m: number): number[] {
  var half = Math.floor((m + 1) / 2);
  var delta = (m % 2) === 0 ? 1 : 0;
  var sigma: number[] = new Array(m);

  for (var index = 1; index <= m; index += 1) {
    var value = index <= half ? m - (2 * (index - 1)) : (2 * (index - half)) - delta;
    sigma[index - 1] = value - 1;
  }

  return sigma;
}

export function interpolation(sequence: SlonimskySequence, k: number | SlonimskySequence): number[] {
  var result: number[] = [];
  var offsets = typeof k === "number" ? [k] : cloneNumberArray(k);
  var index = 0;
  var offsetIndex = 0;

  for (index = 0; index < sequence.length - 1; index += 1) {
    result.push(sequence[index]);
    for (offsetIndex = 0; offsetIndex < offsets.length; offsetIndex += 1) {
      result.push(sequence[index] + offsets[offsetIndex]);
    }
  }

  if (sequence.length > 0) {
    result.push(sequence[sequence.length - 1]);
  }

  return result;
}

export function symmetricInterpolation(sequence: SlonimskySequence, k: number | SlonimskySequence): number[] {
  var offsets = typeof k === "number" ? [k] : cloneNumberArray(k);
  var pairs = Math.floor((sequence.length - 1) / 2);
  var result: number[] = [];
  var pairIndex = 0;
  var offsetIndex = 0;

  for (pairIndex = 0; pairIndex < pairs; pairIndex += 1) {
    result.push(sequence[2 * pairIndex]);
    for (offsetIndex = 0; offsetIndex < offsets.length; offsetIndex += 1) {
      result.push(sequence[2 * pairIndex] + offsets[offsetIndex]);
    }
    result.push(sequence[(2 * pairIndex) + 1]);
    for (offsetIndex = offsets.length - 1; offsetIndex >= 0; offsetIndex -= 1) {
      result.push(sequence[(2 * pairIndex) + 2] - offsets[offsetIndex]);
    }
  }

  if ((sequence.length % 2) === 1 && sequence.length >= 2) {
    result.push(sequence[sequence.length - 2]);
    for (offsetIndex = 0; offsetIndex < offsets.length; offsetIndex += 1) {
      result.push(sequence[sequence.length - 2] + offsets[offsetIndex]);
    }
    result.push(sequence[sequence.length - 1]);
  } else if (sequence.length > 0) {
    result.push(sequence[sequence.length - 1]);
  }

  return result;
}

export function asymmetricInterpolation(
  sequence: SlonimskySequence,
  upperOffsets: SlonimskySequence,
  lowerOffsets: SlonimskySequence
): number[] {
  var pairs = Math.floor((sequence.length - 1) / 2);
  var result: number[] = [];
  var pairIndex = 0;
  var offsetIndex = 0;

  for (pairIndex = 0; pairIndex < pairs; pairIndex += 1) {
    result.push(sequence[2 * pairIndex]);
    for (offsetIndex = 0; offsetIndex < upperOffsets.length; offsetIndex += 1) {
      result.push(sequence[2 * pairIndex] + upperOffsets[offsetIndex]);
    }
    result.push(sequence[(2 * pairIndex) + 1]);
    for (offsetIndex = lowerOffsets.length - 1; offsetIndex >= 0; offsetIndex -= 1) {
      result.push(sequence[(2 * pairIndex) + 2] - lowerOffsets[offsetIndex]);
    }
  }

  if ((sequence.length % 2) === 1 && sequence.length >= 2) {
    result.push(sequence[sequence.length - 2]);
    for (offsetIndex = 0; offsetIndex < upperOffsets.length; offsetIndex += 1) {
      result.push(sequence[sequence.length - 2] + upperOffsets[offsetIndex]);
    }
    result.push(sequence[sequence.length - 1]);
  } else if (sequence.length > 0) {
    result.push(sequence[sequence.length - 1]);
  }

  return result;
}

export function ultrapolation(sequence: SlonimskySequence, k: number | SlonimskySequence): number[] {
  var result: number[] = [];
  var offsets = typeof k === "number" ? [k] : cloneNumberArray(k);
  var index = 0;
  var offsetIndex = 0;

  if (sequence.length > 0) {
    result.push(sequence[0]);
  }

  for (index = 1; index < sequence.length; index += 1) {
    for (offsetIndex = offsets.length - 1; offsetIndex >= 0; offsetIndex -= 1) {
      result.push(sequence[index] + offsets[offsetIndex]);
    }
    result.push(sequence[index]);
  }

  return result;
}

export function infrapolation(sequence: SlonimskySequence, k: number | SlonimskySequence): number[] {
  var result: number[] = [];
  var offsets = typeof k === "number" ? [k] : cloneNumberArray(k);
  var index = 0;
  var offsetIndex = 0;

  for (index = 0; index < sequence.length - 1; index += 1) {
    result.push(sequence[index]);
    for (offsetIndex = 0; offsetIndex < offsets.length; offsetIndex += 1) {
      result.push(sequence[index] - offsets[offsetIndex]);
    }
  }

  if (sequence.length > 0) {
    result.push(sequence[sequence.length - 1]);
  }

  return result;
}

export function infraInterpolation(sequence: SlonimskySequence, k: number, l: number): number[] {
  var result: number[] = [];

  for (var index = 0; index < sequence.length - 1; index += 1) {
    result.push(sequence[index]);
    result.push(sequence[index] - k);
    result.push(sequence[index] + l);
  }

  if (sequence.length > 0) {
    result.push(sequence[sequence.length - 1]);
  }

  return result;
}

export function interInfrapolation(sequence: SlonimskySequence, k: number, l: number): number[] {
  var result: number[] = [];

  for (var index = 0; index < sequence.length - 1; index += 1) {
    result.push(sequence[index]);
    result.push(sequence[index] + k);
    result.push(sequence[index] - l);
  }

  if (sequence.length > 0) {
    result.push(sequence[sequence.length - 1]);
  }

  return result;
}

export function infraUltrapolation(sequence: SlonimskySequence, k: number, l: number): number[] {
  var result: number[] = [];

  if (sequence.length > 0) {
    result.push(sequence[0]);
    result.push(sequence[0] - k);
  }

  for (var index = 1; index < sequence.length - 1; index += 1) {
    result.push(sequence[index] + l);
    result.push(sequence[index]);
    result.push(sequence[index] - k);
  }

  if (sequence.length > 1) {
    result.push(sequence[sequence.length - 1] + l);
    result.push(sequence[sequence.length - 1]);
  }

  return result;
}

export function interUltrapolation(sequence: SlonimskySequence, k: number, l: number): number[] {
  var result: number[] = [];

  for (var index = 0; index < sequence.length - 1; index += 1) {
    result.push(sequence[index]);
    result.push(sequence[index] + k);
    result.push(sequence[index + 1] - l);
  }

  if (sequence.length > 0) {
    result.push(sequence[sequence.length - 1]);
  }

  return result;
}

export function ultraInterpolation(sequence: SlonimskySequence, k: number, l: number): number[] {
  var result: number[] = [];

  for (var index = 0; index < sequence.length - 1; index += 1) {
    result.push(sequence[index]);
    result.push(sequence[index + 1] + k);
    result.push(sequence[index] + l);
  }

  if (sequence.length > 0) {
    result.push(sequence[sequence.length - 1]);
  }

  return result;
}

export function infraInterUltrapolation(sequence: SlonimskySequence, k: number, l: number, m: number): number[] {
  var result: number[] = [];

  for (var index = 0; index < sequence.length - 1; index += 1) {
    result.push(sequence[index]);
    result.push(sequence[index] - k);
    result.push(sequence[index] + l);
    result.push(sequence[index + 1] + m);
  }

  if (sequence.length > 0) {
    result.push(sequence[sequence.length - 1]);
  }

  return result;
}

export function interInfraInterpolation(sequence: SlonimskySequence, k: number, l: number, m: number): number[] {
  var result: number[] = [];

  for (var index = 0; index < sequence.length - 1; index += 1) {
    result.push(sequence[index]);
    result.push(sequence[index + 1] - k);
    result.push(sequence[index] - l);
    result.push(sequence[index] + m);
  }

  if (sequence.length > 0) {
    result.push(sequence[sequence.length - 1]);
  }

  return result;
}

export function ultraInfraInterpolation(sequence: SlonimskySequence, k: number, l: number, m: number): number[] {
  var result: number[] = [];

  for (var index = 0; index < sequence.length - 1; index += 1) {
    result.push(sequence[index]);
    result.push(sequence[index + 1] + k);
    result.push(sequence[index] - l);
    result.push(sequence[index] + m);
  }

  if (sequence.length > 0) {
    result.push(sequence[sequence.length - 1]);
  }

  return result;
}
