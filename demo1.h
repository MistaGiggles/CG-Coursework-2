#ifndef _rt_H
#define _rt_H

//These include basic math and vector support, as well as
//a helper library for OpenGL.
//For the Java developers amongst you, this is slightly similar
//to an import statement in Java.
#include <cmath>
#include <vector>
#include <GL/glut.h>

//this allows you to not have to write the std namespace before each
//function from the std library.
//for instance: std::vector becomes vector.
using namespace std; 

// -- forward declarations of the classes --
//Vector3f is a class to store 3 values, often used for x,y,z coordinates 
class Vector3f;

//the Triangle class stores an index to 3 vertices. These are internally stored
//as indices to a vector of vertices (which are Vector3f).
class Triangle;

//The TriangleMesh class actually stores the geometry of an object.
//It stores the vertices and a list of triangles (indices into the vertex list).
class TriangleMesh;

//--- Definitions for the classes ---
class Vector3f {

	
	
	
	public:

	float _item[3];//item internally stores the 3 floating point values
	vector<int> relatedTriangles;
	float _nx, _ny, _nz;
	float count;
	bool isNormalised;
	float _normalised_x;
	float _normalised_y;
	float _normalised_z;
	
	void addNormal(float x, float y, float z) 
	{
		//std::cout<<"ADDING NORMAL " <<x<<" "<<y<<" "<<z<<" "<<std::endl;
		_nx += x;
		_ny += y;
		_nz += z;
		count += 1;
	}



	//A Vector3f is indexed like an array, with a range of 0 to 2.
	//When we are using these for cartesian coordinates, index 0 is
	//the x coordinate, 1 is y, and 2 is z.
	float & operator [] (int i) {
		return _item[i];
    	}

	//a constructor for the Vector3f class. 
	Vector3f(float x, float y, float z) 
	{  _item[0] = x ; _item[1] = y ; _item[2] = z; 
		_nx = 0;
		_ny = 0;
		_nz = 0;
		count = 0;
		isNormalised=false;};

	//Another constructor, initialising to 0,0,0
	Vector3f() 
	{
		_item[0] = 0.0f ; _item[1] = 0.0f ; _item[2] = 0.0f; 
		_nx = 0;
		_ny = 0;
		_nz = 0;
		count = 0;
		isNormalised=false;
	}
	void print()
	{
		std::cout<<_item[0] << ", " << _item[1] << ", " << _item[2] << std::endl;
	}

	void addTriangle(int t) {
		relatedTriangles.push_back(t);
	}

	void getNormal(Vector3f& toFill)
	{
		if(isNormalised)
		{
			toFill[0] = _normalised_x;
			toFill[1] = _normalised_y;
			toFill[2] = _normalised_z;
		}
		else {
			if(count < 1) {
				std::cout<<"AVERAGE NORMAL ERROR"<<std::endl;
				return;
			}
			toFill[0] = _nx/(float)count;
			toFill[1] = _ny/(float)count;
			toFill[2] = _nz/(float)count;
		}
	}

	// Normalises the normal information for a vertice loaded from the obj file 
	void NormaliseNormal()
	{
		float len = sqrt(_nx*_nx + _ny*_ny + _nz*_nz);
		_normalised_x = _nx/len;
		_normalised_y = _ny/len;
		_normalised_z = _nz/len;
		isNormalised=true;
	}

	

	// Normalises a vector
	void Normalise()
	{

		float x, y, z = 0;
		x = _item[0];
		y = _item[1];
		z = _item[2];
		
		if(_item[0]==0 && _item[1]==0 && _item[2]==0) {std::cout<<"NULL VECTOR"<<std::endl;return;}
	
		double length = sqrt((x * x) + (y * y) + (z*z));
		if(length == 0) {std::cout<<"NORMALISATION ERROR  "<<x<<" "<<y<<" "<<z<<std::endl;return;}
		_item[0] = x/length;
		_item[1] = y/length;
		_item[2] = z/length;
	}

	Vector3f & operator = (Vector3f & obj) 
	{
		_item[0] = obj[0];
		_item[1] = obj[1];
		_item[2] = obj[2];
		_nx = obj._nx;
		_ny = obj._ny;
		_nz = obj._nz;
		count = obj.count;

		return *this;
	}

	Vector3f & operator += (Vector3f & obj) 
	{
		_item[0] += obj[0];
		_item[1] += obj[1];
		_item[2] += obj[2];

		return *this;
	}

	
};

//An output stream, useful for debugging.
//Allows you to stream Vector3f objects to the terminal.
ostream & operator << (ostream & stream, Vector3f & obj) 
{
	stream << obj[0] << ' ' << obj[1] << ' ' << obj[2] << ' ';
}




class Triangle {
friend class TriangleMesh;
friend class Vector3f;

	int _vertex[3];//indices to the 3 vertices of the triangle
public:
	Vector3f normal;
	Triangle(int v1, int v2, int v3) 
	{
		_vertex[0] = v1;  _vertex[1] = v2;  _vertex[2] = v3;  
	}
};

//Utilities
//fmax returns the maximum of 3 floats
float fmax(float f1,float f2, float f3) {
	float f = f1;

	if (f < f2) f = f2;
	if (f < f3) f = f3;

	return f;
}

//fmin returns the minimum of 3 floats.
float fmin(float f1,float f2, float f3) {
	float f = f1;

	if (f > f2) f = f2;
	if (f > f3) f = f3;

	return f;
}


class TriangleMesh 
{
	vector <Vector3f> _v; //_v stores the vertices as Vector3f
	vector <Triangle> _trig; //_trig stores the triangles as instances of the Triangle class
	
	float _xmax, _xmin, _ymax, _ymin, _zmin, _zmax;//These store the bounding values for each axis.

public: 
	TriangleMesh(char * filename) { loadFile(filename) ;};
	TriangleMesh() {};
	void loadFile(char * filename);

	int trigNum() { return _trig.size() ;};
	int vNum() { return _v.size();};
	Vector3f v(int i) { return _v[i];};

	//v1, v2 and v3 are return values for the vertices. i is the input index.
	void getTriangleVertices(int i, Vector3f & v1, Vector3f & v2, Vector3f & v3)
	{
		v1 = _v[_trig[i]._vertex[0]]; 
		v2 = _v[_trig[i]._vertex[1]]; 
		v3 = _v[_trig[i]._vertex[2]]; 
	};

	void preComputeNormals();
			
};

// Holds utility functions for working with vectors
class Operations
{
public:

		static float DotProduct(Vector3f& vec1, Vector3f& vec2)
		{
			return (vec1[0]*vec2[0] + vec1[1]*vec2[1] + vec1[2]*vec2[2] );
		};

		static double f(Vector3f& a, Vector3f& b, int x, int y)
		{
			return ((double)a[1] - (double)b[1])*(double)x + ((double)b[0] - (double)a[0])*(double)y + (double)a[0]*(double)b[1] - (double)b[0]*(double)a[1];
		};

		static void interpolateNormals(Vector3f& rtn, Vector3f& bary, Vector3f& a1, Vector3f& a2, Vector3f& a3)
		{
			
			rtn[0] = a1[0]*bary[0] + a2[0]*bary[1] + a3[0]*bary[2];
			rtn[1] = a1[1]*bary[0] + a2[1]*bary[1] + a3[1]*bary[2];
			rtn[2] = a1[2]*bary[0] + a2[2]*bary[1] + a3[2]*bary[2];
		};

		// Calculates barycentric coordinates
		static void Barycentric(Vector3f& bar, Vector3f& v0, Vector3f& v1, Vector3f& v2, int x, int y)
		{
			bar[0] = f(v1, v2, x, y)/f(v1, v2, v0[0], v0[1]);
			bar[1] = f(v2,v0,x,y)/f(v2,v0,v1[0], v1[1]);
			bar[2] = f(v0,v1,x,y)/f(v0,v1,v2[0], v2[1]);

			double alpha, beta, gamma = 0;
			double _x = (double)x;
			double _y = (double)y;
			double x0 = v0[0]; double y0 = v0[1];
			double x1 = v1[0]; double y1 = v1[1];
			double x2 = v2[0]; double y2 = v2[1];

			alpha = ((y1-y2)*_x + (x2-x1)*_y + x1*y2 - x2*y1) / ((y1-y2)*x0 + (x2-x1)*y0 + x1*y2 - x2*y1);
			beta = ((y2-y0)*_x + (x0-x2)*_y + x2*y0 - x0*y2) / ((y2-y0)*x1 + (x0-x2)*y1 + x2*y0 - x0*y2);
			//gamma = ((y0-y1)*_x + (x1-x0)*_y + x0*y1 - y1*x0) / ((y0-y1)*x2 + (x1-x0)*y2 + x0*y1 - y1*x0);
			gamma = 1 - alpha - beta;

			bar[0] = alpha;
			bar[1] = beta;
			bar[2] = gamma;

		};

		static float MinX(Vector3f& v1, Vector3f& v2, Vector3f& v3)
		{
			float min = v1[0];
			if(v2[0] < min)
			{
				min = v2[0];
			}
			if(v3[0] < min)
			{
				min = v3[0];
			}
			return (min);
		};

		static float MinY(Vector3f& v1, Vector3f& v2, Vector3f& v3)
		{
			float min = v1[1];
			if(v2[1] < min)
			{
				min = v2[1];
			}
			if(v3[1] < min)
			{
				min = v3[1];
			}
			return (min);
		};

		static float MaxX(Vector3f& v1, Vector3f& v2, Vector3f& v3)
		{
			float max = v1[0];
			if(v2[0] > max)
			{
				max = v2[0];
			}
			if(v3[0] > max)
			{
				max = v3[0];
			}
			return (max);
		};

		static float MaxY(Vector3f& v1, Vector3f& v2, Vector3f& v3)
		{
			float max = v1[1];
			if(v2[1] > max)
			{
				max = v2[1];
			}
			if(v3[1] > max)
			{
				max = v3[1];
			}
			return (max);
		};

		static void CrossProduct(Vector3f& res, Vector3f& a, Vector3f& b)
		{
			res[0] = a[1]*b[2] - a[2]*b[1];
			res[1] = a[2]*b[0] - a[0]*b[2];
			res[2] = a[0]*b[1] - a[1]*b[0];
		};

		// Calculates normal of a face
		static void FaceNormal(Vector3f& normal, Vector3f& v1, Vector3f& v2, Vector3f& v3)
		{
			Vector3f a(v2[0]-v1[0], v2[1]-v1[1], v2[2] - v1[2]);
			Vector3f b(v3[0]-v1[0], v3[1]-v1[1], v3[2] - v1[2]);
			CrossProduct(normal, a, b);
		};
};


// Simple class used to hold light information
class Light
{
public:

	Vector3f direction;
	Vector3f color;
	Light(float x, float y, float z, float r, float g, float b)
	{
		direction[0] = x;
		direction[1] = y;
		direction[2] = z;
		color[0] = r;
		color[1] = g;
		color[2] = b;
		
	};
	
	

};
std::vector<Light> lights;
class Lighting
{
		
	public:
		// Generates three lights, a red, green and blue 
		static void setLights()
		{
				Light a(0.5f, 0.5f, 0, 0,0,0.85f);
				Light b(-0.5f, -0.5f, -0.5f,0,0.6f,0);
				Light c(1,-1,0, 0.85f, 0,0);
				a.direction.Normalise();
				b.direction.Normalise();
				c.direction.Normalise();
				lights.push_back(a);
				lights.push_back(b);
				lights.push_back(c);
		};


		// Computes the color of the pixel using the phong illumination model
		static Vector3f phongIllumination(Vector3f& point, Vector3f& norm, float ka, float kd, float ks, Light& ambient, int n) 
		{


			// Apply Ambient lighting
			Vector3f color(ambient.color[0] * ka, ambient.color[1] * ka, ambient.color[2] * ka);
			

			Vector3f reflect(0,0,0);
			Vector3f view(0,0,1); // Direction to the screen/viewer

			
			for(int i = 0; i < lights.size(); i++)
			{
				Lighting::reflection(reflect, norm, lights[i].direction);
				reflect.Normalise();
				// Calculate specular highlights
				double spec = ks*pow((double)Operations::DotProduct(reflect, view),n);
				// calculate rgb for diffuse
				color[0] += lights[i].color[0]*(kd*Operations::DotProduct(norm, lights[i].direction) + spec );
				color[1] += lights[i].color[1]*(kd*Operations::DotProduct(norm, lights[i].direction) + spec);
				color[2] += lights[i].color[2]*(kd*Operations::DotProduct(norm, lights[i].direction) + spec);


			}
			

			return color;
			 
		};

		// Calculates the reflection vector
		static void reflection(Vector3f& reflect, Vector3f& normal, Vector3f& incident)
		{
			// r = d- 2(d.n)n
			float dot = Operations::DotProduct(normal, incident);
			reflect[0] = incident[0] - 2*dot*normal[0];
			reflect[1] = incident[1] - 2*dot*normal[1];
			reflect[2] = incident[2] - 2*dot*normal[2];
		};

};


// Holds transformations from previous coursework, used again for rotation of teapot
class Transformations
{
	public:
	

	static void RotX(Vector3f& vec, double angle)
	{
		double cosa = cos(angle);
		double sina = sin(angle);
		double xa, ya, za, wa = 0;
		xa = 1*vec[0] + 0*vec[1] + 0*vec[2] + 0*vec[3];
		ya = 0*vec[0] + cosa*vec[1] - sina*vec[2] + 0*vec[3];
		za = 0*vec[0] + sina*vec[1] + cosa*vec[2] + 0*vec[3];
		wa = 0*vec[0] + 0*vec[1] + 0*vec[2] + 1*vec[3];
		vec[3] = wa;
		vec[0] = xa;
		vec[1] = ya;
		vec[2] = za;
	}
	
	
	static void RotY(Vector3f& vec, double angle)
	{
		double cosa = cos(angle);
		double sina = sin(angle);
		double xa, ya, za, wa = 0;
		xa = cosa*vec[0] + 0*vec[1] + sina*vec[2] + 0*vec[3];
		ya = 0*vec[0] + 1*vec[1] + 0*vec[2] + 0*vec[3];
		za = -sina*vec[0] + 0*vec[1] + cosa*vec[2] + 0*vec[3];
		wa = 0*vec[0] + 0*vec[1] + 0*vec[2] + 1*vec[3];
		vec[3] = wa;
		vec[0] = xa;
		vec[1] = ya;
		vec[2] = za;
		
	}
	
	static void RotZ(Vector3f& vec, double angle)
	{
		double cosa = cos(angle);
		double sina = sin(angle);
		double xa, ya, za, wa = 0;
		xa = cosa*vec[0] - sina*vec[1] + 0*vec[2] + 0*vec[3];
		ya = sina*vec[0] + cosa*vec[1] + 0*vec[2] + 0*vec[3];
		za = 0*vec[0] + 0*vec[1] + 1*vec[2] + 0*vec[3];
		wa = 0*vec[0] + 0*vec[1] + 0*vec[2] + 1*vec[3];
		vec[3] = wa;
		vec[0] = xa;
		vec[1] = ya;
		vec[2] = za;
	}


	static void Rotate(Vector3f& vec, double rotx, double roty, double rotz)
	{
		RotX(vec, rotx);
		RotY(vec, roty);
		RotZ(vec, rotz);
	}

	
};

#endif //_rt_H
