namespace Musicpp;

public readonly record struct NoteInfo(int Position, int Degree, int Octave, int NoteValue);

public readonly record struct Analysis(NoteInfo Chord, NoteInfo Scale, NoteInfo Chromatic);

public sealed class VectorModification
{
    public enum Kind { Chord, Scale, Chromatic }
    public Kind Type { get; }
    public int Delta { get; }
    public VectorModification(Kind type, int delta)
    {
        Type = type;
        Delta = delta;
    }
}

public sealed class TripleSelectResult
{
    public List<int> Results { get; } = new();
    public List<bool> IsOut { get; } = new();
}

public static class Melody
{
    public static NoteInfo InvalidNote() => new(-666, -666, -666, -666);

    public static List<int> Diminution(int degree, int length, bool up, bool left)
    {
        if (length <= 0)
            return new List<int>();
        if (length == 1)
            return new List<int> { degree };
        var result = new int[length];
        result[0] = degree;
        result[length - 1] = degree;
        var distance = length - 2;
        var firstNum = up ? degree - distance : degree + distance;
        var middle = new List<int>();
        for (var i = 1; i < length - 1; i++)
            middle.Add(up ? firstNum + (i - 1) : firstNum - (i - 1));
        if (left)
            middle.Sort();
        else
            middle.Sort(Comparer<int>.Create((a, b) => b.CompareTo(a)));
        for (var i = 0; i < middle.Count; i++)
            result[i + 1] = middle[i];
        return result.ToList();
    }

    public static List<int> Run(int degree, int length, bool direction)
    {
        if (length <= 0)
            throw new ArgumentException("run: length must be positive");
        var step = direction ? 1 : -1;
        var start = direction ? degree - (length - 1) : degree + (length - 1);
        var result = new List<int>(length);
        for (var i = 0; i < length; i++)
            result.Add(start + i * step);
        return result;
    }

    public static List<int> Run2(int start, int end)
    {
        var result = new List<int>();
        if (start <= end)
        {
            for (var i = start; i <= end; i++)
                result.Add(i);
        }
        else
        {
            for (var i = start; i >= end; i--)
                result.Add(i);
        }
        return result;
    }

    public static List<int> NormalizeNotes(IReadOnlyList<int> notes, int mod)
    {
        if (mod <= 0)
            throw new ArgumentException("normalizeNotes: mod must be positive");
        var result = new List<int>(notes.Count);
        for (var i = 0; i < notes.Count; i++)
        {
            var n = notes[i] % mod;
            result.Add(n < 0 ? n + mod : n);
        }
        return result;
    }

    public static NoteInfo Info(int inputNote, IReadOnlyList<int> vector, int mod)
    {
        if (mod <= 0)
            throw new ArgumentException("info: mod must be positive");
        var normalizedNote = inputNote % mod;
        if (normalizedNote < 0)
            normalizedNote += mod;
        var baseOctave = (int)Math.Floor((double)inputNote / mod);
        var degree = -1;
        for (var i = 0; i < vector.Count; i++)
        {
            var v = vector[i] % mod;
            if (v < 0)
                v += mod;
            if (v == normalizedNote)
            {
                degree = i;
                break;
            }
        }
        if (degree == -1)
            return InvalidNote();
        var octaveAdjustment = (int)Math.Floor((double)vector[degree] / mod);
        var position = (baseOctave + octaveAdjustment) * vector.Count + degree;
        return new NoteInfo(position, degree, baseOctave + octaveAdjustment, inputNote);
    }

    public static Analysis Hierarchy(
        int note,
        IReadOnlyList<int> chord,
        IReadOnlyList<int> scale,
        IReadOnlyList<int> chromatic,
        int mod)
    {
        if (mod <= 0)
            throw new ArgumentException("hierarchy: mod must be positive");
        var normChord = NormalizeNotes(chord, mod);
        return new Analysis(
            Info(note, normChord, mod),
            Info(note, scale, mod),
            Info(note, chromatic, mod));
    }

    public static int GetNoteFromPosition(int position, IReadOnlyList<int> vector, int mod)
    {
        if (vector.Count == 0)
            throw new ArgumentException("getNoteFromPosition: vector must be non-empty");
        if (mod <= 0)
            throw new ArgumentException("getNoteFromPosition: mod must be positive");
        var vlen = vector.Count;
        var octave = (int)Math.Floor((double)position / vlen);
        var degree = ((position % vlen) + vlen) % vlen;
        return vector[degree] + octave * mod;
    }

    public static TripleSelectResult TripleSelect(
        Analysis analysis,
        IReadOnlyList<VectorModification> modifications,
        IReadOnlyList<int> chord,
        IReadOnlyList<int> scale,
        IReadOnlyList<int> chromatic,
        int mod)
    {
        if (mod <= 0)
            throw new ArgumentException("tripleSelect: mod must be positive");
        var outRes = new TripleSelectResult();
        foreach (var mod_ in modifications)
        {
            int basePosition;
            IReadOnlyList<int>? vec;
            switch (mod_.Type)
            {
                case VectorModification.Kind.Chord:
                    basePosition = analysis.Chord.Position;
                    vec = chord;
                    break;
                case VectorModification.Kind.Scale:
                    basePosition = analysis.Scale.Position;
                    vec = scale;
                    break;
                case VectorModification.Kind.Chromatic:
                    basePosition = analysis.Chromatic.Position;
                    vec = chromatic;
                    break;
                default:
                    throw new InvalidOperationException("Invalid modification type");
            }
            if (basePosition == -666)
            {
                outRes.Results.Add(0);
                outRes.IsOut.Add(true);
                continue;
            }
            var targetPosition = basePosition + mod_.Delta;
            outRes.Results.Add(GetNoteFromPosition(targetPosition, vec!, mod));
            outRes.IsOut.Add(false);
        }
        return outRes;
    }

    public static List<VectorModification> ParseModifications(
        IReadOnlyList<int> deltas,
        IReadOnlyList<VectorModification.Kind> types)
    {
        var typeList = types.ToList();
        if (typeList.Count == 1)
        {
            var broadcast = typeList[0];
            typeList = Enumerable.Repeat(broadcast, deltas.Count).ToList();
        }
        if (deltas.Count != typeList.Count)
            throw new InvalidOperationException("Deltas and types must have the same length.");
        var mods = new List<VectorModification>();
        for (var i = 0; i < deltas.Count; i++)
            mods.Add(new VectorModification(typeList[i], deltas[i]));
        return mods;
    }

    public static List<int> OrnamentLoop(IReadOnlyList<int> input, int length)
    {
        if (length <= 0)
            return new List<int>();
        var result = new List<int>();
        var n = input.Count;
        while (result.Count < length)
        {
            for (var i = n - 1; i >= 0 && result.Count < length; i--)
                result.Insert(0, input[i]);
        }
        if (result.Count > length)
            result.RemoveRange(0, result.Count - length);
        return result;
    }

    public static List<VectorModification.Kind> OrnamentTypesLoop(
        IReadOnlyList<VectorModification.Kind> types,
        int length)
    {
        if (length <= 0)
            return new List<VectorModification.Kind>();
        var result = new List<VectorModification.Kind>();
        var n = types.Count;
        while (result.Count < length)
        {
            for (var i = n - 1; i >= 0 && result.Count < length; i--)
                result.Insert(0, types[i]);
        }
        if (result.Count > length)
            result.RemoveRange(0, result.Count - length);
        return result;
    }

    public static string Join(IReadOnlyList<int> v) => string.Join(", ", v);

    public static void ApplyTripleSelect(
        IReadOnlyList<int> notes,
        IReadOnlyList<int> chord,
        IReadOnlyList<int> scale,
        IReadOnlyList<int> chromatic,
        int mod,
        IReadOnlyList<int> ornaments,
        IReadOnlyList<VectorModification.Kind> types)
    {
        if (mod <= 0)
            throw new ArgumentException("applyTripleSelect: mod must be positive");
        var expandedTypes = OrnamentTypesLoop(types, ornaments.Count);
        var modifications = ParseModifications(ornaments, expandedTypes);
        foreach (var note in notes)
        {
            var analysis = Hierarchy(note, chord, scale, chromatic, mod);
            var res = TripleSelect(analysis, modifications, chord, scale, chromatic, mod);
            Console.WriteLine($"\nMIDI note {note}:");
            Console.WriteLine("Selected notes: [" + Join(res.Results) + "]");
        }
    }
}
