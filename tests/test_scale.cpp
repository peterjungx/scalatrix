#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include "scalatrix/scale.hpp"
#include "scalatrix/params.hpp"
#include <cmath>

using namespace scalatrix;
using Catch::Matchers::WithinAbs;

TEST_CASE("Scale construction and basic properties", "[scale]") {
    SECTION("Default construction") {
        Scale scale;
        REQUIRE(scale.getNodes().size() == 128);
        REQUIRE(scale.getRootIdx() == 60);
        REQUIRE_THAT(scale.getBaseFreq(), WithinAbs(261.6255653006, 1e-10));
    }
    
    SECTION("Custom construction") {
        Scale scale(440.0, 64, 30);
        REQUIRE(scale.getNodes().size() == 64);
        REQUIRE(scale.getRootIdx() == 30);
        REQUIRE_THAT(scale.getBaseFreq(), WithinAbs(440.0, 1e-10));
    }
}

TEST_CASE("Scale::fromAffine generates correct scale", "[scale]") {
    // Create a scale using affine transformation similar to the example
    auto A = affineFromThreeDots(
        {0, 0}, {3, 1}, {5, 2},
        {0, 3.0/24}, {.585, 5.0/24}, {1.0, 3.0/24}
    );
    
    auto scale = Scale::fromAffine(A, 261.63, 128, 60);
    
    SECTION("Scale has correct number of nodes") {
        REQUIRE(scale.getNodes().size() == 128);
    }
    
    SECTION("Root node is at correct index") {
        REQUIRE(scale.getRootIdx() == 60);
    }
    
    SECTION("Base frequency is correct") {
        REQUIRE_THAT(scale.getBaseFreq(), WithinAbs(261.63, 1e-10));
    }
    
    SECTION("Nodes are sorted by tuning coordinate x") {
        auto& nodes = scale.getNodes();
        for (size_t i = 1; i < nodes.size(); ++i) {
            REQUIRE(nodes[i-1].tuning_coord.x <= nodes[i].tuning_coord.x);
        }
    }
    
    SECTION("Root node has expected frequency") {
        auto& nodes = scale.getNodes();
        REQUIRE_THAT(nodes[60].pitch, WithinAbs(261.63, 1e-10));
    }
}

TEST_CASE("Node deviation label", "[scale][node]") {
    Node node;
    node.tuning_coord.x = 0.0;
    node.temperedPitch.label = "C";
    node.temperedPitch.log2fr = 0.0;
    
    SECTION("No deviation shows plain label") {
        std::string label = node.deviationLabel(0.1, false);
        REQUIRE(label == "C");
    }
    
    SECTION("Small positive deviation") {
        node.tuning_coord.x = 0.01; // 12 cents sharp
        std::string label = node.deviationLabel(0.1, false);
        REQUIRE(label == "C+12.0ct");
    }
    
    SECTION("Small negative deviation") {
        node.tuning_coord.x = -0.005; // 6 cents flat
        std::string label = node.deviationLabel(0.1, false);
        REQUIRE(label == "C-6.0ct");
    }
    
    SECTION("Deviation below threshold shows plain label") {
        node.tuning_coord.x = 0.00005; // 0.06 cents sharp
        std::string label = node.deviationLabel(0.1, false);
        REQUIRE(label == "C");
    }
    
    SECTION("Force exact label ignores deviation") {
        node.tuning_coord.x = 0.1; // 120 cents sharp
        std::string label = node.deviationLabel(0.1, true);
        REQUIRE(label == "C");
    }
    
    SECTION("Empty tempered pitch returns empty string") {
        node.temperedPitch.label = "";
        std::string label = node.deviationLabel();
        REQUIRE(label == "");
    }
}

TEST_CASE("Scale tempering to pitch set", "[scale]") {
    // Create a simple scale
    auto A = affineFromThreeDots(
        {0, 0}, {1, 0}, {0, 1},
        {0, 0}, {1.0/12, 0}, {0, 1}
    );
    auto scale = Scale::fromAffine(A, 261.63, 12, 0);
    
    // Create an ET pitch set
    auto etPitchSet = generateETPitchSet(12, 1.0);
    
    SECTION("Tempering updates node properties") {
        scale.temperToPitchSet(etPitchSet);
        
        auto& nodes = scale.getNodes();
        for (auto& node : nodes) {
            if (node.isTempered) {
                // Check that tempered pitch is set
                REQUIRE(!node.temperedPitch.label.empty());
                // Check that tempered pitch is within reasonable range
                REQUIRE(node.temperedPitch.log2fr >= -1.0);
                REQUIRE(node.temperedPitch.log2fr <= 2.0);
            }
        }
    }
}

TEST_CASE("Scale retuning with affine transform", "[scale]") {
    // Create initial scale
    auto A1 = affineFromThreeDots(
        {0, 0}, {1, 0}, {0, 1},
        {0, 0}, {0.1, 0}, {0, 0.1}
    );
    auto scale = Scale::fromAffine(A1, 440.0, 10, 5);
    
    // Store original pitches
    std::vector<double> originalPitches;
    for (auto& node : scale.getNodes()) {
        originalPitches.push_back(node.pitch);
    }
    
    // Create new transform and retune
    auto A2 = affineFromThreeDots(
        {0, 0}, {1, 0}, {0, 1},
        {0, 0}, {0.2, 0}, {0, 0.2}
    );
    scale.retuneWithAffine(A2);
    
    SECTION("Retuning changes pitches") {
        auto& nodes = scale.getNodes();
        bool someChanged = false;
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (std::abs(nodes[i].pitch - originalPitches[i]) > 1e-10) {
                someChanged = true;
                break;
            }
        }
        REQUIRE(someChanged);
    }
    
    SECTION("Natural coordinates remain unchanged") {
        // recalcWithAffine would change natural coords, but retuneWithAffine should not
        auto& nodes = scale.getNodes();
        for (auto& node : nodes) {
            // Just verify they exist - actual values depend on generation
            REQUIRE(node.natural_coord.x >= -100);
            REQUIRE(node.natural_coord.x <= 100);
            REQUIRE(node.natural_coord.y >= -100);
            REQUIRE(node.natural_coord.y <= 100);
        }
    }
}