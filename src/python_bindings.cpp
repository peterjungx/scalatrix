#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // For std::vector, std::pair
#include <scalatrix.hpp>

namespace py = pybind11;
using namespace scalatrix;

PYBIND11_MODULE(scalatrix, m) {
    py::class_<Vector2d>(m, "Vector2d")
        .def(py::init<double, double>())
        .def_readwrite("x", &Vector2d::x)
        .def_readwrite("y", &Vector2d::y)
        .def("__add__", [](Vector2d a, const Vector2d& b) { return a + b; })
        .def("__sub__", [](Vector2d a, const Vector2d& b) { return a - b; })
        .def("__mul__", [](Vector2d a, double s) { return a * s; })
        .def("__rmul__", [](double s, const Vector2d& a) { return s * a; })
        .def("__repr__", [](const Vector2d &v) {
            return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        });
        

    py::class_<Vector2i>(m, "Vector2i")
        .def(py::init<int, int>())
        .def_readwrite("x", &Vector2i::x)
        .def_readwrite("y", &Vector2i::y)
        .def("__eq__", [](const Vector2i& a, const Vector2i& b) { return a == b; })
        .def("__add__", [](Vector2i a, const Vector2i& b) { return a + b; })
        .def("__sub__", [](Vector2i a, const Vector2i& b) { return a - b; })
        .def("__mul__", [](Vector2i a, int s) { return a * s; })
        .def("__rmul__", [](int s, const Vector2i& a) { return s * a; })
        .def("__repr__", [](const Vector2i &v) {
            return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        });

    py::class_<Node>(m, "Node")
        .def(py::init<>())
        .def_readwrite("natural_coord", &Node::natural_coord)
        .def_readwrite("tuning_coord", &Node::tuning_coord)
        .def_readwrite("pitch", &Node::pitch)
        .def_readwrite("isTempered", &Node::isTempered)
        .def_readwrite("temperedPitch", &Node::temperedPitch)
        .def("__repr__", [](const Node &n) {
            return "Node(natural_coord=(" + std::to_string(n.natural_coord.x)
             + ", " + std::to_string(n.natural_coord.y) +
                   "), tuning_coord=(" + std::to_string(n.tuning_coord.x) + ", " + std::to_string(n.tuning_coord.y) +
                   "), pitch=" + std::to_string(n.pitch) +
                   ", isTempered=" + std::to_string(n.isTempered);// +
                  //", temperedPitch=" + std::to_string(n.temperedPitch) + ")";
        });


    py::class_<IntegerAffineTransform>(m, "IntegerAffineTransform")
        .def(py::init<int, int, int, int, int, int>())
        .def_readwrite("a", &IntegerAffineTransform::a)
        .def_readwrite("b", &IntegerAffineTransform::b)
        .def_readwrite("c", &IntegerAffineTransform::c)
        .def_readwrite("d", &IntegerAffineTransform::d)
        .def_readwrite("tx", &IntegerAffineTransform::tx)
        .def_readwrite("ty", &IntegerAffineTransform::ty)
        .def("apply", &IntegerAffineTransform::apply)
        .def("inverse", &IntegerAffineTransform::inverse)
        .def("__mul__", [](const IntegerAffineTransform &a, const Vector2i &b) {
            return a.apply(b);
        }, py::is_operator())
        .def("__repr__", [](const IntegerAffineTransform &t) {
            return "IntegerAffineTransform(a=" + std::to_string(t.a) + ", b=" + std::to_string(t.b) +
                   ", c=" + std::to_string(t.c) + ", d=" + std::to_string(t.d) +
                   ", tx=" + std::to_string(t.tx) + ", ty=" + std::to_string(t.ty) + ")";
        })
        .def_static("linearFromTwoDots", &IntegerAffineTransform::linearFromTwoDots);

    py::class_<AffineTransform>(m, "AffineTransform")
        .def(py::init<double, double, double, double, double, double>())
        .def_readwrite("a", &AffineTransform::a)
        .def_readwrite("b", &AffineTransform::b)
        .def_readwrite("c", &AffineTransform::c)
        .def_readwrite("d", &AffineTransform::d)
        .def_readwrite("tx", &AffineTransform::tx)
        .def_readwrite("ty", &AffineTransform::ty)
        .def("apply", &AffineTransform::apply)
        .def("inverse", &AffineTransform::inverse)
        .def("__mul__", [](const AffineTransform &a, const Vector2d &b) {
            return a.apply(b);
        }, py::is_operator())
        .def("applyToVector2i", [](const AffineTransform &a, const Vector2i &b) {
            return a * b;
        })
        .def("__repr__", [](const AffineTransform &t) {
            return "AffineTransform(a=" + std::to_string(t.a) + ", b=" + std::to_string(t.b) +
                   ", c=" + std::to_string(t.c) + ", d=" + std::to_string(t.d) +
                   ", tx=" + std::to_string(t.tx) + ", ty=" + std::to_string(t.ty) + ")";
        });

    py::class_<Scale>(m, "Scale")
        .def(py::init<double>())
        .def("fromAffine", &Scale::fromAffine)
        .def("recalcWithAffine", &Scale::recalcWithAffine)
        .def("retuneWithAffine", &Scale::retuneWithAffine)
        .def("getNodes", &Scale::getNodes, py::return_value_policy::reference)
        .def("getRootIdx", &Scale::getRootIdx)
        .def("temperToPitchSet", &Scale::temperToPitchSet)
        .def("print", &Scale::print);

    py::class_<MOS>(m, "MOS")
        .def(py::init<int, int, int, double, double>())
        .def_readwrite("L_vec", &MOS::L_vec)
        .def_readwrite("s_vec", &MOS::s_vec)
        .def_readwrite("chroma_vec", &MOS::chroma_vec)
        .def_readwrite("L_fr", &MOS::L_fr)
        .def_readwrite("s_fr", &MOS::s_fr)
        .def_readwrite("chroma_fr", &MOS::chroma_fr)
        .def_readwrite("a", &MOS::a)
        .def_readwrite("b", &MOS::b)
        .def_readwrite("n", &MOS::n)
        .def_readwrite("a0", &MOS::a0)
        .def_readwrite("b0", &MOS::b0)
        .def_readwrite("n0", &MOS::n0)
        .def_readwrite("mode", &MOS::mode)
        .def_readwrite("repetitions", &MOS::repetitions)
        .def_readwrite("depth", &MOS::depth)
        .def_readwrite("equave", &MOS::equave)
        .def_readwrite("period", &MOS::period)
        .def_readwrite("generator", &MOS::generator)
        .def_readwrite("path", &MOS::path)
        .def_readwrite("impliedAffine", &MOS::impliedAffine)
        .def_readwrite("mosTransform", &MOS::mosTransform)
        .def_readwrite("v_gen", &MOS::v_gen)
        .def_readwrite("base_scale", &MOS::base_scale)
        .def_static("fromParams", &MOS::fromParams)
        .def_static("fromG", &MOS::fromG)
        .def("adjustParams", &MOS::adjustParams)
        .def("angle", &MOS::angle)
        .def("angleStd", &MOS::angleStd)
        .def("calcImpliedAffine", &MOS::calcImpliedAffine)
        .def("gFromAngle", &MOS::gFromAngle)
        .def("nodeLabelDigit", &MOS::nodeLabelDigit)
        .def("nodeLabelLetter", &MOS::nodeLabelLetter)
        .def("nodeLabelLetterWithOctaveNumber", &MOS::nodeLabelLetterWithOctaveNumber)
        .def("retuneZeroPoint", &MOS::retuneZeroPoint)
        .def("retuneOnePoint", &MOS::retuneOnePoint)
        .def("retuneTwoPoints", &MOS::retuneTwoPoints)
        .def("retuneThreePoints", &MOS::retuneThreePoints)
        .def("generateScaleFromMOS", &MOS::generateScaleFromMOS)
        .def("retuneScaleWithMOS", &MOS::retuneScaleWithMOS)
        .def("mapFromMOS", &MOS::mapFromMOS);

    // pitchset.hpp

    py::class_<PitchSetPitch>(m, "PitchSetPitch")
        .def(py::init<>())
        .def_readwrite("label", &PitchSetPitch::label)
        .def_readwrite("log2fr", &PitchSetPitch::log2fr);
    
    py::class_<PitchSet>(m, "PitchSet")
        .def(py::init<>())
        .def_static("generateETPitchSet", &generateETPitchSet)
        .def_static("generateJIPitchSet", &generateJIPitchSet)
        .def_static("generateHarmonicSeriesPitchSet", &generateHarmonicSeriesPitchSet);
    
    py::class_<PseudoPrimeInt>(m, "PseudoPrimeInt")
        .def(py::init<>())
        .def_readwrite("label", &PseudoPrimeInt::label)
        .def_readwrite("number", &PseudoPrimeInt::number)
        .def_readwrite("log2fr", &PseudoPrimeInt::log2fr);
    
    py::class_<PrimeList>(m, "PrimeList")
        .def(py::init<>())
        .def_static("generateDefaultPrimeList", &generateDefaultPrimeList)
        .def_static("pseudoPrimeFromIndexNumber", &pseudoPrimeFromIndexNumber);



    m.def("affineFromThreeDots", &scalatrix::affineFromThreeDots);
}