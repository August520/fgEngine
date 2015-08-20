#### Frenetic Game Engine v0.10

It is open source native C++ game framework for creating small 2D and 3D mobile games.
 
It offers code-oriented workflow, but it simple to understand, simple to prototyping, simple to experiments

It uses modified versions or parts of:

- Lua
- LodePNG
- stb libraries

#### Supported platforms
- Direct3D 11 / Windows Desktop (debug and gameplay writing)
- Direct3D 11 / Windows Phone 8.1/10
- OpenGL ES 3 / in progress

#### Compiling

All sources of engine are included relatively into fgEngine.h/fgEngine.cpp

Now it compiles in Visual Studio only:

- include fgEngine.h to precompiled header
- add fgEngine.cpp to project or include to other *.cpp
- enjoy (see for samples in wiki)

#### RoadMap 2D

- Correcting coordinate system behaviour for non-interface Sprite  
- TextField display object
- Sound display object
- Texture coordinates control
- Scissor rectangle

#### RoadMap 3D

- unified render state (hello dx12)
- OpenGL wrapper
- Dirty transform mechanism
- Cache-friendly meshes
- Partial resource loading
- Partial render targets

#### Public Domain

Source code only.
Please, contact me if you are looking for engine formats or exporters
