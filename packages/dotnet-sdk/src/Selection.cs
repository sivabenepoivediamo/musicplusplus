namespace Musicpp;

public static class Selection
{
    public static PositionVector Select(PositionVector source, PositionVector criterion,
        int criterionRelativeMode = 0, int voices = 0)
    {
        var sourceData = source.MutableData.ToList();
        var criterionModulo = sourceData.Count;
        var actualCriterion = new PositionVector(criterion.MutableData.ToList(), criterionModulo);
        actualCriterion.SetMod(source.Count);
        var steppedCriterion = criterionRelativeMode != 0
            ? actualCriterion.RelativeMode(criterionRelativeMode, voices)
            : actualCriterion;
        var outLength = voices > 0 ? voices : steppedCriterion.Count;
        var result = new List<int>(outLength);
        for (var k = 0; k < outLength; k++)
            result.Add(source[steppedCriterion[k]]);
        return new PositionVector(result, source.Mod, source.UserRange, source.AutoRangeUpdates, source.UseUserRange);
    }

    public static PositionVector Select(PositionVector source, IntervalVector criterion,
        int criterionParallelMode = 0, int voices = 0)
    {
        var actualCriterion = criterion.Clone();
        actualCriterion.SetMod(source.Count);
        var steppedCriterion = criterionParallelMode != 0
            ? actualCriterion.ParallelMode(criterionParallelMode, voices)
            : actualCriterion;
        var outLength = voices > 0 ? voices : steppedCriterion.Count;
        var result = new List<int>(outLength);
        var cumulativePosition = steppedCriterion.Offset;
        for (var k = 0; k < outLength; k++)
        {
            result.Add(source[cumulativePosition]);
            cumulativePosition += steppedCriterion[k];
        }
        return new PositionVector(result, source.Mod, source.UserRange, source.AutoRangeUpdates, source.UseUserRange);
    }

    public static IntervalVector Select(IntervalVector source, IntervalVector indices,
        int criterionParallelMode = 0, int voices = 0)
    {
        var actualCriterion = indices.Clone();
        actualCriterion.SetMod(source.Count);
        var steppedCriterion = criterionParallelMode != 0
            ? actualCriterion.ParallelMode(criterionParallelMode, voices)
            : actualCriterion;
        var sourceOffset = source.Offset;
        var criterionOffset = steppedCriterion.Offset;
        var sOut = sourceOffset;
        for (var j = 0; j < criterionOffset; j++)
            sOut += source[j];
        var outLength = voices > 0 ? voices : steppedCriterion.Count;
        var result = new List<int>(outLength);
        var cumulativeIndex = criterionOffset;
        for (var k = 0; k < outLength; k++)
        {
            var spanLength = steppedCriterion[k];
            var sum = 0;
            for (var j = 0; j < spanLength; j++)
                sum += source[cumulativeIndex + j];
            result.Add(sum);
            cumulativeIndex += spanLength;
        }
        return new IntervalVector(result, sOut, source.Mod);
    }

    public static IntervalVector Select(IntervalVector source, PositionVector criterion,
        int criterionRelativeMode = 0, int voices = 0)
    {
        var off = source.Offset;
        var actualCriterion = criterion.Clone();
        actualCriterion.SetMod(source.Count);
        var steppedCriterion = criterionRelativeMode != 0
            ? actualCriterion.RelativeMode(criterionRelativeMode, voices)
            : actualCriterion;
        if (steppedCriterion.Count == 0)
            return new IntervalVector(new List<int>(), source.Offset, source.Mod);
        var outLength = voices > 0 ? voices : steppedCriterion.Count;
        var result = new List<int>(outLength);
        var n = source.Count;
        for (var k = 0; k < outLength; k++)
        {
            var pK = steppedCriterion[k];
            var pNext = steppedCriterion[k + 1];
            var deltaK = pNext - pK;
            if (deltaK <= 0)
                deltaK += n;
            var sum = 0;
            for (var j = 0; j < deltaK; j++)
                sum += source[pK + j];
            result.Add(sum);
        }
        var sOut = off;
        if (criterion[0] >= 0)
        {
            for (var j = 0; j < criterion[0]; j++)
                sOut += source[j];
        }
        else
        {
            for (var j = criterion[0]; j < 0; j++)
                sOut -= source[j];
        }
        return new IntervalVector(result, sOut, source.Mod);
    }
}
