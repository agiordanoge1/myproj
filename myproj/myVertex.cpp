#include "myVertex.h"
#include "myvector3d.h"
#include "myHalfedge.h"
#include "myFace.h"

myVertex::myVertex(void)
{
	point = NULL;
	originof = NULL;
	normal = new myVector3D(1.0,1.0,1.0);
}

myVertex::~myVertex(void)
{
	if (normal) delete normal;
}

void myVertex::computeNormal()
{
	/**** TODO ****/
	if (originof != NULL)
	{
		originof->adjacent_face->computeNormal();
		originof->twin->adjacent_face->computeNormal();
		originof->prev->adjacent_face->computeNormal();

		myVector3D* v1 = originof->adjacent_face->normal;
		myVector3D* v2 = originof->twin->adjacent_face->normal;
		myVector3D* v3 = originof->prev->adjacent_face->normal;

		double normal_x = (v1->dX + v2->dX + v3->dX) / 3;
		double normal_y = (v1->dY + v2->dY + v3->dY) / 3;
		double normal_z = (v1->dZ + v2->dY + v3->dZ) / 3;;

		double normalize = sqrt(pow(normal_x, 2) + pow(normal_y, 2) + pow(normal_z, 2));

		normal_x /= normalize;
		normal_y /= normalize;
		normal_z /= normalize;

		if (normal)
		{
			delete normal;
		}
		normal = new myVector3D(normal_x, normal_y, normal_z);
	}
	else return;
}
