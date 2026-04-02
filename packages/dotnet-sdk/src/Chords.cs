namespace Musicpp;

public sealed class ChordParams
{
    public int Shift { get; set; }
    public int CriterionMode { get; set; }
    public int PreVoices { get; set; }
    public int RelativeMode { get; set; }
    public bool Invert { get; set; }
    public int Axis { get; set; }
    public bool NegativeOrMirror { get; set; }
    public int NegativeOrMirrorPos { get; set; } = 10;

    public ChordParams(int shift = 0, int criterionMode = 0, int preVoices = 0, int relativeMode = 0,
        bool invert = false, int axis = 0, bool negativeOrMirror = false, int negativeOrMirrorPos = 10)
    {
        Shift = shift;
        CriterionMode = criterionMode;
        PreVoices = preVoices;
        RelativeMode = relativeMode;
        Invert = invert;
        Axis = axis;
        NegativeOrMirror = negativeOrMirror;
        NegativeOrMirrorPos = negativeOrMirrorPos;
    }
}

public sealed class Chord
{
    private enum CriterionKind { Position, Interval }
    private enum ScaleKind { Position, Interval }

    private PositionVector _scalePositions;
    private IntervalVector _scaleIntervals;
    private PositionVector _criterionPositions;
    private IntervalVector _criterionIntervals;
    private ScaleKind _scaleKind;
    private CriterionKind _criterionKind;
    private ChordParams _params;
    private PositionVector _resultPositions;
    private IntervalVector _resultIntervals;
    private bool _isResultPositions;

    public Chord(PositionVector scale, PositionVector degrees, ChordParams? p = null)
    {
        _scalePositions = scale.Clone();
        _criterionPositions = degrees.Clone();
        _scaleKind = ScaleKind.Position;
        _criterionKind = CriterionKind.Position;
        _params = p ?? new ChordParams();
        _isResultPositions = true;
        Generate();
    }

    public Chord(PositionVector scale, IntervalVector intervals, ChordParams? p = null)
    {
        _scalePositions = scale.Clone();
        _criterionIntervals = intervals.Clone();
        _scaleKind = ScaleKind.Position;
        _criterionKind = CriterionKind.Interval;
        _params = p ?? new ChordParams();
        _isResultPositions = true;
        Generate();
    }

    public Chord(IntervalVector scale, PositionVector degrees, ChordParams? p = null)
    {
        _scaleIntervals = scale.Clone();
        _criterionPositions = degrees.Clone();
        _scaleKind = ScaleKind.Interval;
        _criterionKind = CriterionKind.Position;
        _params = p ?? new ChordParams();
        _isResultPositions = false;
        Generate();
    }

    public Chord(IntervalVector scale, IntervalVector intervals, ChordParams? p = null)
    {
        _scaleIntervals = scale.Clone();
        _criterionIntervals = intervals.Clone();
        _scaleKind = ScaleKind.Interval;
        _criterionKind = CriterionKind.Interval;
        _params = p ?? new ChordParams();
        _isResultPositions = false;
        Generate();
    }

    private void Generate()
    {
        if (_scaleKind == ScaleKind.Position && _criterionKind == CriterionKind.Position)
            GeneratePosPos();
        else if (_scaleKind == ScaleKind.Position && _criterionKind == CriterionKind.Interval)
            GeneratePosInt();
        else if (_scaleKind == ScaleKind.Interval && _criterionKind == CriterionKind.Position)
            GenerateIntPos();
        else
            GenerateIntInt();
    }

    private void GeneratePosPos()
    {
        var offsetDegrees = _criterionPositions + _params.Shift;
        _resultPositions = Selection.Select(_scalePositions, offsetDegrees, _params.CriterionMode, _params.PreVoices);
        if (_params.Invert)
            _resultPositions = _resultPositions.Inversion(_params.Axis, true);
        if (_params.NegativeOrMirror)
            _resultPositions = _resultPositions.Negative(_params.NegativeOrMirrorPos);
        _resultPositions = _resultPositions.RelativeMode(_params.RelativeMode);
        _isResultPositions = true;
    }

    private void GeneratePosInt()
    {
        var offsetIntervals = _criterionIntervals.Clone();
        offsetIntervals.SetOffset(offsetIntervals.Offset + _params.Shift);
        _resultPositions = Selection.Select(_scalePositions, offsetIntervals, _params.CriterionMode, _params.PreVoices);
        if (_params.Invert)
            _resultPositions = _resultPositions.Inversion(_params.Axis, true);
        if (_params.NegativeOrMirror)
            _resultPositions = _resultPositions.Negative(_params.NegativeOrMirrorPos);
        _resultPositions = _resultPositions.RelativeMode(_params.RelativeMode);
        _isResultPositions = true;
    }

    private void GenerateIntPos()
    {
        var offsetDegrees = _criterionPositions + _params.Shift;
        var tempResult = Selection.Select(_scaleIntervals, offsetDegrees, _params.CriterionMode, _params.PreVoices);
        _resultIntervals = tempResult;
        if (_params.Invert)
            _resultIntervals = _resultIntervals.Inversion(_params.Axis);
        if (_params.NegativeOrMirror)
            _resultIntervals = _resultIntervals.SingleMirror(_params.NegativeOrMirrorPos, true);
        _resultIntervals = _resultIntervals.RelativeMode(_params.RelativeMode);
        _isResultPositions = false;
    }

    private void GenerateIntInt()
    {
        var offsetIntervals = _criterionIntervals.Clone();
        var off = _criterionIntervals.Offset;
        offsetIntervals.SetOffset(_params.Shift + off);
        var tempResult = Selection.Select(_scaleIntervals, offsetIntervals, _params.CriterionMode, _params.PreVoices);
        _resultIntervals = tempResult;
        if (_params.Invert)
            _resultIntervals = _resultIntervals.Inversion(_params.Axis);
        if (_params.NegativeOrMirror)
            _resultIntervals = _resultIntervals.SingleMirror(_params.NegativeOrMirrorPos, true);
        _resultIntervals = _resultIntervals.RelativeMode(_params.RelativeMode);
        _isResultPositions = false;
    }

    public PositionVector ToPositions() =>
        _isResultPositions ? _resultPositions.Clone() : MusicVectors.IntervalsToPositions(_resultIntervals);

    public IntervalVector ToIntervals() =>
        _isResultPositions ? MusicVectors.PositionsToIntervals(_resultPositions) : _resultIntervals.Clone();

    public ChordParams Params => _params;

    public void SetShift(int v) { _params.Shift = v; Generate(); }
    public void SetCriterionMode(int v) { _params.CriterionMode = v; Generate(); }
    public void SetPreVoices(int v) { _params.PreVoices = v; Generate(); }
    public void SetRelativeMode(int v) { _params.RelativeMode = v; Generate(); }
    public void SetInvert(bool v) { _params.Invert = v; Generate(); }
    public void SetAxis(int v) { _params.Axis = v; Generate(); }
    public void SetNegativeOrMirror(bool v) { _params.NegativeOrMirror = v; Generate(); }
    public void SetNegativeOrMirrorPos(int v) { _params.NegativeOrMirrorPos = v; Generate(); }
    public void SetParams(ChordParams p) { _params = p; Generate(); }

    public void SetScale(PositionVector s) { _scalePositions = s.Clone(); _scaleKind = ScaleKind.Position; Generate(); }
    public void SetScale(IntervalVector s) { _scaleIntervals = s.Clone(); _scaleKind = ScaleKind.Interval; Generate(); }
    public void SetCriterion(PositionVector c) { _criterionPositions = c.Clone(); _criterionKind = CriterionKind.Position; Generate(); }
    public void SetCriterion(IntervalVector c) { _criterionIntervals = c.Clone(); _criterionKind = CriterionKind.Interval; Generate(); }
}

public static class ChordFunctions
{
    public static PositionVector Chord(PositionVector scale, PositionVector degrees, int shift = 0,
        int criterionRelativeMode = 0, int preVoices = 0, int relativeMode = 0, bool invert = false, int axis = 0,
        bool negative = false, int negativePos = 10)
    {
        var offsetDegrees = degrees + shift;
        var result = Selection.Select(scale, offsetDegrees, criterionRelativeMode, preVoices);
        if (invert)
            result = result.Inversion(axis, true);
        if (negative)
            result = result.Negative(negativePos);
        return result.RelativeMode(relativeMode);
    }

    public static PositionVector Chord(PositionVector scale, IntervalVector intervals, int shift = 0,
        int criterionParallelMode = 0, int preVoices = 0, int relativeMode = 0, bool invert = false, int axis = 0,
        bool negative = false, int negativePos = 10)
    {
        var offsetIntervals = intervals.Clone();
        offsetIntervals.SetOffset(offsetIntervals.Offset + shift);
        var result = Selection.Select(scale, offsetIntervals, criterionParallelMode, preVoices);
        if (invert)
            result = result.Inversion(axis, true);
        if (negative)
            result = result.Negative(negativePos);
        return result.RelativeMode(relativeMode);
    }

    public static IntervalVector Chord(IntervalVector scale, PositionVector degrees, int shift = 0,
        int criterionRelativeMode = 0, int preVoices = 0, int relativeMode = 0, bool invert = false, int axis = 0,
        bool mirror = false, int mirrorPos = 10)
    {
        var offsetDegrees = degrees + shift;
        var result = Selection.Select(scale, offsetDegrees, criterionRelativeMode, preVoices);
        if (invert)
            result = result.Inversion(axis);
        if (mirror)
            result = result.SingleMirror(mirrorPos, true);
        return result.RelativeMode(relativeMode);
    }

    public static IntervalVector Chord(IntervalVector scale, IntervalVector intervals, int shift = 0,
        int criterionParallelMode = 0, int preVoices = 0, int relativeMode = 0, bool invert = false, int axis = 0,
        bool mirror = false, int mirrorPos = 10)
    {
        var offsetIntervals = intervals.Clone();
        offsetIntervals.SetOffset(shift + intervals.Offset);
        var result = Selection.Select(scale, offsetIntervals, criterionParallelMode, preVoices);
        if (invert)
            result = result.Inversion(axis);
        if (mirror)
            result = result.SingleMirror(mirrorPos, true);
        return result.RelativeMode(relativeMode);
    }
}
