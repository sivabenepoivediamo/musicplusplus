import type { BinaryVector, PositionVector } from "../core";

export interface OccurrenceCount {
  readonly distance: number;
  readonly count: number;
}

export interface GeneratedResult {
  readonly found: boolean;
  readonly multiplier: number;
}

export type AksakClassification = "authentic aksak" | "quasi-aksak" | "pseudo-aksak";

export interface QuantizeTransposeOptions {
  readonly inRoot?: number;
  readonly outRoot?: number;
}

export interface QuantizeTransposeResult {
  readonly degrees: PositionVector;
  readonly notes: PositionVector;
}

export interface MeasureSummary {
  readonly positions: ReturnType<PositionVector["toJSON"]>;
  readonly intervals: readonly number[];
  readonly onsets: ReturnType<BinaryVector["toJSON"]>;
  readonly differences: readonly number[];
  readonly geodesicDistances: readonly number[];
  readonly occurrences: readonly OccurrenceCount[];
  readonly winogradDeep: boolean;
  readonly erdosDeep: boolean;
  readonly regressionEvenness: number;
  readonly rhythmicOddity: number;
  readonly transitionComplexity: number;
  readonly entropy: number;
  readonly longestSubsequence: number;
  readonly isEuclidean: boolean;
  readonly distributionSpectra: ReadonlyArray<readonly number[]>;
  readonly spectrumWidths: readonly number[];
  readonly spectrumVariation: number;
  readonly rotationalSymmetryAxes: readonly number[];
  readonly reflectiveSymmetryAxes: readonly number[];
  readonly modulo: number;
  readonly aksakClassification: AksakClassification;
  readonly palindrome: boolean;
  readonly chiral: boolean;
  readonly balanced: boolean;
  readonly generated: GeneratedResult;
}
