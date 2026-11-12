namespace Musicpp;

public readonly record struct ScaleMatch(string SheetName, string ScaleName, IReadOnlyList<int> Intervals);

public sealed partial class ScaleDatabase
{
    private readonly List<ScaleMatch> _scales = new();

    public ScaleDatabase() => ScaleDictionaryData.AddAllScales(this);

    internal void AddScaleInternal(string sheetName, string scaleName, int[] intervals) =>
        _scales.Add(new ScaleMatch(sheetName, scaleName, intervals.ToList()));

    public List<ScaleMatch> FindScale(IReadOnlyList<int> inputIntervals)
    {
        var results = new List<ScaleMatch>();
        if (inputIntervals.Count == 0)
            return results;
        var root = inputIntervals[0];
        var normalized = inputIntervals.Select(i => i - root).ToList();
        var processed = normalized.Distinct().OrderBy(x => x).ToList();
        foreach (var scale in _scales)
        {
            var sortedScale = scale.Intervals.OrderBy(x => x).ToList();
            if (processed.SequenceEqual(sortedScale))
                results.Add(scale);
        }
        return results;
    }

    public HashSet<List<int>> GetAllIntervalSets()
    {
        var unique = new HashSet<List<int>>(new SequenceComparer());
        foreach (var scale in _scales)
            unique.Add(scale.Intervals.OrderBy(x => x).ToList());
        return unique;
    }

    public void DisplayResults(IReadOnlyList<int> inputIntervals, string rootNote = "C")
    {
        var found = FindScale(inputIntervals);
        Console.WriteLine();
        Console.Write("Input notes: ");
        Console.WriteLine(string.Join(" ", inputIntervals));
        if (found.Count == 0)
        {
            Console.WriteLine("No matching scale found in database.");
            return;
        }
        Console.WriteLine($"Found {found.Count} matching scale(s):");
        Console.WriteLine("=================================");
        foreach (var scale in found)
        {
            Console.WriteLine("Category: " + scale.SheetName);
            Console.WriteLine("Scale: " + rootNote + " " + scale.ScaleName);
            Console.Write("Pitch Classes: ");
            Console.WriteLine(string.Join(" ", scale.Intervals));
            Console.WriteLine();
        }
    }

    private sealed class SequenceComparer : IEqualityComparer<List<int>>
    {
        public bool Equals(List<int>? x, List<int>? y) =>
            x is not null && y is not null && x.SequenceEqual(y);
        public int GetHashCode(List<int> obj) => string.Join(",", obj).GetHashCode();
    }
}

public static class ScaleDictionaryUtil
{
    public static List<int> ParseInput(string input) =>
        string.IsNullOrWhiteSpace(input)
            ? new List<int>()
            : input.Split((char[]?)null, StringSplitOptions.RemoveEmptyEntries)
                .Select(int.Parse).ToList();

    public static string GetRootNote(IReadOnlyList<int> intervals)
    {
        if (intervals.Count == 0)
            return "C";
        var names = new[] { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        var root = intervals[0] % 12;
        if (root < 0)
            root += 12;
        return names[root];
    }
}
