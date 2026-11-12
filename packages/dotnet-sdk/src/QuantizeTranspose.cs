namespace Musicpp;

public static class QuantizeTranspose
{
    public static int Quantize(int note, IReadOnlyList<int> scale, bool left = true)
    {
        var lower = -1;
        var upper = -1;
        for (var i = 0; i < scale.Count; i++)
        {
            if (scale[i] <= note)
                lower = scale[i];
            if (scale[i] >= note)
            {
                upper = scale[i];
                break;
            }
        }
        if (lower == -1)
            return upper;
        if (upper == -1)
            return lower;
        return left ? lower : upper;
    }

    public static (PositionVector Degrees, PositionVector Notes) Transpose(
        PositionVector inputScale,
        PositionVector outputScale,
        int inRoot,
        int outRoot,
        IReadOnlyList<int> notes,
        out PositionVector outDegrees,
        out PositionVector outNotes)
    {
        var inScale = inputScale.MutableData;
        var outScale = outputScale.MutableData;
        if (outScale.Count == 0)
            throw new ArgumentException("transpose: output scale must not be empty");
        var mod = inputScale.Mod;
        var degreesData = new List<int>();
        var notesData = new List<int>();
        var length = outScale.Count;
        for (var i = 0; i < notes.Count; i++)
        {
            var note = notes[i];
            var inPc = (note - inRoot) % mod;
            if (inPc < 0)
                inPc += mod;
            var octave = (note - inRoot) / mod;
            if (note - inRoot < 0 && (note - inRoot) % mod != 0)
                octave--;
            var left = true;
            var index = -1;
            var idxFound = inScale.IndexOf(inPc);
            if (idxFound >= 0)
                index = idxFound;
            else
            {
                inPc = Quantize(inPc, inScale, left);
                idxFound = inScale.IndexOf(inPc);
                if (idxFound >= 0)
                    index = idxFound;
            }
            if (index != -1)
            {
                var degree = index;
                var outPc = outScale[degree % length];
                var outNote = outPc + outRoot + octave * mod;
                if (notesData.Count > 0 && notesData[^1] == outNote)
                {
                    if (i > 0 && notes[i] != notes[i - 1])
                    {
                        left = !left;
                        inPc = (notes[i] - inRoot) % mod;
                        if (inPc < 0)
                            inPc += mod;
                        inPc = Quantize(inPc, inScale, left);
                        idxFound = inScale.IndexOf(inPc);
                        if (idxFound >= 0)
                        {
                            index = idxFound;
                            degree = index;
                            outPc = outScale[degree % length];
                            outNote = outPc + outRoot + octave * mod;
                        }
                    }
                }
                notesData.Add(outNote);
                degreesData.Add(degree);
            }
        }
        outDegrees = new PositionVector(degreesData, inputScale.Count, inputScale.UserRange, inputScale.AutoRangeUpdates, inputScale.UseUserRange);
        outNotes = new PositionVector(notesData, inputScale.Mod, inputScale.UserRange, inputScale.AutoRangeUpdates, inputScale.UseUserRange);
        return (outDegrees, outNotes);
    }
}
