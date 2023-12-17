#include "myFace.h"
#include "myvector3d.h"
#include "myHalfedge.h"
#include "myVertex.h"
#include <GL/glew.h>

myFace::myFace(void)
{
	adjacent_halfedge = NULL;
	normal = new myVector3D(1.0, 1.0, 1.0);
}

myFace::~myFace(void)
{
	if (normal) delete normal;
}

void myFace::computeNormal()
{
	/**** TODO ****/
	if (adjacent_halfedge != NULL)
	{
		myVertex* first_v = adjacent_halfedge->source;
		myVertex* second_v = adjacent_halfedge->next->source;
		myVertex* third_v = adjacent_halfedge->prev->source;

		myVector3D first_vec = *(second_v->point) - *(first_v->point);
		myVector3D second_vec = *(third_v->point) - *(first_v->point);

		double normal_x = (first_vec.dY) * (second_vec.dZ) - (first_vec.dZ) * (second_vec.dY);
		double normal_y = (first_vec.dZ) * (second_vec.dX) - (first_vec.dX) * (second_vec.dZ);
		double normal_z = (first_vec.dX) * (second_vec.dY) - (first_vec.dY) * (second_vec.dX);

		double normalize = sqrt(pow(normal_x, 2) + pow(normal_y, 2) + pow(normal_z, 2));
		if (normalize != 0)
		{
			normal_x /= normalize;
			normal_y /= normalize;
			normal_z /= normalize;
		}

		if (normal)
		{
			delete normal;
		}
		this->normal= new myVector3D(normal_x, normal_y, normal_z);

		//myVector3D normal_r = first_vec.crossproduct(second_vec);
		//normal_r.normalize();

		//*(this->normal) = normal_r;

	}
	else return;
}
