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
        .constructor<double, int>()
        .class_function("fromAffine", &Scale::fromAffine)
        .function("recalcWithAffine", &Scale::recalcWithAffine)
        .function("retuneWithAffine", &Scale::retuneWithAffine)
        .function("getNodes", &Scale::getNodes)
        .function("print", &Scale::print);


    emscripten::class_<MOS>("MOS")
        .class_function("fromG", &MOS::fromG)
        .function("angle", &MOS::angle)
        .function("angleStd", &MOS::angleStd)
        .function("gFromAngle", &MOS::gFromAngle)
        .function("accidentalString", &MOS::accidentalString)
        .function("nodeLabelDigit", &MOS::nodeLabelDigit)
        .function("nodeLabelLetter", &MOS::nodeLabelLetter)
        .function("nodeLabelLetterWithOctaveNumber", &MOS::nodeLabelLetterWithOctaveNumber)
        .function("retuneZeroPoint", &MOS::retuneZeroPoint)
        .function("retuneOnePoint", &MOS::retuneOnePoint)
        .function("retuneTwoPoints", &MOS::retuneTwoPoints)
        .function("retuneThreePoints", &MOS::retuneThreePoints)
        .function("generateScaleFromMOS", &MOS::generateScaleFromMOS)
        .function("retuneScaleWithMOS", &MOS::retuneScaleWithMOS)
        .property("L_vec", &MOS::L_vec)
        .property("s_vec", &MOS::s_vec)
        .property("chroma_vec", &MOS::chroma_vec)
        .property("L_fr", &MOS::L_fr)
        .property("s_fr", &MOS::s_fr)
        .property("chroma_fr", &MOS::chroma_fr)
        .property("a", &MOS::a)
        .property("b", &MOS::b)
        .property("n", &MOS::n)
        .property("a0", &MOS::a0)
        .property("b0", &MOS::b0)
        .property("n0", &MOS::n0)
        .property("mode", &MOS::mode)
        .property("repetitions", &MOS::repetitions)
        .property("depth", &MOS::depth)
        .property("equave", &MOS::equave)
        .property("period", &MOS::period)
        .property("generator", &MOS::generator)
        .property("path", &MOS::path)
        .property("impliedAffine", &MOS::impliedAffine)
        .property("mosTransform", &MOS::mosTransform)
        .property("v_gen", &MOS::v_gen)
        .property("base_scale", &MOS::base_scale);

    emscripten::value_object<PitchSetPitch>("PitchSetPitch")
        .field("label", &PitchSetPitch::label)
        .field("log2fr", &PitchSetPitch::log2fr);
    
    emscripten::class_<PitchSet>("PitchSet")
        .constructor<>()
        .function("generateETPitchSet", &generateETPitchSet)
        .function("generateJIPitchSet", &generateJIPitchSet)
        .function("generateHarmonicSeriesPitchSet", &generateHarmonicSeriesPitchSet);
    
    emscripten::value_object<PseudoPrimeInt>("PseudoPrimeInt")
        .field("label", &PseudoPrimeInt::label)
        .field("number", &PseudoPrimeInt::number)
        .field("log2fr", &PseudoPrimeInt::log2fr);
    
    emscripten::class_<PrimeList>("PrimeList")
        .constructor<>()
        .function("pseudoPrimeFromIndexNumber", &pseudoPrimeFromIndexNumber)
        .function("generateDefaultPrimeList", &generateDefaultPrimeList);

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

    emscripten::function("affineFromThreeDots", &scalatrix::affineFromThreeDots);
}
#endif


int main() {
    return 0;
}