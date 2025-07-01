#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include "scalatrix/mos.hpp"
#include "scalatrix/scale.hpp"
#include "scalatrix/pitchset.hpp"
#include "scalatrix/label_calculator.hpp"
#include "scalatrix/params.hpp"
#include <cmath>

using namespace scalatrix;
using Catch::Matchers::WithinAbs;

TEST_CASE("Complete MOS workflow", "[integration]") {
    // Create a MOS similar to the native example
    MOS mos = MOS::fromG(3, 4, 0.585, 1.0, 1);
    
    SECTION("MOS construction and basic properties") {
        // Based on actual output from scalatrix_example
        REQUIRE(mos.a == 5);
        REQUIRE(mos.b == 2);
        REQUIRE(mos.n == 7);
        REQUIRE(mos.depth == 3);
        REQUIRE(mos.mode == 4);
        REQUIRE_THAT(mos.generator, WithinAbs(0.585, 1e-10));
        REQUIRE_THAT(mos.equave, WithinAbs(1.0, 1e-10));
    }
    
    SECTION("Generate scale from MOS") {
        auto scale = mos.generateScaleFromMOS(261.63, 128, 60);
        
        REQUIRE(scale.getNodes().size() == 128);
        REQUIRE(scale.getRootIdx() == 60);
        REQUIRE_THAT(scale.getBaseFreq(), WithinAbs(261.63, 1e-10));
        
        // Check that root node has expected frequency
        auto& nodes = scale.getNodes();
        REQUIRE_THAT(nodes[60].pitch, WithinAbs(261.63, 1e-10));
    }
    
    SECTION("MOS path generation") {
        // Based on actual output: "Path: 1 0 0"
        REQUIRE(mos.path.size() == 3);
        
        // Count L and s steps
        int countL = 0, countS = 0;
        for (auto step : mos.path) {
            if (step == 0) countL++;
            else if (step == 1) countS++;
        }
        
        // From the path "1 0 0": one 1 and two 0s
        REQUIRE(countS == 1); // one 's' step
        REQUIRE(countL == 2); // two 'L' steps
    }
}

TEST_CASE("Scale generation and tempering workflow", "[integration]") {
    // Create scale using affine transformation (like the native example)
    auto A = affineFromThreeDots(
        {0, 0}, {3, 1}, {5, 2},
        {0, 3.0/24}, {.585, 5.0/24}, {1.0, 3.0/24}
    );
    auto scale = Scale::fromAffine(A, 261.63, 128, 60);
    
    SECTION("Scale from affine has correct structure") {
        REQUIRE(scale.getNodes().size() == 128);
        REQUIRE(scale.getRootIdx() == 60);
        REQUIRE_THAT(scale.getBaseFreq(), WithinAbs(261.63, 1e-10));
        
        // Nodes should be sorted by tuning coordinate x
        auto& nodes = scale.getNodes();
        for (size_t i = 1; i < nodes.size(); ++i) {
            REQUIRE(nodes[i-1].tuning_coord.x <= nodes[i].tuning_coord.x);
        }
    }
    
    SECTION("Tempering with different pitch sets") {
        // Test ET tempering
        auto etPitchSet = generateETPitchSet(12, 1.0);
        scale.temperToPitchSet(etPitchSet);
        
        // Check that some nodes are tempered
        auto& nodes = scale.getNodes();
        bool hasTempered = false;
        for (auto& node : nodes) {
            if (node.isTempered) {
                hasTempered = true;
                REQUIRE(!node.temperedPitch.label.empty());
                break;
            }
        }
        REQUIRE(hasTempered);
        
        // Test JI tempering
        auto primes = generateDefaultPrimeList(4); // Get 2, 3, 5, 7
        auto jiPitchSet = generateJIPitchSet(primes, 20);
        scale.temperToPitchSet(jiPitchSet);
        
        // Should still have tempered nodes
        hasTempered = false;
        for (auto& node : nodes) {
            if (node.isTempered) {
                hasTempered = true;
                break;
            }
        }
        REQUIRE(hasTempered);
    }
}

TEST_CASE("MOS retuning operations", "[integration]") {
    MOS mos = MOS::fromG(3, 4, 0.585, 1.0, 1);
    auto scale = mos.generateScaleFromMOS(261.63, 12, 0);
    
    SECTION("One point retuning") {
        Vector2i targetCoord(0, 0);
        double targetPitch = 1.0;
        
        mos.retuneOnePoint(targetCoord, targetPitch);
        mos.retuneScaleWithMOS(scale, 261.63);
        
        // Check that the transform maps correctly
        auto transformed = mos.impliedAffine.apply(targetCoord);
        REQUIRE_THAT(transformed.x, WithinAbs(targetPitch, 1e-10));
    }
    
    SECTION("Two point retuning") {
        Vector2i coord1(0, 0);
        Vector2i coord2(mos.a, mos.b);
        double pitch2 = 1.1;
        
        mos.retuneTwoPoints(coord1, coord2, pitch2);
        mos.retuneScaleWithMOS(scale, 261.63);
        
        // Check equave adjustment
        REQUIRE_THAT(mos.equave, WithinAbs(pitch2, 1e-10));
    }
    
    SECTION("Three point retuning") {
        Vector2i coord1(0, 0);
        Vector2i coord2(mos.a, mos.b);
        Vector2i coord3 = mos.v_gen;
        double pitch3 = 7.0/12;
        
        mos.retuneThreePoints(coord1, coord2, coord3, pitch3);
        mos.retuneScaleWithMOS(scale, 261.63);
        
        // The generator should be adjusted (less strict due to iterative nature)
        auto transformed = mos.impliedAffine.apply(coord3);
        REQUIRE_THAT(transformed.x, WithinAbs(pitch3, 0.01));
    }
    
    SECTION("Reset to original tuning") {
        // First modify the MOS
        mos.retuneOnePoint({1, 1}, 0.5);
        
        // Then reset
        mos.retuneZeroPoint();
        mos.retuneScaleWithMOS(scale, 261.63);
        
        // Should be back to something reasonable (may not be exactly 0)
        auto origin = mos.impliedAffine.apply({0, 0});
        REQUIRE(std::abs(origin.x) < 1.0);
    }
}

TEST_CASE("Complete labeling workflow", "[integration]") {
    MOS mos = MOS::fromParams(5, 2, 1, 1.0, 0.585);
    LabelCalculator lc;
    auto scale = mos.generateScaleFromMOS(261.63, 128, 60);
    
    SECTION("Node labeling consistency") {
        auto& nodes = scale.getNodes();
        
        // Generate labels for scale nodes
        std::vector<std::string> labels;
        for (const auto& node : nodes) {
            labels.push_back(lc.noteLabelNormalized(mos, node.natural_coord));
        }
        
        // All labels should be non-empty
        for (const auto& label : labels) {
            REQUIRE(!label.empty());
        }
        
        // Root node should have a consistent label
        std::string rootLabel = lc.noteLabelNormalized(mos, nodes[60].natural_coord);
        REQUIRE(!rootLabel.empty());
    }
    
    SECTION("MOS node labeling with letter system") {
        // Test the MOS's own labeling system
        auto& baseNodes = mos.base_scale.getNodes();
        
        for (const auto& node : baseNodes) {
            std::string letterLabel = mos.nodeLabelLetterWithOctaveNumber(node.natural_coord);
            REQUIRE(!letterLabel.empty());
            
            // Coordinate should be recognized as in scale
            REQUIRE(mos.nodeInScale(node.natural_coord));
        }
    }
}

TEST_CASE("Pitch set integration with scales", "[integration]") {
    auto primes = generateDefaultPrimeList(4); // Get 2, 3, 5, 7
    auto etPitchSet = generateETPitchSet(12, 1.0);
    auto jiPitchSet = generateJIPitchSet(primes, 20);
    auto hsPitchSet = generateHarmonicSeriesPitchSet(primes, 16);
    
    SECTION("Different pitch sets produce different temperings") {
        // Create same base scale
        auto A = affineFromThreeDots(
            {0, 0}, {1, 0}, {0, 1},
            {0, 0}, {1.0/12, 0}, {0, 1}
        );
        auto scale1 = Scale::fromAffine(A, 440.0, 24, 12);
        auto scale2 = Scale::fromAffine(A, 440.0, 24, 12);
        auto scale3 = Scale::fromAffine(A, 440.0, 24, 12);
        
        // Temper with different pitch sets
        scale1.temperToPitchSet(etPitchSet);
        scale2.temperToPitchSet(jiPitchSet);
        scale3.temperToPitchSet(hsPitchSet);
        
        // Should have different tempered pitches
        auto& nodes1 = scale1.getNodes();
        auto& nodes2 = scale2.getNodes();
        auto& nodes3 = scale3.getNodes();
        
        bool foundDifference = false;
        for (size_t i = 0; i < nodes1.size(); ++i) {
            if (nodes1[i].isTempered && nodes2[i].isTempered) {
                if (std::abs(nodes1[i].temperedPitch.log2fr - nodes2[i].temperedPitch.log2fr) > 1e-10) {
                    foundDifference = true;
                    break;
                }
            }
        }
        REQUIRE(foundDifference);
    }
}

TEST_CASE("Coordinate mapping between MOS systems", "[integration]") {
    MOS mos1 = MOS::fromParams(5, 2, 1, 1.0, 0.585);
    MOS mos2 = MOS::fromParams(3, 2, 1, 1.0, 0.6);
    
    SECTION("Coordinate mapping preserves relationships") {
        Vector2i coord(2, 1);
        Vector2i mapped = mos2.mapFromMOS(mos1, coord);
        
        // Mapping should produce valid coordinates
        REQUIRE(std::abs(mapped.x) <= 100);  // Reasonable bounds
        REQUIRE(std::abs(mapped.y) <= 100);
        
        // Identity mapping should work
        Vector2i identity = mos1.mapFromMOS(mos1, coord);
        REQUIRE(identity.x == coord.x);
        REQUIRE(identity.y == coord.y);
    }
}

TEST_CASE("Complete workflow: native example simulation", "[integration]") {
    // Simulate key parts of the native example workflow
    
    SECTION("Scale generation and printing workflow") {
        // Create affine transform scale
        auto A = affineFromThreeDots(
            {0, 0}, {3, 1}, {5, 2},
            {0, 3.0/24}, {.585, 5.0/24}, {1.0, 3.0/24}
        );
        auto scale = Scale::fromAffine(A, 261.63, 128, 60);
        
        // Generate MOS
        MOS mos = MOS::fromG(3, 4, 0.585, 1.0, 1);
        
        // Create pitch sets
        auto etPitchSet = generateETPitchSet(12, 1.0);
        auto primes = generateDefaultPrimeList(4);
        primes.erase(primes.begin() + 2);  // Remove one prime like in example
        auto jiPitchSet = generateJIPitchSet(primes, 240);
        
        // Test the complete workflow
        auto mosScale = mos.generateScaleFromMOS(261.63, 128, 60);
        
        // Basic validation
        REQUIRE(scale.getNodes().size() == 128);
        REQUIRE(mosScale.getNodes().size() == 128);
        REQUIRE(etPitchSet.size() == 13); // Includes octave
        REQUIRE(jiPitchSet.size() > 0);
        
        // Test retuning operations
        mos.retuneOnePoint({0, 0}, 1.0);
        mos.retuneScaleWithMOS(mosScale, 261.63);
        
        mos.retuneTwoPoints({0, 0}, {mos.a, mos.b}, 1.1);
        mos.retuneScaleWithMOS(mosScale, 261.63);
        
        mos.retuneThreePoints({0, 0}, {mos.a, mos.b}, mos.v_gen, 7.0/12);
        mos.retuneScaleWithMOS(mosScale, 261.63);
        
        // Test tempering
        mos.retuneZeroPoint();
        mos.base_scale.temperToPitchSet(jiPitchSet);
        mos.retuneScaleWithMOS(mosScale, 261.63);
        
        // All operations should complete successfully
        REQUIRE(mosScale.getNodes().size() == 128);
    }
}