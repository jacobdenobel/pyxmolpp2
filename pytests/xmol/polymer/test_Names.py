import pytest
import os


def test_AtomName():
    from pyxmolpp2.polymer import AtomName

    with pytest.raises(RuntimeError):
        a = AtomName("ATOME")

    assert AtomName("ATOM").str == "ATOM"
    assert AtomName("ATOM") == AtomName("ATOM")




def test_ResidueName():
    from pyxmolpp2.polymer import ResidueName

    with pytest.raises(RuntimeError):
        a = ResidueName("RESI")

    assert ResidueName("RES").str == "RES"
    assert ResidueName("RES") == ResidueName("RES")


def test_ChainName():
    from pyxmolpp2.polymer import ChainName

    with pytest.raises(RuntimeError):
        a = ChainName("CH")

    assert ChainName("C").str == "C"
    assert ChainName("C") == ChainName("C")


