#include "scalatrix/pitchset.hpp"
#include <numeric>

const int PRIMES[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};


namespace scalatrix {

PseudoPrimeInt pseudoPrimeFromIndexNumber(unsigned int index) {
    PseudoPrimeInt p;
    p.label = std::to_string(PRIMES[index]);
    p.number = PRIMES[index];
    p.log2fr = log2(p.number);
    return p;
};


PrimeList generateDefaultPrimeList(int n_primes) {
    n_primes = std::min(n_primes, 25);
    PrimeList primes;
    for (int i = 0; i < n_primes; i++) {
        PseudoPrimeInt p = pseudoPrimeFromIndexNumber(i);
        primes.push_back(p);
    }
    return primes;    
};


PitchSet generateETPitchSet(unsigned int n_et, double equave_log2fr) {
    PitchSet pitchset;
    for (int i = 0; i < n_et + 1; i++) {
        PitchSetPitch p;
        p.label = std::to_string(i) + "\\" + std::to_string(n_et);
        p.log2fr = i * equave_log2fr / n_et;
        pitchset.push_back(p);
    }
    return pitchset;
};

PitchSet generateJIPitchSet(PrimeList primes, int max_numorden, double min_log2fr, double max_log2fr) {
    PitchSet pitchset;
    PrimeList nums;
    // generate all numbers with prime factors in primes up to max_numorden
    for (int i = 1; i < max_numorden; i++) {
        int r = i;
        double log2fr = 0.0;
        for (auto p : primes) {
            while (r % p.number == 0) {
                r /= p.number;
                log2fr += p.log2fr;
            }
        }
        bool include = r == 1;
        if(include){
            PseudoPrimeInt p;
            p.label = std::to_string(i);
            p.number = i;
            p.log2fr = log2fr;
            nums.push_back(p);
        }
    }
    for (auto num: nums){
        for (auto den: nums){
            if (std::gcd(num.number, den.number) > 1){
                continue;
            }
            if (num.number <= max_numorden && den.number <= max_numorden) {
                PitchSetPitch pitch;
                pitch.label = num.label + ":" + den.label;
                pitch.log2fr = num.log2fr - den.log2fr;
                pitchset.push_back(pitch);
            }
        }
    }
    // filter out pitches with log2fr outside of min_log2fr and max_log2fr
    PitchSet filtered_pitchset;
    for (auto pitch : pitchset) {
        if (pitch.log2fr > min_log2fr - 1e-6 && pitch.log2fr < max_log2fr + 1e-6) {
            filtered_pitchset.push_back(pitch);
        }
    }
    // sort
    std::sort(filtered_pitchset.begin(), filtered_pitchset.end(), 
        [](PitchSetPitch a, PitchSetPitch b) {
            return a.log2fr < b.log2fr;
        }
    );
    return filtered_pitchset;
};


PitchSet generateHarmonicSeriesPitchSet(PrimeList primes, int base, double max_log2fr) {
    PitchSet pitchset;
    double base_log2fr = log2(base);
    for (int num = base; num <= base * exp2(max_log2fr); num++) {
        PitchSetPitch pitch;
        pitch.label = std::to_string(num) + ":" + std::to_string(base);
        pitch.log2fr = -base_log2fr;
        int r = num;
        for (auto p : primes) {
            while (r % p.number == 0) {
                r /= p.number;
                pitch.log2fr += p.log2fr;
            }
        }
        pitch.log2fr += log2(r);
        pitchset.push_back(pitch);
    }
    std::sort(pitchset.begin(), pitchset.end(), 
        [](PitchSetPitch a, PitchSetPitch b) {
            return a.log2fr < b.log2fr;
        }
    );
    return pitchset;
};


};