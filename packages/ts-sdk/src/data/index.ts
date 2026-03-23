import {
  SCALE_RECORDS,
  type ScaleRecord
} from "../generated/scales.generated";

export interface ScaleMatch extends ScaleRecord {}

export class ScaleDatabase {
  private readonly scales: readonly ScaleRecord[];

  public constructor(scales: readonly ScaleRecord[] = SCALE_RECORDS) {
    this.scales = scales;
  }

  public findScale(inputIntervals: readonly number[]): readonly ScaleMatch[] {
    if (inputIntervals.length === 0) {
      return [];
    }

    var root = inputIntervals[0];
    var normalizedInput: number[] = [];
    for (var index = 0; index < inputIntervals.length; index += 1) {
      normalizedInput.push(inputIntervals[index] - root);
    }

    var processedInput = normalizedInput.slice().sort(function (left, right) {
      return left - right;
    });
    processedInput = processedInput.filter(function (value, index, values) {
      return index === 0 || value !== values[index - 1];
    });

    var results: ScaleMatch[] = [];
    for (index = 0; index < this.scales.length; index += 1) {
      var sortedScale = this.scales[index].intervals.slice().sort(function (left, right) {
        return left - right;
      });
      if (processedInput.length !== sortedScale.length) {
        continue;
      }

      var matches = true;
      for (var intervalIndex = 0; intervalIndex < processedInput.length; intervalIndex += 1) {
        if (processedInput[intervalIndex] !== sortedScale[intervalIndex]) {
          matches = false;
          break;
        }
      }

      if (matches) {
        results.push({
          intervals: this.scales[index].intervals.slice(),
          scaleName: this.scales[index].scaleName,
          sheetName: this.scales[index].sheetName
        });
      }
    }

    return results;
  }

  public getAllScales(): readonly ScaleMatch[] {
    return this.scales.slice();
  }
}

export {
  SCALE_RECORDS
};
