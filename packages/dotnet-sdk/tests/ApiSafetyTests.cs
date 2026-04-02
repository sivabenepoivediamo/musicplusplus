using Musicpp;
using Musicpp.Slonimsky;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class ApiSafetyTests
{
    [Fact]
    public void normalize_empty_vector_throws()
    {
        Assert.Throws<ArgumentException>(() => Distances.Normalize(Array.Empty<int>()));
    }

    [Fact]
    public void onset_vector_constructor_rejects_invalid_pattern()
    {
        Assert.Throws<ArgumentException>(() => _ = new OnsetVector(Ints(1, 2), 0, 4));
    }

    [Fact]
    public void onset_vector_validate_after_mutating_data()
    {
        var bv = new OnsetVector(Ints(1, 0, 1, 0), 0, 4);
        bv.MutableData[1] = 7;
        Assert.Throws<ArgumentException>(() => bv.ValidateOnsetPattern());
    }

    [Fact]
    public void onset_vector_scalar_multiply_non_positive_throws()
    {
        var bv = new OnsetVector(Ints(1, 0, 0, 0), 0, 4);
        Assert.Throws<ArgumentException>(() => _ = bv * 0);
        Assert.Throws<ArgumentException>(() => _ = bv * -1);
    }

    [Fact]
    public void onset_vector_scalar_multiply_one_is_identity()
    {
        var bv = new OnsetVector(Ints(1, 0, 1, 0), 0, 4);
        var scaled = bv * 1;
        AssertSeqEqual(scaled.MutableData, bv.MutableData);
        Assert.Equal(bv.Offset, scaled.Offset);
        Assert.Equal(bv.Mod, scaled.Mod);
    }

    [Fact]
    public void onset_vector_scalar_divide_invalid_throws()
    {
        var bv = new OnsetVector(Ints(1, 0, 1, 0), 0, 4);
        Assert.Throws<ArgumentException>(() => _ = bv / 0);
        Assert.Throws<ArgumentException>(() => _ = bv / 5);
        Assert.Throws<ArgumentException>(() => _ = bv / 3);
    }

    [Fact]
    public void vector_set_multiply_onset_non_positive_scalar()
    {
        var s = VectorSet.FromOnset(Ints(1, 0, 1, 0), 0, 4);
        Assert.Throws<ArgumentException>(() => _ = s.MultiplyOnset(0));
    }

    [Fact]
    public void vector_set_divide_onset_propagates_onset_rules()
    {
        var s = VectorSet.FromOnset(Ints(1, 0, 1, 0), 0, 4);
        Assert.Throws<ArgumentException>(() => _ = s.DivideOnset(3));
    }

    [Fact]
    public void position_vector_componentwise_division_empty_divisor()
    {
        var p = new PositionVector(Ints(4, 8), 12, 12);
        Assert.Throws<ArgumentException>(() => _ = p / Array.Empty<int>());
        Assert.Throws<ArgumentException>(() => _ = p % Array.Empty<int>());
    }

    [Fact]
    public void interval_vector_componentwise_division_empty_divisor()
    {
        var iv = new IntervalVector(Ints(2, 2, 2), 0, 12);
        Assert.Throws<ArgumentException>(() => _ = iv / Array.Empty<int>());
        Assert.Throws<ArgumentException>(() => _ = iv % Array.Empty<int>());
    }

    [Fact]
    public void triple_select_invalid_modification_type_throws()
    {
        var chord = Ints(0, 4, 7);
        var scale = Ints(0, 2, 4, 5, 7, 9, 11);
        var chrom = Enumerable.Range(0, 12).ToList();
        var a = Melody.Hierarchy(0, chord, scale, chrom, 12);
        var mods = new List<VectorModification> { new((VectorModification.Kind)99, 0) };
        Assert.Throws<InvalidOperationException>(() => Melody.TripleSelect(a, mods, chord, scale, chrom, 12));
    }

    [Fact]
    public void rhythm_euclidean_non_positive_args_throws()
    {
        Assert.Throws<ArgumentException>(() => RhythmAlgorithms.EuclideanIntervals(12, 0));
        Assert.Throws<ArgumentException>(() => RhythmAlgorithms.EuclideanIntervals(0, 4));
        Assert.Throws<ArgumentException>(() => RhythmAlgorithms.EuclideanIntervals(-1, 3));
    }

    [Fact]
    public void positions_to_onset_non_positive_range_returns_empty_pattern()
    {
        var pv = new PositionVector(Ints(0, 4, 7), 12, 12);
        pv.SetEffectiveRange(0);
        var b = MusicVectors.PositionsToOnset(pv);
        Assert.Empty(b.MutableData);
        Assert.Equal(pv.Mod, b.Mod);
    }

    [Fact]
    public void align_empty_or_bad_range_throws()
    {
        var a = new PositionVector(Ints(0, 4, 7), 12, 12);
        var b = new PositionVector(Ints(2, 5, 9), 12, 12);
        var empty = new PositionVector(new List<int>(), 12, 12, true, false);
        Assert.Throws<ArgumentException>(() => MatrixDistanceOps.Align(empty, b));
        Assert.Throws<ArgumentException>(() => MatrixDistanceOps.Align(a, empty));
        var z = new PositionVector(Ints(0, 4, 7), 12, 12);
        z.SetEffectiveRange(0);
        Assert.Throws<ArgumentException>(() => MatrixDistanceOps.Align(z, b));
    }

    [Fact]
    public void slonimsky_min_interval_too_short_throws()
    {
        Assert.Throws<ArgumentException>(() => SlonimskySequences.MinInterval(Array.Empty<int>()));
        Assert.Throws<ArgumentException>(() => SlonimskySequences.MinInterval(new[] { 0 }));
    }

    [Fact]
    public void slonimsky_intervals_short_sequences()
    {
        Assert.Empty(SlonimskySequences.Intervals(Array.Empty<int>()));
        Assert.Empty(SlonimskySequences.Intervals(new[] { 5 }));
    }

    [Fact]
    public void matrix_distance_empty_at_throws_out_of_range()
    {
        var modal = new ModalMatrixDistance<PositionVector>();
        var trans = new TranspositionMatrixDistance(new List<(PositionVector, int, double)>());
        var roto = new RelativeModeMatrixDistance(new List<(PositionVector, int, double)>(), 0);
        var sel = new ModalSelectionMatrixDistance<PositionVector>(new List<(PositionVector, int, double)>());
        var mrt = new ModalRelativeModeMatrixDistance(new List<(int, int, PositionVector, double)>());
        Assert.Throws<ArgumentOutOfRangeException>(() => _ = modal[0]);
        Assert.Throws<ArgumentOutOfRangeException>(() => _ = trans[0]);
        Assert.Throws<ArgumentOutOfRangeException>(() => _ = roto[0]);
        Assert.Throws<ArgumentOutOfRangeException>(() => _ = sel[0]);
        Assert.Throws<ArgumentOutOfRangeException>(() => _ = mrt[0]);
    }
}
