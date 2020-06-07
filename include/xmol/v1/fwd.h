#pragma once
#include "xmol/polymer/Atom_fwd.h"
#include "xmol/utils/ShortAsciiString.h"
#include <stdexcept>

/// Reworked original xmol
namespace xmol::v1 {

namespace detail {
using AtomNameTag = xmol::polymer::detail::AtomNameTag;
using ResidueNameTag = xmol::polymer::detail::ResidueNameTag;
using ChainNameTag = xmol::polymer::detail::ChainNameTag;
} // namespace detail

using AtomId = int32_t;
using AtomName = xmol::utils::ShortAsciiString<4, false, detail::AtomNameTag>;
using ResidueName = xmol::utils::ShortAsciiString<3, false, detail::ResidueNameTag>;
using MoleculeName = xmol::utils::ShortAsciiString<1, false, detail::ChainNameTag>;
using ResidueId = xmol::polymer::ResidueId;
using XYZ = xmol::geometry::XYZ;

struct BaseAtom;
struct BaseResidue;
struct BaseMolecule;

/// Proxy references to frame
namespace proxy {

class AtomRef;
class ResidueRef;
class MoleculeRef;

class AtomSelection;
class ResidueSelection;
class MoleculeSelection;

class AtomRefSpan;
class ResidueRefSpan;
class MoleculeRefSpan;

/// Reference counting (smart) proxies
namespace smart {

class AtomSmartRef;
class ResidueSmartRef;
class MoleculeSmartRef;

class AtomSmartSelection;
class ResidueSmartSelection;
class MoleculeSmartSelection;

class AtomSmartSpan;
class ResidueSmartSpan;
class MoleculeSmartSpan;

} // namespace smart
} // namespace proxy

/// life holder
class Frame;

class DeadFrameAccessError : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

} // namespace xmol::v1