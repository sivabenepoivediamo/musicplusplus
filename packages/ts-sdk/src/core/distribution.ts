import { cppNumber } from "./internal";

export function normalizeDistribution(values: readonly number[]): number[] {
  var sum = 0;
  for (var index = 0; index < values.length; index += 1) {
    sum += values[index];
  }

  if (sum === 0) {
    throw new Error("Sum of vector elements is zero, cannot normalize");
  }

  var result: number[] = [];
  for (index = 0; index < values.length; index += 1) {
    result.push(cppNumber(values[index] / sum));
  }
  return result;
}

export function computeCDF(pdf: readonly number[]): number[] {
  var result: number[] = [];
  var total = 0;
  for (var index = 0; index < pdf.length; index += 1) {
    total += pdf[index];
    result.push(cppNumber(total));
  }
  return result;
}
