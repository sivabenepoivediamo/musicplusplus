namespace Musicpp;

public interface ICopyable<out T>
{
    T Copy();
}

public readonly record struct DivisionResult(int Quotient, int Remainder);

public static class MathUtil
{
    public static DivisionResult EuclideanDivision(int dividend, int divisor)
    {
        if (divisor == 0)
            throw new ArgumentException("euclidean_division: divisor is zero", nameof(divisor));
        var b = Math.Abs(divisor);
        var quotient = dividend / b;
        var remainder = dividend - quotient * b;
        if (remainder < 0)
        {
            quotient -= 1;
            remainder += b;
        }
        if (divisor < 0)
            quotient = -quotient;
        return new DivisionResult(quotient, remainder);
    }

    public static int Gcd(int a, int b)
    {
        if (b == 0)
            return Math.Abs(a);
        return Gcd(b, a % b);
    }

    public static int Lcm(IReadOnlyList<int> values)
    {
        if (values.Count == 0)
            return 1;
        if (values.Count == 1)
            return Math.Abs(values[0]);
        var result = Math.Abs(values[0]);
        for (var i = 1; i < values.Count; i++)
        {
            var ai = Math.Abs(values[i]);
            if (ai == 0)
                return 0;
            var g = Gcd(result, ai);
            result = result / g * ai;
        }
        return result;
    }
}
