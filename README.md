#### Frenetic Game Engine v0.121
News: [http://ka-decimu.github.io/](http://ka-decimu.github.io/)

It is open source native C++ game framework for creating small 2D and 3D mobile games.   
It offers code-oriented workflow, 
But it simple to understand, simple to prototyping (especially, if you use the C# Integration) and experiments

Framework uses modified versions or parts of:  
- Lua
- LodePNG
- stb libraries

#### Supported platforms
- Direct3D 11 / Windows Desktop
- OpenGL ES 3 / Windows Desktop via PowerVR SDK
- Direct3D 11 / Windows Phone 8.1 / Universal Windows Platform
- OpenGL ES 3 / Linux X11

#### Compiling

All sources of engine are included relatively into fgEngine.h and fgEngine.cpp

Visual Studio 2013+:  
- include fgEngine.h to precompiled header
- add fgEngine.cpp to project
- enjoy (see for samples in fgEngineSamples repository)

GCC 4.9.3:  
- see fgEngineSamples/EmptyAxisX11 for instructions

#### RoadMap 2D

- Full support for different screen resolutions and DPI
- Sound display object
- Geometry display object
- Descriptions (scene from resource)
- Texture coordinates control
- More examples

#### RoadMap 3D

- Sound 3d object
- Custom material parameters
- Descriptions (scene from resource)
- Dirty transform mechanism
- Cache-friendly meshes
- Particle improvements
- Unified render state (hello dx12)
- Partial resource loading
- Partial render targets
- More examples

#### Public Domain

Source code only.  
Please, contact me if you are looking for engine formats or exporters
