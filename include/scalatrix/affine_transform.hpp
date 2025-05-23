#ifndef SCALATRIX_AFFINE_TRANSFORM_HPP
#define SCALATRIX_AFFINE_TRANSFORM_HPP

#include <utility>

namespace scalatrix {

struct Vector2i {
    int x, y;
    Vector2i(int x_ = 0, int y_ = 0) noexcept : x(x_), y(y_) {}
    Vector2i operator-(){ return {-x, -y}; }
    void operator+=(const Vector2i& v) { x += v.x; y += v.y; }
    void operator-=(const Vector2i& v) { x -= v.x; y -= v.y; }
    Vector2i operator+(const Vector2i& v) { return {x + v.x, y + v.y}; }
    Vector2i operator-(const Vector2i& v) { return {x - v.x, y - v.y}; }
    Vector2i operator*(const int s) { return {x * s, y * s}; }
    bool operator<(const Vector2i& v) const {
        return (x < v.x) || (x == v.x && y < v.y);
    };
    bool operator==(const Vector2i& v) const { return x == v.x && y == v.y; }

};

class IntegerAffineTransform {
public:
    int a, b, c, d;  // 2x2 matrix
    int tx, ty;      // Offset vector

    IntegerAffineTransform(int a_ = 1, int b_ = 0, int c_ = 0, int d_ = 1,
                          int tx_ = 0, int ty_ = 0);
    IntegerAffineTransform operator*(int s) const;
    Vector2i operator*(const Vector2i& v) const;
    IntegerAffineTransform inverse() const;  // May need special handling if not invertible
    Vector2i apply(const Vector2i& v) const;
    IntegerAffineTransform applyAffine(const IntegerAffineTransform& M) const;

    static IntegerAffineTransform& linearFromTwoDots(
        const Vector2i& a1, const Vector2i& a2,
        const Vector2i& b1, const Vector2i& b2);

};

struct Vector2d {
    double x, y;
    Vector2d(double x_ = 0.0, double y_ = 0.0) noexcept : x(x_), y(y_) {}
    Vector2d(Vector2i v) noexcept : x(v.x), y(v.y) {}
    Vector2d operator-(){ return {-x, -y}; }
    void operator+=(const Vector2d& v) { x += v.x; y += v.y; }
    void operator-=(const Vector2d& v) { x -= v.x; y -= v.y; }
    Vector2d operator+(const Vector2d& v) { return {x + v.x, y + v.y}; }
    Vector2d operator-(const Vector2d& v) { return {x - v.x, y - v.y}; }
};

class AffineTransform {
public:
    double a, b, c, d;  // 2x2 matrix
    double tx, ty;      // Offset vector

    AffineTransform(double a_ = 1.0, double b_ = 0.0, double c_ = 0.0, double d_ = 1.0,
                    double tx_ = 0.0, double ty_ = 0.0);
    AffineTransform operator*(double s) const;
    Vector2d operator*(const Vector2d& v) const;
    Vector2d operator*(const Vector2i& v) const;
    AffineTransform operator*(const AffineTransform& M) const;
    AffineTransform inverse() const;
    Vector2d apply(const Vector2d& v) const;
    AffineTransform applyAffine(const AffineTransform& M) const;
};

} // namespace scalatrix

#endif // SCALATRIX_AFFINE_TRANSFORM_HPP