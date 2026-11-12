using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class DistancesUtilTests
{
    [Fact]
    public void normalize_throws_on_zero_sum()
    {
        Assert.Throws<ArgumentException>(() => Distances.Normalize(new[] { 0, 0, 0 }));
    }

    [Fact]
    public void normalize_probability_sum_one()
    {
        var v = new[] { 2, 2, 4 };
        var outList = Distances.Normalize(v);
        Assert.Equal(3, outList.Count);
        Assert.Equal(0.25, outList[0], 12);
        Assert.Equal(0.25, outList[1], 12);
        Assert.Equal(0.5, outList[2], 12);
        Assert.Equal(1.0, outList.Sum(), 12);
    }

    [Fact]
    public void compute_cdf_monotone()
    {
        var pdf = new List<double> { 0.25, 0.25, 0.5 };
        var cdf = Distances.ComputeCdf(pdf);
        Assert.Equal(pdf.Count, cdf.Count);
        Assert.Equal(0.25, cdf[0], 9);
        Assert.Equal(0.5, cdf[1], 9);
        Assert.Equal(1.0, cdf[2], 9);
    }

    [Fact]
    public void euclidean_distance_int_vectors()
    {
        Assert.Equal(5.0, Distances.EuclideanDistance(new[] { 0, 0 }, new[] { 3, 4 }), 9);
        Assert.Equal(0.0, Distances.EuclideanDistance(new[] { 1, 2, 3 }, new[] { 1, 2 }), 9);
    }

    [Fact]
    public void manhattan_hamming_difference_edit_int_vectors()
    {
        var a = new List<int> { 1, 2, 3 };
        var b = new List<int> { 1, 0, 5 };
        Assert.Equal(4, Distances.ManhattanDistance(a, b));
        Assert.Equal(2, Distances.HammingDistance(a, b));
        Assert.Equal(0, Distances.Difference(a, b));

        var x = new List<int> { 1, 2, 3 };
        var y = new List<int> { 1, 2, 3, 9 };
        Assert.Equal(1, Distances.EditDistance(x, y));
    }

    [Fact]
    public void variation_distance_sorted_and_empty()
    {
        Assert.Equal(0.0, Distances.VariationDistance(new List<int>(), new List<int>()), 9);
        Assert.Throws<ArgumentException>(() => Distances.VariationDistance(new List<int>(), new List<int> { 1 }));
        Assert.Throws<ArgumentException>(() => Distances.VariationDistance(new List<int> { 1 }, new List<int>()));

        Assert.Equal(0.0, Distances.VariationDistance(new List<int> { 0, 4, 7 }, new List<int> { 0, 4, 7 }), 9);
        Assert.Equal(0.0, Distances.VariationDistance(new List<int> { 7, 0, 4 }, new List<int> { 0, 4, 7 }), 9);
        Assert.Equal(12.0, Distances.VariationDistance(new List<int> { 0, 12 }, new List<int> { 0 }), 9);
    }

    [Fact]
    public void generalized_neo_riemann_bounds()
    {
        var input = new List<int> { 0, 4, 7 };
        AssertSeqEqual(Distances.GeneralizedNeoRiemann(input, -1, 99), input);
        AssertSeqEqual(Distances.GeneralizedNeoRiemann(input, 3, 5), input);
        AssertSeqEqual(Distances.GeneralizedNeoRiemann(input, 1, 3), new List<int> { 0, 7, 7 });
    }

    [Fact]
    public void transformation_steps_identical_and_resize()
    {
        Assert.Empty(Distances.TransformationSteps(new[] { 1, 2 }, new[] { 1, 2 }));

        var add = Distances.TransformationSteps(new[] { 0 }, new[] { 0, 5 });
        Assert.Single(add);
        Assert.Equal(1, add[0].Type);

        var rem = Distances.TransformationSteps(new[] { 0, 1, 2 }, new[] { 0, 1 });
        Assert.NotEmpty(rem);
        Assert.Equal(2, rem[^1].Type);
    }

    [Fact]
    public void weighted_transformation_distance_int()
    {
        Assert.Equal(8, Distances.WeightedTransformationDistance(new List<int> { 0, 0 }, new List<int> { 3, 5 }));
    }

    [Fact]
    public void distance_overloads_interval_vectors()
    {
        var iv1 = new IntervalVector(Ints(0, 4, 7), 0, 12);
        var iv2 = new IntervalVector(Ints(1, 5, 8), 0, 12);
        Assert.Equal(3, Distances.HammingDistance(iv1, iv2));
        Assert.Equal(3, Distances.ManhattanDistance(iv1, iv2));
        Assert.Equal(Math.Sqrt(3), Distances.EuclideanDistance(iv1, iv2), 9);
        Assert.Equal(-3, Distances.Difference(iv1, iv2));
        Assert.Equal(3, Distances.EditDistance(iv1, iv2));
        Assert.Equal(3, Distances.WeightedTransformationDistance(iv1, iv2));
    }

    [Fact]
    public void variation_distance_position_vector_overload()
    {
        var a = new PositionVector(Ints(7, 0, 4), 12, 12);
        var b = new PositionVector(Ints(0, 4, 7), 12, 12);
        Assert.Equal(0.0, Distances.VariationDistance(a, b), 9);
    }

    [Fact]
    public void compute_cdf_empty()
    {
        Assert.Empty(Distances.ComputeCdf(new List<double>()));
    }
}
