# [DASI](https://github.com/ecmwf-projects/dasi) Example: Electron Microscopy

This exaple mimics an electron microscopy workflow, acquiring images and visualizing them.

## Requirements

* Python (3.7 or higher),
* [pydasi](https://pypi.org/project/pydasi/) (the python interface to DASI),
* `numpy` and `matplotlib`.

## How-to

### 1. Setup:

Set a valid path for the DASI store root in `dasi-config.yml`.

Make sure the DASI library can be loaded, updating `LD_LIBRARY_PATH` if needed.

### 2. Generate mock images:

```
python3 acquire.py
```

This will generate mock images and put them into the DASI. The number of images
can be set using the `-n` option on the command line (use `-h` to see the
available options).

### 3. Visualize:

```
python3 plot.py -t 202206071435
```

This will pick up previously generated images (set the `-t` parameter to the
correct timestamp, see also `-h` for other options) and generate a plot.


## DASI schema

The DASI schema uses the following keys:

* **User** The OrcID of the user
* **Project** The name of the project
* **DateTime** The timestamp of the acquisition (`YYYYMMDDHHMM`)
* **Processing** The type of processing (`g1`, `g2`)
* **Type** The type of measurement (`spa`, `tomo`, `edt`, `FIB-SEM`, `screening`)
* **Object** The type of data (`metadata`, `images`)
