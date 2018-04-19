#pragma once

#include "xmol/trajectory/Trajectory.h"

#include <fstream>
#include <memory>

namespace xmol::pdb {

class PdbReader;

class PdbFile : public xmol::trajectory::TrajectoryPortion {
public:
  explicit PdbFile(const std::string& filename);
  ~PdbFile() override = default;
  void set_coordinates(xmol::polymer::frameIndex_t frameIndex,
                       const std::vector<std::pair<int, xmol::polymer::Atom*>>&
                           index_atoms) override;
  void set_coordinates(xmol::polymer::frameIndex_t frameIndex,
                       xmol::polymer::AtomSelection& atoms) override;
  xmol::polymer::frameIndex_t n_frames() const override;
  xmol::polymer::atomIndex_t n_atoms_per_frame() const override;
  bool match(const xmol::polymer::AtomSelection& atoms) const override;
  void close() override;

  xmol::polymer::Frame get_frame();
  xmol::polymer::Frame get_frame(int n);

private:
  std::string m_filename;
  std::unique_ptr<std::ifstream> m_stream;
  std::unique_ptr<PdbReader> m_reader;
};
}