using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class ChordTests
{
    [Fact]
    public void chord_helper_examples()
    {
        var cMajor = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12);
        var majorIv = new IntervalVector(Ints(2, 2, 1, 2, 2, 2, 1), 0, 12);
        var triadDegrees = new PositionVector(Ints(0, 2, 4), 12);
        var grouping = new IntervalVector(Ints(2), 0, 12);

        var positionsFromDegrees = ChordFunctions.Chord(cMajor, triadDegrees, 0, 0, 5, 0);
        var positionsFromGrouping = ChordFunctions.Chord(cMajor, grouping, 0, 0, 5, 0);
        var intervalsFromGrouping = ChordFunctions.Chord(majorIv, grouping, 0, 0, 5, 0);
        var intervalsFromDegrees = ChordFunctions.Chord(majorIv, triadDegrees, 0, 0, 5, 0);

        AssertPvEq(positionsFromDegrees, new[] { 0, 4, 7, 12, 16 }, 12);
        AssertPvEq(positionsFromGrouping, new[] { 0, 4, 7, 11, 14 }, 12);
        AssertIvEq(intervalsFromGrouping, new[] { 4, 3, 4, 3, 3 }, 0, 12);
        AssertIvEq(intervalsFromDegrees, new[] { 4, 3, 5, 4, 3 }, 0, 12);
    }

    [Fact]
    public void chord_class_examples()
    {
        var cMajor = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12);
        var majorIv = new IntervalVector(Ints(2, 2, 1, 2, 2, 2, 1), 0, 12);
        var triadDegrees = new PositionVector(Ints(0, 2, 4), 12);
        var grouping = new IntervalVector(Ints(2), 0, 12);

        var p = new ChordParams { Shift = 0, RelativeMode = 1, PreVoices = 3 };

        var chord1 = new Chord(cMajor, triadDegrees, p);
        var chord2 = new Chord(cMajor, grouping, p);
        var chord3 = new Chord(majorIv, grouping, p);
        var chord4 = new Chord(majorIv, triadDegrees, p);

        AssertPvEq(chord1.ToPositions(), new[] { 4, 7, 12 }, 12);
        AssertPvEq(chord2.ToPositions(), new[] { 4, 7, 12 }, 12);
        AssertIvEq(chord3.ToIntervals(), new[] { 3, 4, 4 }, 4, 12);
        AssertIvEq(chord4.ToIntervals(), new[] { 3, 5, 4 }, 4, 12);

        chord1.SetCriterionMode(2);
        AssertPvEq(chord1.ToPositions(), new[] { 12, 16, 19 }, 12);

        var invertedP = new ChordParams
        {
            Shift = 0,
            RelativeMode = 0,
            PreVoices = 3,
            Invert = true,
            Axis = 6
        };
        var invertedChord = new Chord(cMajor, triadDegrees, invertedP);
        AssertPvEq(invertedChord.ToPositions(), new[] { -7, -4, 0 }, 12);
    }

    [Fact]
    public void chord_name_examples()
    {
        var cMajor = new[] { 60, 64, 67 };
        var cMinor = new[] { 60, 63, 67 };
        var cDom7 = new[] { 60, 64, 67, 70 };
        var diminished7 = new[] { 60, 63, 66, 69 };
        var dMinor7 = new[] { 62, 65, 69, 72 };

        Assert.Equal("C", ChordNames.BuildChordName(ChordNames.AnalyzeChord(cMajor, 0)));
        Assert.Equal("Cmin", ChordNames.BuildChordName(ChordNames.AnalyzeChord(cMinor, 0)));
        Assert.Equal("C7", ChordNames.BuildChordName(ChordNames.AnalyzeChord(cDom7, 0)));
        Assert.Equal("Cdim7", ChordNames.BuildChordName(ChordNames.AnalyzeChord(diminished7, 0)));
        Assert.Equal("Dmin7", ChordNames.BuildChordName(ChordNames.AnalyzeChord(dMinor7, 0)));
    }
}
