import {
  align,
  containsAllNotesModulo,
  IntervalVector,
  intervalsToPositions,
  manhattanDistance,
  PositionVector,
  positionsToIntervals
} from "../core";
import {
  chord
} from "../harmony";

export interface ModalMatrixRow<T> {
  readonly index: number;
  readonly vector: T;
}

export interface ModalDistanceRow<T> extends ModalMatrixRow<T> {
  readonly distance: number;
}

export interface TranspositionRow {
  readonly transposition: number;
  readonly vector: PositionVector;
}

export interface TranspositionDistanceRow extends TranspositionRow {
  readonly distance: number;
}

export interface RototranslationRow {
  readonly translation: number;
  readonly vector: PositionVector;
}

export interface RototranslationDistanceRow extends RototranslationRow {
  readonly center: number;
  readonly distance: number;
}

export interface RototranslationMatrix {
  readonly center: number;
  readonly rows: readonly RototranslationRow[];
}

export interface ModalSelectionRow<T> {
  readonly modeIndex: number;
  readonly vector: T;
}

export interface ModalSelectionDistanceRow<T> extends ModalSelectionRow<T> {
  readonly distance: number;
}

export interface ModalRototranslationMatrixRow {
  readonly modeIndex: number;
  readonly matrix: RototranslationMatrix;
}

export interface ModalRototranslationDistanceRow {
  readonly modeIndex: number;
  readonly translationIndex: number;
  readonly vector: PositionVector;
  readonly distance: number;
}

function sortDistanceRows<T extends { readonly distance: number }>(rows: readonly T[]): T[] {
  return rows.slice().sort(function (left, right) {
    return left.distance - right.distance;
  });
}

function assertComplexity(complexity: number): void {
  if (complexity < 0 || complexity > 100) {
    throw new Error("Complexity must be between 0 and 100");
  }
}

function positionOptions(vector: PositionVector): {
  readonly mod: number;
  readonly userRange: number;
  readonly rangeUpdate: boolean;
  readonly user: boolean;
} {
  return {
    mod: vector.mod,
    rangeUpdate: vector.rangeUpdate,
    user: vector.user,
    userRange: vector.userRange
  };
}

export function modalMatrix(value: PositionVector): readonly ModalMatrixRow<PositionVector>[];
export function modalMatrix(value: IntervalVector): readonly ModalMatrixRow<IntervalVector>[];
export function modalMatrix(value: PositionVector | IntervalVector): readonly ModalMatrixRow<PositionVector | IntervalVector>[] {
  var result: Array<ModalMatrixRow<PositionVector | IntervalVector>> = [];
  var index = 0;

  if (value instanceof IntervalVector) {
    for (index = 0; index < value.size; index += 1) {
      result.push({
        index: index,
        vector: value.rotate(index)
      });
    }
    return result;
  }

  var intervalMatrix = modalMatrix(positionsToIntervals(value));
  for (index = 0; index < intervalMatrix.length; index += 1) {
    result.push({
      index: intervalMatrix[index].index,
      vector: intervalsToPositions(intervalMatrix[index].vector as IntervalVector)
    });
  }

  return result;
}

export function rototranslationMatrix(value: PositionVector, center: number = 0): RototranslationMatrix {
  var rows: RototranslationRow[] = [];
  var size = value.size;

  for (var translation = center - size; translation < center + size + 1; translation += 1) {
    rows.push({
      translation: translation,
      vector: value.rotoTranslate(translation)
    });
  }

  return {
    center: center,
    rows: rows
  };
}

export function transpositionMatrix(value: PositionVector): readonly TranspositionRow[] {
  var rows: TranspositionRow[] = [];

  for (var transposition = 0; transposition < value.mod; transposition += 1) {
    var transposed = value.addScalar(transposition).moduloScalar(value.mod);
    rows.push({
      transposition: transposition,
      vector: new PositionVector(transposed.data, positionOptions(transposed))
    });
  }

  return rows.map(function (row) {
    return {
      transposition: row.transposition,
      vector: new PositionVector(row.vector.data.slice().sort(function (left, right) {
        return left - right;
      }), positionOptions(row.vector))
    };
  });
}

export function modalSelection(source: IntervalVector, criterion: IntervalVector, degree?: number): readonly ModalSelectionRow<IntervalVector>[];
export function modalSelection(source: PositionVector, criterion: IntervalVector, degree?: number): readonly ModalSelectionRow<PositionVector>[];
export function modalSelection(
  source: PositionVector | IntervalVector,
  criterion: IntervalVector,
  degree: number = 0
): readonly ModalSelectionRow<PositionVector | IntervalVector>[] {
  var modes = modalMatrix(criterion) as readonly ModalMatrixRow<IntervalVector>[];
  var result: Array<ModalSelectionRow<PositionVector | IntervalVector>> = [];
  var rowIndex = 0;

  if (source instanceof IntervalVector) {
    for (rowIndex = 0; rowIndex < modes.length; rowIndex += 1) {
      var intervalCandidate = chord(source, modes[rowIndex].vector, { shift: degree }).toIntervals();
      var intervalSum = 0;
      for (var sumIndex = 0; sumIndex < rowIndex; sumIndex += 1) {
        intervalSum += criterion.at(sumIndex);
      }
      var modeIndex = ((degree - intervalSum) % source.size + source.size) % source.size;
      result.push({
        modeIndex: modeIndex,
        vector: intervalCandidate
      });
    }
    return result;
  }

  var intervalSource = positionsToIntervals(source);
  for (rowIndex = 0; rowIndex < modes.length; rowIndex += 1) {
    var positionCandidate = chord(intervalSource, modes[rowIndex].vector, { shift: degree }).toPositions();
    var accumulated = 0;
    for (var criterionIndex = 0; criterionIndex < rowIndex; criterionIndex += 1) {
      accumulated += criterion.at(criterionIndex);
    }
    var sourceModeIndex = ((degree - accumulated) % source.size + source.size) % source.size;
    result.push({
      modeIndex: sourceModeIndex,
      vector: positionCandidate
    });
  }

  return result;
}

export function modalRototranslation(selection: readonly ModalSelectionRow<PositionVector>[]): readonly ModalRototranslationMatrixRow[] {
  var result: ModalRototranslationMatrixRow[] = [];

  for (var index = 0; index < selection.length; index += 1) {
    result.push({
      matrix: rototranslationMatrix(selection[index].vector, 0),
      modeIndex: selection[index].modeIndex
    });
  }

  return result;
}

export function filterModalMatrix(matrix: readonly ModalMatrixRow<PositionVector>[], notes: readonly number[]): readonly ModalMatrixRow<PositionVector>[] {
  if (notes.length === 0) {
    return matrix.slice();
  }

  var result: ModalMatrixRow<PositionVector>[] = [];
  for (var index = 0; index < matrix.length; index += 1) {
    if (containsAllNotesModulo(matrix[index].vector, notes)) {
      result.push(matrix[index]);
    }
  }
  return result;
}

export function filterTranspositionMatrix(matrix: readonly TranspositionRow[], notes: readonly number[]): readonly TranspositionRow[] {
  if (notes.length === 0) {
    return matrix.slice();
  }

  var result: TranspositionRow[] = [];
  for (var index = 0; index < matrix.length; index += 1) {
    if (containsAllNotesModulo(matrix[index].vector, notes)) {
      result.push(matrix[index]);
    }
  }
  return result;
}

export function calculateDistances(
  reference: PositionVector,
  matrix: readonly ModalMatrixRow<PositionVector>[],
  distanceFn?: (left: PositionVector, right: PositionVector) => number,
  sort?: boolean
): readonly ModalDistanceRow<PositionVector>[];
export function calculateDistances(
  reference: IntervalVector,
  matrix: readonly ModalMatrixRow<IntervalVector>[],
  distanceFn?: (left: IntervalVector, right: IntervalVector) => number,
  sort?: boolean
): readonly ModalDistanceRow<IntervalVector>[];
export function calculateDistances(
  reference: PositionVector,
  matrix: readonly TranspositionRow[],
  distanceFn?: (left: PositionVector, right: PositionVector) => number,
  sort?: boolean
): readonly TranspositionDistanceRow[];
export function calculateDistances(
  reference: PositionVector,
  matrix: RototranslationMatrix,
  distanceFn?: (left: PositionVector, right: PositionVector) => number,
  sort?: boolean
): readonly RototranslationDistanceRow[];
export function calculateDistances(
  reference: PositionVector,
  matrix: readonly ModalSelectionRow<PositionVector>[],
  distanceFn?: (left: PositionVector, right: PositionVector) => number,
  sort?: boolean
): readonly ModalSelectionDistanceRow<PositionVector>[];
export function calculateDistances(
  reference: IntervalVector,
  matrix: readonly ModalSelectionRow<IntervalVector>[],
  distanceFn?: (left: IntervalVector, right: IntervalVector) => number,
  sort?: boolean
): readonly ModalSelectionDistanceRow<IntervalVector>[];
export function calculateDistances(
  reference: PositionVector,
  matrix: readonly ModalRototranslationMatrixRow[],
  distanceFn?: (left: PositionVector, right: PositionVector) => number,
  sort?: boolean
): readonly ModalRototranslationDistanceRow[];
export function calculateDistances(
  reference: PositionVector | IntervalVector,
  matrix:
    | readonly ModalMatrixRow<PositionVector>[]
    | readonly ModalMatrixRow<IntervalVector>[]
    | readonly TranspositionRow[]
    | RototranslationMatrix
    | readonly ModalSelectionRow<PositionVector>[]
    | readonly ModalSelectionRow<IntervalVector>[]
    | readonly ModalRototranslationMatrixRow[],
  distanceFn?: ((left: PositionVector, right: PositionVector) => number) | ((left: IntervalVector, right: IntervalVector) => number),
  sort: boolean = true
):
  | readonly ModalDistanceRow<PositionVector>[]
  | readonly ModalDistanceRow<IntervalVector>[]
  | readonly TranspositionDistanceRow[]
  | readonly RototranslationDistanceRow[]
  | readonly ModalSelectionDistanceRow<PositionVector>[]
  | readonly ModalSelectionDistanceRow<IntervalVector>[]
  | readonly ModalRototranslationDistanceRow[] {
  var resolvedDistance = (distanceFn || manhattanDistance) as any;

  if ((matrix as RototranslationMatrix).rows !== undefined) {
    var rotationMatrix = matrix as RototranslationMatrix;
    var rotationRows: RototranslationDistanceRow[] = [];
    for (var rotationIndex = 0; rotationIndex < rotationMatrix.rows.length; rotationIndex += 1) {
      rotationRows.push({
        center: rotationMatrix.center,
        distance: resolvedDistance(reference as any, rotationMatrix.rows[rotationIndex].vector),
        translation: rotationMatrix.rows[rotationIndex].translation,
        vector: rotationMatrix.rows[rotationIndex].vector
      });
    }
    return sort ? sortDistanceRows(rotationRows) : rotationRows;
  }

  var rows = matrix as unknown as ReadonlyArray<Record<string, unknown>>;
  if (rows.length === 0) {
    return [];
  }

  if (rows[0].matrix !== undefined) {
    var flattened: ModalRototranslationDistanceRow[] = [];
    var modalRotations = matrix as readonly ModalRototranslationMatrixRow[];
    for (var modeIndex = 0; modeIndex < modalRotations.length; modeIndex += 1) {
      for (var translationIndex = 0; translationIndex < modalRotations[modeIndex].matrix.rows.length; translationIndex += 1) {
        flattened.push({
          distance: resolvedDistance(reference as any, modalRotations[modeIndex].matrix.rows[translationIndex].vector),
          modeIndex: modalRotations[modeIndex].modeIndex,
          translationIndex: modalRotations[modeIndex].matrix.rows[translationIndex].translation,
          vector: modalRotations[modeIndex].matrix.rows[translationIndex].vector
        });
      }
    }
    return sort ? sortDistanceRows(flattened) : flattened;
  }

  if (rows[0].transposition !== undefined) {
    var transpositionRows = matrix as readonly TranspositionRow[];
    var transpositionDistances: TranspositionDistanceRow[] = [];
    for (var transpositionIndex = 0; transpositionIndex < transpositionRows.length; transpositionIndex += 1) {
      transpositionDistances.push({
        distance: resolvedDistance(reference as any, transpositionRows[transpositionIndex].vector),
        transposition: transpositionRows[transpositionIndex].transposition,
        vector: transpositionRows[transpositionIndex].vector
      });
    }
    return sort ? sortDistanceRows(transpositionDistances) : transpositionDistances;
  }

  if (rows[0].modeIndex !== undefined) {
    var selectionRows = matrix as ReadonlyArray<ModalSelectionRow<PositionVector> | ModalSelectionRow<IntervalVector>>;
    var selectionDistances: any[] = [];
    for (var selectionIndex = 0; selectionIndex < selectionRows.length; selectionIndex += 1) {
      selectionDistances.push({
        distance: resolvedDistance(reference as any, selectionRows[selectionIndex].vector as any),
        modeIndex: selectionRows[selectionIndex].modeIndex,
        vector: selectionRows[selectionIndex].vector
      });
    }
    return (sort ? sortDistanceRows(selectionDistances) : selectionDistances) as unknown as
      | readonly ModalSelectionDistanceRow<PositionVector>[]
      | readonly ModalSelectionDistanceRow<IntervalVector>[];
  }

  var modalRows = matrix as ReadonlyArray<ModalMatrixRow<PositionVector> | ModalMatrixRow<IntervalVector>>;
  var modalDistances: any[] = [];
  for (var index = 0; index < modalRows.length; index += 1) {
    modalDistances.push({
      distance: resolvedDistance(reference as any, modalRows[index].vector as any),
      index: modalRows[index].index,
      vector: modalRows[index].vector
    });
  }
  return (sort ? sortDistanceRows(modalDistances) : modalDistances) as unknown as
    | readonly ModalDistanceRow<PositionVector>[]
    | readonly ModalDistanceRow<IntervalVector>[];
}

export function getClosest<T extends { readonly distance: number }>(rows: readonly T[]): T {
  if (rows.length === 0) {
    throw new Error("Cannot get closest from empty matrix");
  }
  return sortDistanceRows(rows)[0];
}

export function getFurthest<T extends { readonly distance: number }>(rows: readonly T[]): T {
  if (rows.length === 0) {
    throw new Error("Cannot get furthest from empty matrix");
  }
  var sorted = sortDistanceRows(rows);
  return sorted[sorted.length - 1];
}

export function getByComplexity<T extends { readonly distance: number }>(rows: readonly T[], complexity: number = 0): T {
  if (rows.length === 0) {
    throw new Error("Cannot get by complexity from empty matrix");
  }
  assertComplexity(complexity);
  var sorted = sortDistanceRows(rows);
  var index = Math.floor((complexity / 100) * (sorted.length - 1));
  return sorted[index];
}

export { align };
