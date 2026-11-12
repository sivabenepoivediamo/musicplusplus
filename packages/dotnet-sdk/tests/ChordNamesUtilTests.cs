using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class ChordNamesUtilTests
{
    [Fact]
    public void note_to_string_octave_wrap()
    {
        Assert.Equal("C", ChordNames.NoteToString(60));
        Assert.Equal("C#", ChordNames.NoteToString(61));
        Assert.Equal("C", ChordNames.NoteToString(72));
    }

    [Fact]
    public void note_to_string_negative_midi_pitch_class()
    {
        Assert.Equal("B", ChordNames.NoteToString(-1));
        Assert.Equal("B", ChordNames.NoteToString(-13));
        Assert.Equal("C", ChordNames.NoteToString(-12));
    }

    [Fact]
    public void interval_to_string_known_and_fallback()
    {
        Assert.Equal("m3", ChordNames.IntervalToString(3));
        Assert.Equal("M3", ChordNames.IntervalToString(4));
        Assert.Equal("999", ChordNames.IntervalToString(999));
    }

    [Fact]
    public void analyze_chord_invalid_root_throws()
    {
        Assert.Throws<ArgumentOutOfRangeException>(() => ChordNames.AnalyzeChord(Array.Empty<int>(), 0));
        Assert.Throws<ArgumentOutOfRangeException>(() => ChordNames.AnalyzeChord(new[] { 60 }, -1));
        Assert.Throws<ArgumentOutOfRangeException>(() => ChordNames.AnalyzeChord(new[] { 60 }, 1));
    }

    [Fact]
    public void analyze_chord_nonzero_root_index()
    {
        var inv = new[] { 64, 60, 67 };
        var a = ChordNames.AnalyzeChord(inv, 1);
        Assert.Equal(60, a.Root);
        Assert.True(a.HasMajorThird);
        Assert.True(a.HasPerfectFifth);
    }

    [Fact]
    public void build_chord_name_minor_seventh()
    {
        var dmin7 = new[] { 62, 65, 69, 72 };
        Assert.Equal("Dmin7", ChordNames.BuildChordName(ChordNames.AnalyzeChord(dmin7, 0)));
    }

    [Fact]
    public void build_chord_name_major_and_diminished_triads()
    {
        Assert.Equal("C", ChordNames.BuildChordName(ChordNames.AnalyzeChord(new[] { 60, 64, 67 }, 0)));
        Assert.Equal("Cdim", ChordNames.BuildChordName(ChordNames.AnalyzeChord(new[] { 60, 63, 66 }, 0)));
    }

    [Fact]
    public void build_chord_name_sus4()
    {
        Assert.Equal("Csus4", ChordNames.BuildChordName(ChordNames.AnalyzeChord(new[] { 60, 65, 67 }, 0)));
    }

    [Fact]
    public void build_chord_name_sus2_and_aug7()
    {
        Assert.Equal("Csus2", ChordNames.BuildChordName(ChordNames.AnalyzeChord(new[] { 60, 62, 67 }, 0)));
        Assert.Equal("Caug7", ChordNames.BuildChordName(ChordNames.AnalyzeChord(new[] { 60, 64, 68, 70 }, 0)));
    }

    [Fact]
    public void interval_to_string_perfect_fifth()
    {
        Assert.Equal("5", ChordNames.IntervalToString(7));
    }

    [Fact]
    public void build_chord_name_seventh_sus_and_half_diminished()
    {
        Assert.Equal("Cmaj7sus4", ChordNames.BuildChordName(ChordNames.AnalyzeChord(new[] { 60, 65, 67, 71 }, 0)));
        Assert.Equal("C7sus2", ChordNames.BuildChordName(ChordNames.AnalyzeChord(new[] { 60, 62, 67, 70 }, 0)));
        Assert.Equal("Cmin7b5", ChordNames.BuildChordName(ChordNames.AnalyzeChord(new[] { 60, 63, 66, 70 }, 0)));
    }
}
