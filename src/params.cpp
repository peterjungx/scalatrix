#include "scalatrix/params.hpp"
#include <Eigen/Dense>
#include <cassert>

namespace scalatrix {


AffineTransform affineFromThreeDots(
    const Vector2d& a1, const Vector2d& a2, const Vector2d& a3,
    const Vector2d& b1, const Vector2d& b2, const Vector2d& b3) {
    Eigen::Matrix<double, 6, 6> M;
    M << a1.x, a1.y, 1, 0, 0, 0,
         0, 0, 0, a1.x, a1.y, 1,
         a2.x, a2.y, 1, 0, 0, 0,
         0, 0, 0, a2.x, a2.y, 1,
         a3.x, a3.y, 1, 0, 0, 0,
         0, 0, 0, a3.x, a3.y, 1;

    Eigen::Matrix<double, 6, 1> b;
    b << b1.x, b1.y, b2.x, b2.y, b3.x, b3.y;

    Eigen::Matrix<double, 6, 1> sol = M.colPivHouseholderQr().solve(b);

    AffineTransform res = AffineTransform(sol[0], sol[1], sol[3], sol[4], sol[2], sol[5]);

    return res;

};

AffineTransform affineFromMOSParams(int a, int b, int m, double e, double r) {
    // Paste your implementation
    return AffineTransform(1, 0, 0, 1, 0, 0);
};

} // namespace scalatrix