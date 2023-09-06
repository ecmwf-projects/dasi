Introduction
============

DASI is built on top of FDB [#]_, which has been developed at ECMWF for previous EU projects (NEXTGenIO) and has been adapted to be highly configurable for different domains.
Using FDB allows DASI to use many backends, such as high-performance POSIX backend, Ceph object-storage backend, and NVRAM backend.

.. figure:: /_static/dasi-with-wps.png

   Relationship of DASI and the other applications in the IOSEA project.


.. _dasi-config:

Configuration
-------------

An example configuration in YAML format:

.. code-block:: yaml

    ---
    schema: /path/to/schema/file
    catalogue: toc
    store: file
    spaces:
        - roots:
            - path: /path/to/output/data


Schema
------

.. _dasi-schema:

DASI is domain-agnostic, and is configured for each scientific domain using a schema.
The schema defines the metadata keys which index and identify the data within a domain.

An example schema describing a hierarchical taxonomy of metadata keys:

.. code-block:: yaml

   [ User, Laboratory?, Project
      [ Date, Processing
         [ Type, Object ]]]

.. rubric:: Footnotes

.. [#] `Fields DataBase (FDB) is a domain-specific object store <https://github.com/ecmwf/fdb>`_
