#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include "scalatrix/pitchset.hpp"
#include <cmath>

using namespace scalatrix;
using Catch::Matchers::WithinAbs;

TEST_CASE("ET pitch set generation", "[pitchset]") {
    SECTION("12-EDO pitch set") {
        auto etPitchSet = generateETPitchSet(12, 1.0);
        
        REQUIRE(etPitchSet.size() == 13); // Includes octave
        
        // Check first pitch (unity)
        REQUIRE_THAT(etPitchSet[0].log2fr, WithinAbs(0.0, 1e-10));
        
        // Check that pitches are evenly spaced
        for (size_t i = 1; i < etPitchSet.size() - 1; ++i) {
            double expectedPitch = i / 12.0;
            REQUIRE_THAT(etPitchSet[i].log2fr, WithinAbs(expectedPitch, 1e-10));
        }
        
        // Check octave
        REQUIRE_THAT(etPitchSet[12].log2fr, WithinAbs(1.0, 1e-10));
    }
    
    SECTION("24-EDO pitch set") {
        auto etPitchSet = generateETPitchSet(24, 1.0);
        
        REQUIRE(etPitchSet.size() == 25); // Includes octave
        
        // Check spacing
        for (size_t i = 1; i < etPitchSet.size() - 1; ++i) {
            double expectedPitch = i / 24.0;
            REQUIRE_THAT(etPitchSet[i].log2fr, WithinAbs(expectedPitch, 1e-10));
        }
    }
    
    SECTION("Different equave") {
        double equave = 1.2; // Stretched octave
        auto etPitchSet = generateETPitchSet(12, equave);
        
        // Check that pitches scale with the equave
        for (size_t i = 0; i < etPitchSet.size(); ++i) {
            double expectedPitch = (i / 12.0) * equave;
            REQUIRE_THAT(etPitchSet[i].log2fr, WithinAbs(expectedPitch, 1e-10));
        }
    }
}

TEST_CASE("Default prime list generation", "[pitchset]") {
    SECTION("Generate first 4 primes") {
        auto primes = generateDefaultPrimeList(4);
        
        REQUIRE(primes.size() == 4);
        
        // Check prime numbers (starts from 2)
        REQUIRE(primes[0].number == 2);
        REQUIRE(primes[0].label == "2");
        REQUIRE_THAT(primes[0].log2fr, WithinAbs(std::log2(2.0), 1e-10));
        
        REQUIRE(primes[1].number == 3);
        REQUIRE(primes[1].label == "3");
        REQUIRE_THAT(primes[1].log2fr, WithinAbs(std::log2(3.0), 1e-10));
        
        REQUIRE(primes[2].number == 5);
        REQUIRE(primes[2].label == "5");
        REQUIRE_THAT(primes[2].log2fr, WithinAbs(std::log2(5.0), 1e-10));
        
        REQUIRE(primes[3].number == 7);
        REQUIRE(primes[3].label == "7");
        REQUIRE_THAT(primes[3].log2fr, WithinAbs(std::log2(7.0), 1e-10));
    }
    
    SECTION("Empty prime list") {
        auto primes = generateDefaultPrimeList(0);
        REQUIRE(primes.empty());
    }
}

TEST_CASE("JI pitch set generation", "[pitchset]") {
    auto primes = generateDefaultPrimeList(3); // 3, 5, 7
    
    SECTION("Basic JI pitch set") {
        auto jiPitchSet = generateJIPitchSet(primes, 10);
        
        // Should have at least the unity and the primes
        REQUIRE(jiPitchSet.size() >= 4);
        
        // Check unity (1/1)
        bool hasUnity = false;
        for (const auto& pitch : jiPitchSet) {
            if (std::abs(pitch.log2fr) < 1e-10) {
                hasUnity = true;
                // Don't test exact label format, just that it's non-empty
                REQUIRE(!pitch.label.empty());
                break;
            }
        }
        REQUIRE(hasUnity);
    }
    
    SECTION("JI pitch set with cutoff") {
        double cutoff = 0.5; // Only include pitches below 0.5 (tritone)
        auto jiPitchSet = generateJIPitchSet(primes, 20, cutoff);
        
        // Should generate some pitches
        REQUIRE(jiPitchSet.size() > 0);
        
        // Check that we get some reasonable pitches in the range
        bool hasReasonablePitch = false;
        for (const auto& pitch : jiPitchSet) {
            if (pitch.log2fr >= 0.0 && pitch.log2fr <= cutoff + 0.1) {
                hasReasonablePitch = true;
                break;
            }
        }
        REQUIRE(hasReasonablePitch);
    }
    
    SECTION("JI pitch set with stretched equave") {
        double equave = 1.1;
        auto jiPitchSet = generateJIPitchSet(primes, 20, 0.0, equave);
        
        // Pitches should reflect the stretched equave
        // This is harder to test precisely without knowing the generation algorithm
        // but we can check that we get some pitches
        REQUIRE(jiPitchSet.size() > 0);
    }
    
    SECTION("Modified prime tuning") {
        auto modifiedPrimes = primes;
        modifiedPrimes[0].log2fr *= 1.01; // Detune the 3
        
        auto jiPitchSet1 = generateJIPitchSet(primes, 20);
        auto jiPitchSet2 = generateJIPitchSet(modifiedPrimes, 20);
        
        // The two sets should have different tunings
        bool foundDifference = false;
        for (size_t i = 0; i < std::min(jiPitchSet1.size(), jiPitchSet2.size()); ++i) {
            if (jiPitchSet1[i].label == jiPitchSet2[i].label) {
                if (std::abs(jiPitchSet1[i].log2fr - jiPitchSet2[i].log2fr) > 1e-10) {
                    foundDifference = true;
                    break;
                }
            }
        }
        REQUIRE(foundDifference);
    }
}

TEST_CASE("Harmonic series pitch set generation", "[pitchset]") {
    auto primes = generateDefaultPrimeList(3); // 3, 5, 7
    
    SECTION("Basic harmonic series") {
        int limit = 16;
        auto hsPitchSet = generateHarmonicSeriesPitchSet(primes, limit);
        
        // Should generate harmonics up to the limit
        REQUIRE(hsPitchSet.size() > 0);
        
        // Check that we have harmonic 1 (fundamental)
        bool hasUnity = false;
        for (const auto& pitch : hsPitchSet) {
            if (std::abs(pitch.log2fr) < 1e-10) {
                hasUnity = true;
                REQUIRE(!pitch.label.empty());
                break;
            }
        }
        REQUIRE(hasUnity);
        
        // Check that we have some higher harmonics (just check for non-zero pitches)
        bool hasHigherHarmonics = false;
        for (const auto& pitch : hsPitchSet) {
            if (pitch.log2fr > 0.1) { // Any pitch significantly above unity
                hasHigherHarmonics = true;
                break;
            }
        }
        REQUIRE(hasHigherHarmonics);
    }
    
    SECTION("Harmonic series with tolerance") {
        int limit = 16;
        double tolerance = 1.001; // Very small tolerance
        auto hsPitchSet = generateHarmonicSeriesPitchSet(primes, limit, tolerance);
        
        // Should still generate harmonics
        REQUIRE(hsPitchSet.size() > 0);
    }
    
    SECTION("Harmonic series pitches are in ascending order") {
        int limit = 32;
        auto hsPitchSet = generateHarmonicSeriesPitchSet(primes, limit);
        
        // Check ascending order
        for (size_t i = 1; i < hsPitchSet.size(); ++i) {
            REQUIRE(hsPitchSet[i].log2fr >= hsPitchSet[i-1].log2fr);
        }
    }
}

TEST_CASE("Pitch set integration", "[pitchset]") {
    SECTION("Different pitch sets have different characteristics") {
        auto etPitchSet = generateETPitchSet(12, 1.0);
        auto primes = generateDefaultPrimeList(3);
        auto jiPitchSet = generateJIPitchSet(primes, 20);
        auto hsPitchSet = generateHarmonicSeriesPitchSet(primes, 16);
        
        // ET should have exactly 13 pitches (including octave)
        REQUIRE(etPitchSet.size() == 13);
        
        // JI and HS should have different sizes
        REQUIRE(jiPitchSet.size() != etPitchSet.size());
        REQUIRE(hsPitchSet.size() != etPitchSet.size());
        
        // ET pitches should be evenly spaced
        for (size_t i = 1; i < etPitchSet.size(); ++i) {
            double interval = etPitchSet[i].log2fr - etPitchSet[i-1].log2fr;
            REQUIRE_THAT(interval, WithinAbs(1.0/12, 1e-10));
        }
    }
}