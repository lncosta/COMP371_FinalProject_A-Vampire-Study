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

Due to the high polygon count of the scene, the initial loading of the program can take upwards of 5 minutes. This is expected behavior as ASSIMP must first process the entire .obj file beforehand. 

Commands:

AWSD + mouse used to move around the scene.

Mouse scroll to zoom.

Toggle volumetric light: R

Toggle shadowmapping: Z

Change lights: Q

Video Demo available at: https://www.youtube.com/watch?v=JzEjXX5c5sE
