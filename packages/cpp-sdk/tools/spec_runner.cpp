#include "../src/automations.h"
#include "../src/chordNames.h"
#include "../src/measures.h"
#include "../src/melody.h"
#include "../src/noteNames.h"
#include "../src/quantizeTranspose.h"
#include "../src/rhythmGen.h"
#include "../src/scale.h"
#include "../src/scaleDictionary.h"
#include "../src/slonimsky.h"
#include "../src/Vector.h"

using Args = map<string, string>;

static string jsonEscape(const string& input) {
    string out;
    out.reserve(input.size() + 8);
    for (char ch : input) {
        switch (ch) {
            case '\\':
                out += "\\\\";
                break;
            case '"':
                out += "\\\"";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\t':
                out += "\\t";
                break;
            default:
                out += ch;
                break;
        }
    }
    return out;
}

static vector<int> parseIntList(const string& value) {
    vector<int> result;
    if (value.empty()) {
        return result;
    }

    istringstream stream(value);
    string token;
    while (getline(stream, token, ',')) {
        if (!token.empty()) {
            result.emplace_back(stoi(token));
        }
    }
    return result;
}

static vector<vector<int>> parseIntMatrix(const string& value) {
    vector<vector<int>> result;
    if (value.empty()) {
        return result;
    }

    istringstream stream(value);
    string token;
    while (getline(stream, token, ';')) {
        result.emplace_back(parseIntList(token));
    }
    return result;
}

static vector<string> parseStringList(const string& value) {
    vector<string> result;
    if (value.empty()) {
        return result;
    }

    istringstream stream(value);
    string token;
    while (getline(stream, token, ',')) {
        if (!token.empty()) {
            result.emplace_back(token);
        }
    }
    return result;
}

static int parseInt(const Args& args, const string& key, int fallback = 0) {
    auto it = args.find(key);
    return it == args.end() ? fallback : stoi(it->second);
}

static bool parseBool(const Args& args, const string& key, bool fallback = false) {
    auto it = args.find(key);
    if (it == args.end()) {
        return fallback;
    }

    const string& value = it->second;
    return value == "1" || value == "true" || value == "yes" || value == "on";
}

static vector<int> parseOptionalIntList(const Args& args, const string& key) {
    auto it = args.find(key);
    return it == args.end() ? vector<int>() : parseIntList(it->second);
}

static string requireArg(const Args& args, const string& key) {
    auto it = args.find(key);
    if (it == args.end()) {
        throw invalid_argument("Missing required argument: --" + key);
    }
    return it->second;
}

static void writeIntVector(ostream& out, const vector<int>& data) {
    out << "[";
    for (size_t i = 0; i < data.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        out << data[i];
    }
    out << "]";
}

static void writeStringVector(ostream& out, const vector<string>& data) {
    out << "[";
    for (size_t i = 0; i < data.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        out << "\"" << jsonEscape(data[i]) << "\"";
    }
    out << "]";
}

static void writeBoolVector(ostream& out, const vector<bool>& data) {
    out << "[";
    for (size_t i = 0; i < data.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        out << (data[i] ? "true" : "false");
    }
    out << "]";
}

static void writeDoubleVector(ostream& out, const vector<double>& data) {
    out << "[";
    for (size_t i = 0; i < data.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        out << setprecision(15) << data[i];
    }
    out << "]";
}

static void writeOccurrences(ostream& out, const map<int, int>& data) {
    out << "[";
    size_t index = 0;
    for (const auto& entry : data) {
        if (index > 0) {
            out << ",";
        }
        out << "{\"distance\":" << entry.first << ",\"count\":" << entry.second << "}";
        ++index;
    }
    out << "]";
}

static void writeDistributionSpectra(ostream& out, const vector<set<int>>& spectra) {
    out << "[";
    for (size_t i = 0; i < spectra.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        out << "[";
        size_t innerIndex = 0;
        for (int value : spectra[i]) {
            if (innerIndex > 0) {
                out << ",";
            }
            out << value;
            ++innerIndex;
        }
        out << "]";
    }
    out << "]";
}

static void writeIntervalVector(ostream& out, const IntervalVector& value);
static void writeBinaryVector(ostream& out, const BinaryVector& value);

static void writePositionVector(ostream& out, const PositionVector& value) {
    out << "{";
    out << "\"data\":";
    writeIntVector(out, value.getData());
    out << ",\"mod\":" << value.getMod();
    out << ",\"userRange\":" << value.getUserRange();
    out << ",\"range\":" << value.getRange();
    out << ",\"rangeUpdate\":" << (value.getRangeUpdate() ? "true" : "false");
    out << ",\"user\":" << (value.getUser() ? "true" : "false");
    out << "}";
}

static void writePositionVectorList(ostream& out, const vector<PositionVector>& values) {
    out << "[";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        writePositionVector(out, values[i]);
    }
    out << "]";
}

static void writeIntervalVectorList(ostream& out, const vector<IntervalVector>& values) {
    out << "[";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        writeIntervalVector(out, values[i]);
    }
    out << "]";
}

static void writeBinaryVectorList(ostream& out, const vector<BinaryVector>& values) {
    out << "[";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        writeBinaryVector(out, values[i]);
    }
    out << "]";
}

static void writeIntervalVector(ostream& out, const IntervalVector& value) {
    out << "{";
    out << "\"data\":";
    writeIntVector(out, value.getData());
    out << ",\"offset\":" << value.getOffset();
    out << ",\"mod\":" << value.getMod();
    out << "}";
}

static void writeBinaryVector(ostream& out, const BinaryVector& value) {
    out << "{";
    out << "\"data\":";
    writeIntVector(out, value.getData());
    out << ",\"offset\":" << value.getOffset();
    out << ",\"mod\":" << value.getMod();
    out << "}";
}

template <typename T>
static void writeModalMatrix(ostream& out, const ModalMatrix<T>& matrix, void (*writer)(ostream&, const T&)) {
    out << "[";
    for (size_t i = 0; i < matrix.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        out << "{\"index\":" << matrix[i].second << ",\"vector\":";
        writer(out, matrix[i].first);
        out << "}";
    }
    out << "]";
}

template <typename T>
static void writeModalSelectionMatrix(ostream& out, const ModalSelectionMatrix<T>& matrix, void (*writer)(ostream&, const T&)) {
    out << "[";
    for (size_t i = 0; i < matrix.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        out << "{\"modeIndex\":" << matrix[i].second << ",\"vector\":";
        writer(out, matrix[i].first);
        out << "}";
    }
    out << "]";
}

static void writeTranspositionMatrix(ostream& out, const TranspositionMatrix& matrix) {
    out << "[";
    for (size_t i = 0; i < matrix.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        out << "{\"transposition\":" << matrix[i].second << ",\"vector\":";
        writePositionVector(out, matrix[i].first);
        out << "}";
    }
    out << "]";
}

static void writeRototranslationMatrix(ostream& out, const RototranslationMatrix& matrix) {
    out << "{";
    out << "\"center\":" << matrix.getCenter();
    out << ",\"rows\":[";
    for (size_t i = 0; i < matrix.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        out << "{\"translation\":" << matrix[i].second << ",\"vector\":";
        writePositionVector(out, matrix[i].first);
        out << "}";
    }
    out << "]}";
}

static void writeModalRototranslationMatrix(ostream& out, const ModalRototranslationMatrix<PositionVector>& matrix) {
    out << "[";
    for (size_t i = 0; i < matrix.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        out << "{\"modeIndex\":" << matrix[i].second << ",\"matrix\":";
        writeRototranslationMatrix(out, matrix[i].first);
        out << "}";
    }
    out << "]";
}

static void writeTranspositionRow(ostream& out, const TranspositionMatrixRow& row) {
    out << "{";
    out << "\"transposition\":" << row.getTransposition();
    out << ",\"distance\":" << row.getDistance();
    out << ",\"vector\":";
    writePositionVector(out, row.getVector());
    out << "}";
}

static void writeRototranslationRow(ostream& out, const RototranslationMatrixRow& row) {
    out << "{";
    out << "\"translation\":" << row.getTranslation();
    out << ",\"center\":" << row.getCenter();
    out << ",\"distance\":" << row.getDistance();
    out << ",\"vector\":";
    writePositionVector(out, row.getVector());
    out << "}";
}

template <typename T>
static void writeModalMatrixRow(ostream& out, const ModalMatrixRow<T>& row, void (*writer)(ostream&, const T&)) {
    out << "{";
    out << "\"index\":" << row.getIndex();
    out << ",\"distance\":" << row.getDistance();
    out << ",\"vector\":";
    writer(out, row.getVector());
    out << "}";
}

template <typename T>
static void writeModalSelectionRow(ostream& out, const ModalSelectionMatrixRow<T>& row, void (*writer)(ostream&, const T&)) {
    out << "{";
    out << "\"modeIndex\":" << row.getModeIndex();
    out << ",\"distance\":" << row.getDistance();
    out << ",\"vector\":";
    writer(out, row.getChord());
    out << "}";
}

static void writeModalRototranslationRow(ostream& out, const ModalRototranslationMatrixRow& row) {
    out << "{";
    out << "\"modeIndex\":" << row.getModeIndex();
    out << ",\"translationIndex\":" << row.getTranslationIndex();
    out << ",\"distance\":" << row.getDistance();
    out << ",\"vector\":";
    writePositionVector(out, row.getVector());
    out << "}";
}

static PositionVector parsePositionVector(const Args& args, const string& prefix, int defaultMod = 12) {
    const vector<int> data = parseIntList(requireArg(args, prefix + "-data"));
    return PositionVector(
        data,
        parseInt(args, prefix + "-mod", defaultMod),
        parseInt(args, prefix + "-user-range", 0),
        parseBool(args, prefix + "-range-update", true),
        parseBool(args, prefix + "-user", false));
}

static IntervalVector parseIntervalVector(const Args& args, const string& prefix, int defaultMod = 12) {
    return IntervalVector(
        parseIntList(requireArg(args, prefix + "-data")),
        parseInt(args, prefix + "-offset", 0),
        parseInt(args, prefix + "-mod", defaultMod));
}

static BinaryVector parseBinaryVector(const Args& args, const string& prefix, int defaultMod = 4) {
    return BinaryVector(
        parseIntList(requireArg(args, prefix + "-data")),
        parseInt(args, prefix + "-offset", 0),
        parseInt(args, prefix + "-mod", defaultMod));
}

static vector<PositionVector> parsePositionVectorSequence(const Args& args, const string& prefix, int defaultMod = 12) {
    const vector<vector<int>> data = parseIntMatrix(requireArg(args, prefix + "-data"));
    vector<PositionVector> result;
    result.reserve(data.size());

    for (const vector<int>& row : data) {
        result.emplace_back(
            row,
            parseInt(args, prefix + "-mod", defaultMod),
            parseInt(args, prefix + "-user-range", 0),
            parseBool(args, prefix + "-range-update", true),
            parseBool(args, prefix + "-user", false));
    }

    return result;
}

static void writeVectors(ostream& out, const Vectors& value) {
    out << "{";
    out << "\"positions\":";
    writePositionVector(out, value.getPositions());
    out << ",\"intervals\":";
    writeIntervalVector(out, value.getIntervals());
    out << ",\"binary\":";
    writeBinaryVector(out, value.getBinary());
    out << ",\"mod\":" << value.getMod();
    out << "}";
}

static void writeScale(ostream& out, const Scale& scale) {
    out << "{";
    out << "\"positions\":";
    writePositionVector(out, scale.toPositions());
    out << ",\"intervals\":";
    writeIntervalVector(out, scale.toIntervals());
    out << ",\"params\":{";
    out << "\"root\":" << scale.getRoot();
    out << ",\"mode\":" << scale.getMode();
    out << ",\"degree\":" << scale.getDegree();
    out << ",\"invert\":" << (scale.getInvert() ? "true" : "false");
    out << ",\"inversionAxis\":" << scale.getInversionAxis();
    out << ",\"mirror\":" << (scale.getMirror() ? "true" : "false");
    out << ",\"mirrorAxis\":" << scale.getMirrorAxis();
    out << "}}";
}

static void writeChord(ostream& out, const Chord& chordValue) {
    out << "{";
    out << "\"positions\":";
    writePositionVector(out, chordValue.toPositions());
    out << ",\"intervals\":";
    writeIntervalVector(out, chordValue.toIntervals());
    out << "}";
}

static void writeNoteResult(ostream& out, const NoteResult& result) {
    out << "{";
    out << "\"noteNames\":";
    writeStringVector(out, result.noteNames);
    out << ",\"centsInfo\":";
    writeStringVector(out, result.centsInfo);
    out << "}";
}

static void writeChordAnalysis(ostream& out, const ChordAnalysis& analysis) {
    out << "{";
    out << "\"root\":" << analysis.root;
    out << ",\"chordName\":\"" << jsonEscape(analysis.chordName) << "\"";
    out << ",\"hasThird\":" << (analysis.hasThird ? "true" : "false");
    out << ",\"hasMajorThird\":" << (analysis.hasMajorThird ? "true" : "false");
    out << ",\"hasMinorThird\":" << (analysis.hasMinorThird ? "true" : "false");
    out << ",\"hasFifth\":" << (analysis.hasFifth ? "true" : "false");
    out << ",\"hasPerfectFifth\":" << (analysis.hasPerfectFifth ? "true" : "false");
    out << ",\"hasAugmentedFifth\":" << (analysis.hasAugmentedFifth ? "true" : "false");
    out << ",\"hasDiminishedFifth\":" << (analysis.hasDiminishedFifth ? "true" : "false");
    out << ",\"hasCompleteTriad\":" << (analysis.hasCompleteTriad ? "true" : "false");
    out << ",\"hasSeventh\":" << (analysis.hasSeventh ? "true" : "false");
    out << ",\"hasMinorSeventh\":" << (analysis.hasMinorSeventh ? "true" : "false");
    out << ",\"hasMajorSeventh\":" << (analysis.hasMajorSeventh ? "true" : "false");
    out << ",\"hasDiminishedSeventh\":" << (analysis.hasDiminishedSeventh ? "true" : "false");
    out << ",\"addedNotes\":[";
    for (size_t i = 0; i < analysis.addedNotes.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        out << "{\"interval\":" << analysis.addedNotes[i].first
            << ",\"label\":\"" << jsonEscape(analysis.addedNotes[i].second) << "\"}";
    }
    out << "]";
    out << "}";
}

template <typename Match>
static void writeScaleMatches(ostream& out, const vector<Match>& matches) {
    out << "[";
    for (size_t i = 0; i < matches.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        out << "{\"sheetName\":\"" << jsonEscape(matches[i].sheetName) << "\"";
        out << ",\"scaleName\":\"" << jsonEscape(matches[i].scaleName) << "\"";
        out << ",\"intervals\":";
        writeIntVector(out, matches[i].intervals);
        out << "}";
    }
    out << "]";
}

static void writeNoteInfo(ostream& out, const NoteInfo& info) {
    out << "{";
    out << "\"position\":" << info.position;
    out << ",\"degree\":" << info.degree;
    out << ",\"octave\":" << info.octave;
    out << ",\"noteValue\":" << info.noteValue;
    out << "}";
}

static void writeMelodyAnalysis(ostream& out, const Analysis& analysis) {
    out << "{";
    out << "\"chord\":";
    writeNoteInfo(out, analysis.chord);
    out << ",\"scale\":";
    writeNoteInfo(out, analysis.scale);
    out << ",\"chromatic\":";
    writeNoteInfo(out, analysis.chromatic);
    out << "}";
}

static void writeTripleSelectResult(ostream& out, const TripleSelectResult& result) {
    out << "{";
    out << "\"results\":";
    writeIntVector(out, result.results);
    out << ",\"isOut\":";
    writeBoolVector(out, result.isOut);
    out << "}";
}

static void writeQuantizeTransposeResult(ostream& out, const pair<PositionVector, PositionVector>& result) {
    out << "{";
    out << "\"degrees\":";
    writePositionVector(out, result.first);
    out << ",\"notes\":";
    writePositionVector(out, result.second);
    out << "}";
}

static vector<VectorModification::Type> parseModificationTypes(const string& value) {
    vector<VectorModification::Type> result;
    for (const string& token : parseStringList(value)) {
        if (token == "chord") {
            result.emplace_back(VectorModification::Type::CHORD);
        } else if (token == "scale") {
            result.emplace_back(VectorModification::Type::SCALE);
        } else if (token == "chromatic") {
            result.emplace_back(VectorModification::Type::CHROMATIC);
        } else {
            throw invalid_argument("Unsupported modification type: " + token);
        }
    }
    return result;
}

static void writeMeasureSummary(ostream& out, PositionVector value) {
    vector<int> positionData = value.getData();
    vector<int> intervalData = positionsToIntervals(value).getData();
    vector<int> differenceData = differences(positionData);
    vector<int> geodesicData = geodesicDistances(value);
    map<int, int> occurrences = calculateOccurrences(geodesicData);
    vector<set<int>> spectra = calculateDistributionSpectra(value);
    vector<int> widths = calculateSpectrumWidths(spectra);
    vector<int> rotationalAxes = findRotationalSymmetryAxes(value);
    vector<double> reflectiveAxes = findReflectiveSymmetryAxes(value);
    BinaryVector onsets = positionsToBinary(value);
    pair<bool, int> generated = isGenerated(positionData, value.getRange());
    const int analysisModulo = value.getRange();
    const string aksakClassification = isPrime(analysisModulo)
        ? "authentic aksak"
        : (analysisModulo % 2 != 0 ? "quasi-aksak" : "pseudo-aksak");

    out << "{";
    out << "\"positions\":";
    writePositionVector(out, value);
    out << ",\"intervals\":";
    writeIntVector(out, intervalData);
    out << ",\"onsets\":";
    writeBinaryVector(out, onsets);
    out << ",\"differences\":";
    writeIntVector(out, differenceData);
    out << ",\"geodesicDistances\":";
    writeIntVector(out, geodesicData);
    out << ",\"occurrences\":";
    writeOccurrences(out, occurrences);
    out << ",\"winogradDeep\":" << (isWinogradDeep(occurrences, static_cast<int>(value.size())) ? "true" : "false");
    out << ",\"erdosDeep\":" << (isErdosDeep(occurrences) ? "true" : "false");
    out << ",\"regressionEvenness\":" << setprecision(15) << calculateRegressionEvenness(positionData, analysisModulo);
    out << ",\"rhythmicOddity\":" << calculateRhythmicOddity(value);
    out << ",\"transitionComplexity\":" << computeTransitionComplexity(value, analysisModulo);
    out << ",\"entropy\":" << setprecision(15) << computeEntropy(value);
    out << ",\"longestSubsequence\":" << computeLongestSubsequence(value);
    out << ",\"isEuclidean\":" << (isEuclidean(value, analysisModulo) ? "true" : "false");
    out << ",\"distributionSpectra\":";
    writeDistributionSpectra(out, spectra);
    out << ",\"spectrumWidths\":";
    writeIntVector(out, widths);
    out << ",\"spectrumVariation\":" << setprecision(15) << calculateSpectrumVariation(widths, static_cast<int>(value.size()));
    out << ",\"rotationalSymmetryAxes\":";
    writeIntVector(out, rotationalAxes);
    out << ",\"reflectiveSymmetryAxes\":";
    writeDoubleVector(out, reflectiveAxes);
    out << ",\"modulo\":" << analysisModulo;
    out << ",\"aksakClassification\":\"" << aksakClassification << "\"";
    out << ",\"palindrome\":" << (isPalindrome(value) ? "true" : "false");
    out << ",\"chiral\":" << (isChiral(value) ? "true" : "false");
    out << ",\"balanced\":" << (isBalanced(value) ? "true" : "false");
    out << ",\"generated\":{\"found\":" << (generated.first ? "true" : "false") << ",\"multiplier\":" << generated.second << "}";
    out << "}";
}

static Vectors parseVectors(const Args& args, const string& prefix) {
    const string kind = requireArg(args, prefix + "-kind");

    if (kind == "position") {
        return Vectors(parsePositionVector(args, prefix));
    }
    if (kind == "interval") {
        return Vectors(parseIntervalVector(args, prefix));
    }
    if (kind == "binary") {
        return Vectors(parseBinaryVector(args, prefix));
    }

    throw invalid_argument("Unsupported vectors kind: " + kind);
}

static Args parseArgs(int argc, char** argv, int startIndex = 2) {
    Args args;
    for (int i = startIndex; i < argc; ++i) {
        string token = argv[i];
        if (token.rfind("--", 0) != 0) {
            throw invalid_argument("Unexpected argument: " + token);
        }

        string key = token.substr(2);
        string value = "true";
        if (i + 1 < argc) {
            string next = argv[i + 1];
            if (next.rfind("--", 0) != 0) {
                value = next;
                ++i;
            }
        }
        args[key] = value;
    }
    return args;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: musicplusplus-spec <command> [--key value]" << endl;
        return 1;
    }

    try {
        const string command = argv[1];
        const Args args = parseArgs(argc, argv);

        if (command == "positions-to-intervals") {
            PositionVector value = parsePositionVector(args, "value");
            writeIntervalVector(cout, positionsToIntervals(value));
        } else if (command == "intervals-to-positions") {
            IntervalVector value = parseIntervalVector(args, "value");
            writePositionVector(cout, intervalsToPositions(value));
        } else if (command == "positions-to-binary") {
            PositionVector value = parsePositionVector(args, "value");
            writeBinaryVector(cout, positionsToBinary(value));
        } else if (command == "align") {
            PositionVector reference = parsePositionVector(args, "reference");
            PositionVector target = parsePositionVector(args, "target");
            cout << align(reference, target);
        } else if (command == "position-op") {
            PositionVector value = parsePositionVector(args, "value");
            const string op = requireArg(args, "op");

            if (op == "identity") {
                writePositionVector(cout, value);
            } else if (op == "element") {
                cout << value[parseInt(args, "index")];
            } else if (op == "add-scalar") {
                writePositionVector(cout, value + parseInt(args, "scalar"));
            } else if (op == "subtract-scalar") {
                writePositionVector(cout, value - parseInt(args, "scalar"));
            } else if (op == "multiply-scalar") {
                writePositionVector(cout, value * parseInt(args, "scalar"));
            } else if (op == "divide-scalar") {
                writePositionVector(cout, value / parseInt(args, "scalar"));
            } else if (op == "modulo-scalar") {
                writePositionVector(cout, value % parseInt(args, "scalar"));
            } else if (op == "add-vector") {
                writePositionVector(cout, value.componentwiseSum(parseIntList(requireArg(args, "other")), parseBool(args, "loop", false)));
            } else if (op == "subtract-vector") {
                writePositionVector(cout, value.componentwiseSubtraction(parseIntList(requireArg(args, "other")), parseBool(args, "loop", false)));
            } else if (op == "multiply-vector") {
                writePositionVector(cout, value.componentwiseProduct(parseIntList(requireArg(args, "other")), parseBool(args, "loop", true)));
            } else if (op == "divide-vector") {
                writePositionVector(cout, value.componentwiseDivision(parseIntList(requireArg(args, "other")), parseBool(args, "loop", true)));
            } else if (op == "modulo-vector") {
                writePositionVector(cout, value.componentwiseModulo(parseIntList(requireArg(args, "other")), parseBool(args, "loop", true)));
            } else if (op == "rotate") {
                writePositionVector(cout, value.rotate(parseInt(args, "amount")));
            } else if (op == "roto-translate") {
                writePositionVector(cout, value.rotoTranslate(parseInt(args, "amount"), parseInt(args, "length", 0)));
            } else if (op == "complement") {
                writePositionVector(cout, value.complement());
            } else if (op == "inversion") {
                writePositionVector(cout, value.inversion(parseInt(args, "axis"), parseBool(args, "sort", true)));
            } else if (op == "negative") {
                writePositionVector(cout, value.negative(parseInt(args, "axis", 10), parseBool(args, "standard", true), parseBool(args, "sort", true)));
            } else if (op == "concatenate") {
                writePositionVector(cout, value.concatenate(parsePositionVector(args, "other")));
            } else if (op == "repeat") {
                writePositionVector(cout, value.repeat(parseInt(args, "times")));
            } else if (op == "resize") {
                writePositionVector(cout, value.resize(parseInt(args, "start"), parseInt(args, "end")));
            } else {
                throw invalid_argument("Unsupported position op: " + op);
            }
        } else if (command == "position-adapt-lcm") {
            const vector<PositionVector> values = {
                parsePositionVector(args, "left"),
                parsePositionVector(args, "right")
            };
            writePositionVectorList(cout, PositionVector::adaptToLCM(values));
        } else if (command == "interval-op") {
            IntervalVector value = parseIntervalVector(args, "value");
            const string op = requireArg(args, "op");

            if (op == "identity") {
                writeIntervalVector(cout, value);
            } else if (op == "element") {
                cout << value[parseInt(args, "index")];
            } else if (op == "add-scalar") {
                writeIntervalVector(cout, value + parseInt(args, "scalar"));
            } else if (op == "subtract-scalar") {
                writeIntervalVector(cout, value - parseInt(args, "scalar"));
            } else if (op == "multiply-scalar") {
                writeIntervalVector(cout, value * parseInt(args, "scalar"));
            } else if (op == "divide-scalar") {
                writeIntervalVector(cout, value / parseInt(args, "scalar"));
            } else if (op == "modulo-scalar") {
                writeIntervalVector(cout, value % parseInt(args, "scalar"));
            } else if (op == "add-vector") {
                writeIntervalVector(cout, value.componentwiseSum(parseIntList(requireArg(args, "other")), parseBool(args, "loop", false)));
            } else if (op == "subtract-vector") {
                writeIntervalVector(cout, value.componentwiseSubtraction(parseIntList(requireArg(args, "other")), parseBool(args, "loop", false)));
            } else if (op == "multiply-vector") {
                writeIntervalVector(cout, value.componentwiseProduct(parseIntList(requireArg(args, "other")), parseBool(args, "loop", true)));
            } else if (op == "divide-vector") {
                writeIntervalVector(cout, value.componentwiseDivision(parseIntList(requireArg(args, "other")), parseBool(args, "loop", true)));
            } else if (op == "modulo-vector") {
                writeIntervalVector(cout, value.componentwiseModulo(parseIntList(requireArg(args, "other")), parseBool(args, "loop", true)));
            } else if (op == "rotate") {
                writeIntervalVector(cout, value.rotate(parseInt(args, "amount"), parseInt(args, "length", 0)));
            } else if (op == "roto-translate") {
                writeIntervalVector(cout, value.rotoTranslate(parseInt(args, "amount"), parseInt(args, "length", 0)));
            } else if (op == "reverse") {
                writeIntervalVector(cout, value.reverse());
            } else if (op == "retrograde") {
                writeIntervalVector(cout, value.retrograde());
            } else if (op == "inversion") {
                writeIntervalVector(cout, value.inversion(parseInt(args, "axis", 0)));
            } else if (op == "negate") {
                writeIntervalVector(cout, value.negate());
            } else if (op == "normalize") {
                writeIntervalVector(cout, value.normalize(parseInt(args, "modulo", 0)));
            } else if (op == "concatenate") {
                writeIntervalVector(cout, value.concatenate(parseIntervalVector(args, "other")));
            } else if (op == "repeat") {
                writeIntervalVector(cout, value.repeat(parseInt(args, "times")));
            } else if (op == "single-mirror") {
                writeIntervalVector(cout, value.singleMirror(parseInt(args, "position"), parseBool(args, "left", false)));
            } else if (op == "double-mirror") {
                writeIntervalVector(cout, value.doubleMirror(parseInt(args, "position")));
            } else if (op == "cross-mirror") {
                writeIntervalVector(cout, value.crossMirror(parseInt(args, "position"), parseBool(args, "left", false)));
            } else {
                throw invalid_argument("Unsupported interval op: " + op);
            }
        } else if (command == "interval-adapt-lcm") {
            const vector<IntervalVector> values = {
                parseIntervalVector(args, "left"),
                parseIntervalVector(args, "right")
            };
            writeIntervalVectorList(cout, IntervalVector::adaptToLCM(values));
        } else if (command == "binary-op") {
            BinaryVector value = parseBinaryVector(args, "value");
            const string op = requireArg(args, "op");

            if (op == "identity") {
                writeBinaryVector(cout, value);
            } else if (op == "element") {
                cout << value[parseInt(args, "index")];
            } else if (op == "multiply-scalar") {
                writeBinaryVector(cout, value * parseInt(args, "scalar"));
            } else if (op == "divide-scalar") {
                writeBinaryVector(cout, value / parseInt(args, "scalar"));
            } else if (op == "rotate") {
                writeBinaryVector(cout, value.rotate(parseInt(args, "amount")));
            } else if (op == "complement") {
                writeBinaryVector(cout, value.complement());
            } else if (op == "inversion") {
                writeBinaryVector(cout, value.inversion(parseInt(args, "axis")));
            } else if (op == "transpose") {
                writeBinaryVector(cout, value.transpose(parseInt(args, "amount")));
            } else if (op == "concatenate") {
                writeBinaryVector(cout, value.concatenate(parseBinaryVector(args, "other")));
            } else if (op == "repeat") {
                writeBinaryVector(cout, value.repeat(parseInt(args, "times")));
            } else if (op == "count-pulses") {
                cout << value.countPulses();
            } else if (op == "density") {
                cout << setprecision(15) << value.density();
            } else if (op == "pulse-indices") {
                writeIntVector(cout, value.getPulseIndices());
            } else if (op == "inter-onset-intervals") {
                writeIntVector(cout, value.getInterOnsetIntervals());
            } else if (op == "or") {
                writeBinaryVector(cout, value | parseBinaryVector(args, "other"));
            } else if (op == "and") {
                writeBinaryVector(cout, value & parseBinaryVector(args, "other"));
            } else if (op == "xor") {
                writeBinaryVector(cout, value ^ parseBinaryVector(args, "other"));
            } else if (op == "nor") {
                writeBinaryVector(cout, value.nor(parseBinaryVector(args, "other")));
            } else if (op == "nand") {
                writeBinaryVector(cout, value.nand(parseBinaryVector(args, "other")));
            } else if (op == "xnor") {
                writeBinaryVector(cout, value.xnor(parseBinaryVector(args, "other")));
            } else {
                throw invalid_argument("Unsupported binary op: " + op);
            }
        } else if (command == "binary-adapt-lcm") {
            const vector<BinaryVector> values = {
                parseBinaryVector(args, "left"),
                parseBinaryVector(args, "right")
            };
            writeBinaryVectorList(cout, BinaryVector::adaptToLCM(values));
        } else if (command == "vectors-factory") {
            const string kind = requireArg(args, "kind");

            if (kind == "positions") {
                writeVectors(cout, Vectors::fromPositions(parseIntList(requireArg(args, "data")), parseInt(args, "mod", 12)));
            } else if (kind == "intervals") {
                writeVectors(cout, Vectors::fromIntervals(parseIntList(requireArg(args, "data")), parseInt(args, "mod", 12)));
            } else if (kind == "binary") {
                writeVectors(cout, Vectors::fromBinary(parseIntList(requireArg(args, "data")), parseInt(args, "offset", 0), parseInt(args, "mod", 12)));
            } else if (kind == "euclidean") {
                writeVectors(cout, Vectors::euclidean(parseInt(args, "pulses"), parseInt(args, "steps")));
            } else {
                throw invalid_argument("Unsupported vectors factory kind: " + kind);
            }
        } else if (command == "vectors-op") {
            Vectors value = parseVectors(args, "value");
            const string op = requireArg(args, "op");

            if (op == "identity") {
                writeVectors(cout, value);
            } else if (op == "transpose") {
                writeVectors(cout, value.transpose(parseInt(args, "amount")));
            } else if (op == "multiply-positions") {
                writeVectors(cout, value.multiplyPositions(parseInt(args, "scalar")));
            } else if (op == "negative") {
                writeVectors(cout, value.negative(parseInt(args, "axis", 10)));
            } else if (op == "rotate-positions") {
                writeVectors(cout, value.rotatePositions(parseInt(args, "amount")));
            } else if (op == "rototranslate-positions") {
                writeVectors(cout, value.rototranslatePositions(parseInt(args, "amount"), parseInt(args, "length", 0)));
            } else if (op == "invert-positions") {
                writeVectors(cout, value.invertPositions(parseInt(args, "axis"), parseBool(args, "sort", true)));
            } else if (op == "complement-positions") {
                writeVectors(cout, value.complementPositions());
            } else if (op == "add-to-intervals") {
                writeVectors(cout, value.addToIntervals(parseInt(args, "amount")));
            } else if (op == "multiply-intervals") {
                writeVectors(cout, value.multiplyIntervals(parseInt(args, "scalar")));
            } else if (op == "rotate-intervals") {
                writeVectors(cout, value.rotateIntervals(parseInt(args, "amount")));
            } else if (op == "reverse-intervals") {
                writeVectors(cout, value.reverseIntervals());
            } else if (op == "invert-intervals") {
                writeVectors(cout, value.invertIntervals(parseInt(args, "axis")));
            } else if (op == "mode") {
                writeVectors(cout, value.mode(parseInt(args, "amount")));
            } else if (op == "rotate-binary") {
                writeVectors(cout, value.rotateBinary(parseInt(args, "amount")));
            } else if (op == "complement-binary") {
                writeVectors(cout, value.complementBinary());
            } else if (op == "multiply-binary") {
                writeVectors(cout, value.multiplyBinary(parseInt(args, "scalar")));
            } else if (op == "divide-binary") {
                writeVectors(cout, value.divideBinary(parseInt(args, "scalar")));
            } else if (op == "or") {
                writeVectors(cout, value | parseVectors(args, "other"));
            } else if (op == "and") {
                writeVectors(cout, value & parseVectors(args, "other"));
            } else if (op == "xor") {
                writeVectors(cout, value ^ parseVectors(args, "other"));
            } else {
                throw invalid_argument("Unsupported vectors op: " + op);
            }
        } else if (command == "select-pos-pos") {
            PositionVector source = parsePositionVector(args, "source");
            PositionVector criterion = parsePositionVector(args, "criterion", static_cast<int>(source.size()));
            writePositionVector(cout, select(source, criterion, parseInt(args, "rotation", 0), parseInt(args, "voices", 0)));
        } else if (command == "select-pos-int") {
            PositionVector source = parsePositionVector(args, "source");
            IntervalVector criterion = parseIntervalVector(args, "criterion", static_cast<int>(source.size()));
            writePositionVector(cout, select(source, criterion, parseInt(args, "rotation", 0), parseInt(args, "voices", 0)));
        } else if (command == "select-int-int") {
            IntervalVector source = parseIntervalVector(args, "source");
            IntervalVector criterion = parseIntervalVector(args, "criterion", static_cast<int>(source.size()));
            writeIntervalVector(cout, select(source, criterion, parseInt(args, "rotation", 0), parseInt(args, "voices", 0)));
        } else if (command == "select-int-pos") {
            IntervalVector source = parseIntervalVector(args, "source");
            PositionVector criterion = parsePositionVector(args, "criterion", static_cast<int>(source.size()));
            writeIntervalVector(cout, select(source, criterion, parseInt(args, "rotation", 0), parseInt(args, "voices", 0)));
        } else if (command == "scale-pos") {
            PositionVector generator = parsePositionVector(args, "generator");
            Scale scale(generator, parseInt(args, "root", 0), parseInt(args, "mode", 0), parseInt(args, "degree", 0),
                        parseBool(args, "invert", false), parseInt(args, "inversion-axis", 0),
                        parseBool(args, "mirror", false), parseInt(args, "mirror-axis", 0));
            writeScale(cout, scale);
        } else if (command == "scale-int") {
            IntervalVector generator = parseIntervalVector(args, "generator");
            Scale scale(generator, parseInt(args, "root", 0), parseInt(args, "mode", 0), parseInt(args, "degree", 0),
                        parseBool(args, "invert", false), parseInt(args, "inversion-axis", 0),
                        parseBool(args, "mirror", false), parseInt(args, "mirror-axis", 0));
            writeScale(cout, scale);
        } else if (command == "chord-pos-pos") {
            PositionVector scaleValue = parsePositionVector(args, "scale");
            PositionVector criterion = parsePositionVector(args, "criterion");
            Chord chordValue(scaleValue, criterion, ChordParams(
                parseInt(args, "shift", 0),
                parseInt(args, "rotation", 0),
                parseInt(args, "pre-voices", 0),
                parseInt(args, "position", 0),
                parseBool(args, "invert", false),
                parseInt(args, "axis", 0),
                parseBool(args, "negative-or-mirror", false),
                parseInt(args, "negative-or-mirror-pos", 0)));
            writeChord(cout, chordValue);
        } else if (command == "chord-pos-int") {
            PositionVector scaleValue = parsePositionVector(args, "scale");
            IntervalVector criterion = parseIntervalVector(args, "criterion");
            Chord chordValue(scaleValue, criterion, ChordParams(
                parseInt(args, "shift", 0),
                parseInt(args, "rotation", 0),
                parseInt(args, "pre-voices", 0),
                parseInt(args, "position", 0),
                parseBool(args, "invert", false),
                parseInt(args, "axis", 0),
                parseBool(args, "negative-or-mirror", false),
                parseInt(args, "negative-or-mirror-pos", 0)));
            writeChord(cout, chordValue);
        } else if (command == "chord-int-pos") {
            IntervalVector scaleValue = parseIntervalVector(args, "scale");
            PositionVector criterion = parsePositionVector(args, "criterion");
            Chord chordValue(scaleValue, criterion, ChordParams(
                parseInt(args, "shift", 0),
                parseInt(args, "rotation", 0),
                parseInt(args, "pre-voices", 0),
                parseInt(args, "position", 0),
                parseBool(args, "invert", false),
                parseInt(args, "axis", 0),
                parseBool(args, "negative-or-mirror", false),
                parseInt(args, "negative-or-mirror-pos", 0)));
            writeChord(cout, chordValue);
        } else if (command == "chord-int-int") {
            IntervalVector scaleValue = parseIntervalVector(args, "scale");
            IntervalVector criterion = parseIntervalVector(args, "criterion");
            Chord chordValue(scaleValue, criterion, ChordParams(
                parseInt(args, "shift", 0),
                parseInt(args, "rotation", 0),
                parseInt(args, "pre-voices", 0),
                parseInt(args, "position", 0),
                parseBool(args, "invert", false),
                parseInt(args, "axis", 0),
                parseBool(args, "negative-or-mirror", false),
                parseInt(args, "negative-or-mirror-pos", 0)));
            writeChord(cout, chordValue);
        } else if (command == "distance") {
            const string shape = requireArg(args, "shape");
            const string kind = requireArg(args, "kind");

            if (shape == "vector") {
                vector<int> left = parseIntList(requireArg(args, "left"));
                vector<int> right = parseIntList(requireArg(args, "right"));
                if (kind == "euclidean") {
                    cout << euclideanDistance(left, right);
                } else if (kind == "manhattan") {
                    cout << manhattanDistance(left, right);
                } else if (kind == "edit") {
                    cout << editDistance(left, right);
                } else if (kind == "hamming") {
                    cout << hammingDistance(left, right);
                } else if (kind == "difference") {
                    cout << difference(left, right);
                } else if (kind == "variation") {
                    cout << variationDistance(left, right);
                } else if (kind == "weightedTransformation") {
                    cout << weightedTransformationDistance(left, right);
                } else {
                    throw invalid_argument("Unsupported vector distance kind: " + kind);
                }
            } else if (shape == "position") {
                PositionVector left = parsePositionVector(args, "left");
                PositionVector right = parsePositionVector(args, "right");
                if (kind == "euclidean") {
                    cout << euclideanDistance(left, right);
                } else if (kind == "manhattan") {
                    cout << manhattanDistance(left, right);
                } else if (kind == "edit") {
                    cout << editDistance(left, right);
                } else if (kind == "hamming") {
                    cout << hammingDistance(left, right);
                } else if (kind == "difference") {
                    cout << difference(left, right);
                } else if (kind == "variation") {
                    cout << variationDistance(left, right);
                } else if (kind == "weightedTransformation") {
                    cout << weightedTransformationDistance(left, right);
                } else {
                    throw invalid_argument("Unsupported position distance kind: " + kind);
                }
            } else if (shape == "interval") {
                IntervalVector left = parseIntervalVector(args, "left");
                IntervalVector right = parseIntervalVector(args, "right");
                if (kind == "euclidean") {
                    cout << euclideanDistance(left, right);
                } else if (kind == "manhattan") {
                    cout << manhattanDistance(left, right);
                } else if (kind == "edit") {
                    cout << editDistance(left, right);
                } else if (kind == "hamming") {
                    cout << hammingDistance(left, right);
                } else if (kind == "difference") {
                    cout << difference(left, right);
                } else if (kind == "weightedTransformation") {
                    cout << weightedTransformationDistance(left, right);
                } else {
                    throw invalid_argument("Unsupported interval distance kind: " + kind);
                }
            } else {
                throw invalid_argument("Unsupported distance shape: " + shape);
            }
        } else if (command == "distribution") {
            const string op = requireArg(args, "op");
            if (op == "normalize") {
                vector<int> values = parseIntList(requireArg(args, "values"));
                writeDoubleVector(cout, normalize(values));
            } else if (op == "cdf") {
                vector<int> values = parseIntList(requireArg(args, "values"));
                vector<double> pdf = normalize(values);
                writeDoubleVector(cout, computeCDF(pdf));
            } else {
                throw invalid_argument("Unsupported distribution op: " + op);
            }
        } else if (command == "rhythm") {
            const string kind = requireArg(args, "kind");
            const int steps = parseInt(args, "steps", 0);
            const int events = parseInt(args, "events", 0);
            const int offset = parseInt(args, "offset", 0);

            if (kind == "euclidean-interval") {
                writeIntervalVector(cout, euclidean(steps, events, offset));
            } else if (kind == "euclidean-binary") {
                writeBinaryVector(cout, BinaryVector::euclidean(events, steps));
            } else if (kind == "clough-douthett") {
                writePositionVector(cout, CloughDouthettVector(steps, events, offset));
            } else if (kind == "deep-rhythm") {
                writePositionVector(cout, deepRhythm(steps, events, parseInt(args, "multiplicity", 0), offset));
            } else if (kind == "tihai-binary") {
                writeBinaryVector(cout, tihai(steps, parseInt(args, "repetitions", 0), parseBool(args, "pseudo", false), offset));
            } else {
                throw invalid_argument("Unsupported rhythm kind: " + kind);
            }
        } else if (command == "modal-matrix-pos") {
            PositionVector value = parsePositionVector(args, "value");
            writeModalMatrix(cout, modalMatrix(value), writePositionVector);
        } else if (command == "modal-matrix-int") {
            IntervalVector value = parseIntervalVector(args, "value");
            writeModalMatrix(cout, modalMatrix(value), writeIntervalVector);
        } else if (command == "transposition-matrix") {
            PositionVector value = parsePositionVector(args, "value");
            writeTranspositionMatrix(cout, transpositionMatrix(value));
        } else if (command == "rototranslation-matrix") {
            PositionVector value = parsePositionVector(args, "value");
            writeRototranslationMatrix(cout, rototranslationMatrix(value, parseInt(args, "center", 0)));
        } else if (command == "modal-selection-pos") {
            PositionVector source = parsePositionVector(args, "source");
            IntervalVector criterion = parseIntervalVector(args, "criterion");
            writeModalSelectionMatrix(cout, modalSelection(source, criterion, parseInt(args, "degree", 0)), writePositionVector);
        } else if (command == "modal-selection-int") {
            IntervalVector source = parseIntervalVector(args, "source");
            IntervalVector criterion = parseIntervalVector(args, "criterion");
            writeModalSelectionMatrix(cout, modalSelection(source, criterion, parseInt(args, "degree", 0)), writeIntervalVector);
        } else if (command == "modal-rototranslation") {
            PositionVector source = parsePositionVector(args, "source");
            IntervalVector criterion = parseIntervalVector(args, "criterion");
            auto selection = modalSelection(source, criterion, parseInt(args, "degree", 0));
            writeModalRototranslationMatrix(cout, modalRototranslation(selection));
        } else if (command == "automation-degree") {
            PositionVector scaleValue = parsePositionVector(args, "scale");
            IntervalVector criterion = parseIntervalVector(args, "criterion");
            PositionVector reference = parsePositionVector(args, "reference");
            writeModalRototranslationRow(cout, degreeAutomation(scaleValue, criterion, parseInt(args, "degree", 0), reference, parseInt(args, "complexity", 0)));
        } else if (command == "automation-voice-leading") {
            PositionVector reference = parsePositionVector(args, "reference");
            PositionVector target = parsePositionVector(args, "target");
            writeRototranslationRow(cout, voiceLeadingAutomation(reference, target, parseInt(args, "complexity", 0)));
        } else if (command == "automation-modal-interchange") {
            PositionVector scaleValue = parsePositionVector(args, "scale");
            const vector<int> notes = parseIntList(requireArg(args, "notes"));
            writeModalMatrixRow(cout, modalInterchangeAutomation(scaleValue, notes, parseInt(args, "complexity", 0)), writePositionVector);
        } else if (command == "automation-modulation") {
            PositionVector scaleValue = parsePositionVector(args, "scale");
            const vector<int> notes = parseIntList(requireArg(args, "notes"));
            writeTranspositionRow(cout, modulationAutomation(scaleValue, notes, parseInt(args, "complexity", 0)));
        } else if (command == "automation-auto-scale") {
            PositionVector scaleValue = parsePositionVector(args, "scale");
            vector<int> notes = parseIntList(requireArg(args, "notes"));
            writePositionVector(cout, autoScale(scaleValue, notes));
        } else if (command == "automation-voice-leading-vector-reference") {
            vector<PositionVector> targets = parsePositionVectorSequence(args, "targets");
            vector<PositionVector> references = parsePositionVectorSequence(args, "references");
            vector<int> complexities = parseOptionalIntList(args, "complexities");
            writePositionVectorList(cout, voiceLeadingAutomationVectorReference(targets, references, complexities));
        } else if (command == "automation-voice-leading-reference-seq") {
            vector<PositionVector> targets = parsePositionVectorSequence(args, "targets");
            PositionVector reference = parsePositionVector(args, "reference");
            vector<int> complexities = parseOptionalIntList(args, "complexities");
            writePositionVectorList(cout, voiceLeadingAutomationReference(targets, reference, complexities));
        } else if (command == "automation-voice-leading-forward-seq") {
            vector<PositionVector> targets = parsePositionVectorSequence(args, "targets");
            vector<int> complexities = parseOptionalIntList(args, "complexities");
            writePositionVectorList(cout, forwardVoiceLeading(targets, complexities));
        } else if (command == "automation-voice-leading-backward-seq") {
            vector<PositionVector> targets = parsePositionVectorSequence(args, "targets");
            vector<int> complexities = parseOptionalIntList(args, "complexities");
            writePositionVectorList(cout, voiceLeadingAutomationSequentialBackward(targets, complexities));
        } else if (command == "automation-degree-vector-reference") {
            PositionVector scaleValue = parsePositionVector(args, "scale");
            IntervalVector criterion = parseIntervalVector(args, "criterion");
            vector<int> degrees = parseIntList(requireArg(args, "degrees"));
            vector<PositionVector> references = parsePositionVectorSequence(args, "references");
            vector<int> complexities = parseOptionalIntList(args, "complexities");
            writePositionVectorList(cout, degreeAutomationVectorReference(scaleValue, criterion, degrees, references, complexities));
        } else if (command == "automation-degree-reference-seq") {
            PositionVector scaleValue = parsePositionVector(args, "scale");
            IntervalVector criterion = parseIntervalVector(args, "criterion");
            vector<int> degrees = parseIntList(requireArg(args, "degrees"));
            PositionVector reference = parsePositionVector(args, "reference");
            vector<int> complexities = parseOptionalIntList(args, "complexities");
            writePositionVectorList(cout, degreeAutomationReference(scaleValue, criterion, degrees, reference, complexities));
        } else if (command == "automation-degree-forward-seq") {
            PositionVector scaleValue = parsePositionVector(args, "scale");
            IntervalVector criterion = parseIntervalVector(args, "criterion");
            vector<int> degrees = parseIntList(requireArg(args, "degrees"));
            PositionVector reference = parsePositionVector(args, "reference");
            vector<int> complexities = parseOptionalIntList(args, "complexities");
            writePositionVectorList(cout, forwardDegreeAutomation(scaleValue, criterion, degrees, reference, complexities));
        } else if (command == "automation-degree-backward-seq") {
            PositionVector scaleValue = parsePositionVector(args, "scale");
            IntervalVector criterion = parseIntervalVector(args, "criterion");
            vector<int> degrees = parseIntList(requireArg(args, "degrees"));
            PositionVector reference = parsePositionVector(args, "reference");
            vector<int> complexities = parseOptionalIntList(args, "complexities");
            writePositionVectorList(cout, degreeAutomationSequentialBackward(scaleValue, criterion, degrees, reference, complexities));
        } else if (command == "note-names-midi") {
            NoteNamingSystem system;
            NoteMapperOptions options(parseBool(args, "prefer-sharps", true), parseBool(args, "is-diatonic-scale", false), parseInt(args, "modulo-value", 12));
            writeNoteResult(cout, system.midiNumbersToNoteNames(parseIntList(requireArg(args, "notes")), options));
        } else if (command == "note-names-position") {
            NoteNamingSystem system;
            PositionVector value = parsePositionVector(args, "value");
            NoteMapperOptions options(parseBool(args, "prefer-sharps", true), parseBool(args, "is-diatonic-scale", false), parseInt(args, "modulo-value", value.getMod()));
            writeNoteResult(cout, system.positionVectorToNoteNames(value, options));
        } else if (command == "analyze-chord") {
            const vector<int> notes = parseIntList(requireArg(args, "notes"));
            const int rootIndex = parseInt(args, "root-index", 0);
            ChordAnalysis analysis = analyzeChord(notes, rootIndex);
            analysis.chordName = buildChordName(analysis);
            writeChordAnalysis(cout, analysis);
        } else if (command == "measure-summary") {
            PositionVector value = parsePositionVector(args, "value");
            writeMeasureSummary(cout, value);
        } else if (command == "measure-generate") {
            writeIntVector(cout, generate(parseInt(args, "m"), parseInt(args, "k"), parseInt(args, "n"), false));
        } else if (command == "measure-is-generated") {
            vector<int> data = parseIntList(requireArg(args, "data"));
            pair<bool, int> generated = isGenerated(data, parseInt(args, "n"));
            cout << "{\"found\":" << (generated.first ? "true" : "false") << ",\"multiplier\":" << generated.second << "}";
        } else if (command == "scale-database-find") {
            ScaleDatabase database;
            vector<int> notes = parseIntList(requireArg(args, "notes"));
            writeScaleMatches(cout, database.findScale(notes));
        } else if (command == "quantize") {
            cout << quantize(parseInt(args, "note"), parseIntList(requireArg(args, "scale")), parseBool(args, "left", true));
        } else if (command == "transpose-notes") {
            PositionVector inputScale = parsePositionVector(args, "input-scale");
            PositionVector outputScale = parsePositionVector(args, "output-scale");
            PositionVector outDegrees;
            PositionVector outNotes;
            writeQuantizeTransposeResult(
                cout,
                transpose(
                    inputScale,
                    outputScale,
                    parseInt(args, "in-root", 0),
                    parseInt(args, "out-root", 0),
                    parseIntList(requireArg(args, "notes")),
                    outDegrees,
                    outNotes));
        } else if (command == "melody-diminution") {
            writeIntVector(cout, diminution(parseInt(args, "degree"), parseInt(args, "length"), parseBool(args, "up", false), parseBool(args, "left", false)));
        } else if (command == "melody-run") {
            writeIntVector(cout, run(parseInt(args, "degree"), parseInt(args, "length"), parseBool(args, "direction", false)));
        } else if (command == "melody-run2") {
            writeIntVector(cout, run2(parseInt(args, "start"), parseInt(args, "end")));
        } else if (command == "melody-normalize-notes") {
            writeIntVector(cout, normalizeNotes(parseIntList(requireArg(args, "notes")), parseInt(args, "mod")));
        } else if (command == "melody-hierarchy") {
            writeMelodyAnalysis(
                cout,
                hierarchy(
                    parseInt(args, "note"),
                    parseIntList(requireArg(args, "chord")),
                    parseIntList(requireArg(args, "scale")),
                    parseIntList(requireArg(args, "chromatic")),
                    parseInt(args, "mod")));
        } else if (command == "melody-note-from-position") {
            cout << getNoteFromPosition(parseInt(args, "position"), parseIntList(requireArg(args, "vector")), parseInt(args, "mod"));
        } else if (command == "melody-triple-select") {
            Analysis analysis = hierarchy(
                parseInt(args, "note"),
                parseIntList(requireArg(args, "chord")),
                parseIntList(requireArg(args, "scale")),
                parseIntList(requireArg(args, "chromatic")),
                parseInt(args, "mod"));
            vector<int> deltas = parseIntList(requireArg(args, "deltas"));
            vector<VectorModification::Type> types = parseModificationTypes(requireArg(args, "types"));
            vector<VectorModification> modifications = parseModifications(deltas, types);
            TripleSelectResult result = tripleSelect(
                analysis,
                modifications,
                parseIntList(requireArg(args, "chord")),
                parseIntList(requireArg(args, "scale")),
                parseIntList(requireArg(args, "chromatic")),
                parseInt(args, "mod"));
            writeTripleSelectResult(cout, result);
        } else if (command == "melody-ornament-loop") {
            writeIntVector(cout, ornamentLoop(parseIntList(requireArg(args, "values")), parseInt(args, "length")));
        } else if (command == "melody-apply-triple-select") {
            vector<int> notes = parseIntList(requireArg(args, "notes"));
            vector<int> chordNotes = parseIntList(requireArg(args, "chord"));
            vector<int> scaleNotes = parseIntList(requireArg(args, "scale"));
            vector<int> chromaticNotes = parseIntList(requireArg(args, "chromatic"));
            vector<int> ornaments = parseIntList(requireArg(args, "ornaments"));
            vector<VectorModification::Type> types = parseModificationTypes(requireArg(args, "types"));
            vector<VectorModification::Type> expandedTypes = ornamentTypesLoop(types, static_cast<int>(ornaments.size()));
            vector<VectorModification> modifications = parseModifications(ornaments, expandedTypes);
            cout << "[";
            for (size_t i = 0; i < notes.size(); ++i) {
                if (i > 0) {
                    cout << ",";
                }
                Analysis analysis = hierarchy(notes[i], chordNotes, scaleNotes, chromaticNotes, parseInt(args, "mod"));
                TripleSelectResult result = tripleSelect(analysis, modifications, chordNotes, scaleNotes, chromaticNotes, parseInt(args, "mod"));
                cout << "{\"note\":" << notes[i] << ",\"selection\":";
                writeTripleSelectResult(cout, result);
                cout << "}";
            }
            cout << "]";
        } else if (command == "slonimsky") {
            const string operation = requireArg(args, "op");
            if (operation == "infrapolation-permutation") {
                writeIntVector(cout, slonimsky::infrapolationPermutation(parseInt(args, "m")));
            } else {
                const slonimsky::Seq sequence = parseIntList(requireArg(args, "seq"));
                if (operation == "intervals") {
                writeIntVector(cout, slonimsky::intervals(sequence));
                } else if (operation == "min-interval") {
                cout << slonimsky::minInterval(sequence);
                } else if (operation == "interpolation-int") {
                writeIntVector(cout, slonimsky::interpolation(sequence, parseInt(args, "k")));
                } else if (operation == "interpolation-list") {
                writeIntVector(cout, slonimsky::interpolation(sequence, parseIntList(requireArg(args, "k"))));
                } else if (operation == "symmetric-interpolation-int") {
                writeIntVector(cout, slonimsky::symmetricInterpolation(sequence, parseInt(args, "k")));
                } else if (operation == "symmetric-interpolation-list") {
                writeIntVector(cout, slonimsky::symmetricInterpolation(sequence, parseIntList(requireArg(args, "k"))));
                } else if (operation == "asymmetric-interpolation") {
                writeIntVector(cout, slonimsky::asymmetricInterpolation(sequence, parseIntList(requireArg(args, "k")), parseIntList(requireArg(args, "l"))));
                } else if (operation == "ultrapolation-int") {
                writeIntVector(cout, slonimsky::ultrapolation(sequence, parseInt(args, "k")));
                } else if (operation == "ultrapolation-list") {
                writeIntVector(cout, slonimsky::ultrapolation(sequence, parseIntList(requireArg(args, "k"))));
                } else if (operation == "infrapolation-int") {
                writeIntVector(cout, slonimsky::infrapolation(sequence, parseInt(args, "k")));
                } else if (operation == "infrapolation-list") {
                writeIntVector(cout, slonimsky::infrapolation(sequence, parseIntList(requireArg(args, "k"))));
                } else if (operation == "infra-interpolation") {
                writeIntVector(cout, slonimsky::infraInterpolation(sequence, parseInt(args, "k"), parseInt(args, "l")));
                } else if (operation == "inter-infrapolation") {
                writeIntVector(cout, slonimsky::interInfrapolation(sequence, parseInt(args, "k"), parseInt(args, "l")));
                } else if (operation == "infra-ultrapolation") {
                writeIntVector(cout, slonimsky::infraUltrapolation(sequence, parseInt(args, "k"), parseInt(args, "l")));
                } else if (operation == "inter-ultrapolation") {
                writeIntVector(cout, slonimsky::interUltrapolation(sequence, parseInt(args, "k"), parseInt(args, "l")));
                } else if (operation == "ultra-interpolation") {
                writeIntVector(cout, slonimsky::ultraInterpolation(sequence, parseInt(args, "k"), parseInt(args, "l")));
                } else if (operation == "infra-inter-ultrapolation") {
                writeIntVector(cout, slonimsky::infraInterUltrapolation(sequence, parseInt(args, "k"), parseInt(args, "l"), parseInt(args, "m")));
                } else if (operation == "inter-infra-interpolation") {
                writeIntVector(cout, slonimsky::interInfraInterpolation(sequence, parseInt(args, "k"), parseInt(args, "l"), parseInt(args, "m")));
                } else if (operation == "ultra-infra-interpolation") {
                writeIntVector(cout, slonimsky::ultraInfraInterpolation(sequence, parseInt(args, "k"), parseInt(args, "l"), parseInt(args, "m")));
                } else {
                    throw invalid_argument("Unsupported slonimsky operation: " + operation);
                }
            }
        } else {
            throw invalid_argument("Unsupported command: " + command);
        }

        cout << endl;
        return 0;
    } catch (const exception& error) {
        cerr << error.what() << endl;
        return 1;
    }
}
