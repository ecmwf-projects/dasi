Usage
=====

Python Usage
------------

The Python interface to DASI is called **pydasi**.


.. note::

   Please refer to the :ref:`install_pydasi` about the installation of **pydasi**.


Simple Archive
~~~~~~~~~~~~~~

We use the ``dasi.archive(key, data)`` function to save a simple data
``SIMPLE_DATA`` using the ``key`` metadata as:

.. code-block:: python

   from dasi import Dasi
   dasi = Dasi("config.yaml")
   key=["User":"muster","Project":"simple","Date":"16-03-23","Type":"text"]
   data="SIMPLE_DATA"
   dasi.archive(key, data)


where, the simple schema is:

.. code-block:: yaml

   [ User, Project
      [ Date
         [ Type ]]]


Reference:

.. autoclass:: dasi.Key

.. .. autofunction:: dasi.Dasi.archive

.. The ``kind`` parameter should be either ``"meat"``, ``"fish"``,
.. or ``"veggies"``. Otherwise, :py:func:`dasi.archive`
.. will raise an exception.
..
.. .. autoexception:: dasi.DASIException
