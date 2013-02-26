#include <iostream>
#include <fstream>
#include <cstring>
#include <GL/glut.h>
#include "demo1.h"
#include <float.h>

using namespace std;

//window resolution (default 640x480)
const int nRows = 640;
const int nCols = 480; 

double yaw = 0;
double roll = 0;
double pitch = 0;
double xmod, ymod, zmod  = 0;
double scalemod = 1;
int mode = 0;
bool once = false;
int mx, my  = 0;

//The mesh global variable
TriangleMesh trig;



//This function loads an obj format file
//This is a utility and should not have to be modified for teapot.obj (the assignment).
void TriangleMesh::loadFile(char * filename)
{
	ifstream f(filename); // the file stream, for reading in a file

	//if the file was unabled to be opened (ifstream pointer is NULL), exit with error message.
	if (f == NULL) {
		cerr << "failed reading polygon data file: " << filename << endl;
		exit(1);
	}

	char buf[1024];
	char header[100];
	float x,y,z;
	float xmax,ymax,zmax,xmin,ymin,zmin;
	int v1, v2, v3, n1, n2, n3;

	xmax =-10000; ymax =-10000; zmax =-10000;
	xmin =10000; ymin =10000; zmin =10000;
	Vector3f av;
	av[0] = av[1] = av[2] = 0.f;

	while (!f.eof()) {
		    f.getline(buf, sizeof(buf));
		    sscanf(buf, "%s", header);  

		    if (strcmp(header, "v") == 0) {
				sscanf(buf, "%s %f %f %f", header, &x, &y, &z);  

				_v.push_back(Vector3f(x,y,z));

				av[0] += x; av[1] += y; av[2] += z;

				if (x > xmax) xmax = x;
				if (y > ymax) ymax = y;
				if (z > zmax) zmax = z;

				if (x < xmin) xmin = x;
				if (y < ymin) ymin = y;
				if (z < zmin) zmin = z;
		    }
		    else if (strcmp(header, "f") == 0) {
				sscanf(buf, "%s %d %d %d", header, &v1, &v2, &v3);
				
				Triangle trig(v1-1, v2-1, v3-1);
				_trig.push_back(trig);
				// tells vertices which triangles they are a part of
				_v[v1-1].relatedTriangles.push_back(_trig.size() -1);
				_v[v2-1].relatedTriangles.push_back(_trig.size() -1);
				_v[v3-1].relatedTriangles.push_back(_trig.size() -1);
		    }
 	}

	_xmin = xmin; _ymin = ymin; _zmin = zmin;
	_xmax = xmax; _ymax = ymax; _zmax = zmax;
	
	// Add a floor composed of 2 triangles

	int stat = _v.size();
	int trigno = _trig.size();
	Vector3f floor_1(-5, -0.3, -5);
	Vector3f floor_2(-5, 0,  5);
	Vector3f floor_3( 5, -0.3, -5);
	Vector3f floor_4( 5, 0,  5);

	floor_1.relatedTriangles.push_back(trigno);
	floor_2.relatedTriangles.push_back(trigno); 
	floor_3.relatedTriangles.push_back(trigno); 

	floor_2.relatedTriangles.push_back(trigno+1); 
	floor_3.relatedTriangles.push_back(trigno+1); 
	floor_4.relatedTriangles.push_back(trigno+1); 

	Triangle trig1(stat, stat+1, stat+2);
	Triangle trig2(stat+3, stat+2, stat+1);



	_v.push_back(floor_1); _v.push_back(floor_2); _v.push_back(floor_3); _v.push_back(floor_4);
	_trig.push_back(trig1);
	_trig.push_back(trig2);
	

	float range; 
	if (xmax-xmin > ymax-ymin) range = xmax-xmin;
	else range = ymax-ymin;

	for (int j = 0; j < 3; j++) av[j] /= _v.size();

	for (int i = 0; i < _v.size(); i++) 
	{
		for (int j = 0; j < 3; j++) _v[i][j] = (_v[i][j]-av[j])/range*400;  
	}
	cout << "Number of triangles: " << _trig.size() << ", number of vertices: " << _v.size() << endl;
	f.close();
	preComputeNormals();
	Lighting::setLights();

};

void TriangleMesh::preComputeNormals() 
{
	
	for(int v = 0; v < _v.size(); v++)
		{
			
			for(int i = 0; i < _v[v].relatedTriangles.size(); i++) {
				// Goes through every vertice, and every triangle it is associated with.
				// Calculates the normal of that triangle and adds it to the vertices normal accumulator
				// Once every triangle has been inspected, each vertice can produce an average normal
				Vector3f norm(0,0,0);
				int trig = _v[v].relatedTriangles[i];
				Vector3f& a = _v[_trig[trig]._vertex[0]];
				Vector3f& b = _v[_trig[trig]._vertex[1]];
				Vector3f& c = _v[_trig[trig]._vertex[2]];
				
				Operations::FaceNormal(norm, a, b, c);
				norm.Normalise();
				
				_trig[trig].normal[0] = norm[0];
				_trig[trig].normal[1] = norm[1];
				_trig[trig].normal[2] = norm[2];
				_v[v].addNormal(norm[0], norm[1], norm[2]);
				
			}
			
					
					
			
			
		}

		for(int c = 0; c < _v.size(); c++)
		{
			_v[c].NormaliseNormal();
			if(_v[c].count < 1) 
			{
				std::cout<<"Vertex "<<c<<" has no associated triangles"<<std::endl;
			}
		}
}

/*--- Display Function ---*/
//The main display function.
//This allows you to draw pixels onto the display by using GL_POINTS.
//Drawn every time an update is required.
//Students: This is the main file you'll need to modify or replace.
//The idea with this example function is the following:
//1)Clear the screen so we can draw a new frame
//2)Calculate the vertex points for each triangle and draw them (vertices only)
//3)Flush the pipeline so that the instructions we gave are performed.
void demoDisplay()
{
	if(once){return;}
	double zbuffer[nRows][nCols];
	for(int i = 0; i < nRows; i++) {
		for(int j= 0; j < nCols; j++) {
			zbuffer[i][j] = FLT_MAX;
		}
	}
	Vector3f camera(0,0,1);
	glClear(GL_COLOR_BUFFER_BIT); // Clear OpenGL Window

	int trignum = trig.trigNum(); //Number of triangles
	Vector3f v1,v2,v3; // Vector objects to hold the returned vertex values

	glColor3f(1,1,1);  // The colour we will be drawing is white (red = 1, green = 1, blue = 1).

	//
	// for each triangle, get the location of the vertices,
	// project them on the xy plane (remove z value), and color the corresponding pixel white
	//
	for (int i = 0 ; i < trignum; i++)  
	{
		/* getting the vertices of the triangle i */
		trig.getTriangleVertices(i, v1,v2,v3); //For the Java programmers, v1,v2 and v3 are the return values here.

		//
		// An example:
		// Colouring the pixels at the vertex location 
		// (just doing parallel projectiion to the xy plane.) 
		// students: only use glBegin(GL_POINTS) for rendering the scene  
		// GL_LINES, GL_POLYGONS and similar may NOT be used in your submission.
		//

		Vector3f a1 = v1;
		Vector3f a2 = v2;
		Vector3f a3 = v3;

		// Allows left/right rotation of teapot
		Transformations::Rotate(a1, 0, yaw, 0);
		Transformations::Rotate(a2, 0, yaw, 0);
		Transformations::Rotate(a3, 0, yaw, 0);


		glBegin(GL_POINTS);	
			// First two for loops loop through each point in the triangles bounding box
			for(int x = Operations::MinX(a1, a2, a3); x <= Operations::MaxX(a1,a2,a3); x++)
					{
						for(int y = Operations::MinY(a1, a2, a3); y <=Operations::MaxY(a1, a2, a3); y++)
						{
							Vector3f bar(0,0,0);
							// Calculate barycentric coordinates
							Operations::Barycentric(bar, a1, a2, a3, x, y);
							
							// Is the current point inside the triangle?
							bool inside = bar[0]>=0 && bar[0]<=1 && bar[1] >= 0 && bar[1] <= 1 && bar[2] >= 0 && bar[2] <= 1;
							if(inside) {
								// Calulate the point on the triangles' depth
								double  depth = a1[2]*bar[0] + a2[2]*bar[1] + a3[2]*bar[2];
								//std::cout<<x<<" "<<y<<" "<<depth<<std::endl;
								int screenx = x + nCols/2;
								int screeny = y + nRows/2;
								if(screenx >= 0 && screenx < 640 && screeny >= 0 && screeny < 480) 
								{
									// if current point is closer to the screen, then draw
									if(zbuffer[screenx][screeny] > depth) 
									{
										Light ambient(0,0,0,0.25f, 0.25f, 0.25f);
										zbuffer[screenx][screeny] = depth;

										// Mode 0 is for flat shading, mode 1 is for phong
										if(mode==0) 
										{
											// Calculate normal for the triangle, and normalise
											Vector3f FaceNorm(0,0,0);
											Operations::FaceNormal(FaceNorm, a1, a2, a3);
											FaceNorm.Normalise();
											// if the normal is pointing towards the camera
											// ie Back Face Culling
											if(Operations::DotProduct(FaceNorm, camera) < 0 )
											{


												// Calculate Lighting
												Vector3f color = Lighting::phongIllumination(FaceNorm, FaceNorm, 0.15f, 0.30f, 0.50f, ambient, 1 );
												
												if(mx==screenx && my==screeny) {
													std::cout<<"TRIG SELECTED = "<<i<<" : "<<mx<<", "<<my<<std::endl;
													std::cout<<"NORMAL: "<<FaceNorm[0]<<" "<<FaceNorm[1]<<" "<<FaceNorm[2]<<std::endl;

												}

												// Draw!
												glColor3f(color[0], color[1], color[2]);
												
												glVertex2i(x, y);
													
												
											}
										}
										if(mode==1)
										{
											Vector3f na1(0,0,0);
											Vector3f na2(0,0,0);
											Vector3f na3(0,0,0);
											// Fetch vertice normals
											v1.getNormal(na1);
											v2.getNormal(na2);
											v3.getNormal(na3);
											// Rotate them to match model rotation
											Transformations::Rotate(na1, roll, yaw, pitch);
											Transformations::Rotate(na2, roll, yaw, pitch);
											Transformations::Rotate(na3, roll, yaw, pitch);

											// Normalise
											na1.Normalise();
											na2.Normalise();
											na3.Normalise();
											float dist = 15;
											bool mid = true;


											Vector3f interNorm(1,1,1);
											// Interpolate the normals to find the normal of the current point
											Operations::interpolateNormals(interNorm, bar, na1, na2, na3);

											if(mx==screenx && my==screeny) {
													std::cout<<"TRIG SELECTED = "<<i<<" : "<<mx<<", "<<my<<std::endl;
													std::cout<<"NORMAL: "<<interNorm[0]<<" "<<interNorm[1]<<" "<<interNorm[2]<<std::endl;

											}


											// Back Face Culling, checks if normal of point is facing the camera
											if(Operations::DotProduct(interNorm, camera) < 0 )
											{
												
												interNorm.Normalise();

												// Calculate Lighting
												Vector3f cp = Lighting::phongIllumination(interNorm, interNorm, 0.350f, 0.30f, 0.50f, ambient, 64);

												
												// Draw"
												glColor3f(cp[0], cp[1], cp[2]);
												
												glVertex2i(x, y);


											}
										}

										
									}
								} 
								
							}
						}
					}
		glEnd();	
	}
	once = false;
	glFlush();// Output everything (write to the screen)
}


//This function is called when a (normal) key is pressed
//x and y give the mouse coordinates when a keyboard key is pressed
void demoKeyboardHandler(unsigned char key, int x, int y)
{
    if(key == 'm')
    {
        cout << "Mouse location: " << x << " " << y << endl;
        mx = x;
        my = y;
    }

    cout << "Key pressed: " << key << endl;

}
void processSpecialKeys(int key, int x, int y) {

	
		
			switch(key) {
			
			case GLUT_KEY_RIGHT :
					yaw += 0.1f;
					break;
		
			case GLUT_KEY_LEFT :
					yaw -= 0.1f;
					break;
			
		
			case GLUT_KEY_F1:
					mode =0;
					once = false;
					break;
				
			case GLUT_KEY_F2:
					mode=1;
					once = false;
					break;

			case GLUT_KEY_F3:
					mode=2;
					break;

			case GLUT_KEY_F4:
					mode=3;
					break;
				
			case GLUT_KEY_F12:
					yaw = roll = pitch = 0;
					
					break;
			}
			
	
}

void idle()
{
	
	glutPostRedisplay();
	

}


//Program entry point.
//argc is a count of the number of arguments (including the filename of the program).
//argv is a pointer to each c-style string.
int main(int argc, char **argv)
{

    cout << "Computer Graphics Assignment 1 Demo Program" << endl;

	if (argc >  1)  
	{
		trig.loadFile(argv[1]);
	}
	else 
	{
        cerr << "Usage:" << endl;
		cerr << argv[0] << " <filename> " << endl;
		exit(1);
	}

	//initialise OpenGL
	glutInit(&argc, argv);
	//Define the window size with the size specifed at the top of this file
	glutInitWindowSize(nRows, nCols);
	//Create the window for drawing with the title "SimpleExample"
	glutCreateWindow("CG-CW1");
	//Apply a 2D orthographic projection matrix, allowing you to draw
	//directly to pixels
	gluOrtho2D(-nRows/2, nRows/2, -(float)nCols/2,  (float)nCols/2);

	//Set the function demoDisplay (defined above) as the function that
	//is called when the window must display
	glutDisplayFunc(demoDisplay);// Callback function
    //similarly for keyboard input
    glutKeyboardFunc(demoKeyboardHandler);	
    glutSpecialFunc(processSpecialKeys);
    glutIdleFunc(idle);
	//Run the GLUT internal loop
	glutMainLoop();// Display everything and wait
}
