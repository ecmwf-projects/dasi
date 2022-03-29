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

dasi-put -c $cfgfile top1=a1,top2=a2,top3=a3,mid1=b1,mid2=b2,mid3=b3,bot1=c1,bot2=c2,bot3=c3 sample.data top1=a1,top2=a2,top3=a3,mid1=b1,mid2=b2,mid3=b3,bot1=c1,bot2=c2,bot3=c3b sample2.data

echo "dasi-get of one object"
rm -f out.data || :
dasi-get -c $cfgfile top1=a1,top2=a2,top3=a3,mid1=b1,mid2=b2,mid3=b3,bot1=c1,bot2=c2,bot3=c3 out.data
diff sample.data out.data && echo "OK"

echo "dasi-get of two objects"
rm -f out2.data || :
dasi-get -c $cfgfile top1=a1,top2=a2,top3=a3,mid1=b1,mid2=b2,mid3=b3,bot1=c1,bot2=c2,bot3=c3/c3b out2.data
diff out2.data <(cat sample.data sample2.data) && echo "OK"

echo "dasi-get of nonexistent object"
rm -f out3.data || :
dasi-get -c $cfgfile top1=a1,top2=a2,top3=a3,mid1=b1,mid2=b2,mid3=unknown,bot1=c1,bot2=c2,bot3=c3 out3.data && false
[[ ! -e out3.data ]] || die "FAIL"
echo "OK"
