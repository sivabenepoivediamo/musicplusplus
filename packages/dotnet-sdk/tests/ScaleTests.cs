using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class ScaleTests
{
    [Fact]
    public void scale_class_examples()
    {
        var intervalGen = new IntervalVector(Ints(2, 2, 1, 2, 2, 2, 1), 0, 12);
        var scale1 = new Scale(intervalGen, 0, 0, 0, false, 0, false, 0);
        AssertIvEq(scale1.Intervals, new[] { 2, 2, 1, 2, 2, 2, 1 }, 0, 12);
        AssertPvEq(scale1.ToPositions(), new[] { 0, 2, 4, 5, 7, 9, 11 }, 12);
        Assert.Equal(0, scale1.Root);
        Assert.Equal(0, scale1.Mode);
        Assert.False(scale1.Invert);
        Assert.False(scale1.Mirror);

        var posGen = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12);
        var scale2 = new Scale(posGen, 3, 1, 0, false, 0, false, 0);
        AssertIvEq(scale2.Intervals, new[] { 2, 1, 2, 2, 2, 1, 2 }, 3, 12);
        AssertPvEq(scale2.ToPositions(), new[] { 3, 5, 6, 8, 10, 12, 13 }, 12);
        Assert.Equal(3, scale2.Root);
        Assert.Equal(1, scale2.Mode);
        Assert.True(scale2.IsFromPositions);

        scale1.SetRoot(5);
        AssertPvEq(scale1.ToPositions(), new[] { 5, 7, 9, 10, 12, 14, 16 }, 12);

        scale1.SetMode(2);
        AssertIvEq(scale1.Intervals, new[] { 1, 2, 2, 2, 1, 2, 2 }, 5, 12);
        AssertPvEq(scale1.ToPositions(), new[] { 5, 6, 8, 10, 12, 13, 15 }, 12);

        scale1.SetInvert(true);
        scale1.SetInversionAxis(4);
        AssertIvEq(scale1.Intervals, new[] { 2, 2, 2, 1, 2, 2, 1 }, 5, 12);
        AssertPvEq(scale1.ToPositions(), new[] { 5, 7, 9, 11, 12, 14, 16 }, 12);
        Assert.Equal(4, scale1.InversionAxis);
        Assert.True(scale1.Invert);

        var scale3 = new Scale(posGen, 0, 0, 0, false, 0, false, 0);
        AssertIvEq(scale3.ToIntervals(), new[] { 2, 2, 1, 2, 2, 2, 1 }, 0, 12);
        AssertPvEq(scale3.ToPositions(), new[] { 0, 2, 4, 5, 7, 9, 11 }, 12);
    }

    [Fact]
    public void autoscale_examples()
    {
        var scale1 = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11));
        AssertPvEq(Automations.AutoScale(scale1, new[] { 61, 70 }), new[] { 1, 2, 4, 5, 7, 9, 10 }, 12);

        var scale2 = new PositionVector(Ints(0, 2, 4, 7, 9));
        AssertPvEq(Automations.AutoScale(scale2, new[] { 61, 65 }), new[] { 1, 2, 5, 7, 9 }, 12);

        var scale3 = new PositionVector(Ints(7, 9, 11, 12, 14, 16, 18));
        AssertPvEq(Automations.AutoScale(scale3, new[] { 63, 70 }), new[] { 7, 10, 11, 12, 14, 15, 18 }, 12);

        var scale4 = new PositionVector(Ints(0, 2, 4, 6, 8, 10));
        AssertPvEq(Automations.AutoScale(scale4, new[] { 61, 63 }), new[] { 1, 3, 4, 6, 8, 10 }, 12);

        var scale5 = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11));
        AssertPvEq(Automations.AutoScale(scale5, new[] { 63, 66 }), new[] { 0, 2, 3, 5, 6, 9, 11 }, 12);

        var scale6 = new PositionVector(Ints(1, 3, 5, 6, 8, 10, 12));
        AssertPvEq(Automations.AutoScale(scale6, new[] { 67 }), new[] { 1, 3, 5, 7, 8, 10, 12 }, 12);

        var scale7 = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11));
        AssertPvEq(Automations.AutoScale(scale7, new[] { 70 }), new[] { 0, 2, 4, 5, 7, 9, 10 }, 12);
    }

    [Fact]
    public void scale_dictionary_examples()
    {
        var db = new ScaleDatabase();
        var majorC = new[] { 0, 2, 4, 5, 7, 9, 11 };
        var mixolydianG = new[] { 7, 9, 11, 12, 14, 16, 17 };
        var majorPentatonicC = new[] { 0, 2, 4, 7, 9 };
        var wholeToneC = new[] { 0, 2, 4, 6, 8, 10 };
        var chromaticC = Enumerable.Range(0, 12).ToArray();
        var harmonicMajorC = new[] { 0, 2, 4, 5, 7, 8, 11 };
        var noMatch = new[] { 0, 1, 4, 8 };

        Assert.Single(db.FindScale(majorC));
        Assert.Equal(2, db.FindScale(mixolydianG).Count);
        Assert.Single(db.FindScale(majorPentatonicC));
        Assert.Single(db.FindScale(wholeToneC));
        Assert.Single(db.FindScale(chromaticC));
        Assert.Single(db.FindScale(harmonicMajorC));
        Assert.Empty(db.FindScale(noMatch));

        AssertStringSeqEqual(
            ScaleLabels(db.FindScale(majorC)),
            new[] { "Major and minor scales|Ionian (Major)" });
        AssertStringSeqEqual(
            ScaleLabels(db.FindScale(mixolydianG)),
            new[] { "Indian Scales|Raga Khamach (desc)", "Major and minor scales|Mixolydian" });
        AssertStringSeqEqual(
            ScaleLabels(db.FindScale(majorPentatonicC)),
            new[] { "Pentatonic Scales|Major Pentatonic" });
        AssertStringSeqEqual(
            ScaleLabels(db.FindScale(wholeToneC)),
            new[] { "Symmetrical scales|Whole-Tone" });
        AssertStringSeqEqual(
            ScaleLabels(db.FindScale(chromaticC)),
            new[] { "Symmetrical scales|Chromatic" });
        AssertStringSeqEqual(
            ScaleLabels(db.FindScale(harmonicMajorC)),
            new[] { "Modal Scales|Harmonic Major" });
        Assert.Empty(ScaleLabels(db.FindScale(noMatch)));

        Assert.Equal("C", ScaleDictionaryUtil.GetRootNote(majorC));
        Assert.Equal("D", ScaleDictionaryUtil.GetRootNote(Ints(2, 4, 6, 7, 9, 11, 13)));
    }

    [Fact]
    public void scale_params_chaining_and_merge()
    {
        var a = new ScaleParams();
        var b = new ScaleParams();
        b.WithRoot(7).WithMode(3).WithDegree(2).WithInvert(true).WithInversionAxis(4).WithMirror(true).WithMirrorAxis(1);
        a.Merge(b);
        Assert.Equal(7, a.Root);
        Assert.Equal(3, a.Mode);
        Assert.Equal(2, a.Degree);
        Assert.True(a.Invert);
        Assert.Equal(4, a.InversionAxis);
        Assert.True(a.Mirror);
        Assert.Equal(1, a.MirrorAxis);

        var c = new ScaleParams();
        c.WithRoot(1).WithMode(0);
        Assert.Equal(1, c.Root);
        Assert.Equal(0, c.Mode);
    }
}
