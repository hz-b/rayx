#!/bin/sh

[[ ! -d .git ]] && echo "You need to be in the root directory of the repo" && exit

find Intern Tests '(' -name "*.cpp" -or -name "*.h" ')' -exec clang-format -i {} \;
