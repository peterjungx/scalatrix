#ifndef SCALATRIX_PITCHSET_HPP
#define SCALATRIX_PITCHSET_HPP

#include <string>
#include <vector>

namespace scalatrix {

typedef struct { 
    std::string label; // the prime number as label
    unsigned int number; // the prime number as int
    double log2fr; // log2 frequency ratio assigned to prime
} PseudoPrimeInt;
typedef std::vector<PseudoPrimeInt> PrimeList;
PseudoPrimeInt pseudoPrimeFromIndexNumber(unsigned int index);
PrimeList generateDefaultPrimeList(int n_primes=8);


typedef struct { 
    std::string label;
    double log2fr; // log2 frequency ratio
} PitchSetPitch;
typedef std::vector<PitchSetPitch> PitchSet;
PitchSet generateETPitchSet(unsigned int n_et, double equave_log2fr = 1.0);
PitchSet generateJIPitchSet(PrimeList primes, int max_numtimesden = 20, double min_log2fr = 0.0, double max_log2fr = 1.0);
PitchSet generateHarmonicSeriesPitchSet(PrimeList primes, int base, double min_log2fr = 0.0, double max_log2fr = 1.001);


}; // namespace scalatrix

#endif // SCALATRIX_PITCHSET_HPP