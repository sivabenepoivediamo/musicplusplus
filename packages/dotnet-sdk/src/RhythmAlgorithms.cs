namespace Musicpp;

public static class RhythmAlgorithms
{
    public static List<int> EuclideanIntervals(int steps, int events)
    {
        if (steps <= 0 || events <= 0)
            throw new ArgumentException("euclidean: steps and events must be positive");
        var div = MathUtil.EuclideanDivision(steps, events);
        if (div.Remainder == 0)
            return Enumerable.Repeat(div.Quotient, events).ToList();
        var a = div.Remainder;
        var x = EuclideanIntervals(events, a);
        var o = new List<int>();
        for (var i = 0; i < a; i++)
        {
            for (var j = 0; j < x[i] - 1; j++)
                o.Add(div.Quotient);
            o.Add(div.Quotient + 1);
        }
        return o;
    }

    public static List<int> CloughDouthettPositions(int steps, int events)
    {
        var o = new List<int>(events);
        for (var i = 0; i < events; i++)
            o.Add((int)Math.Floor(i * steps / (double)events));
        return o;
    }

    public static List<int> DeepRhythmPositions(int steps, int events, int multiplicity)
    {
        var o = new List<int>(events);
        for (var i = 0; i < events; i++)
            o.Add((i * multiplicity) % steps);
        o.Sort();
        return o;
    }

    public static PositionVector CloughDouthettVector(int steps, int events, int offset)
    {
        var data = CloughDouthettPositions(steps, events);
        var pv = new PositionVector(data, steps);
        return pv + offset;
    }

    public static IntervalVector EuclideanAsIntervalVector(int steps, int events, int offset) =>
        new IntervalVector(EuclideanIntervals(steps, events), offset, steps);

    public static PositionVector DeepRhythmVector(int steps, int events, int multiplicity, int offset)
    {
        var data = DeepRhythmPositions(steps, events, multiplicity);
        var pv = new PositionVector(data, steps);
        return pv + offset;
    }

    public static int PhraseLength(int e, int c, int n, int s, int l) => (e + c * n - s) * l;

    public static (int Bols, int Dams) TihaiGenerator(int steps, int repetitions)
    {
        var length = steps;
        while (length % repetitions != 0)
            length++;
        var dams = length - steps;
        var bols = length / repetitions - dams;
        return (bols, dams);
    }

    public static List<int> TihaiReader(int b, int d, int m, int steps)
    {
        var o = new List<int>();
        for (var i = 0; i < b; i++)
            o.Add(1);
        for (var i = 0; i < d; i++)
            o.Add(0);
        for (var i = 0; i < m - 2; i++)
        {
            for (var j = 0; j < b; j++)
                o.Add(1);
            for (var j = 0; j < d; j++)
                o.Add(0);
        }
        for (var i = 0; i < b; i++)
            o.Add(1);
        return o;
    }

    public static bool IsAllZeros(IReadOnlyList<int> vec) => vec.All(x => x == 0);
    public static bool IsAllOnes(IReadOnlyList<int> vec) => vec.All(x => x == 1);

    public static void AppendOnes(List<int> vec, int targetSize)
    {
        while (vec.Count < targetSize)
            vec.Add(1);
    }

    public static List<int> Cut(IReadOnlyList<int> vec, int length)
    {
        if (length >= vec.Count)
            return vec.ToList();
        return vec.Take(length).ToList();
    }

    public static List<int> Tihai(int steps, int repetitions, bool pseudo)
    {
        if (steps <= 2)
            return Enumerable.Repeat(1, steps).ToList();
        if (repetitions == 1)
            return Enumerable.Repeat(1, steps).ToList();
        if (repetitions <= 0)
            return Enumerable.Repeat(0, steps).ToList();
        var (bols, dams) = TihaiGenerator(steps, repetitions);
        var pattern = TihaiReader(bols, dams, repetitions, steps);
        if (((IsAllZeros(pattern) && pseudo) || IsAllOnes(pattern)) && pseudo)
        {
            var shorter = Tihai(steps - 1, repetitions, pseudo);
            AppendOnes(shorter, steps);
            return shorter;
        }
        return Cut(pattern, steps);
    }

    public static OnsetVector TihaiOnset(int steps, int repetitions, bool pseudo, int offset) =>
        new OnsetVector(Tihai(steps, repetitions, pseudo), offset, steps);
}
