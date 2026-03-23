export interface NoteMapperOptions {
  readonly preferSharps?: boolean;
  readonly isDiatonicScale?: boolean;
  readonly moduloValue?: number;
}

export interface NoteResult {
  readonly noteNames: readonly string[];
  readonly centsInfo: readonly string[];
}

export interface ClassifiedNote {
  readonly note: string;
  readonly label: "natural" | "right" | "left" | "none";
}

export interface ChordAnalysis {
  root: number;
  chordName: string;
  hasThird: boolean;
  hasMajorThird: boolean;
  hasMinorThird: boolean;
  hasFifth: boolean;
  hasPerfectFifth: boolean;
  hasAugmentedFifth: boolean;
  hasDiminishedFifth: boolean;
  hasCompleteTriad: boolean;
  hasSeventh: boolean;
  hasMinorSeventh: boolean;
  hasMajorSeventh: boolean;
  hasDiminishedSeventh: boolean;
  hasNinth: boolean;
  hasFlatNinth: boolean;
  hasNaturalNinth: boolean;
  hasEleventh: boolean;
  hasNaturalEleventh: boolean;
  hasSharpEleventh: boolean;
  hasThirteenth: boolean;
  hasFlatThirteenth: boolean;
  hasNaturalThirteenth: boolean;
  hasSecond: boolean;
  hasFlatSecond: boolean;
  hasNaturalSecond: boolean;
  hasFourth: boolean;
  hasNaturalFourth: boolean;
  hasSharpFourth: boolean;
  hasSixth: boolean;
  hasFlatSixth: boolean;
  hasNaturalSixth: boolean;
  addedNotes: Array<[number, string]>;
}
