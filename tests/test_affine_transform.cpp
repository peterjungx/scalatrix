#define CATCH_CONFIG_MAIN
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include "scalatrix/params.hpp"
#include <cmath>

using namespace scalatrix;
using Catch::Matchers::WithinAbs;

TEST_CASE("affineFromThreeDots basic transformation", "[affine]") {
    // Test case 1: Identity transformation
    Vector2d a1(0, 0), a2(1, 0), a3(0, 1);
    Vector2d b1(0, 0), b2(1, 0), b3(0, 1);
    
    AffineTransform result = affineFromThreeDots(a1, a2, a3, b1, b2, b3);
    
    // Should be identity transformation
    REQUIRE_THAT(result.a, WithinAbs(1.0, 1e-10));
    REQUIRE_THAT(result.b, WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(result.c, WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(result.d, WithinAbs(1.0, 1e-10));
    REQUIRE_THAT(result.tx, WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(result.ty, WithinAbs(0.0, 1e-10));
}

TEST_CASE("affineFromThreeDots translation", "[affine]") {
    // Test case 2: Pure translation
    Vector2d a1(0, 0), a2(1, 0), a3(0, 1);
    Vector2d b1(2, 3), b2(3, 3), b3(2, 4);
    
    AffineTransform result = affineFromThreeDots(a1, a2, a3, b1, b2, b3);
    
    // Should be translation by (2, 3)
    REQUIRE_THAT(result.a, WithinAbs(1.0, 1e-10));
    REQUIRE_THAT(result.b, WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(result.c, WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(result.d, WithinAbs(1.0, 1e-10));
    REQUIRE_THAT(result.tx, WithinAbs(2.0, 1e-10));
    REQUIRE_THAT(result.ty, WithinAbs(3.0, 1e-10));
}

TEST_CASE("affineFromThreeDots scaling", "[affine]") {
    // Test case 3: Scaling
    Vector2d a1(0, 0), a2(1, 0), a3(0, 1);
    Vector2d b1(0, 0), b2(2, 0), b3(0, 3);
    
    AffineTransform result = affineFromThreeDots(a1, a2, a3, b1, b2, b3);
    
    // Should scale x by 2 and y by 3
    REQUIRE_THAT(result.a, WithinAbs(2.0, 1e-10));
    REQUIRE_THAT(result.b, WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(result.c, WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(result.d, WithinAbs(3.0, 1e-10));
    REQUIRE_THAT(result.tx, WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(result.ty, WithinAbs(0.0, 1e-10));
}

TEST_CASE("affineFromThreeDots rotation 90 degrees", "[affine]") {
    // Test case 4: 90 degree rotation
    Vector2d a1(0, 0), a2(1, 0), a3(0, 1);
    Vector2d b1(0, 0), b2(0, 1), b3(-1, 0);
    
    AffineTransform result = affineFromThreeDots(a1, a2, a3, b1, b2, b3);
    
    // Should be 90 degree rotation
    REQUIRE_THAT(result.a, WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(result.b, WithinAbs(-1.0, 1e-10));
    REQUIRE_THAT(result.c, WithinAbs(1.0, 1e-10));
    REQUIRE_THAT(result.d, WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(result.tx, WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(result.ty, WithinAbs(0.0, 1e-10));
}

TEST_CASE("affineFromThreeDots complex transformation", "[affine]") {
    // Test case 5: Complex transformation
    Vector2d a1(1, 1), a2(3, 2), a3(2, 4);
    Vector2d b1(5, 5), b2(8, 4), b3(6, 7);
    
    AffineTransform result = affineFromThreeDots(a1, a2, a3, b1, b2, b3);
    
    // Verify the transformation works correctly by applying it to the input points
    Vector2d check1 = result.apply(a1);
    Vector2d check2 = result.apply(a2);
    Vector2d check3 = result.apply(a3);
    
    REQUIRE_THAT(check1.x, WithinAbs(b1.x, 1e-10));
    REQUIRE_THAT(check1.y, WithinAbs(b1.y, 1e-10));
    REQUIRE_THAT(check2.x, WithinAbs(b2.x, 1e-10));
    REQUIRE_THAT(check2.y, WithinAbs(b2.y, 1e-10));
    REQUIRE_THAT(check3.x, WithinAbs(b3.x, 1e-10));
    REQUIRE_THAT(check3.y, WithinAbs(b3.y, 1e-10));
}

TEST_CASE("affineFromThreeDots shear transformation", "[affine]") {
    // Test case 6: Shear transformation
    Vector2d a1(0, 0), a2(1, 0), a3(0, 1);
    Vector2d b1(0, 0), b2(1, 1), b3(0, 1);
    
    AffineTransform result = affineFromThreeDots(a1, a2, a3, b1, b2, b3);
    
    // Should be horizontal shear
    REQUIRE_THAT(result.a, WithinAbs(1.0, 1e-10));
    REQUIRE_THAT(result.b, WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(result.c, WithinAbs(1.0, 1e-10));
    REQUIRE_THAT(result.d, WithinAbs(1.0, 1e-10));
    REQUIRE_THAT(result.tx, WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(result.ty, WithinAbs(0.0, 1e-10));
}