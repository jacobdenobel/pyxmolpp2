import pytest
import os
from make_polygly import make_polyglycine


def test_atom_name():
    from pyxmolpp2.v1 import aName
    frame = make_polyglycine([("A", 10)])

    assert frame.atoms.filter(aName == "CA").size == 10
    assert frame.atoms.filter(aName.is_in({"CA", "N"})).size == 20
    assert frame.atoms.filter(~aName.is_in({"CA", "N"})).size == 50
    assert frame.atoms.filter((aName == "CA") | (aName == "N")).size == 20


def test_residue_name():
    from pyxmolpp2.v1 import rName
    frame = make_polyglycine([("A", 10)])

    assert frame.atoms.filter(rName == "GLY").size == 70
    assert frame.atoms.filter(rName.is_in({"GLY", "LYS"})).size == 70
    assert frame.atoms.filter(~rName.is_in({"GLY"})).size == 0
    assert frame.atoms.filter((rName == "GLY") | (rName != "GLY")).size == 70

    assert frame.residues.filter(rName == "GLY").size == 10
    assert frame.residues.filter(rName.is_in({"GLY", "LYS"})).size == 10
    assert frame.residues.filter(~rName.is_in({"GLY"})).size == 0
    assert frame.residues.filter((rName == "GLY") | (rName != "GLY")).size == 10



def test_chain_name():
    from pyxmolpp2.v1 import cName
    frame = make_polyglycine([("A", 10),("B",20)])

    assert frame.atoms.filter(cName == "A").size == 10*7
    assert frame.atoms.filter(cName.is_in({"A", "B"})).size == 30*7
    assert frame.atoms.filter(~cName.is_in({"B"})).size == 10*7
    assert frame.atoms.filter((cName == "A") | (cName != "B")).size == 10*7

    assert frame.residues.filter(cName == "A").size == 10
    assert frame.residues.filter(cName.is_in({"A", "B"})).size == 30
    assert frame.residues.filter(~cName.is_in({"B"})).size == 10
    assert frame.residues.filter((cName == "A") | (cName != "B")).size == 10

    assert frame.molecules.filter(cName == "A").size == 1
    assert frame.molecules.filter(cName.is_in({"A", "B"})).size == 2
    assert frame.molecules.filter(~cName.is_in({"B"})).size == 1
    assert frame.molecules.filter((cName == "A") | (cName != "B")).size == 1


def test_atom_id():
    from pyxmolpp2.v1 import aId
    frame = make_polyglycine([("A", 10)])

    assert frame.atoms.filter(aId == 5).size == 1
    assert frame.atoms.filter(aId.is_in({1,2,3})).size == 3
    assert frame.atoms.filter(~aId.is_in({1,2,3})).size == 70-3
    assert frame.atoms.filter((aId == 2) | (aId == 3)).size == 2


def test_residue_id():
    from pyxmolpp2.v1 import rId, ResidueId
    frame = make_polyglycine([("A", 10)])

    assert frame.atoms.filter(rId == 5).size == 1*7
    assert frame.atoms.filter(rId.is_in({1,2,3})).size == 3*7
    assert frame.atoms.filter(~rId.is_in({1,2,3})).size == 7*7
    assert frame.atoms.filter((rId == 2) | (rId == 3)).size == 2*7

    assert frame.residues.filter(rId == 5).size == 1
    assert frame.residues.filter(rId.is_in({1,2,3})).size == 3
    assert frame.residues.filter(~rId.is_in({1,2,3})).size == 7
    assert frame.residues.filter((rId == 2) | (rId == 3)).size == 2

    assert frame.residues.filter(rId == ResidueId(5,"A")).size == 0
