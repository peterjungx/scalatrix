#include "scalatrix/affine_transform.hpp"
#include <Eigen/Dense>
#include <cassert>


namespace scalatrix {


IntegerAffineTransform::IntegerAffineTransform(int a_, int b_, int c_, int d_, int tx_, int ty_)
    : a(a_), b(b_), c(c_), d(d_), tx(tx_), ty(ty_) {}

IntegerAffineTransform IntegerAffineTransform::operator*(int s) const {
    return {a * s, b * s, c * s, d * s, tx * s, ty * s};
}

Vector2i IntegerAffineTransform::operator*(const Vector2i& v) const {
    return {a * v.x + b * v.y + tx, c * v.x + d * v.y + ty};
}

Vector2i IntegerAffineTransform::apply(const Vector2i& v) const {
    return {a * v.x + b * v.y + tx, c * v.x + d * v.y + ty};
}

IntegerAffineTransform IntegerAffineTransform::inverse() const {
    int det = a * d - b * c;
    assert(det != 0);
    return {d / det, -b / det, -c / det, a / det, (d * tx - b * ty) / det, (a * ty - c * tx) / det};
}


AffineTransform::AffineTransform(double a_, double b_, double c_, double d_, double tx_, double ty_)
    : a(a_), b(b_), c(c_), d(d_), tx(tx_), ty(ty_) {}

AffineTransform AffineTransform::operator*(double s) const {
    return {a * s, b * s, c * s, d * s, tx * s, ty * s};
}

Vector2d AffineTransform::operator*(const Vector2d& v) const {
    return {a * v.x + b * v.y + tx, c * v.x + d * v.y + ty};
}

Vector2d AffineTransform::operator*(const Vector2i& v) const {
    return {a * v.x + b * v.y + tx, c * v.x + d * v.y + ty};
}

Vector2d AffineTransform::apply(const Vector2d& v) const {
    return {a * v.x + b * v.y + tx, c * v.x + d * v.y + ty};
}

AffineTransform AffineTransform::inverse() const {
    double det = a * d - b * c;
    assert(std::abs(det) > 1e-7);
    return {d / det, -b / det, -c / det, a / det, (d * tx - b * ty) / det, (a * ty - c * tx) / det};
}


AffineTransform find_affine_transform(
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

    return AffineTransform(sol[0], sol[1], sol[3], sol[4], sol[2], sol[5]);
}


} // namespace scalatrix