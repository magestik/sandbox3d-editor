sandbox3d
=========

### Introduction

A sandbox that I use to test some graphic techniques.

Implemented :
- Light Prepass,
- Simple Fog,
- Normal mapping,
- Shadow mapping (PCF),
- Tone Mapping (the scene is first rendered in HDR format),
- Bloom,
- Pick Buffer.

TODO :
- Ray of God,
- Ambient Occlusion,
- Depth of Field,
- PBR.

Render.xml is great but it should be improved to reduce state change (FBO per Technique instead of per Pass, Avoid sampler duplicate) and to be API independant !

### Build

Compile C++ code, and preprocess shaders :
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

### Dependencies

This project is designed to run on GNU/Linux and does not support Windows (maybe using GLEW would be enough to make it work).

Libraries needed :
- Qt 5
- OpenGL 3.3
- Assimp
- TinyXML2

Other requierements :
- cpp (to preprocess shaders)
- xmllint (to check render.xml)

