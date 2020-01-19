[DESIRE](https://github.com/nyaki-HUN/DESIRE)
=============================================

Development Experiment for a Simple and Robust Engine

What is it?
-----------
This is an experimental project to create an Engine which makes game development easier.
The main focus is on creating a powerful and simple to use framework.

This is not a plug-and-play engine. If you think about cars, other game engines are like having the whole car and then customizing it. DESIRE is more like a high-performance engine where you still need to build the car around.  

Documentation
-------------

### Module support
The architecture of the engine emphasizes separating the functionalities into independent, interchangeable pieces that we call modules.  
Each module contains everything necessary to execute only one aspect of the desired functionality.

### Supported platforms

(DESIRE is meant to be a cross-platform engine, but it is under development)

 * Windows 10

Modules
-------

### Compression

 * [Lizard](https://github.com/inikep/lizard) *(WIP)*
 * [LZ4](http://lz4.github.io/lz4) *(WIP)*
 * [zlib-ng](https://github.com/Dead2/zlib-ng)
 * [zstd](http://facebook.github.io/zstd)

### Network
 * [curl](https://curl.haxx.se/) *(WIP)*

### Physics

* [Box2D](http://box2d.org) *(WIP)*
* [Bullet](http://bulletphysics.org) *(WIP)*
* [PhysX](https://developer.nvidia.com/physx-sdk) *(WIP)*

### Render

 * [bgfx](https://bkaradzic.github.io/bgfx)
 * [Direct3D11](https://docs.microsoft.com/en-us/windows/desktop/direct3d11/atoc-dx-graphics-direct3d-11)
 * [Direct3D12](https://docs.microsoft.com/en-us/windows/desktop/direct3d12/direct3d-12-graphics) *(WIP)*

### Resource loader

 * [Assimp](http://www.assimp.org)

### Script

 * [AngelScript](http://www.angelcode.com/angelscript)
 * [Lua](https://www.lua.org)
 * [Squirrel](http://www.squirrel-lang.org)

### UI

 * [imgui](https://github.com/ocornut/imgui)

Contact
-------

nyakacska.zoltan@gmail.com

[License (BSD 2-Clause)](../master/LICENSE)
-------------------------------------------

<a href="http://opensource.org/licenses/BSD-2-Clause" target="_blank">
<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">
</a>

	Copyright (c) 2017, Zoltan Nyakacska
	All rights reserved.
	
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	
	* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
	
	* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
