#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include "scalatrix/node.hpp"

using namespace scalatrix;
using Catch::Matchers::WithinAbs;

TEST_CASE("Node construction and basic properties", "[node]") {
    SECTION("Default construction") {
        Node node;
        
        REQUIRE(node.getNaturalCoord().x == 0);
        REQUIRE(node.getNaturalCoord().y == 0);
        REQUIRE(node.getTuningCoord().x == 0.0);
        REQUIRE(node.getTuningCoord().y == 0.0);
        REQUIRE(node.getPitch() == 0.0);
        REQUIRE(node.getIsTempered() == false);
        REQUIRE(node.getTemperedPitch().label.empty());
    }
    
    SECTION("Construction with parameters") {
        Vector2i natural(3, 2);
        Vector2d tuning(0.5, 0.3);
        double freq = 440.0;
        
        Node node(natural, tuning, freq);
        
        REQUIRE(node.getNaturalCoord().x == 3);
        REQUIRE(node.getNaturalCoord().y == 2);
        REQUIRE_THAT(node.getTuningCoord().x, WithinAbs(0.5, 1e-10));
        REQUIRE_THAT(node.getTuningCoord().y, WithinAbs(0.3, 1e-10));
        REQUIRE_THAT(node.getPitch(), WithinAbs(440.0, 1e-10));
        REQUIRE(node.getIsTempered() == false);
    }
}

TEST_CASE("Node backward compatibility", "[node]") {
    SECTION("Legacy field access") {
        Node node;
        
        // Test that legacy field access works
        node.natural_coord = {5, 3};
        node.tuning_coord = {0.7, 0.2};
        node.pitch = 880.0;
        node.isTempered = true;
        
        // Verify the getters return the same values
        REQUIRE(node.getNaturalCoord().x == 5);
        REQUIRE(node.getNaturalCoord().y == 3);
        REQUIRE_THAT(node.getTuningCoord().x, WithinAbs(0.7, 1e-10));
        REQUIRE_THAT(node.getTuningCoord().y, WithinAbs(0.2, 1e-10));
        REQUIRE_THAT(node.getPitch(), WithinAbs(880.0, 1e-10));
        REQUIRE(node.getIsTempered() == true);
    }
    
    SECTION("Mixed access patterns") {
        Node node;
        
        // Set via setters
        node.setNaturalCoord({7, 4});
        node.setTuningCoord({0.9, 0.1});
        node.setPitch(330.0);
        
        // Read via legacy fields
        REQUIRE(node.natural_coord.x == 7);
        REQUIRE(node.natural_coord.y == 4);
        REQUIRE_THAT(node.tuning_coord.x, WithinAbs(0.9, 1e-10));
        REQUIRE_THAT(node.tuning_coord.y, WithinAbs(0.1, 1e-10));
        REQUIRE_THAT(node.pitch, WithinAbs(330.0, 1e-10));
    }
}

TEST_CASE("Node tempering functionality", "[node]") {
    SECTION("Setting tempered pitch") {
        Node node;
        PitchSetPitch temperedPitch = {"A4", 0.75};
        
        node.setTemperedPitch(temperedPitch);
        
        REQUIRE(node.getIsTempered() == true);
        REQUIRE(node.getTemperedPitch().label == "A4");
        REQUIRE_THAT(node.getTemperedPitch().log2fr, WithinAbs(0.75, 1e-10));
    }
    
    SECTION("Clearing tempered pitch") {
        Node node;
        PitchSetPitch temperedPitch = {"A4", 0.75};
        
        node.setTemperedPitch(temperedPitch);
        REQUIRE(node.getIsTempered() == true);
        
        node.clearTempered();
        REQUIRE(node.getIsTempered() == false);
        REQUIRE(node.getTemperedPitch().label.empty());
    }
}

TEST_CASE("Node deviation label functionality", "[node]") {
    SECTION("No tempered pitch returns empty string") {
        Node node;
        node.setTuningCoord({0.5, 0.0});
        
        std::string label = node.deviationLabel();
        REQUIRE(label.empty());
    }
    
    SECTION("No deviation shows plain label") {
        Node node;
        node.setTuningCoord({0.5, 0.0});
        node.setTemperedPitch({"C", 0.5});
        
        std::string label = node.deviationLabel(0.1, false);
        REQUIRE(label == "C");
    }
    
    SECTION("Small positive deviation") {
        Node node;
        node.setTuningCoord({0.51, 0.0}); // 12 cents sharp
        node.setTemperedPitch({"C", 0.5});
        
        std::string label = node.deviationLabel(0.1, false);
        REQUIRE(label == "C+12.0ct");
    }
    
    SECTION("Small negative deviation") {
        Node node;
        node.setTuningCoord({0.495, 0.0}); // 6 cents flat
        node.setTemperedPitch({"C", 0.5});
        
        std::string label = node.deviationLabel(0.1, false);
        REQUIRE(label == "C-6.0ct");
    }
    
    SECTION("Force exact label ignores deviation") {
        Node node;
        node.setTuningCoord({0.6, 0.0}); // 120 cents sharp
        node.setTemperedPitch({"C", 0.5});
        
        std::string label = node.deviationLabel(0.1, true);
        REQUIRE(label == "C");
    }
}

TEST_CASE("Node comparison operators", "[node]") {
    SECTION("Less than operator based on tuning_coord.x") {
        Node node1, node2;
        node1.setTuningCoord({0.3, 0.0});
        node2.setTuningCoord({0.7, 0.0});
        
        REQUIRE(node1 < node2);
        REQUIRE(!(node2 < node1));
    }
    
    SECTION("Equal tuning coordinates") {
        Node node1, node2;
        node1.setTuningCoord({0.5, 0.0});
        node2.setTuningCoord({0.5, 1.0}); // Different y, same x
        
        REQUIRE(!(node1 < node2));
        REQUIRE(!(node2 < node1));
    }
}

TEST_CASE("Node copy and assignment", "[node]") {
    SECTION("Copy constructor") {
        Node original;
        original.setNaturalCoord({3, 2});
        original.setTuningCoord({0.5, 0.3});
        original.setPitch(440.0);
        original.setTemperedPitch({"A4", 0.75});
        
        Node copy(original);
        
        REQUIRE(copy.getNaturalCoord().x == 3);
        REQUIRE(copy.getNaturalCoord().y == 2);
        REQUIRE_THAT(copy.getTuningCoord().x, WithinAbs(0.5, 1e-10));
        REQUIRE_THAT(copy.getTuningCoord().y, WithinAbs(0.3, 1e-10));
        REQUIRE_THAT(copy.getPitch(), WithinAbs(440.0, 1e-10));
        REQUIRE(copy.getIsTempered() == true);
        REQUIRE(copy.getTemperedPitch().label == "A4");
    }
    
    SECTION("Assignment operator") {
        Node original, assigned;
        original.setNaturalCoord({3, 2});
        original.setTuningCoord({0.5, 0.3});
        original.setPitch(440.0);
        original.setTemperedPitch({"A4", 0.75});
        
        assigned = original;
        
        REQUIRE(assigned.getNaturalCoord().x == 3);
        REQUIRE(assigned.getNaturalCoord().y == 2);
        REQUIRE_THAT(assigned.getTuningCoord().x, WithinAbs(0.5, 1e-10));
        REQUIRE_THAT(assigned.getTuningCoord().y, WithinAbs(0.3, 1e-10));
        REQUIRE_THAT(assigned.getPitch(), WithinAbs(440.0, 1e-10));
        REQUIRE(assigned.getIsTempered() == true);
        REQUIRE(assigned.getTemperedPitch().label == "A4");
    }
}