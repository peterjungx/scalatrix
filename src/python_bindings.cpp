#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // For std::vector, std::pair
#include <scalatrix.hpp>

namespace py = pybind11;
using namespace scalatrix;

PYBIND11_MODULE(scalatrix, m) {
    py::class_<Vector2d>(m, "Vector2d")
        .def(py::init<double, double>())
        .def_readwrite("x", &Vector2d::x)
        .def_readwrite("y", &Vector2d::y);

    py::class_<Vector2i>(m, "Vector2i")
        .def(py::init<int, int>())
        .def_readwrite("x", &Vector2i::x)
        .def_readwrite("y", &Vector2i::y);

    py::class_<Node>(m, "Node")
        .def(py::init<>())
        .def_readwrite("natural_coord", &Node::natural_coord)
        .def_readwrite("tuning_coord", &Node::tuning_coord)
        .def_readwrite("pitch", &Node::pitch);

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
        ;

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
        .def("fromG", &MOS::fromG)
        .def("adjustParams", &MOS::adjustParams)
        .def("angle", &MOS::angle)
        .def("angleStd", &MOS::angleStd)
        .def("calcImpliedAffine", &MOS::calcImpliedAffine)
        .def("gFromAngle", &MOS::gFromAngle)
        .def("accidentalString", &MOS::accidentalString)
        .def("nodeLabelDigit", &MOS::nodeLabelDigit)
        .def("nodeLabelLetter", &MOS::nodeLabelLetter)
        .def("nodeLabelLetterWithOctaveNumber", &MOS::nodeLabelLetterWithOctaveNumber)
        .def("retuneZeroPoint", &MOS::retuneZeroPoint)
        .def("retuneOnePoint", &MOS::retuneOnePoint)
        .def("retuneTwoPoints", &MOS::retuneTwoPoints)
        .def("retuneThreePoints", &MOS::retuneThreePoints)
        .def("generateScaleFromMOS", &MOS::generateScaleFromMOS)
        .def("retuneScaleWithMOS", &MOS::retuneScaleWithMOS);

    // pitchset.hpp

    py::class_<PitchSetPitch>(m, "PitchSetPitch")
        .def(py::init<>())
        .def_readwrite("label", &PitchSetPitch::label)
        .def_readwrite("log2fr", &PitchSetPitch::log2fr);
    
    py::class_<PitchSet>(m, "PitchSet")
        .def(py::init<>())
        .def("generateETPitchSet", &generateETPitchSet)
        .def("generateJIPitchSet", &generateJIPitchSet)
        .def("generateHarmonicSeriesPitchSet", &generateHarmonicSeriesPitchSet);
    
    py::class_<PseudoPrimeInt>(m, "PseudoPrimeInt")
        .def(py::init<>())
        .def_readwrite("label", &PseudoPrimeInt::label)
        .def_readwrite("number", &PseudoPrimeInt::number)
        .def_readwrite("log2fr", &PseudoPrimeInt::log2fr);
    
    py::class_<PrimeList>(m, "PrimeList")
        .def(py::init<>())
        .def("generateDefaultPrimeList", &generateDefaultPrimeList)
        .def("pseudoPrimeFromIndexNumber", &pseudoPrimeFromIndexNumber);



    m.def("affineFromThreeDots", &scalatrix::affineFromThreeDots);
}