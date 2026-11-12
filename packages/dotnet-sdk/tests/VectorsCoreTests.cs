using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class VectorsCoreTests
{
    [Fact]
    public void vectors_construction_and_transforms()
    {
        var majorTriad = VectorSet.FromPositions(Ints(0, 4, 7));
        AssertPvEq(majorTriad.Positions, new[] { 0, 4, 7 }, 12);
        AssertIvEq(majorTriad.Intervals, new[] { 4, 3, 5 }, 0, 12);
        AssertOvEq(
            majorTriad.Onset,
            new[] { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0 },
            0,
            12);

        var fromIntervals = VectorSet.FromIntervals(Ints(4, 3, 5));
        AssertPvEq(fromIntervals.Positions, new[] { 0, 4, 7 }, 12);

        var fromBinary = VectorSet.FromOnset(new[] { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0 });
        AssertPvEq(fromBinary.Positions, new[] { 0, 4, 7 }, 12);

        var euclidean = VectorSet.Euclidean(5, 8);
        AssertPvEq(euclidean.Positions, new[] { 0, 2, 3, 5, 7 }, -1);
        AssertIvEq(euclidean.Intervals, new[] { 2, 1, 2, 2, 1 }, 0, -1);
        AssertOvEq(euclidean.Onset, new[] { 1, 0, 1, 1, 0, 1, 0, 1 }, 0, -1);
    }

    [Fact]
    public void vectors_operator_examples()
    {
        var majorTriad = VectorSet.FromPositions(Ints(0, 4, 7));

        AssertPvEq(majorTriad.Transpose(5).Positions, new[] { 5, 9, 12 }, 12);
        AssertPvEq(majorTriad.MultiplyPositions(5).Positions, new[] { 0, 20, 35 }, 12);
        AssertPvEq(majorTriad.Rotate(1).Positions, new[] { 7, 0, 4 }, 12);
        AssertPvEq(majorTriad.RelativeMode(1).Positions, new[] { 4, 7, 12 }, 12);
        AssertPvEq(majorTriad.InvertPositions(0).Positions, new[] { -7, -4, 0 }, 12);
        AssertPvEq(
            majorTriad.ComplementPositions().Positions,
            new[] { 1, 2, 3, 5, 6, 8, 9, 10, 11 },
            12);

        AssertPvEq(majorTriad.AddToIntervals(2).Positions, new[] { 0, 6, 11 }, 12);
        AssertPvEq(majorTriad.MultiplyIntervals(2).Positions, new[] { 0, 8, 14 }, 12);
        AssertPvEq(majorTriad.ParallelMode(1).Positions, new[] { 0, 3, 8 }, 12);
        AssertPvEq(majorTriad.ReverseIntervals().Positions, new[] { 0, 5, 8 }, 12);
        AssertPvEq(majorTriad.InvertIntervals(0).Positions, new[] { 0, 5, 8 }, 12);

        AssertOvEq(
            majorTriad.RotateOnset(3).Onset,
            new[] { 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 },
            0,
            12);
        AssertPvEq(
            majorTriad.ComplementOnset().Positions,
            new[] { 1, 2, 3, 5, 6, 8, 9, 10, 11 },
            12);
        AssertPvEq(majorTriad.MultiplyOnset(2).Positions, new[] { 0, 8, 14 }, 24);

        var pentatonic = VectorSet.FromPositions(Ints(0, 2, 4, 7, 9));
        AssertPvEq(pentatonic.DivideOnset(2).Positions, new[] { 0, 1, 2 }, 6);

        var setA = VectorSet.FromPositions(Ints(0, 2, 4, 6));
        var setB = VectorSet.FromPositions(Ints(0, 3, 6, 9));
        AssertPvEq((setA | setB).Positions, new[] { 0, 2, 3, 4, 6, 9 }, 12);
        AssertPvEq((setA & setB).Positions, new[] { 0, 6 }, 12);
        AssertPvEq((setA ^ setB).Positions, new[] { 2, 3, 4, 9 }, 12);

        var motif = VectorSet.FromPositions(Ints(0, 4, 7));
        AssertPvEq(motif.Transpose(7).Positions, new[] { 7, 11, 14 }, 12);
        AssertPvEq(motif.Negative().Positions, new[] { 7, 12, 15 }, 12);
        AssertPvEq(motif.InvertPositions(0).Positions, new[] { -7, -4, 0 }, 12);
        AssertPvEq(motif.RelativeMode(3, 4).Positions, new[] { 12, 16, 19, 24 }, 12);

        AssertOvEq(VectorSet.Euclidean(3, 8).Onset, new[] { 1, 0, 0, 1, 0, 1, 0, 0 }, 0, -1);
        AssertOvEq(VectorSet.Euclidean(5, 8).Onset, new[] { 1, 0, 1, 1, 0, 1, 0, 1 }, 0, -1);
        AssertOvEq(VectorSet.Euclidean(5, 12).Onset, new[] { 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0 }, 0, -1);
        AssertOvEq(
            VectorSet.Euclidean(7, 16).Onset,
            new[] { 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0 },
            0,
            -1);
    }

    [Fact]
    public void vectors_roundtrip_and_invariants()
    {
        var source = VectorSet.FromPositions(Ints(0, 3, 7, 10));

        AssertPvEq(
            VectorSet.FromIntervals(source.Intervals.MutableData).Positions,
            source.Positions.MutableData.ToArray(),
            12);
        AssertPvEq(
            VectorSet.FromOnset(source.Onset.MutableData).Positions,
            source.Positions.MutableData.ToArray(),
            12);

        var transposed = source.Transpose(1);
        AssertPvEq(transposed.Positions, new[] { 1, 4, 8, 11 }, 12);
        AssertIvEq(transposed.Intervals, source.Intervals.MutableData.ToArray(), 1, 12);
        AssertSeqEqual(
            VectorSet.FromOnset(transposed.Onset.MutableData).Positions.MutableData,
            NormalizePositions(transposed.Positions));
        AssertSeqEqual(
            NormalizePositions(transposed.Positions),
            VectorSet.FromIntervals(transposed.Intervals.MutableData).Positions.MutableData);

        var rotated = source.Rotate(1);
        AssertOvEq(
            VectorSet.FromOnset(rotated.Onset.MutableData).Onset,
            rotated.Onset.MutableData.ToArray(),
            rotated.Onset.Offset,
            rotated.Onset.Mod);
        AssertPvEq(rotated.Rotate(3).Positions, source.Positions.MutableData.ToArray(), 12);

        var inverted = source.InvertPositions(0);
        AssertSeqEqual(
            NormalizePositions(inverted.InvertPositions(0).Positions),
            source.Positions.MutableData);
        AssertSeqEqual(
            NormalizePositions(inverted.Positions),
            VectorSet.FromIntervals(inverted.Intervals.MutableData).Positions.MutableData);
    }

    [Fact]
    public void interval_vector_examples()
    {
        var iv1 = new IntervalVector();
        AssertIvEq(iv1, new[] { 0 }, 0, 12);

        var iv2 = new IntervalVector(Ints(2, 3, 5, 7), 1, 12);
        Assert.Equal(4, iv2.Count);
        Assert.Equal(1, iv2.Offset);
        Assert.Equal(12, iv2.Mod);
        Assert.False(iv2.IsEmpty);

        iv2.SetOffset(3);
        iv2.SetMod(24);
        iv2.SetData(Ints(1, 4, 7));
        AssertIvEq(iv2, new[] { 1, 4, 7 }, 3, 24);

        var iv3 = new IntervalVector(Ints(1, 2, 3, 4));
        AssertIvEq(iv3 + 5, new[] { 6, 7, 8, 9 }, 0, 12);
        AssertIvEq(iv3 - 2, new[] { -1, 0, 1, 2 }, 0, 12);
        AssertIvEq(iv3 * 3, new[] { 3, 6, 9, 12 }, 0, 12);
        AssertIvEq(iv3 / 2, new[] { 0, 1, 1, 2 }, 0, 12);
        AssertIvEq(iv3 % 3, new[] { 1, 2, 0, 1 }, 0, 12);

        var iv4 = new IntervalVector(Ints(1, 2, 3));
        var iv5 = new IntervalVector(Ints(4, 5, 6));
        AssertIvEq(iv4 + iv5, new[] { 5, 7, 9 }, 0, 12);
        AssertIvEq(iv4 - iv5, new[] { -3, -3, -3 }, 0, 12);
        AssertIvEq(iv4 * iv5, new[] { 4, 10, 18 }, 0, 12);
        AssertIvEq(iv4 / iv5, new[] { 0, 0, 0 }, 0, 12);
        AssertIvEq(iv4 % iv5, new[] { 1, 2, 3 }, 0, 12);

        var iv8 = new IntervalVector(Ints(10, 20, 30, 40));
        Assert.Equal(10, iv8[0]);
        Assert.Equal(40, iv8[-1]);

        var iv12 = new IntervalVector(Ints(1, 2, 3, 4, 5));
        AssertIvEq(iv12.ParallelMode(2), new[] { 3, 4, 5, 1, 2 }, 0, 12);
        AssertIvEq(iv12.Reverse(), new[] { 5, 4, 3, 2, 1 }, 0, 12);

        var iv13 = new IntervalVector(Ints(1, 2, 3, 4, 5, 6));
        AssertIvEq(iv13.Inversion(1), new[] { 1, 6, 5, 4, 3, 2 }, 0, 12);

        var iv14 = new IntervalVector(Ints(13, 25, -3, 14));
        AssertIvEq(iv14.Normalize(12), new[] { 1, 1, 9, 2 }, 0, 12);

        var iv15 = new IntervalVector(Ints(1, 2, 3));
        AssertIvEq(iv15.ComponentwiseSum(Ints(4, 5), true), new[] { 5, 7, 7 }, 0, 12);

        var iv16 = new IntervalVector(Ints(1, 2, 3));
        var iv17 = new IntervalVector(Ints(4, 5));
        AssertIvEq(iv16.Concatenate(iv17), new[] { 1, 2, 3, 4, 5 }, 0, 12);
        AssertIvEq(iv16.Repeat(3), new[] { 1, 2, 3, 1, 2, 3, 1, 2, 3 }, 0, 12);

        var iv18 = new IntervalVector(Ints(1, 2, 3, 4, 5, 6));
        AssertIvEq(iv18.SingleMirror(3, true), new[] { 3, 2, 1, 4, 5, 6 }, 0, 12);
        AssertIvEq(iv18.DoubleMirror(3), new[] { 3, 2, 1, 6, 5, 4 }, 0, 12);
    }

    [Fact]
    public void onset_vector_examples()
    {
        var bv1 = new OnsetVector();
        AssertOvEq(bv1, new[] { 1, 0, 0, 0 }, 0, 4);

        var bv2 = new OnsetVector(Ints(1, 0, 1, 1, 0), 0, 5);
        Assert.Equal(3, bv2.CountPulses());
        Assert.Equal(0.6, bv2.Density(), 6);

        var bv7 = new OnsetVector(Ints(1, 0, 1, 0, 1));
        AssertOvEq(~bv7, new[] { 0, 1, 0, 1, 0 }, 0, 4);
        AssertOvEq(bv7.Complement(), new[] { 0, 1, 0, 1, 0 }, 0, 4);

        var bv16 = new OnsetVector(Ints(1, 0, 0, 1, 0));
        AssertOvEq(bv16.Rotate(1), new[] { 0, 0, 1, 0, 1 }, 0, 4);
        AssertOvEq(bv16.Rotate(2), new[] { 0, 1, 0, 1, 0 }, 0, 4);

        var bv17 = new OnsetVector(Ints(1, 0, 1, 0, 1));
        AssertOvEq(bv17.Inversion(0), new[] { 1, 1, 0, 1, 0 }, 0, 4);

        var bv19 = new OnsetVector(Ints(1, 0, 1));
        var bv20 = new OnsetVector(Ints(0, 1));
        AssertOvEq(bv19.Concatenate(bv20), new[] { 1, 0, 1, 0, 1 }, 0, 4);
        AssertOvEq(bv19.Repeat(3), new[] { 1, 0, 1, 1, 0, 1, 1, 0, 1 }, 0, 4);

        var bv21 = new OnsetVector(Ints(1, 0, 0, 1, 0, 1, 0, 0));
        Assert.Equal(3, bv21.CountPulses());
        Assert.Equal(0.375, bv21.Density(), 6);
        AssertSeqEqual(bv21.GetPulseIndices(), new[] { 0, 3, 5 });
        AssertSeqEqual(bv21.GetInterOnsetIntervals(), new[] { 3, 2, 3 });

        AssertOvEq(OnsetVector.Euclidean(3, 8), new[] { 1, 0, 0, 1, 0, 1, 0, 0 }, 0, -1);
        AssertOvEq(OnsetVector.Euclidean(5, 12), new[] { 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0 }, 0, -1);
    }

    [Fact]
    public void position_vector_examples()
    {
        var pv1 = new PositionVector();
        AssertPvEq(pv1, new[] { 0 }, 12);

        var pv2 = new PositionVector(Ints(0, 2, 4, 7, 9), 12, 0, true, false);
        Assert.Equal(5, pv2.Count);
        Assert.Equal(12, pv2.Mod);
        Assert.Equal(12, pv2.UserRange);
        Assert.Equal(12, pv2.EffectiveRange);
        Assert.True(pv2.AutoRangeUpdates);
        Assert.False(pv2.UseUserRange);

        var pv3 = new PositionVector(Ints(0, 3, 7, 10));
        AssertPvEq(pv3 + 5, new[] { 5, 8, 12, 15 }, 12);
        AssertPvEq(pv3 - 2, new[] { -2, 1, 5, 8 }, 12);
        AssertPvEq(pv3 * 2, new[] { 0, 6, 14, 20 }, 12);
        AssertPvEq(pv3 / 2, new[] { 0, 1, 3, 5 }, 12);
        AssertPvEq(pv3 % 3, new[] { 0, 0, 1, 1 }, 12);

        var pv12 = new PositionVector(Ints(0, 3, 7, 10));
        AssertPvEq(pv12.Rotate(1), new[] { 10, 0, 3, 7 }, 12);
        AssertPvEq(pv12.Rotate(2), new[] { 7, 10, 0, 3 }, 12);

        var pv14 = new PositionVector(Ints(0, 3, 7));
        AssertPvEq(pv14.Complement(), new[] { 1, 2, 4, 5, 6, 8, 9, 10, 11 }, 12);

        var pv15 = new PositionVector(Ints(0, 3, 7, 11));
        AssertPvEq(pv15.Inversion(0), new[] { -11, -7, -3, 0 }, 12);

        var pv18 = new PositionVector(Ints(0, 3, 7));
        var pv19 = new PositionVector(Ints(10, 14));
        AssertPvEq(pv18.Concatenate(pv19), new[] { 0, 3, 7, 10, 14 }, 12);

        var pv22 = new PositionVector(Ints(0, 5, 12, 18), 12, 0, true, false);
        Assert.Equal(24, pv22.EffectiveRange);
    }
}
