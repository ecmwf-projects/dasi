#!/usr/bin/env bash

set -eux

echo "DASI_SRC_DIR=$DASI_SRC_DIR"
echo "DASI_LIB_DIR=$DASI_LIB_DIR"
echo "TESTS_DIR=$TESTS_DIR"

PYTHON_INTERP=${PYTHON_INTERP:-python3}

if [[ ! -d testenv ]] ; then
    set +x
    $PYTHON_INTERP -m venv --system-site-packages testenv
    source testenv/bin/activate
    python3 -m pip install --upgrade pip setuptools pytest
    python3 -m pip install $DASI_SRC_DIR
    deactivate
    set -x
fi

export LD_LIBRARY_PATH=$DASI_LIB_DIR:${LD_LIBRARY_PATH:-}

set +x
source testenv/bin/activate
set -x

python3 -m pytest -v $TESTS_DIR
