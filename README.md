# Aether3D Game Engine
This codebase will evolve into the next generation [Aether3D](http://twiren.kapsi.fi/aether3d.html). More info: http://bioglaze.blogspot.fi/2014/12/planning-aether3d-rewrite-for-2015.html

# Features

  - Windows, OS X, iOS and Linux support.
  - OpenGL 4.1, 4.5, D3D12 (WIP) and Metal renderers.
  - Component-based game object system.
  - Oculus Rift support.
  - Sprite rendering, texture atlasing and batching.
  - Bitmap and Signed Distance Field font rendering using BMFont fonts.
  - Variance shadow mapping.
  - Audio support for .wav and .ogg.
  - Custom model format with .obj and Blender exporter.
  - Virtual file system for .pak files.
  - XBox controller support.
  - Cross-Platform scene editor.

# Planned Features

  - Vulkan renderer.
  - Clustered Forward lighting.
  - Most of the features in my [previous engine](http://twiren.kapsi.fi/aether3d.html)

# Build

  - After building build artifacts can be found in aether3d_build next to aether3d.
  - Grab the [sample asset archive](http://twiren.kapsi.fi/files/aether3d_sample_v0.4.zip) and extract it into aether3d_build/Samples after building.

## Windows

  - Open the project in Engine\VisualStudio_GL45 and build it. For MinGW you can use Engine/Makefile.
  - Build and run Samples\01_OpenWindow.

## OS X / GNU/Linux

  - Open the project Engine/Aether3D_OSX in Xcode and build it or run the Makefile.
  - Open the project Samples/01_OpenWindow/OpenWindow.xcodeproj and run it or run the Makefile.

## iOS
  - Build Aether3D_iOS in Engine. It creates a framework.
  - Open Samples/02_iOS_Hello and add the framework into the project.

# License

The engine is licensed under zlib license.

Third party library licenses are:

  - stb_image.c is public domain
  - stb_vorbis.c is public domain
  - glxw is under zlib license
  - OpenAL-soft is under LGPLv2 license

# Roadmap/internal TODO

https://docs.google.com/document/d/1jKuEkIUHiFtF4Ys2-duCNKfV0SrxGgCSN_A2GhWeLDw/edit?usp=sharing
