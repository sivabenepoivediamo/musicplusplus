using Musicpp;

namespace Musicplusplus.Tests;

public class MatrixRowTests
{
    [Fact]
    public void Modal_matrix_distance_empty_throws()
    {
        var empty = new ModalMatrixDistance<PositionVector>();
        Assert.Throws<InvalidOperationException>(() => empty.GetClosest());
        Assert.Throws<InvalidOperationException>(() => empty.GetFurthest());
        Assert.Throws<InvalidOperationException>(() => empty.GetByComplexity(0));
    }

    [Fact]
    public void Modal_matrix_distance_sort_and_extractors()
    {
        var a = new PositionVector(TestHelpers.Ints(0, 4, 7));
        var b = new PositionVector(TestHelpers.Ints(0, 3, 7));
        var rows = new List<(PositionVector Vector, int Index, double Distance)>
        {
            (a, 0, 2.5),
            (b, 1, 0.5),
        };
        var mmd = new ModalMatrixDistance<PositionVector>(rows);
        mmd.SortByDistance();
        Assert.Equal(0.5, mmd[0].Distance, 9);
        var vecs = mmd.GetVectors();
        Assert.Equal(2, vecs.Count);
        TestHelpers.AssertPvEq(vecs[0], TestHelpers.Ints(0, 3, 7).ToArray());
        Assert.Equal(1, mmd.GetIndices()[0]);
        Assert.Equal(2.5, mmd.GetDistances()[1], 9);
        var closest = mmd.GetClosest();
        Assert.Equal(0.5, closest.Distance, 9);
    }

    [Fact]
    public void Modal_matrix_row_accessors()
    {
        var v = new PositionVector(TestHelpers.Ints(1, 2, 3));
        var row = new ModalMatrixRow<PositionVector>(v, 4, 1.25);
        Assert.Equal(4, row.Index);
        Assert.Equal(1.25, row.Distance, 9);
        TestHelpers.AssertPvEq(row.Vector, TestHelpers.Ints(1, 2, 3).ToArray());
        var tup = row.ToTuple();
        Assert.Equal(1.25, tup.Distance, 9);
        var s = row.ToString();
        Assert.Contains("Mode = 4", s);
    }

    [Fact]
    public void Transposition_matrix_row_accessors()
    {
        var v = new PositionVector(TestHelpers.Ints(0, 4, 7));
        var row = new TranspositionMatrixRow(v, 5, 0.0);
        Assert.Equal(5, row.Transposition);
        TestHelpers.AssertPvEq(row.Vector, TestHelpers.Ints(0, 4, 7).ToArray(), 12);
    }

    [Fact]
    public void Relative_mode_matrix_row_accessors()
    {
        var v = new PositionVector(TestHelpers.Ints(0, 4, 7));
        var row = new RelativeModeMatrixRow(v, 3, 1.5, -1);
        Assert.Equal(3, row.RelativeModeOffset);
        Assert.Equal(-1, row.Center);
        Assert.Equal(1.5, row.Distance, 9);
    }

    [Fact]
    public void Modal_selection_matrix_row_accessors()
    {
        var c = new PositionVector(TestHelpers.Ints(0, 4, 7));
        var row = new ModalSelectionMatrixRow<PositionVector>(c, 2, 4.0);
        Assert.Equal(2, row.ModeIndex);
        Assert.Equal(4.0, row.Distance, 9);
    }

    [Fact]
    public void Transposition_matrix_distance_complexity_bounds()
    {
        var tmd = new TranspositionMatrixDistance();
        Assert.Throws<InvalidOperationException>(() => tmd.GetByComplexity(-1));
        Assert.Throws<InvalidOperationException>(() => tmd.GetByComplexity(101));
    }

    [Fact]
    public void Modal_relative_mode_matrix_distance_empty_throws()
    {
        var empty = new ModalRelativeModeMatrixDistance();
        Assert.Throws<InvalidOperationException>(() => empty.GetClosest());
        Assert.Throws<InvalidOperationException>(() => empty.GetFurthest());
        Assert.Throws<InvalidOperationException>(() => empty.GetByComplexity(0));
    }

    [Fact]
    public void Modal_relative_mode_matrix_distance_complexity_bounds()
    {
        var v = new PositionVector(TestHelpers.Ints(0, 4, 7), 12, 12);
        var mrd = new ModalRelativeModeMatrixDistance(new[] { (0, 0, v, 1.0) });
        Assert.Throws<ArgumentOutOfRangeException>(() => mrd.GetByComplexity(-1));
        Assert.Throws<ArgumentOutOfRangeException>(() => mrd.GetByComplexity(101));
    }

    [Fact]
    public void Modal_relative_mode_matrix_distance_sort_and_closest()
    {
        var a = new PositionVector(TestHelpers.Ints(0, 4, 7), 12, 12);
        var b = new PositionVector(TestHelpers.Ints(1, 5, 8), 12, 12);
        var rows = new List<(int, int, PositionVector, double)>
        {
            (0, 0, a, 3.0),
            (1, 2, b, 0.5),
        };
        var mrd = new ModalRelativeModeMatrixDistance(rows);
        mrd.SortByDistance();
        Assert.Equal(0.5, mrd[0].Distance, 9);
        var closest = mrd.GetClosest();
        Assert.Equal(0.5, closest.Distance, 9);
        Assert.Equal(1, closest.ModeIndex);
        Assert.Equal(2, closest.RelativeModeIndex);
    }

    [Fact]
    public void Modal_relative_mode_matrix_row_accessors()
    {
        var v = new PositionVector(TestHelpers.Ints(0, 4, 7), 12, 12);
        var row = new ModalRelativeModeMatrixRow(5, -1, v, 2.25);
        Assert.Equal(5, row.ModeIndex);
        Assert.Equal(-1, row.RelativeModeIndex);
        Assert.Equal(2.25, row.Distance, 9);
        TestHelpers.AssertPvEq(row.Vector, TestHelpers.Ints(0, 4, 7).ToArray(), 12);
        Assert.Contains("Degree = 5", row.ToString());
    }
}
