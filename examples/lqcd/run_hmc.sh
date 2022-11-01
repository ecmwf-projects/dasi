#!/usr/bin/env bash

set -eu

FERMION_ACTION=${FERMION_ACTION:-CLOVER}
GAUGE_ACTION=${GAUGE_ACTION:-WILSON}
FLAVORS=${FLAVORS:-2}
BETA=${BETA:-3.6}
QMASS1=${QMASS1:-0.005}
QMASS2=${QMASS2:-0.01}
QMASS3=${QMASS3:-}
NX=${NX:-64}
NT=${NT:-128}
VERSION=${VERSION:-1}

RESTART=${RESTART:-}
N_TRAJ=${N_TRAJ:-5}

qmass="qmass1=$QMASS1,qmass2=$QMASS2"
case $FLAVORS in
2 | 2+1 | 3+1)
    : ;;
2+1+1)
    qmass="$qmass,qmass3=$QMASS3" ;;
esac

basekey=""
basekey+="fermion_action=$FERMION_ACTION,"
basekey+="gauge_action=$GAUGE_ACTION,"
basekey+="flavors=$FLAVORS,"
basekey+="beta=$BETA,"
basekey+="$qmass,"
basekey+="nx=$NX,"
basekey+="nt=$NT,"
basekey+="version=$VERSION"

BEGIN=0
if [[ -n "$RESTART" ]] ; then
    restartkey="$basekey,trajectory=$RESTART,type=restart"

    dasi-get -c dasi-config.yml $restartkey init.xml

    BEGIN=$RESTART
    RESTART=init.xml
fi

export RESTART N_TRAJ
./lqcd_hmc.sh

rm -f init.xml

END=$((BEGIN + N_TRAJ))
for ((traj=BEGIN+1; traj <= END; ++traj)) ; do
    dasi-put -c dasi-config.yml "$basekey,trajectory=$traj,type=gauge_config" config_$traj.lime
    dasi-put -c dasi-config.yml "$basekey,trajectory=$traj,type=restart" restart_$traj.xml
    rm -f config_$traj.lime restart_$traj.xml
done