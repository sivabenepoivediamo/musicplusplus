namespace Musicpp;

public enum AlterationDirection
{
    Natural,
    Right,
    Left,
    None
}

public readonly record struct ClassifiedNote(string? Note, AlterationDirection Label);

public sealed class NoteResult
{
    public List<string> NoteNames { get; }
    public List<string> CentsInfo { get; }

    public NoteResult()
    {
        NoteNames = new List<string>();
        CentsInfo = new List<string>();
    }

    public NoteResult(IReadOnlyList<string> names, IReadOnlyList<string> cents)
    {
        NoteNames = names.ToList();
        CentsInfo = cents.ToList();
    }
}

public sealed class NoteMapperOptions
{
    public bool PreferSharps { get; set; }
    public bool IsDiatonicScale { get; set; }
    public int ModuloValue { get; set; }

    public NoteMapperOptions(bool sharps = true, bool diatonic = false, int modulo = 12)
    {
        PreferSharps = sharps;
        IsDiatonicScale = diatonic;
        ModuloValue = modulo;
    }
}

public sealed class NoteNamingSystem
{
    private readonly List<List<string>> _noteArrays = new()
    {
        new() { "C", "B♯", "D♭♭" },
        new() { "C♯", "D♭" },
        new() { "D", "C♯♯", "E♭♭" },
        new() { "D♯", "E♭" },
        new() { "E", "D♯♯", "F♭" },
        new() { "F", "E♯", "G♭♭" },
        new() { "F♯", "G♭" },
        new() { "G", "F♯♯", "A♭♭" },
        new() { "G♯", "A♭" },
        new() { "A", "G♯♯", "B♭♭" },
        new() { "A♯", "B♭" },
        new() { "B", "A♯♯", "C♭" }
    };

    private List<List<ClassifiedNote>> _classifiedNotes = new();
    private static readonly char[] NoteOrder = { 'A', 'B', 'C', 'D', 'E', 'F', 'G' };

    public NoteNamingSystem() => ClassifyNotes();

    private void ClassifyNotes()
    {
        _classifiedNotes = new List<List<ClassifiedNote>>();
        foreach (var array in _noteArrays)
        {
            var classified = new List<ClassifiedNote>();
            if (array.Count == 3)
            {
                classified.Add(new ClassifiedNote(array[0], AlterationDirection.Natural));
                classified.Add(new ClassifiedNote(array[1], AlterationDirection.Right));
                classified.Add(new ClassifiedNote(array[2], AlterationDirection.Left));
            }
            else if (array.Count == 2)
            {
                classified.Add(new ClassifiedNote(array[0], AlterationDirection.Right));
                classified.Add(new ClassifiedNote(array[1], AlterationDirection.Left));
            }
            else
            {
                foreach (var note in array)
                    classified.Add(new ClassifiedNote(note, AlterationDirection.None));
            }
            _classifiedNotes.Add(classified);
        }
    }

    private static char GetBasicNoteName(string noteName) =>
        string.IsNullOrEmpty(noteName) ? '\0' : noteName[0];

    private char GetNextNoteLetter(char currentNote)
    {
        var idx = Array.IndexOf(NoteOrder, currentNote);
        if (idx < 0)
            return '\0';
        return NoteOrder[(idx + 1) % 7];
    }

    private bool AreNotesConsecutive(IReadOnlyList<string> noteNames)
    {
        if (noteNames.Count == 0)
            return true;
        var basicNotes = noteNames.Select(GetBasicNoteName).ToList();
        for (var start = 0; start < NoteOrder.Length; start++)
        {
            var isValid = true;
            for (var i = 0; i < basicNotes.Count; i++)
            {
                var expected = NoteOrder[(start + i) % NoteOrder.Length];
                if (basicNotes[i] != expected)
                {
                    isValid = false;
                    break;
                }
            }
            if (isValid)
                return true;
        }
        return false;
    }

    private static bool HasDoubleAccidentals(IReadOnlyList<string> noteNames)
    {
        foreach (var note in noteNames)
        {
            if (note.Contains("♯♯", StringComparison.Ordinal) || note.Contains("##", StringComparison.Ordinal) ||
                note.Contains("♭♭", StringComparison.Ordinal) || note.Contains("bb", StringComparison.Ordinal))
                return true;
        }
        return false;
    }

    private int ScoreConfiguration(IReadOnlyList<string> noteNames, bool preferSharps)
    {
        var score = 0;
        foreach (var note in noteNames)
        {
            if (note.Contains('♯') || note.Contains('#'))
                score += preferSharps ? 10 : -10;
            else if (note.Contains('♭'))
                score += preferSharps ? -10 : 10;
            else
                score += 5;
        }
        return score;
    }

    private List<string>? FindConsecutiveConfiguration(IReadOnlyList<int> noteIndices, bool preferSharps)
    {
        if (noteIndices.Count != 7)
            return null;
        var validConfigurations = new List<List<string>>();
        for (var startIdx = 0; startIdx < 7; startIdx++)
        {
            var candidate = new List<string>();
            var isValid = true;
            for (var i = 0; i < 7; i++)
            {
                var requiredLetter = NoteOrder[(startIdx + i) % 7];
                var noteIndex = noteIndices[i];
                var possibleNotes = _noteArrays[noteIndex];
                var foundNote = "";
                foreach (var note in possibleNotes)
                {
                    if (GetBasicNoteName(note) == requiredLetter)
                    {
                        foundNote = note;
                        break;
                    }
                }
                if (string.IsNullOrEmpty(foundNote))
                {
                    isValid = false;
                    break;
                }
                candidate.Add(foundNote);
            }
            if (isValid && AreNotesConsecutive(candidate) && !HasDoubleAccidentals(candidate))
                validConfigurations.Add(candidate);
        }
        if (validConfigurations.Count == 0)
            return null;
        var bestScore = int.MinValue;
        List<string>? bestConfig = null;
        foreach (var config in validConfigurations)
        {
            var score = ScoreConfiguration(config, preferSharps);
            if (score > bestScore)
            {
                bestScore = score;
                bestConfig = config;
            }
        }
        return bestConfig;
    }

    private List<(int IntPart, double DecPart)> ProcessMidiNumbers(IReadOnlyList<int> midiNumbers, int moduloValue)
    {
        var processed = new List<(int, double)>();
        foreach (var midi in midiNumbers)
        {
            double adjusted = midi;
            if (moduloValue > 0 && moduloValue != 12)
            {
                var modResult = ((midi % moduloValue) + moduloValue) % moduloValue;
                adjusted = modResult * (12.0 / moduloValue);
            }
            adjusted = Math.Round(adjusted * 100.0) / 100.0;
            var intPart = (int)Math.Floor(adjusted);
            var decPart = adjusted - intPart;
            decPart = Math.Round(decPart * 100.0) / 100.0;
            processed.Add((intPart, decPart));
        }
        return processed;
    }

    public NoteResult MidiNumbersToNoteNames(IReadOnlyList<int> midiNumbers, NoteMapperOptions options)
    {
        if (midiNumbers.Count == 0)
            return new NoteResult();
        var processed = ProcessMidiNumbers(midiNumbers, options.ModuloValue);
        var integerParts = processed.Select(p => p.IntPart).ToList();
        var decimalParts = processed.Select(p => p.DecPart).ToList();
        var noteIndices = new List<int>();
        for (var i = 0; i < integerParts.Count; i++)
        {
            var noteValue = integerParts[i];
            if (decimalParts[i] > 0.5)
                noteValue++;
            noteIndices.Add(((noteValue % 12) + 12) % 12);
        }
        List<string> result = new();
        var isDiatonic = options.IsDiatonicScale && noteIndices.Count == 7;
        if (isDiatonic)
        {
            var cfg = FindConsecutiveConfiguration(noteIndices, options.PreferSharps);
            if (cfg != null)
                result = cfg;
            else
                isDiatonic = false;
        }
        if (result.Count == 0)
        {
            for (var i = 0; i < noteIndices.Count; i++)
            {
                var noteIndex = noteIndices[i];
                var possibleNotes = _classifiedNotes[noteIndex];
                var naturalIt = possibleNotes.FirstOrDefault(cn => cn.Label == AlterationDirection.Natural);
                if (!string.IsNullOrEmpty(naturalIt.Note))
                    result.Add(naturalIt.Note!);
                else if (options.PreferSharps)
                {
                    var sharpIt = possibleNotes.FirstOrDefault(cn => cn.Label == AlterationDirection.Right);
                    result.Add(!string.IsNullOrEmpty(sharpIt.Note) ? sharpIt.Note! : possibleNotes[0].Note!);
                }
                else
                {
                    var flatIt = possibleNotes.FirstOrDefault(cn => cn.Label == AlterationDirection.Left);
                    result.Add(!string.IsNullOrEmpty(flatIt.Note) ? flatIt.Note! : possibleNotes[0].Note!);
                }
            }
        }
        var centsInfo = new List<string>();
        for (var i = 0; i < decimalParts.Count; i++)
        {
            if (decimalParts[i] > 0.0)
            {
                var cents = (int)Math.Round(decimalParts[i] * 100.0);
                var finalNoteName = result[i];
                var finalCents = cents;
                if (cents > 50)
                    finalCents = cents - 100;
                centsInfo.Add(finalNoteName + " " + (finalCents >= 0 ? "+" : "") + finalCents + " cents");
            }
        }
        return new NoteResult(result, centsInfo);
    }

    public NoteResult PositionVectorToNoteNames(PositionVector pv, NoteMapperOptions options) =>
        MidiNumbersToNoteNames(pv.MutableData, options);

    public void TestMidiNumbersToNoteNames(IReadOnlyList<IReadOnlyList<int>> testCases)
    {
        var optionsList = new[]
        {
            new NoteMapperOptions(true, true, 12),
            new NoteMapperOptions(false, true, 12),
            new NoteMapperOptions(true, false, 12),
            new NoteMapperOptions(false, false, 12)
        };
        var labels = new[]
        {
            "Sharps (Diatonic scale)",
            "Flats (Diatonic scale)",
            "Sharps (Non diatonic scale)",
            "Flats (Non diatonic scale)"
        };
        for (var i = 0; i < testCases.Count; i++)
        {
            Console.WriteLine($"\nTest Case #{i + 1}: Notes [{string.Join(", ", testCases[i])}]");
            for (var j = 0; j < optionsList.Length; j++)
            {
                if (optionsList[j].IsDiatonicScale && testCases[i].Count != 7)
                    continue;
                var res = MidiNumbersToNoteNames(testCases[i], optionsList[j]);
                Console.WriteLine(labels[j] + ":");
                Console.WriteLine("  Notes: " + string.Join(" ", res.NoteNames));
                if (res.CentsInfo.Count > 0)
                    Console.WriteLine("  Cents: " + string.Join(", ", res.CentsInfo));
            }
        }
    }
}
