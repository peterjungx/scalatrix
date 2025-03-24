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
        .def("inverse", &AffineTransform::inverse);

    py::class_<Scale>(m, "Scale")
        .def(py::init<double>())
        .def("fromAffine", &Scale::fromAffine)
        .def("recalcWithAffine", &Scale::recalcWithAffine)
        .def("retuneWithAffine", &Scale::retuneWithAffine)
        .def("getNodes", &Scale::getNodes, py::return_value_policy::reference)
        .def("print", &Scale::print);

    

    m.def("affineFromThreeDots", &scalatrix::affineFromThreeDots);
}