COMP 371 Final Project
Written by Luiza Nogueira Costa
ID 40124771

Project was written and compiled using Visual Studio 2022 for Windows 11.

The dynamic library manager vcpkg was used to import the necessary OpenGL libraries. These include:

	OpenGL::GL
	OpenGL::GLU
	GLEW::glew
	glfw
	assimp::assimp

It may be necessary to modify the CMake configuration to run this project on other machines. I have included a copy of my own settings to serve as a guideline. 

Note the file organization of the assets to ensure that all paths are correct. 

Due to the high polygon count of the scene, the initial loading of the program can take upwards of 5 minutes. This is expected behavior as ASSIMP must first process the entire .obj file beforehand. 

Commands:

AWSD + mouse used to move around the scene.

Mouse scroll to zoom.

Toggle volumetric light: R

Toggle shadowmapping: Z

Change lights: Q

Packaged executable to be found in the out/ subfolder. 