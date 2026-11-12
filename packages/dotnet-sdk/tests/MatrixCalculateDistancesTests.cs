using Musicpp;

namespace Musicplusplus.Tests;

public class MatrixCalculateDistancesTests
{
    [Fact]
    public void Calculate_distances_modal_interval_euclidean()
    {
        var reference = new IntervalVector(TestHelpers.Ints(2, 2, 1, 2, 2, 2, 1), 0, 12);
        var other = new IntervalVector(TestHelpers.Ints(2, 2, 2, 1, 2, 2, 1), 0, 12);
        var modes = MatrixOps.ModalMatrix(reference);
        var mmd = MatrixDistanceOps.CalculateDistances(other, modes, Distances.ManhattanDistance, true);
        Assert.True(mmd.Count > 0);
        var closest = mmd.GetClosest();
        Assert.Equal(0.0, closest.Distance, 6);
        Assert.Equal(3, closest.Index);
    }

    [Fact]
    public void Calculate_distances_modal_position_sort_flag()
    {
        var cMajor = new PositionVector(TestHelpers.Ints(0, 2, 4, 5, 7, 9, 11));
        var cLydian = new PositionVector(TestHelpers.Ints(0, 2, 4, 6, 7, 9, 11));
        var modes = MatrixOps.ModalMatrix(cMajor);
        var sorted = MatrixDistanceOps.CalculateDistances(cLydian, modes, Distances.ManhattanDistance, true);
        var unsorted = MatrixDistanceOps.CalculateDistances(cLydian, modes, Distances.ManhattanDistance, false);
        Assert.Equal(sorted.Count, unsorted.Count);
        Assert.Equal(0.0, sorted.GetClosest().Distance, 6);
        var firstRowDist = unsorted[0].Distance;
        Assert.True(firstRowDist > 1e-6);
        Assert.Equal(sorted.GetClosest().Distance, sorted[0].Distance, 9);
    }

    [Fact]
    public void Calculate_distances_transposition_matrix_closest()
    {
        var cMajor = new PositionVector(TestHelpers.Ints(0, 2, 4, 5, 7, 9, 11));
        var cLydian = new PositionVector(TestHelpers.Ints(0, 2, 4, 6, 7, 9, 11));
        var tmd = MatrixDistanceOps.CalculateDistances(cLydian, MatrixOps.TranspositionMatrix(cMajor), Distances.ManhattanDistance, true);
        Assert.True(tmd.Count > 0);
        var best = tmd.GetClosest();
        Assert.Equal(0.0, best.Distance, 6);
        Assert.Equal(7, best.Transposition);
    }

    [Fact]
    public void Align_chord_pair()
    {
        var cMajorChord = new PositionVector(TestHelpers.Ints(0, 4, 7));
        var gMajorChord = new PositionVector(TestHelpers.Ints(7, 11, 14));
        Assert.Equal(-2, MatrixDistanceOps.Align(cMajorChord, gMajorChord));
    }

    [Fact]
    public void Calculate_distances_modal_relative_mode_closest()
    {
        var cMajor = new PositionVector(TestHelpers.Ints(0, 2, 4, 5, 7, 9, 11));
        var crit = new IntervalVector(TestHelpers.Ints(2, 2, 3), 0, 12);
        var gMajorChord = new PositionVector(TestHelpers.Ints(7, 11, 14));
        var sel = MatrixOps.ModalSelection(cMajor, crit, 0);
        var mrmd = MatrixDistanceOps.CalculateDistances(gMajorChord, MatrixOps.ModalRelativeMode(sel), Distances.ManhattanDistance, true);
        var best = mrmd.GetClosest();
        Assert.Equal(0, best.ModeIndex);
        Assert.Equal(2, best.RelativeModeIndex);
        Assert.Equal(3.0, best.Distance, 6);
    }

    [Fact]
    public void Calculate_distances_modal_selection_closest()
    {
        var cMajor = new PositionVector(TestHelpers.Ints(0, 2, 4, 5, 7, 9, 11));
        var crit = new IntervalVector(TestHelpers.Ints(2, 2, 3), 0, 12);
        var gMajorChord = new PositionVector(TestHelpers.Ints(7, 11, 14));
        var sel = MatrixOps.ModalSelection(cMajor, crit, 0);
        var msd = MatrixDistanceOps.CalculateDistances(gMajorChord, sel, Distances.ManhattanDistance, true);
        var best = msd.GetClosest();
        Assert.Equal(3, best.ModeIndex);
        Assert.Equal(18.0, best.Distance, 6);
        TestHelpers.AssertPvEq(best.Chord, TestHelpers.Ints(0, 5, 9).ToArray(), 12);
    }
}
