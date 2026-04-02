namespace Musicpp;

public static class Measures
{
    public static List<int> Differences(IReadOnlyList<int> input)
    {
        if (input.Count < 2)
            return new List<int>();
        var outList = new List<int>();
        for (var i = 1; i < input.Count; i++)
            outList.Add(input[i] - input[i - 1]);
        return outList;
    }

    public static int GeodesicDistance(int a, int b, int mod)
    {
        if (mod <= 0)
            throw new ArgumentException("geodesicDistance: mod must be positive");
        var distance = (b - a + mod) % mod;
        if (distance > mod / 2)
            distance = mod - distance;
        return distance;
    }

    public static List<int> GeodesicDistances(PositionVector input)
    {
        var distances = new List<int>();
        var n = input.Count;
        for (var i = 0; i < n; i++)
        {
            for (var j = i + 1; j < n; j++)
                distances.Add(GeodesicDistance(input[i], input[j], input.Mod));
        }
        return distances;
    }

    public static bool IsEuclidean(PositionVector input, int mod)
    {
        var j = MusicVectors.PositionsToIntervals(input);
        var temp = j.MutableData.ToList();
        var temp2 = temp.ToList();
        var n = temp.Count;
        if (n == 0)
            return false;
        temp[0]++;
        temp[n - 1]--;
        for (var i = 0; i < n; i++)
        {
            if (temp.SequenceEqual(temp2))
                return true;
            var first = temp[0];
            temp.RemoveAt(0);
            temp.Add(first);
        }
        return false;
    }

    public static SortedDictionary<int, int> CalculateOccurrences(IReadOnlyList<int> input)
    {
        var occurrences = new SortedDictionary<int, int>();
        foreach (var occurrence in input)
            occurrences[occurrence] = occurrences.GetValueOrDefault(occurrence) + 1;
        return occurrences;
    }

    public static bool IsWinogradDeep(SortedDictionary<int, int> input, int size)
    {
        var counts = new HashSet<int>();
        foreach (var entry in input)
        {
            var count = entry.Value;
            if (count < 1 || count >= size)
                continue;
            if (!counts.Add(count))
                return false;
        }
        return counts.Count == size - 1;
    }

    public static bool IsErdosDeep(SortedDictionary<int, int> input)
    {
        var seen = new HashSet<int>();
        foreach (var pair in input)
        {
            if (!seen.Add(pair.Value))
                return false;
        }
        return true;
    }

    public static double CalculateRegressionEvenness(IReadOnlyList<int> rhythm, int totalTimeUnits)
    {
        var numNotes = rhythm.Count;
        if (numNotes == 0)
            return 0.0;
        var idealInterval = (double)totalTimeUnits / numNotes;
        var deviations = new double[numNotes];
        for (var i = 0; i < numNotes; i++)
            deviations[i] = Math.Abs(rhythm[i] - i * idealInterval);
        return deviations.Sum();
    }

    public static int CalculateRhythmicOddity(PositionVector input)
    {
        var k = (int)input.Count;
        var rhythmicOddity = 0;
        var mod = input.Mod;
        for (var i = 0; i < k; i++)
        {
            for (var j = i + 1; j < k; j++)
            {
                var dist1 = (input[j] - input[i] + mod) % mod;
                var dist2 = mod - dist1;
                if (dist1 == dist2)
                    rhythmicOddity++;
            }
        }
        return rhythmicOddity;
    }

    public static int ComputeTransitionComplexity(PositionVector input, int mod)
    {
        var onsetVec = MusicVectors.PositionsToOnset(input);
        if (onsetVec.Count == 0)
            return 0;
        var complexity = 0;
        for (var i = 1; i < onsetVec.Count; i++)
        {
            if (onsetVec[i] != onsetVec[i - 1])
                complexity++;
        }
        if (onsetVec[^1] != onsetVec[0])
            complexity++;
        return complexity;
    }

    public static double ComputeEntropy(PositionVector input)
    {
        var onsetVec = MusicVectors.PositionsToOnset(input);
        if (onsetVec.Count == 0)
            return 0.0;
        var frequency = new Dictionary<int, int>();
        foreach (var v in onsetVec.MutableData)
            frequency[v] = frequency.GetValueOrDefault(v) + 1;
        var entropy = 0.0;
        var n = onsetVec.Count;
        foreach (var pair in frequency)
        {
            var probability = (double)pair.Value / n;
            entropy -= probability * Math.Log2(probability);
        }
        return entropy;
    }

    public static int ComputeLongestSubsequence(PositionVector input)
    {
        var onsetVec = MusicVectors.PositionsToOnset(input);
        var data = onsetVec.MutableData;
        var n = data.Count;
        if (n == 0)
            return 0;
        var longest = 1;
        var currentRun = 1;
        for (var i = 1; i < n; i++)
        {
            if (data[i] == data[i - 1])
            {
                currentRun++;
                if (currentRun > longest)
                    longest = currentRun;
            }
            else
                currentRun = 1;
        }

        if (n >= 2 && data[0] == data[n - 1])
        {
            var wrapVal = data[0];
            var prefix = 0;
            while (prefix < n && data[prefix] == wrapVal)
                prefix++;
            var suffix = 0;
            while (suffix < n && data[n - 1 - suffix] == wrapVal)
                suffix++;
            if (prefix == n)
                longest = n;
            else
                longest = Math.Max(longest, prefix + suffix);
        }

        return longest;
    }

    public static void PrintDistances(PositionVector input, IReadOnlyList<int> distances)
    {
        var index = 0;
        var n = input.Count;
        for (var i = 0; i < n; i++)
        {
            for (var j = i + 1; j < n; j++)
                Console.WriteLine($"Distance between {input[i]} and {input[j]} is: {distances[index++]}");
        }
    }

    public static void PrintOccurrences(SortedDictionary<int, int> occurrences)
    {
        foreach (var pair in occurrences)
            Console.WriteLine($"Distance {pair.Key} appears {pair.Value} times");
    }

    public static void PrintDeepness(SortedDictionary<int, int> occurrences, int size)
    {
        var winogradDeep = IsWinogradDeep(occurrences, size);
        var erdosDeep = IsErdosDeep(occurrences);
        Console.WriteLine("The vector is " + (winogradDeep ? "" : "not ") + "Winograd-deep");
        Console.WriteLine("The vector is " + (erdosDeep ? "" : "not ") + "Erdos-deep");
    }

    public static List<SortedSet<int>> CalculateDistributionSpectra(PositionVector input)
    {
        var normalizedScale = input.MutableData.ToList();
        if (normalizedScale.Count == 0)
            return new List<SortedSet<int>>();
        var mod = input.Mod;
        var distributionSpectra = new List<SortedSet<int>>();
        for (var k = 0; k < normalizedScale.Count - 1; k++)
            distributionSpectra.Add(new SortedSet<int>());
        var sz = normalizedScale.Count;
        for (var i = 0; i < sz; i++)
        {
            for (var j = 1; j < sz; j++)
            {
                var genericInterval = j;
                var specificInterval = (normalizedScale[(i + j) % sz] - normalizedScale[i] + mod) % mod;
                distributionSpectra[genericInterval - 1].Add(specificInterval);
            }
        }
        return distributionSpectra;
    }

    public static List<int> CalculateSpectrumWidths(List<SortedSet<int>> spectra)
    {
        var widths = new List<int>();
        foreach (var spectrum in spectra)
        {
            if (spectrum.Count == 0)
                widths.Add(0);
            else
                widths.Add(spectrum.Max - spectrum.Min);
        }
        return widths;
    }

    public static double CalculateSpectrumVariation(IReadOnlyList<int> widths, int numberOfTones)
    {
        if (numberOfTones <= 0)
            return 0.0;
        return (double)widths.Sum() / numberOfTones;
    }

    public static List<int> FindRotationalSymmetryAxes(PositionVector scale)
    {
        var normalizedScale = scale.MutableData.ToList();
        var sortedNormalized = normalizedScale.OrderBy(x => x).ToList();
        var axes = new List<int>();
        var n = normalizedScale.Count;
        for (var interval = 1; interval < scale.Mod; interval++)
        {
            var transposedScale = new int[n];
            for (var i = 0; i < n; i++)
                transposedScale[i] = (normalizedScale[i] + interval) % scale.Mod;
            var sortedT = transposedScale.OrderBy(x => x).ToList();
            if (sortedT.SequenceEqual(sortedNormalized))
                axes.Add(interval);
        }
        return axes;
    }

    public static List<double> FindReflectiveSymmetryAxes(PositionVector scale)
    {
        var normalizedScale = scale.MutableData.ToList();
        var axes = new List<double>();
        var n = normalizedScale.Count;
        var modD = (double)scale.Mod;
        for (var axisDoubled = 0; axisDoubled < 2 * scale.Mod; axisDoubled++)
        {
            var axis = axisDoubled / 2.0;
            var isSymmetric = true;
            for (var i = 0; i < n; i++)
            {
                var reflected = 2 * axis - normalizedScale[i];
                var wrapped = reflected % modD;
                if (wrapped < 0)
                    wrapped += modD;
                var reflectedNote = (int)Math.Round(wrapped);
                reflectedNote = ((reflectedNote % scale.Mod) + scale.Mod) % scale.Mod;
                if (!normalizedScale.Contains(reflectedNote))
                {
                    isSymmetric = false;
                    break;
                }
            }
            if (isSymmetric)
                axes.Add(axis);
        }
        return axes;
    }

    public static bool IsPrime(int num)
    {
        if (num <= 1)
            return false;
        if (num == 2)
            return true;
        if (num % 2 == 0)
            return false;
        for (var i = 3; i <= (int)Math.Sqrt(num); i += 2)
        {
            if (num % i == 0)
                return false;
        }
        return true;
    }

    public static void ClassifyAksakRhythm(int mod)
    {
        if (IsPrime(mod))
            Console.WriteLine("The rhythm is authentic aksak");
        else if (mod % 2 != 0)
            Console.WriteLine("The rhythm is quasi-aksak");
        else
            Console.WriteLine("The rhythm is pseudo-aksak");
    }

    public static bool IsPalindrome(PositionVector scale) =>
        FindReflectiveSymmetryAxes(scale).Any(a => Math.Abs(a - 0.0) < 1e-9);

    public static bool IsChiral(PositionVector scale)
    {
        var normalizedScale = scale.MutableData.ToList();
        var sortedNormalized = normalizedScale.OrderBy(x => x).ToList();
        var mirroredScale = normalizedScale.Select(note => (scale.Mod - note) % scale.Mod).OrderBy(x => x).ToList();
        if (sortedNormalized.SequenceEqual(mirroredScale))
            return false;
        var n = normalizedScale.Count;
        for (var interval = 1; interval < scale.Mod; interval++)
        {
            var transposedMirrored = new int[n];
            for (var i = 0; i < n; i++)
                transposedMirrored[i] = (mirroredScale[i] + interval) % scale.Mod;
            var sortedT = transposedMirrored.OrderBy(x => x).ToList();
            if (sortedT.SequenceEqual(sortedNormalized))
                return false;
        }
        return true;
    }

    public static bool IsBalanced(PositionVector scale)
    {
        double xSum = 0, ySum = 0;
        var angleStep = 2 * Math.PI / scale.Mod;
        foreach (var note in scale.MutableData)
        {
            var angle = note * angleStep;
            xSum += Math.Cos(angle);
            ySum += Math.Sin(angle);
        }
        return Math.Abs(xSum) < 1e-6 && Math.Abs(ySum) < 1e-6;
    }

    public static List<int> Generate(int m, int k, int n, bool printSteps = false)
    {
        if (n <= 0)
            throw new ArgumentException("generate: modulus n must be positive");
        if (k < 0)
            throw new ArgumentException("generate: k must be non-negative");
        var sequence = new List<int>();
        for (var i = 0; i < k; i++)
        {
            var value = (i * m) % n;
            if (printSteps)
                Console.WriteLine($"{i} x {m} mod {n} = {value}");
            sequence.Add(value);
        }
        return sequence;
    }

    public static (bool Found, int Multiplier) IsGenerated(IReadOnlyList<int> input, int n)
    {
        var k = input.Count;
        var sortedIn = input.OrderBy(x => x).ToList();
        for (var mult = 1; mult < n; mult++)
        {
            var generatedSeq = Generate(mult, k, n);
            var sortedG = generatedSeq.OrderBy(x => x).ToList();
            if (sortedG.SequenceEqual(sortedIn))
                return (true, mult);
        }
        return (false, -1);
    }

    public static void PrintGenerators(IReadOnlyList<int> input, int mod)
    {
        var (found, m) = IsGenerated(input.ToList(), mod);
        if (found)
        {
            Console.WriteLine($"The vector is generated by multiples of m = {m} mod {mod}");
            Console.WriteLine("Generators:");
            Generate(m, input.Count, mod, true);
        }
        else
            Console.WriteLine($"The vector is not generated by multiples of any integer m under modulo {mod}");
    }

    public static void PrintDistributionSpectra(List<SortedSet<int>> spectra)
    {
        for (var i = 0; i < spectra.Count; i++)
            Console.WriteLine("<{0}> = {{{1}}}", i + 1, string.Join(",", spectra[i]));
    }

    public static void PrintSpectrumWidths(IReadOnlyList<int> widths)
    {
        for (var i = 0; i < widths.Count; i++)
            Console.WriteLine($"Width of <{i + 1}> = {widths[i]}");
    }

    public static void PrintSymmetryAxes(IReadOnlyList<int> axes, string symmetryType)
    {
        Console.Write(symmetryType + " symmetry axes: ");
        Console.WriteLine(string.Join(", ", axes));
    }

    public static void PrintSymmetryAxes(IReadOnlyList<double> axes, string symmetryType)
    {
        Console.Write(symmetryType + " symmetry axes: ");
        Console.WriteLine(string.Join(", ", axes));
    }

    public static void PrintAnalysis(PositionVector p)
    {
        var mod = p.Mod;
        var j = MusicVectors.PositionsToIntervals(p);
        var intervals = j.MutableData;
        var onsets = MusicVectors.PositionsToOnset(p);
        var distances = GeodesicDistances(p);
        var occurrences = CalculateOccurrences(distances);
        var spectra = CalculateDistributionSpectra(p);
        var widths = CalculateSpectrumWidths(spectra);
        var variation = CalculateSpectrumVariation(widths, (int)p.Count);

        Console.WriteLine("Analysis Results:");
        Console.WriteLine();
        Console.WriteLine("Modulo:");
        Console.WriteLine(mod);
        Console.WriteLine();
        Console.WriteLine("Positions:");
        Console.WriteLine(string.Join(" ", p.MutableData));
        Console.WriteLine();
        Console.WriteLine("Intervals:");
        Console.WriteLine(string.Join(" ", intervals));
        Console.WriteLine();
        Console.WriteLine("Onsets:");
        Console.WriteLine(string.Join(" ", onsets.MutableData));
        Console.WriteLine();
        Console.WriteLine("Distances:");
        PrintDistances(p, distances);
        Console.WriteLine();
        Console.WriteLine("Occurrences:");
        PrintOccurrences(occurrences);
        Console.WriteLine();
        Console.WriteLine("Deepness:");
        PrintDeepness(occurrences, p.MutableData.Count);
        Console.WriteLine();
        Console.WriteLine("Generators check:");
        PrintGenerators(p.MutableData, mod);
        Console.WriteLine();
        Console.WriteLine("Aksak Check:");
        ClassifyAksakRhythm(mod);
        Console.WriteLine();
        Console.WriteLine("Regression Evenness:");
        var rhythmList = p.MutableData.ToList();
        Console.WriteLine(CalculateRegressionEvenness(rhythmList, mod));
        Console.WriteLine();
        Console.WriteLine("Rhythmic Oddity:");
        Console.WriteLine(CalculateRhythmicOddity(p));
        Console.WriteLine();
        Console.WriteLine("Shannon Entropy:");
        Console.WriteLine(ComputeEntropy(p));
        Console.WriteLine();
        Console.WriteLine("Longest Subsequence:");
        Console.WriteLine(ComputeLongestSubsequence(p));
        Console.WriteLine();
        Console.WriteLine("Euclidean String Check:");
        var euclidean = IsEuclidean(p, mod);
        Console.WriteLine("The interval vector is " + (euclidean ? "" : "not ") + "an Euclidean string");
        Console.WriteLine();
        Console.WriteLine("Distribution Spectra:");
        PrintDistributionSpectra(spectra);
        Console.WriteLine();
        Console.WriteLine("Spectrum Widths:");
        PrintSpectrumWidths(widths);
        Console.WriteLine();
        Console.WriteLine("Spectrum Variation:");
        Console.WriteLine(variation);
        Console.WriteLine();
        Console.WriteLine("Symmetry Analysis:");
        var rot = FindRotationalSymmetryAxes(p);
        var refl = FindReflectiveSymmetryAxes(p);
        PrintSymmetryAxes(rot, "Rotational");
        PrintSymmetryAxes(refl, "Reflective");
        Console.WriteLine();
        Console.WriteLine("Transition complexity:");
        Console.WriteLine(ComputeTransitionComplexity(p, mod));
        Console.WriteLine();
        Console.WriteLine("Palindrome Check:");
        Console.WriteLine("The vector is " + (IsPalindrome(p) ? "" : "not ") + "palindrome");
        Console.WriteLine();
        Console.WriteLine("Chirality Check:");
        Console.WriteLine("The vector is " + (IsChiral(p) ? "" : "not ") + "chiral");
        Console.WriteLine();
        Console.WriteLine("Balance Check:");
        Console.WriteLine("The vector is " + (IsBalanced(p) ? "" : "not ") + "balanced");
        Console.WriteLine();
    }
}
