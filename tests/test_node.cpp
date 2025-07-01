#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include "scalatrix/node.hpp"
#include "scalatrix/label_calculator.hpp"

using namespace scalatrix;
using Catch::Matchers::WithinAbs;

TEST_CASE("Node deviation label functionality", "[node]") {
    SECTION("Compare tuning_coord vs tempered pitch with closest") {
        Node node;
        node.tuning_coord = {0.51, 0.0}; // Actual pitch at tuning_coord
        node.temperedPitch = {"C#", 0.5};    // Tempered pitch
        node.closestPitch = {"C#", 0.52};   // Closest pitch in pitch set
        
        // Deviation from tuning_coord to closest (compareWithTempered=false)
        // Deviation: 1200 * (0.51 - 0.52) = -12.0 cents
        std::string tuningLabel = LabelCalculator::deviationLabel(node, 0.1, false);
        REQUIRE(tuningLabel == "C#-12.0ct");
        
        // Deviation from tempered pitch to closest (compareWithTempered=true)
        // Deviation: 1200 * (0.5 - 0.52) = -24.0 cents
        std::string temperedLabel = LabelCalculator::deviationLabel(node, 0.1, true);
        REQUIRE(temperedLabel == "C#-24.0ct");
    }
    
    SECTION("Empty labels when no reference pitch") {
        Node node;
        node.tuning_coord = {0.5, 0.0};
        node.closestPitch = {"", 0.0}; // Empty label
        
        REQUIRE(LabelCalculator::deviationLabel(node).empty());
        REQUIRE(LabelCalculator::deviationLabel(node, 0.1, false).empty());
        REQUIRE(LabelCalculator::deviationLabel(node, 0.1, true).empty());
    }
}