namespace Musicpp;

public delegate int DistanceFuncPv(PositionVector a, PositionVector b);
public delegate int DistanceFuncIv(IntervalVector a, IntervalVector b);

public sealed class ModalMatrixRow<T> where T : class
{
    public T Vector { get; }
    public int Index { get; }
    public double Distance { get; }

    public ModalMatrixRow(T vec, int idx, double dist)
    {
        Vector = vec;
        Index = idx;
        Distance = dist;
    }

    public ModalMatrixRow((T Vector, int Index, double Distance) t) : this(t.Vector, t.Index, t.Distance) { }

    public (T Vector, int Index, double Distance) ToTuple() => (Vector, Index, Distance);

    public override string ToString() => $"(Mode = {Index}, Distance = {Distance}, Vector = {Vector})";
}

public sealed class TranspositionMatrixRow
{
    public PositionVector Vector { get; }
    public int Transposition { get; }
    public double Distance { get; }

    public TranspositionMatrixRow(PositionVector vec, int trans, double dist)
    {
        Vector = vec;
        Transposition = trans;
        Distance = dist;
    }

    public TranspositionMatrixRow((PositionVector Vector, int Transposition, double Distance) t)
        : this(t.Vector, t.Transposition, t.Distance) { }

    public override string ToString() => $"(Transposition = {Transposition}, Distance = {Distance}, Vector = {Vector})";
}

public sealed class RelativeModeMatrixRow
{
    public PositionVector Vector { get; }
    public int RelativeModeOffset { get; }
    public double Distance { get; }
    public int Center { get; }

    public RelativeModeMatrixRow(PositionVector vec, int offset, double dist, int center)
    {
        Vector = vec;
        RelativeModeOffset = offset;
        Distance = dist;
        Center = center;
    }

    public RelativeModeMatrixRow((PositionVector Vector, int Offset, double Distance) t, int center)
        : this(t.Vector, t.Offset, t.Distance, center) { }

    public override string ToString() =>
        $"(Relative mode offset = {RelativeModeOffset}, Center = {Center}, Distance = {Distance}, Vector = {Vector})";
}

public sealed class ModalSelectionMatrixRow<T> where T : class
{
    public T Chord { get; }
    public int ModeIndex { get; }
    public double Distance { get; }

    public ModalSelectionMatrixRow(T chord, int mode, double dist)
    {
        Chord = chord;
        ModeIndex = mode;
        Distance = dist;
    }

    public ModalSelectionMatrixRow((T Chord, int ModeIndex, double Distance) t)
        : this(t.Chord, t.ModeIndex, t.Distance) { }

    public override string ToString() => $"(Degree = {ModeIndex}, Distance = {Distance}, Vector = {Chord})";
}

public sealed class ModalRelativeModeMatrixRow
{
    public int ModeIndex { get; }
    public int RelativeModeIndex { get; }
    public PositionVector Vector { get; }
    public double Distance { get; }

    public ModalRelativeModeMatrixRow(int mode, int rel, PositionVector vec, double dist)
    {
        ModeIndex = mode;
        RelativeModeIndex = rel;
        Vector = vec;
        Distance = dist;
    }

    public ModalRelativeModeMatrixRow((int ModeIndex, int RelativeModeIndex, PositionVector Vector, double Distance) t)
        : this(t.ModeIndex, t.RelativeModeIndex, t.Vector, t.Distance) { }

    public override string ToString() =>
        $"(Degree = {ModeIndex}, Relative mode = {RelativeModeIndex}, Distance = {Distance}, Vector = {Vector})";
}

public sealed class ModalMatrixDistance<T> where T : class
{
    private readonly List<(T Vector, int Index, double Distance)> _data;
    public ModalMatrixDistance() => _data = new List<(T, int, double)>();
    public ModalMatrixDistance(IEnumerable<(T Vector, int Index, double Distance)> data) => _data = data.ToList();

    public int Count => _data.Count;
    public (T Vector, int Index, double Distance) this[int i] => _data[i];
    public IReadOnlyList<(T Vector, int Index, double Distance)> Data => _data;

    public void SortByDistance() =>
        _data.Sort((a, b) => a.Distance.CompareTo(b.Distance));

    public List<T> GetVectors() => _data.Select(x => x.Vector).ToList();
    public List<int> GetIndices() => _data.Select(x => x.Index).ToList();
    public List<double> GetDistances() => _data.Select(x => x.Distance).ToList();

    public (T Vector, int Index, double Distance) GetClosest() =>
        _data.Count == 0 ? throw new InvalidOperationException("empty") : _data.MinBy(x => x.Distance);

    public (T Vector, int Index, double Distance) GetFurthest() =>
        _data.Count == 0 ? throw new InvalidOperationException("empty") : _data.MaxBy(x => x.Distance);

    public ModalMatrixRow<T> GetByComplexity(int complexity = 0)
    {
        if (_data.Count == 0)
            throw new InvalidOperationException("empty");
        if (complexity < 0 || complexity > 100)
            throw new ArgumentOutOfRangeException(nameof(complexity));
        var index = (int)((complexity / 100.0) * (_data.Count - 1));
        return new ModalMatrixRow<T>(_data[index]);
    }
}

public sealed class TranspositionMatrixDistance
{
    private readonly List<(PositionVector Vector, int Transposition, double Distance)> _data;
    public TranspositionMatrixDistance() => _data = new List<(PositionVector, int, double)>();
    public TranspositionMatrixDistance(IEnumerable<(PositionVector, int, double)> data) => _data = data.ToList();
    public int Count => _data.Count;
    public (PositionVector Vector, int Transposition, double Distance) this[int i] => _data[i];
    public void SortByDistance() => _data.Sort((a, b) => a.Distance.CompareTo(b.Distance));
    public List<PositionVector> GetVectors() => _data.Select(x => x.Vector).ToList();
    public List<int> GetTranspositions() => _data.Select(x => x.Transposition).ToList();
    public List<double> GetDistances() => _data.Select(x => x.Distance).ToList();

    public TranspositionMatrixRow GetByComplexity(int complexity = 0)
    {
        if (_data.Count == 0)
            throw new InvalidOperationException("empty");
        if (complexity < 0 || complexity > 100)
            throw new ArgumentOutOfRangeException(nameof(complexity));
        var index = (int)((complexity / 100.0) * (_data.Count - 1));
        return new TranspositionMatrixRow(_data[index]);
    }

    public TranspositionMatrixRow GetClosest() => new TranspositionMatrixRow(_data.MinBy(x => x.Distance));
    public TranspositionMatrixRow GetFurthest() => new TranspositionMatrixRow(_data.MaxBy(x => x.Distance));
}

public sealed class RelativeModeMatrixDistance
{
    private readonly List<(PositionVector Vector, int Offset, double Distance)> _data;
    private readonly int _center;
    public RelativeModeMatrixDistance(IEnumerable<(PositionVector, int, double)> data, int center = 0)
    {
        _data = data.ToList();
        _center = center;
    }
    public int Count => _data.Count;
    public int Center => _center;
    public (PositionVector Vector, int Offset, double Distance) this[int i] => _data[i];
    public void SortByDistance() => _data.Sort((a, b) => a.Distance.CompareTo(b.Distance));
    public List<PositionVector> GetVectors() => _data.Select(x => x.Vector).ToList();
    public List<int> GetRelativeModeOffsets() => _data.Select(x => x.Offset).ToList();
    public List<double> GetDistances() => _data.Select(x => x.Distance).ToList();

    public RelativeModeMatrixRow GetByComplexity(int complexity = 0)
    {
        if (_data.Count == 0)
            throw new InvalidOperationException("empty");
        var index = (int)((complexity / 100.0) * (_data.Count - 1));
        return new RelativeModeMatrixRow(_data[index], _center);
    }

    public RelativeModeMatrixRow GetClosest() => new RelativeModeMatrixRow(_data.MinBy(x => x.Distance), _center);
    public RelativeModeMatrixRow GetFurthest() => new RelativeModeMatrixRow(_data.MaxBy(x => x.Distance), _center);
}

public sealed class ModalSelectionMatrixDistance<T> where T : class
{
    private readonly List<(T Chord, int ModeIndex, double Distance)> _data;
    public ModalSelectionMatrixDistance(IEnumerable<(T, int, double)> data) => _data = data.ToList();
    public int Count => _data.Count;
    public (T Chord, int ModeIndex, double Distance) this[int i] => _data[i];
    public void SortByDistance() => _data.Sort((a, b) => a.Distance.CompareTo(b.Distance));
    public List<T> GetChords() => _data.Select(x => x.Chord).ToList();
    public List<int> GetModeIndices() => _data.Select(x => x.ModeIndex).ToList();
    public List<double> GetDistances() => _data.Select(x => x.Distance).ToList();

    public ModalSelectionMatrixRow<T> GetByComplexity(int complexity = 0)
    {
        if (_data.Count == 0)
            throw new InvalidOperationException("empty");
        var index = (int)((complexity / 100.0) * (_data.Count - 1));
        return new ModalSelectionMatrixRow<T>(_data[index]);
    }

    public ModalSelectionMatrixRow<T> GetClosest() => new ModalSelectionMatrixRow<T>(_data.MinBy(x => x.Distance));
    public ModalSelectionMatrixRow<T> GetFurthest() => new ModalSelectionMatrixRow<T>(_data.MaxBy(x => x.Distance));
}

public sealed class ModalRelativeModeMatrixDistance
{
    private readonly List<(int ModeIndex, int RelativeModeIndex, PositionVector Vector, double Distance)> _data;
    public ModalRelativeModeMatrixDistance() => _data = new List<(int, int, PositionVector, double)>();
    public ModalRelativeModeMatrixDistance(IEnumerable<(int, int, PositionVector, double)> data) => _data = data.ToList();
    public int Count => _data.Count;
    public (int ModeIndex, int RelativeModeIndex, PositionVector Vector, double Distance) this[int i] => _data[i];
    public void SortByDistance() => _data.Sort((a, b) =>
    {
        var c = a.Distance.CompareTo(b.Distance);
        if (c != 0) return c;
        c = a.ModeIndex.CompareTo(b.ModeIndex);
        if (c != 0) return c;
        return a.RelativeModeIndex.CompareTo(b.RelativeModeIndex);
    });
    public void SortByMode() => _data.Sort((a, b) =>
        a.ModeIndex != b.ModeIndex ? a.ModeIndex.CompareTo(b.ModeIndex) : a.Distance.CompareTo(b.Distance));
    public List<PositionVector> GetVectors() => _data.Select(x => x.Vector).ToList();
    public List<int> GetModeIndices() => _data.Select(x => x.ModeIndex).ToList();
    public List<int> GetRelativeModeIndices() => _data.Select(x => x.RelativeModeIndex).ToList();
    public List<double> GetDistances() => _data.Select(x => x.Distance).ToList();

    public ModalRelativeModeMatrixRow GetByComplexity(int complexity = 0)
    {
        if (_data.Count == 0)
            throw new InvalidOperationException("empty");
        if (complexity < 0 || complexity > 100)
            throw new ArgumentOutOfRangeException(nameof(complexity));
        var index = (int)((complexity / 100.0) * (_data.Count - 1));
        return new ModalRelativeModeMatrixRow(_data[index]);
    }

    public ModalRelativeModeMatrixRow GetClosest()
    {
        if (_data.Count == 0)
            throw new InvalidOperationException("empty");
        return new ModalRelativeModeMatrixRow(_data.MinBy(x => x.Distance));
    }

    public ModalRelativeModeMatrixRow GetFurthest()
    {
        if (_data.Count == 0)
            throw new InvalidOperationException("empty");
        return new ModalRelativeModeMatrixRow(_data.MaxBy(x => x.Distance));
    }
}

public static class MatrixDistanceOps
{
    public static ModalMatrixDistance<PositionVector> CalculateDistances(
        PositionVector reference,
        ModalMatrix<PositionVector> matrix,
        DistanceFuncPv? distFunc = null,
        bool sort = true)
    {
        distFunc ??= Distances.ManhattanDistance;
        var result = new List<(PositionVector, int, double)>();
        for (var i = 0; i < matrix.Count; i++)
        {
            var (vec, idx) = matrix[i];
            var dist = distFunc(reference, vec);
            result.Add((vec, idx, dist));
        }
        var mmd = new ModalMatrixDistance<PositionVector>(result);
        if (sort)
            mmd.SortByDistance();
        return mmd;
    }

    public static ModalMatrixDistance<IntervalVector> CalculateDistances(
        IntervalVector reference,
        ModalMatrix<IntervalVector> matrix,
        DistanceFuncIv? distFunc = null,
        bool sort = true)
    {
        distFunc ??= Distances.ManhattanDistance;
        var result = new List<(IntervalVector, int, double)>();
        for (var i = 0; i < matrix.Count; i++)
        {
            var (vec, idx) = matrix[i];
            var dist = distFunc(reference, vec);
            result.Add((vec, idx, dist));
        }
        var mmd = new ModalMatrixDistance<IntervalVector>(result);
        if (sort)
            mmd.SortByDistance();
        return mmd;
    }

    public static TranspositionMatrixDistance CalculateDistances(
        PositionVector reference,
        TranspositionMatrix matrix,
        DistanceFuncPv? distFunc = null,
        bool sort = true)
    {
        distFunc ??= Distances.ManhattanDistance;
        var result = new List<(PositionVector, int, double)>();
        for (var i = 0; i < matrix.Count; i++)
        {
            var (vec, idx) = matrix[i];
            result.Add((vec, idx, distFunc(reference, vec)));
        }
        var tmd = new TranspositionMatrixDistance(result);
        if (sort)
            tmd.SortByDistance();
        return tmd;
    }

    public static int Align(PositionVector reference, PositionVector target)
    {
        if (reference.Count == 0 || target.Count == 0)
            throw new ArgumentException("align: reference and target must be non-empty");
        var refRange = reference.EffectiveRange;
        var tgtRange = target.EffectiveRange;
        if (refRange <= 0 || tgtRange <= 0)
            throw new ArgumentException("align: effective range must be positive");
        var minV = reference[0];
        var referenceDiv = MathUtil.EuclideanDivision(reference[0], refRange);
        var targetDiv = MathUtil.EuclideanDivision(target[0], tgtRange);
        var diffOct = referenceDiv.Quotient - targetDiv.Quotient;
        var size = (int)target.Count;
        var i = diffOct * size;
        while (target[i] <= minV)
            i++;
        while (target[i] > minV)
            i--;
        return i;
    }

    public static RelativeModeMatrixDistance CalculateDistances(
        PositionVector reference,
        RelativeModeMatrix matrix,
        DistanceFuncPv? distFunc = null,
        bool sort = true)
    {
        distFunc ??= Distances.ManhattanDistance;
        var result = new List<(PositionVector, int, double)>();
        for (var i = 0; i < matrix.Count; i++)
        {
            var (vec, idx) = matrix[i];
            result.Add((vec, idx, distFunc(reference, vec)));
        }
        var rmd = new RelativeModeMatrixDistance(result, matrix.Center);
        if (sort)
            rmd.SortByDistance();
        return rmd;
    }

    public static ModalSelectionMatrixDistance<PositionVector> CalculateDistances(
        PositionVector reference,
        ModalSelectionMatrix<PositionVector> matrix,
        DistanceFuncPv? distFunc = null,
        bool sort = true)
    {
        distFunc ??= Distances.ManhattanDistance;
        var result = new List<(PositionVector, int, double)>();
        for (var i = 0; i < matrix.Count; i++)
        {
            var (vec, idx) = matrix[i];
            result.Add((vec, idx, distFunc(reference, vec)));
        }
        var msd = new ModalSelectionMatrixDistance<PositionVector>(result);
        if (sort)
            msd.SortByDistance();
        return msd;
    }

    public static ModalSelectionMatrixDistance<IntervalVector> CalculateDistances(
        IntervalVector reference,
        ModalSelectionMatrix<IntervalVector> matrix,
        DistanceFuncIv? distFunc = null,
        bool sort = true)
    {
        distFunc ??= Distances.ManhattanDistance;
        var result = new List<(IntervalVector, int, double)>();
        for (var i = 0; i < matrix.Count; i++)
        {
            var (vec, idx) = matrix[i];
            result.Add((vec, idx, distFunc(reference, vec)));
        }
        var msd = new ModalSelectionMatrixDistance<IntervalVector>(result);
        if (sort)
            msd.SortByDistance();
        return msd;
    }

    public static ModalRelativeModeMatrixDistance CalculateDistances(
        PositionVector reference,
        ModalRelativeModeMatrix<PositionVector> matrix,
        DistanceFuncPv? distFunc = null,
        bool sort = true)
    {
        distFunc ??= Distances.ManhattanDistance;
        var result = new List<(int, int, PositionVector, double)>();
        for (var i = 0; i < matrix.Count; i++)
        {
            var (rtm, modeIdx) = matrix[i];
            for (var j = 0; j < rtm.Count; j++)
            {
                var (vec, transIdx) = rtm[j];
                result.Add((modeIdx, transIdx, vec, distFunc(reference, vec)));
            }
        }
        var mrmd = new ModalRelativeModeMatrixDistance(result);
        if (sort)
            mrmd.SortByDistance();
        return mrmd;
    }
}
