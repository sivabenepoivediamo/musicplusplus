# **music++** C++ SDK

> **Part of the **music++** Turborepo Monorepo**  
> For monorepo documentation, see the [root README](../../README.md).

## Overview

**music++** is a C++ library for the vectorial representation and manipulation of musical phenomena. It provides a unified framework for working with musical structures such as scales, chords, rhythms, and transformations using mathematical and algorithmic tools. The library is designed for music theorists, computational musicologists, composers, and developers interested in algorithmic and generative music.

## Features

- Unified vector classes for positions, intervals, and binary patterns (`PositionVector`, `IntervalVector`, `BinaryVector`).
- A unified `Vectors` container that keeps the three representations synchronized and provides convenient conversions and high-level operations.
- Meta-operators for selection and transformation (position/interval selection, modal selection, modal interchange).
- Chord and scale utilities for generating chords from scales/degrees and transforming them (transposition, inversion, rototranslation, mirroring).
- Rich distance and similarity metrics (Euclidean, Manhattan, Hamming, Levenshtein/edit distance, weighted transformation distance) with matrix-based search utilities for best matches.
- Matrix utilities: modal matrices, transposition matrices, and rototranslation matrices plus helpers to compute distances between a reference and all matrix rows (`matrix.h`, `matrixDistance.h`).
- Rhythmic utilities and generators: Euclidean rhythms, Clough–Douthett, deep rhythms, tihai and conversion helpers (`rhythmGen.h`).
- Note naming and mapping utilities to convert MIDI/position vectors to human-readable note names with enharmonic handling (`noteNames.h`).
- Analysis and measurement helpers (spectrum, symmetry, entropy, deepness checks, geodesic distances) in `measures.h`.
- Automation helpers for voice-leading, degree-based automations, modal interchange and modulation (`automations.h`).
- Examples covering most features are provided under `examples/` to serve as usage references and simple tests.

## Library Structure

```
src/
	automations.h          # High-level automation helpers (voice leading, degree automation, modal interchange, modulation)
	binaryVector.h        # BinaryVector class for rhythmic patterns and logical operations
	chord.h               # Chord class and ChordParams: generate chords from scales or intervals
	distances.h           # Distance and transformation metrics and helpers
	intervalVector.h      # IntervalVector class (intervallic representations and operations)
	mathUtil.h            # Math helpers (Euclidean division, GCD, LCM)
	matrixDistance.h      # Distance calculation wrappers for matrix result rows and utilities
	matrix.h              # Modal, transposition and rototranslation matrix generators
	measures.h            # Analytical measures: spectra, symmetry, entropy, deepness, etc.
	noteNames.h           # Mapping position vectors / MIDI numbers to note names (enharmonic handling)
	positionVector.h      # PositionVector class (positional representations and geometric ops)
	quantizeTranspose.h   # Quantize/transposition helpers between scales
	rhythmGen.h           # Rhythmic pattern generators (Euclidean, Clough-Douthett, deep rhythms, tihai)
	scale.h               # Scale class and ScaleParams
	selection.h           # Selection meta-operators for position/interval sources
	utility.h             # Common includes and project-wide using declarations
	Vector.h              # Vectors: unified representation and convenience constructors
	vectors.h             # Standalone conversion helpers between representations

examples/
	automations.cpp       # Examples: automation helpers (degree/voice-leading/modulation)
	automationsSeq.cpp    # Sequential voice-leading and degree automation example
	chordClass.cpp        # Chord class usage and examples
	chordTest.cpp         # Chord helper tests
	classtest.cpp         # Core class test suite (PositionVector/IntervalVector/BinaryVector)
	distances.cpp         # Distance metrics and transformation examples
	matrixDistances.cpp   # Matrix-distance examples
	matrix.cpp            # Matrix generation and utilities examples
	measures.cpp          # Example usage of measures/analysis helpers
	noteNames.cpp         # Note naming system examples and tests
	rhythmGen.cpp         # Rhythmic generators demonstration
	scale.cpp             # Scale class demonstrations
	selection.cpp         # Selection meta-operators demo
	vectortest.cpp        # Demonstration of Vectors unified API

LICENSE
README.md
```

## Main Components

### Vector Classes
- **PositionVector**: Cyclic positional vectors for pitch sets, supports rotation, inversion, complement, and more.
- **IntervalVector**: Intervallic structures with cyclic access, rotation, inversion, and scalar/vector operations.
- **BinaryVector**: Binary (0/1) vectors for rhythmic patterns, logical operations, and cyclic transformations.
- **Vectors**: Unified class maintaining synchronized position, interval, and binary representations.

### Meta-Operators & Algorithms
- **Selection**: Meta-operators for extracting or generating new vectors from source vectors using position or interval criteria.
- **Chord**: Functions to generate chords from scales and degrees/intervals, supporting transformations and inversions.
- **Matrix**: Modal, transpositional, and rototranslational matrix generation for musical analysis.
- **Distances**: Euclidean, Manhattan, edit, Hamming, and weighted transformation distances for musical vectors.

### Rhythmic Generators
- **Euclidean rhythms**: Evenly distributed pulses in a given number of steps.
- **Clough-Douthett patterns**: Rhythmic patterns based on the Clough-Douthett Algorithm.
- **Deep rhythms**: Patterns with multiplicity and distributed onsets.
- **Tihai**: North Indian rhythmic patterns with repetition and offset.

### Scale Class
- **Scale**: Encapsulates musical scales, supporting construction from positions or intervals, and transformations such as transposition, mode, inversion, and mirroring.

### Chord Class

### Automation Functions
- **Voice Leading**: Computes the best inversion for the transition between two chords, based on a complexity factor.
-**Degree Selection**: Computes the best degree of a chord in a transition based on a reference intervallic structure and a complexity factor. 
-**Modal Interchange**: Selection of a mode of the parent scale based on a note vector input and a complexity factor. 
-**Modulation**: Selection of a transposition of a scale, based on a note vector input and a complexity factor.

### Vector analysis and measures


## TODO
- **Rhythmic subdivision priority grid**
- **Block Chords**
- **Spread Voicings**
- **Melody and phrase abstractions**
- **Imitation and counterpoint functions**
- **Slonimsky 'Thesaurus' implementation**
- **Scale dictionary**
- **Chord from parameters**



## Getting Started

> **Note**: This C++ SDK is part of a Turborepo monorepo. For monorepo setup and root-level commands, see the [root README](../../README.md).

### As a Header-Only Library

1. Include the `src/` directory in your C++ project.
2. All headers are self-contained and can be included as needed.
3. Requires C++17 or later.

### Building Examples (Monorepo)

From the monorepo root:

```bash
# Build all packages (including cpp-sdk examples)
npm run build

# Build only cpp-sdk
npm run build --workspace=packages/cpp-sdk
```

Or from this package directory:

```bash
# Configure and build with CMake
npm run build

# Clean build artifacts
npm run clean
```

### Running Examples

After building, executables are in the `build/` directory:

```powershell
# Windows
.\build\autoscale.exe
.\build\vectortest.exe

# Linux/macOS
./build/autoscale
./build/vectortest
```

### Debugging in VS Code

**Quick Start:**
1. Open any example file from `examples/` (e.g., `autoscale.cpp`)
2. Press **F5** to build and debug
3. Set breakpoints and inspect variables

The workspace is configured with CMake-based build and debug tasks. See [DEBUGGING.md](DEBUGGING.md) for:
- Detailed debugging instructions
- Build system architecture
- Troubleshooting common issues
- Advanced build configurations

### Generating Documentation

```bash
# Generate Doxygen documentation
npm run docs

# Clean documentation
npm run docs:clean
```

Documentation will be available at `docs/html/index.html`.

### Manual Build (without npm)

You can also build manually with CMake:

```powershell
# Configure with CMake (MinGW Makefiles on Windows)
cmake -S . -B build -G "MinGW Makefiles"

# Build all examples
cmake --build build --config Debug

# Build a specific example
cmake --build build --target autoscale --config Debug

# Run an example
.\build\autoscale.exe
```

**Other generators:**
```bash
# Ninja (faster)
cmake -S . -B build -G Ninja

# Unix Makefiles (Linux/macOS/WSL)
cmake -S . -B build -G "Unix Makefiles"
```

## License

MIT License. See [LICENSE](LICENSE) for details.

## Bibliography:

- Albini, G. (2021). *Perché le note sono sette: Una storia dei fondamenti della musica tra matematica e percezione*. Fuori dal coro. Milano: Algama.

- Arbonés, J., & Milrud, P. (2011). *L'armonia è questione di numeri: musica e matematica*. Milano: RBA Italia.

- Arom, S. (2004). *African polyphony and polyrhythm: Musical structure and methodology*. Cambridge: Cambridge University Press.

- Assayag, G., Feictinger, H. G., & Rodrigues, J. F. (2002). *Mathematics and music: a Diderot mathematical forum*. Berlin: Springer.

- Benson, D. J. (2007). *Music: a mathematical offering* (3rd print., 2008). Cambridge: Cambridge University Press.

- Boebinger, D., Norman-Haignere, S. V., McDermott, J. H., & Kanwisher, N. (2021). Music-selective neural populations arise without musical training. *Journal of Neurophysiology*, 125(6), 2237–2263.

- Brent, J., & Barkley, S. (2011). *Modalogy: scales, modes, chords: the primordial building blocks of Music*. Milwaukee: Hal Leonard Corporation.

- Burnham, B. R., Long, E., & Zeide, J. (2021). Pitch direction on the perception of major and minor modes. *Attention, Perception & Psychophysics*, 83(1), 399–414.

- Caimmi, R., Franzon, A., & Tognon, S. (2017). *Intervalli musicali nella scala temperata a 12 note: interpretazione geometrica*. Canterano (RM): Aracne.

- Callender, C. (2004). Continuous transformations. *Music Theory Online*, 10(3), 3.

- Callender, C. (2007). Continuous harmonic spaces. *Journal of Music Theory*, 51(2), 277–332.

- Callender, C., Quinn, I., & Tymoczko, D. (2008). Generalized voice-leading spaces. *Science*, 320(5874), 346–348.

- Camiz, P., & Giansanti, A. (2019). *Fantalezioni di musimatefisica: un approccio musicale alle scienze esatte*. Milano: Tab.

- Cohn, R. (1991). Properties and generability of transpositionally invariant sets. *Journal of Music Theory*, 35(1/2), 1–32.

- Cohn, R. (1997). Neo-riemannian operations, parsimonious trichords, and their "tonnetz" representations. *Journal of Music Theory*, 41(1), 1–66.

- Cohn, R. (1998). Introduction to neo-riemannian theory: a survey and a historical perspective. *Journal of Music Theory*, 42(2), 167–180.

- Demaine, E. D., Gomez-Martin, F., Meijer, H., Rappaport, D., Taslakian, P., Toussaint, G. T., Winograd, T., & Wood, D. R. The distance geometry of deep rhythms and scales. [Manuscript or working paper].

- Douthett, J., & Steinbach, P. (1998). Parsimonious graphs: A study in parsimony, contextual transformations, and modes of limited transposition. *Journal of Music Theory*, 42(2), 241–263.

- Douthett, J., Hyde, M. M., & Smith, C. J. (2008). *Music Theory and Mathematics: Chords, Collections, and Transformations* (NED-New edition). Woodbridge: Boydell & Brewer.

- Emmer, M. (2012). *Imagine Math: between culture and mathematics*. Milano: Springer.

- Emmer, M. (2013). *Imagine Math 2: between culture and mathematics*. Milano: Springer.

- Emmer, M., Abate, M., & Villarreal, M. (2015). *Imagine Math 4: between culture and mathematics*. Milano: Springer.

- Fauvel, J., Flood, R., & Wilson, R. J. (2003). *Music and mathematics: from Pythagoras to fractals*. Oxford: Oxford University Press.

- Forte, A. (1973). *The structure of atonal music*. New Haven: Yale University Press.

- Garland, T. H., & Kahn, C. V. (1995). *Math and music: harmonious connections*. Palo Alto, CA: Dale Seymour.

- Hall, R. W., & Tymoczko, D. (2007, July). Poverty and polyphony: A connection between economics and music. In *Bridges Donostia: Mathematics, Music, Art, Architecture, Culture* (pp. 259–268).

- Harkleroad, L. (2006). *The math behind the music*. New York: Cambridge University Press.

- Hasty, C. (1997). *Meter As Rhythm*. New York: Oxford University Press.

- Hoffman, J. (2008). On pitch-class set cartography: Relations between voice-leading spaces and Fourier spaces. *Journal of Music Theory*, 52(2), 219–249.

- Huron, D. (1989). Voice Denumerability in Polyphonic Music of Homogeneous Timbres. *Music Perception: An Interdisciplinary Journal*, 6(4), 361–382.

- Huron, D. (2006). *Sweet Anticipation: Music and the Psychology of Expectation*. Cambridge, MA: The MIT Press.

- Illiano, R., & Locanto, M. (2019). *Twentieth-century music and mathematics*. Turnhout: Brepols.

- Jacoby, N., Tishby, N., & Tymoczko, D. (2015). An Information Theoretic Approach to Chord Categorization and Functional Harmony. *Journal of New Music Research*, 44(3), 219–244.

- Jedrzejewski, F. (2006). *Mathematical theory of music*. Sampzon: Delatour France; Paris: Ircam-Centre Pompidou.

- Johnson, T. A. (2008). *Foundations of diatonic theory: a mathematically based approach to music fundamentals*. Lanham, MD: Scarecrow Press.

- Keith, M. (1991). *From Polychords to Polya: Adventures in Musical Combinatorics*. Princeton: Vinculum Press.

- Kircher, A. (1650). *Musurgia universalis sive Ars magna consoni et dissoni*. Roma.

- Kung, D. (2013). *How music and mathematics relate: course guidebook*. Chantilly, VA: Great Courses.

- Levy, E. (1985). *A Theory of Harmony*. Albany: State University of New York Press.

- Lewin, D. (1959). Re: Intervallic relations between two collections of notes. *Journal of Music Theory*, 3(2), 298–301.

- Lewin, D. (2001). Special cases of the interval function between pitch-class sets X and Y. *Journal of Music Theory*, 45(1), 1–29.

- Lewin, D. (2011). *Generalized musical intervals and transformations*. Oxford: Oxford University Press.

- Loy, G. (2011). *Musimathics: the mathematical foundations of music*. Cambridge, MA; London: MIT Press.

- Maor, E. (2018). *La musica dai numeri: musica e matematica, da Pitagora a Schoenberg*. Torino: Codice.

- Mazzola, G. (1985). *Gruppen und Kategorien in der Musik: Entwurf einer mathematischen Musiktheorie*. Berlin: Heldermann.

- Mazzola, G., Mannone, M., & Pang, Y. (2016). *Cool math for hot music: a first introduction to mathematics for music theorists*. Cham: Springer.

- Mazzola, G. (2017). *The topos of music* (2nd ed.). Cham: Springer.

- Micallef Grimaud, A., & Eerola, T. (2022). Emotional expression through musical cues: A comparison of production and perception approaches. *PLoS One*, 17(12), e0278593.

- Odifreddi, P. (2005). *Penna, pennello e bacchetta: le tre invidie del matematico*. Roma; Bari: Laterza.

- Palmer, S. E., Schloss, K. B., Xu, Z., & Prado-León, L. R. (2013). Music-color associations are mediated by emotion. *Proceedings of the National Academy of Sciences of the United States of America*, 110(22), 8836–8841.

- Palmer, S. E., Langlois, T. A., & Schloss, K. B. (2016). Music-to-Color Associations of Single-Line Piano Melodies in Non-synesthetes. *Multisensory Research*, 29(1-3), 157–193.

- Petrarca, S. (2010). *Matematica per la musica e il suono*. Roma: Aracne.

- Quinn, I. (2006). General equal-tempered harmony (introduction and part I). *Perspectives of New Music*, 44(2), 114–158.

- Quinn, I. (2007). General equal-tempered harmony: parts 2 and 3. *Perspectives of New Music*, 45(1), 4–63.

- Roberts, G. E. (2016). *From music to mathematics: exploring the connections*. Baltimore, MD: Johns Hopkins University Press.

- Robinson, T. (2006, April). The End of Similarity? Semitonal Offset as Similarity Measure. In *Annual meeting of the Music Theory Society of New York State*, Saratoga Springs, NY.

- Russell, G. (1971). *The Lydian Chromatic Concept of Tonal Organization for Improvisation: For All Instruments*. New York: Concept Publishing Company.

- Sivakumar, A., & Tymoczko, D. (2019). Intuitive Musical Homotopy: Algebraic, Geometric, Combinatorial, Topological and Applied Approaches to Understanding Musical Phenomena. [Working paper or manuscript].

- Spence, C., & Di Stefano, N. (2022). Coloured hearing, colour music, colour organs, and the search for perceptually meaningful correspondences between colour and sound. *i-Perception*, 13(3), 20416695221092802.

- Straus, J. N. (2003). Uniformity, balance, and smoothness in atonal voice leading. *Music Theory Spectrum*, 25(2), 305–352.

- Straus, J. N. (2005). Voice leading in set-class space. *Journal of Music Theory*, 49(1), 45–108.

- Toussaint, G. D. (2020). *The geometry of musical rhythm: what makes a "good" rhythm good?* Boca Raton, FL: CRC Press.

- Tymoczko, D. (2004). Scale networks and Debussy. *Journal of Music Theory*, 48(2), 219–294.

- Tymoczko, D. (2006). The geometry of musical chords. *Science*, 313(5783), 72–74.

- Tymoczko, D. (2008). Scale theory, serial theory and voice leading. *Music Analysis*, 27(1), 1–69.

- Tymoczko, D. (2008). Set-class similarity, voice leading, and the Fourier transform. *Journal of Music Theory*, 52(2), 251–272.

- Tymoczko, D. (2009). Three Conceptions of Musical Distance. In *Mathematics and Computation in Music. MCM 2009. Communications in Computer and Information Science* (Vol. 38). Berlin, Heidelberg: Springer.

- Tymoczko, D. (2011). *A Geometry of Music: Harmony and Counterpoint in the Extended Common Practice*. New York: Oxford University Press.

- Walker, J., & Don, G. W. (2013). *Mathematics and music: composition, perception, and performance*. Boca Raton, FL: CRC Press.


- Whiteford, K. L., Schloss, K. B., Helwig, N. E., & Palmer, S. E. (2018). Color, Music, and Emotion: Bach to the Blues. *i-Perception*, 9(6), 2041669518808535.

