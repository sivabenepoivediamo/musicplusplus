namespace Musicpp;

public sealed class ChordAnalysis
{
    public int Root { get; set; }
    public string ChordName { get; set; } = "";
    public bool HasThird { get; set; }
    public bool HasMajorThird { get; set; }
    public bool HasMinorThird { get; set; }
    public bool HasFifth { get; set; }
    public bool HasPerfectFifth { get; set; }
    public bool HasAugmentedFifth { get; set; }
    public bool HasDiminishedFifth { get; set; }
    public bool HasCompleteTriad { get; set; }
    public bool HasSeventh { get; set; }
    public bool HasMinorSeventh { get; set; }
    public bool HasMajorSeventh { get; set; }
    public bool HasDiminishedSeventh { get; set; }
    public bool HasNinth { get; set; }
    public bool HasFlatNinth { get; set; }
    public bool HasNaturalNinth { get; set; }
    public bool HasEleventh { get; set; }
    public bool HasNaturalEleventh { get; set; }
    public bool HasSharpEleventh { get; set; }
    public bool HasThirteenth { get; set; }
    public bool HasFlatThirteenth { get; set; }
    public bool HasNaturalThirteenth { get; set; }
    public bool HasSecond { get; set; }
    public bool HasFlatSecond { get; set; }
    public bool HasNaturalSecond { get; set; }
    public bool HasFourth { get; set; }
    public bool HasNaturalFourth { get; set; }
    public bool HasSharpFourth { get; set; }
    public bool HasSixth { get; set; }
    public bool HasFlatSixth { get; set; }
    public bool HasNaturalSixth { get; set; }
    public List<(int Interval, string Label)> AddedNotes { get; } = new();
}

public static class ChordNames
{
    private static readonly Dictionary<int, string> IntervalNames = new()
    {
        [1] = "b2", [2] = "2", [3] = "m3", [4] = "M3", [5] = "4", [6] = "b5/#4",
        [7] = "5", [8] = "b6", [9] = "6", [10] = "7", [11] = "maj7",
        [13] = "b9", [14] = "9", [17] = "11", [18] = "#11", [20] = "b13", [21] = "13"
    };

    public static string NoteToString(int midi)
    {
        var notes = new[] { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        var pc = ((midi % 12) + 12) % 12;
        return notes[pc];
    }

    public static string IntervalToString(int interval) =>
        IntervalNames.TryGetValue(interval, out var s) ? s : interval.ToString();

    public static ChordAnalysis AnalyzeChord(IReadOnlyList<int> midiNotes, int rootIndex)
    {
        if (midiNotes.Count == 0 || rootIndex < 0 || rootIndex >= midiNotes.Count)
            throw new ArgumentOutOfRangeException(nameof(rootIndex), "analyzeChord: midiNotes is empty or rootIndex is out of range");
        var analysis = new ChordAnalysis { Root = midiNotes[rootIndex] };
        var intervals = new List<int>();
        var usedIntervals = new HashSet<int>();
        for (var i = 0; i < midiNotes.Count; i++)
        {
            if (i == rootIndex)
                continue;
            var interval = midiNotes[i] - midiNotes[rootIndex];
            while (interval < 0)
                interval += 12;
            while (interval >= 24)
                interval -= 12;
            intervals.Add(interval);
        }

        if (intervals.Contains(3) && !usedIntervals.Contains(3))
        {
            analysis.HasThird = true;
            analysis.HasMinorThird = true;
            usedIntervals.Add(3);
        }
        else if (intervals.Contains(4) && !usedIntervals.Contains(4))
        {
            analysis.HasThird = true;
            analysis.HasMajorThird = true;
            usedIntervals.Add(4);
        }

        if (intervals.Contains(7) && !usedIntervals.Contains(7))
        {
            analysis.HasFifth = true;
            analysis.HasPerfectFifth = true;
            usedIntervals.Add(7);
        }
        else if (intervals.Contains(6) && !usedIntervals.Contains(6) && analysis.HasThird)
        {
            analysis.HasFifth = true;
            analysis.HasDiminishedFifth = true;
            usedIntervals.Add(6);
        }
        else if (intervals.Contains(8) && !usedIntervals.Contains(8) && analysis.HasMajorThird)
        {
            analysis.HasFifth = true;
            analysis.HasAugmentedFifth = true;
            usedIntervals.Add(8);
        }

        analysis.HasCompleteTriad = analysis.HasThird && analysis.HasFifth;

        if (intervals.Contains(11) && !usedIntervals.Contains(11))
        {
            analysis.HasSeventh = true;
            analysis.HasMajorSeventh = true;
            usedIntervals.Add(11);
        }
        else if (intervals.Contains(10) && !usedIntervals.Contains(10))
        {
            analysis.HasSeventh = true;
            analysis.HasMinorSeventh = true;
            usedIntervals.Add(10);
        }
        else if (intervals.Contains(9) && !usedIntervals.Contains(9) && analysis.HasMinorThird && analysis.HasDiminishedFifth)
        {
            analysis.HasSeventh = true;
            analysis.HasDiminishedSeventh = true;
            usedIntervals.Add(9);
        }

        if (intervals.Contains(13) && !usedIntervals.Contains(13))
        {
            analysis.HasNinth = true;
            analysis.HasFlatNinth = true;
            usedIntervals.Add(13);
        }
        else if (intervals.Contains(14) && !usedIntervals.Contains(14))
        {
            analysis.HasNinth = true;
            analysis.HasNaturalNinth = true;
            usedIntervals.Add(14);
        }

        if (intervals.Contains(17) && !usedIntervals.Contains(17))
        {
            analysis.HasEleventh = true;
            analysis.HasNaturalEleventh = true;
            usedIntervals.Add(17);
        }
        else if (intervals.Contains(18) && !usedIntervals.Contains(18))
        {
            analysis.HasEleventh = true;
            analysis.HasSharpEleventh = true;
            usedIntervals.Add(18);
        }

        if (intervals.Contains(20) && !usedIntervals.Contains(20))
        {
            analysis.HasThirteenth = true;
            analysis.HasFlatThirteenth = true;
            usedIntervals.Add(20);
        }
        else if (intervals.Contains(21) && !usedIntervals.Contains(21))
        {
            analysis.HasThirteenth = true;
            analysis.HasNaturalThirteenth = true;
            usedIntervals.Add(21);
        }

        if (intervals.Contains(1) && !usedIntervals.Contains(1))
        {
            analysis.HasSecond = true;
            analysis.HasFlatSecond = true;
            usedIntervals.Add(1);
        }
        else if (intervals.Contains(2) && !usedIntervals.Contains(2))
        {
            analysis.HasSecond = true;
            analysis.HasNaturalSecond = true;
            usedIntervals.Add(2);
        }

        if (intervals.Contains(5) && !usedIntervals.Contains(5))
        {
            analysis.HasFourth = true;
            analysis.HasNaturalFourth = true;
            usedIntervals.Add(5);
        }
        else if (intervals.Contains(6) && !usedIntervals.Contains(6))
        {
            analysis.HasFourth = true;
            analysis.HasSharpFourth = true;
            usedIntervals.Add(6);
        }

        if (intervals.Contains(8) && !usedIntervals.Contains(8))
        {
            analysis.HasSixth = true;
            analysis.HasFlatSixth = true;
            usedIntervals.Add(8);
        }
        if (intervals.Contains(9) && !usedIntervals.Contains(9))
        {
            analysis.HasSixth = true;
            analysis.HasNaturalSixth = true;
            usedIntervals.Add(9);
        }

        foreach (var interval in intervals)
        {
            if (!usedIntervals.Contains(interval))
            {
                analysis.AddedNotes.Add((interval, IntervalToString(interval)));
                usedIntervals.Add(interval);
            }
        }

        return analysis;
    }

    public static string BuildChordName(ChordAnalysis analysis)
    {
        var name = NoteToString(analysis.Root);
        var omitFifth = false;
        var omitThird = false;

        if (!analysis.HasThird && analysis.HasNaturalFourth && analysis.HasPerfectFifth)
        {
            if (analysis.HasMinorSeventh)
                name += "7sus4";
            else if (analysis.HasMajorSeventh)
                name += "maj7sus4";
            else
                name += "sus4";
        }
        else if (!analysis.HasThird && analysis.HasNaturalSecond && analysis.HasPerfectFifth)
        {
            if (analysis.HasMinorSeventh)
                name += "7sus2";
            else if (analysis.HasMajorSeventh)
                name += "maj7sus2";
            else
                name += "sus2";
        }
        else if (!analysis.HasThird && analysis.HasNaturalFourth && analysis.HasMinorSeventh)
        {
            name += "7sus4";
            omitFifth = !analysis.HasFifth;
        }
        else if (!analysis.HasThird && analysis.HasDiminishedFifth && analysis.HasMinorSeventh)
        {
            name += "7 b5";
            omitThird = true;
        }
        else if (!analysis.HasThird && analysis.HasPerfectFifth && analysis.HasMinorSeventh
                 && !analysis.HasNaturalFourth && !analysis.HasNaturalSecond)
        {
            name += "7";
            omitThird = true;
        }
        else if (analysis.HasMajorThird && analysis.HasMinorSeventh && !analysis.HasFifth)
        {
            name += "7";
            omitFifth = true;
        }
        else if (analysis.HasMinorThird && analysis.HasMinorSeventh && !analysis.HasFifth)
        {
            name += "min7";
            omitFifth = true;
        }
        else if (analysis.HasMajorThird && !analysis.HasFifth)
        {
            if (analysis.HasMajorSeventh)
                name += "maj7";
            else
                name += "maj";
            omitFifth = true;
        }
        else if (analysis.HasMinorThird && !analysis.HasFifth)
        {
            if (analysis.HasMajorSeventh)
                name += "min/maj7";
            else
                name += "min";
            omitFifth = true;
        }
        else if (analysis.HasMajorThird && analysis.HasPerfectFifth)
        {
            if (analysis.HasMinorSeventh)
                name += "7";
            else if (analysis.HasMajorSeventh)
                name += "maj7";
        }
        else if (analysis.HasMajorThird && analysis.HasAugmentedFifth)
        {
            if (analysis.HasMajorSeventh)
                name += "aug/maj7";
            else if (analysis.HasMinorSeventh)
                name += "aug7";
            else
                name += "aug";
        }
        else if (analysis.HasMajorThird && analysis.HasDiminishedFifth)
            name += "maj b5";
        else if (analysis.HasMinorThird && analysis.HasDiminishedFifth)
        {
            if (analysis.HasMajorSeventh)
                name += "dim/maj7";
            else if (analysis.HasMinorSeventh)
                name += "min7b5";
            else if (analysis.HasDiminishedSeventh)
                name += "dim7";
            else
                name += "dim";
        }
        else if (analysis.HasMinorThird && analysis.HasPerfectFifth)
        {
            if (analysis.HasMajorSeventh)
                name += "min/maj7";
            else if (analysis.HasMinorSeventh)
                name += "min7";
            else
                name += "min";
        }

        var hasSeventh = name.Contains("7") || name.Contains("maj7");

        if (hasSeventh)
        {
            if (analysis.HasNinth)
            {
                if (analysis.HasFlatNinth)
                {
                    name += "b9";
                    if (analysis.HasEleventh)
                    {
                        if (analysis.HasNaturalEleventh)
                            name += "/11";
                        else if (analysis.HasSharpEleventh)
                            name += " #11";
                        if (analysis.HasThirteenth)
                        {
                            if (analysis.HasFlatThirteenth)
                                name += "/b13";
                            else if (analysis.HasNaturalThirteenth)
                                name += "/13";
                        }
                    }
                    else if (analysis.HasThirteenth)
                    {
                        if (analysis.HasFlatThirteenth)
                            name += "/b13";
                        else if (analysis.HasNaturalThirteenth)
                            name += "/13";
                    }
                }
                else if (analysis.HasNaturalNinth)
                {
                    var pos = name.IndexOf("maj7", StringComparison.Ordinal);
                    if (pos >= 0)
                        name = name.Remove(pos, 4).Insert(pos, "9");
                    else if ((pos = name.IndexOf("aug7", StringComparison.Ordinal)) >= 0)
                        name = name.Remove(pos + 3, 1).Insert(pos + 3, "9");
                    else if ((pos = name.IndexOf("min7b5", StringComparison.Ordinal)) >= 0)
                        name += " 9";
                    else if ((pos = name.IndexOf("min7", StringComparison.Ordinal)) >= 0)
                        name = name.Remove(pos + 3, 1).Insert(pos + 3, "9");
                    else if ((pos = name.IndexOf("dim7", StringComparison.Ordinal)) >= 0)
                        name += " 9";
                    else if ((pos = name.IndexOf('7')) >= 0)
                    {
                        var isStandalone = true;
                        if (pos > 0)
                        {
                            var start = Math.Max(0, pos - 3);
                            var before = name.Substring(start, Math.Min(4, pos + 1 - start));
                            if (before.Contains("maj") || before.Contains("min") || before.Contains("dim") || before.Contains("aug"))
                                isStandalone = false;
                        }
                        if (isStandalone)
                            name = name.Remove(pos, 1).Insert(pos, "9");
                    }

                    if (analysis.HasEleventh)
                    {
                        if (analysis.HasNaturalEleventh)
                            name += "/11";
                        else if (analysis.HasSharpEleventh)
                            name += "/#11";
                        if (analysis.HasThirteenth)
                        {
                            if (analysis.HasFlatThirteenth)
                                name += "/b13";
                            else if (analysis.HasNaturalThirteenth)
                                name += "/13";
                        }
                    }
                }
            }
            else if (analysis.HasEleventh || analysis.HasThirteenth)
            {
                if (analysis.HasEleventh)
                {
                    if (analysis.HasNaturalEleventh)
                        name += "/11";
                    else if (analysis.HasSharpEleventh)
                        name += "/#11";
                }
                if (analysis.HasThirteenth)
                {
                    if (analysis.HasFlatThirteenth)
                        name += "/b13";
                    else if (analysis.HasNaturalThirteenth)
                        name += "/13";
                }
            }
        }

        var isAddedNote = false;
        if (analysis.HasNaturalSecond && !name.Contains("sus2") && !name.Contains(" 9"))
        {
            name += isAddedNote ? "" : " ";
            name += "2 ";
            isAddedNote = true;
        }
        if (analysis.HasFlatSecond)
        {
            name += isAddedNote ? "" : " ";
            name += "b2 ";
            isAddedNote = true;
        }
        if (analysis.HasNaturalFourth && !name.Contains("sus4"))
        {
            name += isAddedNote ? "" : " ";
            name += "4 ";
            isAddedNote = true;
        }
        if (analysis.HasSharpFourth)
        {
            name += isAddedNote ? "" : " ";
            name += "#4 ";
            isAddedNote = true;
        }
        if (analysis.HasNaturalSixth)
        {
            name += isAddedNote ? "" : " ";
            name += "6 ";
            isAddedNote = true;
        }
        if (analysis.HasFlatSixth)
        {
            name += isAddedNote ? "" : " ";
            name += "b6 ";
            isAddedNote = true;
        }

        foreach (var note in analysis.AddedNotes)
            name += " " + note.Label;

        if (omitFifth)
            name += " (omit 5)";
        if (omitThird)
            name += " (omit 3)";

        return name;
    }
}
