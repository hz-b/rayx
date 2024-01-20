#!/bin/bash

# Use with care!
# This script is used to rewrite the seeded part of the test suite.
# If you introduce a bug, and then run this script; the test suite will be faulty.

cd ..

prefix="Intern/rayx-core/tests/input"

for f in "PointSource" "MatrixSource" "PlaneMirror_refl" "MatrixSource_distr" "slit1" "slit2" "slit3" "slit4" "slit5"
do
    input="$prefix/${f}_seeded.rml"
    a="$prefix/${f}_seeded.csv"
    b="$prefix/${f}_seeded.correct.csv"
    ./build/bin/debug/rayx -i "$input" -x -c -f
    mv "$a" "$b"
done
