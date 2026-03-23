/**
 * @file classtest.cpp
 * @brief Example: unit-like tests for core vector classes
 *
 * Exercises constructors, arithmetic and utility methods of `IntervalVector`,
 * `BinaryVector`, and `PositionVector` and prints results to stdout.
 *
 * @example
 */
#include "../src/intervalVector.h"
#include "../src/binaryVector.h"
#include "../src/positionVector.h"

void printSeparator(const string& title) {
    cout << "\n" << string(60, '=') << "\n";
    cout << "  " << title << "\n";
    cout << string(60, '=') << "\n";
}

void testIntervalVector() {
    printSeparator("INTERVALVECTOR TESTS");
    
    // Constructors
    cout << "\n--- Constructors ---\n";
    IntervalVector iv1;
    cout << "Default constructor: " << iv1 << endl;
    
    IntervalVector iv2({2, 3, 5, 7}, 1, 12);
    cout << "Parameterized constructor: " << iv2 << endl;
    
    // Getters
    cout << "\n--- Getters ---\n";
    cout << "Size: " << iv2.size() << endl;
    cout << "Offset: " << iv2.getOffset() << endl;
    cout << "Mod: " << iv2.getMod() << endl;
    cout << "Empty: " << (iv2.empty() ? "true" : "false") << endl;
    
    // Setters
    cout << "\n--- Setters ---\n";
    iv2.setOffset(3);
    cout << "After setOffset(3): offset = " << iv2.getOffset() << endl;
    iv2.setMod(24);
    cout << "After setMod(24): mod = " << iv2.getMod() << endl;
    iv2.setData({1, 4, 7});
    cout << "After setData({1, 4, 7}): " << iv2 << endl;
    
    // Scalar operations
    cout << "\n--- Scalar Operations ---\n";
    IntervalVector iv3({1, 2, 3, 4});
    cout << "Original: " << iv3 << endl;
    cout << "iv3 + 5: " << (iv3 + 5) << endl;
    cout << "iv3 - 2: " << (iv3 - 2) << endl;
    cout << "iv3 * 3: " << (iv3 * 3) << endl;
    cout << "iv3 / 2: " << (iv3 / 2) << endl;
    cout << "iv3 % 3: " << (iv3 % 3) << endl;
    cout << "5 + iv3: " << (5 + iv3) << endl;
    cout << "10 - iv3: " << (10 - iv3) << endl;
    cout << "2 * iv3: " << (2 * iv3) << endl;
    
    // Vector operations
    cout << "\n--- Vector Operations ---\n";
    IntervalVector iv4({1, 2, 3});
    IntervalVector iv5({4, 5, 6});
    cout << "iv4: " << iv4 << endl;
    cout << "iv5: " << iv5 << endl;
    cout << "iv4 + iv5: " << (iv4 + iv5) << endl;
    cout << "iv4 - iv5: " << (iv4 - iv5) << endl;
    cout << "iv4 * iv5: " << (iv4 * iv5) << endl;
    cout << "iv4 / iv5: " << (iv4 / iv5) << endl;
    cout << "iv4 % iv5: " << (iv4 % iv5) << endl;
    
    // Operations with vector<int>
    cout << "\n--- Operations with vector<int> ---\n";
    vector<int> vec1 = {2, 3, 4};
    cout << "iv4 + vec: " << (iv4 + vec1) << endl;
    cout << "iv4 - vec: " << (iv4 - vec1) << endl;
    cout << "iv4 * vec: " << (iv4 * vec1) << endl;
    cout << "iv4 / vec: " << (iv4 / vec1) << endl;
    cout << "iv4 % vec: " << (iv4 % vec1) << endl;
    
    // Compound assignment operators
    cout << "\n--- Compound Assignment Operators ---\n";
    IntervalVector iv6({10, 20, 30});
    cout << "Original: " << iv6 << endl;
    iv6 += 5;
    cout << "After += 5: " << iv6 << endl;
    iv6 -= 3;
    cout << "After -= 3: " << iv6 << endl;
    iv6 *= 2;
    cout << "After *= 2: " << iv6 << endl;
    iv6 /= 4;
    cout << "After /= 4: " << iv6 << endl;
    iv6 %= 3;
    cout << "After %= 3: " << iv6 << endl;
    
    IntervalVector iv7({5, 10, 15});
    iv7 += IntervalVector({1, 2, 3});
    cout << "After += IntervalVector: " << iv7 << endl;
    
    // Element access
    cout << "\n--- Element Access ---\n";
    IntervalVector iv8({10, 20, 30, 40});
    cout << "iv8: " << iv8 << endl;
    cout << "iv8[0]: " << iv8[0] << endl;
    cout << "iv8[2]: " << iv8[2] << endl;
    cout << "iv8[-1]: " << iv8[-1] << endl;
    cout << "iv8[5]: " << iv8[5] << endl;
    
    // Comparison operators
    cout << "\n--- Comparison Operators ---\n";
    IntervalVector iv9({1, 2, 3});
    IntervalVector iv10({1, 2, 3});
    IntervalVector iv11({4, 5, 6});
    cout << "iv9 == iv10: " << (iv9 == iv10 ? "true" : "false") << endl;
    cout << "iv9 == iv11: " << (iv9 == iv11 ? "true" : "false") << endl;
    cout << "iv9 != iv11: " << (iv9 != iv11 ? "true" : "false") << endl;
    
    // Rotation and transformation
    cout << "\n--- Rotation and Transformation ---\n";
    IntervalVector iv12({1, 2, 3, 4, 5});
    cout << "Original: " << iv12 << endl;
    cout << "rotate(2): " << iv12.rotate(2) << endl;
    cout << "rotate(-1): " << iv12.rotate(-1) << endl;
    cout << "rotate(2, 3): " << iv12.rotate(2, 3) << endl;
    cout << "reverse(): " << iv12.reverse() << endl;
    cout << "retrograde(): " << iv12.retrograde() << endl;
    cout << "negate(): " << iv12.negate() << endl;
    
    // Inversion
    cout << "\n--- Inversion ---\n";
    IntervalVector iv13({1, 2, 3, 4, 5, 6});
    cout << "Original: " << iv13 << endl;
    cout << "inversion(0): " << iv13.inversion(0) << endl;
    cout << "inversion(1): " << iv13.inversion(1) << endl;
    cout << "inversion(2): " << iv13.inversion(2) << endl;
    
    // Normalization
    cout << "\n--- Normalization ---\n";
    IntervalVector iv14({13, 25, -3, 14});
    cout << "Original: " << iv14 << endl;
    cout << "normalize(12): " << iv14.normalize(12) << endl;
    cout << "normalize(7): " << iv14.normalize(7) << endl;
    
    // Componentwise operations
    cout << "\n--- Componentwise Operations ---\n";
    IntervalVector iv15({1, 2, 3});
    vector<int> vec2 = {4, 5};
    cout << "iv15: " << iv15 << endl;
    cout << "vec2: [4, 5]" << endl;
    cout << "componentwiseSum (no loop): " << iv15.componentwiseSum(vec2, false) << endl;
    cout << "componentwiseSum (loop): " << iv15.componentwiseSum(vec2, true) << endl;
    cout << "componentwiseProduct (loop): " << iv15.componentwiseProduct(vec2, true) << endl;
    
    // Utility methods
    cout << "\n--- Utility Methods ---\n";
    IntervalVector iv16({1, 2, 3});
    IntervalVector iv17({4, 5});
    cout << "iv16: " << iv16 << endl;
    cout << "iv17: " << iv17 << endl;
    cout << "concatenate: " << iv16.concatenate(iv17) << endl;
    cout << "repeat(3): " << iv16.repeat(3) << endl;
    
    // Mirroring operations
    cout << "\n--- Mirroring Operations ---\n";
    IntervalVector iv18({1, 2, 3, 4, 5, 6});
    cout << "Original: " << iv18 << endl;
    cout << "singleMirror(3, true): " << iv18.singleMirror(3, true) << endl;
    cout << "singleMirror(3, false): " << iv18.singleMirror(3, false) << endl;
    cout << "doubleMirror(3): " << iv18.doubleMirror(3) << endl;
    cout << "crossMirror(3, true): " << iv18.crossMirror(3, true) << endl;
    cout << "crossMirror(3, false): " << iv18.crossMirror(3, false) << endl;
    
    // Static method - adaptToLCM
    cout << "\n--- Static Method: adaptToLCM ---\n";
    IntervalVector iv19({1, 2}, 0, 6);
    IntervalVector iv20({3, 4}, 0, 4);
    vector<IntervalVector> ivVec = {iv19, iv20};
    cout << "Before LCM adaptation:" << endl;
    cout << "  iv19 (mod 6): " << iv19 << endl;
    cout << "  iv20 (mod 4): " << iv20 << endl;
    vector<IntervalVector> adapted = IntervalVector::adaptToLCM(ivVec);
    cout << "After LCM adaptation:" << endl;
    cout << "  adapted[0]: " << adapted[0] << " (mod " << adapted[0].getMod() << ")" << endl;
    cout << "  adapted[1]: " << adapted[1] << " (mod " << adapted[1].getMod() << ")" << endl;
    
    // Print methods
    cout << "\n--- Print Methods ---\n";
    IntervalVector iv21({7, 14, 21}, 2, 12);
    iv21.printAll();
}

void testBinaryVector() {
    printSeparator("BINARYVECTOR TESTS");
    
    // Constructors
    cout << "\n--- Constructors ---\n";
    BinaryVector bv1;
    cout << "Default constructor: " << bv1 << endl;
    
    BinaryVector bv2({1, 0, 1, 1, 0}, 0, 5);
    cout << "Parameterized constructor: " << bv2 << endl;
    
    // Getters
    cout << "\n--- Getters ---\n";
    cout << "Size: " << bv2.size() << endl;
    cout << "Offset: " << bv2.getOffset() << endl;
    cout << "Mod: " << bv2.getMod() << endl;
    cout << "Pulse count: " << bv2.countPulses() << endl;
    cout << "Density: " << bv2.density() << endl;
    
    // Setters
    cout << "\n--- Setters ---\n";
    bv2.setOffset(2);
    cout << "After setOffset(2): offset = " << bv2.getOffset() << endl;
    bv2.setMod(8);
    cout << "After setMod(8): mod = " << bv2.getMod() << endl;
    
    // Scalar operations (multiplication/division)
    cout << "\n--- Scalar Operations ---\n";
    BinaryVector bv3({1, 0, 1, 0});
    cout << "Original: " << bv3 << endl;
    cout << "bv3 * 2: " << (bv3 * 2) << endl;
    cout << "bv3 * 3: " << (bv3 * 3) << endl;
    BinaryVector bv3_expanded = bv3 * 4;
    cout << "bv3 * 4: " << bv3_expanded << endl;
    cout << "Compressed / 2: " << (bv3 / 2) << endl;
    cout << "2 * bv3: " << (2 * bv3) << endl;
    
    // Compound assignment
    cout << "\n--- Compound Assignment ---\n";
    BinaryVector bv4({1, 0, 1});
    cout << "Original: " << bv4 << endl;
    bv4 *= 2;
    cout << "After *= 2: " << bv4 << endl;
    bv4 /= 2;
    cout << "After /= 2: " << bv4 << endl;
    
    // Logical operations with LCM adaptation
    cout << "\n--- Logical Operations (LCM adapted) ---\n";
    BinaryVector bv5({1, 0, 1, 0}, 0, 4);
    BinaryVector bv6({1, 1, 0}, 0, 3);
    cout << "bv5 (mod 4): " << bv5 << endl;
    cout << "bv6 (mod 3): " << bv6 << endl;
    cout << "bv5 | bv6 (OR): " << (bv5 | bv6) << endl;
    cout << "bv5 & bv6 (AND): " << (bv5 & bv6) << endl;
    cout << "bv5 ^ bv6 (XOR): " << (bv5 ^ bv6) << endl;
    cout << "bv5.nor(bv6): " << bv5.nor(bv6) << endl;
    cout << "bv5.nand(bv6): " << bv5.nand(bv6) << endl;
    cout << "bv5.xnor(bv6): " << bv5.xnor(bv6) << endl;
    
    // Complement (NOT)
    cout << "\n--- Complement ---\n";
    BinaryVector bv7({1, 0, 1, 0, 1});
    cout << "Original: " << bv7 << endl;
    cout << "~bv7 (NOT): " << (~bv7) << endl;
    cout << "complement(): " << bv7.complement() << endl;
    
    // Compound logical assignment
    cout << "\n--- Compound Logical Assignment ---\n";
    BinaryVector bv8({1, 0, 1, 0});
    BinaryVector bv9({1, 1, 0, 0});
    cout << "bv8: " << bv8 << endl;
    cout << "bv9: " << bv9 << endl;
    bv8 |= bv9;
    cout << "After |=: " << bv8 << endl;
    
    BinaryVector bv10({1, 1, 1, 1});
    BinaryVector bv11({1, 0, 1, 0});
    bv10 &= bv11;
    cout << "After &=: " << bv10 << endl;
    
    // Element access
    cout << "\n--- Element Access ---\n";
    BinaryVector bv12({1, 0, 1, 0});
    cout << "bv12: " << bv12 << endl;
    cout << "bv12[0]: " << bv12[0] << endl;
    cout << "bv12[2]: " << bv12[2] << endl;
    cout << "bv12[-1]: " << bv12[-1] << endl;
    cout << "bv12[5]: " << bv12[5] << endl;
    
    // Comparison operators
    cout << "\n--- Comparison Operators ---\n";
    BinaryVector bv13({1, 0, 1});
    BinaryVector bv14({1, 0, 1});
    BinaryVector bv15({0, 1, 0});
    cout << "bv13 == bv14: " << (bv13 == bv14 ? "true" : "false") << endl;
    cout << "bv13 == bv15: " << (bv13 == bv15 ? "true" : "false") << endl;
    cout << "bv13 != bv15: " << (bv13 != bv15 ? "true" : "false") << endl;
    
    // Rotation
    cout << "\n--- Rotation ---\n";
    BinaryVector bv16({1, 0, 0, 1, 0});
    cout << "Original: " << bv16 << endl;
    cout << "rotate(1): " << bv16.rotate(1) << endl;
    cout << "rotate(2): " << bv16.rotate(2) << endl;
    cout << "rotate(-1): " << bv16.rotate(-1) << endl;
    
    // Inversion
    cout << "\n--- Inversion ---\n";
    BinaryVector bv17({1, 0, 1, 0, 1});
    cout << "Original: " << bv17 << endl;
    cout << "inversion(0): " << bv17.inversion(0) << endl;
    cout << "inversion(2): " << bv17.inversion(2) << endl;
    
    // Transposition (offset change)
    cout << "\n--- Transposition ---\n";
    BinaryVector bv18({1, 0, 1, 0});
    cout << "Original: " << bv18 << endl;
    cout << "transpose(3): " << bv18.transpose(3) << endl;
    
    // Utility methods
    cout << "\n--- Utility Methods ---\n";
    BinaryVector bv19({1, 0, 1});
    BinaryVector bv20({0, 1});
    cout << "bv19: " << bv19 << endl;
    cout << "bv20: " << bv20 << endl;
    cout << "concatenate: " << bv19.concatenate(bv20) << endl;
    cout << "repeat(3): " << bv19.repeat(3) << endl;
    
    // Pulse analysis
    cout << "\n--- Pulse Analysis ---\n";
    BinaryVector bv21({1, 0, 0, 1, 0, 1, 0, 0});
    cout << "Pattern: " << bv21 << endl;
    cout << "Pulse count: " << bv21.countPulses() << endl;
    cout << "Density: " << bv21.density() << endl;
    vector<int> indices = bv21.getPulseIndices();
    cout << "Pulse indices: [";
    for (size_t i = 0; i < indices.size(); ++i) {
        cout << indices[i];
        if (i < indices.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
    vector<int> intervals = bv21.getInterOnsetIntervals();
    cout << "Inter-onset intervals: [";
    for (size_t i = 0; i < intervals.size(); ++i) {
        cout << intervals[i];
        if (i < intervals.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
    
    // Euclidean rhythm generation
    cout << "\n--- Euclidean Rhythm Generation ---\n";
    BinaryVector eucl1 = BinaryVector::euclidean(3, 8);
    cout << "euclidean(3, 8): " << eucl1 << endl;
    BinaryVector eucl2 = BinaryVector::euclidean(5, 12);
    cout << "euclidean(5, 12): " << eucl2 << endl;
    BinaryVector eucl3 = BinaryVector::euclidean(5, 16);
    cout << "euclidean(5, 16): " << eucl3 << endl;
    
    // Static method - adaptToLCM
    cout << "\n--- Static Method: adaptToLCM ---\n";
    BinaryVector bv22({1, 0, 1}, 0, 3);
    BinaryVector bv23({1, 1, 0, 0}, 0, 4);
    vector<BinaryVector> bvVec = {bv22, bv23};
    cout << "Before LCM adaptation:" << endl;
    cout << "  bv22 (mod 3): " << bv22 << endl;
    cout << "  bv23 (mod 4): " << bv23 << endl;
    vector<BinaryVector> adapted = BinaryVector::adaptToLCM(bvVec);
    cout << "After LCM adaptation:" << endl;
    cout << "  adapted[0]: " << adapted[0] << " (mod " << adapted[0].getMod() << ")" << endl;
    cout << "  adapted[1]: " << adapted[1] << " (mod " << adapted[1].getMod() << ")" << endl;
    
    // Componentwise operations
    cout << "\n--- Componentwise Operations ---\n";
    BinaryVector bv24({1, 0, 1});
    vector<int> vec1 = {1, 1};
    cout << "bv24: " << bv24 << endl;
    cout << "vec: [1, 1]" << endl;
    cout << "componentwiseOr (no loop): " << bv24.componentwiseOr(vec1, false) << endl;
    cout << "componentwiseOr (loop): " << bv24.componentwiseOr(vec1, true) << endl;
    cout << "componentwiseAnd (loop): " << bv24.componentwiseAnd(vec1, true) << endl;
    cout << "componentwiseXor (loop): " << bv24.componentwiseXor(vec1, true) << endl;
    
    // Print methods
    cout << "\n--- Print Methods ---\n";
    BinaryVector bv25({1, 0, 0, 1, 0, 1, 0, 0}, 0, 8);
    bv25.printInfo();
    cout << "Pattern visualization: ";
    bv25.printPattern();
}

void testPositionVector() {
    printSeparator("POSITIONVECTOR TESTS");
    
    // Constructors
    cout << "\n--- Constructors ---\n";
    PositionVector pv1;
    cout << "Default constructor: " << pv1 << endl;
    
    PositionVector pv2({0, 2, 4, 7, 9}, 12, 0, true, false);
    cout << "Parameterized constructor: " << pv2 << endl;
    
    // Getters
    cout << "\n--- Getters ---\n";
    cout << "Size: " << pv2.size() << endl;
    cout << "Mod: " << pv2.getMod() << endl;
    cout << "UserRange: " << pv2.getUserRange() << endl;
    cout << "Range: " << pv2.getRange() << endl;
    cout << "RangeUpdate: " << (pv2.getRangeUpdate() ? "true" : "false") << endl;
    cout << "User: " << (pv2.getUser() ? "true" : "false") << endl;
    
    // Setters
    cout << "\n--- Setters ---\n";
    pv2.setMod(24);
    cout << "After setMod(24): mod = " << pv2.getMod() << endl;
    pv2.setUserRange(36);
    cout << "After setUserRange(36): userRange = " << pv2.getUserRange() << endl;
    pv2.setRangeUpdate(false);
    cout << "After setRangeUpdate(false): rangeUpdate = " << (pv2.getRangeUpdate() ? "true" : "false") << endl;
    pv2.setUser(true);
    cout << "After setUser(true): user = " << (pv2.getUser() ? "true" : "false") << endl;
    pv2.setRange(48);
    cout << "After setRange(48): range = " << pv2.getRange() << endl;
    
    // Scalar operations
    cout << "\n--- Scalar Operations ---\n";
    PositionVector pv3({0, 3, 7, 10});
    cout << "Original: " << pv3 << endl;
    cout << "pv3 + 5: " << (pv3 + 5) << endl;
    cout << "pv3 - 2: " << (pv3 - 2) << endl;
    cout << "pv3 * 2: " << (pv3 * 2) << endl;
    cout << "pv3 / 2: " << (pv3 / 2) << endl;
    cout << "pv3 % 3: " << (pv3 % 3) << endl;
    cout << "5 + pv3: " << (5 + pv3) << endl;
    cout << "10 - pv3: " << (10 - pv3) << endl;
    cout << "3 * pv3: " << (3 * pv3) << endl;
    
    // Vector operations
    cout << "\n--- Vector Operations ---\n";
    PositionVector pv4({1, 2, 3});
    PositionVector pv5({4, 5, 6});
    cout << "pv4: " << pv4 << endl;
    cout << "pv5: " << pv5 << endl;
    cout << "pv4 + pv5: " << (pv4 + pv5) << endl;
    cout << "pv4 - pv5: " << (pv4 - pv5) << endl;
    cout << "pv4 * pv5: " << (pv4 * pv5) << endl;
    cout << "pv4 / pv5: " << (pv4 / pv5) << endl;
    cout << "pv4 % pv5: " << (pv4 % pv5) << endl;
    
    // Operations with vector<int>
    cout << "\n--- Operations with vector<int> ---\n";
    vector<int> vec1 = {2, 3, 4};
    cout << "pv4 + vec: " << (pv4 + vec1) << endl;
    cout << "pv4 - vec: " << (pv4 - vec1) << endl;
    cout << "pv4 * vec: " << (pv4 * vec1) << endl;
    cout << "pv4 / vec: " << (pv4 / vec1) << endl;
    cout << "pv4 % vec: " << (pv4 % vec1) << endl;
    
    // Compound assignment operators
    cout << "\n--- Compound Assignment Operators ---\n";
    PositionVector pv6({10, 20, 30});
    cout << "Original: " << pv6 << endl;
    pv6 += 5;
    cout << "After += 5: " << pv6 << endl;
    pv6 -= 3;
    cout << "After -= 3: " << pv6 << endl;
    pv6 *= 2;
    cout << "After *= 2: " << pv6 << endl;
    pv6 /= 4;
    cout << "After /= 4: " << pv6 << endl;
    pv6 %= 3;
    cout << "After %= 3: " << pv6 << endl;
    
    PositionVector pv7({5, 10, 15});
    pv7 += PositionVector({1, 2, 3});
    cout << "After += PositionVector: " << pv7 << endl;
    
    // Element access
    cout << "\n--- Element Access ---\n";
    PositionVector pv8({0, 3, 7, 10}, 12);
    cout << "pv8: " << pv8 << endl;
    cout << "pv8[0]: " << pv8[0] << endl;
    cout << "pv8[2]: " << pv8[2] << endl;
    cout << "pv8[-1]: " << pv8[-1] << endl;
    cout << "pv8[4]: " << pv8[4] << endl;
    cout << "pv8[5]: " << pv8[5] << endl;
    
    // Comparison operators
    cout << "\n--- Comparison Operators ---\n";
    PositionVector pv9({0, 3, 7});
    PositionVector pv10({0, 3, 7});
    PositionVector pv11({1, 4, 8});
    cout << "pv9 == pv10: " << (pv9 == pv10 ? "true" : "false") << endl;
    cout << "pv9 == pv11: " << (pv9 == pv11 ? "true" : "false") << endl;
    cout << "pv9 != pv11: " << (pv9 != pv11 ? "true" : "false") << endl;
    
    // Rotation
    cout << "\n--- Rotation ---\n";
    PositionVector pv12({0, 3, 7, 10});
    cout << "Original: " << pv12 << endl;
    cout << "rotate(1): " << pv12.rotate(1) << endl;
    cout << "rotate(2): " << pv12.rotate(2) << endl;
    cout << "rotate(-1): " << pv12.rotate(-1) << endl;
    
    // Roto-translation
    cout << "\n--- Roto-Translation ---\n";
    PositionVector pv13({0, 2, 5, 7, 9});
    cout << "Original: " << pv13 << endl;
    cout << "rotoTranslate(2): " << pv13.rotoTranslate(2) << endl;
    cout << "rotoTranslate(2, 3): " << pv13.rotoTranslate(2, 3) << endl;
    cout << "rotoTranslate(-1): " << pv13.rotoTranslate(-1) << endl;
    
    // Complement
    cout << "\n--- Complement ---\n";
    PositionVector pv14({0, 3, 7}, 12);
    cout << "Original: " << pv14 << endl;
    cout << "complement(): " << pv14.complement() << endl;
    
    // Inversion
    cout << "\n--- Inversion ---\n";
    PositionVector pv15({0, 4, 7, 11});
    cout << "Original: " << pv15 << endl;
    cout << "inversion(0): " << pv15.inversion(0) << endl;
    cout << "inversion(1): " << pv15.inversion(1) << endl;
    cout << "inversion(0, true): " << pv15.inversion(0, true) << endl;
    
    // Negative transformation
    cout << "\n--- Negative Transformation ---\n";
    PositionVector pv16({0, 4, 7});
    cout << "Original: " << pv16 << endl;
    cout << "negative() standard: " << pv16.negative() << endl;
    cout << "negative(5) standard: " << pv16.negative(5) << endl;
    cout << "negative(10, false): " << pv16.negative(10, false) << endl;
    
    // Componentwise operations
    cout << "\n--- Componentwise Operations ---\n";
    PositionVector pv17({1, 2, 3});
    vector<int> vec2 = {4, 5};
    cout << "pv17: " << pv17 << endl;
    cout << "vec2: [4, 5]" << endl;
    cout << "componentwiseSum (no loop): " << pv17.componentwiseSum(vec2, false) << endl;
    cout << "componentwiseSum (loop): " << pv17.componentwiseSum(vec2, true) << endl;
    cout << "componentwiseProduct (loop): " << pv17.componentwiseProduct(vec2, true) << endl;
    cout << "componentwiseDivision (loop): " << pv17.componentwiseDivision(vec2, true) << endl;
    cout << "componentwiseModulo (loop): " << pv17.componentwiseModulo(vec2, true) << endl;
    
    // Utility methods
    cout << "\n--- Utility Methods ---\n";
    PositionVector pv18({0, 3, 7});
    PositionVector pv19({10, 14});
    cout << "pv18: " << pv18 << endl;
    cout << "pv19: " << pv19 << endl;
    cout << "scalarSum(5): " << pv18.scalarSum(5) << endl;
    cout << "scalarProduct(2): " << pv18.scalarProduct(2) << endl;
    cout << "concatenate: " << pv18.concatenate(pv19) << endl;
    cout << "repeat(2): " << pv18.repeat(2) << endl;
    
    // Static method - adaptToLCM
    cout << "\n--- Static Method: adaptToLCM ---\n";
    PositionVector pv20({0, 2}, 6);
    PositionVector pv21({0, 3}, 4);
    vector<PositionVector> pvVec = {pv20, pv21};
    cout << "Before LCM adaptation:" << endl;
    cout << "  pv20 (mod 6): " << pv20 << endl;
    cout << "  pv21 (mod 4): " << pv21 << endl;
    vector<PositionVector> adaptedPV = PositionVector::adaptToLCM(pvVec);
    cout << "After LCM adaptation:" << endl;
    cout << "  adapted[0]: " << adaptedPV[0] << " (mod " << adaptedPV[0].getMod() << ")" << endl;
    cout << "  adapted[1]: " << adaptedPV[1] << " (mod " << adaptedPV[1].getMod() << ")" << endl;
    
    // Range calculation tests
    cout << "\n--- Range Calculation ---\n";
    PositionVector pv22({0, 5, 12, 18}, 12, 0, true, false);
    cout << "pv22 (span 0-18, mod 12): " << pv22 << endl;
    cout << "Calculated range: " << pv22.getRange() << endl;
    
    PositionVector pv23({-3, 0, 5}, 12, 24, false, true);
    cout << "pv23 (userRange 24, user=true): " << pv23 << endl;
    cout << "Effective range: " << pv23.getRange() << endl;
    
    //Resize method
    cout << "\n--- Resize Method ---\n";
    PositionVector pvResize({0, 3, 6, 9}, 12);
    cout << "Original: " << pvResize << endl;
    PositionVector resized = pvResize.resize(-3, 6);
    cout << "After resize(-1, 6): " << resized << endl;

    // Print methods
    cout << "\n--- Print Methods ---\n";
    PositionVector pv24({0, 4, 7, 11}, 12, 24, true, false);
    pv24.printAll();
}

/*void testCrossClassInteractions() {
    
    cout << "\n--- LCM Adaptation Comparison ---\n";
    
    // IntervalVector LCM
    IntervalVector iv1({1, 2}, 0, 3);
    IntervalVector iv2({2, 3}, 0, 4);
    vector<IntervalVector> ivVec = {iv1, iv2};
    auto ivAdapted = IntervalVector::adaptToLCM(ivVec);
    cout << "IntervalVector LCM adaptation (mod 3 and 4 -> mod 12):" << endl;
    cout << "  Original iv1: " << iv1 << " (mod " << iv1.getMod() << ")" << endl;
    cout << "  Original iv2: " << iv2 << " (mod " << iv2.getMod() << ")" << endl;
    cout << "  Adapted iv1:  " << ivAdapted[0] << " (mod " << ivAdapted[0].getMod() << ")" << endl;
    cout << "  Adapted iv2:  " << ivAdapted[1] << " (mod " << ivAdapted[1].getMod() << ")" << endl;
    
    // BinaryVector LCM
    BinaryVector bv1({1, 0, 1}, 0, 3);
    BinaryVector bv2({1, 1, 0, 0}, 0, 4);
    vector<BinaryVector> bvVec = {bv1, bv2};
    auto bvAdapted = BinaryVector::adaptToLCM(bvVec);
    cout << "\nBinaryVector LCM adaptation (mod 3 and 4 -> mod 12):" << endl;
    cout << "  Original bv1: " << bv1 << endl;
    cout << "  Original bv2: " << bv2 << endl;
    cout << "  Adapted bv1:  " << bvAdapted[0] << endl;
    cout << "  Adapted bv2:  " << bvAdapted[1] << endl;
    
    // PositionVector LCM
    PositionVector pv1({0, 2}, 6);
    PositionVector pv2({0, 3}, 4);
    vector<PositionVector> pvVec = {pv1, pv2};
    auto pvAdapted = PositionVector::adaptToLCM(pvVec);
    cout << "\nPositionVector LCM adaptation (mod 6 and 4 -> mod 12):" << endl;
    cout << "  Original pv1: " << pv1 << " (mod " << pv1.getMod() << ")" << endl;
    cout << "  Original pv2: " << pv2 << " (mod " << pv2.getMod() << ")" << endl;
    cout << "  Adapted pv1:  " << pvAdapted[0] << " (mod " << pvAdapted[0].getMod() << ")" << endl;
    cout << "  Adapted pv2:  " << pvAdapted[1] << " (mod " << pvAdapted[1].getMod() << ")" << endl;
}
*/
int main() {
    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════╗\n";
    cout << "║         COMPREHENSIVE VECTOR CLASS TEST SUITE             ║\n";
    cout << "╚════════════════════════════════════════════════════════════╝\n";
    
    try {
        testIntervalVector();
        testBinaryVector();
        testPositionVector();
        //testCrossClassInteractions();
        
        printSeparator("ALL TESTS COMPLETED SUCCESSFULLY");
        cout << "\nNo exceptions were thrown during testing.\n";
        cout << "All operators, methods, and transformations have been exercised.\n\n";
        
    } catch (const exception& e) {
        cerr << "\n!!! UNEXPECTED EXCEPTION !!!" << endl;
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
