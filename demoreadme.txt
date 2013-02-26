Computer Graphics Assignment 1
==============================

Objective - Draw the teapot as wireframe, using techniques from the lectures. Write readme file explaining techniques used.

The website for this assignment is the authoritative marking scheme.

The program must compile and run on DICE. If it does not, you run the risk of getting 0 marks.

Hints for the more complex possible objectives (not neccesary but might help):
- Read and understand demo1.h and demo1.cc first. If you don't understand, go to a demonstration session or ask a fellow student.
For transformations
- You may want to extend the Vector3f class (in demo1.h) to support more complex operations, as it is very basic.
- You may want to write a Matrix class


DICE Instructions for use
-------------------------
This demonstration program is written in C++.
It needs to be compiled into executable code before running.

#To compile the demo program, open up a terminal window and run the command:
g++ -o demo1 demo1.cc -lglut -lGLU -lGL
#this compiles the code and links libraries glut, GLU and openGL.


# Then, to run the program using the supplied object:
./demo1 MIT_teapot.obj 


Notes on obj files
------------------
The obj file is ascii encoded, and so can be viewed by a text editor.
Lines starting with v are vertices.
Lines starting with vn are vertex normals.
Lines starting with f are faces. In the supplied file (teapot.obj), all faces are triangles.

This is a subset of the specification. The full specification for obj files is not 
commonly followed in parsers or exporters.
The simple importer supplied does not deal with many aspects of 
the full specification including vertex normals and textures.
Further information can be found here: http://www.martinreddy.net/gfx/3d/OBJ.spec 
and here: http://en.wikipedia.org/wiki/Wavefront_.obj_file
