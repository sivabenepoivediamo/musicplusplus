using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class MathUtilTests
{
    [Fact]
    public void euclidean_division_zero_divisor_throws()
    {
        Assert.Throws<ArgumentException>(() => MathUtil.EuclideanDivision(1, 0));
        Assert.Throws<ArgumentException>(() => MathUtil.EuclideanDivision(0, 0));
    }

    [Fact]
    public void euclidean_division_positive_divisor()
    {
        var r = MathUtil.EuclideanDivision(17, 5);
        Assert.Equal(3, r.Quotient);
        Assert.Equal(2, r.Remainder);

        r = MathUtil.EuclideanDivision(15, 5);
        Assert.Equal(3, r.Quotient);
        Assert.Equal(0, r.Remainder);
    }

    [Fact]
    public void euclidean_division_negative_dividend()
    {
        var r = MathUtil.EuclideanDivision(-17, 5);
        Assert.Equal(3, r.Remainder);
        Assert.Equal(-17, r.Quotient * 5 + r.Remainder);
        Assert.True(r.Remainder >= 0 && r.Remainder < 5);

        r = MathUtil.EuclideanDivision(-1, 3);
        Assert.Equal(2, r.Remainder);
        Assert.Equal(-1, r.Quotient);
    }

    [Fact]
    public void euclidean_division_negative_divisor_identity()
    {
        var r = MathUtil.EuclideanDivision(17, -5);
        Assert.Equal(17, r.Quotient * (-5) + r.Remainder);
        Assert.True(r.Remainder >= 0);
        Assert.True(r.Remainder < 5);

        var r2 = MathUtil.EuclideanDivision(-7, -3);
        Assert.Equal(-7, r2.Quotient * (-3) + r2.Remainder);
        Assert.True(r2.Remainder >= 0);
        Assert.True(r2.Remainder < 3);
    }

    [Fact]
    public void gcd_cases()
    {
        Assert.Equal(6, MathUtil.Gcd(48, 18));
        Assert.Equal(6, MathUtil.Gcd(18, 48));
        Assert.Equal(1, MathUtil.Gcd(7, 13));
        Assert.Equal(5, MathUtil.Gcd(0, 5));
        Assert.Equal(5, MathUtil.Gcd(5, 0));
        Assert.Equal(6, MathUtil.Gcd(-12, 18));
        Assert.Equal(1, MathUtil.Gcd(1, 1));
    }

    [Fact]
    public void lcm_cases()
    {
        Assert.Equal(1, MathUtil.Lcm(new List<int>()));
        Assert.Equal(7, MathUtil.Lcm(Ints(7)));
        Assert.Equal(7, MathUtil.Lcm(Ints(-7)));
        Assert.Equal(12, MathUtil.Lcm(Ints(4, 6)));
        Assert.Equal(60, MathUtil.Lcm(Ints(3, 4, 5)));
        Assert.Equal(120, MathUtil.Lcm(Ints(8, 12, 15)));
        Assert.Equal(0, MathUtil.Lcm(Ints(0, 5)));
        Assert.Equal(0, MathUtil.Lcm(Ints(5, 0)));
    }
}
