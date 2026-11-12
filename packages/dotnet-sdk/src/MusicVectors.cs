namespace Musicpp;

public static class MusicVectors
{
    public static IntervalVector PositionsToIntervals(PositionVector positions)
    {
        var m = positions.Mod;
        if (positions.Count == 0)
            return new IntervalVector(new List<int>(), 0, m);
        var n = positions.Count;
        if (n == 1)
            return new IntervalVector(new List<int>(), positions[0], m);
        var intervalData = new List<int>();
        for (var i = 0; i + 1 < n; i++)
            intervalData.Add(positions[(int)(i + 1)] - positions[(int)i]);
        var closureRaw = positions[0] - positions[(int)(n - 1)];
        var closurePeriod = positions.EffectiveRange;
        if (closurePeriod > 0)
            intervalData.Add(MathUtil.EuclideanDivision(closureRaw, closurePeriod).Remainder);
        else
            intervalData.Add(closureRaw);
        return new IntervalVector(intervalData, positions[0], m);
    }

    public static PositionVector IntervalsToPositions(IntervalVector intervals)
    {
        var m = intervals.Mod;
        var intervalData = intervals.MutableData.ToList();
        if (intervalData.Count == 0)
            return new PositionVector(new List<int> { intervals.Offset }, m, 0, true, false);
        var posData = new List<int> { intervals.Offset };
        var currentPos = intervals.Offset;
        for (var i = 0; i + 1 < intervalData.Count; i++)
        {
            currentPos += intervalData[i];
            posData.Add(currentPos);
        }
        return new PositionVector(posData, m, 0, true, false);
    }

    public static OnsetVector PositionsToOnset(PositionVector positions)
    {
        if (positions.Count == 0)
            return new OnsetVector(new List<int>(), 0, positions.Mod);
        var posData = positions.MutableData.ToList();
        var range = positions.EffectiveRange;
        if (range <= 0)
            return new OnsetVector(new List<int>(), 0, positions.Mod);
        var onsetData = Enumerable.Repeat(0, range).ToList();
        var minPos = posData.Min();
        foreach (var pos in posData)
        {
            var normalizedPos = pos - minPos;
            var div = MathUtil.EuclideanDivision(normalizedPos, range);
            onsetData[div.Remainder] = 1;
        }
        return new OnsetVector(onsetData, minPos, range);
    }
}

public sealed class VectorSet : IEquatable<VectorSet>
{
    private PositionVector _positions;
    private IntervalVector _intervals;
    private OnsetVector _onset;
    private int _mod;

    public VectorSet()
    {
        _positions = new PositionVector(new List<int> { 0 }, 12, 0, true, false);
        _intervals = new IntervalVector(new List<int>(), 0, 12);
        _onset = new OnsetVector(new List<int> { 1 }, 0, 12);
        _mod = 12;
    }

    public VectorSet(int modulo)
    {
        _positions = new PositionVector(new List<int> { 0 }, modulo, 0, true, false);
        _intervals = new IntervalVector(new List<int>(), 0, modulo);
        _onset = new OnsetVector(new List<int> { 1 }, 0, modulo);
        _mod = modulo;
    }

    public VectorSet(PositionVector pv)
    {
        _positions = pv.Clone();
        _mod = pv.Mod;
        UpdateFromPositions();
    }

    public VectorSet(IntervalVector iv)
    {
        _intervals = iv.Clone();
        _mod = iv.Mod;
        UpdateFromIntervals();
    }

    public VectorSet(OnsetVector ov)
    {
        _onset = ov.Clone();
        _mod = ov.Mod;
        UpdateFromOnset();
    }

    public PositionVector Positions => _positions;
    public IntervalVector Intervals => _intervals;
    public OnsetVector Onset => _onset;
    public int Mod => _mod;

    public void SetMod(int m)
    {
        _mod = m;
        _positions.SetMod(m);
        _intervals.SetMod(m);
        _onset.SetMod(m);
    }

    public void UpdateFromPositions()
    {
        _intervals = MusicVectors.PositionsToIntervals(_positions);
        _onset = MusicVectors.PositionsToOnset(_positions);
    }

    public void UpdateFromIntervals()
    {
        _positions = MusicVectors.IntervalsToPositions(_intervals);
        _onset = MusicVectors.PositionsToOnset(_positions);
    }

    private PositionVector OnsetToPositions()
    {
        var onsetData = _onset.MutableData;
        var offset = _onset.Offset;
        var posData = new List<int>();
        for (var i = 0; i < onsetData.Count; i++)
            if (onsetData[i] == 1)
                posData.Add((int)i + offset);
        if (posData.Count == 0)
            return new PositionVector(new List<int> { 0 }, _mod, 0, true, false);
        return new PositionVector(posData, _mod, 0, true, false);
    }

    public void UpdateFromOnset()
    {
        _positions = OnsetToPositions();
        _intervals = MusicVectors.PositionsToIntervals(_positions);
    }

    public VectorSet Transpose(int amount)
    {
        var r = Clone();
        r._positions = r._positions + amount;
        r.UpdateFromPositions();
        return r;
    }

    public VectorSet MultiplyPositions(int scalar)
    {
        var r = Clone();
        r._positions = r._positions * scalar;
        r.UpdateFromPositions();
        return r;
    }

    public VectorSet Negative(int axis = 10)
    {
        var r = Clone();
        r._positions = r._positions.Negative(axis);
        r.UpdateFromPositions();
        return r;
    }

    public VectorSet Rotate(int amount)
    {
        var r = Clone();
        r._positions = r._positions.Rotate(amount);
        r.UpdateFromPositions();
        return r;
    }

    public VectorSet RelativeMode(int amount, int length = 0)
    {
        var r = Clone();
        r._positions = r._positions.RelativeMode(amount, length);
        r.UpdateFromPositions();
        return r;
    }

    public VectorSet Inversion(int amount, int length = 0) => RelativeMode(amount, length);

    public VectorSet InvertPositions(int axisIndex, bool sortOutput = true)
    {
        var r = Clone();
        r._positions = r._positions.Inversion(axisIndex, sortOutput);
        r.UpdateFromPositions();
        return r;
    }

    public VectorSet ComplementPositions()
    {
        var r = Clone();
        r._positions = r._positions.Complement();
        r.UpdateFromPositions();
        return r;
    }

    public VectorSet AddToIntervals(int amount)
    {
        var r = Clone();
        r._intervals = r._intervals + amount;
        r.UpdateFromIntervals();
        return r;
    }

    public VectorSet MultiplyIntervals(int scalar)
    {
        var r = Clone();
        r._intervals = r._intervals * scalar;
        r.UpdateFromIntervals();
        return r;
    }

    public VectorSet ParallelMode(int amount)
    {
        var r = Clone();
        r._intervals = r._intervals.ParallelMode(amount);
        r.UpdateFromIntervals();
        return r;
    }

    public VectorSet ReverseIntervals()
    {
        var r = Clone();
        r._intervals = r._intervals.Reverse();
        r.UpdateFromIntervals();
        return r;
    }

    public VectorSet InvertIntervals(int axisIndex)
    {
        var r = Clone();
        r._intervals = r._intervals.Inversion(axisIndex);
        r.UpdateFromIntervals();
        return r;
    }

    public VectorSet Mode(int amount) => ParallelMode(amount);

    public VectorSet RotateOnset(int amount)
    {
        var r = Clone();
        r._onset = r._onset.Rotate(amount);
        r.UpdateFromOnset();
        return r;
    }

    public VectorSet ComplementOnset()
    {
        var r = Clone();
        r._onset = r._onset.Complement();
        r.UpdateFromOnset();
        return r;
    }

    public VectorSet MultiplyOnset(int scalar)
    {
        var r = Clone();
        r._onset = r._onset * scalar;
        r._mod = r._onset.Mod;
        r.UpdateFromOnset();
        return r;
    }

    public VectorSet DivideOnset(int divisor)
    {
        var r = Clone();
        r._onset = r._onset / divisor;
        r._mod = r._onset.Mod;
        r.UpdateFromOnset();
        return r;
    }

    public static VectorSet operator |(VectorSet a, VectorSet b)
    {
        var r = a.Clone();
        r._onset = r._onset | b._onset;
        r.UpdateFromOnset();
        return r;
    }

    public static VectorSet operator &(VectorSet a, VectorSet b)
    {
        var r = a.Clone();
        r._onset = r._onset & b._onset;
        r.UpdateFromOnset();
        return r;
    }

    public static VectorSet operator ^(VectorSet a, VectorSet b)
    {
        var r = a.Clone();
        r._onset = r._onset ^ b._onset;
        r.UpdateFromOnset();
        return r;
    }

    public bool Equals(VectorSet? other) =>
        other is not null &&
        _positions.Equals(other._positions) &&
        _intervals.Equals(other._intervals) &&
        _onset.Equals(other._onset);

    public override bool Equals(object? obj) => obj is VectorSet v && Equals(v);
    public override int GetHashCode() => HashCode.Combine(_positions, _intervals, _onset);

    public VectorSet Clone() => new VectorSet(_positions.Clone());

    public static VectorSet FromPositions(IEnumerable<int> data, int modulo = 12) =>
        new VectorSet(new PositionVector(data.ToList(), modulo, -1, true, false));

    public static VectorSet FromIntervals(IEnumerable<int> data, int modulo = 12) =>
        new VectorSet(new IntervalVector(data.ToList(), 0, modulo));

    public static VectorSet FromOnset(IEnumerable<int> data, int offset = 0, int modulo = 12) =>
        new VectorSet(new OnsetVector(data.ToList(), offset, modulo));

    public static VectorSet Euclidean(int pulses, int steps) =>
        new VectorSet(OnsetVector.Euclidean(pulses, steps));
}
