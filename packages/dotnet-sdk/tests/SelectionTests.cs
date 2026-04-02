using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class SelectionTests
{
    [Fact]
    public void selection_position_examples()
    {
        var cMajor = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12);
        var triadDegrees = new PositionVector(Ints(0, 2, 4), 7);
        AssertPvEq(Selection.Select(cMajor, triadDegrees, 0), new[] { 0, 4, 7 }, 12);
        AssertPvEq(Selection.Select(cMajor, triadDegrees, 1), new[] { 4, 7, 12 }, 12);
        AssertPvEq(Selection.Select(cMajor, triadDegrees, 2), new[] { 7, 12, 16 }, 12);
        AssertPvEq(Selection.Select(cMajor, triadDegrees, 3), new[] { 12, 16, 19 }, 12);
        AssertPvEq(Selection.Select(cMajor, triadDegrees, -1), new[] { -5, 0, 4 }, 12);
        AssertPvEq(Selection.Select(cMajor, triadDegrees, 0, 2), new[] { 0, 4 }, 12);
        AssertPvEq(Selection.Select(cMajor, triadDegrees, 0, 5), new[] { 0, 4, 7, 12, 16 }, 12);

        var extendedDegrees = new PositionVector(Ints(0, 3, 7), 12);
        AssertPvEq(Selection.Select(cMajor, extendedDegrees), new[] { 0, 5, 12 }, 12);
    }

    [Fact]
    public void selection_interval_examples()
    {
        var cMajor = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12);
        var majorIv = new IntervalVector(Ints(2, 2, 1, 2, 2, 2, 1), 0, 12);
        var thirds = new IntervalVector(Ints(2, 2, 3), 0, 12);
        AssertPvEq(Selection.Select(cMajor, thirds, 0), new[] { 0, 4, 7 }, 12);
        AssertPvEq(Selection.Select(cMajor, thirds, 1), new[] { 0, 4, 9 }, 12);
        AssertPvEq(Selection.Select(cMajor, thirds, 2), new[] { 0, 5, 9 }, 12);

        thirds.SetOffset(1);
        AssertPvEq(Selection.Select(cMajor, thirds, 0), new[] { 2, 5, 9 }, 12);
        thirds.SetOffset(2);
        AssertPvEq(Selection.Select(cMajor, thirds, 0), new[] { 4, 7, 11 }, 12);
        thirds.SetOffset(-1);
        AssertPvEq(Selection.Select(cMajor, thirds, 0), new[] { -1, 2, 5 }, 12);
        thirds.SetOffset(0);
        AssertPvEq(Selection.Select(cMajor, thirds, 0, 7), new[] { 0, 4, 7, 12, 16, 19, 24 }, 12);

        var grouping = new IntervalVector(Ints(2), 1, 12);
        AssertIvEq(Selection.Select(majorIv, grouping, 0, 3), new[] { 3, 4, 3 }, 2, 12);
        AssertIvEq(Selection.Select(majorIv, grouping, 1, 3), new[] { 3, 4, 3 }, 2, 12);
        AssertIvEq(Selection.Select(majorIv, grouping, 2, 3), new[] { 3, 4, 3 }, 2, 12);

        var scaleWithOffset = new IntervalVector(Ints(2, 2, 1, 2, 2, 2, 1), 7, 12);
        grouping.SetOffset(0);
        AssertIvEq(Selection.Select(scaleWithOffset, grouping, 0, 3), new[] { 4, 3, 4 }, 7, 12);
        grouping.SetOffset(1);
        AssertIvEq(Selection.Select(scaleWithOffset, grouping, 0, 3), new[] { 3, 4, 3 }, 9, 12);
        grouping.SetOffset(2);
        AssertIvEq(Selection.Select(scaleWithOffset, grouping, 0, 3), new[] { 3, 4, 3 }, 11, 12);
        grouping.SetOffset(0);
        AssertIvEq(Selection.Select(scaleWithOffset, grouping, 0, 6), new[] { 4, 3, 4, 3, 3, 4 }, 7, 12);
    }

    [Fact]
    public void selection_position_from_interval_examples()
    {
        var majorIv = new IntervalVector(Ints(2, 2, 1, 2, 2, 2, 1), 0, 12);
        var positions = new PositionVector(Ints(0, 2, 5), 12);
        AssertIvEq(Selection.Select(majorIv, positions, 0), new[] { 4, 5, 3 }, 0, 12);
        AssertIvEq(Selection.Select(majorIv, positions, 1), new[] { 5, 3, 4 }, 0, 12);
        AssertIvEq(Selection.Select(majorIv, positions, 2), new[] { 3, 4, 5 }, 0, 12);

        var offsetSource = new IntervalVector(Ints(2, 2, 1, 2, 2, 2, 1), 5, 12);
        AssertIvEq(Selection.Select(offsetSource, positions, 0), new[] { 4, 5, 3 }, 5, 12);
        AssertIvEq(Selection.Select(offsetSource, positions, 0, 5), new[] { 4, 5, 3, 4, 5 }, 5, 12);

        var singlePos = new PositionVector(Ints(3), 12);
        AssertIvEq(Selection.Select(majorIv, singlePos, 0), new[] { 12 }, 5, 12);
    }

    [Fact]
    public void selection_musical_and_edge_examples()
    {
        var cMajor = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12);
        var harmonyByThirds = new IntervalVector(Ints(2), 0, 12);
        harmonyByThirds.SetOffset(1);
        AssertPvEq(Selection.Select(cMajor, harmonyByThirds, 0, 4), new[] { 2, 5, 9, 12 }, 12);
        harmonyByThirds.SetOffset(4);
        AssertPvEq(Selection.Select(cMajor, harmonyByThirds, 0, 4), new[] { 7, 11, 14, 17 }, 12);
        harmonyByThirds.SetOffset(0);
        AssertPvEq(Selection.Select(cMajor, harmonyByThirds, 0, 4), new[] { 0, 4, 7, 11 }, 12);

        var triadPattern = new PositionVector(Ints(0, 2, 4), 12);
        AssertPvEq(Selection.Select(cMajor, triadPattern, 6), new[] { 24, 28, 31 }, 12);

        var emptyPos = new PositionVector(new List<int>(), 12);
        AssertPvEq(Selection.Select(cMajor, emptyPos), Array.Empty<int>(), 12);

        var singleElem = new PositionVector(Ints(5), 12);
        var singleCrit = new PositionVector(Ints(0), 12);
        AssertPvEq(Selection.Select(singleElem, singleCrit), new[] { 5 }, 12);

        var majorIv = new IntervalVector(Ints(2, 2, 1, 2, 2, 2, 1), 0, 12);
        var smallCrit = new IntervalVector(Ints(1, 1), 0, 12);
        AssertIvEq(
            Selection.Select(majorIv, smallCrit, 0, 15),
            new[] { 2, 2, 1, 2, 2, 2, 1, 2, 2, 1, 2, 2, 2, 1, 2 },
            0,
            12);

        var triadDegrees = new PositionVector(Ints(0, 2, 4), 7);
        AssertPvEq(Selection.Select(cMajor, triadDegrees, -3), new[] { -12, -8, -5 }, 12);
    }

    [Fact]
    public void selection_degenerate_examples()
    {
        var cMajor = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11), 12);
        var repeatedDegrees = new PositionVector(Ints(0, 0, 2), 7);
        var oversizedDegrees = new PositionVector(Ints(0, 2, 9), 7);
        var triadDegrees = new PositionVector(Ints(0, 2, 4), 7);
        var emptyPositions = new PositionVector(new List<int>(), 12);
        var singleElem = new PositionVector(Ints(5), 12);
        var repeatedSingle = new PositionVector(Ints(0, 0, 0), 1);
        var emptyInterval = new IntervalVector(new List<int>(), 0, 12);
        var majorIv = new IntervalVector(Ints(2, 2, 1, 2, 2, 2, 1), 0, 12);

        AssertPvEq(Selection.Select(cMajor, repeatedDegrees, 0), new[] { 0, 0, 4 }, 12);
        AssertPvEq(Selection.Select(cMajor, oversizedDegrees, 0), new[] { 0, 4, 16 }, 12);
        AssertPvEq(Selection.Select(cMajor, triadDegrees, 9), new[] { 36, 40, 43 }, 12);
        AssertPvEq(Selection.Select(cMajor, emptyPositions, 0), Array.Empty<int>(), 12);
        AssertPvEq(Selection.Select(cMajor, emptyInterval, 0), Array.Empty<int>(), 12);
        AssertIvEq(Selection.Select(majorIv, emptyPositions, 0), Array.Empty<int>(), 0, 12);
        AssertPvEq(Selection.Select(singleElem, repeatedSingle), new[] { 5, 5, 5 }, 12);
    }
}
