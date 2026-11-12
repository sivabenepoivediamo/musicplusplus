using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class AutomationHelpersTests
{
    [Fact]
    public void normalize_complexity_vector_empty_fills_zero()
    {
        var v = Automations.NormalizeComplexityVector(Array.Empty<int>(), 4);
        AssertSeqEqual(v, new[] { 0, 0, 0, 0 });
    }

    [Fact]
    public void normalize_complexity_vector_same_size_unchanged()
    {
        var v = Automations.NormalizeComplexityVector(new[] { 1, 2, 3 }, 3);
        AssertSeqEqual(v, new[] { 1, 2, 3 });
    }

    [Fact]
    public void normalize_complexity_vector_cycle_and_truncate()
    {
        var c = Automations.NormalizeComplexityVector(new[] { 10, 20 }, 5);
        AssertSeqEqual(c, new[] { 10, 20, 10, 20, 10 });
        var t = Automations.NormalizeComplexityVector(new[] { 1, 2, 3, 4, 5 }, 2);
        AssertSeqEqual(t, new[] { 1, 2 });
    }

    [Fact]
    public void get_max_interval()
    {
        Assert.Equal(0, Automations.GetMaxInterval(new[] { 42 }));
        Assert.Equal(7, Automations.GetMaxInterval(new[] { 0, 5, 12, 13 }));
    }

    [Fact]
    public void forward_voice_leading_empty_throws()
    {
        Assert.Throws<ArgumentException>(() => Automations.ForwardVoiceLeading(Array.Empty<PositionVector>()));
    }

    [Fact]
    public void forward_voice_leading_single_returns_copy()
    {
        var one = new List<PositionVector> { new(Ints(0, 4, 7)) };
        var outList = Automations.ForwardVoiceLeading(one);
        Assert.Single(outList);
        AssertPvEq(outList[0], new[] { 0, 4, 7 }, 12);
    }

    [Fact]
    public void voice_leading_automation_vector_reference_size_mismatch()
    {
        var t = new List<PositionVector> { new(Ints(0, 4, 7)) };
        var r = new List<PositionVector>();
        Assert.Throws<ArgumentException>(() => Automations.VoiceLeadingAutomationVectorReference(t, r, Array.Empty<int>()));
    }
}
