namespace Musicpp.Slonimsky;

public static class SlonimskySequences
{
    public static List<int> Intervals(IReadOnlyList<int> x)
    {
        var n = x.Count;
        if (n < 2)
            return new List<int>();
        var d = new List<int>(n - 1);
        for (var i = 0; i < n - 1; i++)
            d.Add(x[i + 1] - x[i]);
        return d;
    }

    public static int MinInterval(IReadOnlyList<int> x)
    {
        var d = Intervals(x);
        if (d.Count == 0)
            throw new ArgumentException("minInterval: need at least two ascending pitch positions");
        return d.Min();
    }

    public static List<int> InfrapolationPermutation(int m)
    {
        var h = (m + 1) / 2;
        var delta = m % 2 == 0 ? 1 : 0;
        var sigma = new List<int>(m);
        for (var i = 1; i <= m; i++)
        {
            var val = i <= h ? m - 2 * (i - 1) : 2 * (i - h) - delta;
            sigma.Add(val - 1);
        }
        return sigma;
    }

    public static List<int> Interpolation(IReadOnlyList<int> x, int k)
    {
        var n = x.Count;
        if (n == 0)
            return new List<int>();
        var outList = new List<int>(Math.Max(0, 2 * n - 1));
        for (var i = 0; i < n - 1; i++)
        {
            outList.Add(x[i]);
            outList.Add(x[i] + k);
        }
        outList.Add(x[n - 1]);
        return outList;
    }

    public static List<int> Interpolation(IReadOnlyList<int> x, IReadOnlyList<int> k)
    {
        var n = x.Count;
        var m = k.Count;
        var outList = new List<int>((m + 1) * (n - 1) + 1);
        for (var i = 0; i < n - 1; i++)
        {
            outList.Add(x[i]);
            for (var j = 0; j < m; j++)
                outList.Add(x[i] + k[j]);
        }
        outList.Add(x[n - 1]);
        return outList;
    }

    public static List<int> SymmetricInterpolation(IReadOnlyList<int> x, int k)
    {
        var n = x.Count;
        var pairs = (n - 1) / 2;
        var outList = new List<int>(4 * pairs + (n % 2 == 1 ? 3 : 1));
        for (var j = 0; j < pairs; j++)
        {
            outList.Add(x[2 * j]);
            outList.Add(x[2 * j] + k);
            outList.Add(x[2 * j + 1]);
            outList.Add(x[2 * j + 2] - k);
        }
        if (n % 2 == 1)
        {
            outList.Add(x[n - 2]);
            outList.Add(x[n - 2] + k);
            outList.Add(x[n - 1]);
        }
        else
            outList.Add(x[n - 1]);
        return outList;
    }

    public static List<int> SymmetricInterpolation(IReadOnlyList<int> x, IReadOnlyList<int> k)
    {
        var n = x.Count;
        var m = k.Count;
        var pairs = (n - 1) / 2;
        var outList = new List<int>((2 * m + 2) * pairs + (n % 2 == 1 ? m + 2 : 1));
        for (var j = 0; j < pairs; j++)
        {
            outList.Add(x[2 * j]);
            for (var r = 0; r < m; r++)
                outList.Add(x[2 * j] + k[r]);
            outList.Add(x[2 * j + 1]);
            for (var r = m - 1; r >= 0; r--)
                outList.Add(x[2 * j + 2] - k[r]);
        }
        if (n % 2 == 1)
        {
            outList.Add(x[n - 2]);
            for (var r = 0; r < m; r++)
                outList.Add(x[n - 2] + k[r]);
            outList.Add(x[n - 1]);
        }
        else
            outList.Add(x[n - 1]);
        return outList;
    }

    public static List<int> AsymmetricInterpolation(IReadOnlyList<int> x, IReadOnlyList<int> k, IReadOnlyList<int> l)
    {
        var n = x.Count;
        var m = k.Count;
        var pairs = (n - 1) / 2;
        var outList = new List<int>((2 * m + 2) * pairs + (n % 2 == 1 ? m + 2 : 1));
        for (var j = 0; j < pairs; j++)
        {
            outList.Add(x[2 * j]);
            for (var r = 0; r < m; r++)
                outList.Add(x[2 * j] + k[r]);
            outList.Add(x[2 * j + 1]);
            for (var r = m - 1; r >= 0; r--)
                outList.Add(x[2 * j + 2] - l[r]);
        }
        if (n % 2 == 1)
        {
            outList.Add(x[n - 2]);
            for (var r = 0; r < m; r++)
                outList.Add(x[n - 2] + k[r]);
            outList.Add(x[n - 1]);
        }
        else
            outList.Add(x[n - 1]);
        return outList;
    }

    public static List<int> Ultrapolation(IReadOnlyList<int> x, int k)
    {
        var n = x.Count;
        var outList = new List<int>(2 * n - 1) { x[0] };
        for (var i = 1; i < n; i++)
        {
            outList.Add(x[i] + k);
            outList.Add(x[i]);
        }
        return outList;
    }

    public static List<int> Ultrapolation(IReadOnlyList<int> x, IReadOnlyList<int> k)
    {
        var n = x.Count;
        var m = k.Count;
        var outList = new List<int>((m + 1) * (n - 1) + 1) { x[0] };
        for (var i = 1; i < n; i++)
        {
            for (var r = m - 1; r >= 0; r--)
                outList.Add(x[i] + k[r]);
            outList.Add(x[i]);
        }
        return outList;
    }

    public static List<int> Infrapolation(IReadOnlyList<int> x, int k)
    {
        var n = x.Count;
        var outList = new List<int>(2 * n - 1);
        for (var i = 0; i < n - 1; i++)
        {
            outList.Add(x[i]);
            outList.Add(x[i] - k);
        }
        outList.Add(x[n - 1]);
        return outList;
    }

    public static List<int> Infrapolation(IReadOnlyList<int> x, IReadOnlyList<int> k)
    {
        var n = x.Count;
        var m = k.Count;
        var outList = new List<int>((m + 1) * (n - 1) + 1);
        for (var i = 0; i < n - 1; i++)
        {
            outList.Add(x[i]);
            for (var r = 0; r < m; r++)
                outList.Add(x[i] - k[r]);
        }
        outList.Add(x[n - 1]);
        return outList;
    }

    public static List<int> InfraInterpolation(IReadOnlyList<int> x, int k, int l)
    {
        var n = x.Count;
        var outList = new List<int>(3 * (n - 1) + 1);
        for (var i = 0; i < n - 1; i++)
        {
            outList.Add(x[i]);
            outList.Add(x[i] - k);
            outList.Add(x[i] + l);
        }
        outList.Add(x[n - 1]);
        return outList;
    }

    public static List<int> InterInfrapolation(IReadOnlyList<int> x, int k, int l)
    {
        var n = x.Count;
        var outList = new List<int>(3 * (n - 1) + 1);
        for (var i = 0; i < n - 1; i++)
        {
            outList.Add(x[i]);
            outList.Add(x[i] + k);
            outList.Add(x[i] - l);
        }
        outList.Add(x[n - 1]);
        return outList;
    }

    public static List<int> InfraUltrapolation(IReadOnlyList<int> x, int k, int l)
    {
        var n = x.Count;
        var outList = new List<int>(3 * n - 2) { x[0], x[0] - k };
        for (var i = 1; i < n - 1; i++)
        {
            outList.Add(x[i] + l);
            outList.Add(x[i]);
            outList.Add(x[i] - k);
        }
        outList.Add(x[n - 1] + l);
        outList.Add(x[n - 1]);
        return outList;
    }

    public static List<int> InterUltrapolation(IReadOnlyList<int> x, int k, int l)
    {
        var n = x.Count;
        var outList = new List<int>(3 * (n - 1) + 1);
        for (var i = 0; i < n - 1; i++)
        {
            outList.Add(x[i]);
            outList.Add(x[i] + k);
            outList.Add(x[i + 1] - l);
        }
        outList.Add(x[n - 1]);
        return outList;
    }

    public static List<int> UltraInterpolation(IReadOnlyList<int> x, int k, int l)
    {
        var n = x.Count;
        var outList = new List<int>(3 * (n - 1) + 1);
        for (var i = 0; i < n - 1; i++)
        {
            outList.Add(x[i]);
            outList.Add(x[i + 1] + k);
            outList.Add(x[i] + l);
        }
        outList.Add(x[n - 1]);
        return outList;
    }

    public static List<int> InfraInterUltrapolation(IReadOnlyList<int> x, int k, int l, int m)
    {
        var n = x.Count;
        var outList = new List<int>(4 * (n - 1) + 1);
        for (var i = 0; i < n - 1; i++)
        {
            outList.Add(x[i]);
            outList.Add(x[i] - k);
            outList.Add(x[i] + l);
            outList.Add(x[i + 1] + m);
        }
        outList.Add(x[n - 1]);
        return outList;
    }

    public static List<int> InterInfraInterpolation(IReadOnlyList<int> x, int k, int l, int m)
    {
        var n = x.Count;
        var outList = new List<int>(4 * (n - 1) + 1);
        for (var i = 0; i < n - 1; i++)
        {
            outList.Add(x[i]);
            outList.Add(x[i + 1] - k);
            outList.Add(x[i] - l);
            outList.Add(x[i] + m);
        }
        outList.Add(x[n - 1]);
        return outList;
    }

    public static List<int> UltraInfraInterpolation(IReadOnlyList<int> x, int k, int l, int m)
    {
        var n = x.Count;
        var outList = new List<int>(4 * (n - 1) + 1);
        for (var i = 0; i < n - 1; i++)
        {
            outList.Add(x[i]);
            outList.Add(x[i + 1] + k);
            outList.Add(x[i] - l);
            outList.Add(x[i] + m);
        }
        outList.Add(x[n - 1]);
        return outList;
    }
}
