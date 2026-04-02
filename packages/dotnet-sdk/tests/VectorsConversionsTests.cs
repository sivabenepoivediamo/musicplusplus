using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class VectorsConversionsTests
{
    [Fact]
    public void positions_to_intervals_empty_and_singleton()
    {
        var emptyPv = new PositionVector(new List<int>(), 12, 12, true, false);
        var ivEmpty = MusicVectors.PositionsToIntervals(emptyPv);
        Assert.Empty(ivEmpty.MutableData);
        Assert.Equal(0, ivEmpty.Offset);
        Assert.Equal(12, ivEmpty.Mod);

        var one = new PositionVector(Ints(5), 12, 12);
        var ivOne = MusicVectors.PositionsToIntervals(one);
        Assert.Empty(ivOne.MutableData);
        Assert.Equal(5, ivOne.Offset);
    }

    [Fact]
    public void positions_to_intervals_closure()
    {
        var triad = new PositionVector(Ints(0, 4, 7), 12, 12);
        var iv = MusicVectors.PositionsToIntervals(triad);
        AssertIvEq(iv, new[] { 4, 3, 5 }, 0, 12);
    }

    [Fact]
    public void intervals_to_positions_roundtrip()
    {
        var iv = new IntervalVector(Ints(2, 2, 1), 0, 12);
        var pv = MusicVectors.IntervalsToPositions(iv);
        AssertPvEq(pv, new[] { 0, 2, 4 }, 12);

        var emptyIv = new IntervalVector(new List<int>(), 3, 12);
        var pv0 = MusicVectors.IntervalsToPositions(emptyIv);
        AssertPvEq(pv0, new[] { 3 }, 12);
    }

    [Fact]
    public void positions_to_onset_uses_minimum_pitch_not_first_element()
    {
        var unsorted = new PositionVector(Ints(5, 2, 9), 12, 12);
        var o = MusicVectors.PositionsToOnset(unsorted);
        Assert.Equal(2, o.Offset);
        Assert.Equal(3, o.CountPulses());
        Assert.Equal(1, o.MutableData[0]);
        Assert.Equal(1, o.MutableData[3]);
        Assert.Equal(1, o.MutableData[7]);
    }

    [Fact]
    public void positions_to_onset_and_roundtrip_positions()
    {
        var empty = new PositionVector(new List<int>(), 12, 12, true, false);
        var bin = MusicVectors.PositionsToOnset(empty);
        Assert.Empty(bin.MutableData);

        var triad = new PositionVector(Ints(0, 4, 7), 12, 12);
        var b = MusicVectors.PositionsToOnset(triad);
        Assert.Equal(3, b.CountPulses());

        var ivBack = MusicVectors.PositionsToIntervals(triad);
        var pvBack = MusicVectors.IntervalsToPositions(ivBack);
        AssertSeqEqual(pvBack.MutableData, triad.MutableData);
    }

    [Fact]
    public void vector_set_set_mod_propagates_to_representations()
    {
        var s = VectorSet.FromPositions(Ints(0, 4, 7));
        s.SetMod(24);
        Assert.Equal(24, s.Mod);
        Assert.Equal(24, s.Positions.Mod);
        Assert.Equal(24, s.Intervals.Mod);
        Assert.Equal(24, s.Onset.Mod);
    }

    [Fact]
    public void vector_set_constructors_and_equality()
    {
        var def = new VectorSet();
        Assert.Equal(12, def.Mod);
        AssertPvEq(def.Positions, new[] { 0 }, 12);

        var mod7 = new VectorSet(7);
        Assert.Equal(7, mod7.Mod);

        var a = VectorSet.FromPositions(Ints(0, 3, 7));
        var b = VectorSet.FromPositions(Ints(0, 3, 7));
        var c = VectorSet.FromPositions(Ints(0, 4, 7));
        Assert.True(a.Equals(b));
        Assert.False(a.Equals(c));
    }

    [Fact]
    public void vector_set_from_interval_and_onset()
    {
        var fromIv = new VectorSet(new IntervalVector(Ints(2, 2, 1), 0, 12));
        AssertPvEq(fromIv.Positions, new[] { 0, 2, 4 }, 12);

        var fromBv = VectorSet.FromOnset(Ints(1, 0, 1, 0, 1), 0, 5);
        AssertPvEq(fromBv.Positions, new[] { 0, 2, 4 }, 5);
    }

    [Fact]
    public void vector_set_inversion_alias_matches_relative_mode()
    {
        var s = VectorSet.FromPositions(Ints(0, 4, 7));
        AssertSeqEqual(s.Inversion(2, 0).Positions.MutableData, s.RelativeMode(2, 0).Positions.MutableData);
    }

    [Fact]
    public void vector_set_transpose_multiply_complement()
    {
        var s = VectorSet.FromPositions(Ints(0, 4, 7));
        var t = s.Transpose(3);
        Assert.Equal(12, t.Mod);
        AssertPvEq(t.Positions, new[] { 3, 7, 10 }, 12);

        var m = s.MultiplyPositions(2);
        AssertPvEq(m.Positions, new[] { 0, 8, 14 }, 12);

        var c = s.ComplementPositions();
        Assert.Equal(9, (int)c.Positions.Count);
        Assert.Equal(12, c.Mod);
    }

    [Fact]
    public void vector_set_mode_parallel_mode_intervals()
    {
        var s = VectorSet.FromIntervals(Ints(2, 2, 1), 12);
        var r = s.Mode(1);
        // C# recomputes intervals from positions (closure uses effective range); data differs from C++ raw {2,2,1}.
        AssertIvEq(r.Intervals, new[] { 2, 8, 2 }, 0, 12);
    }

    [Fact]
    public void vector_set_onset_or_and_and()
    {
        var a = VectorSet.FromOnset(Ints(1, 0, 1, 0, 0), 0, 5);
        var b = VectorSet.FromOnset(Ints(0, 1, 1, 0, 0), 0, 5);
        var u = a | b;
        var n = a & b;
        Assert.Equal(3, u.Onset.CountPulses());
        Assert.Equal(1, n.Onset.CountPulses());
    }

    [Fact]
    public void vector_set_reverse_intervals()
    {
        var s = VectorSet.FromIntervals(Ints(2, 2, 1), 12);
        var rev = s.ReverseIntervals();
        AssertIvEq(rev.Intervals, new[] { 8, 2, 2 }, 0, 12);
    }

    [Fact]
    public void vector_set_add_to_intervals()
    {
        var s = VectorSet.FromIntervals(Ints(2, 2, 1), 12);
        var t = s.AddToIntervals(1);
        AssertIvEq(t.Intervals, new[] { 3, 3, 9 }, 0, 12);
    }

    [Fact]
    public void vector_set_onset_xor()
    {
        var a = VectorSet.FromOnset(Ints(1, 0, 1, 0, 0), 0, 5);
        var b = VectorSet.FromOnset(Ints(1, 1, 0, 0, 0), 0, 5);
        var x = a ^ b;
        Assert.Equal(2, x.Onset.CountPulses());
    }

    [Fact]
    public void vector_set_invert_positions_preserves_size()
    {
        var s = VectorSet.FromPositions(Ints(0, 4, 7));
        var inv = s.InvertPositions(0);
        Assert.Equal(3, (int)inv.Positions.Count);
        Assert.Equal(12, inv.Mod);
    }

    [Fact]
    public void vector_set_multiply_intervals()
    {
        var s = VectorSet.FromIntervals(Ints(2, 2, 1), 12);
        var m = s.MultiplyIntervals(2);
        AssertIvEq(m.Intervals, new[] { 4, 4, 16 }, 0, 12);
    }

    [Fact]
    public void vector_set_rotate_onset_preserves_pulse_count()
    {
        var s = VectorSet.FromOnset(Ints(1, 0, 0, 1, 0), 0, 5);
        var r = s.RotateOnset(2);
        Assert.Equal(5, r.Onset.Mod);
        Assert.Equal(5, r.Onset.Count);
        Assert.Equal(s.Onset.CountPulses(), r.Onset.CountPulses());
    }

    [Fact]
    public void vector_set_complement_onset()
    {
        var s = VectorSet.FromOnset(Ints(1, 0, 0, 1, 0), 0, 5);
        var c = s.ComplementOnset();
        Assert.Equal(5, c.Onset.Mod);
        Assert.Equal(5, c.Onset.CountPulses() + s.Onset.CountPulses());
    }

    [Fact]
    public void vector_set_invert_intervals()
    {
        var s = VectorSet.FromIntervals(Ints(2, 2, 1), 12);
        var iv = s.InvertIntervals(1);
        Assert.Equal(3, iv.Intervals.Count);
        Assert.Equal(12, iv.Intervals.Mod);
    }
}
