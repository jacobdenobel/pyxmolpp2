#include "GromacsXtcFile.h"
#include "xmol/proxy/smart/spans.h"

namespace py = pybind11;
using namespace xmol::io;
using namespace xmol::proxy::smart;

void pyxmolpp::v1::populate(py::class_<GromacsXtcFile, xmol::trajectory::TrajectoryInputFile>& pyGromacsXtc) {

  pyGromacsXtc
      .def(py::init<std::string, size_t>(), py::arg("filename"), py::arg("n_frames"))
      .def("n_frames", &GromacsXtcFile::n_frames, "Number of frames")
      .def("n_atoms", &GromacsXtcFile::n_atoms, "Number of atoms per frame")
      .def(
          "read_coordinates",
          [](GromacsXtcFile& self, size_t index, CoordSmartSpan& span) { self.read_coordinates(index, span); },
          py::arg("index"), py::arg("coords"), "Assign `index` frame coordinates to `coords`")
      .def("advance", &GromacsXtcFile::advance, py::arg("shift"), "Shift internal pointer by `shift`");
  ;
}

void pyxmolpp::v1::populate(pybind11::class_<xmol::io::xdr::XtcWriter>& pyXtcWriter) {

  pyXtcWriter
      .def(py::init<std::string, float>(), py::arg("filename"), py::arg("precision"))
      .def("write", &xdr::XtcWriter::write, "Write frame");
}
