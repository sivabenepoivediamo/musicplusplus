import {
  absoluteInteger,
  arraysEqual,
  cloneNumberArray,
  componentwiseOperation,
  euclideanDivision,
  extractData,
  lcm,
  maxNumber,
  minNumber,
  truncateTowardZero
} from "./internal";
import type { NumericVectorInput, PositionVectorOptions } from "./types";

function asPositionVectorOptions(source: PositionVector): PositionVectorOptions {
  return {
    mod: source.mod,
    rangeUpdate: source.rangeUpdate,
    user: source.user,
    userRange: source.userRange
  };
}

export class PositionVector {
  private readonly _data: number[];
  private readonly _mod: number;
  private readonly _userRange: number;
  private readonly _range: number;
  private readonly _rangeUpdate: boolean;
  private readonly _user: boolean;

  public constructor(data: readonly number[] = [0], options: PositionVectorOptions = {}) {
    var normalizedMod = typeof options.mod === "number" ? options.mod : 12;
    var normalizedUserRange = typeof options.userRange === "number" && options.userRange > 0 ? options.userRange : normalizedMod;
    var normalizedRangeUpdate = typeof options.rangeUpdate === "boolean" ? options.rangeUpdate : true;
    var normalizedUser = typeof options.user === "boolean" ? options.user : false;

    this._data = cloneNumberArray(data);
    this._mod = normalizedMod;
    this._userRange = normalizedUserRange;
    this._rangeUpdate = normalizedRangeUpdate;
    this._user = normalizedUser;
    this._range = normalizedRangeUpdate ? this.rangeCalculation() : (normalizedUser ? normalizedUserRange : normalizedMod);
  }

  private rangeCalculation(): number {
    var modulo = this._user ? this._userRange : this._mod;
    if (this._data.length === 0) {
      return modulo;
    }

    var span = maxNumber(this._data) - minNumber(this._data);
    var division = euclideanDivision(span, modulo);
    return modulo * (division.quotient + 1);
  }

  public get data(): readonly number[] {
    return this.toArray();
  }

  public get mod(): number {
    return this._mod;
  }

  public get userRange(): number {
    return this._userRange;
  }

  public get range(): number {
    return this._range;
  }

  public get rangeUpdate(): boolean {
    return this._rangeUpdate;
  }

  public get user(): boolean {
    return this._user;
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
    readonly mod: number;
    readonly userRange: number;
    readonly range: number;
    readonly rangeUpdate: boolean;
    readonly user: boolean;
  } {
    return {
      data: this.toArray(),
      mod: this._mod,
      range: this._range,
      rangeUpdate: this._rangeUpdate,
      user: this._user,
      userRange: this._userRange
    };
  }

  public equals(other: PositionVector): boolean {
    return arraysEqual(this._data, other._data) &&
      this._mod === other._mod &&
      this._userRange === other._userRange &&
      this._user === other._user;
  }

  public at(index: number): number {
    return this.element(index);
  }

  public element(index: number): number {
    if (this._data.length === 0) {
      return 0;
    }

    var division = euclideanDivision(index, this._data.length);
    var cycles = (index - division.remainder) / this._data.length;
    return this._data[division.remainder] + absoluteInteger(this._range) * cycles;
  }

  public withMod(mod: number): PositionVector {
    return new PositionVector(this._data, {
      mod: mod,
      rangeUpdate: this._rangeUpdate,
      user: this._user,
      userRange: this._userRange
    });
  }

  public withUserRange(userRange: number): PositionVector {
    return new PositionVector(this._data, {
      mod: this._mod,
      rangeUpdate: this._rangeUpdate,
      user: this._user,
      userRange: userRange
    });
  }

  public withRange(range: number): PositionVector {
    return new PositionVector(this._data, {
      mod: this._mod,
      rangeUpdate: false,
      user: this._user,
      userRange: range
    });
  }

  public withRangeUpdate(rangeUpdate: boolean): PositionVector {
    return new PositionVector(this._data, {
      mod: this._mod,
      rangeUpdate: rangeUpdate,
      user: this._user,
      userRange: this._userRange
    });
  }

  public withUser(user: boolean): PositionVector {
    return new PositionVector(this._data, {
      mod: this._mod,
      rangeUpdate: this._rangeUpdate,
      user: user,
      userRange: this._userRange
    });
  }

  public addScalar(scalar: number): PositionVector {
    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      result.push(this._data[index] + scalar);
    }
    return new PositionVector(result, asPositionVectorOptions(this));
  }

  public subtractScalar(scalar: number): PositionVector {
    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      result.push(this._data[index] - scalar);
    }
    return new PositionVector(result, asPositionVectorOptions(this));
  }

  public multiplyScalar(scalar: number): PositionVector {
    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      result.push(this._data[index] * scalar);
    }
    return new PositionVector(result, asPositionVectorOptions(this));
  }

  public divideScalar(divisor: number): PositionVector {
    if (divisor === 0) {
      throw new Error("Division by zero");
    }

    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      result.push(euclideanDivision(this._data[index], divisor).quotient);
    }
    return new PositionVector(result, asPositionVectorOptions(this));
  }

  public moduloScalar(divisor: number): PositionVector {
    if (divisor === 0) {
      throw new Error("Division by zero");
    }

    var result: number[] = [];
    for (var index = 0; index < this._data.length; index += 1) {
      result.push(euclideanDivision(this._data[index], divisor).remainder);
    }
    return new PositionVector(result, asPositionVectorOptions(this));
  }

  public addVector(other: NumericVectorInput, useLooping: boolean = false): PositionVector {
    return new PositionVector(
      componentwiseOperation(this._data, extractData(other), useLooping, function (leftValue, rightValue) {
        return leftValue + rightValue;
      }),
      asPositionVectorOptions(this)
    );
  }

  public subtractVector(other: NumericVectorInput, useLooping: boolean = false): PositionVector {
    return new PositionVector(
      componentwiseOperation(this._data, extractData(other), useLooping, function (leftValue, rightValue) {
        return leftValue - rightValue;
      }),
      asPositionVectorOptions(this)
    );
  }

  public multiplyVector(other: NumericVectorInput, useLooping: boolean = true): PositionVector {
    return new PositionVector(
      componentwiseOperation(this._data, extractData(other), useLooping, function (leftValue, rightValue) {
        return leftValue * rightValue;
      }),
      asPositionVectorOptions(this)
    );
  }

  public divideVector(other: NumericVectorInput, useLooping: boolean = true): PositionVector {
    return new PositionVector(
      componentwiseOperation(this._data, extractData(other), useLooping, function (leftValue, rightValue) {
        return euclideanDivision(leftValue, rightValue).quotient;
      }),
      asPositionVectorOptions(this)
    );
  }

  public moduloVector(other: NumericVectorInput, useLooping: boolean = true): PositionVector {
    return new PositionVector(
      componentwiseOperation(this._data, extractData(other), useLooping, function (leftValue, rightValue) {
        return euclideanDivision(leftValue, rightValue).remainder;
      }),
      asPositionVectorOptions(this)
    );
  }

  public rotate(rotationAmount: number): PositionVector {
    if (this._data.length === 0) {
      return this;
    }

    var size = this._data.length;
    var absRotation = absoluteInteger(rotationAmount);
    var rotatedData: number[] = new Array(size);

    for (var index = 0; index < size; index += 1) {
      rotatedData[(index + absRotation) % size] = this.at(index);
    }

    return new PositionVector(rotatedData, asPositionVectorOptions(this));
  }

  public rotoTranslate(startOffset: number, length: number = 0): PositionVector {
    var outLength = length === 0 ? this._data.length : absoluteInteger(length);
    var result: number[] = [];
    for (var index = 0; index < outLength; index += 1) {
      result.push(this.at(startOffset + index));
    }
    return new PositionVector(result, asPositionVectorOptions(this));
  }

  public complement(): PositionVector {
    var effectiveRange = this._range;
    if (this._data.length === 0) {
      var universe: number[] = [];
      for (var index = 0; index < effectiveRange; index += 1) {
        universe.push(index);
      }
      return new PositionVector(universe, asPositionVectorOptions(this));
    }

    var minimum = minNumber(this._data);
    var normalized = this.subtractScalar(minimum);
    var complementData: number[] = [];
    for (var value = 0; value < effectiveRange; value += 1) {
      if (normalized._data.indexOf(value) === -1) {
        complementData.push(value);
      }
    }

    return new PositionVector(complementData, asPositionVectorOptions(this)).addScalar(minimum);
  }

  public inversion(axisIndex: number, sortOutput: boolean = true): PositionVector {
    if (this._data.length === 0) {
      return this;
    }

    var normalizedAxisIndex = euclideanDivision(axisIndex, this._data.length).remainder;
    var axisValue = this._data[normalizedAxisIndex];
    var invertedData: number[] = [];

    for (var index = 0; index < this._data.length; index += 1) {
      invertedData.push((2 * axisValue) - this._data[index]);
    }

    if (sortOutput) {
      invertedData.sort(function (left, right) {
        return left - right;
      });
    }

    return new PositionVector(invertedData, asPositionVectorOptions(this));
  }

  public negative(axis: number = 10, standard: boolean = true, sortResult: boolean = true): PositionVector {
    var result = this.toArray();
    var adjustedPosition = axis;
    var index = 0;

    if (standard) {
      for (index = 0; index < result.length; index += 1) {
        result[index] = result[index] * 2;
      }
      adjustedPosition = (axis * 2) - 1;
    }

    for (index = 0; index < result.length; index += 1) {
      result[index] = result[index] - adjustedPosition;
      result[index] = result[index] * -1;
      result[index] = result[index] + adjustedPosition;
    }

    if (standard) {
      for (index = 0; index < result.length; index += 1) {
        result[index] = truncateTowardZero(result[index] / 2);
      }
    }

    if (sortResult) {
      result.sort(function (left, right) {
        return left - right;
      });
    }

    return new PositionVector(result, asPositionVectorOptions(this)).rotoTranslate(-1);
  }

  public concatenate(other: PositionVector): PositionVector {
    return new PositionVector(this._data.concat(other.toArray()), asPositionVectorOptions(this));
  }

  public repeat(times: number): PositionVector {
    if (times <= 0) {
      return new PositionVector([], asPositionVectorOptions(this));
    }

    var result: number[] = [];
    for (var repeatIndex = 0; repeatIndex < times; repeatIndex += 1) {
      result = result.concat(this._data);
    }

    return new PositionVector(result, asPositionVectorOptions(this));
  }

  public resize(start: number, end: number): PositionVector {
    if (this._data.length === 0) {
      return this;
    }

    var result: number[] = [];
    if (start <= end) {
      for (var index = start; index <= end; index += 1) {
        result.push(this.at(index));
      }
    } else {
      for (var index = start; index >= end; index -= 1) {
        result.push(this.at(index));
      }
    }

    return new PositionVector(result, asPositionVectorOptions(this));
  }

  public static adaptToLCM(vectors: readonly PositionVector[]): PositionVector[] {
    if (vectors.length === 0) {
      return [];
    }

    var modulos: number[] = [];
    var index = 0;
    for (index = 0; index < vectors.length; index += 1) {
      if (modulos.indexOf(vectors[index].mod) === -1) {
        modulos.push(vectors[index].mod);
      }
    }

    if (modulos.length === 1) {
      return vectors.slice() as PositionVector[];
    }

    var commonMod = lcm(modulos);
    var adapted: PositionVector[] = [];

    for (index = 0; index < vectors.length; index += 1) {
      var scaleFactor = commonMod / vectors[index].mod;
      var scaledData: number[] = [];
      var dataIndex = 0;
      for (dataIndex = 0; dataIndex < vectors[index]._data.length; dataIndex += 1) {
        scaledData.push(vectors[index]._data[dataIndex] * scaleFactor);
      }
      adapted.push(new PositionVector(scaledData, {
        mod: commonMod,
        rangeUpdate: false,
        user: vectors[index].user,
        userRange: vectors[index].userRange * scaleFactor
      }));
    }

    return adapted;
  }
}
