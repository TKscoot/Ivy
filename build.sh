#! /bin/sh


if ! which /bin/time &> /dev/null
then
    make -j$(getconf _NPROCESSORS_ONLN)
else
    make -j$(getconf _NPROCESSORS_ONLN)
    tmpf="$(mktemp)"
    /bin/time -f %e -o "$tmpf" sleep 0.5
    elapsed="$(cat "$tmpf")"
    rm "$tmpf"
    echo "Build time: $elapsed"
fi
