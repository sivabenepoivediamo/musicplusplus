using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class AutomationTests
{
    private static void AssertPositionSequenceEq(IReadOnlyList<PositionVector> actual, IReadOnlyList<int[]> expected)
    {
        Assert.Equal(expected.Count, actual.Count);
        for (var i = 0; i < actual.Count; i++)
            AssertPvEq(actual[i], expected[i], 12);
    }

    [Fact]
    public void automation_row_examples()
    {
        var scale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11));
        var criterion = new IntervalVector(Ints(2, 2, 3), 35, 12);
        var reference = new PositionVector(Ints(60, 64, 67));
        var target = new PositionVector(Ints(67, 71, 74));
        var notes = new[] { 63 };

        var degreeRow = Automations.DegreeAutomation(scale, criterion, 3, target, 0);
        Assert.Equal(6, degreeRow.ModeIndex);
        Assert.Equal(0, degreeRow.RelativeModeIndex);
        Assert.Equal(2.0, degreeRow.Distance, 6);
        AssertPvEq(degreeRow.Vector, new[] { 65, 71, 74 }, 12);

        var voiceRow = Automations.VoiceLeadingAutomation(reference, target, 0);
        Assert.Equal(-2, voiceRow.RelativeModeOffset);
        Assert.Equal(-2, voiceRow.Center);
        Assert.Equal(3.0, voiceRow.Distance, 6);
        AssertPvEq(voiceRow.Vector, new[] { 59, 62, 67 }, 12);

        var modalRow = Automations.ModalInterchangeAutomation(scale, notes, 0);
        Assert.Equal(1, modalRow.Index);
        Assert.Equal(2.0, modalRow.Distance, 6);
        AssertPvEq(modalRow.Vector, new[] { 0, 2, 3, 5, 7, 9, 10 }, 12);

        var modulationRow = Automations.ModulationAutomation(scale, notes, 0);
        Assert.Equal(10, modulationRow.Transposition);
        Assert.Equal(2.0, modulationRow.Distance, 6);
        AssertPvEq(modulationRow.Vector, new[] { 0, 2, 3, 5, 7, 9, 10 }, 12);
    }

    [Fact]
    public void automation_sequence_examples()
    {
        var scale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11));
        var crit = new IntervalVector(Ints(2, 2, 2, 2));
        var I = ChordFunctions.Chord(scale, crit, 0, 0, 4);
        var II = ChordFunctions.Chord(scale, crit, 1, 0, 4);
        var V = ChordFunctions.Chord(scale, crit, 4, 0, 4);
        var VI = ChordFunctions.Chord(scale, crit, 5, 0, 4);
        var IV = ChordFunctions.Chord(scale, crit, 3, 0, 4);
        var VII = ChordFunctions.Chord(scale, crit, 6, 0, 4);
        var IV6 = ChordFunctions.Chord(scale, crit, 1, 0, 4, 1);

        var chords = new List<PositionVector> { I, II, V, I, VI, IV, II, VII, I, V, VI, II, V, I };
        var complexities = new[] { 0 };
        var octaveRule = new[] { 0, 5, 20, 7, 20, 10, 15, 0, 5, 15, 7, 20, 0, 15, 7 };
        var degrees = new[] { 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0 };

        var forwardVoiceLeading = Automations.ForwardVoiceLeading(chords, complexities);
        var backwardVoiceLeading = Automations.VoiceLeadingAutomationSequentialBackward(chords, complexities);
        var referenceVoiceLeading = Automations.VoiceLeadingAutomationReference(chords, IV6, complexities);
        var forwardDegree = Automations.ForwardDegreeAutomation(scale, crit, degrees, I, octaveRule);
        var backwardDegree = Automations.DegreeAutomationSequentialBackward(scale, crit, degrees, I, complexities);
        var referenceDegree = Automations.DegreeAutomationReference(scale, crit, degrees, I, complexities);

        AssertPositionSequenceEq(forwardVoiceLeading, new[]
        {
            new[] { 0, 4, 7, 11 }, new[] { 0, 2, 5, 9 }, new[] { -1, 2, 5, 7 }, new[] { -1, 0, 4, 7 },
            new[] { -3, 0, 4, 7 }, new[] { -3, 0, 4, 5 }, new[] { -3, 0, 2, 5 }, new[] { -3, -1, 2, 5 },
            new[] { -5, -1, 0, 4 }, new[] { -5, -1, 2, 5 }, new[] { -5, -3, 0, 4 }, new[] { -7, -3, 0, 2 },
            new[] { -7, -5, -1, 2 }, new[] { -8, -5, -1, 0 },
        });

        AssertPositionSequenceEq(backwardVoiceLeading, new[]
        {
            new[] { 7, 11, 12, 16 }, new[] { 9, 12, 14, 17 }, new[] { 7, 11, 14, 17 }, new[] { 7, 11, 12, 16 },
            new[] { 7, 9, 12, 16 }, new[] { 5, 9, 12, 16 }, new[] { 5, 9, 12, 14 }, new[] { 5, 9, 11, 14 },
            new[] { 4, 7, 11, 12 }, new[] { 5, 7, 11, 14 }, new[] { 4, 7, 9, 12 }, new[] { 2, 5, 9, 12 },
            new[] { 2, 5, 7, 11 }, new[] { 0, 4, 7, 11 },
        });

        AssertPositionSequenceEq(referenceVoiceLeading, new[]
        {
            new[] { 4, 7, 11, 12 }, new[] { 5, 9, 12, 14 }, new[] { 5, 7, 11, 14 }, new[] { 4, 7, 11, 12 },
            new[] { 7, 9, 12, 16 }, new[] { 5, 9, 12, 16 }, new[] { 5, 9, 12, 14 }, new[] { 5, 9, 11, 14 },
            new[] { 4, 7, 11, 12 }, new[] { 5, 7, 11, 14 }, new[] { 7, 9, 12, 16 }, new[] { 5, 9, 12, 14 },
            new[] { 5, 7, 11, 14 }, new[] { 4, 7, 11, 12 },
        });

        AssertPositionSequenceEq(forwardDegree, new[]
        {
            new[] { 0, 4, 7, 11 }, new[] { 2, 5, 9, 12 }, new[] { 4, 7, 11, 14 }, new[] { 4, 5, 9, 12 },
            new[] { 5, 7, 11, 14 }, new[] { 4, 7, 9, 12 }, new[] { 5, 9, 11, 14 }, new[] { 4, 7, 11, 12 },
            new[] { 5, 9, 11, 14 }, new[] { 4, 7, 9, 12 }, new[] { 5, 7, 11, 14 }, new[] { 4, 5, 9, 12 },
            new[] { 2, 4, 7, 11 }, new[] { 0, 2, 5, 9 }, new[] { -1, 0, 4, 7 },
        });

        AssertPositionSequenceEq(backwardDegree, new[]
        {
            new[] { 4, 7, 11, 12 }, new[] { 5, 9, 12, 14 }, new[] { 4, 7, 11, 14 }, new[] { 5, 9, 12, 16 },
            new[] { 5, 7, 11, 14 }, new[] { 4, 7, 9, 12 }, new[] { 2, 5, 9, 11 }, new[] { 0, 4, 7, 11 },
            new[] { -1, 2, 5, 9 }, new[] { -3, 0, 4, 7 }, new[] { -5, -1, 2, 5 }, new[] { -3, 0, 4, 5 },
            new[] { -1, 2, 4, 7 }, new[] { 0, 2, 5, 9 }, new[] { 0, 4, 7, 11 },
        });

        AssertPositionSequenceEq(referenceDegree, new[]
        {
            new[] { 0, 4, 7, 11 }, new[] { 0, 2, 5, 9 }, new[] { 2, 4, 7, 11 }, new[] { 0, 4, 5, 9 },
            new[] { 2, 5, 7, 11 }, new[] { 0, 4, 7, 9 }, new[] { 2, 5, 9, 11 }, new[] { 0, 4, 7, 11 },
            new[] { 2, 5, 9, 11 }, new[] { 0, 4, 7, 9 }, new[] { 2, 5, 7, 11 }, new[] { 0, 4, 5, 9 },
            new[] { 2, 4, 7, 11 }, new[] { 0, 2, 5, 9 }, new[] { 0, 4, 7, 11 },
        });
    }
}
