#!/bin/bash

# Ever changed anything in Ray-X? maybe changed the event ID of something? or maybe the order in the CSV Writer format?
# Many changes like this will cause the __whole__ set-seeded part of the test suite to fail.
# Just because the stored "correct" solution just isn't correct anymore.

# But fear not, overwrite-correct.sh is here to help!
# It allows you to re-generate the "correct" solutions based on what rayx currently outputs.

### USE WITH CARE! ###

# With great power comes great responsibility!
# If you introduce a bug, and then run this script; the test suite will be filled with __wrong solutions__!
# It is advisable to check using `git diff` / plotting whether the changes that this script made are the ones you intended.

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
