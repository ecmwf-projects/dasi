# [DASI](https://github.com/ecmwf-projects/dasi) Example: Electron Microscopy

This exaple mimics an electron microscopy workflow, acquiring images and visualizing them.

## Requirements

* Python (3.7 or higher),
* [pydasi](https://pypi.org/project/pydasi/) (the python interface to DASI),
* `numpy` and `matplotlib`.

## How-to

### 1. Setup:

Replace the paths in `dasi-config.yml` such that they point to your local repository:

```yaml
schema: PATH_TO_DASI_REPO/examples/cryoem/schema
- path: PATH_TO_DASI_REPO/examples/cryoem
```

Make sure the DASI library can be found:

**Option 1.** update `LD_LIBRARY_PATH` to point to `libdasi.so` (linux) or `libdasi.dylib` (macos).

**Option 2.** pass the environment variable `DASI_DIR` via command line:

```bash
DASI_DIR="PATH_TO_DASI_INSTALLATION" python3 example.py
```

### 2. Generate mock images:

```
python3 acquire.py
```

This will generate mock images and put them into the DASI. The number of images
can be set using the `-n` option on the command line (use `-h` to see the
available options).

### 3. Visualize:

```
python3 plot.py -t 202302102043
```

This will pick up previously generated images (set the `-t` parameter to the
correct timestamp, see also `-h` for other options) and generate a plot.

![sample plot](sample.png)

## DASI schema

The DASI schema uses the following keys:

* **User** The OrcID of the user
* **Project** The name of the project
* **DateTime** The timestamp of the acquisition (`YYYYMMDDHHMM`)
* **Processing** The type of processing (`g1`, `g2`)
* **Type** The type of measurement (`spa`, `tomo`, `edt`, `FIB-SEM`, `screening`)
* **Object** The type of data (`metadata`, `images`)
