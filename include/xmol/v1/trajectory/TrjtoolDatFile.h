#pragma once
#include "TrajectoryFile.h"
#include <fstream>

namespace xmol::v1::trajectory {

/// 3d MD coordinates in "trjtool .dat" format
class TrjtoolDatFile : public TrajectoryInputFile {
  struct Header {
    int32_t nitems;
    int32_t ndim;
    int32_t dtype;
  };

  union HeaderUnion {
    Header header;
    char bytes[sizeof(Header)];
  };

public:
  TrjtoolDatFile(std::string filename);
  [[nodiscard]] size_t n_frames() const final;
  [[nodiscard]] size_t n_atoms() const final;
  void read_coordinates(size_t index, future::Span<XYZ>& coordinates) final;
  void advance(size_t shift) final;

private:
  std::string m_filename;
  std::unique_ptr<std::istream> m_stream;
  Header m_header;
  size_t m_n_frames;
  size_t m_current_frame = 0;
  std::streampos m_offset;

  void read_header();
};
} // namespace xmol::v1::trajectory