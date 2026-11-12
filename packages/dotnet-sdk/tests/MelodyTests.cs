using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class MelodyTests
{
    [Fact]
    public void diminution_length_edge_cases()
    {
        AssertSeqEqual(Melody.Diminution(5, 0, true, true), Array.Empty<int>());
        AssertSeqEqual(Melody.Diminution(5, 1, true, true), new[] { 5 });
        var d = Melody.Diminution(3, 4, true, true);
        Assert.Equal(4, d.Count);
        Assert.Equal(3, d[0]);
        Assert.Equal(3, d[^1]);
    }

    [Fact]
    public void diminution_right_neighbor_order()
    {
        var d = Melody.Diminution(5, 4, true, false);
        Assert.Equal(4, d.Count);
        Assert.Equal(5, d[0]);
        Assert.Equal(5, d[^1]);
        Assert.True(d[1] > d[2]);
    }

    [Fact]
    public void run_and_run2()
    {
        AssertSeqEqual(Melody.Run(5, 4, true), new[] { 2, 3, 4, 5 });
        AssertSeqEqual(Melody.Run(5, 4, false), new[] { 8, 7, 6, 5 });
        AssertSeqEqual(Melody.Run2(1, 3), new[] { 1, 2, 3 });
        AssertSeqEqual(Melody.Run2(3, 1), new[] { 3, 2, 1 });
    }

    [Fact]
    public void normalize_notes_mod()
    {
        AssertSeqEqual(Melody.NormalizeNotes(new[] { -1, 12, 25 }, 12), new[] { 11, 0, 1 });
    }

    [Fact]
    public void normalize_notes_empty()
    {
        Assert.Empty(Melody.NormalizeNotes(Array.Empty<int>(), 12));
    }

    [Fact]
    public void hierarchy_negative_note_matches_pitch_class()
    {
        var chord = Ints(0, 4, 7);
        var scale = Ints(0, 2, 4, 5, 7, 9, 11);
        var chrom = Enumerable.Range(0, 12).ToList();
        var a = Melody.Hierarchy(-1, chord, scale, chrom, 12);
        Assert.Equal(11, a.Chromatic.Degree);
    }

    [Fact]
    public void hierarchy_and_triple_select()
    {
        var chord = Ints(0, 4, 7);
        var scale = Ints(0, 2, 4, 5, 7, 9, 11);
        var chrom = Enumerable.Range(0, 12).ToList();
        var a = Melody.Hierarchy(0, chord, scale, chrom, 12);
        Assert.Equal(0, a.Chord.Degree);
        Assert.Equal(0, a.Scale.Degree);
        Assert.Equal(0, a.Chromatic.Degree);

        var mods = new List<VectorModification> { new(VectorModification.Kind.Chord, 0) };
        var ts = Melody.TripleSelect(a, mods, chord, scale, chrom, 12);
        Assert.Single(ts.Results);
        Assert.Equal(0, ts.Results[0]);
        Assert.False(ts.IsOut[0]);
    }

    [Fact]
    public void triple_select_chromatic_type()
    {
        var chord = Ints(0, 4, 7);
        var scale = Ints(0, 2, 4, 5, 7, 9, 11);
        var chrom = Enumerable.Range(0, 12).ToList();
        var a = Melody.Hierarchy(0, chord, scale, chrom, 12);
        var mods = new List<VectorModification> { new(VectorModification.Kind.Chromatic, 0) };
        var ts = Melody.TripleSelect(a, mods, chord, scale, chrom, 12);
        Assert.False(ts.IsOut[0]);
        Assert.Equal(0, ts.Results[0]);
    }

    [Fact]
    public void triple_select_invalid_chord_degree()
    {
        var chord = Ints(0, 4, 7);
        var scale = Ints(0, 2, 4, 5, 7, 9, 11);
        var chrom = Enumerable.Range(0, 12).ToList();
        var a = Melody.Hierarchy(1, chord, scale, chrom, 12);
        var mods = new List<VectorModification> { new(VectorModification.Kind.Chord, 0) };
        var ts = Melody.TripleSelect(a, mods, chord, scale, chrom, 12);
        Assert.True(ts.IsOut[0]);
        Assert.Equal(0, ts.Results[0]);
    }

    [Fact]
    public void parse_modifications_length_mismatch_throws()
    {
        Assert.Throws<InvalidOperationException>(() =>
            Melody.ParseModifications(new[] { 1 }, new[] { VectorModification.Kind.Chord, VectorModification.Kind.Scale }));
    }

    [Fact]
    public void ornament_loop_and_types()
    {
        AssertSeqEqual(Melody.OrnamentLoop(new[] { 1, 2 }, 0), Array.Empty<int>());
        AssertSeqEqual(Melody.OrnamentLoop(new[] { 9, 8 }, 3), new[] { 8, 9, 8 });
        var t = Melody.OrnamentTypesLoop(new[] { VectorModification.Kind.Chord, VectorModification.Kind.Scale }, 3);
        Assert.Equal(3, t.Count);
    }

    [Fact]
    public void join_int_vector()
    {
        Assert.Equal("1, 2, 3", Melody.Join(new[] { 1, 2, 3 }));
        Assert.Equal("", Melody.Join(Array.Empty<int>()));
    }

    [Fact]
    public void parse_modifications_single_type_broadcast()
    {
        var m = Melody.ParseModifications(new[] { 1, 2 }, new[] { VectorModification.Kind.Scale });
        Assert.Equal(2, m.Count);
        Assert.Equal(VectorModification.Kind.Scale, m[0].Type);
        Assert.Equal(2, m[1].Delta);
    }

    [Fact]
    public void parse_modifications_paired_types()
    {
        var m = Melody.ParseModifications(
            new[] { 2, -1 },
            new[] { VectorModification.Kind.Scale, VectorModification.Kind.Chromatic });
        Assert.Equal(2, m.Count);
        Assert.Equal(VectorModification.Kind.Scale, m[0].Type);
        Assert.Equal(VectorModification.Kind.Chromatic, m[1].Type);
        Assert.Equal(2, m[0].Delta);
        Assert.Equal(-1, m[1].Delta);
    }
}
