namespace Musicpp;

public static class Automations
{
    public static int GetMaxInterval(IReadOnlyList<int> scale)
    {
        if (scale.Count < 2)
            return 0;
        var maxInterval = 0;
        for (var i = 1; i < scale.Count; i++)
            maxInterval = Math.Max(maxInterval, Math.Abs(scale[i] - scale[i - 1]));
        return maxInterval;
    }

    public static PositionVector AutoScale(PositionVector scale, IReadOnlyList<int> notes)
    {
        var scaleData = scale.MutableData.ToList();
        var mod = scale.Mod;
        var pitchClasses = new List<int>();
        foreach (var note in notes)
        {
            var pc = note % mod;
            if (pc < 0)
                pc += mod;
            pitchClasses.Add(pc);
        }

        var used = new bool[scaleData.Count];

        foreach (var pc in pitchClasses)
        {
            var closest = -1;
            var minDist = mod;
            var bestMaxInterval = 999;

            for (var i = 0; i < scaleData.Count; i++)
            {
                if (used[i])
                    continue;

                var scalePc = scaleData[i] % mod;
                if (scalePc < 0)
                    scalePc += mod;
                var upDist = (pc - scalePc + mod) % mod;
                var downDist = (scalePc - pc + mod) % mod;
                var dist = Math.Min(upDist, downDist);

                if (dist < minDist)
                {
                    minDist = dist;
                    closest = i;
                    var tempScale = scaleData.ToList();
                    var octave = MathUtil.EuclideanDivision(tempScale[i], mod).Quotient;
                    tempScale[i] = octave * mod + pc;
                    bestMaxInterval = GetMaxInterval(tempScale);
                }
                else if (dist == minDist)
                {
                    var tempScale = scaleData.ToList();
                    var octave = MathUtil.EuclideanDivision(tempScale[i], mod).Quotient;
                    tempScale[i] = octave * mod + pc;
                    var maxInterval = GetMaxInterval(tempScale);

                    if (maxInterval < bestMaxInterval)
                    {
                        closest = i;
                        bestMaxInterval = maxInterval;
                    }
                    else if (maxInterval == bestMaxInterval && closest != -1)
                    {
                        var toEdge = Math.Min(i, scaleData.Count - 1 - i);
                        var closestToEdge = Math.Min(closest, scaleData.Count - 1 - closest);
                        if (toEdge < closestToEdge)
                        {
                            closest = i;
                            bestMaxInterval = maxInterval;
                        }
                    }
                }
            }

            if (closest != -1)
            {
                var octave = MathUtil.EuclideanDivision(scaleData[closest], mod).Quotient;
                scaleData[closest] = octave * mod + pc;
                used[closest] = true;
            }
        }

        return new PositionVector(scaleData, scale.Mod, scale.UserRange, scale.AutoRangeUpdates, scale.UseUserRange);
    }

    public static List<int> NormalizeComplexityVector(IReadOnlyList<int> complexities, int requiredSize)
    {
        var result = new List<int>(requiredSize);
        if (complexities.Count == 0)
        {
            for (var i = 0; i < requiredSize; i++)
                result.Add(0);
            return result;
        }

        if (complexities.Count == requiredSize)
            return complexities.ToList();
        if (complexities.Count < requiredSize)
        {
            for (var i = 0; i < requiredSize; i++)
                result.Add(complexities[i % complexities.Count]);
            return result;
        }

        return complexities.Take(requiredSize).ToList();
    }

    public static ModalRelativeModeMatrixRow DegreeAutomation(
        PositionVector scale,
        IntervalVector criterion,
        int degree,
        PositionVector reference,
        int complexity = 0)
    {
        var sel = MatrixOps.ModalSelection(scale, criterion, degree);
        var degrees = MatrixOps.ModalRelativeMode(sel);
        var distances = MatrixDistanceOps.CalculateDistances(reference, degrees);
        return distances.GetByComplexity(complexity);
    }

    public static RelativeModeMatrixRow VoiceLeadingAutomation(
        PositionVector reference,
        PositionVector target,
        int complexity = 0)
    {
        var center = MatrixDistanceOps.Align(reference, target);
        var positions = MatrixOps.RelativeModeMatrix(target, center);
        var distances = MatrixDistanceOps.CalculateDistances(reference, positions);
        return distances.GetByComplexity(complexity);
    }

    public static ModalMatrixRow<PositionVector> ModalInterchangeAutomation(
        PositionVector scale,
        IReadOnlyList<int> notes,
        int complexity)
    {
        var modes = MatrixOps.ModalMatrix(scale);
        var filter = MatrixOps.FilterModalMatrix(modes, notes);
        var distances = MatrixDistanceOps.CalculateDistances(scale, filter);
        return distances.GetByComplexity(complexity);
    }

    public static TranspositionMatrixRow ModulationAutomation(
        PositionVector scale,
        IReadOnlyList<int> notes,
        int complexity)
    {
        var transpositions = MatrixOps.TranspositionMatrix(scale);
        var filter = MatrixOps.FilterTranspositionMatrix(transpositions, notes);
        var distances = MatrixDistanceOps.CalculateDistances(scale, filter);
        return distances.GetByComplexity(complexity);
    }

    public static List<PositionVector> VoiceLeadingAutomationVectorReference(
        IReadOnlyList<PositionVector> targets,
        IReadOnlyList<PositionVector> references,
        IReadOnlyList<int>? complexities = null)
    {
        if (targets.Count != references.Count)
            throw new ArgumentException("targets and references must have the same size");
        complexities ??= Array.Empty<int>();
        var normalized = NormalizeComplexityVector(complexities, targets.Count);
        var result = new List<PositionVector>();
        for (var i = 0; i < targets.Count; i++)
        {
            var row = VoiceLeadingAutomation(references[i], targets[i], normalized[i]);
            result.Add(row.Vector);
        }
        return result;
    }

    public static List<PositionVector> VoiceLeadingAutomationReference(
        IReadOnlyList<PositionVector> targets,
        PositionVector reference,
        IReadOnlyList<int>? complexities = null)
    {
        complexities ??= Array.Empty<int>();
        var normalized = NormalizeComplexityVector(complexities, targets.Count);
        var result = new List<PositionVector>();
        for (var i = 0; i < targets.Count; i++)
        {
            var row = VoiceLeadingAutomation(reference, targets[i], normalized[i]);
            result.Add(row.Vector);
        }
        return result;
    }

    public static List<PositionVector> ForwardVoiceLeading(
        IReadOnlyList<PositionVector> targets,
        IReadOnlyList<int>? complexities = null)
    {
        if (targets.Count == 0)
            throw new ArgumentException("targets vector cannot be empty");
        var result = new List<PositionVector> { targets[0].Copy() };
        if (targets.Count == 1)
            return result;
        complexities ??= Array.Empty<int>();
        var normalized = NormalizeComplexityVector(complexities, targets.Count - 1);
        for (var i = 1; i < targets.Count; i++)
        {
            var target = targets[i].Copy();
            var reference = result[i - 1];
            var row = VoiceLeadingAutomation(reference, target, normalized[i - 1]);
            result.Add(row.Vector);
        }
        return result;
    }

    public static List<PositionVector> VoiceLeadingAutomationSequentialBackward(
        IReadOnlyList<PositionVector> targets,
        IReadOnlyList<int>? complexities = null)
    {
        if (targets.Count == 0)
            throw new ArgumentException("targets vector cannot be empty");
        var result = new PositionVector[targets.Count];
        result[^1] = targets[^1].Copy();
        if (targets.Count == 1)
            return result.ToList();
        complexities ??= Array.Empty<int>();
        var normalized = NormalizeComplexityVector(complexities, targets.Count - 1);
        for (var i = targets.Count - 2; i >= 0; i--)
        {
            var target = targets[i].Copy();
            var reference = result[i + 1];
            var row = VoiceLeadingAutomation(reference, target, normalized[i]);
            result[i] = row.Vector;
        }
        return result.ToList();
    }

    public static List<PositionVector> DegreeAutomationReference(
        PositionVector scale,
        IntervalVector criterion,
        IReadOnlyList<int> degrees,
        PositionVector reference,
        IReadOnlyList<int>? complexities = null)
    {
        complexities ??= Array.Empty<int>();
        var normalized = NormalizeComplexityVector(complexities, degrees.Count);
        var result = new List<PositionVector>();
        for (var i = 0; i < degrees.Count; i++)
        {
            var row = DegreeAutomation(scale, criterion, degrees[i], reference, normalized[i]);
            result.Add(row.Vector);
        }
        return result;
    }

    public static List<PositionVector> DegreeAutomationVectorReference(
        PositionVector scale,
        IntervalVector criterion,
        IReadOnlyList<int> degrees,
        IReadOnlyList<PositionVector> references,
        IReadOnlyList<int>? complexities = null)
    {
        if (degrees.Count != references.Count)
            throw new ArgumentException("degrees and references must have the same size");
        complexities ??= Array.Empty<int>();
        var normalized = NormalizeComplexityVector(complexities, degrees.Count);
        var result = new List<PositionVector>();
        for (var i = 0; i < degrees.Count; i++)
        {
            var row = DegreeAutomation(scale, criterion, degrees[i], references[i], normalized[i]);
            result.Add(row.Vector);
        }
        return result;
    }

    public static List<PositionVector> ForwardDegreeAutomation(
        PositionVector scale,
        IntervalVector criterion,
        IReadOnlyList<int> degrees,
        PositionVector initialReference,
        IReadOnlyList<int>? complexities = null)
    {
        if (degrees.Count == 0)
            throw new ArgumentException("degrees vector cannot be empty");
        complexities ??= Array.Empty<int>();
        var normalized = NormalizeComplexityVector(complexities, degrees.Count);
        var result = new List<PositionVector>();
        var first = DegreeAutomation(scale, criterion, degrees[0], initialReference, normalized[0]);
        result.Add(first.Vector);
        for (var i = 1; i < degrees.Count; i++)
        {
            var row = DegreeAutomation(scale, criterion, degrees[i], result[i - 1], normalized[i]);
            result.Add(row.Vector);
        }
        return result;
    }

    public static List<PositionVector> DegreeAutomationSequentialBackward(
        PositionVector scale,
        IntervalVector criterion,
        IReadOnlyList<int> degrees,
        PositionVector finalReference,
        IReadOnlyList<int>? complexities = null)
    {
        if (degrees.Count == 0)
            throw new ArgumentException("degrees vector cannot be empty");
        complexities ??= Array.Empty<int>();
        var normalized = NormalizeComplexityVector(complexities, degrees.Count);
        var result = new PositionVector[degrees.Count];
        var last = DegreeAutomation(scale, criterion, degrees[^1], finalReference, normalized[^1]);
        result[^1] = last.Vector;
        for (var i = degrees.Count - 2; i >= 0; i--)
        {
            var row = DegreeAutomation(scale, criterion, degrees[i], result[i + 1], normalized[i]);
            result[i] = row.Vector;
        }
        return result.ToList();
    }
}
