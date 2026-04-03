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

    [Fact]
    public void scale_database_try_get_scale_by_string_keys()
    {
        var db = ScaleDatabase.Shared;
        const string sheet = "Major and minor scales";
        Assert.True(db.TryGetScale(sheet, "Ionian (Major)", out var m));
        Assert.Equal(sheet, m.SheetName);
        Assert.Equal(new[] { 0, 2, 4, 5, 7, 9, 11 }, m.ToPitchClasses());
    }

    [Fact]
    public void scale_database_get_scale_names_in_major_sheet()
    {
        var db = new ScaleDatabase();
        var names = db.GetScaleNames("Major and minor scales");
        Assert.Contains("Dorian", names);
        Assert.True(names.SequenceEqual(names.Order(StringComparer.Ordinal)));
    }

    [Fact]
    public void scale_database_get_sheets_contains_expected_categories()
    {
        var db = new ScaleDatabase();
        var sheets = db.GetSheetNames();
        Assert.Contains("Jazz Scales", sheets);
        Assert.Contains("Indian Scales", sheets);
    }

    [Fact]
    public void scale_database_shared_same_count_as_fresh_instance()
    {
        Assert.Equal(new ScaleDatabase().Count, ScaleDatabase.Shared.Count);
    }

    [Fact]
    public void scale_database_readonly_dictionary_indexer_matches_try_get()
    {
        var db = new ScaleDatabase();
        var key = ("Jazz Scales", "Bebop");
        Assert.True(db.ScalesBySheetAndName.TryGetValue(key, out var fromDict));
        Assert.True(db.TryGetScale("Jazz Scales", "Bebop", out var fromTry));
        Assert.Equal(fromTry, fromDict);
        Assert.Equal(fromDict, db.ScalesBySheetAndName[key]);
    }
}
