#include <scalatrix.hpp>
using namespace scalatrix;

#ifdef EMSCRIPTEN
#include <emscripten/bind.h>

EMSCRIPTEN_BINDINGS(scalatrix) {
    emscripten::class_<AffineTransform>("AffineTransform")
        .constructor<double, double, double, double, double, double>()  // Full constructor with tx, ty
        .property("a", &AffineTransform::a)
        .property("b", &AffineTransform::b)
        .property("c", &AffineTransform::c)
        .property("d", &AffineTransform::d)
        .property("tx", &AffineTransform::tx)
        .property("ty", &AffineTransform::ty)
        .function("apply", &AffineTransform::apply)
        .function("inverse", &AffineTransform::inverse);

    emscripten::class_<Scale>("Scale")
        .constructor<double>()
        .class_function("fromAffine", &Scale::fromAffine)
        .function("getNodes", &Scale::getNodes)
        .function("print", &Scale::print);

    emscripten::value_object<Vector2d>("Vector2d")
        .field("x", &Vector2d::x)
        .field("y", &Vector2d::y);

    emscripten::value_object<Vector2i>("Vector2i")
        .field("x", &Vector2i::x)
        .field("y", &Vector2i::y);

    emscripten::value_object<Node>("Node")
        .field("natural_coord", &Node::natural_coord)
        .field("tuning_coord", &Node::tuning_coord)
        .field("pitch", &Node::pitch);

    emscripten::register_vector<Node>("VectorNode");

    emscripten::function("find_affine_transform", &scalatrix::find_affine_transform);
}
#endif


int main() {
    return 0;
}