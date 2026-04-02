using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class RhythmTests
{
    private static List<int> ApplyOrnament(
        int note,
        IReadOnlyList<int> chord,
        IReadOnlyList<int> scale,
        IReadOnlyList<int> chromatic,
        int mod,
        IReadOnlyList<int> ornaments,
        IReadOnlyList<VectorModification.Kind> types)
    {
        var analysis = Melody.Hierarchy(note, chord, scale, chromatic, mod);
        var expandedTypes = Melody.OrnamentTypesLoop(types, ornaments.Count);
        var modifications = Melody.ParseModifications(ornaments, expandedTypes);
        return Melody.TripleSelect(analysis, modifications, chord, scale, chromatic, mod).Results;
    }

    [Fact]
    public void rhythm_generation_examples()
    {
        var ev = RhythmAlgorithms.EuclideanAsIntervalVector(16, 3, 0);
        AssertIvEq(ev, new[] { 5, 5, 6 }, 0, 16);

        var euc = VectorSet.FromIntervals(ev.MutableData, 16);
        AssertPvEq(euc.Positions, new[] { 0, 5, 10 }, 16);

        var euc2 = euc.Mode(-1);
        AssertPvEq(euc2.Positions, new[] { 0, 6, 11 }, 16);
        AssertPvEq(euc2.Transpose(1).Positions, new[] { 1, 7, 12 }, 16);
        AssertPvEq(euc2.RelativeMode(2).Positions, new[] { 11, 16, 22 }, 16);

        AssertPvEq(RhythmAlgorithms.CloughDouthettVector(16, 3, 0), new[] { 0, 5, 10 }, 16);
        AssertPvEq(RhythmAlgorithms.DeepRhythmVector(16, 3, 5, 0), new[] { 0, 5, 10 }, 16);
        AssertOvEq(
            new OnsetVector(RhythmAlgorithms.Tihai(16, 3, false), 0, 16),
            new[] { 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1 },
            0,
            16);
    }

    [Fact]
    public void melody_examples()
    {
        var chord = Ints(0, 4, 7);
        var scale = Ints(0, 2, 4, 5, 7, 9, 11);
        var chromatic = Enumerable.Range(0, 12).ToList();
        const int mod = 12;

        var loop10 = Melody.OrnamentLoop(new[] { 1, 0 }, 7);
        var diminutionResult = Melody.Diminution(0, 4, false, true);
        var runResult = Melody.Run(4, 6, true);
        var run2Result = Melody.Run2(3, -3);
        var scaleOrnament = ApplyOrnament(60, chord, scale, chromatic, mod, Melody.OrnamentLoop(new[] { 1, 0 }, 7), new[] { VectorModification.Kind.Scale });
        var downwardOrnament = ApplyOrnament(60, chord, scale, chromatic, mod, Melody.OrnamentLoop(new[] { -1, 0 }, 5), new[] { VectorModification.Kind.Scale });
        var mixedOrnament = ApplyOrnament(60, chord, scale, chromatic, mod, Melody.OrnamentLoop(new[] { 1, 0, -1, 0 }, 5), new[] { VectorModification.Kind.Scale, VectorModification.Kind.Chromatic });
        var chordRun = ApplyOrnament(60, chord, scale, chromatic, mod, Melody.Run(3, 5, true), new[] { VectorModification.Kind.Chord });
        var chordRun2 = ApplyOrnament(60, chord, scale, chromatic, mod, Melody.Run2(0, 3), new[] { VectorModification.Kind.Chord });
        var phrase = ApplyOrnament(
            60,
            chord,
            scale,
            chromatic,
            mod,
            new[] { 0, 1, 2, 0, 0, 1, 2, 0, 2, 3, 4, 2, 3, 4 },
            new[] { VectorModification.Kind.Scale });

        AssertSeqEqual(loop10, new[] { 0, 1, 0, 1, 0, 1, 0 });
        AssertSeqEqual(diminutionResult, new[] { 0, 1, 2, 0 });
        AssertSeqEqual(runResult, new[] { -1, 0, 1, 2, 3, 4 });
        AssertSeqEqual(run2Result, new[] { 3, 2, 1, 0, -1, -2, -3 });

        AssertSeqEqual(scaleOrnament, new[] { 60, 62, 60, 62, 60, 62, 60 });
        AssertSeqEqual(downwardOrnament, new[] { 60, 59, 60, 59, 60 });
        AssertSeqEqual(mixedOrnament, new[] { 60, 62, 60, 59, 60 });
        AssertSeqEqual(chordRun, new[] { 55, 60, 64, 67, 72 });
        AssertSeqEqual(chordRun2, new[] { 60, 64, 67, 72 });
        AssertSeqEqual(phrase, new[] { 60, 62, 64, 60, 60, 62, 64, 60, 64, 65, 67, 64, 65, 67 });
    }
}
