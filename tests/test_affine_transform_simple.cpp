#include <iostream>
#include <iomanip>
#include <cmath>
#include "scalatrix/params.hpp"

using namespace scalatrix;

// Helper function to check if two doubles are approximately equal
bool approxEqual(double a, double b, double epsilon = 1e-10) {
    return std::abs(a - b) < epsilon;
}

void printTransform(const AffineTransform& t, const std::string& name) {
    std::cout << name << ":\n";
    std::cout << "  a = " << std::setprecision(15) << t.a << "\n";
    std::cout << "  b = " << std::setprecision(15) << t.b << "\n";
    std::cout << "  c = " << std::setprecision(15) << t.c << "\n";
    std::cout << "  d = " << std::setprecision(15) << t.d << "\n";
    std::cout << "  tx = " << std::setprecision(15) << t.tx << "\n";
    std::cout << "  ty = " << std::setprecision(15) << t.ty << "\n\n";
}

void testCase(const std::string& name,
              const Vector2d& a1, const Vector2d& a2, const Vector2d& a3,
              const Vector2d& b1, const Vector2d& b2, const Vector2d& b3) {
    
    std::cout << "=== " << name << " ===\n";
    AffineTransform result = affineFromThreeDots(a1, a2, a3, b1, b2, b3);
    printTransform(result, "Result");
    
    // Verify by applying transformation
    Vector2d check1 = result.apply(a1);
    Vector2d check2 = result.apply(a2);
    Vector2d check3 = result.apply(a3);
    
    std::cout << "Verification:\n";
    std::cout << "  a1(" << a1.x << "," << a1.y << ") -> (" 
              << check1.x << "," << check1.y << ") expected (" 
              << b1.x << "," << b1.y << ")\n";
    std::cout << "  a2(" << a2.x << "," << a2.y << ") -> (" 
              << check2.x << "," << check2.y << ") expected (" 
              << b2.x << "," << b2.y << ")\n";
    std::cout << "  a3(" << a3.x << "," << a3.y << ") -> (" 
              << check3.x << "," << check3.y << ") expected (" 
              << b3.x << "," << b3.y << ")\n";
    
    bool success = approxEqual(check1.x, b1.x) && approxEqual(check1.y, b1.y) &&
                   approxEqual(check2.x, b2.x) && approxEqual(check2.y, b2.y) &&
                   approxEqual(check3.x, b3.x) && approxEqual(check3.y, b3.y);
    
    std::cout << "  Status: " << (success ? "PASS" : "FAIL") << "\n\n";
}

int main() {
    // Test 1: Identity
    testCase("Identity transformation",
             Vector2d(0, 0), Vector2d(1, 0), Vector2d(0, 1),
             Vector2d(0, 0), Vector2d(1, 0), Vector2d(0, 1));
    
    // Test 2: Translation
    testCase("Translation",
             Vector2d(0, 0), Vector2d(1, 0), Vector2d(0, 1),
             Vector2d(2, 3), Vector2d(3, 3), Vector2d(2, 4));
    
    // Test 3: Scaling
    testCase("Scaling",
             Vector2d(0, 0), Vector2d(1, 0), Vector2d(0, 1),
             Vector2d(0, 0), Vector2d(2, 0), Vector2d(0, 3));
    
    // Test 4: 90 degree rotation
    testCase("90 degree rotation",
             Vector2d(0, 0), Vector2d(1, 0), Vector2d(0, 1),
             Vector2d(0, 0), Vector2d(0, 1), Vector2d(-1, 0));
    
    // Test 5: Complex transformation
    testCase("Complex transformation",
             Vector2d(1, 1), Vector2d(3, 2), Vector2d(2, 4),
             Vector2d(5, 5), Vector2d(8, 4), Vector2d(6, 7));
    
    // Test 6: Shear
    testCase("Shear transformation",
             Vector2d(0, 0), Vector2d(1, 0), Vector2d(0, 1),
             Vector2d(0, 0), Vector2d(1, 1), Vector2d(0, 1));
    
    return 0;
}