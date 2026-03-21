#include "../src/slonimsky.h"

int main()
{
    using namespace slonimsky;

    const Seq x = {0, 6, 12};

    cout << "=== Source vector ===\n";
    print("x", x);
    cout << "minInterval = " << minInterval(x) << "\n\n";

    cout << "=== Interpolation ===\n";
    print("interpolation(x, 2)",        interpolation(x, 2));
    print("interpolation(x, {2,4})",    interpolation(x, {2, 4}));
    print("interpolation(x, {1,2,4})",  interpolation(x, {1, 2, 4}));

    cout << "\n=== Symmetric interpolation ===\n";
    print("symmetricInterpolation(x, 2)",       symmetricInterpolation(x, 2));
    print("symmetricInterpolation(x, {2,4})",   symmetricInterpolation(x, {2, 4}));

    cout << "\n=== Asymmetric interpolation ===\n";
    print("asymmetricInterpolation(x, {2,4}, {1,3})", asymmetricInterpolation(x, {2, 4}, {1, 3}));

    cout << "\n=== Ultrapolation ===\n";
    print("ultrapolation(x, 2)",       ultrapolation(x, 2));
    print("ultrapolation(x, {2,4})",   ultrapolation(x, {2, 4}));

    cout << "\n=== Infrapolation ===\n";
    print("infrapolation(x, 2)",        infrapolation(x, 2));
    print("infrapolation(x, {2,4})",    infrapolation(x, {2, 4}));
    print("infrapolation(x, {1,3,5})",  infrapolation(x, {1, 3, 5}));

    auto sigma3 = infrapolationPermutation(3);
    cout << "infrapolationPermutation(3) [0-based]: [ ";
    for (int v : sigma3) cout << v << " ";
    cout << "]\n";

    cout << "\n=== Composite operations ===\n";
    print("infraInterpolation(x, 2, 3)",         infraInterpolation(x, 2, 3));
    print("interInfrapolation(x, 3, 2)",          interInfrapolation(x, 3, 2));
    print("infraUltrapolation(x, 2, 3)",          infraUltrapolation(x, 2, 3));
    print("interUltrapolation(x, 2, 3)",          interUltrapolation(x, 2, 3));
    print("ultraInterpolation(x, 2, 3)",          ultraInterpolation(x, 2, 3));
    print("infraInterUltrapolation(x, 2, 3, 1)",  infraInterUltrapolation(x, 1, 1, 1));
    print("interInfraInterpolation(x, 1, 2, 3)",  interInfraInterpolation(x, 1, 2, 3));
    print("ultraInfraInterpolation(x, 1, 2, 3)",  ultraInfraInterpolation(x, 1, 2, 3));

    return 0;
}
