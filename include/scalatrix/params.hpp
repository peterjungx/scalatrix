#ifndef SCALATRIX_PARAMS_HPP
#define SCALATRIX_PARAMS_HPP

#include "affine_transform.hpp"
#include <utility>

namespace scalatrix {

struct MOSParams {
    int a, b, m;
    double e, r;
};


AffineTransform affineFromThreeDots(
    const Vector2d& a1, const Vector2d& a2, const Vector2d& a3,
    const Vector2d& b1, const Vector2d& b2, const Vector2d& b3
);


AffineTransform affineFromMOSParams(int a, int b, int m, double e, double r);


} // namespace scalatrix

#endif // SCALATRIX_PARAMS_HPP
