using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class AutomationExtendedTests
{
    [Fact]
    public void normalize_complexity_vector_single_repeats()
    {
        var v = Automations.NormalizeComplexityVector(new[] { 7 }, 4);
        AssertSeqEqual(v, new[] { 7, 7, 7, 7 });
    }

    [Fact]
    public void forward_degree_automation_empty_degrees_throws()
    {
        var scale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var crit = new IntervalVector(Ints(2, 2, 3), 0, 12);
        var seed = new PositionVector(Ints(0, 4, 7), 12, 12);
        Assert.Throws<ArgumentException>(() =>
            Automations.ForwardDegreeAutomation(scale, crit, Array.Empty<int>(), seed, Array.Empty<int>()));
    }

    [Fact]
    public void degree_automation_sequential_backward_empty_throws()
    {
        var scale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var crit = new IntervalVector(Ints(2, 2, 3), 0, 12);
        var seed = new PositionVector(Ints(0, 4, 7), 12, 12);
        Assert.Throws<ArgumentException>(() =>
            Automations.DegreeAutomationSequentialBackward(scale, crit, Array.Empty<int>(), seed, Array.Empty<int>()));
    }

    [Fact]
    public void voice_leading_sequential_backward_empty_throws()
    {
        Assert.Throws<ArgumentException>(() =>
            Automations.VoiceLeadingAutomationSequentialBackward(Array.Empty<PositionVector>(), Array.Empty<int>()));
    }

    [Fact]
    public void degree_automation_vector_reference_size_mismatch_throws()
    {
        var scale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var crit = new IntervalVector(Ints(2, 2, 3), 0, 12);
        var degrees = new[] { 0, 1 };
        var refs = new List<PositionVector> { new(Ints(0, 4, 7)) };
        Assert.Throws<ArgumentException>(() =>
            Automations.DegreeAutomationVectorReference(scale, crit, degrees, refs, Array.Empty<int>()));
    }

    [Fact]
    public void degree_automation_vector_reference_two_degrees()
    {
        var scale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var crit = new IntervalVector(Ints(2, 2, 3), 0, 12);
        var degrees = new[] { 0, 1 };
        var refs = new List<PositionVector>
        {
            new(Ints(0, 4, 7), 12, 12),
            new(Ints(2, 5, 9), 12, 12),
        };
        var outList = Automations.DegreeAutomationVectorReference(scale, crit, degrees, refs, new[] { 0, 0 });
        Assert.Equal(2, outList.Count);
        Assert.Equal(12, outList[0].Mod);
        Assert.Equal(12, outList[1].Mod);
        Assert.Equal(3, (int)outList[0].Count);
        Assert.Equal(3, (int)outList[1].Count);
    }

    [Fact]
    public void voice_leading_automation_identical_ref_target_zero_distance()
    {
        var chord = new PositionVector(Ints(0, 4, 7), 12, 12);
        var copy = chord.Clone();
        var row = Automations.VoiceLeadingAutomation(chord, copy, 0);
        Assert.Equal(0.0, row.Distance, 6);
        AssertPvEq(row.Vector, new[] { 0, 4, 7 }, 12);
    }

    [Fact]
    public void voice_leading_automation_vector_reference_two_chords()
    {
        var targets = new List<PositionVector>
        {
            new(Ints(0, 4, 7), 12, 12),
            new(Ints(2, 5, 9), 12, 12),
        };
        var refs = new List<PositionVector>
        {
            new(Ints(0, 4, 7), 12, 12),
            new(Ints(2, 5, 9), 12, 12),
        };
        var outList = Automations.VoiceLeadingAutomationVectorReference(targets, refs, Array.Empty<int>());
        Assert.Equal(2, outList.Count);
        Assert.Equal(0, Distances.ManhattanDistance(outList[0], targets[0]));
        Assert.Equal(0, Distances.ManhattanDistance(outList[1], targets[1]));
    }

    [Fact]
    public void auto_scale_preserves_length_and_mod()
    {
        var scale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var notes = new[] { 60, 64, 67 };
        var outPv = Automations.AutoScale(scale, notes);
        Assert.Equal(scale.Count, outPv.Count);
        Assert.Equal(scale.Mod, outPv.Mod);
    }

    [Fact]
    public void auto_scale_single_note_no_throw()
    {
        var scale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var outPv = Automations.AutoScale(scale, new[] { 12 });
        Assert.Equal(scale.MutableData.Count, outPv.MutableData.Count);
    }
}
