#include "scalatrix/affine_transform.hpp"
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

IntegerAffineTransform& IntegerAffineTransform::linearFromTwoDots(
    const Vector2i& a1, const Vector2i& a2,
    const Vector2i& b1, const Vector2i& b2) 
{
    // make sure a1 and a2 are not collinear
    assert(a1.x * a2.y - a1.y * a2.x != 0);
    // make sure b1 and b2 are not collinear
    assert(b1.x * b2.y - b1.y * b2.x != 0);

    static IntegerAffineTransform _self;
    _self.tx=0;
    _self.ty=0;
    // find linear transform that maps a1 to b1 and a2 to b2
    // find the linear transform that maps a1 to b1 and a2 to b2
    int det = a1.x * a2.y - a1.y * a2.x;
    _self.a = (b1.x * a2.y - b2.x * a1.y) / det;
    _self.b = (a1.x * b2.x - b1.x * a2.x) / det;
    _self.c = (b1.y * a2.y - a1.y * b2.y) / det;
    _self.d = (a1.x * b2.y - a2.x * b1.y) / det;

    return _self;
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

AffineTransform AffineTransform::operator*(const AffineTransform& M) const {
    return {a * M.a + b * M.c, a * M.b + b * M.d, c * M.a + d * M.c, c * M.b + d * M.d, tx * M.a + ty * M.c + M.tx, tx * M.b + ty * M.d + M.ty};
}

Vector2d AffineTransform::apply(const Vector2d& v) const {
    return {a * v.x + b * v.y + tx, c * v.x + d * v.y + ty};
}

AffineTransform AffineTransform::inverse() const {
    double det = a * d - b * c;
    assert(std::abs(det) > 1e-7);
    return {d / det, -b / det, -c / det, a / det, (d * tx - b * ty) / det, (a * ty - c * tx) / det};
}





} // namespace scalatrix