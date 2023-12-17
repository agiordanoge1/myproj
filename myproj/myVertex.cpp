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
		myVector3D Somme_n(0.0, 0.0, 0.0);
		myHalfedge* first_he = this->originof;
		myHalfedge* next_he = first_he;
		int count=0;


		do
		{
			Somme_n += *next_he->adjacent_face->normal;
			count++;
			next_he = next_he->twin->next;

		} while (first_he != next_he);


		if (count == 0)return;

		Somme_n = Somme_n / static_cast<float>(count);
		Somme_n.normalize();
		*this->normal = Somme_n;
		//originof->adjacent_face->computeNormal();
		//originof->twin->adjacent_face->computeNormal();
		//originof->prev->twin->adjacent_face->computeNormal();

		//myVector3D* v1 = originof->adjacent_face->normal;
		//myVector3D* v2 = originof->twin->adjacent_face->normal;
		//myVector3D* v3 = originof->prev->twin->adjacent_face->normal;

		//double normal_x = (v1->dX + v2->dX + v3->dX) / 3;
		//double normal_y = (v1->dY + v2->dY + v3->dY) / 3;
		//double normal_z = (v1->dZ + v2->dZ + v3->dZ) / 3;

		//double normalize = sqrt(pow(normal_x, 2) + pow(normal_y, 2) + pow(normal_z, 2));
		//if (normalize != 0)
		//{
		//	normal_x /= normalize;
		//	normal_y /= normalize;
		//	normal_z /= normalize;
		//}

		//if (normal)
		//{
		//	delete normal;
		//}
		/*normal = new myVector3D(normal_x, normal_y, normal_z);*/
	}
	else return;
}
