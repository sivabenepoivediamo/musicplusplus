using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class MeasuresUtilTests
{
    [Fact]
    public void differences_empty_and_short()
    {
        Assert.Empty(Measures.Differences(new[] { 5 }));
        Assert.Empty(Measures.Differences(Array.Empty<int>()));
        AssertSeqEqual(Measures.Differences(new[] { 1, 4, 9 }), new[] { 3, 5 });
    }

    [Fact]
    public void geodesic_distance_cycle()
    {
        Assert.Equal(6, Measures.GeodesicDistance(0, 6, 12));
        Assert.Equal(2, Measures.GeodesicDistance(11, 1, 12));
        Assert.Equal(0, Measures.GeodesicDistance(5, 5, 12));
    }

    [Fact]
    public void geodesic_distances_pairing_order()
    {
        var pv = new PositionVector(Ints(0, 3, 7), 12, 12);
        var d = Measures.GeodesicDistances(pv);
        Assert.Equal(3, d.Count);
        Assert.Equal(Measures.GeodesicDistance(0, 3, 12), d[0]);
        Assert.Equal(Measures.GeodesicDistance(0, 7, 12), d[1]);
        Assert.Equal(Measures.GeodesicDistance(3, 7, 12), d[2]);
    }

    [Fact]
    public void calculate_occurrences()
    {
        var m = Measures.CalculateOccurrences(new[] { 1, 1, 2 });
        Assert.Equal(2, m[1]);
        Assert.Equal(1, m[2]);
        Assert.Empty(Measures.CalculateOccurrences(Array.Empty<int>()));
    }

    [Fact]
    public void is_winograd_deep_and_erdos_deep()
    {
        var good = new SortedDictionary<int, int> { [1] = 3, [2] = 1, [3] = 2 };
        Assert.True(Measures.IsWinogradDeep(good, 4));
        Assert.True(Measures.IsErdosDeep(good));

        var badDup = new SortedDictionary<int, int> { [1] = 2, [2] = 2 };
        Assert.False(Measures.IsErdosDeep(badDup));
    }

    [Fact]
    public void calculate_regression_evenness()
    {
        var e = Measures.CalculateRegressionEvenness(new[] { 0, 3, 6 }, 9);
        Assert.Equal(0.0, e, 9);
    }

    [Fact]
    public void calculate_regression_evenness_empty_rhythm()
    {
        Assert.Equal(0.0, Measures.CalculateRegressionEvenness(Array.Empty<int>(), 12), 12);
    }

    [Fact]
    public void is_balanced_two_antipodal_notes()
    {
        var pv = new PositionVector(Ints(0, 6), 12, 12);
        Assert.True(Measures.IsBalanced(pv));
    }

    [Fact]
    public void is_prime()
    {
        Assert.False(Measures.IsPrime(0));
        Assert.False(Measures.IsPrime(1));
        Assert.True(Measures.IsPrime(2));
        Assert.True(Measures.IsPrime(17));
        Assert.False(Measures.IsPrime(18));
    }

    [Fact]
    public void generate_sequence_mod()
    {
        var seq = Measures.Generate(2, 5, 7, false);
        AssertSeqEqual(seq, new[] { 0, 2, 4, 6, 1 });
    }

    [Fact]
    public void is_generated_finds_multiplier()
    {
        var g = Measures.Generate(1, 5, 5, false);
        var p = Measures.IsGenerated(g, 5);
        Assert.True(p.Found);
        Assert.Equal(1, p.Multiplier);
    }

    [Fact]
    public void is_euclidean_empty_intervals_is_false()
    {
        var one = new PositionVector(Ints(0), 12, 12);
        Assert.False(Measures.IsEuclidean(one, 12));
    }

    [Fact]
    public void compute_entropy_counts_slot_indices()
    {
        var sparse = new PositionVector(Ints(0), 12, 12);
        var dense = new PositionVector(Ints(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11), 12, 12);
        Assert.Equal(0.413817, Measures.ComputeEntropy(sparse), 5);
        Assert.Equal(0.0, Measures.ComputeEntropy(dense), 9);
    }

    [Fact]
    public void compute_longest_subsequence_and_transition_complexity()
    {
        var pv = new PositionVector(Ints(0, 2, 4), 12, 12);
        Assert.Equal(7, Measures.ComputeLongestSubsequence(pv));
        Assert.True(Measures.ComputeTransitionComplexity(pv, 12) > 0);
    }

    [Fact]
    public void calculate_distribution_spectra_major_third()
    {
        var major = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var spectra = Measures.CalculateDistributionSpectra(major);
        Assert.Equal(6, spectra.Count);
        var third = spectra[2];
        Assert.Contains(5, third);
        Assert.Contains(6, third);
    }

    [Fact]
    public void calculate_spectrum_widths_and_variation()
    {
        var spectra = new List<SortedSet<int>> { new() { 1, 3 }, new() };
        var w = Measures.CalculateSpectrumWidths(spectra);
        Assert.Equal(2, w[0]);
        Assert.Equal(0, w[1]);
        Assert.Equal(2.0 / 7.0, Measures.CalculateSpectrumVariation(w, 7), 9);
    }

    [Fact]
    public void augmented_triad_is_balanced_mod_twelve()
    {
        var aug = new PositionVector(Ints(0, 4, 8), 12, 12);
        Assert.True(Measures.IsBalanced(aug));
    }

    [Fact]
    public void whole_tone_has_rotational_symmetry_two()
    {
        var wt = new PositionVector(Ints(0, 2, 4, 6, 8, 10), 12, 12);
        var axes = Measures.FindRotationalSymmetryAxes(wt);
        Assert.Contains(2, axes);
    }

    [Fact]
    public void tritone_pair_palindrome_axis_zero()
    {
        var tritone = new PositionVector(Ints(0, 6), 12, 12);
        Assert.True(Measures.IsPalindrome(tritone));
    }

    [Fact]
    public void diminished_seventh_not_chiral()
    {
        var dim = new PositionVector(Ints(0, 3, 6, 9), 12, 12);
        Assert.False(Measures.IsChiral(dim));
    }

    [Fact]
    public void calculate_rhythmic_oddity_singleton()
    {
        var one = new PositionVector(Ints(0), 12, 12);
        Assert.Equal(0, Measures.CalculateRhythmicOddity(one));
    }
}
