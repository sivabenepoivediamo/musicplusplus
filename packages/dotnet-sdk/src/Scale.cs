namespace Musicpp;

public sealed class ScaleParams
{
    public int Root { get; set; }
    public int Mode { get; set; }
    public int Degree { get; set; }
    public bool Invert { get; set; }
    public int InversionAxis { get; set; }
    public bool Mirror { get; set; }
    public int MirrorAxis { get; set; }

    public ScaleParams(int root = 0, int mode = 0, int degree = 0, bool invert = false,
        int inversionAxis = 0, bool mirror = false, int mirrorAxis = 0)
    {
        Root = root;
        Mode = mode;
        Degree = degree;
        Invert = invert;
        InversionAxis = inversionAxis;
        Mirror = mirror;
        MirrorAxis = mirrorAxis;
    }

    public void Merge(ScaleParams other)
    {
        Root = other.Root;
        Mode = other.Mode;
        Degree = other.Degree;
        Invert = other.Invert;
        InversionAxis = other.InversionAxis;
        Mirror = other.Mirror;
        MirrorAxis = other.MirrorAxis;
    }

    public ScaleParams WithRoot(int val)
    {
        Root = val;
        return this;
    }

    public ScaleParams WithMode(int val)
    {
        Mode = val;
        return this;
    }

    public ScaleParams WithDegree(int val)
    {
        Degree = val;
        return this;
    }

    public ScaleParams WithInvert(bool val)
    {
        Invert = val;
        return this;
    }

    public ScaleParams WithInversionAxis(int val)
    {
        InversionAxis = val;
        return this;
    }

    public ScaleParams WithMirror(bool val)
    {
        Mirror = val;
        return this;
    }

    public ScaleParams WithMirrorAxis(int val)
    {
        MirrorAxis = val;
        return this;
    }
}

public sealed class Scale
{
    private IntervalVector _intervals;
    private IntervalVector _generator;
    private bool _isFromPositions;
    private ScaleParams _params;

    public Scale(IntervalVector generator, int root = 0, int mode = 0, int degree = 0,
        bool invert = false, int inversionAxis = 0, bool mirror = false, int mirrorAxis = 0)
    {
        _generator = generator.Clone();
        _isFromPositions = false;
        _params = new ScaleParams(root, mode, degree, invert, inversionAxis, mirror, mirrorAxis);
        ApplyTransformations();
    }

    public Scale(IntervalVector generator, ScaleParams p)
    {
        _generator = generator.Clone();
        _isFromPositions = false;
        _params = p;
        ApplyTransformations();
    }

    public Scale(PositionVector generator, int root = 0, int mode = 0, int degree = 0,
        bool invert = false, int inversionAxis = 0, bool mirror = false, int mirrorAxis = 0)
    {
        _generator = MusicVectors.PositionsToIntervals(generator);
        _isFromPositions = true;
        _params = new ScaleParams(root, mode, degree, invert, inversionAxis, mirror, mirrorAxis);
        ApplyTransformations();
    }

    public Scale(PositionVector generator, ScaleParams p)
    {
        _generator = MusicVectors.PositionsToIntervals(generator);
        _isFromPositions = true;
        _params = p;
        ApplyTransformations();
    }

    private void ApplyTransformations()
    {
        _intervals = _generator.Clone();
        _intervals.SetOffset(_params.Root);
        _intervals = _intervals.ParallelMode(_params.Mode);
        if (_params.Invert)
            _intervals = _intervals.Inversion(_params.InversionAxis);
        if (_params.Mirror)
            _intervals = _intervals.SingleMirror(_params.MirrorAxis, true);
    }

    public PositionVector ToPositions() => MusicVectors.IntervalsToPositions(_intervals);
    public IntervalVector ToIntervals() => _intervals.Clone();
    public IntervalVector GetGenerator() => _generator.Clone();
    public PositionVector GetGeneratorAsPositions() => MusicVectors.IntervalsToPositions(_generator);
    public ScaleParams Params => _params;
    public bool IsFromPositions => _isFromPositions;
    public IntervalVector Intervals => _intervals.Clone();

    public int Root => _params.Root;
    public int Mode => _params.Mode;
    public int Degree => _params.Degree;
    public bool Invert => _params.Invert;
    public int InversionAxis => _params.InversionAxis;
    public bool Mirror => _params.Mirror;
    public int MirrorAxis => _params.MirrorAxis;

    public void SetRoot(int v) { _params.Root = v; ApplyTransformations(); }
    public void SetMode(int v) { _params.Mode = v; ApplyTransformations(); }
    public void SetDegree(int v) { _params.Degree = v; ApplyTransformations(); }
    public void SetInvert(bool v) { _params.Invert = v; ApplyTransformations(); }
    public void SetInversionAxis(int v) { _params.InversionAxis = v; ApplyTransformations(); }
    public void SetMirror(bool v) { _params.Mirror = v; ApplyTransformations(); }
    public void SetMirrorAxis(int v) { _params.MirrorAxis = v; ApplyTransformations(); }

    public void SetGenerator(IntervalVector g) { _generator = g.Clone(); _isFromPositions = false; ApplyTransformations(); }
    public void SetGenerator(PositionVector g) { _generator = MusicVectors.PositionsToIntervals(g); _isFromPositions = true; ApplyTransformations(); }
    public void SetParams(ScaleParams p) { _params = p; ApplyTransformations(); }
}
