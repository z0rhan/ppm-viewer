# About
A simple CLI tool to convert `.ppm` images to `.png`.

**Lmao, I lost all motivation since the gnome image-viewer does support the .ppm image files.   
My file just had some rgb componentes > 255.   
Anyway, I used stbi_image_write to implement a basic version.**

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
The program will create an output.png file in the current directory.
