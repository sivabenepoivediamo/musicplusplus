using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class NoteNamesTests
{
    [Fact]
    public void midi_note_name_examples()
    {
        var system = new NoteNamingSystem();
        var harmonicMajor = system.MidiNumbersToNoteNames(
            new[] { 1, 3, 5, 6, 8, 9, 12 },
            new NoteMapperOptions(true, true, 12));
        AssertStringSeqEqual(harmonicMajor.NoteNames,
            new[] { "C♯", "D♯", "E♯", "F♯", "G♯", "A", "B♯" });

        var alteredDiatonicFlats = system.MidiNumbersToNoteNames(
            new[] { 0, 1, 3, 4, 6, 8, 10 },
            new NoteMapperOptions(false, true, 12));
        AssertStringSeqEqual(alteredDiatonicFlats.NoteNames,
            new[] { "C", "D♭", "E♭", "F♭", "G♭", "A♭", "B♭" });

        var chromaticFlats = system.MidiNumbersToNoteNames(
            new[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
            new NoteMapperOptions(false, false, 12));
        AssertStringSeqEqual(
            chromaticFlats.NoteNames,
            new[] { "C", "D♭", "D", "E♭", "E", "F", "G♭", "G", "A♭", "A", "B♭", "B" });
    }

    [Fact]
    public void position_vector_note_name_examples()
    {
        var system = new NoteNamingSystem();
        var sharpsDiatonic = new NoteMapperOptions(true, true, 12);
        var flatsDiatonic = new NoteMapperOptions(false, true, 12);
        var sharpsNonDiatonic = new NoteMapperOptions(true, false, 12);

        var cMajor = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12);
        AssertStringSeqEqual(
            system.PositionVectorToNoteNames(cMajor, sharpsDiatonic).NoteNames,
            new[] { "C", "D", "E", "F", "G", "A", "B" });

        var fMajor = new PositionVector(Ints(5, 7, 9, 10, 0, 2, 4), 12);
        AssertStringSeqEqual(
            system.PositionVectorToNoteNames(fMajor, flatsDiatonic).NoteNames,
            new[] { "F", "G", "A", "B♭", "C", "D", "E" });

        var g7Chord = new PositionVector(Ints(7, 11, 2, 5), 12);
        AssertStringSeqEqual(
            system.PositionVectorToNoteNames(g7Chord, sharpsNonDiatonic).NoteNames,
            new[] { "G", "B", "D", "F" });

        var microtonal = new PositionVector(Ints(0, 5, 10, 13, 18, 23, 28), 31).RelativeMode(2);
        var microtonalResult = system.PositionVectorToNoteNames(microtonal, new NoteMapperOptions(false, false, 31));
        AssertStringSeqEqual(
            microtonalResult.NoteNames,
            new[] { "E", "F", "G", "A", "B", "C", "D" });
        AssertStringSeqEqual(
            microtonalResult.CentsInfo,
            new[]
            {
                "E -13 cents",
                "F +3 cents",
                "G -3 cents",
                "A -10 cents",
                "B -16 cents",
                "D -6 cents",
            });

        var transposed = cMajor + 5;
        AssertStringSeqEqual(
            system.PositionVectorToNoteNames(transposed, sharpsDiatonic).NoteNames,
            new[] { "F", "G", "A", "B♭", "C", "D", "E" });

        var rotated = cMajor.Rotate(2);
        AssertStringSeqEqual(
            system.PositionVectorToNoteNames(rotated, flatsDiatonic).NoteNames,
            new[] { "A", "B", "C", "D", "E", "F", "G" });

        var cMajorChord = new PositionVector(Ints(0, 4, 7), 12);
        var inverted = cMajorChord.Inversion(0);
        AssertStringSeqEqual(
            system.PositionVectorToNoteNames(inverted, sharpsNonDiatonic).NoteNames,
            new[] { "F", "G♯", "C" });

        var pentatonic = new PositionVector(Ints(0, 2, 4, 7, 9), 12);
        AssertStringSeqEqual(
            system.PositionVectorToNoteNames(pentatonic.Complement(), sharpsNonDiatonic).NoteNames,
            new[] { "C♯", "D♯", "F", "F♯", "G♯", "A♯", "B" });
    }

    [Fact]
    public void midi_numbers_empty_returns_empty_note_result()
    {
        var system = new NoteNamingSystem();
        var r = system.MidiNumbersToNoteNames(Array.Empty<int>(), new NoteMapperOptions(true, false, 12));
        Assert.Empty(r.NoteNames);
        Assert.Empty(r.CentsInfo);
    }
}
