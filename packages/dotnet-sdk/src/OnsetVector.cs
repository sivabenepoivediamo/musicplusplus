namespace Musicpp;

public sealed class OnsetVector : IEquatable<OnsetVector>, ICopyable<OnsetVector>
{
    private List<int> _data;
    private int _offset;
    private int _mod;

    private static void ValidatePattern(IReadOnlyList<int> data)
    {
        foreach (var v in data)
            if (v != 0 && v != 1)
                throw new ArgumentException("onset_vector pattern must contain only 0s and 1s");
    }

    public OnsetVector()
    {
        _data = new List<int> { 1, 0, 0, 0 };
        _offset = 0;
        _mod = 4;
    }

    public OnsetVector(IEnumerable<int> values, int offset = 0, int mod = 4)
    {
        _data = values.ToList();
        _offset = offset;
        _mod = mod;
        ValidatePattern(_data);
    }

    public IReadOnlyList<int> Data => _data;
    public List<int> MutableData => _data;
    public int Offset => _offset;
    public int Mod => _mod;
    public int Count => _data.Count;

    public void SetOffset(int v) => _offset = v;
    public void SetMod(int v) => _mod = v;

    public void ValidateOnsetPattern() => ValidatePattern(_data);

    public static OnsetVector operator *(OnsetVector bv, int scalar)
    {
        if (scalar <= 0)
            throw new ArgumentException("scalar must be positive");
        var n = bv._data.Count;
        var o = new List<int>(Enumerable.Repeat(0, scalar * n));
        for (var j = 0; j < n; j++)
            o[j * scalar] = bv._data[j];
        return new OnsetVector(o, bv._offset, bv._mod * scalar);
    }

    public static OnsetVector operator *(int scalar, OnsetVector bv) => bv * scalar;

    public static OnsetVector operator /(OnsetVector bv, int scalar)
    {
        var n = bv._data.Count;
        if (scalar <= 0)
            throw new ArgumentException("scalar must be positive");
        if (scalar > n)
            throw new ArgumentException("scalar must be less than or equal to vector size");
        if (n % scalar != 0)
            throw new ArgumentException("Vector size must be divisible by scalar");
        if (bv._mod % scalar != 0)
            throw new ArgumentException("mod must be divisible by scalar");
        var o = new List<int>();
        for (var i = 0; i < n / scalar; i++)
            o.Add(bv._data[i * scalar]);
        return new OnsetVector(o, bv._offset, bv._mod / scalar);
    }

    public OnsetVector ComponentwiseOr(IReadOnlyList<int> other, bool useLooping = false)
    {
        if (other.Count == 0)
            return Clone();
        if (_data.Count == 0)
            return new OnsetVector(other.ToList(), _offset, _mod);
        return LogicalCombineCore(other, useLooping, (a, b) => a | b);
    }

    public OnsetVector ComponentwiseAnd(IReadOnlyList<int> other, bool useLooping = false)
    {
        if (other.Count == 0)
            return new OnsetVector(new List<int>(), _offset, _mod);
        if (_data.Count == 0)
            return Clone();
        return LogicalCombineCore(other, useLooping, (a, b) => a & b);
    }

    public OnsetVector ComponentwiseXor(IReadOnlyList<int> other, bool useLooping = false)
    {
        if (other.Count == 0)
            return Clone();
        if (_data.Count == 0)
            return new OnsetVector(other.ToList(), _offset, _mod);
        return LogicalCombineCore(other, useLooping, (a, b) => a ^ b);
    }

    private OnsetVector LogicalCombineCore(IReadOnlyList<int> other, bool useLooping, Func<int, int, int> op)
    {
        List<int> result;
        if (useLooping)
        {
            var maxLength = Math.Max(_data.Count, other.Count);
            result = new List<int>(maxLength);
            for (var i = 0; i < maxLength; i++)
                result.Add(op(_data[i % _data.Count], other[i % other.Count]));
        }
        else
        {
            var minLength = Math.Min(_data.Count, other.Count);
            result = new List<int>();
            for (var i = 0; i < minLength; i++)
                result.Add(op(_data[i], other[i]));
            for (var i = minLength; i < _data.Count; i++)
                result.Add(_data[i]);
            for (var i = minLength; i < other.Count; i++)
                result.Add(other[i]);
        }
        return new OnsetVector(result, _offset, _mod);
    }

    public static OnsetVector operator |(OnsetVector a, OnsetVector b)
    {
        var adapted = AdaptToLcm(new[] { a, b });
        return adapted[0].ComponentwiseOr(adapted[1].MutableData, false);
    }

    public static OnsetVector operator &(OnsetVector a, OnsetVector b)
    {
        var adapted = AdaptToLcm(new[] { a, b });
        return adapted[0].ComponentwiseAnd(adapted[1].MutableData, false);
    }

    public static OnsetVector operator ^(OnsetVector a, OnsetVector b)
    {
        var adapted = AdaptToLcm(new[] { a, b });
        return adapted[0].ComponentwiseXor(adapted[1].MutableData, false);
    }

    public OnsetVector Nor(OnsetVector other) => ~(this | other);
    public OnsetVector Nand(OnsetVector other) => ~(this & other);
    public OnsetVector Xnor(OnsetVector other) => ~(this ^ other);
    public OnsetVector Complement() => new OnsetVector(_data.Select(x => 1 - x).ToList(), _offset, _mod);
    public static OnsetVector operator ~(OnsetVector bv) => bv.Complement();

    public int this[int index]
    {
        get
        {
            if (_data.Count == 0)
                return 0;
            var div = MathUtil.EuclideanDivision(index, _data.Count);
            return _data[div.Remainder];
        }
    }

    public bool Equals(OnsetVector? other) =>
        other is not null && _data.SequenceEqual(other._data) && _offset == other._offset && _mod == other._mod;

    public override bool Equals(object? obj) => obj is OnsetVector o && Equals(o);
    public override int GetHashCode() => HashCode.Combine(string.Join(",", _data), _offset, _mod);

    public static bool operator ==(OnsetVector? a, OnsetVector? b) =>
        ReferenceEquals(a, b) || (a is not null && a.Equals(b));
    public static bool operator !=(OnsetVector? a, OnsetVector? b) => !(a == b);

    public override string ToString() => "[" + string.Join(", ", _data) + "] (offset: " + _offset + ")";

    public OnsetVector Clone() => new OnsetVector(_data.ToList(), _offset, _mod);

    public OnsetVector Copy() => Clone();

    public static List<OnsetVector> AdaptToLcm(IReadOnlyList<OnsetVector> vectors)
    {
        if (vectors.Count == 0)
            return new List<OnsetVector>();
        var unique = new SortedSet<int>(vectors.Select(v => v._mod));
        if (unique.Count == 1)
            return vectors.Select(v => v.Clone()).ToList();
        var lcmVal = MathUtil.Lcm(unique.ToList());
        var adapted = new List<OnsetVector>();
        foreach (var bv in vectors)
        {
            var scaleFactor = lcmVal / bv._mod;
            var spaced = new List<int>();
            foreach (var val in bv._data)
            {
                spaced.Add(val);
                for (var i = 1; i < scaleFactor; i++)
                    spaced.Add(0);
            }
            adapted.Add(new OnsetVector(spaced, bv._offset, lcmVal));
        }
        return adapted;
    }

    public static OnsetVector Euclidean(int pulses, int steps)
    {
        if (pulses <= 0 || steps <= 0 || pulses > steps)
            throw new ArgumentException("Invalid Euclidean parameters");
        var groups = new List<List<int>>();
        for (var i = 0; i < pulses; i++)
            groups.Add(new List<int> { 1 });
        for (var i = 0; i < steps - pulses; i++)
            groups.Add(new List<int> { 0 });
        var p = pulses;
        var s = steps;
        while (groups.Count > 1)
        {
            var minSize = Math.Min(p, s - p);
            if (minSize <= 0)
                break;
            for (var i = 0; i < minSize; i++)
            {
                groups[i].AddRange(groups[^1]);
                groups.RemoveAt(groups.Count - 1);
            }
            p = minSize;
            s = groups.Count;
        }
        var result = new List<int>();
        foreach (var g in groups)
            result.AddRange(g);
        return new OnsetVector(result, 0, result.Count);
    }

    public OnsetVector Rotate(int rotationAmount)
    {
        if (_data.Count == 0)
            return Clone();
        var size = _data.Count;
        var normalizedRotation = MathUtil.EuclideanDivision(rotationAmount, size).Remainder;
        var rotated = new List<int>(size);
        for (var i = 0; i < size; i++)
            rotated.Add(_data[(i + normalizedRotation) % size]);
        return new OnsetVector(rotated, _offset, _mod);
    }

    public OnsetVector Inversion(int axisIndex)
    {
        if (_data.Count == 0)
            return Clone();
        var size = _data.Count;
        var normalizedAxis = MathUtil.EuclideanDivision(axisIndex, size).Remainder;
        var inverted = new List<int>(size);
        for (var i = 0; i < size; i++)
        {
            var distance = i - normalizedAxis;
            var mirrorIndex = normalizedAxis - distance;
            var mirrorDiv = MathUtil.EuclideanDivision(mirrorIndex, size);
            inverted.Add(_data[mirrorDiv.Remainder]);
        }
        return new OnsetVector(inverted, _offset, _mod);
    }

    public OnsetVector Transpose(int amount) => new OnsetVector(_data.ToList(), _offset + amount, _mod);

    public OnsetVector Concatenate(OnsetVector other)
    {
        var r = new List<int>(_data);
        r.AddRange(other._data);
        return new OnsetVector(r, _offset, _mod);
    }

    public OnsetVector Repeat(int times)
    {
        if (times <= 0)
            return new OnsetVector(new List<int>(), _offset, _mod);
        var r = new List<int>(_data.Count * times);
        for (var t = 0; t < times; t++)
            r.AddRange(_data);
        return new OnsetVector(r, _offset, _mod);
    }

    public int CountPulses() => _data.Count(x => x == 1);

    public double Density() => _data.Count == 0 ? 0 : (double)CountPulses() / _data.Count;

    public List<int> GetPulseIndices()
    {
        var idx = new List<int>();
        for (var i = 0; i < _data.Count; i++)
            if (_data[i] == 1)
                idx.Add(i);
        return idx;
    }

    public List<int> GetInterOnsetIntervals()
    {
        var indices = GetPulseIndices();
        if (indices.Count < 2)
            return new List<int>();
        var intervals = new List<int>();
        for (var i = 1; i < indices.Count; i++)
            intervals.Add(indices[i] - indices[i - 1]);
        intervals.Add(_data.Count - indices[^1] + indices[0]);
        return intervals;
    }
}
