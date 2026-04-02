#ifndef MUSICPP_MELODY_H
#define MUSICPP_MELODY_H

#include "utility.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>



namespace musicpp {

struct NoteInfo {
    int position;
    int degree;
    int octave;
    int noteValue;
};

struct Analysis {
    NoteInfo chord;
    NoteInfo scale;
    NoteInfo chromatic;
};

struct VectorModification {
    enum class Type { CHORD, SCALE, CHROMATIC };
    Type type;
    int  delta;
};

struct TripleSelectResult {
    std::vector<int>  results;
    std::vector<bool> isOut;
};


static NoteInfo invalidNote() {
    return { -666, -666, -666, -666 };
}

std::vector<int> diminution(int degree, int length, bool up, bool left) {
    if (length <= 0) return {};
    if (length == 1) return { degree };

    std::vector<int> result(length);
    result[0]          = degree;
    result[length - 1] = degree;

    const int distance = length - 2;
    const int firstNum = up ? degree - distance : degree + distance;

    std::vector<int> middle;
    for (int i = 1; i < length - 1; ++i) {
        middle.push_back(up ? firstNum + (i - 1) : firstNum - (i - 1));
    }

    if (left)
        std::sort(middle.begin(), middle.end());
    else
        std::sort(middle.begin(), middle.end(), std::greater<int>());

    for (size_t i = 0; i < middle.size(); ++i)
        result[i + 1] = middle[i];

    return result;
}

std::vector<int> run(int degree, int length, bool direction) {
    if (length <= 0) {
        throw std::invalid_argument("run: length must be positive");
    }
    const int step  = direction ? 1 : -1;
    const int start = direction ? degree - (length - 1) : degree + (length - 1);
    std::vector<int> result(length);
    for (int i = 0; i < length; ++i)
        result[i] = start + i * step;
    return result;
}

std::vector<int> run2(int start, int end) {
    std::vector<int> result;
    if (start <= end) {
        for (int i = start; i <= end; ++i) result.push_back(i);
    } else {
        for (int i = start; i >= end; --i) result.push_back(i);
    }
    return result;
}

std::vector<int> normalizeNotes(const std::vector<int>& notes, int mod) {
    std::vector<int> result(notes.size());
    for (size_t i = 0; i < notes.size(); ++i) {
        int n = notes[i] % mod;
        result[i] = n < 0 ? n + mod : n;
    }
    return result;
}

static NoteInfo info(int inputNote, const std::vector<int>& vector, int mod) {
    const int normalizedNote = inputNote % mod;
    const int baseOctave     = static_cast<int>(std::floor(static_cast<double>(inputNote) / mod));

    int degree = -1;
    for (size_t i = 0; i < vector.size(); ++i) {
        if (vector[i] % mod == normalizedNote) {
            degree = static_cast<int>(i);
            break;
        }
    }

    if (degree == -1) return invalidNote();

    const int octaveAdjustment = static_cast<int>(std::floor(static_cast<double>(vector[degree]) / mod));
    const int position         = (baseOctave + octaveAdjustment) * static_cast<int>(vector.size()) + degree;

    return { position, degree, baseOctave + octaveAdjustment, inputNote };
}

Analysis hierarchy(
    int note,
    const std::vector<int>& chord,
    const std::vector<int>& scale,
    const std::vector<int>& chromatic,
    int mod)
{
    const std::vector<int> normChord = normalizeNotes(chord, mod);
    return {
        info(note, normChord,   mod),
        info(note, scale,       mod),
        info(note, chromatic,   mod)
    };
}

static int getNoteFromPosition(int position, const std::vector<int>& vector, int mod) {
    const int vlen   = static_cast<int>(vector.size());
    const int octave = static_cast<int>(std::floor(static_cast<double>(position) / vlen));
    const int degree = ((position % vlen) + vlen) % vlen;   // safe mod for negatives
    return vector[degree] + octave * mod;
}

TripleSelectResult tripleSelect(
    const Analysis&                       analysis,
    const std::vector<VectorModification>& modifications,
    const std::vector<int>&               chord,
    const std::vector<int>&               scale,
    const std::vector<int>&               chromatic,
    int                                   mod)
{
    TripleSelectResult out;

    for (const auto& mod_ : modifications) {
        int                    basePosition;
        const std::vector<int>* vec;

        switch (mod_.type) {
            case VectorModification::Type::CHORD:
                basePosition = analysis.chord.position;
                vec          = &chord;
                break;
            case VectorModification::Type::SCALE:
                basePosition = analysis.scale.position;
                vec          = &scale;
                break;
            case VectorModification::Type::CHROMATIC:
                basePosition = analysis.chromatic.position;
                vec          = &chromatic;
                break;
            default:
                throw std::runtime_error("Invalid modification type");
        }

        if (basePosition == -666) {
            out.results.push_back(0);
            out.isOut.push_back(true);
            continue;
        }

        const int targetPosition = basePosition + mod_.delta;
        out.results.push_back(getNoteFromPosition(targetPosition, *vec, mod));
        out.isOut.push_back(false);
    }

    return out;
}

std::vector<VectorModification> parseModifications(
    const std::vector<int>&                         deltas,
    std::vector<VectorModification::Type>           types)
{
    if (types.size() == 1) {
        const VectorModification::Type broadcast = types[0];
        types.assign(deltas.size(), broadcast);
    }

    if (deltas.size() != types.size())
        throw std::runtime_error("Deltas and types must have the same length.");

    std::vector<VectorModification> mods(deltas.size());
    for (size_t i = 0; i < deltas.size(); ++i)
        mods[i] = { types[i], deltas[i] };

    return mods;
}

std::vector<int> ornamentLoop(const std::vector<int>& in, int length) {
    if (length <= 0) return {};
    std::vector<int> result;
    const int n = static_cast<int>(in.size());

    while (static_cast<int>(result.size()) < length) {
        for (int i = n - 1; i >= 0 && static_cast<int>(result.size()) < length; --i)
            result.insert(result.begin(), in[i]);
    }

    if (static_cast<int>(result.size()) > length)
        result.erase(result.begin(), result.begin() + (result.size() - length));

    return result;
}

std::vector<VectorModification::Type> ornamentTypesLoop(
    const std::vector<VectorModification::Type>& types,
    int length)
{
    if (length <= 0) return {};
    std::vector<VectorModification::Type> result;
    const int n = static_cast<int>(types.size());

    while (static_cast<int>(result.size()) < length) {
        for (int i = n - 1; i >= 0 && static_cast<int>(result.size()) < length; --i)
            result.insert(result.begin(), types[i]);
    }

    if (static_cast<int>(result.size()) > length)
        result.erase(result.begin(), result.begin() + (result.size() - length));

    return result;
}

std::string join(const std::vector<int>& v) {
    std::ostringstream oss;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) oss << ", ";
        oss << v[i];
    }
    return oss.str();
}

void applyTripleSelect(
    const std::vector<int>&                notes,
    const std::vector<int>&                chord,
    const std::vector<int>&                scale,
    const std::vector<int>&                chromatic,
    int                                    mod,
    const std::vector<int>&                ornaments,
    const std::vector<VectorModification::Type>& types)
{
    const auto expandedTypes = ornamentTypesLoop(types, static_cast<int>(ornaments.size()));
    const auto modifications = parseModifications(ornaments, expandedTypes);

    for (int note : notes) {
        const Analysis analysis = hierarchy(note, chord, scale, chromatic, mod);
        const TripleSelectResult res = tripleSelect(analysis, modifications, chord, scale, chromatic, mod);
        const std::vector<int>&  selectedNotes = res.results;

        std::cout << "\nMIDI note " << note << ":\n";
        std::cout << "Selected notes: [" << join(selectedNotes) << "]\n";
    }
}

} // namespace musicpp

#endif // MUSICPP_MELODY_H
