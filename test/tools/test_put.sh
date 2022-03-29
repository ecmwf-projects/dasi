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

echo "TESTING TESTING" >sample.data
echo "MORE DATA" >sample2.data

echo "dasi-put without a root"
dasi-put -c $cfgfile top1=a1,top2=a2,top3=a3,mid1=b1,mid2=b2,mid3=b3,bot1=c1,bot2=c2,bot3=c3 sample.data
diff sample.data $rootdir/top1=a1,top2=a2,top3=a3/mid1=b1,mid2=b2,mid3=b3/bot1=c1,bot2=c2,bot3=c3 && echo "OK"

echo "dasi-put with a root"
dasi-put -c $cfgfile top1=a1,top2=a2,top3=a3,mid1b=b1,mid2b=b2,mid3=b3,bot1=c1,bot2=c2,bot3=c3 sample.data
diff sample.data $rootdir/top1=a1,top2=a2,top3=a3/mid1b=b1,mid2b=b2,mid3=b3/bot1=c1,bot2=c2,bot3=c3 && echo "OK"

echo "dasi-put of multiple objects"
dasi-put -c $cfgfile top1b=a1,top2b=a2,top3b=a3,mid1=b1,mid2=b2,mid3=b3,bot1=c1,bot2=c2,bot3=c3 sample.data top1b=a1,top2b=a2,top3b=a3,mid1b=b1,mid2b=b2,mid3b=b3,bot1b=c1,bot2b=c2,bot3b=c3 sample2.data
diff sample.data $rootdir/top1b=a1,top2b=a2,top3b=a3/mid1=b1,mid2=b2,mid3=b3/bot1=c1,bot2=c2,bot3=c3 && \
diff sample2.data $rootdir/top1b=a1,top2b=a2,top3b=a3/mid1b=b1,mid2b=b2,mid3b=b3/bot1b=c1,bot2b=c2,bot3b=c3 && echo "OK"