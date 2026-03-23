import type { BinaryVector } from "./binaryVector";
import type { IntervalVector } from "./intervalVector";
import type { PositionVector } from "./positionVector";

export interface DivisionResult {
  readonly quotient: number;
  readonly remainder: number;
}

export interface PositionVectorOptions {
  readonly mod?: number;
  readonly userRange?: number;
  readonly rangeUpdate?: boolean;
  readonly user?: boolean;
}

export interface IntervalVectorOptions {
  readonly offset?: number;
  readonly mod?: number;
}

export interface BinaryVectorOptions {
  readonly offset?: number;
  readonly mod?: number;
}

export type NumericVectorInput = readonly number[] | PositionVector | IntervalVector | BinaryVector;
