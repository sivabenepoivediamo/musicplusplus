using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class RhythmUtilTests
{
    [Fact]
    public void euclidean_interval_vector_even_division()
    {
        var steps = RhythmAlgorithms.EuclideanIntervals(12, 4);
        AssertSeqEqual(steps, new[] { 3, 3, 3, 3 });
    }

    [Fact]
    public void phrase_length_formula()
    {
        Assert.Equal(32, RhythmAlgorithms.PhraseLength(10, 2, 12, 2, 1));
        Assert.Equal(0, RhythmAlgorithms.PhraseLength(0, 0, 8, 0, 2));
    }

    [Fact]
    public void tihai_generator_rounds_length()
    {
        var p = RhythmAlgorithms.TihaiGenerator(16, 3);
        Assert.Equal(4, p.Bols);
        Assert.Equal(2, p.Dams);
    }

    [Fact]
    public void tihai_edge_steps_and_repetitions()
    {
        var a = RhythmAlgorithms.Tihai(2, 5, false);
        Assert.Equal(2, a.Count);
        Assert.True(RhythmAlgorithms.IsAllOnes(a));

        var z = RhythmAlgorithms.Tihai(8, 0, false);
        Assert.Equal(8, z.Count);
        Assert.True(RhythmAlgorithms.IsAllZeros(z));

        var oneRep = RhythmAlgorithms.Tihai(6, 1, false);
        Assert.True(RhythmAlgorithms.IsAllOnes(oneRep));
    }

    [Fact]
    public void is_all_zeros_and_ones()
    {
        Assert.True(RhythmAlgorithms.IsAllZeros(new[] { 0, 0, 0 }));
        Assert.False(RhythmAlgorithms.IsAllZeros(new[] { 0, 1 }));
        Assert.True(RhythmAlgorithms.IsAllOnes(new[] { 1, 1 }));
        Assert.False(RhythmAlgorithms.IsAllOnes(new[] { 1, 0 }));
    }

    [Fact]
    public void append_ones_and_cut()
    {
        var v = new List<int> { 1 };
        RhythmAlgorithms.AppendOnes(v, 4);
        AssertSeqEqual(v, new[] { 1, 1, 1, 1 });

        AssertSeqEqual(RhythmAlgorithms.Cut(new[] { 1, 2, 3, 4 }, 2), new[] { 1, 2 });
        AssertSeqEqual(RhythmAlgorithms.Cut(new[] { 9 }, 10), new[] { 9 });
    }

    [Fact]
    public void tihai_reader_matches_generator_bols_dams()
    {
        var gen = RhythmAlgorithms.TihaiGenerator(16, 3);
        var pat = RhythmAlgorithms.TihaiReader(gen.Bols, gen.Dams, 3, 16);
        Assert.Equal(16, pat.Count);
    }

    [Fact]
    public void clough_douthett_positions()
    {
        AssertSeqEqual(RhythmAlgorithms.CloughDouthettPositions(12, 4), new[] { 0, 3, 6, 9 });
        AssertSeqEqual(RhythmAlgorithms.CloughDouthettPositions(8, 3), new[] { 0, 2, 5 });
    }

    [Fact]
    public void deep_rhythm_sorted_positions()
    {
        AssertSeqEqual(RhythmAlgorithms.DeepRhythmPositions(8, 4, 3), new[] { 0, 1, 3, 6 });
    }

    [Fact]
    public void clough_douthett_vector_offset()
    {
        var pv = RhythmAlgorithms.CloughDouthettVector(12, 4, 5);
        AssertPvEq(pv, new[] { 5, 8, 11, 14 }, 12);
    }

    [Fact]
    public void deep_rhythm_position_vector_offset()
    {
        var pv = RhythmAlgorithms.DeepRhythmVector(8, 4, 3, 10);
        AssertPvEq(pv, new[] { 10, 11, 13, 16 }, 8);
    }

    [Fact]
    public void euclidean_interval_vector_odd_split()
    {
        var iv = RhythmAlgorithms.EuclideanAsIntervalVector(12, 5, 2);
        Assert.Equal(5, iv.Count);
        Assert.Equal(12, iv.Mod);
        Assert.Equal(2, iv.Offset);
        Assert.Equal(12, iv.MutableData.Sum());
    }

    [Fact]
    public void tihai_onset_vector_wrap()
    {
        var bv = RhythmAlgorithms.TihaiOnset(8, 3, false, 1);
        Assert.Equal(8, bv.Mod);
        Assert.Equal(8, bv.Count);
        Assert.Equal(1, bv.Offset);
    }
}
