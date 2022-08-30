# What is Code Coverage:
Analyzing your code Coverage is a necessary step in devloping complex big projects.

Getting the code's coverage is simply generating a summary of how much a piece of code is executed and "covered".

Code Coverage offers line, function, and branch analysis.

Generally, the most reasonable method is to check how much code your Testsuites cover.

# Requirements 

_Please Note: This has only been tested on UNIX System (Ubuntu)_ 

Code Coverage is supported by GNU GCC and LLVM. Make sure that one of these build tools is installed. Gcov, lcov and genhtml are packages that are also needed. For more information, you can check the usage of gcov (GCC) [here](https://gcc.gnu.org/onlinedocs).

You can also use gcovr as an alternative to gcov. 

# How it works
Code Coverage only works if the main function returns 0. (The Test Suite returns 0 if all testcases pass). 

The test suite should run only in DEBUG Mode and with all optimizations turned off "-O0" for Coverage. Warnings will pop if not.

With the correct gcov and -fdump added to `CXX_COMPILER_FLAGS`, running the Testsuite will generate `*.gcno` and `*.gch` files meant for lcov. The file generation and flags are handled by a CMAKE submodule. An `*.info` file is then created and transformed afterwards to interactable `index.html` page on the web browser. Configuration information on the CMAKE Output window show up as the submodule defaults to verbose.


# How to enable 
## Visual Stuido Code
- Uncomment the code under `#CodeCoverage` in the CMakeLists.txt on the project's root directory, to set the option `BUILD_WITH_GCOV`.
- (Optional) You can also build with `DBUILD_WITH_GCOV` instead of last step.
- Make sure to choose RAYX_tst as target in Debug Mode.
- Once ran, `cd build` and run: 

`lcov --capture --directory . --output-file coverage.info`

`genhtml coverage.info --output-directory coverage`

- A new file in `build/` called coverage, with and `index.html` can be opened to see Code Coverage.

## JetBrains Clion
- Chose the Google Test "RAYX_tst | Debug-Coverage" as configuration, if first time doing this you should only see "RAYX_tst | Debug " as option, chose that instead.
- Near the __Run__ button, you should see "Run "RAYX_tst" with Coverage".
- If configration is missing, let CLion handle the creation of a new Cmake configuration.
- Once all testsuites are successfuly ran, a dialog box  of the Line, Branch coverages pops out. Notice also the colors in the editor.


