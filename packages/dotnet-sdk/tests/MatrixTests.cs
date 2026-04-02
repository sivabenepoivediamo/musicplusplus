using Musicpp;
using static Musicplusplus.Tests.TestHelpers;

namespace Musicplusplus.Tests;

public class MatrixTests
{
    [Fact]
    public void matrix_generation_examples()
    {
        var majorIv = new IntervalVector(Ints(2, 2, 1, 2, 2, 2, 1), 0, 12);
        var intervalModes = MatrixOps.ModalMatrix(majorIv);
        Assert.Equal(7, intervalModes.Count);
        AssertIvEq(intervalModes[0].Vector, new[] { 2, 2, 1, 2, 2, 2, 1 }, 0, 12);
        AssertIvEq(intervalModes[3].Vector, new[] { 2, 2, 2, 1, 2, 2, 1 }, 0, 12);
        AssertSeqEqual(intervalModes.GetIndices(), new[] { 0, 1, 2, 3, 4, 5, 6 });

        var cMajor = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11));
        var positionModes = MatrixOps.ModalMatrix(cMajor);
        Assert.Equal(7, positionModes.Count);
        AssertPvEq(positionModes[0].Vector, new[] { 0, 2, 4, 5, 7, 9, 11 }, 12);
        AssertPvEq(positionModes[3].Vector, new[] { 0, 2, 4, 6, 7, 9, 11 }, 12);

        var transpositions = MatrixOps.TranspositionMatrix(cMajor);
        Assert.Equal(12, transpositions.Count);
        AssertPvEq(transpositions[0].Vector, new[] { 0, 2, 4, 5, 7, 9, 11 }, 12);
        AssertPvEq(transpositions[7].Vector, new[] { 0, 2, 4, 6, 7, 9, 11 }, 12);

        var triad = new PositionVector(Ints(0, 4, 7));
        var relModeMatrix = MatrixOps.RelativeModeMatrix(triad, 0);
        Assert.Equal(0, relModeMatrix.Center);
        Assert.Equal(7, relModeMatrix.Count);
        AssertPvEq(relModeMatrix[0].Vector, new[] { -12, -8, -5 }, 12);
        AssertPvEq(relModeMatrix[3].Vector, new[] { 0, 4, 7 }, 12);
        AssertPvEq(relModeMatrix[6].Vector, new[] { 12, 16, 19 }, 12);
    }

    [Fact]
    public void matrix_selection_and_filter_examples()
    {
        var cMajor = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11));
        var criterion = new IntervalVector(Ints(2, 2, 3), 0, 12);
        var intervalSelection = MatrixOps.ModalSelection(MusicVectors.PositionsToIntervals(cMajor), criterion, 0);
        Assert.Equal(3, intervalSelection.Count);
        AssertIvEq(intervalSelection[0].Chord, new[] { 4, 3, 5 }, 0, 12);
        AssertSeqEqual(intervalSelection.GetModeIndices(), new[] { 0, 5, 3 });

        var positionSelection = MatrixOps.ModalSelection(cMajor, criterion, 0);
        Assert.Equal(3, positionSelection.Count);
        AssertPvEq(positionSelection[0].Chord, new[] { 0, 4, 7 }, 12);
        AssertPvEq(positionSelection[1].Chord, new[] { 0, 4, 9 }, 12);
        AssertPvEq(positionSelection[2].Chord, new[] { 0, 5, 9 }, 12);

        var modalRelMode = MatrixOps.ModalRelativeMode(positionSelection);
        Assert.Equal(3, modalRelMode.Count);
        AssertSeqEqual(Enumerable.Range(0, modalRelMode.Count).Select(i => modalRelMode[i].ModeIndex), new[] { 0, 5, 3 });
        Assert.Equal(21, modalRelMode.GetTotalVectorCount());

        var notes = new[] { 63 };
        var filteredModes = MatrixOps.FilterModalMatrix(MatrixOps.ModalMatrix(cMajor), notes);
        var filteredTranspositions = MatrixOps.FilterTranspositionMatrix(MatrixOps.TranspositionMatrix(cMajor), notes);
        Assert.Equal(4, filteredModes.Count);
        Assert.Equal(7, filteredTranspositions.Count);

        var modesCopy = MatrixOps.ModalMatrix(cMajor);
        var transpositionsCopy = MatrixOps.TranspositionMatrix(cMajor);
        modesCopy = MatrixOps.FilterModalMatrix(modesCopy, notes);
        transpositionsCopy = MatrixOps.FilterTranspositionMatrix(transpositionsCopy, notes);
        Assert.Equal(4, modesCopy.Count);
        Assert.Equal(7, transpositionsCopy.Count);
    }

    [Fact]
    public void matrix_distance_examples()
    {
        var cMajor = new PositionVector(Ints(0, 2, 4, 5, 7, 9, 11));
        var cLydian = new PositionVector(Ints(0, 2, 4, 6, 7, 9, 11));
        var cMajorChord = new PositionVector(Ints(0, 4, 7));
        var gMajorChord = new PositionVector(Ints(7, 11, 14));
        const int complexity = 0;

        var transpositionDistances = MatrixDistanceOps.CalculateDistances(cLydian, MatrixOps.TranspositionMatrix(cMajor));
        var bestTransposition = transpositionDistances.GetByComplexity(complexity);
        Assert.Equal(7, bestTransposition.Transposition);
        Assert.Equal(0.0, bestTransposition.Distance, 6);
        AssertPvEq(bestTransposition.Vector, new[] { 0, 2, 4, 6, 7, 9, 11 }, 12);

        var modalDistances = MatrixDistanceOps.CalculateDistances(cLydian, MatrixOps.ModalMatrix(cMajor));
        var bestMode = modalDistances.GetByComplexity(complexity);
        Assert.Equal(3, bestMode.Index);
        Assert.Equal(0.0, bestMode.Distance, 6);
        AssertPvEq(bestMode.Vector, new[] { 0, 2, 4, 6, 7, 9, 11 }, 12);

        var center = MatrixDistanceOps.Align(cMajorChord, gMajorChord);
        Assert.Equal(-2, center);
        var relModeDistances = MatrixDistanceOps.CalculateDistances(cMajorChord, MatrixOps.RelativeModeMatrix(gMajorChord, center));
        var bestRelMode = relModeDistances.GetByComplexity(complexity);
        Assert.Equal(-2, bestRelMode.RelativeModeOffset);
        Assert.Equal(-2, bestRelMode.Center);
        Assert.Equal(3.0, bestRelMode.Distance, 6);
        AssertPvEq(bestRelMode.Vector, new[] { -1, 2, 7 }, 12);

        var crit = new IntervalVector(Ints(2, 2, 3));
        var modalSelectionDistances = MatrixDistanceOps.CalculateDistances(gMajorChord, MatrixOps.ModalSelection(cMajor, crit, 0));
        var bestDegree = modalSelectionDistances.GetByComplexity(complexity);
        Assert.Equal(3, bestDegree.ModeIndex);
        Assert.Equal(18.0, bestDegree.Distance, 6);
        AssertPvEq(bestDegree.Chord, new[] { 0, 5, 9 }, 12);

        var modalRelModeDistances = MatrixDistanceOps.CalculateDistances(
            gMajorChord,
            MatrixOps.ModalRelativeMode(MatrixOps.ModalSelection(cMajor, crit, 0)));
        var bestModalRelMode = modalRelModeDistances.GetByComplexity(complexity);
        Assert.Equal(0, bestModalRelMode.ModeIndex);
        Assert.Equal(2, bestModalRelMode.RelativeModeIndex);
        Assert.Equal(3.0, bestModalRelMode.Distance, 6);
        AssertPvEq(bestModalRelMode.Vector, new[] { 7, 12, 16 }, 12);
    }
}
