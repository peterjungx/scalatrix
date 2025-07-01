#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include "scalatrix/node.hpp"

using namespace scalatrix;
using Catch::Matchers::WithinAbs;

TEST_CASE("Node deviation label functionality", "[node]") {
    SECTION("Different tempered vs closest pitch") {
        Node node;
        node.tuning_coord = {0.51, 0.0}; // Actual pitch
        node.temperedPitch = {"C", 0.5};    // Tempered to C
        node.closestPitch = {"C#", 0.52};   // But closest is C#
        
        // Deviation from tempered pitch (default)
        std::string temperedLabel = node.deviationLabel(0.1, false, DeviationReference::TEMPERED);
        REQUIRE(temperedLabel == "C+12.0ct");
        
        // Deviation from closest pitch
        std::string closestLabel = node.deviationLabel(0.1, false, DeviationReference::CLOSEST);
        REQUIRE(closestLabel == "C#-12.0ct");
    }
    
    SECTION("Empty labels when no reference pitch") {
        Node node;
        node.tuning_coord = {0.5, 0.0};
        
        REQUIRE(node.deviationLabel().empty());
        REQUIRE(node.deviationLabel(0.1, false, DeviationReference::CLOSEST).empty());
    }
}