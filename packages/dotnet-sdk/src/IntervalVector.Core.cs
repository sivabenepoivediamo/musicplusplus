namespace Musicpp;

public sealed partial class IntervalVector : IEquatable<IntervalVector>, ICopyable<IntervalVector>
{
    private List<int> _data;
    private int _offset;
    private int _mod;

    public IntervalVector()
    {
        _data = new List<int> { 0 };
        _offset = 0;
        _mod = 12;
    }

    public IntervalVector(IEnumerable<int> data, int offset = 0, int mod = 12)
    {
        _data = data.ToList();
        _offset = offset;
        _mod = mod;
    }

    public IReadOnlyList<int> Data => _data;
    public List<int> MutableData => _data;
    public int Offset => _offset;
    public int Mod => _mod;
    public int Count => _data.Count;
    public bool IsEmpty => _data.Count == 0;

    public void SetOffset(int v) => _offset = v;
    public void SetMod(int v) => _mod = v;
    public void SetData(IEnumerable<int> d) => _data = d.ToList();

    public int Element(int i)
    {
        if (_data.Count == 0)
            return 0;
        var size = _data.Count;
        var div = MathUtil.EuclideanDivision(i, size);
        return _data[div.Remainder];
    }

    public int this[int index] => Element(index);

    public bool Equals(IntervalVector? other) =>
        other is not null && _data.SequenceEqual(other._data) && _offset == other._offset && _mod == other._mod;

    public override bool Equals(object? obj) => obj is IntervalVector iv && Equals(iv);
    public override int GetHashCode() => HashCode.Combine(string.Join(",", _data), _offset, _mod);

    public static bool operator ==(IntervalVector? a, IntervalVector? b) =>
        ReferenceEquals(a, b) || (a is not null && a.Equals(b));
    public static bool operator !=(IntervalVector? a, IntervalVector? b) => !(a == b);

    public override string ToString() => "[" + string.Join(", ", _data) + "] (offset: " + _offset + ")";

    public IntervalVector Clone() => new IntervalVector(_data.ToList(), _offset, _mod);

    public IntervalVector Copy() => Clone();

    public static List<IntervalVector> AdaptToLcm(IReadOnlyList<IntervalVector> vectors)
    {
        if (vectors.Count == 0)
            return new List<IntervalVector>();
        var unique = new SortedSet<int>();
        foreach (var iv in vectors)
            if (iv._mod != 0)
                unique.Add(iv._mod);
        if (unique.Count == 0 || unique.Count == 1)
            return vectors.Select(v => v.Clone()).ToList();
        var lcmVal = MathUtil.Lcm(unique.ToList());
        var adapted = new List<IntervalVector>();
        foreach (var iv in vectors)
        {
            if (iv._mod == 0)
            {
                adapted.Add(iv.Clone());
                continue;
            }
            var scale = lcmVal / iv._mod;
            var scaled = iv._data.Select(x => x * scale).ToList();
            adapted.Add(new IntervalVector(scaled, iv._offset * scale, lcmVal));
        }
        return adapted;
    }
}
