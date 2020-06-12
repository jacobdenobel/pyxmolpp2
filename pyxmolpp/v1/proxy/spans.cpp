#include "spans.h"
#include "v1/iterator-helpers.h"
#include "xmol/proxy/smart/references.h"
#include "xmol/proxy/smart/selections.h"
#include "xmol/proxy/smart/spans.h"
#include "xmol/proxy/spans-impl.h"
#include "xmol/Frame.h"

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace xmol;
using namespace xmol::proxy;
using namespace xmol::proxy::smart;

void pyxmolpp::v1::populate(pybind11::class_<xmol::proxy::smart::CoordSmartSpan>& pyCoordSpan) {
  using Span = CoordSmartSpan;
  pyCoordSpan.def(py::init<Span>())
      .def_property_readonly("size", &Span::size)
      .def_property_readonly("empty", &Span::empty)
      .def_property_readonly("__frame", &Span::frame, py::return_value_policy::reference)
      .def("filter", [](Span& span, const std::function<bool(const XYZ&)>& f) { return span.filter(f).smart(); })
      .def("__len__", &Span::size)
      //      .def("__contains__", &Span::contains)
      .def("__getitem__", [](Span& span, size_t i) { return XYZ(span[i]); })
      .def(
          "__iter__", [](Span& s) { return common::make_coord_value_iterator(s.begin(), s.end()); },
          py::keep_alive<0, 1>())
      .def_property_readonly("values", [](py::object self) {
        py::object owner=self.attr("__frame");
        Span& span = self.cast<Span&>();
        auto eigen_map = span._eigen();
        size_t shape[] = {(size_t)eigen_map.rows(), (size_t)eigen_map.cols()};
        size_t strides[] = {(size_t)eigen_map.rowStride()*sizeof(double), (size_t)eigen_map.colStride()*sizeof(double)};
        auto array = py::array(shape, strides, eigen_map.data(), owner);
        return array;
      });
}
void pyxmolpp::v1::populate(pybind11::class_<xmol::proxy::smart::AtomSmartSpan>& pyAtomSpan) {
  using Span = AtomSmartSpan;
  pyAtomSpan.def(py::init<Span>())
      .def_property_readonly("size", &Span::size)
      .def_property_readonly("empty", &Span::empty)
      .def("filter",
           [](Span& span, const std::function<bool(const AtomSmartRef&)>& f) { return span.filter(f).smart(); })
      .def_property_readonly("coords", [](Span& span) { return span.coords().smart(); })
      .def_property_readonly("residues", [](Span& span) { return span.residues().smart(); })
      .def_property_readonly("molecules", [](Span& span) { return span.molecules().smart(); })
      .def("__len__", &Span::size)
      .def("__contains__", &Span::contains)
      .def("__getitem__", [](Span& span, size_t i) { return span[i].smart(); })
      .def(
          "__iter__", [](Span& s) { return common::make_smart_iterator(s.begin(), s.end()); }, py::keep_alive<0, 1>());
  // todo: add operators
}
void pyxmolpp::v1::populate(pybind11::class_<xmol::proxy::smart::ResidueSmartSpan>& pyResidueSpan) {
  using Span = ResidueSmartSpan;
  pyResidueSpan.def(py::init<Span>())
      .def_property_readonly("size", &Span::size)
      .def_property_readonly("empty", &Span::empty)
      .def("filter",
           [](Span& span, const std::function<bool(const ResidueSmartRef&)>& f) { return span.filter(f).smart(); })
      .def_property_readonly("coords", [](Span& span) { return span.coords().smart(); })
      .def_property_readonly("atoms", [](Span& span) { return span.atoms().smart(); })
      .def_property_readonly("molecules", [](Span& span) { return span.molecules().smart(); })
      .def("__len__", &Span::size)
      .def("__contains__", &Span::contains)
      .def("__getitem__", [](Span& span, size_t i) { return span[i].smart(); })
      .def(
          "__iter__", [](Span& s) { return common::make_smart_iterator(s.begin(), s.end()); }, py::keep_alive<0, 1>());
  // todo: add operators
}
void pyxmolpp::v1::populate(pybind11::class_<xmol::proxy::smart::MoleculeSmartSpan>& pyMoleculeSpan) {
  using Span = MoleculeSmartSpan;
  pyMoleculeSpan.def(py::init<Span>())
      .def_property_readonly("size", &Span::size)
      .def_property_readonly("empty", &Span::empty)
      .def("filter",
           [](Span& span, const std::function<bool(const MoleculeSmartRef&)>& f) { return span.filter(f).smart(); })
      .def_property_readonly("coords", [](Span& span) { return span.coords().smart(); })
      .def_property_readonly("atoms", [](Span& span) { return span.atoms().smart(); })
      .def_property_readonly("residues", [](Span& span) { return span.residues().smart(); })
      .def("__len__", &Span::size)
      .def("__contains__", &Span::contains)
      .def("__getitem__", [](Span& span, size_t i) { return span[i].smart(); })
      .def(
          "__iter__", [](Span& s) { return common::make_smart_iterator(s.begin(), s.end()); }, py::keep_alive<0, 1>());
  // todo: add operators
}