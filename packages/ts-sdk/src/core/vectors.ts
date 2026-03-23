import { BinaryVector } from "./binaryVector";
import {
  binaryToPositions,
  intervalsToPositions,
  positionsToBinary,
  positionsToIntervals
} from "./conversions";
import { IntervalVector } from "./intervalVector";
import { PositionVector } from "./positionVector";

export class Vectors {
  public readonly positions: PositionVector;
  public readonly intervals: IntervalVector;
  public readonly binary: BinaryVector;
  public readonly mod: number;

  public constructor(source?: number | PositionVector | IntervalVector | BinaryVector) {
    if (typeof source === "number" || typeof source === "undefined") {
      var modulo = typeof source === "number" ? source : 12;
      this.positions = new PositionVector([0], { mod: modulo });
      this.intervals = new IntervalVector([], { mod: modulo, offset: 0 });
      this.binary = new BinaryVector([1], { mod: modulo, offset: 0 });
      this.mod = modulo;
      return;
    }

    if (source instanceof PositionVector) {
      this.positions = source;
      this.intervals = positionsToIntervals(source);
      this.binary = positionsToBinary(source);
      this.mod = source.mod;
      return;
    }

    if (source instanceof IntervalVector) {
      this.intervals = source;
      this.positions = intervalsToPositions(source);
      this.binary = positionsToBinary(this.positions);
      this.mod = source.mod;
      return;
    }

    this.binary = source;
    this.positions = binaryToPositions(source);
    this.intervals = positionsToIntervals(this.positions);
    this.mod = source.mod;
  }

  public transpose(amount: number): Vectors {
    return new Vectors(this.positions.addScalar(amount));
  }

  public multiplyPositions(scalar: number): Vectors {
    return new Vectors(this.positions.multiplyScalar(scalar));
  }

  public negative(axis: number = 10): Vectors {
    return new Vectors(this.positions.negative(axis));
  }

  public rotatePositions(amount: number): Vectors {
    return new Vectors(this.positions.rotate(amount));
  }

  public rototranslatePositions(amount: number, length: number = 0): Vectors {
    return new Vectors(this.positions.rotoTranslate(amount, length));
  }

  public inversion(amount: number, length: number = 0): Vectors {
    return this.rototranslatePositions(amount, length);
  }

  public invertPositions(axisIndex: number, sortOutput: boolean = true): Vectors {
    return new Vectors(this.positions.inversion(axisIndex, sortOutput));
  }

  public complementPositions(): Vectors {
    return new Vectors(this.positions.complement());
  }

  public addToIntervals(amount: number): Vectors {
    return new Vectors(this.intervals.addScalar(amount));
  }

  public multiplyIntervals(scalar: number): Vectors {
    return new Vectors(this.intervals.multiplyScalar(scalar));
  }

  public rotateIntervals(amount: number): Vectors {
    return new Vectors(this.intervals.rotate(amount));
  }

  public reverseIntervals(): Vectors {
    return new Vectors(this.intervals.reverse());
  }

  public invertIntervals(axisIndex: number): Vectors {
    return new Vectors(this.intervals.inversion(axisIndex));
  }

  public mode(amount: number): Vectors {
    return this.rotateIntervals(amount);
  }

  public rotateBinary(amount: number): Vectors {
    return new Vectors(this.binary.rotate(amount));
  }

  public complementBinary(): Vectors {
    return new Vectors(this.binary.complement());
  }

  public multiplyBinary(scalar: number): Vectors {
    return new Vectors(this.binary.multiplyScalar(scalar));
  }

  public divideBinary(divisor: number): Vectors {
    return new Vectors(this.binary.divideScalar(divisor));
  }

  public or(other: Vectors): Vectors {
    return new Vectors(this.binary.or(other.binary));
  }

  public and(other: Vectors): Vectors {
    return new Vectors(this.binary.and(other.binary));
  }

  public xor(other: Vectors): Vectors {
    return new Vectors(this.binary.xor(other.binary));
  }

  public equals(other: Vectors): boolean {
    return this.positions.equals(other.positions) &&
      this.intervals.equals(other.intervals) &&
      this.binary.equals(other.binary);
  }

  public toJSON(): {
    readonly positions: ReturnType<PositionVector["toJSON"]>;
    readonly intervals: ReturnType<IntervalVector["toJSON"]>;
    readonly binary: ReturnType<BinaryVector["toJSON"]>;
    readonly mod: number;
  } {
    return {
      binary: this.binary.toJSON(),
      intervals: this.intervals.toJSON(),
      mod: this.mod,
      positions: this.positions.toJSON()
    };
  }

  public static fromPositions(data: readonly number[], modulo: number = 12): Vectors {
    return new Vectors(new PositionVector(data, { mod: modulo }));
  }

  public static fromIntervals(data: readonly number[], modulo: number = 12): Vectors {
    return new Vectors(new IntervalVector(data, { mod: modulo, offset: 0 }));
  }

  public static fromBinary(data: readonly number[], offset: number = 0, modulo: number = 12): Vectors {
    return new Vectors(new BinaryVector(data, { mod: modulo, offset: offset }));
  }

  public static euclidean(pulses: number, steps: number): Vectors {
    return new Vectors(BinaryVector.euclidean(pulses, steps));
  }
}
