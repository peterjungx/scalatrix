#include <scalatrix.hpp>
#include <iostream>

using namespace scalatrix;

int main() {
    // Create a scale with base frequency 261.63 Hz (Middle C)
    auto A = affineFromThreeDots(
        {0,   0},     {3,     1},      {5,    2},       
        {0,   3.0/24},  {.585,  5.0/24},   {1.0,  3.0/24}    
    );
    auto scale = Scale::fromAffine(A, 261.63, 128, 60);

    // Print some nodes around the root
    std::cout << "Generated scale with " << scale.getNodes().size() << " nodes:\n";

    scale.print(58, 5);  // Print nodes 58-62


    // Test MOS scale creation

    MOS mos = MOS::fromG(3, 0, 0.585, 1.0, 1);
    // print the params
    std::cout << "MOS params: a=" << mos.a << ", b=" << mos.b << ", n=" << mos.n << ", a0=" << mos.a0 << ", b0=" << mos.b0 << ", n0=" << mos.n0 << ", mode=" << mos.mode << ", repetitions=" << mos.repetitions << ", depth=" << mos.depth << ", equave=" << mos.equave << ", period=" << mos.period << ", generator=" << mos.generator << "\n";
    // print angleStd
    std::cout << "AngleStd: " << M_2_PI * mos.angleStd() << "\n";

    // print path
    std::cout << "Path: ";
    for (int i = 0; i < mos.path.size(); i++) {
        std::cout << mos.path[i] << " ";
    }
    std::cout << "\n";

    // print angle
    std::cout << "Angle: " << mos.angle() << "\n";
    std::cout << "Angle + atan2(mos.a0, mos.b0): " << mos.angle() + atan2(mos.a0, mos.b0) << "\n";
    std::cout << "Angle + atan2(mos.b0, mos.a0): " << mos.angle() + atan2(mos.b0, mos.a0) << "\n";
    std::cout << "atan2(mos.a0, mos.b0): " << atan2(mos.a0, mos.b0) << "\n";
    std::cout << "atan2(mos.b0, mos.a0): " << atan2(mos.b0, mos.a0) << "\n";

    // print v_gen
    std::cout << "v_gen: x=" << mos.v_gen.x << ", y=" << mos.v_gen.y << "\n";

    // print implied affine
    std::cout << "Implied affine: a=" << mos.impliedAffine.a << ", b=" << mos.impliedAffine.b << ", c=" << mos.impliedAffine.c << ", d=" << mos.impliedAffine.d << ", tx=" << mos.impliedAffine.tx << ", ty=" << mos.impliedAffine.ty << "\n";

    // print mos transform
    std::cout << "MOS transform: a=" << mos.mosTransform.a << ", b=" << mos.mosTransform.b << ", c=" << mos.mosTransform.c << ", d=" << mos.mosTransform.d << ", tx=" << mos.mosTransform.tx << ", ty=" << mos.mosTransform.ty << "\n";

    // print the scale
    std::cout << "Generated scale with " << mos.base_scale.getNodes().size() << " nodes:\n";
    mos.base_scale.print(0, mos.n+1);  // Print nodes 58-62


    // print gFromAngle
    std::cout << "gFromAngle: " << mos.gFromAngle(mos.angle()) << "\n";
    return 0;
}

