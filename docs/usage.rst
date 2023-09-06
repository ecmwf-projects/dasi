Usage
=====

Python Usage
------------

The Python interface to DASI is called **pydasi**.


.. note::

   Please refer to :ref:`install_pydasi` on how to install **pydasi**.


Simple Archive
~~~~~~~~~~~~~~

We use the :py:func:`dasi.dasi.Dasi.archive` function to save example data ``SIMPLE_DATA`` using the metadata ``key`` as:

.. code-block:: python

   from dasi import Dasi
   dasi = Dasi("config.yaml")
   key=["User":"muster", "Project":"simple", "Date":"16-03-23", "Type":"text"]
   data="SIMPLE_DATA"
   dasi.archive(key, data)

where, the example schema is:

.. code-block:: yaml

   [ User, Project
      [ Date
         [ Type ]]]

See :ref:`dasi-config` for config yaml..

Reference
~~~~~~~~~

.. autoclass:: dasi.dasi.Key

.. autofunction:: dasi.dasi.Dasi.archive
