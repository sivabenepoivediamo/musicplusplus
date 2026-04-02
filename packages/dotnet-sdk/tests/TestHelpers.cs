using Musicpp;

namespace Musicplusplus.Tests;

public static class TestHelpers
{
    public static List<int> Ints(params int[] a) => a.ToList();

    public static void AssertPvEq(PositionVector pv, int[] data, int mod = -1)
    {
        Assert.Equal(data, pv.MutableData);
        if (mod >= 0)
            Assert.Equal(mod, pv.Mod);
    }

    public static void AssertIvEq(IntervalVector iv, int[] data, int offset, int mod)
    {
        Assert.Equal(data, iv.MutableData);
        Assert.Equal(offset, iv.Offset);
        if (mod >= 0)
            Assert.Equal(mod, iv.Mod);
    }

    public static void AssertOvEq(OnsetVector ov, int[] data, int offset, int mod)
    {
        Assert.Equal(data, ov.MutableData);
        Assert.Equal(offset, ov.Offset);
        if (mod >= 0)
            Assert.Equal(mod, ov.Mod);
    }

    public static void AssertSeqEqual(IEnumerable<int> a, IEnumerable<int> b) =>
        Assert.Equal(a.ToList(), b.ToList());

    public static void AssertStringSeqEqual(IEnumerable<string> a, IEnumerable<string> b) =>
        Assert.Equal(a.ToList(), b.ToList());

    public static List<int> NormalizePositions(PositionVector positions)
    {
        var data = positions.MutableData.ToList();
        if (data.Count == 0)
            return data;
        var anchor = data[0];
        for (var i = 0; i < data.Count; i++)
            data[i] -= anchor;
        return data;
    }

    public static List<string> ScaleLabels(IEnumerable<ScaleMatch> results) =>
        results.Select(r => r.SheetName + "|" + r.ScaleName).OrderBy(x => x).ToList();
}
