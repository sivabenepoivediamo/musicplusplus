using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class VectorDivisionSafetyTests
{
    [Fact]
    public void position_vector_scalar_div_mod_zero_throws()
    {
        var p = new PositionVector(Ints(4, 8, 12));
        Assert.Throws<ArgumentException>(() => _ = p / 0);
        Assert.Throws<ArgumentException>(() => _ = p % 0);
    }

    [Fact]
    public void interval_vector_scalar_div_mod_zero_throws()
    {
        var iv = new IntervalVector(Ints(2, 3, 5));
        Assert.Throws<ArgumentException>(() => _ = iv / 0);
        Assert.Throws<ArgumentException>(() => _ = iv % 0);
    }

    [Fact]
    public void position_vector_componentwise_div_mod_zero_throws()
    {
        var a = new PositionVector(Ints(10, 20));
        var b = new PositionVector(Ints(2, 0));
        Assert.Throws<ArgumentException>(() => _ = a / b);
        Assert.Throws<ArgumentException>(() => _ = a % b);
        var z = Ints(1, 0);
        Assert.Throws<ArgumentException>(() => _ = a / z);
    }

    [Fact]
    public void interval_vector_componentwise_div_mod_zero_throws()
    {
        var a = new IntervalVector(Ints(4, 6));
        var b = new IntervalVector(Ints(2, 0));
        Assert.Throws<ArgumentException>(() => _ = a / b);
        Assert.Throws<ArgumentException>(() => _ = a % b);
    }
}
