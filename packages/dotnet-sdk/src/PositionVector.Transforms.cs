namespace Musicpp;

public sealed partial class PositionVector
{
    public PositionVector Rotate(int rotationAmount)
    {
        if (_data.Count == 0)
            return Clone();
        var rotated = new int[_data.Count];
        var absRotation = Math.Abs(rotationAmount);
        var size = _data.Count;
        for (var i = 0; i < size; i++)
        {
            var newPosition = (i + absRotation) % size;
            rotated[newPosition] = this[i];
        }
        return new PositionVector(rotated.ToList(), _mod, _userRange, _rangeUpdate, _user);
    }

    public PositionVector RelativeMode(int startOffset, int length = 0)
    {
        var outLength = length == 0 ? _data.Count : Math.Abs(length);
        var newData = new List<int>(outLength);
        for (var i = 0; i < outLength; i++)
            newData.Add(this[startOffset + i]);
        return new PositionVector(newData, _mod, _userRange, _rangeUpdate, _user);
    }

    public PositionVector Complement()
    {
        var effectiveRange = EffectiveRange;
        if (_data.Count == 0)
        {
            var universe = Enumerable.Range(0, effectiveRange).ToList();
            return new PositionVector(universe, _mod, _userRange, _rangeUpdate, _user);
        }
        var minValue = _data.Min();
        var normalized = this - minValue;
        var normalizedSet = new HashSet<int>(normalized.MutableData);
        var complement = new List<int>();
        for (var i = 0; i < effectiveRange; i++)
            if (!normalizedSet.Contains(i))
                complement.Add(i);
        return new PositionVector(complement, _mod, _userRange, _rangeUpdate, _user) + minValue;
    }

    public PositionVector Inversion(int axisIndex, bool sortOutput = true)
    {
        if (_data.Count == 0)
            return Clone();
        var size = _data.Count;
        var normalizedAxis = MathUtil.EuclideanDivision(axisIndex, size).Remainder;
        var axisValue = _data[normalizedAxis];
        var inverted = _data.Select(x => 2 * axisValue - x).ToList();
        if (sortOutput)
            inverted.Sort();
        return new PositionVector(inverted, _mod, _userRange, _rangeUpdate, _user);
    }

    public PositionVector Negative(int axis = 10, bool standard = true, bool sortResult = true)
    {
        var result = Clone();
        var adjustedPosition = axis;
        if (standard)
        {
            for (var i = 0; i < result.MutableData.Count; i++)
                result.MutableData[i] *= 2;
            adjustedPosition = axis * 2 - 1;
        }
        for (var i = 0; i < result.MutableData.Count; i++)
            result.MutableData[i] -= adjustedPosition;
        for (var i = 0; i < result.MutableData.Count; i++)
            result.MutableData[i] *= -1;
        for (var i = 0; i < result.MutableData.Count; i++)
            result.MutableData[i] += adjustedPosition;
        if (standard)
        {
            for (var i = 0; i < result.MutableData.Count; i++)
                result.MutableData[i] /= 2;
        }
        if (sortResult)
            result.MutableData.Sort();
        return result.RelativeMode(-1);
    }
}
