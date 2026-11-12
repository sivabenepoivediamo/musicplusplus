using System.IO;
using Musicpp;

namespace Musicplusplus.Tests;

public class MatrixPrintTests
{
    [Fact]
    public void Modal_matrix_row_toString_wrapped_in_parentheses()
    {
        var a = new PositionVector(TestHelpers.Ints(0, 4, 7), 12, 12);
        var row = new ModalMatrixRow<PositionVector>(a, 2, 1.25);
        var s = row.ToString();
        Assert.True(s.Length >= 2);
        Assert.Equal('(', s[0]);
        Assert.Equal(')', s[^1]);
        Assert.Contains("Mode = ", s);
    }

    [Fact]
    public void Print_matrix_distance_modal_position_smoke()
    {
        var a = new PositionVector(TestHelpers.Ints(0, 4, 7), 12, 12);
        var mmd = new ModalMatrixDistance<PositionVector>(new[] { (a, 0, 1.5) });
        var sw = new StringWriter();
        MatrixPrint.PrintMatrixDistance(mmd, sw);
        var s = sw.ToString();
        Assert.Contains("Row", s);
        Assert.Contains("Distance", s);
        Assert.Contains("1.5", s);
    }

    [Fact]
    public void Print_matrix_distance_transposition_smoke()
    {
        var v = new PositionVector(TestHelpers.Ints(0, 4, 7), 12, 12);
        var tmd = new TranspositionMatrixDistance(new[] { (v, 3, 0.0) });
        var sw = new StringWriter();
        MatrixPrint.PrintMatrixDistance(tmd, sw);
        Assert.Contains("Transposition", sw.ToString());
    }

    [Fact]
    public void Print_matrix_distance_relative_mode_smoke()
    {
        var v = new PositionVector(TestHelpers.Ints(0, 4, 7), 12, 12);
        var rmd = new RelativeModeMatrixDistance(new[] { (v, 0, 2.0) }, 5);
        var sw = new StringWriter();
        MatrixPrint.PrintMatrixDistance(rmd, sw);
        var s = sw.ToString();
        Assert.Contains("Center", s);
        Assert.Contains("5", s);
    }

    [Fact]
    public void Print_matrix_distance_modal_relative_mode_smoke()
    {
        var v = new PositionVector(TestHelpers.Ints(0, 4, 7), 12, 12);
        var mrmd = new ModalRelativeModeMatrixDistance(new[] { (1, -2, v, 4.5) });
        var sw = new StringWriter();
        MatrixPrint.PrintMatrixDistance(mrmd, sw);
        Assert.Contains("Degree", sw.ToString());
    }

    [Fact]
    public void Print_matrix_distance_modal_selection_interval_smoke()
    {
        var iv = new IntervalVector(TestHelpers.Ints(2, 2, 3), 0, 12);
        var msd = new ModalSelectionMatrixDistance<IntervalVector>(new[] { (iv, 2, 1.0) });
        var sw = new StringWriter();
        MatrixPrint.PrintMatrixDistance(msd, sw);
        Assert.Contains("Chord", sw.ToString());
    }

    [Fact]
    public void Print_matrix_row_modal_in_and_out_of_range()
    {
        var a = new PositionVector(TestHelpers.Ints(0, 4, 7), 12, 12);
        var mmd = new ModalMatrixDistance<PositionVector>(new[] { (a, 1, 2.0) });
        var ok = new StringWriter();
        MatrixPrint.PrintMatrixRow(mmd, 0, ok);
        Assert.Contains("idx=", ok.ToString());
        var bad = new StringWriter();
        MatrixPrint.PrintMatrixRow(mmd, 99, bad);
        Assert.Contains("out of range", bad.ToString());
    }

    [Fact]
    public void Print_transposition_row_out_of_range()
    {
        var v = new PositionVector(TestHelpers.Ints(0, 4, 7), 12, 12);
        var tmd = new TranspositionMatrixDistance(new[] { (v, 0, 0.0) });
        var sw = new StringWriter();
        MatrixPrint.PrintTranspositionRow(tmd, 10, sw);
        Assert.Contains("out of range", sw.ToString());
    }

    [Fact]
    public void Print_tuple_T_int_double_interval()
    {
        var iv = new IntervalVector(TestHelpers.Ints(2, 2, 3), 0, 12);
        var sw = new StringWriter();
        MatrixPrint.PrintTupleTIntDouble((iv, 2, 3.5), sw);
        Assert.Contains("dist=3.5", sw.ToString());
    }

    [Fact]
    public void Tuple_T_int_double_to_string_position()
    {
        var v = new PositionVector(TestHelpers.Ints(1, 2), 12, 12);
        var s = MatrixPrint.TupleToString(v, 7, 0.25);
        Assert.Contains("idx=7", s);
        Assert.Contains("dist=0.25", s);
    }

    [Fact]
    public void Print_relative_mode_row_ok_and_out_of_range()
    {
        var v = new PositionVector(TestHelpers.Ints(0, 4, 7), 12, 12);
        var rmd = new RelativeModeMatrixDistance(new[] { (v, 3, 1.0) }, 0);
        var ok = new StringWriter();
        MatrixPrint.PrintRelativeModeRow(rmd, 0, ok);
        Assert.Contains("idx=3", ok.ToString());
        var bad = new StringWriter();
        MatrixPrint.PrintRelativeModeRow(rmd, 3, bad);
        Assert.Contains("out of range", bad.ToString());
    }

    [Fact]
    public void Print_selection_row_position_ok_and_out_of_range()
    {
        var c = new PositionVector(TestHelpers.Ints(0, 4, 7), 12, 12);
        var msd = new ModalSelectionMatrixDistance<PositionVector>(new[] { (c, 4, 0.5) });
        var ok = new StringWriter();
        MatrixPrint.PrintSelectionRow(msd, 0, ok);
        Assert.Contains("idx=4", ok.ToString());
        var bad = new StringWriter();
        MatrixPrint.PrintSelectionRow(msd, 5, bad);
        Assert.Contains("out of range", bad.ToString());
    }

    [Fact]
    public void Print_modal_relative_mode_row_ok_and_out_of_range()
    {
        var v = new PositionVector(TestHelpers.Ints(0, 4, 7), 12, 12);
        var mrmd = new ModalRelativeModeMatrixDistance(new[] { (2, 1, v, 8.0) });
        var ok = new StringWriter();
        MatrixPrint.PrintModalRelativeModeRow(mrmd, 0, ok);
        var okStr = ok.ToString();
        Assert.Contains("mode=2", okStr);
        Assert.Contains("rel_mode=1", okStr);
        var bad = new StringWriter();
        MatrixPrint.PrintModalRelativeModeRow(mrmd, 9, bad);
        Assert.Contains("out of range", bad.ToString());
    }

    [Fact]
    public void Tuple_int_int_PV_double_string_and_print()
    {
        var v = new PositionVector(TestHelpers.Ints(0, 4, 7), 12, 12);
        var s = MatrixPrint.TupleIntIntPvDoubleToString((1, -3, v, 0.125));
        Assert.Contains("mode=1", s);
        Assert.Contains("rel_mode=-3", s);
        Assert.Contains("dist=0.125", s);
        var sw = new StringWriter();
        MatrixPrint.PrintTupleIntIntPvDouble((1, -3, v, 0.125), sw);
        Assert.Contains("dist=0.125", sw.ToString());
    }
}
