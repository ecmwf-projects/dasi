# Lattice QCD example

This example mimics a lattice QCD workflow, with a first step generating mock
trajectories, and a second step computing mock propagators from a given
trajectory.

## Running the example

Set a valid path for the DASI store root in `dasi-config.yml`.

```
./run_hmc.sh
```

This will generate mock trajectories and put them into the DASI. The number of
trajectories can be controlled via the `N_TRAJ` environment variable, and a
restart can be triggered by setting the `RESTART` environment variable to an
existing trajectory number. Additional metadata can be set using environment
variables, see the `run_hmc.sh` source code.

TODO: propagator computation

## DASI schema

The DASI schema uses the following keys:

* **fermion_action**: fermionic action type (`WILSON`, `CLOVER`, `HISQ`, `TwM`, ...)
* **gauge_action**: gauge action type (`WILSON`, `SYM`, `LW`, ...)
* **flavors**: number of flavors (`2`, `2+1`, `2+1+1`, `3+1`)
* **beta**: coupling parameter
* **qmass1**, **qmass2**, **qmass3**: quark masses
* **nx**, **nt**: lattice parameters
* **version**: version number (e.g. `1`)
* **trajectory**: trajectory number (1..N)
* **type**: data type (`gauge_config`, `restart`)