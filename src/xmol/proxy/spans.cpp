
#include <xmol/proxy/spans.h>

#include "xmol/Frame.h"
#include "xmol/algo/alignment.h"
#include "xmol/proxy/selections.h"
#include "xmol/proxy/smart/spans.h"

using namespace xmol::proxy;

smart::CoordSmartSpan CoordSpan::smart() { return smart::CoordSmartSpan(*this); }

xmol::geom::affine::Transformation3d CoordSpan::alignment_to(CoordSpan& other) {
  return xmol::algo::calc_alignment(other, *this);
}
xmol::geom::affine::Transformation3d CoordSpan::alignment_to(CoordSelection& other) {
  return xmol::algo::calc_alignment(other, *this);
}

double CoordSpan::rmsd(CoordSpan& other) { return xmol::algo::calc_rmsd(other, *this); }
double CoordSpan::rmsd(CoordSelection& other) { return xmol::algo::calc_rmsd(other, *this); }
Eigen::Matrix3d CoordSpan::inertia_tensor() { return xmol::algo::calc_inertia_tensor(*this); }

void CoordSpan::apply(const geom::affine::Transformation3d& t) {
  auto map = this->_eigen();
  map = (t.get_underlying_matrix() * map.transpose()).transpose().rowwise() + t.get_translation()._eigen();
}
void CoordSpan::apply(const geom::affine::UniformScale3d& t) { this->_eigen().array() *= t.scale(); }
void CoordSpan::apply(const geom::affine::Rotation3d& t) {
  auto map = this->_eigen();
  map = (t.get_underlying_matrix() * map.transpose()).transpose();
}
void CoordSpan::apply(const geom::affine::Translation3d& t) {
  auto map = this->_eigen();
  map = map.rowwise() + t.dr()._eigen();
}

xmol::geom::XYZ CoordSpan::mean() { return XYZ(_eigen().colwise().mean()); }
CoordSelection CoordSpan::slice(std::optional<size_t> start, std::optional<size_t> stop, std::optional<size_t> step) {
  if (empty()) {
    return {};
  }
  return CoordSelection(*m_frame, slice_impl(start, stop, step), true);
}

CoordSpan CoordSpan::slice(std::optional<size_t> start, std::optional<size_t> stop) {
  if (empty()) {
    return {};
  }
  return CoordSpan(*m_frame, slice_impl(start, stop));
}

std::vector<xmol::CoordIndex> CoordSpan::index() const {
  std::vector<AtomIndex> result;
  if (!empty()) {
    result.reserve(size());
    AtomIndex first = m_frame->index_of(*m_begin);
    for (int i = 0; i < size(); ++i) {
      result.push_back(first + i);
    }
  }
  return result;
}

CoordSpan AtomRefSpan::coords() {
  if (empty()) {
    return {};
  }
  return {begin()->frame(), begin()->m_coord, size()};
}

ResidueRefSpan AtomRefSpan::residues() {
  if (empty()) {
    return {};
  }
  return ResidueRefSpan(m_begin->residue, (m_begin + size() - 1)->residue + 1);
}

MoleculeRefSpan AtomRefSpan::molecules() {
  if (empty()) {
    return {};
  }
  return MoleculeRefSpan(m_begin->residue->molecule, (m_begin + size() - 1)->residue->molecule + 1);
}

CoordSpan ResidueRefSpan::coords() { return atoms().coords(); }

AtomRefSpan ResidueRefSpan::atoms() {
  if (empty()) {
    return {};
  }
  return AtomRefSpan(m_begin->atoms.m_begin, (m_begin + size() - 1)->atoms.m_end);
}
MoleculeRefSpan ResidueRefSpan::molecules() {
  if (empty()) {
    return {};
  }
  return MoleculeRefSpan(m_begin->molecule, (m_begin + size() - 1)->molecule + 1);
}

CoordSpan MoleculeRefSpan::coords() { return atoms().coords(); }

AtomRefSpan MoleculeRefSpan::atoms() {
  if (empty()) {
    return {};
  }
  auto last_mol = m_begin + size() - 1;
  auto last_mol_last_residue = last_mol->residues.m_begin + last_mol->residues.size() - 1;
  return AtomRefSpan(m_begin->residues.m_begin->atoms.m_begin, last_mol_last_residue->atoms.m_end);
}
ResidueRefSpan MoleculeRefSpan::residues() {
  if (empty()) {
    return {};
  }
  auto last_mol = m_begin + size() - 1;
  return ResidueRefSpan(m_begin->residues.m_begin, last_mol->residues.m_begin + last_mol->residues.size());
}

bool AtomRefSpan::contains(const AtomRef& ref) const {
  return m_begin <= ref.m_atom && ref.m_atom < m_end;
  ;
}

smart::AtomSmartSpan AtomRefSpan::smart() { return *this; }

AtomRefSpan& AtomRefSpan::operator&=(const AtomRefSpan& rhs) {
  intersect(rhs);
  return *this;
}

AtomSelection AtomRefSpan::slice(std::optional<size_t> start, std::optional<size_t> stop, std::optional<size_t> step) {
  return AtomSelection(slice_impl(start, stop, step), true);
}

AtomRefSpan AtomRefSpan::slice(std::optional<size_t> start, std::optional<size_t> stop) {
  return AtomRefSpan(slice_impl(start, stop));
}

std::vector<xmol::AtomIndex> AtomRefSpan::index() const {
  std::vector<AtomIndex> result;
  if (!empty()) {
    result.reserve(size());
    AtomIndex first = frame_ptr()->index_of(*m_begin);
    for (int i = 0; i < size(); ++i) {
      result.push_back(first + i);
    }
  }
  return result;
}

bool ResidueRefSpan::contains(const ResidueRef& ref) const { return m_begin <= ref.m_residue && ref.m_residue < m_end; }
smart::ResidueSmartSpan ResidueRefSpan::smart() { return *this; }

ResidueRefSpan& ResidueRefSpan::operator&=(const ResidueRefSpan& rhs) {
  intersect(rhs);
  return *this;
}

ResidueSelection ResidueRefSpan::slice(std::optional<size_t> start, std::optional<size_t> stop,
                                       std::optional<size_t> step) {
  return ResidueSelection(slice_impl(start, stop, step), true);
}

ResidueRefSpan ResidueRefSpan::slice(std::optional<size_t> start, std::optional<size_t> stop) {
  return ResidueRefSpan(slice_impl(start, stop));
}
std::vector<xmol::ResidueIndex> ResidueRefSpan::index() const {
  std::vector<ResidueIndex> result;
  if (!empty()) {
    result.reserve(size());
    ResidueIndex first = frame_ptr()->index_of(*m_begin);
    for (int i = 0; i < size(); ++i) {
      result.push_back(first + i);
    }
  }
  return result;
}

bool MoleculeRefSpan::contains(const MoleculeRef& ref) const {
  return m_begin <= ref.m_molecule && ref.m_molecule < m_end;
}
smart::MoleculeSmartSpan MoleculeRefSpan::smart() { return *this; }

MoleculeRefSpan& MoleculeRefSpan::operator&=(const MoleculeRefSpan& rhs) {
  intersect(rhs);
  return *this;
}
MoleculeSelection MoleculeRefSpan::slice(std::optional<size_t> start, std::optional<size_t> stop,
                                         std::optional<size_t> step) {
  return MoleculeSelection(slice_impl(start, stop, step), true);
}

MoleculeRefSpan MoleculeRefSpan::slice(std::optional<size_t> start, std::optional<size_t> stop) {
  return MoleculeRefSpan(slice_impl(start, stop));
}
std::vector<xmol::MoleculeIndex> MoleculeRefSpan::index() const {
  std::vector<MoleculeIndex> result;
  if (!empty()) {
    result.reserve(size());
    MoleculeIndex first = frame_ptr()->index_of(*m_begin);
    for (int i = 0; i < size(); ++i) {
      result.push_back(first + i);
    }
  }
  return result;
}

namespace xmol::proxy {

AtomSelection operator|(const AtomRefSpan& lhs, const AtomRefSpan& rhs) {
  return AtomSelection(lhs) | AtomSelection(rhs);
}
AtomSelection operator-(const AtomRefSpan& lhs, const AtomRefSpan& rhs) {
  return AtomSelection(lhs) - AtomSelection(rhs);
}
AtomRefSpan operator&(const AtomRefSpan& lhs, const AtomRefSpan& rhs) {
  AtomRefSpan result(lhs);
  result &= rhs;
  return result;
}
ResidueSelection operator|(const ResidueRefSpan& lhs, const ResidueRefSpan& rhs) {
  return ResidueSelection(lhs) | ResidueSelection(rhs);
}
ResidueSelection operator-(const ResidueRefSpan& lhs, const ResidueRefSpan& rhs) {
  return ResidueSelection(lhs) - ResidueSelection(rhs);
}
ResidueRefSpan operator&(const ResidueRefSpan& lhs, const ResidueRefSpan& rhs) {
  ResidueRefSpan result(lhs);
  result &= rhs;
  return result;
}

MoleculeSelection operator|(const MoleculeRefSpan& lhs, const MoleculeRefSpan& rhs) {
  return MoleculeSelection(lhs) | MoleculeSelection(rhs);
}
MoleculeSelection operator-(const MoleculeRefSpan& lhs, const MoleculeRefSpan& rhs) {
  return MoleculeSelection(lhs) - MoleculeSelection(rhs);
}
MoleculeRefSpan operator&(const MoleculeRefSpan& lhs, const MoleculeRefSpan& rhs) {
  MoleculeRefSpan result(lhs);
  result &= rhs;
  return result;
}

} // namespace xmol::proxy
