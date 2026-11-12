namespace Musicpp;

public sealed partial class IntervalVector
{
    public IntervalVector ParallelMode(int r, int n = 0)
    {
        n = Math.Abs(n);
        if (n == 0)
            n = _data.Count;
        var o = new List<int>(n);
        for (var i = 0; i < n; i++)
            o.Add(Element(r + i));
        return new IntervalVector(o, _offset, _mod);
    }

    public IntervalVector RelativeMode(int r, int n = 0)
    {
        if (_data.Count == 0)
            return new IntervalVector(new List<int>(), _offset, _mod);
        n = Math.Abs(n);
        var dataSize = _data.Count;
        if (n == 0)
            n = dataSize;
        var o = new List<int>(n);
        for (var i = 0; i < n; i++)
            o.Add(Element(r + i));
        var sum = 0;
        if (Math.Abs(r) < dataSize)
        {
            if (r >= 0)
            {
                for (var i = 0; i < r; i++)
                    sum += Element(i);
            }
            else
            {
                for (var i = 0; i < -r; i++)
                    sum -= Element(dataSize - 1 - i);
            }
        }
        else
        {
            var div = MathUtil.EuclideanDivision(r, dataSize);
            if (r >= 0)
            {
                for (var i = 0; i < dataSize; i++)
                {
                    var mult = i < div.Remainder ? div.Quotient + 1 : div.Quotient;
                    sum += Element(i) * mult;
                }
            }
            else
            {
                var thresh = dataSize + div.Remainder;
                for (var i = 0; i < dataSize; i++)
                {
                    var mult = i >= thresh ? div.Quotient - 1 : div.Quotient;
                    sum += Element(i) * mult;
                }
            }
        }
        return new IntervalVector(o, _offset + sum, _mod);
    }

    public IntervalVector Reverse()
    {
        var o = new List<int>(_data.Count);
        for (var i = 0; i < _data.Count; i++)
            o.Add(_data[_data.Count - 1 - i]);
        return new IntervalVector(o, _offset, _mod);
    }

    public IntervalVector Retrograde() => Reverse();

    public IntervalVector Inversion(int axisIndex = 0)
    {
        if (_data.Count == 0)
            return Clone();
        var size = _data.Count;
        var div = MathUtil.EuclideanDivision(axisIndex, size + 1);
        var normalizedAxis = div.Remainder;
        var result = new List<int>(_data);
        for (var i = 0; i < normalizedAxis / 2; i++)
            (result[i], result[normalizedAxis - 1 - i]) = (result[normalizedAxis - 1 - i], result[i]);
        var start = normalizedAxis;
        var end = size;
        var len = end - start;
        for (var i = 0; i < len / 2; i++)
            (result[start + i], result[end - 1 - i]) = (result[end - 1 - i], result[start + i]);
        return new IntervalVector(result, _offset, _mod);
    }

    public IntervalVector Negate()
    {
        var r = _data.Select(x => -x).ToList();
        return new IntervalVector(r, -_offset, _mod);
    }

    public IntervalVector Normalize(int modulo = 0)
    {
        if (modulo == 0)
            modulo = _mod;
        if (modulo == 0)
            return Clone();
        var r = _data.Select(x => MathUtil.EuclideanDivision(x, modulo).Remainder).ToList();
        return new IntervalVector(r, _offset, _mod);
    }

    public IntervalVector SingleMirror(int position, bool left)
    {
        var o = new List<int>(_data);
        var length = o.Count;
        if (position < 0 || position > length)
            return Clone();
        if (left)
        {
            for (var i = 0; i < position / 2; i++)
                (o[i], o[position - 1 - i]) = (o[position - 1 - i], o[i]);
        }
        else
        {
            var end = position + (length - position) / 2;
            for (var i = position; i < end; i++)
                (o[i], o[length - 1 - (i - position)]) = (o[length - 1 - (i - position)], o[i]);
        }
        return new IntervalVector(o, _offset, _mod);
    }

    public IntervalVector DoubleMirror(int position)
    {
        var o = new List<int>(_data);
        var length = o.Count;
        if (position < 0 || position > length)
            return Clone();
        for (var i = 0; i < position / 2; i++)
            (o[i], o[position - 1 - i]) = (o[position - 1 - i], o[i]);
        var end = position + (length - position) / 2;
        for (var i = position; i < end; i++)
            (o[i], o[length - 1 - (i - position)]) = (o[length - 1 - (i - position)], o[i]);
        return new IntervalVector(o, _offset, _mod);
    }

    public IntervalVector CrossMirror(int position, bool left)
    {
        var o = new List<int>(_data);
        var n = _data.Count;
        if (n == 0 || position < 0 || position >= n)
            return new IntervalVector(o, _offset, _mod);
        if (left)
        {
            for (var i = 0; i < position && i < n; i++)
                o[n - 1 - i] = _data[i];
        }
        else
        {
            for (var i = position; i < n; i++)
                o[i - position] = _data[n - 1 - (i - position)];
        }
        return new IntervalVector(o, _offset, _mod);
    }
}
