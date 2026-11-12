using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class QuantizeTransposeTests
{
    [Fact]
    public void quantize_exact_and_between_neighbors()
    {
        var scale = new[] { 0, 2, 4, 5, 7, 9, 11 };
        Assert.Equal(4, QuantizeTranspose.Quantize(4, scale, true));
        Assert.Equal(4, QuantizeTranspose.Quantize(4, scale, false));
        Assert.Equal(2, QuantizeTranspose.Quantize(3, scale, true));
        Assert.Equal(4, QuantizeTranspose.Quantize(3, scale, false));
    }

    [Fact]
    public void quantize_outside_range()
    {
        var scale = new[] { 2, 4, 7 };
        Assert.Equal(2, QuantizeTranspose.Quantize(0, scale, true));
        Assert.Equal(7, QuantizeTranspose.Quantize(100, scale, false));
    }

    [Fact]
    public void quantize_empty_scale_returns_negative_one()
    {
        Assert.Equal(-1, QuantizeTranspose.Quantize(5, Array.Empty<int>(), true));
    }

    [Fact]
    public void transpose_c_major_to_c_major_degrees()
    {
        var inScale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var outScale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var pr = QuantizeTranspose.Transpose(inScale, outScale, 0, 0, new[] { 0, 4, 7 }, out var outDeg, out var outNotes);
        AssertPvEq(pr.Degrees, new[] { 0, 2, 4 }, 7);
        AssertPvEq(pr.Notes, new[] { 0, 4, 7 }, 12);
        AssertPvEq(outDeg, pr.Degrees.MutableData.ToArray(), 7);
        AssertPvEq(outNotes, pr.Notes.MutableData.ToArray(), 12);
    }

    [Fact]
    public void transpose_with_roots_and_octave()
    {
        var inScale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var outScale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        QuantizeTranspose.Transpose(inScale, outScale, 0, 0, new[] { 12, 16 }, out _, out var outNotes);
        AssertPvEq(outNotes, new[] { 12, 16 }, 12);
    }

    [Fact]
    public void transpose_negative_pitch_class_handling()
    {
        var inScale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var outScale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        QuantizeTranspose.Transpose(inScale, outScale, 0, 0, new[] { -1 }, out _, out var outNotes);
        Assert.NotEmpty(outNotes.MutableData);
    }

    [Fact]
    public void transpose_empty_output_scale_throws()
    {
        var inScale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var outEmpty = new PositionVector(new List<int>(), 12, 12, true, false);
        Assert.Throws<ArgumentException>(() =>
            QuantizeTranspose.Transpose(inScale, outEmpty, 0, 0, new[] { 0 }, out _, out _));
    }

    [Fact]
    public void transpose_empty_note_list()
    {
        var inScale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var outScale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var pr = QuantizeTranspose.Transpose(inScale, outScale, 0, 0, Array.Empty<int>(), out var outDeg, out var outNotes);
        Assert.Empty(pr.Degrees.MutableData);
        Assert.Empty(pr.Notes.MutableData);
        Assert.Empty(outDeg.MutableData);
        Assert.Empty(outNotes.MutableData);
    }

    [Fact]
    public void transpose_seven_note_scale_to_pentatonic_wrap_degrees()
    {
        var inScale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var outScale = new PositionVector(Ints(0, 2, 4, 7, 9), 12, 12);
        QuantizeTranspose.Transpose(inScale, outScale, 0, 0, new[] { 0, 11 }, out var outDeg, out var outNotes);
        AssertPvEq(outDeg, new[] { 0, 6 }, 7);
        AssertPvEq(outNotes, new[] { 0, 2 }, 12);
    }

    [Fact]
    public void transpose_nonzero_out_root()
    {
        var inScale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var outScale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        QuantizeTranspose.Transpose(inScale, outScale, 0, 12, new[] { 0 }, out _, out var outNotes);
        AssertPvEq(outNotes, new[] { 12 }, 12);
    }

    [Fact]
    public void transpose_nonzero_in_root()
    {
        var inScale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        var outScale = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12, 12);
        QuantizeTranspose.Transpose(inScale, outScale, 2, 0, new[] { 2 }, out _, out var outNotes);
        AssertPvEq(outNotes, new[] { 0 }, 12);
    }
}
