#!/bin/bash

[[ ! -d .git ]] && echo "You need to be in the root directory of the repo" && exit

changed=0

for f in $(find Intern -name "*.cpp" -or -name "*.h")
do
    oldhash=$(md5sum "$f")
    clang-format -i "$f"
    newhash=$(md5sum "$f")
    if [[ ! "$oldhash" == "$newhash" ]]; then
        echo Formatting fixed: $f
        changed=1
    fi
done

exit $changed
