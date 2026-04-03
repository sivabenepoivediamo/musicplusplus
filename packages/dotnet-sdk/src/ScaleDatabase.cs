namespace Musicpp;

public readonly record struct ScaleMatch(string SheetName, string ScaleName, IReadOnlyList<int> PitchClasses)
{
    /// <summary>Defensive copy of <see cref="PitchClasses"/>.</summary>
    public int[] ToPitchClasses() => PitchClasses is int[] a ? (int[])a.Clone() : PitchClasses.ToArray();
}

public sealed partial class ScaleDatabase
{
    private readonly List<ScaleMatch> _scales = new();
    private readonly Dictionary<(string Sheet, string Name), ScaleMatch> _bySheetAndName = new();
    private IReadOnlyList<string>? _sheetNames;

    /// <summary>Shared catalog; same data as <see cref="ScaleDatabase()"/> without allocating a new list graph per consumer.</summary>
    public static ScaleDatabase Shared { get; } = new();

    public ScaleDatabase()
    {
        ScaleDictionaryData.AddAllScales(this);
        foreach (var m in _scales)
        {
            var key = (m.SheetName, m.ScaleName);
            _bySheetAndName.TryAdd(key, m);
        }
    }

    public int Count => _scales.Count;

    /// <summary>All scales in load order (same order as the generated dictionary).</summary>
    public IReadOnlyList<ScaleMatch> All => _scales;

    /// <summary>
    /// Random access by category and scale name. If the source listed the same pair twice, the first row is kept.
    /// </summary>
    public IReadOnlyDictionary<(string SheetName, string ScaleName), ScaleMatch> ScalesBySheetAndName => _bySheetAndName;

    /// <summary>Distinct category names, sorted with <see cref="StringComparer.Ordinal"/>.</summary>
    public IReadOnlyList<string> GetSheetNames() =>
        _sheetNames ??= _scales.Select(s => s.SheetName).Distinct().Order(StringComparer.Ordinal).ToList();

    /// <summary>Scale names in the given category, sorted with <see cref="StringComparer.Ordinal"/>.</summary>
    public IReadOnlyList<string> GetScaleNames(string sheetName)
    {
        if (string.IsNullOrEmpty(sheetName))
            return Array.Empty<string>();
        return _scales
            .Where(s => s.SheetName == sheetName)
            .Select(s => s.ScaleName)
            .Distinct()
            .Order(StringComparer.Ordinal)
            .ToList();
    }

    /// <summary>Every scale row in a category, in dictionary load order.</summary>
    public IReadOnlyList<ScaleMatch> GetScalesInSheet(string sheetName)
    {
        if (string.IsNullOrEmpty(sheetName))
            return Array.Empty<ScaleMatch>();
        return _scales.Where(s => s.SheetName == sheetName).ToList();
    }

    public bool TryGetScale(string sheetName, string scaleName, out ScaleMatch match)
    {
        match = default;
        if (string.IsNullOrEmpty(sheetName) || string.IsNullOrEmpty(scaleName))
            return false;
        return _bySheetAndName.TryGetValue((sheetName, scaleName), out match);
    }

    public ScaleMatch? GetScaleOrDefault(string sheetName, string scaleName) =>
        TryGetScale(sheetName, scaleName, out var m) ? m : null;

    internal void AddScaleInternal(string sheetName, string scaleName, int[] pitchClasses) =>
        _scales.Add(new ScaleMatch(sheetName, scaleName, pitchClasses.ToList()));

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
            var sortedScale = scale.PitchClasses.OrderBy(x => x).ToList();
            if (processed.SequenceEqual(sortedScale))
                results.Add(scale);
        }
        return results;
    }

    public HashSet<List<int>> GetAllIntervalSets()
    {
        var unique = new HashSet<List<int>>(new SequenceComparer());
        foreach (var scale in _scales)
            unique.Add(scale.PitchClasses.OrderBy(x => x).ToList());
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
            Console.WriteLine(string.Join(" ", scale.PitchClasses));
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
