import {
  arraysEqual,
  cloneNumberArray,
  euclideanDivision,
  lcm,
  truncateTowardZero,
  uniquePositive,
  validateBinaryData
} from "./internal";
import type { BinaryVectorOptions } from "./types";

export class BinaryVector {
  private readonly _data: number[];
  private readonly _offset: number;
  private readonly _mod: number;

  public constructor(data: readonly number[] = [1, 0, 0, 0], options: BinaryVectorOptions = {}) {
    validateBinaryData(data);
    this._data = cloneNumberArray(data);
    this._offset = typeof options.offset === "number" ? options.offset : 0;
    this._mod = typeof options.mod === "number" ? options.mod : 4;
  }

  public get data(): readonly number[] {
    return this.toArray();
  }

  public get offset(): number {
    return this._offset;
  }

  public get mod(): number {
    return this._mod;
  }

  public get size(): number {
    return this._data.length;
  }

  public isEmpty(): boolean {
    return this._data.length === 0;
  }

  public toArray(): number[] {
    return cloneNumberArray(this._data);
  }

  public toJSON(): {
    readonly data: readonly number[];
    readonly offset: number;
    readonly mod: number;
  } {
    return {
      data: this.toArray(),
      mod: this._mod,
      offset: this._offset
    };
  }

  public equals(other: BinaryVector): boolean {
    return arraysEqual(this._data, other._data) &&
      this._offset === other._offset &&
      this._mod === other._mod;
  }

  public at(index: number): number {
    if (this._data.length === 0) {
      return 0;
    }
    return this._data[euclideanDivision(index, this._data.length).remainder];
  }

  public withOffset(offset: number): BinaryVector {
    return new BinaryVector(this._data, { mod: this._mod, offset: offset });
  }

  public withMod(mod: number): BinaryVector {
    return new BinaryVector(this._data, { mod: mod, offset: this._offset });
  }

  public multiplyScalar(scalar: number): BinaryVector {
    if (scalar <= 0) {
      throw new Error("scalar must be positive");
    }

    var length = this._data.length;
    var result: number[] = new Array(scalar * length);
    var index = 0;
    for (index = 0; index < result.length; index += 1) {
      result[index] = 0;
    }

    for (index = 0; index < scalar * length; index += 1) {
      if (((index + 1) % scalar) === 1) {
        result[index] = this._data[truncateTowardZero(index / scalar)];
      }
    }

    return new BinaryVector(result, { mod: this._mod, offset: this._offset });
  }

  public divideScalar(scalar: number): BinaryVector {
    var length = this._data.length;
    if (scalar <= 0) {
      throw new Error("k must be positive");
    }
    if (scalar > length) {
      throw new Error("k must be less than or equal to vector size");
    }
    if ((length % scalar) !== 0) {
      throw new Error("Vector size must be divisible by k");
    }

    var result: number[] = [];
    for (var index = 0; index < length / scalar; index += 1) {
      result.push(this._data[index * scalar]);
    }

    return new BinaryVector(result, { mod: this._mod, offset: this._offset });
  }

  public rotate(rotationAmount: number): BinaryVector {
    if (this._data.length === 0) {
      return this;
    }

    var normalizedRotation = euclideanDivision(rotationAmount, this._data.length).remainder;
    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      result.push(this._data[(index + normalizedRotation) % this._data.length]);
    }

    return new BinaryVector(result, { mod: this._mod, offset: this._offset });
  }

  public complement(): BinaryVector {
    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      result.push(1 - this._data[index]);
    }
    return new BinaryVector(result, { mod: this._mod, offset: this._offset });
  }

  public inversion(axisIndex: number): BinaryVector {
    if (this._data.length === 0) {
      return this;
    }

    var normalizedAxis = euclideanDivision(axisIndex, this._data.length).remainder;
    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      var distance = index - normalizedAxis;
      var mirrorIndex = normalizedAxis - distance;
      result.push(this._data[euclideanDivision(mirrorIndex, this._data.length).remainder]);
    }

    return new BinaryVector(result, { mod: this._mod, offset: this._offset });
  }

  public transpose(transpositionAmount: number): BinaryVector {
    return new BinaryVector(this._data, { mod: this._mod, offset: this._offset + transpositionAmount });
  }

  public concatenate(other: BinaryVector): BinaryVector {
    return new BinaryVector(this._data.concat(other._data), { mod: this._mod, offset: this._offset });
  }

  public repeat(times: number): BinaryVector {
    if (times <= 0) {
      return new BinaryVector([], { mod: this._mod, offset: this._offset });
    }

    var result: number[] = [];
    for (var repeatIndex = 0; repeatIndex < times; repeatIndex += 1) {
      result = result.concat(this._data);
    }
    return new BinaryVector(result, { mod: this._mod, offset: this._offset });
  }

  public countPulses(): number {
    var total = 0;
    for (var index = 0; index < this._data.length; index += 1) {
      if (this._data[index] === 1) {
        total += 1;
      }
    }
    return total;
  }

  public density(): number {
    if (this._data.length === 0) {
      return 0;
    }
    return this.countPulses() / this._data.length;
  }

  public getPulseIndices(): number[] {
    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      if (this._data[index] === 1) {
        result.push(index);
      }
    }
    return result;
  }

  public getInterOnsetIntervals(): number[] {
    var indices = this.getPulseIndices();
    var result: number[] = [];
    if (indices.length < 2) {
      return result;
    }

    for (var index = 1; index < indices.length; index += 1) {
      result.push(indices[index] - indices[index - 1]);
    }
    result.push(this._data.length - indices[indices.length - 1] + indices[0]);
    return result;
  }

  private componentwiseBinary(
    other: readonly number[],
    useLooping: boolean,
    combine: (leftValue: number, rightValue: number) => number
  ): BinaryVector {
    if (other.length === 0) {
      return this;
    }
    if (this._data.length === 0) {
      return new BinaryVector(other, { mod: this._mod, offset: this._offset });
    }

    var result: number[] = [];
    var index = 0;

    if (useLooping) {
      var maxLength = this._data.length > other.length ? this._data.length : other.length;
      for (index = 0; index < maxLength; index += 1) {
        result.push(combine(this._data[index % this._data.length], other[index % other.length]));
      }
      return new BinaryVector(result, { mod: this._mod, offset: this._offset });
    }

    var minLength = this._data.length < other.length ? this._data.length : other.length;
    for (index = 0; index < minLength; index += 1) {
      result.push(combine(this._data[index], other[index]));
    }
    for (index = minLength; index < this._data.length; index += 1) {
      result.push(this._data[index]);
    }
    for (index = minLength; index < other.length; index += 1) {
      result.push(other[index]);
    }
    return new BinaryVector(result, { mod: this._mod, offset: this._offset });
  }

  public or(other: BinaryVector): BinaryVector {
    var adapted = BinaryVector.adaptToLCM([this, other]);
    return adapted[0].componentwiseBinary(adapted[1]._data, false, function (leftValue, rightValue) {
      return leftValue | rightValue;
    });
  }

  public and(other: BinaryVector): BinaryVector {
    var adapted = BinaryVector.adaptToLCM([this, other]);
    return adapted[0].componentwiseBinary(adapted[1]._data, false, function (leftValue, rightValue) {
      return leftValue & rightValue;
    });
  }

  public xor(other: BinaryVector): BinaryVector {
    var adapted = BinaryVector.adaptToLCM([this, other]);
    return adapted[0].componentwiseBinary(adapted[1]._data, false, function (leftValue, rightValue) {
      return leftValue ^ rightValue;
    });
  }

  public nor(other: BinaryVector): BinaryVector {
    return this.or(other).complement();
  }

  public nand(other: BinaryVector): BinaryVector {
    return this.and(other).complement();
  }

  public xnor(other: BinaryVector): BinaryVector {
    return this.xor(other).complement();
  }

  public static adaptToLCM(vectors: readonly BinaryVector[]): BinaryVector[] {
    if (vectors.length === 0) {
      return [];
    }

    var modulos = uniquePositive(vectors.map(function (vector) {
      return vector.mod;
    }));
    if (modulos.length <= 1) {
      return vectors.slice() as BinaryVector[];
    }

    var commonMod = lcm(modulos);
    var adapted: BinaryVector[] = [];
    for (var index = 0; index < vectors.length; index += 1) {
      var scaleFactor = commonMod / vectors[index].mod;
      var spaced: number[] = [];
      for (var dataIndex = 0; dataIndex < vectors[index]._data.length; dataIndex += 1) {
        spaced.push(vectors[index]._data[dataIndex]);
        for (var filler = 1; filler < scaleFactor; filler += 1) {
          spaced.push(0);
        }
      }
      adapted.push(new BinaryVector(spaced, { mod: commonMod, offset: vectors[index].offset }));
    }
    return adapted;
  }

  public static euclidean(pulses: number, steps: number): BinaryVector {
    if (pulses <= 0 || steps <= 0 || pulses > steps) {
      throw new Error("Invalid Euclidean parameters");
    }

    var totalSteps = steps;
    var groups: number[][] = [];
    var index = 0;

    for (index = 0; index < pulses; index += 1) {
      groups.push([1]);
    }
    for (index = 0; index < steps - pulses; index += 1) {
      groups.push([0]);
    }

    while (groups.length > 1) {
      var minSize = pulses < (steps - pulses) ? pulses : (steps - pulses);
      for (index = 0; index < minSize; index += 1) {
        groups[index] = groups[index].concat(groups[groups.length - 1]);
        groups.pop();
      }
      pulses = minSize;
      steps = groups.length;
    }

    var result: number[] = [];
    for (index = 0; index < groups.length; index += 1) {
      result = result.concat(groups[index]);
    }

    return new BinaryVector(result, { mod: totalSteps, offset: 0 });
  }
}
