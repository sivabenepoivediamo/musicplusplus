using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class AnalysisTests
{
    [Fact]
    public void distance_examples()
    {
        var a = new PositionVector(Ints(0, 4, 7, 11));
        var b = new PositionVector(Ints(7, 11, 14, 17));
        var euclidean = Distances.EuclideanDistance(a, b);
        var manhattan = Distances.ManhattanDistance(a, b);
        var edit = Distances.EditDistance(a, b);
        var weighted = Distances.WeightedTransformationDistance(a, b);
        var diff = Distances.Difference(a, b);
        var hamming = Distances.HammingDistance(a, b);

        Assert.Equal(13.5277, euclidean, 4);
        Assert.Equal(27, manhattan);
        Assert.Equal(4, edit);
        Assert.Equal(27, weighted);
        Assert.Equal(-27, diff);
        Assert.Equal(4, hamming);

        var steps = Distances.TransformationSteps(a.MutableData, b.MutableData);
        Assert.Equal(4, steps.Count);
        Assert.Equal(0, steps[0].Type);
        Assert.Equal(0, steps[0].Pos);
        Assert.Equal(7, steps[0].Val);
        Assert.Equal(0, steps[1].Type);
        Assert.Equal(1, steps[1].Pos);
        Assert.Equal(7, steps[1].Val);
        Assert.Equal(0, steps[2].Type);
        Assert.Equal(2, steps[2].Pos);
        Assert.Equal(7, steps[2].Val);
        Assert.Equal(0, steps[3].Type);
        Assert.Equal(3, steps[3].Pos);
        Assert.Equal(6, steps[3].Val);
    }

    [Fact]
    public void measure_examples()
    {
        var cMajor = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var distances = Measures.GeodesicDistances(cMajor);
        var occurrences = Measures.CalculateOccurrences(distances);
        var spectra = Measures.CalculateDistributionSpectra(cMajor);
        var widths = Measures.CalculateSpectrumWidths(spectra);
        var rotationalAxes = Measures.FindRotationalSymmetryAxes(cMajor);
        var reflectiveAxes = Measures.FindReflectiveSymmetryAxes(cMajor);
        var rawPositions = cMajor.MutableData;

        Assert.Equal(21, distances.Count);
        Assert.Equal(2, occurrences[1]);
        Assert.Equal(5, occurrences[2]);
        Assert.Equal(4, occurrences[3]);
        Assert.Equal(3, occurrences[4]);
        Assert.Equal(6, occurrences[5]);
        Assert.Equal(1, occurrences[6]);
        Assert.True(Measures.IsWinogradDeep(occurrences, (int)cMajor.Count));
        Assert.True(Measures.IsErdosDeep(occurrences));

        Assert.Equal(2.28571, Measures.CalculateRegressionEvenness(rawPositions, cMajor.EffectiveRange), 5);
        Assert.Equal(1, Measures.CalculateRhythmicOddity(cMajor));
        Assert.Equal(0.979869, Measures.ComputeEntropy(cMajor), 5);
        Assert.Equal(2, Measures.ComputeLongestSubsequence(cMajor));
        Assert.False(Measures.IsEuclidean(cMajor, cMajor.EffectiveRange));

        AssertSeqEqual(widths, new[] { 1, 1, 1, 1, 1, 1 });
        Assert.Equal(0.857143, Measures.CalculateSpectrumVariation(widths, (int)cMajor.Count), 6);
        Assert.Empty(rotationalAxes);
        Assert.Equal(2, reflectiveAxes.Count);
        Assert.Equal(2.0, reflectiveAxes[0], 6);
        Assert.Equal(8.0, reflectiveAxes[1], 6);

        Assert.Equal(10, Measures.ComputeTransitionComplexity(cMajor, cMajor.EffectiveRange));
        Assert.False(Measures.IsPalindrome(cMajor));
        Assert.False(Measures.IsChiral(cMajor));
        Assert.False(Measures.IsBalanced(cMajor));
        var gen = Measures.IsGenerated(rawPositions, cMajor.EffectiveRange);
        Assert.False(gen.Found);
        Assert.Equal(-1, gen.Multiplier);
    }

    [Fact]
    public void measure_false_cases()
    {
        var asymmetrical = new PositionVector(Ints(0, 1, 4, 6, 9), 12, 12);
        var distances = Measures.GeodesicDistances(asymmetrical);
        var occurrences = Measures.CalculateOccurrences(distances);
        var rotationalAxes = Measures.FindRotationalSymmetryAxes(asymmetrical);
        var reflectiveAxes = Measures.FindReflectiveSymmetryAxes(asymmetrical);
        var rawPositions = asymmetrical.MutableData;

        Assert.False(Measures.IsWinogradDeep(occurrences, (int)asymmetrical.Count));
        Assert.False(Measures.IsErdosDeep(occurrences));
        Assert.Empty(rotationalAxes);
        Assert.Empty(reflectiveAxes);
        Assert.False(Measures.IsBalanced(asymmetrical));
        var gen = Measures.IsGenerated(rawPositions, asymmetrical.EffectiveRange);
        Assert.False(gen.Found);
        Assert.False(Measures.IsEuclidean(asymmetrical, asymmetrical.EffectiveRange));
        Assert.False(Measures.IsPalindrome(asymmetrical));
        Assert.True(Measures.IsChiral(asymmetrical));
        Assert.Equal(8, Measures.ComputeTransitionComplexity(asymmetrical, asymmetrical.EffectiveRange));
    }
}
