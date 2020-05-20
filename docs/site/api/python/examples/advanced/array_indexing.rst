Numpy-style array indexing
^^^^^^^^^^^^^^^^^^^^^^^^^^

Syntax shortcuts are available for construction index by predicate from AtomSeleciton/ResidueSelection/ChainSelection
and vice versa:

.. code:: py

    index = sel.index(predicate)
    subset = sel[index]

.. py-exec::
    :context-id: array_indexing

    import pyxmolpp2
    import os

    from pyxmolpp2.polymer import aName

Let's create a frame to work with

.. py-exec::
    :context-id: array_indexing

    pdb_filename = os.path.join(os.environ["TEST_DATA_PATH"], "pdb/rcsb/1UBQ.pdb")
    pdb_file = pyxmolpp2.pdb.PdbFile(pdb_filename)
    frame = pdb_file.get_frame()


.. py-exec::
    :context-id: array_indexing

    atoms = frame.asAtoms

    # find indices in given selection by predicate
    ca_index = atoms.index(aName=="CA")
    print(ca_index)

.. py-exec::
    :context-id: array_indexing

    for ca in atoms[ca_index]:
        print(ca.rId, ca.name)