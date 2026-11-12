namespace Musicpp;

public sealed class ModalMatrix<T> where T : class, ICopyable<T>, IEquatable<T>
{
    private readonly List<(T Vector, int Index)> _data;

    public ModalMatrix() => _data = new List<(T, int)>();
    public ModalMatrix(IEnumerable<(T Vector, int Index)> data) => _data = data.ToList();

    public int Count => _data.Count;
    public bool IsEmpty => _data.Count == 0;
    public (T Vector, int Index) this[int i] => _data[i];
    public IReadOnlyList<(T Vector, int Index)> Data => _data;

    public List<T> GetVectors() => _data.Select(x => x.Vector).ToList();
    public List<int> GetIndices() => _data.Select(x => x.Index).ToList();
}

public sealed class TranspositionMatrix
{
    private readonly List<(PositionVector Vector, int Transposition)> _data;
    public TranspositionMatrix() => _data = new List<(PositionVector, int)>();
    public TranspositionMatrix(IEnumerable<(PositionVector Vector, int Transposition)> data) => _data = data.ToList();
    public int Count => _data.Count;
    public bool IsEmpty => _data.Count == 0;
    public (PositionVector Vector, int Transposition) this[int i] => _data[i];
    public IReadOnlyList<(PositionVector Vector, int Transposition)> Data => _data;
    public List<PositionVector> GetVectors() => _data.Select(x => x.Vector).ToList();
    public List<int> GetTranspositions() => _data.Select(x => x.Transposition).ToList();
}

public sealed class RelativeModeMatrix
{
    private readonly List<(PositionVector Vector, int Offset)> _data;
    private readonly int _center;
    public RelativeModeMatrix(IEnumerable<(PositionVector Vector, int Offset)> data, int center = 0)
    {
        _data = data.ToList();
        _center = center;
    }
    public int Count => _data.Count;
    public int Center => _center;
    public (PositionVector Vector, int Offset) this[int i] => _data[i];
    public IReadOnlyList<(PositionVector Vector, int Offset)> Data => _data;
    public List<PositionVector> GetVectors() => _data.Select(x => x.Vector).ToList();
    public List<int> GetRelativeModeOffsets() => _data.Select(x => x.Offset).ToList();
}

public sealed class ModalSelectionMatrix<T> where T : class, ICopyable<T>, IEquatable<T>
{
    private readonly List<(T Chord, int ModeIndex)> _data;
    public ModalSelectionMatrix(IEnumerable<(T Chord, int ModeIndex)> data) => _data = data.ToList();
    public int Count => _data.Count;
    public (T Chord, int ModeIndex) this[int i] => _data[i];
    public IReadOnlyList<(T Chord, int ModeIndex)> Data => _data;
    public List<T> GetChords() => _data.Select(x => x.Chord).ToList();
    public List<int> GetModeIndices() => _data.Select(x => x.ModeIndex).ToList();
}

public sealed class ModalRelativeModeMatrix<T> where T : class
{
    private readonly List<(RelativeModeMatrix Matrix, int ModeIndex)> _data;
    public ModalRelativeModeMatrix(IEnumerable<(RelativeModeMatrix Matrix, int ModeIndex)> data) => _data = data.ToList();
    public int Count => _data.Count;
    public (RelativeModeMatrix Matrix, int ModeIndex) this[int i] => _data[i];
    public int GetTotalVectorCount() => _data.Sum(x => x.Matrix.Count);
}

public static class MatrixOps
{
    public static ModalMatrix<IntervalVector> ModalMatrix(IntervalVector iv)
    {
        var n = iv.Count;
        var matrix = new List<(IntervalVector, int)>();
        for (var i = 0; i < n; i++)
            matrix.Add((iv.ParallelMode(i), i));
        return new ModalMatrix<IntervalVector>(matrix);
    }

    public static RelativeModeMatrix RelativeModeMatrix(PositionVector input, int center)
    {
        var matrix = new List<(PositionVector, int)>();
        var n = (int)input.Count;
        for (var i = center - n; i < center + n + 1; i++)
            matrix.Add((input.RelativeMode(i), i));
        return new RelativeModeMatrix(matrix, center);
    }

    public static ModalMatrix<PositionVector> ModalMatrix(PositionVector pv)
    {
        var iv = MusicVectors.PositionsToIntervals(pv);
        var ivMatrix = ModalMatrix(iv);
        var pvMatrix = new List<(PositionVector, int)>();
        for (var i = 0; i < ivMatrix.Count; i++)
            pvMatrix.Add((MusicVectors.IntervalsToPositions(ivMatrix[i].Vector), ivMatrix[i].Index));
        return new ModalMatrix<PositionVector>(pvMatrix);
    }

    public static TranspositionMatrix TranspositionMatrix(PositionVector pv)
    {
        var n = pv.Mod;
        var matrix = new List<(PositionVector, int)>();
        for (var i = 0; i < n; i++)
        {
            var transposed = (pv + i) % n;
            var sorted = transposed.MutableData.OrderBy(x => x).ToList();
            var sortedPv = new PositionVector(sorted, transposed.Mod, transposed.UserRange, transposed.AutoRangeUpdates, transposed.UseUserRange);
            if (!sortedPv.AutoRangeUpdates)
                sortedPv.SetEffectiveRange(transposed.EffectiveRange);
            matrix.Add((sortedPv, i));
        }
        return new TranspositionMatrix(matrix);
    }

    public static ModalSelectionMatrix<IntervalVector> ModalSelection(IntervalVector source, IntervalVector criterion, int degree = 0)
    {
        var modes = ModalMatrix(criterion);
        var selection = new List<(IntervalVector, int)>();
        for (var i = 0; i < modes.Count; i++)
        {
            var candidate = ChordFunctions.Chord(source, modes[i].Vector, degree);
            var sum = 0;
            for (var k = 0; k < i; k++)
                sum += criterion.MutableData[k];
            var g = MathUtil.EuclideanDivision(degree - sum, source.Count).Remainder;
            selection.Add((candidate, g));
        }
        return new ModalSelectionMatrix<IntervalVector>(selection);
    }

    public static ModalSelectionMatrix<PositionVector> ModalSelection(PositionVector source, IntervalVector criterion, int degree = 0)
    {
        var modes = ModalMatrix(criterion);
        var ivSource = MusicVectors.PositionsToIntervals(source);
        var selection = new List<(PositionVector, int)>();
        for (var i = 0; i < modes.Count; i++)
        {
            var candidate = ChordFunctions.Chord(ivSource, modes[i].Vector, degree);
            var pc = MusicVectors.IntervalsToPositions(candidate);
            var sum = 0;
            for (var k = 0; k < i; k++)
                sum += criterion.MutableData[k];
            var g = MathUtil.EuclideanDivision(degree - sum, source.Count).Remainder;
            selection.Add((pc, g));
        }
        return new ModalSelectionMatrix<PositionVector>(selection);
    }

    public static ModalRelativeModeMatrix<PositionVector> ModalRelativeMode(ModalSelectionMatrix<PositionVector> selection)
    {
        var result = new List<(RelativeModeMatrix, int)>();
        for (var i = 0; i < selection.Count; i++)
        {
            var chord = selection[i].Chord.Copy();
            var rtm = RelativeModeMatrix(chord, 0);
            result.Add((rtm, selection[i].ModeIndex));
        }
        return new ModalRelativeModeMatrix<PositionVector>(result);
    }

    public static ModalMatrix<PositionVector> FilterModalMatrix(ModalMatrix<PositionVector> matrix, IReadOnlyList<int> notes)
    {
        if (notes.Count == 0)
            return new ModalMatrix<PositionVector>(matrix.Data.Select(x => (x.Vector.Copy(), x.Index)));
        var filtered = new List<(PositionVector, int)>();
        for (var i = 0; i < matrix.Count; i++)
        {
            var pv = matrix[i].Vector;
            var modeIdx = matrix[i].Index;
            var mod = pv.Mod;
            var containsAll = true;
            foreach (var note in notes)
            {
                var noteMod = ((note % mod) + mod) % mod;
                var found = false;
                foreach (var pos in pv.MutableData)
                {
                    if (((pos % mod) + mod) % mod == noteMod)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    containsAll = false;
                    break;
                }
            }
            if (containsAll)
                filtered.Add((pv.Copy(), modeIdx));
        }
        return new ModalMatrix<PositionVector>(filtered);
    }

    public static TranspositionMatrix FilterTranspositionMatrix(TranspositionMatrix matrix, IReadOnlyList<int> notes)
    {
        if (notes.Count == 0)
            return new TranspositionMatrix(matrix.Data.Select(x => (x.Vector.Copy(), x.Transposition)));
        var filtered = new List<(PositionVector, int)>();
        for (var i = 0; i < matrix.Count; i++)
        {
            var pv = matrix[i].Vector;
            var transIdx = matrix[i].Transposition;
            var mod = pv.Mod;
            var containsAll = true;
            foreach (var note in notes)
            {
                var noteMod = ((note % mod) + mod) % mod;
                var found = false;
                foreach (var pos in pv.MutableData)
                {
                    if (((pos % mod) + mod) % mod == noteMod)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    containsAll = false;
                    break;
                }
            }
            if (containsAll)
                filtered.Add((pv.Copy(), transIdx));
        }
        return new TranspositionMatrix(filtered);
    }
}
