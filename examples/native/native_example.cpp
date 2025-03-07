#include <scalatrix/scale.hpp>
#include <scalatrix/params.hpp>
#include <iostream>

int main() {
    // Create a scale with base frequency 261.63 Hz (Middle C)
    scalatrix::Scale scale(261.63);

    // Define an affine transform using MOS parameters (e.g., 5L 2s, mode 2, octave, ratio 1.5)

    auto M = scalatrix::find_affine_transform({0, 0}, {2, 2}, {3, 5}, {1, 0}, {3, 2}, {4, 5});

    // print elements of M
    std::cout << "M = [[" << M.a << ", " << M.b << "], [" << M.c << ", " << M.d << "]]" << M.tx << ", " << M.ty << "\n";



    //auto [M, y0] = scalatrix::affineFromMOSParams(5, 2, 2, 2.0, 1.5);

    // Generate a 128-note scale with root at index 60
    scale.generatePath(M, 128, 60, 0.5);

    // Print some nodes around the root
    std::cout << "Generated scale with " << scale.getNodes().size() << " nodes:\n";

    scale.print(58, 5);  // Print nodes 58-62

    return 0;
}

