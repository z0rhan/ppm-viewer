# About
A simple program to view .ppm format images. The support might be extended in the future.

**I decided to complete this project after some thought.**

# Build
Make sure you have **Git** and **CMake** installed.   
Clone the repository as:
```
git clone https://github.com/z0rhan/ppm-viewer.git
```
Then, run:
```
cmake -S . -B build
cmake --build build
```
This will generate the binary in `build/bin`.   

# Usage
Run the program with a .ppm file as an argument:
```
path/to/ppm-viewer path/to/image.ppm
```
For now, it will just render an empty white window.
