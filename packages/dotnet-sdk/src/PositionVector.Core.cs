namespace Musicpp;

public sealed partial class PositionVector : IEquatable<PositionVector>, ICopyable<PositionVector>
{
    private List<int> _data;
    private int _mod;
    private int _userRange;
    private int _range;
    private bool _rangeUpdate;
    private bool _user;

    private int RangeCalculation()
    {
        var modulo = _user ? _userRange : _mod;
        if (_data.Count == 0)
            return modulo;
        var maxValue = _data.Max();
        var minValue = _data.Min();
        var span = maxValue - minValue;
        var result = MathUtil.EuclideanDivision(span, modulo);
        return modulo * (result.Quotient + 1);
    }

    private int InitializeRange() => _rangeUpdate ? RangeCalculation() : (_user ? _userRange : _mod);

    private void UpdateRangeIfNeeded()
    {
        if (_rangeUpdate)
            _range = RangeCalculation();
    }

    public PositionVector()
    {
        _data = new List<int> { 0 };
        _mod = 12;
        _userRange = 12;
        _rangeUpdate = true;
        _user = false;
        _range = InitializeRange();
    }

    public PositionVector(IEnumerable<int> values, int mod = 12, int userRange = 0, bool rangeUpdate = true, bool user = false)
    {
        _data = values.ToList();
        _mod = mod;
        _userRange = userRange > 0 ? userRange : mod;
        _rangeUpdate = rangeUpdate;
        _user = user;
        _range = InitializeRange();
    }

    public IReadOnlyList<int> Data => _data;
    public List<int> MutableData => _data;

    public int Mod => _mod;
    public int UserRange => _userRange;
    public int EffectiveRange => _range;
    public bool AutoRangeUpdates => _rangeUpdate;
    public bool UseUserRange => _user;
    public int Count => _data.Count;

    public void SetMod(int newMod)
    {
        _mod = newMod;
        UpdateRangeIfNeeded();
    }

    public void SetUserRange(int newUserRange)
    {
        _userRange = newUserRange;
        UpdateRangeIfNeeded();
    }

    public void SetAutoRangeUpdates(bool enabled)
    {
        _rangeUpdate = enabled;
        UpdateRangeIfNeeded();
    }

    public void SetUseUserRange(bool use)
    {
        _user = use;
        UpdateRangeIfNeeded();
    }

    public void SetEffectiveRange(int newRange)
    {
        _range = newRange;
        _rangeUpdate = false;
    }

    public int Element(int index)
    {
        var size = _data.Count;
        if (size == 0)
            return 0;
        var div = MathUtil.EuclideanDivision(index, size);
        var cycles = (index - div.Remainder) / size;
        var effectiveRange = EffectiveRange;
        return _data[div.Remainder] + Math.Abs(effectiveRange) * cycles;
    }

    public int this[int index] => Element(index);

    public bool Equals(PositionVector? other) =>
        other is not null &&
        _data.SequenceEqual(other._data) &&
        _mod == other._mod &&
        _userRange == other._userRange &&
        _user == other._user;

    public override bool Equals(object? obj) => obj is PositionVector p && Equals(p);

    public override int GetHashCode() => HashCode.Combine(
        string.Join(",", _data), _mod, _userRange, _user);

    public static bool operator ==(PositionVector? a, PositionVector? b) =>
        ReferenceEquals(a, b) || (a is not null && a.Equals(b));

    public static bool operator !=(PositionVector? a, PositionVector? b) => !(a == b);

    public override string ToString() => "[" + string.Join(", ", _data) + "]";

    public static List<PositionVector> AdaptToLcm(IReadOnlyList<PositionVector> vectors)
    {
        if (vectors.Count == 0)
            return new List<PositionVector>();
        var unique = new SortedSet<int>(vectors.Select(v => v.Mod));
        if (unique.Count == 1)
            return vectors.Select(v => v.Clone()).ToList();
        var moduliList = unique.ToList();
        var lcmVal = MathUtil.Lcm(moduliList);
        var adapted = new List<PositionVector>();
        foreach (var pv in vectors)
        {
            var scaleFactor = lcmVal / pv.Mod;
            var scaledData = pv.MutableData.Select(x => x * scaleFactor).ToList();
            var scaledUserRange = pv.UserRange * scaleFactor;
            var scaledRange = pv.EffectiveRange * scaleFactor;
            var adaptedPv = new PositionVector(scaledData, lcmVal, scaledUserRange, false, pv.UseUserRange);
            adaptedPv.SetEffectiveRange(scaledRange);
            adapted.Add(adaptedPv);
        }
        return adapted;
    }

    public PositionVector Clone()
    {
        var p = new PositionVector(MutableData.ToList(), Mod, UserRange, AutoRangeUpdates, UseUserRange);
        if (!AutoRangeUpdates)
            p.SetEffectiveRange(EffectiveRange);
        return p;
    }

    public PositionVector Copy() => Clone();

    private PositionVector WithData(List<int> data) =>
        new(data, _mod, _userRange, _rangeUpdate, _user);
}
