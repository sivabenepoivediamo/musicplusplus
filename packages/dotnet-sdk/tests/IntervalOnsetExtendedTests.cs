using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class IntervalOnsetExtendedTests
{
    [Fact]
    public void interval_vector_compound_assignment_scalars()
    {
        var iv = new IntervalVector(Ints(1, 2), 0, 12);
        for (var i = 0; i < iv.MutableData.Count; i++)
            iv.MutableData[i] += 10;
        Assert.Equal(11, iv[0]);
        Assert.Equal(12, iv[1]);
        for (var i = 0; i < iv.MutableData.Count; i++)
            iv.MutableData[i] -= 1;
        Assert.Equal(10, iv[0]);
        for (var i = 0; i < iv.MutableData.Count; i++)
            iv.MutableData[i] *= 2;
        Assert.Equal(20, iv[0]);
    }

    [Fact]
    public void interval_vector_element_matches_bracket()
    {
        var iv = new IntervalVector(Ints(10, 20, 30), 0, 12);
        Assert.Equal(iv.Element(0), iv[0]);
        Assert.Equal(iv.Element(4), iv[4]);
        Assert.Equal(iv.Element(-1), iv[-1]);
    }

    [Fact]
    public void interval_vector_repeat_non_positive()
    {
        var iv = new IntervalVector(Ints(1, 2), 3, 12);
        Assert.Equal(0, iv.Repeat(0).Count);
        Assert.Equal(0, iv.Repeat(-1).Count);
        AssertIvEq(iv.Repeat(2), new[] { 1, 2, 1, 2 }, 3, 12);
    }

    [Fact]
    public void interval_vector_adapt_to_lcm()
    {
        Assert.Empty(IntervalVector.AdaptToLcm(new List<IntervalVector>()));

        var a = new IntervalVector(Ints(0, 2), 0, 4);
        var b = new IntervalVector(Ints(0, 3), 0, 6);
        var adapted = IntervalVector.AdaptToLcm(new[] { a, b });
        Assert.Equal(2, adapted.Count);
        Assert.Equal(12, adapted[0].Mod);
        Assert.Equal(12, adapted[1].Mod);
        AssertSeqEqual(adapted[0].MutableData, new[] { 0, 6 });
        AssertSeqEqual(adapted[1].MutableData, new[] { 0, 6 });

        var same = IntervalVector.AdaptToLcm(new[] { a, a });
        Assert.Equal(2, same.Count);
        Assert.Equal(4, same[0].Mod);
    }

    [Fact]
    public void interval_vector_negate_flips_offset()
    {
        var iv = new IntervalVector(Ints(1, 2), 5, 12);
        var n = iv.Negate();
        AssertIvEq(n, new[] { -1, -2 }, -5, 12);
    }

    [Fact]
    public void interval_vector_parallel_mode_empty()
    {
        var empty = new IntervalVector(new List<int>(), 7, 12);
        AssertIvEq(empty.ParallelMode(3), Array.Empty<int>(), 7, 12);
    }

    [Fact]
    public void onset_vector_repeat_non_positive()
    {
        var b = new OnsetVector(Ints(1, 0), 0, 4);
        Assert.Equal(0, b.Repeat(0).Count);
        Assert.Equal(0, b.Repeat(-2).Count);
        AssertOvEq(b.Repeat(2), new[] { 1, 0, 1, 0 }, 0, 4);
    }

    [Fact]
    public void onset_vector_adapt_to_lcm_spacing()
    {
        var a = new OnsetVector(Ints(1, 0, 0, 0), 0, 4);
        var b = new OnsetVector(Ints(1, 0, 0, 0, 0, 0), 0, 6);
        var adapted = OnsetVector.AdaptToLcm(new[] { a, b });
        Assert.Equal(2, adapted.Count);
        Assert.Equal(12, adapted[0].Mod);
        Assert.Equal(12, adapted[1].Mod);
        AssertSeqEqual(adapted[0].MutableData, new[] { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
        AssertSeqEqual(adapted[1].MutableData, new[] { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
    }
}
