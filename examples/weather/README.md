# Weather forecasting example

This example mimics a weather forecasting workflow, with a "forecast" part
generating data for a fake ensemble forecast, and a "mean" part computing
per-timestep means of the forecast output.

## Building

```
mkdir build
cd build
ecbuild -Ddasi_ROOT=/path/to/dasi ..
make -j
```

## Running the example

Set a valid path for the DASI store root in `dasi.yml`.

The example can be run directly from its `build` directory. First run the "forecast":

```
bin/forecast -c ../dasi.yml
```

This will generate mock forecast data and put them into the DASI. Options can be
set to manipulate the metadata, see the output of `forecast -h` for the usage.
Then, run the "mean" for a chosen timestep:

```
bin/mean -c ../dasi.yml -t 1234 -s 2
```

This will compute ensemble means for this timestep, and feed them back into the
DASI.  The `-t` argument should match the forecast time (defaulting to the
current time when the executable was called, in HHMM format). The `-s` argument
is the step number. See the output of `mean -h` for the possible arguments.

## DASI schema

The DASI schema uses the following keys:

* **type**: data type (`ensemble` for the forecast and `ens_mean` for the means)
* **version**: experiment version (4-character string, defaults to `0001`)
* **date**: forecast date (`YYYYMMDD` string, defaults to the current date)
* **time**: forecast time (`HHMM` string, default to the current time)
* **number** (only used for `type=ensemble`): ensemble member number (0..N-1)
* **step**: time step number (1..N)
* **level**: vertical level number (0..N-1)
* **param**: physical variable (`p`, `t`, `u`, `v`)
