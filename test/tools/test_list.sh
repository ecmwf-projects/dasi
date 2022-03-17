#!/usr/bin/env bash

set -eux

wdir=$(pwd -P)
rootdir=$wdir/shelf
cfgfile=$wdir/dasi-config.yml

if [[ -n "${DASI_BIN:-}" ]] ; then
    export PATH=$DASI_BIN:$PATH
fi

if [[ -e $rootdir ]] ; then
    rm -rf $rootdir
fi

cat >expected.list <<EOF
{bot1:c1, bot2:c2, bot3:2, mid1:b1, mid2:b2, mid3:b3, top1:a1, top2:a2, top3:a3}
{bot1:c1, bot2:c2, bot3:4, mid1:b1, mid2:b2, mid3:b3, top1:a1, top2:a2, top3:a3}
{bot1:c1, bot2:c2, bot3:6, mid1:b1, mid2:b2, mid3:b3, top1:a1, top2:a2, top3:a3}
{bot1:c1, bot2:c2, bot3:8, mid1:b1, mid2:b2, mid3:b3, top1:a1, top2:a2, top3:a3}
{bot1:c1, bot2:c2, bot3:10, mid1:b1, mid2:b2, mid3:b3, top1:a1, top2:a2, top3:a3}
EOF

for i in $(seq 1 10) ; do
    echo "TEST $i" >sample.data
    dasi-put -c $cfgfile top1=a1,top2=a2,top3=a3,mid1=b1,mid2=b2,mid3=b3,bot1=c1,bot2=c2,bot3=$i sample.data
done

echo "dasi-list with full query"
rm -f out.list || :
dasi-list -c $cfgfile top1=a1,top2=a2,top3=a3,mid1=b1,mid2=b2,mid3=b3,bot1=c1,bot2=c2,bot3=2/4/6/8/10 \
    | tail -n +2 >out.list
diff expected.list out.list && echo "OK"
