using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class PositionVectorExtendedTests
{
    [Fact]
    public void position_vector_compound_assignment_scalars()
    {
        var p = new PositionVector(Ints(1, 2, 3), 12, 12);
        for (var i = 0; i < p.MutableData.Count; i++)
            p.MutableData[i] += 10;
        Assert.Equal(11, p[0]);
        Assert.Equal(12, p[1]);
        for (var i = 0; i < p.MutableData.Count; i++)
            p.MutableData[i] -= 1;
        Assert.Equal(10, p[0]);
        for (var i = 0; i < p.MutableData.Count; i++)
            p.MutableData[i] *= 2;
        Assert.Equal(20, p[0]);
    }

    [Fact]
    public void position_vector_element_matches_bracket()
    {
        var p = new PositionVector(Ints(10, 20, 30), 12, 12);
        Assert.Equal(p.Element(0), p[0]);
        Assert.Equal(p.Element(3), p[3]);
        Assert.Equal(p.Element(-1), p[-1]);
    }

    [Fact]
    public void position_vector_scalar_aliases()
    {
        var p = new PositionVector(Ints(2, 4), 12, 12);
        AssertPvEq(p.ScalarSum(3), new[] { 5, 7 }, 12);
        AssertPvEq(p.ScalarProduct(2), new[] { 4, 8 }, 12);
    }

    [Fact]
    public void position_vector_repeat_non_positive()
    {
        var p = new PositionVector(Ints(1, 2), 12, 12);
        Assert.Equal(0, p.Repeat(0).Count);
        Assert.Equal(0, p.Repeat(-1).Count);
        AssertPvEq(p.Repeat(2), new[] { 1, 2, 1, 2 }, 12);
    }

    [Fact]
    public void position_vector_resize_slice()
    {
        var p = new PositionVector(Ints(10, 20, 30, 40), 12, 12);
        AssertPvEq(p.Resize(0, 2), new[] { 10, 20, 30 }, 12);
        AssertPvEq(p.Resize(3, 1), new[] { 40, 30, 20 }, 12);
    }

    [Fact]
    public void position_vector_adapt_to_lcm()
    {
        Assert.Empty(PositionVector.AdaptToLcm(new List<PositionVector>()));

        var a = new PositionVector(Ints(0, 2), 4, 4, true, false);
        var b = new PositionVector(Ints(0, 3), 6, 6, true, false);
        var adapted = PositionVector.AdaptToLcm(new[] { a, b });
        Assert.Equal(2, adapted.Count);
        Assert.Equal(12, adapted[0].Mod);
        Assert.Equal(12, adapted[1].Mod);
        AssertSeqEqual(adapted[0].MutableData, new[] { 0, 6 });
        AssertSeqEqual(adapted[1].MutableData, new[] { 0, 6 });

        var same = PositionVector.AdaptToLcm(new[] { a, a });
        Assert.Equal(2, same.Count);
        Assert.Equal(4, same[0].Mod);
    }

    [Fact]
    public void position_vector_set_mod_updates_range()
    {
        var p = new PositionVector(Ints(0, 6), 12, 12);
        p.SetMod(24);
        Assert.Equal(24, p.Mod);
    }

    [Fact]
    public void position_vector_rotate_empty_is_unchanged()
    {
        var empty = new PositionVector(new List<int>(), 12, 12, true, false);
        AssertPvEq(empty.Rotate(5), Array.Empty<int>(), 12);
    }
}
