# Quadtree Demo

## Description

Simple interactive visualization of a quadtree using quadtrees with space for
only one (entity) per quad.

Built using an abandoned game engine I wrote using SDL2.

Press F5 to bring up some debug information and scroll to cull the view towards
the centre.

## Compilation

### Linux

- Install sdl2-dev, sdl2-image, sdl2-ttf and sdl2-mixer from your package manager.

### Windows

- Requires mingw32, gcc, gnumake and sdl2(mingw32).

### macOS

- Install sdl2, sdl2_image, sdl2_ttf and sdl2_mixer using the brew package manager.

Run - `make` to build the demo normally, - `make -j` for compiling on multiple
cores and optionally add `debug` to enable the `-g` flag and include the debug
prints/features in the executable (see the makefile for more options).
