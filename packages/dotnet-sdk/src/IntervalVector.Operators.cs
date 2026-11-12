namespace Musicpp;

public sealed partial class IntervalVector
{
    public static IntervalVector operator +(IntervalVector iv, int scalar)
    {
        var r = iv.MutableData.Select(x => x + scalar).ToList();
        return new IntervalVector(r, iv._offset, iv._mod);
    }

    public static IntervalVector operator +(int scalar, IntervalVector iv) => iv + scalar;

    public static IntervalVector operator -(IntervalVector iv, int scalar)
    {
        var r = iv.MutableData.Select(x => x - scalar).ToList();
        return new IntervalVector(r, iv._offset, iv._mod);
    }

    public static IntervalVector operator -(int scalar, IntervalVector iv)
    {
        var r = iv.MutableData.Select(x => scalar - x).ToList();
        return new IntervalVector(r, iv._offset, iv._mod);
    }

    public static IntervalVector operator *(IntervalVector iv, int scalar)
    {
        var r = iv.MutableData.Select(x => x * scalar).ToList();
        return new IntervalVector(r, iv._offset, iv._mod);
    }

    public static IntervalVector operator *(int scalar, IntervalVector iv) => iv * scalar;

    public static IntervalVector operator /(IntervalVector iv, int divisor)
    {
        if (divisor == 0)
            throw new ArgumentException("Division by zero");
        var r = iv.MutableData.Select(x => MathUtil.EuclideanDivision(x, divisor).Quotient).ToList();
        return new IntervalVector(r, iv._offset, iv._mod);
    }

    public static IntervalVector operator %(IntervalVector iv, int divisor)
    {
        if (divisor == 0)
            throw new ArgumentException("Division by zero");
        var r = iv.MutableData.Select(x => MathUtil.EuclideanDivision(x, divisor).Remainder).ToList();
        return new IntervalVector(r, iv._offset, iv._mod);
    }

    public static IntervalVector operator +(IntervalVector a, IntervalVector b) => a.ComponentwiseSum(b.MutableData, false);
    public static IntervalVector operator -(IntervalVector a, IntervalVector b) => a.ComponentwiseSubtraction(b.MutableData, false);
    public static IntervalVector operator *(IntervalVector a, IntervalVector b) => a.ComponentwiseProduct(b.MutableData, true);
    public static IntervalVector operator /(IntervalVector a, IntervalVector b) => a.ComponentwiseDivision(b.MutableData, true);
    public static IntervalVector operator %(IntervalVector a, IntervalVector b) => a.ComponentwiseModulo(b.MutableData, true);

    public static IntervalVector operator +(IntervalVector a, IReadOnlyList<int> b) => a.ComponentwiseSum(b, false);
    public static IntervalVector operator -(IntervalVector a, IReadOnlyList<int> b) => a.ComponentwiseSubtraction(b, false);
    public static IntervalVector operator *(IntervalVector a, IReadOnlyList<int> b) => a.ComponentwiseProduct(b, true);
    public static IntervalVector operator /(IntervalVector a, IReadOnlyList<int> b) => a.ComponentwiseDivision(b, true);
    public static IntervalVector operator %(IntervalVector a, IReadOnlyList<int> b) => a.ComponentwiseModulo(b, true);

    public IntervalVector ComponentwiseSum(IReadOnlyList<int> other, bool useLooping = false)
    {
        if (other.Count == 0)
            return Clone();
        if (_data.Count == 0)
            return new IntervalVector(other.ToList(), _offset, _mod);
        return ComponentwiseCore(other, useLooping, (x, y) => x + y);
    }

    public IntervalVector ComponentwiseSubtraction(IReadOnlyList<int> other, bool useLooping = false)
    {
        if (other.Count == 0)
            return Clone();
        if (_data.Count == 0)
            return new IntervalVector(other.ToList(), _offset, _mod);
        return ComponentwiseCore(other, useLooping, (x, y) => x - y);
    }

    public IntervalVector ComponentwiseProduct(IReadOnlyList<int> other, bool useLooping = true)
    {
        if (other.Count == 0)
            return new IntervalVector(new List<int>(), _offset, _mod);
        if (_data.Count == 0)
            return Clone();
        return ComponentwiseCore(other, useLooping, (x, y) => x * y);
    }

    private IntervalVector ComponentwiseCore(IReadOnlyList<int> other, bool useLooping, Func<int, int, int> op)
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
        return new IntervalVector(result, _offset, _mod);
    }

    public IntervalVector ComponentwiseDivision(IReadOnlyList<int> other, bool useLooping = true)
    {
        if (other.Count == 0)
            throw new ArgumentException("Cannot divide by empty vector");
        if (_data.Count == 0)
            return Clone();
        foreach (var v in other)
            if (v == 0)
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
        return new IntervalVector(result, _offset, _mod);
    }

    public IntervalVector ComponentwiseModulo(IReadOnlyList<int> other, bool useLooping = true)
    {
        if (other.Count == 0)
            throw new ArgumentException("Cannot compute modulo with empty vector");
        if (_data.Count == 0)
            return Clone();
        foreach (var v in other)
            if (v == 0)
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
        return new IntervalVector(result, _offset, _mod);
    }

    public IntervalVector Concatenate(IntervalVector other)
    {
        var r = new List<int>(_data);
        r.AddRange(other.MutableData);
        return new IntervalVector(r, _offset, _mod);
    }

    public IntervalVector Repeat(int times)
    {
        if (times <= 0)
            return new IntervalVector(new List<int>(), _offset, _mod);
        var r = new List<int>(_data.Count * times);
        for (var t = 0; t < times; t++)
            r.AddRange(_data);
        return new IntervalVector(r, _offset, _mod);
    }
}
