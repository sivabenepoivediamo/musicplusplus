using System.Globalization;
using System.Text;

namespace Musicpp;

public static class MatrixPrint
{
    private static string VecToString(PositionVector v) =>
        "[" + string.Join(", ", v.MutableData) + "]";

    private static string VecToString(IntervalVector v) =>
        "[" + string.Join(", ", v.MutableData) + "]";

    public static void PrintMatrixDistance(ModalMatrixDistance<PositionVector> mmd, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        outWriter.WriteLine($"{"Row",-6}{"Mode",-20}{"Vector",-30}Distance");
        outWriter.WriteLine(new string('-', 80));
        for (var i = 0; i < mmd.Count; i++)
        {
            var t = mmd[i];
            outWriter.WriteLine($"{i,-6}{t.Index,-20}{VecToString(t.Vector),-30}{t.Distance.ToString(CultureInfo.InvariantCulture)}");
        }
    }

    public static void PrintMatrixDistance(ModalMatrixDistance<IntervalVector> mmd, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        outWriter.WriteLine($"{"Row",-6}{"Mode",-20}{"Vector",-30}Distance");
        outWriter.WriteLine(new string('-', 80));
        for (var i = 0; i < mmd.Count; i++)
        {
            var t = mmd[i];
            outWriter.WriteLine($"{i,-6}{t.Index,-20}{VecToString(t.Vector),-30}{t.Distance.ToString(CultureInfo.InvariantCulture)}");
        }
    }

    public static void PrintMatrixDistance(TranspositionMatrixDistance tmd, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        outWriter.WriteLine($"{"Row",-6}{"Transposition",-16}{"Vector",-30}Distance");
        outWriter.WriteLine(new string('-', 80));
        for (var i = 0; i < tmd.Count; i++)
        {
            var t = tmd[i];
            outWriter.WriteLine($"{i,-6}{t.Transposition,-16}{VecToString(t.Vector),-30}{t.Distance.ToString(CultureInfo.InvariantCulture)}");
        }
    }

    public static void PrintMatrixDistance(RelativeModeMatrixDistance rmd, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        outWriter.WriteLine($"{"Row",-6}{"Rel.mode",-16}{"Vector",-30}{"Distance",-10}Center");
        outWriter.WriteLine(new string('-', 80));
        for (var i = 0; i < rmd.Count; i++)
        {
            var t = rmd[i];
            outWriter.WriteLine($"{i,-6}{t.Offset,-16}{VecToString(t.Vector),-30}{t.Distance,-10}{rmd.Center}");
        }
    }

    public static void PrintMatrixDistance(ModalSelectionMatrixDistance<IntervalVector> mmd, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        outWriter.WriteLine($"{"Row",-6}{"Degree",-16}{"Chord",-30}Distance");
        outWriter.WriteLine(new string('-', 80));
        for (var i = 0; i < mmd.Count; i++)
        {
            var t = mmd[i];
            outWriter.WriteLine($"{i,-6}{t.ModeIndex,-16}{VecToString(t.Chord),-30}{t.Distance.ToString(CultureInfo.InvariantCulture)}");
        }
    }

    public static void PrintMatrixDistance(ModalSelectionMatrixDistance<PositionVector> mmd, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        outWriter.WriteLine($"{"Row",-6}{"Degree",-16}{"Chord",-30}Distance");
        outWriter.WriteLine(new string('-', 80));
        for (var i = 0; i < mmd.Count; i++)
        {
            var t = mmd[i];
            outWriter.WriteLine($"{i,-6}{t.ModeIndex,-16}{VecToString(t.Chord),-30}{t.Distance.ToString(CultureInfo.InvariantCulture)}");
        }
    }

    public static void PrintMatrixDistance(ModalRelativeModeMatrixDistance mrmd, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        outWriter.WriteLine($"{"Row",-6}{"Degree",-10}{"Rel.mode",-12}{"Vector",-30}Distance");
        outWriter.WriteLine(new string('-', 80));
        for (var i = 0; i < mrmd.Count; i++)
        {
            var t = mrmd[i];
            outWriter.WriteLine($"{i,-6}{t.ModeIndex,-10}{t.RelativeModeIndex,-12}{VecToString(t.Vector),-30}{t.Distance.ToString(CultureInfo.InvariantCulture)}");
        }
    }

    public static string TupleToString(PositionVector vec, int idx, double dist) =>
        $"idx={idx} vec={VecToString(vec)} dist={dist.ToString(CultureInfo.InvariantCulture)}";

    public static string TupleToString(IntervalVector vec, int idx, double dist) =>
        $"idx={idx} vec={VecToString(vec)} dist={dist.ToString(CultureInfo.InvariantCulture)}";

    public static void PrintTupleTIntDouble((IntervalVector vec, int idx, double dist) t, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        outWriter.WriteLine(TupleToString(t.vec, t.idx, t.dist));
    }

    public static string TupleIntIntPvDoubleToString((int mode, int relMode, PositionVector vec, double dist) e) =>
        $"mode={e.mode} rel_mode={e.relMode} vec={VecToString(e.vec)} dist={e.dist.ToString(CultureInfo.InvariantCulture)}";

    public static void PrintTupleIntIntPvDouble((int mode, int relMode, PositionVector vec, double dist) e, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        outWriter.WriteLine(TupleIntIntPvDoubleToString(e));
    }

    public static void PrintMatrixRow(ModalMatrixDistance<PositionVector> mmd, int row, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        if (row < 0 || row >= mmd.Count)
        {
            outWriter.WriteLine("row out of range");
            return;
        }
        var t = mmd[row];
        outWriter.WriteLine(TupleToString(t.Vector, t.Index, t.Distance));
    }

    public static void PrintMatrixRow(ModalMatrixDistance<IntervalVector> mmd, int row, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        if (row < 0 || row >= mmd.Count)
        {
            outWriter.WriteLine("row out of range");
            return;
        }
        var t = mmd[row];
        outWriter.WriteLine(TupleToString(t.Vector, t.Index, t.Distance));
    }

    public static void PrintSelectionRow(ModalSelectionMatrixDistance<PositionVector> mmd, int row, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        if (row < 0 || row >= mmd.Count)
        {
            outWriter.WriteLine("row out of range");
            return;
        }
        var t = mmd[row];
        outWriter.WriteLine(TupleToString(t.Chord, t.ModeIndex, t.Distance));
    }

    public static void PrintSelectionRow(ModalSelectionMatrixDistance<IntervalVector> mmd, int row, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        if (row < 0 || row >= mmd.Count)
        {
            outWriter.WriteLine("row out of range");
            return;
        }
        var t = mmd[row];
        outWriter.WriteLine(TupleToString(t.Chord, t.ModeIndex, t.Distance));
    }

    public static void PrintTranspositionRow(TranspositionMatrixDistance tmd, int row, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        if (row < 0 || row >= tmd.Count)
        {
            outWriter.WriteLine("row out of range");
            return;
        }
        var t = tmd[row];
        outWriter.WriteLine(TupleToString(t.Vector, t.Transposition, t.Distance));
    }

    public static void PrintRelativeModeRow(RelativeModeMatrixDistance rmd, int row, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        if (row < 0 || row >= rmd.Count)
        {
            outWriter.WriteLine("row out of range");
            return;
        }
        var t = rmd[row];
        outWriter.WriteLine(TupleToString(t.Vector, t.Offset, t.Distance) + $" center={rmd.Center}");
    }

    public static void PrintModalRelativeModeRow(ModalRelativeModeMatrixDistance mrmd, int row, TextWriter? outWriter = null)
    {
        outWriter ??= Console.Out;
        if (row < 0 || row >= mrmd.Count)
        {
            outWriter.WriteLine("row out of range");
            return;
        }
        var t = mrmd[row];
        PrintTupleIntIntPvDouble((t.ModeIndex, t.RelativeModeIndex, t.Vector, t.Distance), outWriter);
    }
}
