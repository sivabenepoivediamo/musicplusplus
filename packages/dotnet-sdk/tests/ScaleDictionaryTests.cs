using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class ScaleDictionaryTests
{
    [Fact]
    public void scale_database_major_pitch_classes_find_scale()
    {
        var db = new ScaleDatabase();
        var ionianPc = new[] { 60, 62, 64, 65, 67, 69, 71 };
        var matches = db.FindScale(ionianPc);
        Assert.NotEmpty(matches);
    }

    [Fact]
    public void scale_database_empty_input_no_matches()
    {
        var db = new ScaleDatabase();
        var matches = db.FindScale(Array.Empty<int>());
        Assert.Empty(matches);
    }

    [Fact]
    public void scale_database_get_all_interval_sets_has_ionian()
    {
        var db = new ScaleDatabase();
        var sets = db.GetAllIntervalSets();
        var ionianSorted = new[] { 0, 2, 4, 5, 7, 9, 11 };
        Assert.Contains(sets, l => l.SequenceEqual(ionianSorted));
    }
}
