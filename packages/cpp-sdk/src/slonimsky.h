#ifndef MUSICPP_SLONIMSKY_H
#define MUSICPP_SLONIMSKY_H

#include "utility.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace musicpp {

// ============================================================
//  Slonimsky – Thesaurus of Musical Scales and Patterns
//  x  : ordered pitch-position sequence  x[0] < x[1] < ... < x[n-1]
//  k,l: interval offsets (must be < min interval in x)
//  Single-note variants take a plain int offset.
//  Multi-note variants take a vector of offsets (strictly increasing).
// ============================================================

namespace slonimsky {

using Seq = std::vector<int>;

// ------------------------------------------------------------------
// Helpers
// ------------------------------------------------------------------

inline Seq intervals(const Seq& x)
{
    const int n = static_cast<int>(x.size());
    Seq d(n - 1);
    for (int i = 0; i < n - 1; ++i)
        d[i] = x[i + 1] - x[i];
    return d;
}

inline int minInterval(const Seq& x)
{
    const Seq d = intervals(x);
    return *std::min_element(d.begin(), d.end());
}

// Returns the permutation sigma for infrapolation (0-based).
// Useful for verifying the ordering constraint on the offset vector.
inline std::vector<int> infrapolationPermutation(int m)
{
    const int h     = (m + 1) / 2;
    const int delta = (m % 2 == 0) ? 1 : 0;
    std::vector<int> sigma(m);
    for (int i = 1; i <= m; ++i) {
        int val = (i <= h) ? m - 2 * (i - 1) : 2 * (i - h) - delta;
        sigma[i - 1] = val - 1;
    }
    return sigma;
}

// ------------------------------------------------------------------
// Interpolation
// ------------------------------------------------------------------

// Single note: for i=0..n-2 : (x[i], x[i]+k)  then x[n-1]
inline Seq interpolation(const Seq& x, int k)
{
    const int n = static_cast<int>(x.size());
    Seq out;
    out.reserve(2 * n - 1);
    for (int i = 0; i < n - 1; ++i) {
        out.push_back(x[i]);
        out.push_back(x[i] + k);
    }
    out.push_back(x[n - 1]);
    return out;
}

// Multiple notes: for i=0..n-2 : (x[i], x[i]+k[0], ..., x[i]+k[m-1])  then x[n-1]
inline Seq interpolation(const Seq& x, const Seq& k)
{
    const int n = static_cast<int>(x.size());
    const int m = static_cast<int>(k.size());
    Seq out;
    out.reserve((m + 1) * (n - 1) + 1);
    for (int i = 0; i < n - 1; ++i) {
        out.push_back(x[i]);
        for (int j = 0; j < m; ++j)
            out.push_back(x[i] + k[j]);
    }
    out.push_back(x[n - 1]);
    return out;
}

// ------------------------------------------------------------------
// Symmetric interpolation
// ------------------------------------------------------------------

// Single note: alternates upper/lower interpolations
// for j=0..floor((n-1)/2)-1 : (x[2j], x[2j]+k, x[2j+1], x[2j+2]-k)
// then tau depending on parity of n
inline Seq symmetricInterpolation(const Seq& x, int k)
{
    const int n     = static_cast<int>(x.size());
    const int pairs = (n - 1) / 2;
    Seq out;
    out.reserve(4 * pairs + (n % 2 == 1 ? 3 : 1));
    for (int j = 0; j < pairs; ++j) {
        out.push_back(x[2 * j]);
        out.push_back(x[2 * j] + k);
        out.push_back(x[2 * j + 1]);
        out.push_back(x[2 * j + 2] - k);
    }
    if (n % 2 == 1) {
        out.push_back(x[n - 2]);
        out.push_back(x[n - 2] + k);
        out.push_back(x[n - 1]);
    } else {
        out.push_back(x[n - 1]);
    }
    return out;
}

// Multiple notes:
// for j=0..floor((n-1)/2)-1 :
//   (x[2j], x[2j]+k[0], ..., x[2j]+k[m-1],
//    x[2j+1],
//    x[2j+2]-k[m-1], ..., x[2j+2]-k[0])
// then tau depending on parity of n
inline Seq symmetricInterpolation(const Seq& x, const Seq& k)
{
    const int n     = static_cast<int>(x.size());
    const int m     = static_cast<int>(k.size());
    const int pairs = (n - 1) / 2;
    Seq out;
    out.reserve((2 * m + 2) * pairs + (n % 2 == 1 ? m + 2 : 1));
    for (int j = 0; j < pairs; ++j) {
        out.push_back(x[2 * j]);
        for (int r = 0; r < m; ++r)
            out.push_back(x[2 * j] + k[r]);
        out.push_back(x[2 * j + 1]);
        for (int r = m - 1; r >= 0; --r)
            out.push_back(x[2 * j + 2] - k[r]);
    }
    if (n % 2 == 1) {
        out.push_back(x[n - 2]);
        for (int r = 0; r < m; ++r)
            out.push_back(x[n - 2] + k[r]);
        out.push_back(x[n - 1]);
    } else {
        out.push_back(x[n - 1]);
    }
    return out;
}

// ------------------------------------------------------------------
// Asymmetric interpolation
// ------------------------------------------------------------------

// k = upper offsets (ascending), l = lower offsets (ascending, applied reversed)
// for j=0..floor((n-1)/2)-1 :
//   (x[2j], x[2j]+k[0], ..., x[2j]+k[m-1],
//    x[2j+1],
//    x[2j+2]-l[m-1], ..., x[2j+2]-l[0])
// then tau (uses k for the odd tail)
inline Seq asymmetricInterpolation(const Seq& x, const Seq& k, const Seq& l)
{
    const int n     = static_cast<int>(x.size());
    const int m     = static_cast<int>(k.size());
    const int pairs = (n - 1) / 2;
    Seq out;
    out.reserve((2 * m + 2) * pairs + (n % 2 == 1 ? m + 2 : 1));
    for (int j = 0; j < pairs; ++j) {
        out.push_back(x[2 * j]);
        for (int r = 0; r < m; ++r)
            out.push_back(x[2 * j] + k[r]);
        out.push_back(x[2 * j + 1]);
        for (int r = m - 1; r >= 0; --r)
            out.push_back(x[2 * j + 2] - l[r]);
    }
    if (n % 2 == 1) {
        out.push_back(x[n - 2]);
        for (int r = 0; r < m; ++r)
            out.push_back(x[n - 2] + k[r]);
        out.push_back(x[n - 1]);
    } else {
        out.push_back(x[n - 1]);
    }
    return out;
}

// ------------------------------------------------------------------
// Ultrapolation
// ------------------------------------------------------------------

// Single note: (x[0]) ++ for i=1..n-1 : (x[i]+k, x[i])
inline Seq ultrapolation(const Seq& x, int k)
{
    const int n = static_cast<int>(x.size());
    Seq out;
    out.reserve(2 * n - 1);
    out.push_back(x[0]);
    for (int i = 1; i < n; ++i) {
        out.push_back(x[i] + k);
        out.push_back(x[i]);
    }
    return out;
}

// Multiple notes: (x[0]) ++ for i=1..n-1 : (x[i]+k[m-1], ..., x[i]+k[0], x[i])
inline Seq ultrapolation(const Seq& x, const Seq& k)
{
    const int n = static_cast<int>(x.size());
    const int m = static_cast<int>(k.size());
    Seq out;
    out.reserve((m + 1) * (n - 1) + 1);
    out.push_back(x[0]);
    for (int i = 1; i < n; ++i) {
        for (int r = m - 1; r >= 0; --r)
            out.push_back(x[i] + k[r]);
        out.push_back(x[i]);
    }
    return out;
}

// ------------------------------------------------------------------
// Infrapolation
// ------------------------------------------------------------------

// Single note: for i=0..n-2 : (x[i], x[i]-k)  then x[n-1]
inline Seq infrapolation(const Seq& x, int k)
{
    const int n = static_cast<int>(x.size());
    Seq out;
    out.reserve(2 * n - 1);
    for (int i = 0; i < n - 1; ++i) {
        out.push_back(x[i]);
        out.push_back(x[i] - k);
    }
    out.push_back(x[n - 1]);
    return out;
}

// Multiple notes: for i=0..n-2 : (x[i], x[i]-k[0], ..., x[i]-k[m-1])  then x[n-1]
// The permutation sigma defines the ordering constraint on k (see infrapolationPermutation).
inline Seq infrapolation(const Seq& x, const Seq& k)
{
    const int n = static_cast<int>(x.size());
    const int m = static_cast<int>(k.size());
    Seq out;
    out.reserve((m + 1) * (n - 1) + 1);
    for (int i = 0; i < n - 1; ++i) {
        out.push_back(x[i]);
        for (int r = 0; r < m; ++r)
            out.push_back(x[i] - k[r]);
    }
    out.push_back(x[n - 1]);
    return out;
}

// ------------------------------------------------------------------
// Composite operations
// ------------------------------------------------------------------

// infraInterpolation: for i=0..n-2 : (x[i], x[i]-k, x[i]+l)  then x[n-1]
inline Seq infraInterpolation(const Seq& x, int k, int l)
{
    const int n = static_cast<int>(x.size());
    Seq out;
    out.reserve(3 * (n - 1) + 1);
    for (int i = 0; i < n - 1; ++i) {
        out.push_back(x[i]);
        out.push_back(x[i] - k);
        out.push_back(x[i] + l);
    }
    out.push_back(x[n - 1]);
    return out;
}

// interInfrapolation: for i=0..n-2 : (x[i], x[i]+k, x[i]-l)  then x[n-1]
inline Seq interInfrapolation(const Seq& x, int k, int l)
{
    const int n = static_cast<int>(x.size());
    Seq out;
    out.reserve(3 * (n - 1) + 1);
    for (int i = 0; i < n - 1; ++i) {
        out.push_back(x[i]);
        out.push_back(x[i] + k);
        out.push_back(x[i] - l);
    }
    out.push_back(x[n - 1]);
    return out;
}

// infraUltrapolation: (x[0], x[0]-k) ++ for i=1..n-2 : (x[i]+l, x[i], x[i]-k) ++ (x[n-1]+l, x[n-1])
inline Seq infraUltrapolation(const Seq& x, int k, int l)
{
    const int n = static_cast<int>(x.size());
    Seq out;
    out.reserve(3 * n - 2);
    out.push_back(x[0]);
    out.push_back(x[0] - k);
    for (int i = 1; i < n - 1; ++i) {
        out.push_back(x[i] + l);
        out.push_back(x[i]);
        out.push_back(x[i] - k);
    }
    out.push_back(x[n - 1] + l);
    out.push_back(x[n - 1]);
    return out;
}

// interUltrapolation: for i=0..n-2 : (x[i], x[i]+k, x[i+1]-l)  then x[n-1]
inline Seq interUltrapolation(const Seq& x, int k, int l)
{
    const int n = static_cast<int>(x.size());
    Seq out;
    out.reserve(3 * (n - 1) + 1);
    for (int i = 0; i < n - 1; ++i) {
        out.push_back(x[i]);
        out.push_back(x[i] + k);
        out.push_back(x[i + 1] - l);
    }
    out.push_back(x[n - 1]);
    return out;
}

// ultraInterpolation: for i=0..n-2 : (x[i], x[i+1]+k, x[i]+l)  then x[n-1]
inline Seq ultraInterpolation(const Seq& x, int k, int l)
{
    const int n = static_cast<int>(x.size());
    Seq out;
    out.reserve(3 * (n - 1) + 1);
    for (int i = 0; i < n - 1; ++i) {
        out.push_back(x[i]);
        out.push_back(x[i + 1] + k);
        out.push_back(x[i] + l);
    }
    out.push_back(x[n - 1]);
    return out;
}

// infraInterUltrapolation: for i=0..n-2 : (x[i], x[i]-k, x[i]+l, x[i+1]+m)  then x[n-1]
inline Seq infraInterUltrapolation(const Seq& x, int k, int l, int m)
{
    const int n = static_cast<int>(x.size());
    Seq out;
    out.reserve(4 * (n - 1) + 1);
    for (int i = 0; i < n - 1; ++i) {
        out.push_back(x[i]);
        out.push_back(x[i] - k);
        out.push_back(x[i] + l);
        out.push_back(x[i + 1] + m);
    }
    out.push_back(x[n - 1]);
    return out;
}

// interInfraInterpolation [hapax legomenon]:
// for i=0..n-2 : (x[i], x[i+1]-k, x[i]-l, x[i]+m)  then x[n-1]
inline Seq interInfraInterpolation(const Seq& x, int k, int l, int m)
{
    const int n = static_cast<int>(x.size());
    Seq out;
    out.reserve(4 * (n - 1) + 1);
    for (int i = 0; i < n - 1; ++i) {
        out.push_back(x[i]);
        out.push_back(x[i + 1] - k);
        out.push_back(x[i] - l);
        out.push_back(x[i] + m);
    }
    out.push_back(x[n - 1]);
    return out;
}

// ultraInfraInterpolation [hapax legomenon]:
// for i=0..n-2 : (x[i], x[i+1]+k, x[i]-l, x[i]+m)  then x[n-1]
inline Seq ultraInfraInterpolation(const Seq& x, int k, int l, int m)
{
    const int n = static_cast<int>(x.size());
    Seq out;
    out.reserve(4 * (n - 1) + 1);
    for (int i = 0; i < n - 1; ++i) {
        out.push_back(x[i]);
        out.push_back(x[i + 1] + k);
        out.push_back(x[i] - l);
        out.push_back(x[i] + m);
    }
    out.push_back(x[n - 1]);
    return out;
}

} // namespace slonimsky

static void print(const std::string& label, const slonimsky::Seq& s)
{
    std::cout << label << ": [ ";
    for (int v : s) std::cout << v << " ";
    std::cout << "]\n";
}

} // namespace musicpp

#endif // MUSICPP_SLONIMSKY_H
