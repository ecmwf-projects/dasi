#!/usr/bin/env bash

set -eu

RESTART=${RESTART:-}
N_TRAJ=${N_TRAJ:-5}

BEGIN=0
if [[ -n "$RESTART" ]] ; then
    set +e
    BEGIN=$(grep -o "RESTART [1-9][0-9]*" $RESTART | cut -d' ' -f2)
    if [[ $? -ne 0 ]]; then
        echo "Wrong restart file '$RESTART'" >&2
        exit 1
    fi
    set -e
    echo "Restarting from trajectory $BEGIN"
fi
END=$((BEGIN + N_TRAJ))

for ((traj=BEGIN+1; traj <= END; ++traj)) ; do
    echo "Trajectory $traj/$END"
    echo "TRAJECTORY $traj" >config_$traj.lime
    echo "RESTART $traj" >restart_$traj.xml
done