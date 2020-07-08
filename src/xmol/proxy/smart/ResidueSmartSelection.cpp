#include "xmol/proxy/smart/ResidueSmartSelection.h"
#include "xmol/Frame.h"
#include "xmol/proxy/smart/FrameObserverImpl.h"

using namespace xmol::proxy::smart;

struct ResidueSmartSelection::ResidueRefLessThanComparator {
  bool operator()(ResidueRef& a, BaseResidue* ptr) { return a.res_ptr() < ptr; }
  bool operator()(BaseResidue* ptr, ResidueRef& a) { return ptr < a.res_ptr(); }
};

void ResidueSmartSelection::on_base_residues_move(BaseResidue *from_begin, BaseResidue *from_end, BaseResidue *to_begin) {
  auto it =
      std::lower_bound(m_selection.m_data.begin(), m_selection.m_data.end(), from_begin, ResidueRefLessThanComparator{});
  auto it_end =
      std::upper_bound(m_selection.m_data.begin(), m_selection.m_data.end(), from_end, ResidueRefLessThanComparator{});
  for (; it != it_end; ++it) {
    assert(from_begin <= it->res_ptr());
    assert(it->res_ptr() < from_end);
    it->res_ptr() = to_begin + (it->res_ptr() - from_begin);
  }
}

xmol::proxy::smart::ResidueSmartSelection::ResidueSmartSelection(xmol::proxy::ResidueSelection sel)
    : FrameObserver(sel.frame_ptr()), m_selection(std::move(sel)) {
  if (m_selection.frame_ptr()) {
    m_selection.frame_ptr()->reg(*this);
  }
}

template class xmol::proxy::smart::FrameObserver<ResidueSmartSelection>;
