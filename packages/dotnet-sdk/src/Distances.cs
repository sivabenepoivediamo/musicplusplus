namespace Musicpp;

public static class Distances
{
    public static List<double> Normalize(IReadOnlyList<int> input)
    {
        var sum = input.Sum(x => (double)x);
        if (sum == 0)
            throw new ArgumentException("Sum of vector elements is zero, cannot normalize");
        return input.Select(v => v / sum).ToList();
    }

    public static List<double> ComputeCdf(List<double> pdf)
    {
        var cdf = new List<double>(pdf.Count);
        double acc = 0;
        foreach (var p in pdf)
        {
            acc += p;
            cdf.Add(acc);
        }
        return cdf;
    }

    public static double EuclideanDistance(IReadOnlyList<int> v1, IReadOnlyList<int> v2)
    {
        var length = Math.Min(v1.Count, v2.Count);
        double sum = 0;
        for (var i = 0; i < length; i++)
        {
            var d = v1[i] - v2[i];
            sum += d * d;
        }
        return Math.Sqrt(sum);
    }

    public static int EditDistance(List<int> v1, List<int> v2)
    {
        var n = v1.Count;
        var m = v2.Count;
        var dp = new int[n + 1, m + 1];
        for (var i = 0; i <= n; i++) dp[i, 0] = i;
        for (var j = 0; j <= m; j++) dp[0, j] = j;
        for (var i = 1; i <= n; i++)
            for (var j = 1; j <= m; j++)
            {
                if (v1[i - 1] == v2[j - 1])
                    dp[i, j] = dp[i - 1, j - 1];
                else
                    dp[i, j] = 1 + Math.Min(Math.Min(dp[i - 1, j], dp[i, j - 1]), dp[i - 1, j - 1]);
            }
        return dp[n, m];
    }

    public static int HammingDistance(List<int> v1, List<int> v2)
    {
        var length = Math.Min(v1.Count, v2.Count);
        var d = 0;
        for (var i = 0; i < length; i++)
            if (v1[i] != v2[i])
                d++;
        return d;
    }

    public static int ManhattanDistance(List<int> v1, List<int> v2)
    {
        var length = Math.Min(v1.Count, v2.Count);
        var sum = 0;
        for (var i = 0; i < length; i++)
            sum += Math.Abs(v1[i] - v2[i]);
        return sum;
    }

    public static int Difference(List<int> v1, List<int> v2)
    {
        var length = Math.Min(v1.Count, v2.Count);
        var diff = 0;
        for (var i = 0; i < length; i++)
            diff += v1[i] - v2[i];
        return diff;
    }

    public static double VariationDistance(List<int> a, List<int> b)
    {
        if (a.Count == 0 && b.Count == 0)
            return 0;
        if (a.Count == 0 || b.Count == 0)
            throw new ArgumentException("Empty vector, distance calculation is impossible.");
        a = a.OrderBy(x => x).ToList();
        b = b.OrderBy(x => x).ToList();
        var minLen = Math.Min(a.Count, b.Count);
        var maxLen = Math.Max(a.Count, b.Count);
        double total = 0;
        for (var i = 0; i < minLen; i++)
            total += Math.Abs(a[i] - b[i]);
        var longer = a.Count >= b.Count ? a : b;
        var shorter = a.Count >= b.Count ? b : a;
        for (var i = minLen; i < maxLen; i++)
        {
            var extra = longer[i];
            var minDist = int.MaxValue;
            var maxDist = int.MinValue;
            foreach (var x in shorter)
            {
                var d = Math.Abs(extra - x);
                minDist = Math.Min(minDist, d);
                maxDist = Math.Max(maxDist, d);
            }
            total += (minDist + maxDist) / 2.0;
        }
        return total;
    }

    public static List<int> GeneralizedNeoRiemann(IReadOnlyList<int> input, int position, int shift)
    {
        var output = input.ToList();
        if (position >= 0 && position < output.Count)
            output[position] += shift;
        return output;
    }

    public static List<(int Type, int Pos, int Val)> TransformationSteps(IReadOnlyList<int> start, IReadOnlyList<int> end)
    {
        var steps = new List<(int, int, int)>();
        var startLength = start.Count;
        var endLength = end.Count;
        var minLength = Math.Min(startLength, endLength);
        var addedPosition = startLength;
        for (var i = 0; i < minLength; i++)
        {
            var diff = end[i] - start[i];
            if (diff != 0)
            {
                steps.Add((0, i, diff));
                var transformed = GeneralizedNeoRiemann(start.ToList(), i, diff);
                steps.AddRange(TransformationSteps(transformed, end));
                return steps;
            }
        }
        if (endLength > startLength)
        {
            for (var i = minLength; i < endLength; i++)
                steps.Add((1, addedPosition++, end[i]));
        }
        if (endLength < startLength)
        {
            for (var i = minLength; i < startLength; i++)
                steps.Add((2, i, start[i]));
        }
        return steps;
    }

    public static int WeightedTransformationDistance(List<int> start, List<int> end)
    {
        var steps = TransformationSteps(start, end);
        return steps.Sum(s => Math.Abs(s.Val));
    }

    public static double EuclideanDistance(PositionVector a, PositionVector b) =>
        EuclideanDistance(a.MutableData, b.MutableData);

    public static int ManhattanDistance(PositionVector a, PositionVector b) =>
        ManhattanDistance(a.MutableData, b.MutableData);

    public static int EditDistance(PositionVector a, PositionVector b) =>
        EditDistance(a.MutableData.ToList(), b.MutableData.ToList());

    public static int WeightedTransformationDistance(PositionVector a, PositionVector b) =>
        WeightedTransformationDistance(a.MutableData.ToList(), b.MutableData.ToList());

    public static int Difference(PositionVector a, PositionVector b) =>
        Difference(a.MutableData, b.MutableData);

    public static int HammingDistance(PositionVector a, PositionVector b) =>
        HammingDistance(a.MutableData, b.MutableData);

    public static int Difference(IntervalVector a, IntervalVector b) =>
        Difference(a.MutableData, b.MutableData);

    public static int HammingDistance(IntervalVector a, IntervalVector b) =>
        HammingDistance(a.MutableData, b.MutableData);

    public static int ManhattanDistance(IntervalVector a, IntervalVector b) =>
        ManhattanDistance(a.MutableData, b.MutableData);

    public static double EuclideanDistance(IntervalVector a, IntervalVector b) =>
        EuclideanDistance(a.MutableData, b.MutableData);

    public static int EditDistance(IntervalVector a, IntervalVector b) =>
        EditDistance(a.MutableData.ToList(), b.MutableData.ToList());

    public static int WeightedTransformationDistance(IntervalVector a, IntervalVector b) =>
        WeightedTransformationDistance(a.MutableData.ToList(), b.MutableData.ToList());

    public static double VariationDistance(PositionVector a, PositionVector b) =>
        VariationDistance(a.MutableData.ToList(), b.MutableData.ToList());
}
