namespace Musicpp;

public sealed partial class PositionVector
{
    public static PositionVector operator +(PositionVector pv, int scalar)
    {
        var r = pv.MutableData.Select(x => x + scalar).ToList();
        return new PositionVector(r, pv._mod, pv._userRange, pv._rangeUpdate, pv._user);
    }

    public static PositionVector operator +(int scalar, PositionVector pv) => pv + scalar;

    public static PositionVector operator -(PositionVector pv, int scalar)
    {
        var r = pv.MutableData.Select(x => x - scalar).ToList();
        return new PositionVector(r, pv._mod, pv._userRange, pv._rangeUpdate, pv._user);
    }

    public static PositionVector operator -(int scalar, PositionVector pv) => pv - scalar;

    public static PositionVector operator *(PositionVector pv, int scalar)
    {
        var r = pv.MutableData.Select(x => x * scalar).ToList();
        return new PositionVector(r, pv._mod, pv._userRange, pv._rangeUpdate, pv._user);
    }

    public static PositionVector operator *(int scalar, PositionVector pv) => pv * scalar;

    public static PositionVector operator /(PositionVector pv, int divisor)
    {
        if (divisor == 0)
            throw new ArgumentException("Division by zero", nameof(divisor));
        var r = pv.MutableData.Select(x => MathUtil.EuclideanDivision(x, divisor).Quotient).ToList();
        return new PositionVector(r, pv._mod, pv._userRange, pv._rangeUpdate, pv._user);
    }

    public static PositionVector operator %(PositionVector pv, int divisor)
    {
        if (divisor == 0)
            throw new ArgumentException("Division by zero", nameof(divisor));
        var r = pv.MutableData.Select(x => MathUtil.EuclideanDivision(x, divisor).Remainder).ToList();
        return new PositionVector(r, pv._mod, pv._userRange, pv._rangeUpdate, pv._user);
    }

    public static PositionVector operator +(PositionVector a, PositionVector b) => a.ComponentwiseSum(b.MutableData, false);
    public static PositionVector operator -(PositionVector a, PositionVector b) => a.ComponentwiseSubtraction(b.MutableData, false);
    public static PositionVector operator *(PositionVector a, PositionVector b) => a.ComponentwiseProduct(b.MutableData, true);
    public static PositionVector operator /(PositionVector a, PositionVector b) => a.ComponentwiseDivision(b.MutableData, true);
    public static PositionVector operator %(PositionVector a, PositionVector b) => a.ComponentwiseModulo(b.MutableData, true);

    public static PositionVector operator +(PositionVector a, IReadOnlyList<int> b) => a.ComponentwiseSum(b, false);
    public static PositionVector operator -(PositionVector a, IReadOnlyList<int> b) => a.ComponentwiseSubtraction(b, false);
    public static PositionVector operator *(PositionVector a, IReadOnlyList<int> b) => a.ComponentwiseProduct(b, true);
    public static PositionVector operator /(PositionVector a, IReadOnlyList<int> b) => a.ComponentwiseDivision(b, true);
    public static PositionVector operator %(PositionVector a, IReadOnlyList<int> b) => a.ComponentwiseModulo(b, true);

    public PositionVector ComponentwiseSum(IReadOnlyList<int> other, bool useLooping = false)
    {
        if (other.Count == 0)
            return Clone();
        if (_data.Count == 0)
            return new PositionVector(other.ToList(), _mod, _userRange, _rangeUpdate, _user);
        List<int> result;
        if (useLooping)
        {
            var maxLength = Math.Max(_data.Count, other.Count);
            result = new List<int>(maxLength);
            for (var i = 0; i < maxLength; i++)
                result.Add(_data[i % _data.Count] + other[i % other.Count]);
        }
        else
        {
            var minLength = Math.Min(_data.Count, other.Count);
            result = new List<int>();
            for (var i = 0; i < minLength; i++)
                result.Add(_data[i] + other[i]);
            for (var i = minLength; i < _data.Count; i++)
                result.Add(_data[i]);
            for (var i = minLength; i < other.Count; i++)
                result.Add(other[i]);
        }
        return new PositionVector(result, _mod, _userRange, _rangeUpdate, _user);
    }

    public PositionVector ComponentwiseSubtraction(IReadOnlyList<int> other, bool useLooping = false)
    {
        if (other.Count == 0)
            return Clone();
        if (_data.Count == 0)
            return new PositionVector(other.ToList(), _mod, _userRange, _rangeUpdate, _user);
        List<int> result;
        if (useLooping)
        {
            var maxLength = Math.Max(_data.Count, other.Count);
            result = new List<int>(maxLength);
            for (var i = 0; i < maxLength; i++)
                result.Add(_data[i % _data.Count] - other[i % other.Count]);
        }
        else
        {
            var minLength = Math.Min(_data.Count, other.Count);
            result = new List<int>();
            for (var i = 0; i < minLength; i++)
                result.Add(_data[i] - other[i]);
            for (var i = minLength; i < _data.Count; i++)
                result.Add(_data[i]);
            for (var i = minLength; i < other.Count; i++)
                result.Add(other[i]);
        }
        return new PositionVector(result, _mod, _userRange, _rangeUpdate, _user);
    }

    public PositionVector ComponentwiseProduct(IReadOnlyList<int> other, bool useLooping = true)
    {
        if (other.Count == 0)
            return new PositionVector(new List<int>(), _mod, _userRange, _rangeUpdate, _user);
        if (_data.Count == 0)
            return Clone();
        List<int> result;
        if (useLooping)
        {
            var maxLength = Math.Max(_data.Count, other.Count);
            result = new List<int>(maxLength);
            for (var i = 0; i < maxLength; i++)
                result.Add(_data[i % _data.Count] * other[i % other.Count]);
        }
        else
        {
            var minLength = Math.Min(_data.Count, other.Count);
            result = new List<int>();
            for (var i = 0; i < minLength; i++)
                result.Add(_data[i] * other[i]);
            for (var i = minLength; i < _data.Count; i++)
                result.Add(_data[i]);
            for (var i = minLength; i < other.Count; i++)
                result.Add(other[i]);
        }
        return new PositionVector(result, _mod, _userRange, _rangeUpdate, _user);
    }

    public PositionVector ComponentwiseDivision(IReadOnlyList<int> other, bool useLooping = true)
    {
        if (other.Count == 0)
            throw new ArgumentException("Cannot divide by empty vector");
        if (_data.Count == 0)
            return Clone();
        foreach (var val in other)
            if (val == 0)
                throw new ArgumentException("Division by zero in componentwise division");
        List<int> result;
        if (useLooping)
        {
            var maxLength = Math.Max(_data.Count, other.Count);
            result = new List<int>(maxLength);
            for (var i = 0; i < maxLength; i++)
                result.Add(MathUtil.EuclideanDivision(_data[i % _data.Count], other[i % other.Count]).Quotient);
        }
        else
        {
            var minLength = Math.Min(_data.Count, other.Count);
            result = new List<int>();
            for (var i = 0; i < minLength; i++)
                result.Add(MathUtil.EuclideanDivision(_data[i], other[i]).Quotient);
            for (var i = minLength; i < _data.Count; i++)
                result.Add(_data[i]);
            for (var i = minLength; i < other.Count; i++)
                result.Add(other[i]);
        }
        return new PositionVector(result, _mod, _userRange, _rangeUpdate, _user);
    }

    public PositionVector ComponentwiseModulo(IReadOnlyList<int> other, bool useLooping = true)
    {
        if (other.Count == 0)
            throw new ArgumentException("Cannot compute modulo with empty vector");
        if (_data.Count == 0)
            return Clone();
        foreach (var val in other)
            if (val == 0)
                throw new ArgumentException("Division by zero in componentwise modulo");
        List<int> result;
        if (useLooping)
        {
            var maxLength = Math.Max(_data.Count, other.Count);
            result = new List<int>(maxLength);
            for (var i = 0; i < maxLength; i++)
                result.Add(MathUtil.EuclideanDivision(_data[i % _data.Count], other[i % other.Count]).Remainder);
        }
        else
        {
            var minLength = Math.Min(_data.Count, other.Count);
            result = new List<int>();
            for (var i = 0; i < minLength; i++)
                result.Add(MathUtil.EuclideanDivision(_data[i], other[i]).Remainder);
            for (var i = minLength; i < _data.Count; i++)
                result.Add(_data[i]);
            for (var i = minLength; i < other.Count; i++)
                result.Add(other[i]);
        }
        return new PositionVector(result, _mod, _userRange, _rangeUpdate, _user);
    }

    public PositionVector ScalarSum(int scalar) => this + scalar;
    public PositionVector ScalarProduct(int scalar) => this * scalar;

    public PositionVector Concatenate(PositionVector other)
    {
        var result = new List<int>(_data);
        result.AddRange(other.MutableData);
        return new PositionVector(result, _mod, _userRange, _rangeUpdate, _user);
    }

    public PositionVector Repeat(int times)
    {
        if (times <= 0)
            return new PositionVector(new List<int>(), _mod, _userRange, _rangeUpdate, _user);
        var result = new List<int>(_data.Count * times);
        for (var t = 0; t < times; t++)
            result.AddRange(_data);
        return new PositionVector(result, _mod, _userRange, _rangeUpdate, _user);
    }

    public PositionVector Resize(int start, int end)
    {
        if (_data.Count == 0)
            return Clone();
        var resized = new List<int>();
        if (start <= end)
        {
            for (var i = start; i <= end; i++)
                resized.Add(this[i]);
        }
        else
        {
            for (var i = start; i >= end; i--)
                resized.Add(this[i]);
        }
        return new PositionVector(resized, _mod, _userRange, _rangeUpdate, _user);
    }
}
