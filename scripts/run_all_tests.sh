#!/bin/bash

mkdir -p build/build_docker && cd build/build_docker
cmake ../cmake
make -j
cd ../../

echo "Creating diffs..."
mkdir -p output && mkdir -p output/diffs
echo "------------------Testing all cases------------------"
TESTS="data/task1/*.json"; for i in $TESTS; do build/build_docker/bin/task1 -t 4 "$i" || true; echo "Ran task ${i}"; done
OUTPUTS="output/task1/*"; for i in $OUTPUTS; do compare -compose src "$i" "output/ref/${i##*/}" "output/diffs/${i##*/}" || true; echo "Output diffed ${i}"; done