# How to use Doxygen

## Prerequisites
* Doxygen: http://www.doxygen.nl/download.html#srcbin
* (GraphViz: https://graphviz.gitlab.io/download/) <-- at the moment not needed

Note GraphViz installation: On windows you can install graphviz via MSYS2 since you might already have it to build the project. Simply call: pacman -S mingw-w64-x86_64-graphviz in the MSYS2 commandline window. The DOT_PATH muust then be set to {MSYS2}/mingw64/bin

## Configuration
1.  Open the Doxyfile in RayCore/doc/ with the Doxywizard
2.  Change the DOT_PATH in "Expert->Dot" to the bin-directory in your GraphViz-Folder (where you installed it)
2.  Click on the "Run"-tab and click "Run doxygen" (this will take a while, depending on your cpu performance)
3.  After completion open the Index.html or click "Show HTML output" in the Doxywizard

## Class Diagram

A generated Class Diagram can be found under "Classes->Class Hierarchy"