import {
  IntervalVector,
  intervalsToPositions,
  PositionVector,
  positionsToIntervals
} from "../core";
import {
  SelectionOptions,
  select
} from "../selection";

export interface ScaleOptions {
  readonly root?: number;
  readonly mode?: number;
  readonly degree?: number;
  readonly invert?: boolean;
  readonly inversionAxis?: number;
  readonly mirror?: boolean;
  readonly mirrorAxis?: number;
}

export interface ChordOptions {
  readonly shift?: number;
  readonly rotation?: number;
  readonly preVoices?: number;
  readonly position?: number;
  readonly invert?: boolean;
  readonly axis?: number;
  readonly negativeOrMirror?: boolean;
  readonly negativeOrMirrorPos?: number;
}

function resolveScaleOptions(options?: ScaleOptions): Required<ScaleOptions> {
  return {
    degree: options && typeof options.degree === "number" ? options.degree : 0,
    inversionAxis: options && typeof options.inversionAxis === "number" ? options.inversionAxis : 0,
    invert: options && typeof options.invert === "boolean" ? options.invert : false,
    mirror: options && typeof options.mirror === "boolean" ? options.mirror : false,
    mirrorAxis: options && typeof options.mirrorAxis === "number" ? options.mirrorAxis : 0,
    mode: options && typeof options.mode === "number" ? options.mode : 0,
    root: options && typeof options.root === "number" ? options.root : 0
  };
}

function resolveChordOptions(options?: ChordOptions): Required<ChordOptions> {
  return {
    axis: options && typeof options.axis === "number" ? options.axis : 0,
    invert: options && typeof options.invert === "boolean" ? options.invert : false,
    negativeOrMirror: options && typeof options.negativeOrMirror === "boolean" ? options.negativeOrMirror : false,
    negativeOrMirrorPos: options && typeof options.negativeOrMirrorPos === "number" ? options.negativeOrMirrorPos : 0,
    position: options && typeof options.position === "number" ? options.position : 0,
    preVoices: options && typeof options.preVoices === "number" ? options.preVoices : 0,
    rotation: options && typeof options.rotation === "number" ? options.rotation : 0,
    shift: options && typeof options.shift === "number" ? options.shift : 0
  };
}

export class Scale {
  public readonly generator: IntervalVector;
  public readonly isFromPositions: boolean;
  public readonly options: Required<ScaleOptions>;
  private readonly intervals: IntervalVector;

  public constructor(generator: PositionVector | IntervalVector, options?: ScaleOptions) {
    this.options = resolveScaleOptions(options);
    this.isFromPositions = generator instanceof PositionVector;
    this.generator = generator instanceof PositionVector ? positionsToIntervals(generator) : generator;
    this.intervals = this.applyTransformations();
  }

  private applyTransformations(): IntervalVector {
    var transformed = this.generator.withOffset(this.options.root).rotate(this.options.mode);
    if (this.options.invert) {
      transformed = transformed.inversion(this.options.inversionAxis);
    }
    if (this.options.mirror) {
      transformed = transformed.singleMirror(this.options.mirrorAxis, true);
    }
    return transformed;
  }

  public toPositions(): PositionVector {
    return intervalsToPositions(this.intervals);
  }

  public toIntervals(): IntervalVector {
    return this.intervals;
  }

  public toJSON(): {
    readonly positions: ReturnType<PositionVector["toJSON"]>;
    readonly intervals: ReturnType<IntervalVector["toJSON"]>;
    readonly params: Required<ScaleOptions>;
  } {
    return {
      intervals: this.toIntervals().toJSON(),
      params: this.options,
      positions: this.toPositions().toJSON()
    };
  }
}

export class Chord {
  public readonly options: Required<ChordOptions>;
  private readonly positionsResult: PositionVector | null;
  private readonly intervalsResult: IntervalVector | null;

  public constructor(
    scale: PositionVector | IntervalVector,
    criterion: PositionVector | IntervalVector,
    options?: ChordOptions
  ) {
    this.options = resolveChordOptions(options);

    if (scale instanceof PositionVector && criterion instanceof PositionVector) {
      var offsetDegrees = criterion.addScalar(this.options.shift);
      var selectedPositions = select(scale, offsetDegrees, {
        rotation: this.options.rotation,
        voices: this.options.preVoices
      }) as PositionVector;
      if (this.options.invert) {
        selectedPositions = selectedPositions.inversion(this.options.axis, true);
      }
      if (this.options.negativeOrMirror) {
        selectedPositions = selectedPositions.negative(this.options.negativeOrMirrorPos);
      }
      this.positionsResult = selectedPositions.rotoTranslate(this.options.position);
      this.intervalsResult = null;
      return;
    }

    if (scale instanceof PositionVector && criterion instanceof IntervalVector) {
      var offsetIntervals = criterion.withOffset(this.options.shift);
      var selectedByIntervals = select(scale, offsetIntervals, {
        rotation: this.options.rotation,
        voices: this.options.preVoices
      }) as PositionVector;
      if (this.options.invert) {
        selectedByIntervals = selectedByIntervals.inversion(this.options.axis, true);
      }
      if (this.options.negativeOrMirror) {
        selectedByIntervals = selectedByIntervals.negative(this.options.negativeOrMirrorPos);
      }
      this.positionsResult = selectedByIntervals.rotoTranslate(this.options.position);
      this.intervalsResult = null;
      return;
    }

    if (scale instanceof IntervalVector && criterion instanceof PositionVector) {
      var shiftedCriterion = criterion.addScalar(this.options.shift);
      var intervalSelection = select(scale, shiftedCriterion, {
        rotation: this.options.rotation,
        voices: this.options.preVoices
      }) as IntervalVector;
      intervalSelection = intervalSelection.rotoTranslate(this.options.position);
      if (this.options.invert) {
        intervalSelection = intervalSelection.inversion(this.options.axis);
      }
      if (this.options.negativeOrMirror) {
        intervalSelection = intervalSelection.singleMirror(this.options.negativeOrMirrorPos, true);
      }
      this.positionsResult = null;
      this.intervalsResult = intervalSelection;
      return;
    }

    var intervalCriterion = criterion as IntervalVector;
    var shiftedIntervalCriterion = intervalCriterion.withOffset(intervalCriterion.offset + this.options.shift);
    var selectedIntervals = select(scale as IntervalVector, shiftedIntervalCriterion, {
      rotation: this.options.rotation,
      voices: this.options.preVoices
    }) as IntervalVector;
    selectedIntervals = selectedIntervals.rotoTranslate(this.options.position);
    if (this.options.invert) {
      selectedIntervals = selectedIntervals.inversion(this.options.axis);
    }
    if (this.options.negativeOrMirror) {
      selectedIntervals = selectedIntervals.singleMirror(this.options.negativeOrMirrorPos, true);
    }
    this.positionsResult = null;
    this.intervalsResult = selectedIntervals;
  }

  public toPositions(): PositionVector {
    if (this.positionsResult) {
      return this.positionsResult;
    }
    return intervalsToPositions(this.intervalsResult as IntervalVector);
  }

  public toIntervals(): IntervalVector {
    if (this.intervalsResult) {
      return this.intervalsResult;
    }
    return positionsToIntervals(this.positionsResult as PositionVector);
  }

  public toJSON(): {
    readonly positions: ReturnType<PositionVector["toJSON"]>;
    readonly intervals: ReturnType<IntervalVector["toJSON"]>;
  } {
    return {
      intervals: this.toIntervals().toJSON(),
      positions: this.toPositions().toJSON()
    };
  }
}

export function chord(scale: PositionVector, criterion: PositionVector, options?: ChordOptions): Chord;
export function chord(scale: PositionVector, criterion: IntervalVector, options?: ChordOptions): Chord;
export function chord(scale: IntervalVector, criterion: PositionVector, options?: ChordOptions): Chord;
export function chord(scale: IntervalVector, criterion: IntervalVector, options?: ChordOptions): Chord;
export function chord(
  scale: PositionVector | IntervalVector,
  criterion: PositionVector | IntervalVector,
  options?: ChordOptions
): Chord {
  return new Chord(scale, criterion, options);
}

export function chordSelectionOptionsFromScale(options?: ChordOptions): SelectionOptions {
  return {
    rotation: options && typeof options.rotation === "number" ? options.rotation : 0,
    voices: options && typeof options.preVoices === "number" ? options.preVoices : 0
  };
}
