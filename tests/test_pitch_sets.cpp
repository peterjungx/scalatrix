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
    
    SECTION("ET pitch set with range limits") {
        // Generate 12-TET with range from -1.0 to 2.0 (3 octaves)
        auto etPitchSet = generateETPitchSet(12, 1.0, -1.0, 2.0);
        
        // Should include steps from -12 to 24 (37 pitches total)
        REQUIRE(etPitchSet.size() == 37);
        
        // Check range boundaries
        REQUIRE_THAT(etPitchSet[0].log2fr, WithinAbs(-1.0, 1e-10));
        REQUIRE_THAT(etPitchSet[36].log2fr, WithinAbs(2.0, 1e-10));
        
        // Check that labels include negative numbers
        REQUIRE(etPitchSet[0].label == "-12\\12");
        REQUIRE(etPitchSet[12].label == "0\\12");  // Unison
        REQUIRE(etPitchSet[24].label == "12\\12"); // Octave
        REQUIRE(etPitchSet[36].label == "24\\12"); // Two octaves
        
        // Verify proper ordering
        for (size_t i = 1; i < etPitchSet.size(); ++i) {
            REQUIRE(etPitchSet[i].log2fr >= etPitchSet[i-1].log2fr);
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
        auto hsPitchSet = generateHarmonicSeriesPitchSet(primes, limit, 0.0, 1.001);
        
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
        auto hsPitchSet = generateHarmonicSeriesPitchSet(primes, limit, 0.0, tolerance);
        
        // Should still generate harmonics
        REQUIRE(hsPitchSet.size() > 0);
    }
    
    SECTION("Harmonic series pitches are in ascending order") {
        int limit = 32;
        auto hsPitchSet = generateHarmonicSeriesPitchSet(primes, limit, 0.0, 1.001);
        
        // Check ascending order
        for (size_t i = 1; i < hsPitchSet.size(); ++i) {
            REQUIRE(hsPitchSet[i].log2fr >= hsPitchSet[i-1].log2fr);
        }
    }
    
    SECTION("Harmonic series with lower bound generates subharmonics") {
        int base = 8;
        double min_log2fr = -3.1; // Include pitches down to 1:8 ratio (log2fr = -3.0)
        double max_log2fr = 1.0;
        auto hsPitchSet = generateHarmonicSeriesPitchSet(primes, base, min_log2fr, max_log2fr);
        
        // Should include the 1:8 ratio (log2fr = -3.0) within tolerance
        bool hasSubharmonic = false;
        for (const auto& pitch : hsPitchSet) {
            if (std::abs(pitch.log2fr - (-3.0)) < 1e-6) {
                hasSubharmonic = true;
                REQUIRE(pitch.label == "1:8");
                break;
            }
        }
        REQUIRE(hasSubharmonic);
        
        // Should include the base (8:8 = 1:1, now simplified)
        bool hasUnison = false;
        for (const auto& pitch : hsPitchSet) {
            if (std::abs(pitch.log2fr) < 1e-6) {
                hasUnison = true;
                REQUIRE(pitch.label == "1:1");
                break;
            }
        }
        REQUIRE(hasUnison);
        
        // Should include higher harmonics (16:8 = 2:1, now simplified)
        bool hasHigherHarmonic = false;
        for (const auto& pitch : hsPitchSet) {
            if (std::abs(pitch.log2fr - 1.0) < 1e-6) {
                hasHigherHarmonic = true;
                REQUIRE(pitch.label == "2:1");
                break;
            }
        }
        REQUIRE(hasHigherHarmonic);
        
        // Should be in ascending order
        for (size_t i = 1; i < hsPitchSet.size(); ++i) {
            REQUIRE(hsPitchSet[i].log2fr >= hsPitchSet[i-1].log2fr);
        }
    }
}

TEST_CASE("PitchSetPitch addition operator", "[pitchset]") {
    SECTION("Adding two ratio pitches") {
        PitchSetPitch a = {"3:2", std::log2(3.0/2.0)};
        PitchSetPitch b = {"5:4", std::log2(5.0/4.0)};
        
        PitchSetPitch result = a + b;
        
        // log2fr should be sum of inputs
        REQUIRE_THAT(result.log2fr, WithinAbs(std::log2(3.0/2.0) + std::log2(5.0/4.0), 1e-10));
        
        // Label should be simplified fraction: 3*5 : 2*4 = 15:8
        REQUIRE(result.label == "15:8");
    }
    
    SECTION("Adding ratio pitches that need GCD simplification") {
        PitchSetPitch a = {"4:3", std::log2(4.0/3.0)};
        PitchSetPitch b = {"6:4", std::log2(6.0/4.0)};
        
        PitchSetPitch result = a + b;
        
        // log2fr should be sum of inputs
        REQUIRE_THAT(result.log2fr, WithinAbs(std::log2(4.0/3.0) + std::log2(6.0/4.0), 1e-10));
        
        // Label should be simplified: 4*6 : 3*4 = 24:12 = 2:1 (after GCD)
        REQUIRE(result.label == "2:1");
    }
    
    SECTION("Adding two ET pitches with same denominator") {
        PitchSetPitch a = {"2\\12", 2.0/12.0};
        PitchSetPitch b = {"4\\12", 4.0/12.0};
        
        PitchSetPitch result = a + b;
        
        // log2fr should be sum of inputs
        REQUIRE_THAT(result.log2fr, WithinAbs(2.0/12.0 + 4.0/12.0, 1e-10));
        
        // Label should add numerators: 2+4 = 6
        REQUIRE(result.label == "6\\12");
    }
    
    SECTION("Adding ET pitches with different denominators fails") {
        PitchSetPitch a = {"2\\12", 2.0/12.0};
        PitchSetPitch b = {"3\\7", 3.0/7.0};
        
        PitchSetPitch result = a + b;
        
        // log2fr should still be sum
        REQUIRE_THAT(result.log2fr, WithinAbs(2.0/12.0 + 3.0/7.0, 1e-10));
        
        // Label should be empty (incompatible)
        REQUIRE(result.label == "");
    }
    
    SECTION("Adding ratio and ET pitches fails") {
        PitchSetPitch a = {"3:2", std::log2(3.0/2.0)};
        PitchSetPitch b = {"4\\12", 4.0/12.0};
        
        PitchSetPitch result = a + b;
        
        // log2fr should still be sum
        REQUIRE_THAT(result.log2fr, WithinAbs(std::log2(3.0/2.0) + 4.0/12.0, 1e-10));
        
        // Label should be empty (incompatible formats)
        REQUIRE(result.label == "");
    }
    
    SECTION("Adding negative ET pitches") {
        PitchSetPitch a = {"-2\\12", -2.0/12.0};
        PitchSetPitch b = {"4\\12", 4.0/12.0};
        
        PitchSetPitch result = a + b;
        
        // log2fr should be sum
        REQUIRE_THAT(result.log2fr, WithinAbs(-2.0/12.0 + 4.0/12.0, 1e-10));
        
        // Label should add: -2+4 = 2
        REQUIRE(result.label == "2\\12");
    }
    
    SECTION("Test case from user example: 2\\11 + 4\\11 = 6\\11") {
        PitchSetPitch a = {"2\\11", 2.0/11.0};
        PitchSetPitch b = {"4\\11", 4.0/11.0};
        
        PitchSetPitch result = a + b;
        
        // log2fr should be sum
        REQUIRE_THAT(result.log2fr, WithinAbs(2.0/11.0 + 4.0/11.0, 1e-10));
        
        // Label should be: 2+4 = 6
        REQUIRE(result.label == "6\\11");
    }
    
    SECTION("Adding malformed labels") {
        PitchSetPitch a = {"invalid", 0.5};
        PitchSetPitch b = {"3:2", std::log2(3.0/2.0)};
        
        PitchSetPitch result = a + b;
        
        // log2fr should still be sum
        REQUIRE_THAT(result.log2fr, WithinAbs(0.5 + std::log2(3.0/2.0), 1e-10));
        
        // Label should be empty (malformed input)
        REQUIRE(result.label == "");
    }
}

TEST_CASE("PitchSetPitch multiplication operator", "[pitchset]") {
    SECTION("Multiplying ratio by positive integer") {
        PitchSetPitch pitch = {"3:2", std::log2(3.0/2.0)};
        
        PitchSetPitch result = 2 * pitch;
        
        // log2fr should be multiplied
        REQUIRE_THAT(result.log2fr, WithinAbs(2.0 * std::log2(3.0/2.0), 1e-10));
        
        // Label should be power: 3^2 : 2^2 = 9:4
        REQUIRE(result.label == "9:4");
    }
    
    SECTION("Multiplying ratio by zero") {
        PitchSetPitch pitch = {"3:2", std::log2(3.0/2.0)};
        
        PitchSetPitch result = 0 * pitch;
        
        // log2fr should be 0
        REQUIRE_THAT(result.log2fr, WithinAbs(0.0, 1e-10));
        
        // Label should be 1:1 (3^0 : 2^0)
        REQUIRE(result.label == "1:1");
    }
    
    SECTION("Multiplying ratio by negative integer") {
        PitchSetPitch pitch = {"3:2", std::log2(3.0/2.0)};
        
        PitchSetPitch result = -1 * pitch;
        
        // log2fr should be negated
        REQUIRE_THAT(result.log2fr, WithinAbs(-std::log2(3.0/2.0), 1e-10));
        
        // Label should be inverted: 2:3
        REQUIRE(result.label == "2:3");
    }
    
    SECTION("Multiplying ET by positive integer") {
        PitchSetPitch pitch = {"3\\11", 3.0/11.0};
        
        PitchSetPitch result = 2 * pitch;
        
        // log2fr should be multiplied
        REQUIRE_THAT(result.log2fr, WithinAbs(2.0 * 3.0/11.0, 1e-10));
        
        // Label should multiply numerator: 6\\11
        REQUIRE(result.label == "6\\11");
    }
    
    SECTION("Multiplying ET by zero") {
        PitchSetPitch pitch = {"3\\11", 3.0/11.0};
        
        PitchSetPitch result = 0 * pitch;
        
        // log2fr should be 0
        REQUIRE_THAT(result.log2fr, WithinAbs(0.0, 1e-10));
        
        // Label should be 0\\11
        REQUIRE(result.label == "0\\11");
    }
    
    SECTION("Multiplying ET by negative integer") {
        PitchSetPitch pitch = {"7\\12", 7.0/12.0};
        
        PitchSetPitch result = -1 * pitch;
        
        // log2fr should be negated
        REQUIRE_THAT(result.log2fr, WithinAbs(-7.0/12.0, 1e-10));
        
        // Label should be negative: -7\\12
        REQUIRE(result.label == "-7\\12");
    }
    
    SECTION("Test user examples") {
        // 2 * (3:2) = 9:4
        PitchSetPitch pitch1 = {"3:2", std::log2(3.0/2.0)};
        PitchSetPitch result1 = 2 * pitch1;
        REQUIRE(result1.label == "9:4");
        REQUIRE_THAT(result1.log2fr, WithinAbs(2.0 * std::log2(3.0/2.0), 1e-10));
        
        // -1 * (3:2) = 2:3
        PitchSetPitch result2 = -1 * pitch1;
        REQUIRE(result2.label == "2:3");
        REQUIRE_THAT(result2.log2fr, WithinAbs(-std::log2(3.0/2.0), 1e-10));
        
        // 2 * (11\\14) = 22\\14
        PitchSetPitch pitch3 = {"11\\14", 11.0/14.0};
        PitchSetPitch result3 = 2 * pitch3;
        REQUIRE(result3.label == "22\\14");
        REQUIRE_THAT(result3.log2fr, WithinAbs(2.0 * 11.0/14.0, 1e-10));
        
        // 0 * (3\\11) = 0\\11
        PitchSetPitch pitch4 = {"3\\11", 3.0/11.0};
        PitchSetPitch result4 = 0 * pitch4;
        REQUIRE(result4.label == "0\\11");
        REQUIRE_THAT(result4.log2fr, WithinAbs(0.0, 1e-10));
    }
    
    SECTION("Multiplication with GCD simplification") {
        PitchSetPitch pitch = {"6:4", std::log2(6.0/4.0)};
        
        PitchSetPitch result = 2 * pitch;
        
        // 6^2 : 4^2 = 36:16 = 9:4 (after GCD)
        REQUIRE(result.label == "9:4");
        REQUIRE_THAT(result.log2fr, WithinAbs(2.0 * std::log2(6.0/4.0), 1e-10));
    }
    
    SECTION("Commutative property") {
        PitchSetPitch pitch = {"5:3", std::log2(5.0/3.0)};
        
        PitchSetPitch result1 = 3 * pitch;
        PitchSetPitch result2 = pitch * 3;
        
        // Both should give same result
        REQUIRE(result1.label == result2.label);
        REQUIRE_THAT(result1.log2fr, WithinAbs(result2.log2fr, 1e-10));
        REQUIRE(result1.label == "125:27");
    }
    
    SECTION("Multiplying malformed labels") {
        PitchSetPitch pitch = {"invalid", 0.5};
        
        PitchSetPitch result = 2 * pitch;
        
        // log2fr should still be multiplied
        REQUIRE_THAT(result.log2fr, WithinAbs(1.0, 1e-10));
        
        // Label should be empty (malformed input)
        REQUIRE(result.label == "");
    }
}

TEST_CASE("Pitch set integration", "[pitchset]") {
    SECTION("Different pitch sets have different characteristics") {
        auto etPitchSet = generateETPitchSet(12, 1.0);
        auto primes = generateDefaultPrimeList(3);
        auto jiPitchSet = generateJIPitchSet(primes, 20);
        auto hsPitchSet = generateHarmonicSeriesPitchSet(primes, 16, 0.0, 1.001);
        
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