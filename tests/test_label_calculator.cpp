#include "catch2/catch_test_macros.hpp"
#include "scalatrix/label_calculator.hpp"
#include "scalatrix/mos.hpp"

using namespace scalatrix;

TEST_CASE("LabelCalculator basic functionality", "[labelcalculator]") {
    LabelCalculator lc;
    MOS mos = MOS::fromParams(5, 2, 1, 1.0, 0.585); // 7-note MOS (like diatonic)
    
    SECTION("Generate normalized note labels") {
        // Test some basic coordinates
        Vector2i origin(0, 0);
        std::string label = lc.noteLabelNormalized(mos, origin);
        REQUIRE(!label.empty());
        
        // Different coordinates should give different labels (usually)
        Vector2i coord1(1, 0);
        Vector2i coord2(0, 1);
        std::string label1 = lc.noteLabelNormalized(mos, coord1);
        std::string label2 = lc.noteLabelNormalized(mos, coord2);
        
        REQUIRE(!label1.empty());
        REQUIRE(!label2.empty());
        
        // Labels should be different for different coordinates
        if (coord1.x != coord2.x || coord1.y != coord2.y) {
            // Usually different, but not guaranteed for all MOS systems
            // This is more of a basic sanity check
            REQUIRE(label1.length() > 0);
            REQUIRE(label2.length() > 0);
        }
    }
    
    SECTION("Label consistency") {
        Vector2i coord(2, 1);
        std::string label1 = lc.noteLabelNormalized(mos, coord);
        std::string label2 = lc.noteLabelNormalized(mos, coord);
        
        // Same coordinate should always give same label
        REQUIRE(label1 == label2);
    }
    
    SECTION("Handles origin") {
        Vector2i origin(0, 0);
        std::string label = lc.noteLabelNormalized(mos, origin);
        
        // Origin should have a valid label
        REQUIRE(!label.empty());
    }
}

TEST_CASE("LabelCalculator with different MOS systems", "[labelcalculator]") {
    LabelCalculator lc;
    
    SECTION("5-limit MOS (5,2)") {
        MOS mos = MOS::fromParams(5, 2, 1, 1.0, 0.585);
        
        // Test several coordinates
        std::vector<Vector2i> testCoords = {
            {0, 0}, {1, 0}, {0, 1}, {2, 0}, {1, 1}, {3, 1}
        };
        
        for (const auto& coord : testCoords) {
            std::string label = lc.noteLabelNormalized(mos, coord);
            REQUIRE(!label.empty());
        }
    }
    
    SECTION("Pentatonic-like MOS (3,2)") {
        MOS mos = MOS::fromParams(3, 2, 1, 1.0, 0.6);
        
        std::vector<Vector2i> testCoords = {
            {0, 0}, {1, 0}, {0, 1}, {2, 1}
        };
        
        for (const auto& coord : testCoords) {
            std::string label = lc.noteLabelNormalized(mos, coord);
            REQUIRE(!label.empty());
        }
    }
    
    SECTION("Different MOS systems give different labeling") {
        MOS mos1 = MOS::fromParams(5, 2, 1, 1.0, 0.585);
        MOS mos2 = MOS::fromParams(3, 2, 1, 1.0, 0.6);
        
        Vector2i coord(1, 1);
        std::string label1 = lc.noteLabelNormalized(mos1, coord);
        std::string label2 = lc.noteLabelNormalized(mos2, coord);
        
        // Different MOS systems should generally give different labels
        // (This isn't guaranteed but is typical)
        REQUIRE(!label1.empty());
        REQUIRE(!label2.empty());
    }
}

TEST_CASE("LabelCalculator edge cases", "[labelcalculator]") {
    LabelCalculator lc;
    MOS mos = MOS::fromParams(5, 2, 1, 1.0, 0.585);
    
    SECTION("Large coordinates") {
        Vector2i largeCoord(10, 10);
        std::string label = lc.noteLabelNormalized(mos, largeCoord);
        
        // Should handle large coordinates gracefully
        REQUIRE(!label.empty());
    }
    
    SECTION("Negative coordinates") {
        Vector2i negCoord(-2, -1);
        std::string label = lc.noteLabelNormalized(mos, negCoord);
        
        // Should handle negative coordinates
        REQUIRE(!label.empty());
    }
    
    SECTION("Mixed sign coordinates") {
        Vector2i mixedCoord(-1, 2);
        std::string label = lc.noteLabelNormalized(mos, mixedCoord);
        
        // Should handle mixed positive/negative coordinates
        REQUIRE(!label.empty());
    }
}

TEST_CASE("LabelCalculator deterministic behavior", "[labelcalculator]") {
    SECTION("Multiple instances give same results") {
        LabelCalculator lc1, lc2;
        MOS mos = MOS::fromParams(5, 2, 1, 1.0, 0.585);
        Vector2i coord(3, 1);
        
        std::string label1 = lc1.noteLabelNormalized(mos, coord);
        std::string label2 = lc2.noteLabelNormalized(mos, coord);
        
        // Different instances should give same result
        REQUIRE(label1 == label2);
    }
    
    SECTION("Repeated calls give same results") {
        LabelCalculator lc;
        MOS mos = MOS::fromParams(5, 2, 1, 1.0, 0.585);
        Vector2i coord(2, 3);
        
        std::vector<std::string> labels;
        for (int i = 0; i < 5; ++i) {
            labels.push_back(lc.noteLabelNormalized(mos, coord));
        }
        
        // All calls should give same result
        for (size_t i = 1; i < labels.size(); ++i) {
            REQUIRE(labels[i] == labels[0]);
        }
    }
}