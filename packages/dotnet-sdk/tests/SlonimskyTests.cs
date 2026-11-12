using Musicpp.Slonimsky;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class SlonimskyTests
{
    [Fact]
    public void slonimsky_intervals_helper()
    {
        AssertSeqEqual(SlonimskySequences.Intervals(new[] { 0, 6, 12 }), new[] { 6, 6 });
        AssertSeqEqual(SlonimskySequences.Intervals(new[] { 1, 3, 7 }), new[] { 2, 4 });
    }

    [Fact]
    public void slonimsky_examples()
    {
        var x = new[] { 0, 6, 12 };

        Assert.Equal(6, SlonimskySequences.MinInterval(x));
        AssertSeqEqual(SlonimskySequences.Interpolation(x, 2), new[] { 0, 2, 6, 8, 12 });
        AssertSeqEqual(SlonimskySequences.Interpolation(x, new[] { 2, 4 }), new[] { 0, 2, 4, 6, 8, 10, 12 });
        AssertSeqEqual(SlonimskySequences.Interpolation(x, new[] { 1, 2, 4 }), new[] { 0, 1, 2, 4, 6, 7, 8, 10, 12 });

        AssertSeqEqual(SlonimskySequences.SymmetricInterpolation(x, 2), new[] { 0, 2, 6, 10, 6, 8, 12 });
        AssertSeqEqual(SlonimskySequences.SymmetricInterpolation(x, new[] { 2, 4 }), new[] { 0, 2, 4, 6, 8, 10, 6, 8, 10, 12 });

        AssertSeqEqual(
            SlonimskySequences.AsymmetricInterpolation(x, new[] { 2, 4 }, new[] { 1, 3 }),
            new[] { 0, 2, 4, 6, 9, 11, 6, 8, 10, 12 });

        AssertSeqEqual(SlonimskySequences.Ultrapolation(x, 2), new[] { 0, 8, 6, 14, 12 });
        AssertSeqEqual(SlonimskySequences.Ultrapolation(x, new[] { 2, 4 }), new[] { 0, 10, 8, 6, 16, 14, 12 });

        AssertSeqEqual(SlonimskySequences.Infrapolation(x, 2), new[] { 0, -2, 6, 4, 12 });
        AssertSeqEqual(SlonimskySequences.Infrapolation(x, new[] { 2, 4 }), new[] { 0, -2, -4, 6, 4, 2, 12 });
        AssertSeqEqual(SlonimskySequences.Infrapolation(x, new[] { 1, 3, 5 }), new[] { 0, -1, -3, -5, 6, 5, 3, 1, 12 });
        AssertSeqEqual(SlonimskySequences.InfrapolationPermutation(3), new[] { 2, 0, 1 });

        AssertSeqEqual(SlonimskySequences.InfraInterpolation(x, 2, 3), new[] { 0, -2, 3, 6, 4, 9, 12 });
        AssertSeqEqual(SlonimskySequences.InterInfrapolation(x, 3, 2), new[] { 0, 3, -2, 6, 9, 4, 12 });
        AssertSeqEqual(SlonimskySequences.InfraUltrapolation(x, 2, 3), new[] { 0, -2, 9, 6, 4, 15, 12 });
        AssertSeqEqual(SlonimskySequences.InterUltrapolation(x, 2, 3), new[] { 0, 2, 3, 6, 8, 9, 12 });
        AssertSeqEqual(SlonimskySequences.UltraInterpolation(x, 2, 3), new[] { 0, 8, 3, 6, 14, 9, 12 });
        AssertSeqEqual(SlonimskySequences.InfraInterUltrapolation(x, 1, 1, 1), new[] { 0, -1, 1, 7, 6, 5, 7, 13, 12 });
        AssertSeqEqual(SlonimskySequences.InterInfraInterpolation(x, 1, 2, 3), new[] { 0, 5, -2, 3, 6, 11, 4, 9, 12 });
        AssertSeqEqual(SlonimskySequences.UltraInfraInterpolation(x, 1, 2, 3), new[] { 0, 7, -2, 3, 6, 13, 4, 9, 12 });
    }
}
