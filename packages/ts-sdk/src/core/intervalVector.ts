import {
  absoluteInteger,
  arraysEqual,
  cloneNumberArray,
  componentwiseOperation,
  euclideanDivision,
  extractData,
  lcm,
  truncateTowardZero,
  uniquePositive
} from "./internal";
import type { IntervalVectorOptions, NumericVectorInput } from "./types";

export class IntervalVector {
  private readonly _data: number[];
  private readonly _offset: number;
  private readonly _mod: number;

  public constructor(data: readonly number[] = [0], options: IntervalVectorOptions = {}) {
    this._data = cloneNumberArray(data);
    this._offset = typeof options.offset === "number" ? options.offset : 0;
    this._mod = typeof options.mod === "number" ? options.mod : 12;
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

  public equals(other: IntervalVector): boolean {
    return arraysEqual(this._data, other._data) &&
      this._offset === other._offset &&
      this._mod === other._mod;
  }

  public at(index: number): number {
    return this.element(index);
  }

  public element(index: number): number {
    if (this._data.length === 0) {
      return 0;
    }
    return this._data[euclideanDivision(index, this._data.length).remainder];
  }

  public withOffset(offset: number): IntervalVector {
    return new IntervalVector(this._data, { mod: this._mod, offset: offset });
  }

  public withMod(mod: number): IntervalVector {
    return new IntervalVector(this._data, { mod: mod, offset: this._offset });
  }

  public addScalar(scalar: number): IntervalVector {
    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      result.push(this._data[index] + scalar);
    }
    return new IntervalVector(result, { mod: this._mod, offset: this._offset });
  }

  public subtractScalar(scalar: number): IntervalVector {
    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      result.push(this._data[index] - scalar);
    }
    return new IntervalVector(result, { mod: this._mod, offset: this._offset });
  }

  public multiplyScalar(scalar: number): IntervalVector {
    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      result.push(this._data[index] * scalar);
    }
    return new IntervalVector(result, { mod: this._mod, offset: this._offset });
  }

  public divideScalar(divisor: number): IntervalVector {
    if (divisor === 0) {
      throw new Error("Division by zero");
    }

    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      result.push(euclideanDivision(this._data[index], divisor).quotient);
    }
    return new IntervalVector(result, { mod: this._mod, offset: this._offset });
  }

  public moduloScalar(divisor: number): IntervalVector {
    if (divisor === 0) {
      throw new Error("Division by zero");
    }

    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      result.push(euclideanDivision(this._data[index], divisor).remainder);
    }
    return new IntervalVector(result, { mod: this._mod, offset: this._offset });
  }

  public addVector(other: NumericVectorInput, useLooping: boolean = false): IntervalVector {
    return new IntervalVector(
      componentwiseOperation(this._data, extractData(other), useLooping, function (leftValue, rightValue) {
        return leftValue + rightValue;
      }),
      { mod: this._mod, offset: this._offset }
    );
  }

  public subtractVector(other: NumericVectorInput, useLooping: boolean = false): IntervalVector {
    return new IntervalVector(
      componentwiseOperation(this._data, extractData(other), useLooping, function (leftValue, rightValue) {
        return leftValue - rightValue;
      }),
      { mod: this._mod, offset: this._offset }
    );
  }

  public multiplyVector(other: NumericVectorInput, useLooping: boolean = true): IntervalVector {
    return new IntervalVector(
      componentwiseOperation(this._data, extractData(other), useLooping, function (leftValue, rightValue) {
        return leftValue * rightValue;
      }),
      { mod: this._mod, offset: this._offset }
    );
  }

  public divideVector(other: NumericVectorInput, useLooping: boolean = true): IntervalVector {
    return new IntervalVector(
      componentwiseOperation(this._data, extractData(other), useLooping, function (leftValue, rightValue) {
        return euclideanDivision(leftValue, rightValue).quotient;
      }),
      { mod: this._mod, offset: this._offset }
    );
  }

  public moduloVector(other: NumericVectorInput, useLooping: boolean = true): IntervalVector {
    return new IntervalVector(
      componentwiseOperation(this._data, extractData(other), useLooping, function (leftValue, rightValue) {
        return euclideanDivision(leftValue, rightValue).remainder;
      }),
      { mod: this._mod, offset: this._offset }
    );
  }

  public rotate(rotationAmount: number, length: number = 0): IntervalVector {
    var outLength = absoluteInteger(length);
    if (outLength === 0) {
      outLength = this._data.length;
    }

    var result: number[] = [];
    for (var index = 0; index < outLength; index += 1) {
      result.push(this.element(rotationAmount + index));
    }

    return new IntervalVector(result, { mod: this._mod, offset: this._offset });
  }

  public rotoTranslate(rotationAmount: number, length: number = 0): IntervalVector {
    var outLength = absoluteInteger(length);
    var dataSize = this._data.length;
    if (outLength === 0) {
      outLength = dataSize;
    }

    var result: number[] = [];
    var index = 0;
    for (index = 0; index < outLength; index += 1) {
      result.push(this.element(rotationAmount + index));
    }

    var sum = 0;
    if (dataSize === 0) {
      return new IntervalVector(result, { mod: this._mod, offset: this._offset });
    }

    if (absoluteInteger(rotationAmount) < dataSize) {
      if (rotationAmount >= 0) {
        for (index = 0; index < rotationAmount; index += 1) {
          sum += this.element(index);
        }
      } else {
        for (index = 0; index < -rotationAmount; index += 1) {
          sum -= this.element(dataSize - 1 - index);
        }
      }
    } else {
      var division = euclideanDivision(rotationAmount, dataSize);
      if (rotationAmount >= 0) {
        for (index = 0; index < dataSize; index += 1) {
          var multiplier = index < division.remainder ? division.quotient + 1 : division.quotient;
          sum += this.element(index) * multiplier;
        }
      } else {
        var threshold = dataSize + division.remainder;
        for (index = 0; index < dataSize; index += 1) {
          var negativeMultiplier = index >= threshold ? division.quotient - 1 : division.quotient;
          sum += this.element(index) * negativeMultiplier;
        }
      }
    }

    return new IntervalVector(result, { mod: this._mod, offset: this._offset + sum });
  }

  public reverse(): IntervalVector {
    var result: number[] = [];
    for (var index = this._data.length - 1; index >= 0; index -= 1) {
      result.push(this._data[index]);
    }
    return new IntervalVector(result, { mod: this._mod, offset: this._offset });
  }

  public retrograde(): IntervalVector {
    return this.reverse();
  }

  public inversion(axisIndex: number = 0): IntervalVector {
    if (this._data.length === 0) {
      return this;
    }

    var size = this._data.length;
    var normalizedAxis = euclideanDivision(axisIndex, size + 1).remainder;
    var result = this.toArray();
    var index = 0;

    for (index = 0; index < truncateTowardZero(normalizedAxis / 2); index += 1) {
      var leftValue = result[index];
      result[index] = result[normalizedAxis - 1 - index];
      result[normalizedAxis - 1 - index] = leftValue;
    }

    var start = normalizedAxis;
    var end = size;
    var lengthAfter = end - start;
    for (index = 0; index < truncateTowardZero(lengthAfter / 2); index += 1) {
      var fromIndex = start + index;
      var toIndex = end - 1 - index;
      var current = result[fromIndex];
      result[fromIndex] = result[toIndex];
      result[toIndex] = current;
    }

    return new IntervalVector(result, { mod: this._mod, offset: this._offset });
  }

  public negate(): IntervalVector {
    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      result.push(-this._data[index]);
    }
    return new IntervalVector(result, { mod: this._mod, offset: -this._offset });
  }

  public normalize(modulo?: number): IntervalVector {
    var targetModulo = typeof modulo === "number" && modulo !== 0 ? modulo : this._mod;
    if (targetModulo === 0) {
      return this;
    }

    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      result.push(euclideanDivision(this._data[index], targetModulo).remainder);
    }
    return new IntervalVector(result, { mod: this._mod, offset: this._offset });
  }

  public concatenate(other: IntervalVector): IntervalVector {
    return new IntervalVector(this._data.concat(other._data), { mod: this._mod, offset: this._offset });
  }

  public repeat(times: number): IntervalVector {
    if (times <= 0) {
      return new IntervalVector([], { mod: this._mod, offset: this._offset });
    }

    var result: number[] = [];
    for (var repeatIndex = 0; repeatIndex < times; repeatIndex += 1) {
      result = result.concat(this._data);
    }
    return new IntervalVector(result, { mod: this._mod, offset: this._offset });
  }

  public singleMirror(position: number, left: boolean): IntervalVector {
    var result = this.toArray();
    var length = result.length;
    var index = 0;

    if (position < 0 || position > length) {
      return this;
    }

    if (left) {
      for (index = 0; index < truncateTowardZero(position / 2); index += 1) {
        var leftValue = result[index];
        result[index] = result[position - 1 - index];
        result[position - 1 - index] = leftValue;
      }
    } else {
      var end = position + truncateTowardZero((length - position) / 2);
      for (index = position; index < end; index += 1) {
        var mirrorValue = result[index];
        result[index] = result[length - 1 - (index - position)];
        result[length - 1 - (index - position)] = mirrorValue;
      }
    }

    return new IntervalVector(result, { mod: this._mod, offset: this._offset });
  }

  public doubleMirror(position: number): IntervalVector {
    var result = this.toArray();
    var length = result.length;
    var index = 0;

    if (position < 0 || position > length) {
      return this;
    }

    for (index = 0; index < truncateTowardZero(position / 2); index += 1) {
      var leftValue = result[index];
      result[index] = result[position - 1 - index];
      result[position - 1 - index] = leftValue;
    }

    var end = position + truncateTowardZero((length - position) / 2);
    for (index = position; index < end; index += 1) {
      var rightValue = result[index];
      result[index] = result[length - 1 - (index - position)];
      result[length - 1 - (index - position)] = rightValue;
    }

    return new IntervalVector(result, { mod: this._mod, offset: this._offset });
  }

  public crossMirror(position: number, left: boolean): IntervalVector {
    var result = this.toArray();
    var length = result.length;
    var index = 0;

    if (left) {
      for (index = 0; index < position && index < length; index += 1) {
        result[length - 1 - index] = this._data[index];
      }
    } else {
      for (index = position; index < length; index += 1) {
        result[index - position] = this._data[length - 1 - (index - position)];
      }
    }

    return new IntervalVector(result, { mod: this._mod, offset: this._offset });
  }

  public static adaptToLCM(vectors: readonly IntervalVector[]): IntervalVector[] {
    if (vectors.length === 0) {
      return [];
    }

    var modulos = uniquePositive(vectors.map(function (vector) {
      return vector.mod;
    }));

    if (modulos.length <= 1) {
      return vectors.slice() as IntervalVector[];
    }

    var commonMod = lcm(modulos);
    var adapted: IntervalVector[] = [];

    for (var index = 0; index < vectors.length; index += 1) {
      if (vectors[index].mod === 0) {
        adapted.push(vectors[index]);
      } else {
        var scaleFactor = commonMod / vectors[index].mod;
        adapted.push(new IntervalVector(vectors[index].toArray().map(function (value) {
          return value * scaleFactor;
        }), {
          mod: commonMod,
          offset: vectors[index].offset * scaleFactor
        }));
      }
    }

    return adapted;
  }
}
