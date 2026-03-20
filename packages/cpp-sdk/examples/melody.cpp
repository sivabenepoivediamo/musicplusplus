#include "../src/melody.h"

int main() {
    const vector<int> chord     = { 0, 4, 7 };
    const vector<int> scale     = { 0, 2, 4, 5, 7, 9, 11 };
    const vector<int> chromatic = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    const int              mod       = 12;
    const vector<int> testNotes = { 60 };

    using T = VectorModification::Type;

    struct OrnamentTest {
        vector<int>   ornaments;
        vector<T>     types;
        string        description;
    };

    // Helper to concatenate vectors
    auto cat = [](vector<int> a, const vector<int>& b) {
        a.insert(a.end(), b.begin(), b.end());
        return a;
    };

    auto repeat = [](const vector<int>& v, int times) {
        vector<int> result;
        result.reserve(v.size() * times);
        for (int i = 0; i < times; ++i)
            result.insert(result.end(), v.begin(), v.end());
        return result;
    };

    const vector<OrnamentTest> tests = {
        { ornamentLoop({1, 0}, 7),          {T::SCALE},                          "Trill (7 notes)" },
        { ornamentLoop({-1, 0}, 5),         {T::SCALE},                          "Double descending mordente (5 notes)" },
        { ornamentLoop({1, 0, -1, 0}, 5),   {T::SCALE, T::CHROMATIC},            "Gruppetto (5 notes)" },
        { diminution(0, 4, false, true),    {T::SCALE},                          "Diminution: Ascending scale (4 notes)" },
        { diminution(0, 5, false, false),   {T::SCALE},                          "Diminution: Descending scale (5 notes)" },
        { diminution(0, 4, false, false),   {T::SCALE},                          "Diminution: Descending scale (3 notes)" },
        { run(4, 6, true),                  {T::SCALE},                          "Run: Ascending scale approach to degree 4 (6 notes)" },
        { run(2, 4, false),                 {T::CHROMATIC},                      "Run: Descending chromatic approach to degree 2 (4 notes)" },
        { run(3, 5, true),                  {T::CHORD},                          "Run: Ascending chord approach to degree 3 (5 notes)" },
        { run2(-2, 2),                      {T::SCALE},                          "Run2: Ascending scale from -2 to 2" },
        { run2(3, -3),                      {T::CHROMATIC},                      "Run2: Descending chromatic from 3 to -3" },
        { cat(run(2, 3, true), diminution(2, 4, true, false)), {T::SCALE},       "Combined: Ascending run followed by descending diminution" },
        { cat(run2(0, 3), run2(3, 0)),      {T::CHORD},                          "Combined: Ascending-descending run2 pattern" },
        { run(2, 4, true),                  {T::CHORD, T::CHROMATIC, T::SCALE, T::SCALE}, "Mixed: Ascending run through different contexts" },
        {
            cat(repeat(diminution(0, 4, false, true), 2), repeat(run2(2, 4), 2)),
            {T::SCALE},
            "Fra martino"
        },
    };

    auto typeStr = [](T t) -> string {
        switch (t) {
            case T::CHORD:    return "chord";
            case T::SCALE:    return "scale";
            case T::CHROMATIC: return "chromatic";
        }
        return "?";
    };

    for (size_t i = 0; i < tests.size(); ++i) {
        const auto& test = tests[i];
        cout << "\nTest " << (i + 1) << ": " << test.description << "\n";
        cout << "Ornament pattern: [" << join(test.ornaments) << "]\n";
        cout << "Type: [";
        for (size_t j = 0; j < test.types.size(); ++j) {
            if (j) cout << ", ";
            cout << typeStr(test.types[j]);
        }
        cout << "]\n";

        applyTripleSelect(testNotes, chord, scale, chromatic, mod,
                          test.ornaments, test.types);
    }

    return 0;
}
