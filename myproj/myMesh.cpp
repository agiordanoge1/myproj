#include "myMesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
#include <GL/glew.h>
#include <tuple>
#include <list>
#include <cmath>
#include "myvector3d.h"

using namespace std;

myMesh::myMesh(void)
{
	/**** TODO ****/
}


myMesh::~myMesh(void)
{
	/**** TODO ****/
}

void myMesh::clear()
{
	for (unsigned int i = 0; i < vertices.size(); i++) if (vertices[i]) delete vertices[i];
	for (unsigned int i = 0; i < halfedges.size(); i++) if (halfedges[i]) delete halfedges[i];
	for (unsigned int i = 0; i < faces.size(); i++) if (faces[i]) delete faces[i];

	vector<myVertex *> empty_vertices;    vertices.swap(empty_vertices);
	vector<myHalfedge *> empty_halfedges; halfedges.swap(empty_halfedges);
	vector<myFace *> empty_faces;         faces.swap(empty_faces);
}

void myMesh::checkMesh()
{
	vector<myHalfedge *>::iterator it;
	for (it = halfedges.begin(); it != halfedges.end(); it++)
	{
		if ((*it)->twin == NULL)
			break;
	}
	if (it != halfedges.end())
		cout << "Error! Not all edges have their twins!\n";
	else cout << "Each edge has a twin!\n";
}


bool myMesh::readFile(std::string filename)
{
	string s, t, u;

	ifstream fin(filename);
	if (!fin.is_open()) {
		cout << "Unable to open file!\n";
		return false;
	}
	name = filename;

	map<pair<int, int>, myHalfedge*> twin_map;
	map<pair<int, int>, myHalfedge*>::iterator it;

	// Temporary vector to store faces vertices ids.
	vector<vector<int>> obj_faces;

	while (getline(fin, s))
	{
		stringstream myline(s);
		myline >> t;
		if (t == "g") {}
		else if (t == "v")
		{
			float x, y, z;
			myline >> x >> y >> z;
			cout << "v " << x << " " << y << " " << z << endl;

			// Registering the vertex.
			auto* v = new myVertex();
			v->point = new myPoint3D(x, y, z);
			vertices.push_back(v);
		}
		else if (t == "mtllib") {}
		else if (t == "usemtl") {}
		else if (t == "s") {}
		else if (t == "f")
		{
			cout << "f";

			// Reading the current face's vertices.
			vector<int> face_ids;
			while (myline >> u)
			{
				int vertex = atoi((u.substr(0, u.find("/"))).c_str());
				cout << " " << vertex;

				face_ids.push_back(vertex);
			}
			// Storing the new face as a series of vertices ids.
			obj_faces.push_back(face_ids);

			cout << endl;
		}
	}

	// Rebuilding each face from its vertices.
	for (const vector<int>& face_ids : obj_faces)
	{
		auto* new_face = new myFace();

		// Temporary vector for the face's half-edges
		vector<myHalfedge*> face_hedges;

		// Registering the new face.
		faces.push_back(new_face);

		// Building the face's half-edges.
		int n = static_cast<int>(face_ids.size());
		for (int i = 0; i < n; i++)
		{
			// Building an edge from vertices indexes.
			int idx1 = face_ids[i];
			int idx2 = i < n ? face_ids[(i-1 + face_ids.size())% face_ids.size()] : face_ids[0];

			// Creating the new half-edge.
			auto* new_h_edge = new myHalfedge();
			new_h_edge->adjacent_face = new_face;

			// Retrieving the corresponding vertex and linking the half-edge with it.
			vertices.at(idx1 - 1)->originof = new_h_edge;
			new_h_edge->source = vertices.at(idx1 - 1);

			// Try to retrieve the current half-edge's twin.
			it = twin_map.find(make_pair(idx2, idx1));
			if (it == twin_map.end())
			{
				// This means there was no myHalfedge* present at location(a, b).
				// Then we create an entry in the twin mapping.
				twin_map.insert(pair<pair<int, int>, myHalfedge*>(make_pair(idx1, idx2), new_h_edge));
			}
			else
			{
				// It was found. The variable it->second is of type myHalfedge*,
				// and is the half-edge present at location(a, b).
				new_h_edge->twin = it->second;
				it->second->twin = new_h_edge;
			}

			// Registering the half-edge.
			halfedges.push_back(new_h_edge);
			face_hedges.push_back(new_h_edge);
		}

		// Linking the half-edges together.
		int he_count = static_cast<int>(face_hedges.size());
		for (int i = 0; i < he_count; ++i) {
			face_hedges[i]->next = face_hedges[(i + 1) % he_count];
			face_hedges[i]->prev = face_hedges[(i + he_count - 1) % he_count];
		}

		// Link the new face to a corresponding half-edge
		new_face->adjacent_halfedge = halfedges.back();
		
	}

	// Clear the temporary vector
	obj_faces.clear();
	

	checkMesh();
	normalize();

	return true;
}


void myMesh::computeNormals()
{
	/**** TODO ****/
	for (auto vertex : vertices)
	{
		vertex->computeNormal();
	}
	for (auto face : faces)
	{
		face->computeNormal();
	}
}

void myMesh::normalize()
{
	if (vertices.size() < 1) return;

	int tmpxmin = 0, tmpymin = 0, tmpzmin = 0, tmpxmax = 0, tmpymax = 0, tmpzmax = 0;

	for (unsigned int i = 0; i < vertices.size(); i++) {
		if (vertices[i]->point->X < vertices[tmpxmin]->point->X) tmpxmin = i;
		if (vertices[i]->point->X > vertices[tmpxmax]->point->X) tmpxmax = i;

		if (vertices[i]->point->Y < vertices[tmpymin]->point->Y) tmpymin = i;
		if (vertices[i]->point->Y > vertices[tmpymax]->point->Y) tmpymax = i;

		if (vertices[i]->point->Z < vertices[tmpzmin]->point->Z) tmpzmin = i;
		if (vertices[i]->point->Z > vertices[tmpzmax]->point->Z) tmpzmax = i;
	}

	double xmin = vertices[tmpxmin]->point->X, xmax = vertices[tmpxmax]->point->X,
		ymin = vertices[tmpymin]->point->Y, ymax = vertices[tmpymax]->point->Y,
		zmin = vertices[tmpzmin]->point->Z, zmax = vertices[tmpzmax]->point->Z;

	double scale = (xmax - xmin) > (ymax - ymin) ? (xmax - xmin) : (ymax - ymin);
	scale = scale > (zmax - zmin) ? scale : (zmax - zmin);

	for (unsigned int i = 0; i < vertices.size(); i++) {
		vertices[i]->point->X -= (xmax + xmin) / 2;
		vertices[i]->point->Y -= (ymax + ymin) / 2;
		vertices[i]->point->Z -= (zmax + zmin) / 2;

		vertices[i]->point->X /= scale;
		vertices[i]->point->Y /= scale;
		vertices[i]->point->Z /= scale;
	}
}


void myMesh::splitFaceTRIS(myFace *f, myPoint3D *p)
{
	/**** TODO ****/
}

void myMesh::splitEdge(myHalfedge *e1, myPoint3D *p)
{

	/**** TODO ****/
}

void myMesh::splitFaceQUADS(myFace *f, myPoint3D *p)
{
	/**** TODO ****/
}

float myMesh::calcul_longueur(myHalfedge *edge)
{
	myPoint3D* start = new myPoint3D();
	myPoint3D* end = new myPoint3D();
	start = edge->source->point;
	end = edge->next->source->point;
	float longueur = 0;

	longueur = sqrtf(std::pow(start->X - end->X,2) + std::pow(start->Y - end->Y,2) + std::pow(start->Z - end->Z,2));
	return longueur;
}

myPoint3D* myMesh::findmiddle(myHalfedge* edge)
{
	myPoint3D* middle = new myPoint3D();
	myPoint3D* start = new myPoint3D();
	myPoint3D* end = new myPoint3D();
	start = edge->source->point;
	end = edge->next->source->point;

	middle->X = (start->X + end->X) / 2;
	middle->Y = (start->Y + end->Y) / 2;
	middle->Z = (start->Z + end->Z) / 2;
	return middle;

}

void myMesh::simplifaction(float taille_edge)
{
	vector<myHalfedge*> to_erase_edges;
	float longueur=0;
	myPoint3D* middle = new myPoint3D();
	cout << "passage"<<endl;
	for (auto* edge : halfedges)
	{
		longueur = calcul_longueur(edge);
		cout << "longueur : " << longueur << endl;
		if (longueur < taille_edge)
		{
			cout << "test"<<endl;
			if (!triangulate(edge->adjacent_face))
				continue;
			cout << "triangulate" << endl;
			middle=findmiddle(edge);
			edge->next->source->point = middle;




			//edge->prev->twin->source->point = middle;

			edge->next->prev = edge->prev;
			edge->prev->next = edge->next;
			edge->prev->twin->prev = edge->next->twin;
			edge->next->twin->next = edge->prev->twin;
			to_erase_edges.push_back(edge);
		}
	}

	for (myHalfedge* edge : to_erase_edges)
	{
		//halfedges.erase(remove(halfedges.begin(), halfedges.end(), edge), halfedges.end());
	}
		
}


void myMesh::subdivisionCatmullClark()
{
	/**** TODO ****/

}

myPoint3D calcul_centre(myFace face)
{
	myPoint3D centre;
	myPoint3D* firstpoint;
	myHalfedge* nexthe;
	int count_of_point = 0;
	double somme_x = 0;
	double somme_y = 0;
	double somme_z = 0;

	firstpoint = face.adjacent_halfedge->source->point;
	nexthe = face.adjacent_halfedge->next;
	somme_x += face.adjacent_halfedge->source->point->X;
	somme_y += face.adjacent_halfedge->source->point->Y;
	somme_z += face.adjacent_halfedge->source->point->Z;
	count_of_point++;
	while (nexthe->source->point != firstpoint)
	{

		somme_x += nexthe->source->point->X;
		somme_y += nexthe->source->point->Y;
		somme_z += nexthe->source->point->Z;
		count_of_point++;
		nexthe = nexthe->next;
		cout << "count of point" << count_of_point << endl;
		cout << " X: " << nexthe->source->point->X << " Y: " << nexthe->source->point->Y << " Z: " << nexthe->source->point->Z << endl;
	}
	centre.X = somme_x / count_of_point;
	centre.Y = somme_y / count_of_point;
	centre.Z = somme_z / count_of_point;

	cout << "centre :  X :" << centre.X << "  Y :" << centre.Y << "  Z :" << centre.Z << endl;
	return centre;
}


void myMesh::triangulate()
{
	/**** TODO ****/
	vector<myFace*> newfaces;
	vector<myFace*> to_erase_faces;
	vector<myHalfedge*> to_erase_edges;

	for (auto* face : faces)
	{
		if (triangulate(face))
		{
			myPoint3D* centre = new myPoint3D();
			myPoint3D* firstpoint = new myPoint3D();
			myHalfedge* nexthe = new myHalfedge();
			myPoint3D* somme = new myPoint3D();
			myHalfedge* tmp = new myHalfedge();
			double count_of_point = 0;
			double somme_x = 0;
			double somme_y = 0;
			double somme_z = 0;


			firstpoint = face->adjacent_halfedge->source->point;
			tmp = face->adjacent_halfedge;
			nexthe = face->adjacent_halfedge->next;
			somme_x += face->adjacent_halfedge->source->point->X;
			somme_y += face->adjacent_halfedge->source->point->Y;
			somme_z += face->adjacent_halfedge->source->point->Z;
			count_of_point++;
			while (nexthe->source->point != firstpoint)
			{
				somme_x += nexthe->source->point->X;
				somme_y += nexthe->source->point->Y;
				somme_z += nexthe->source->point->Z;
				count_of_point++;
				nexthe = nexthe->next;
				//cout << "count of point" << count_of_point << endl;
				//cout << " X: " << nexthe->source->point->X << " Y: " << nexthe->source->point->Y << " Z: " << nexthe->source->point->Z << endl;
			}
			centre->X = somme_x / count_of_point;
			centre->Y = somme_y / count_of_point;
			centre->Z = somme_z / count_of_point;

			//cout << "centre :  X :" << centre->X << "  Y :" << centre->Y << "  Z :" << centre->Z << endl;
			auto* centre_v = new myVertex();

			centre_v->originof = nexthe;
			centre_v->point = centre;
			vertices.push_back(centre_v);


			
			do {
				auto* new_face = new myFace();

				auto* firsthe = new myHalfedge();
				auto* secondhe = new myHalfedge();
				auto* thirdhe = new myHalfedge();

				firsthe->source = tmp->source;
				secondhe->source = centre_v;
				thirdhe->source = tmp->next->source;

				firsthe->adjacent_face = new_face;
				secondhe->adjacent_face = new_face;
				thirdhe->adjacent_face = new_face;

				firsthe->prev = thirdhe;
				secondhe->prev = firsthe;
				thirdhe->prev = secondhe;

				secondhe->next = thirdhe;
				firsthe->next = secondhe;
				thirdhe->next = firsthe;

				//secondhe->next->twin = thirdhe;
				//firsthe->next = secondhe;
				//thirdhe->next = firsthe;

				new_face->adjacent_halfedge = secondhe;

				halfedges.push_back(firsthe);
				halfedges.push_back(secondhe);
				halfedges.push_back(thirdhe);

				newfaces.push_back(new_face);
				tmp = tmp->next;
			} while (tmp != face->adjacent_halfedge);
			to_erase_faces.push_back(face);
			to_erase_edges.push_back(face->adjacent_halfedge);
			to_erase_edges.push_back(face->adjacent_halfedge->twin);
			to_erase_edges.push_back(face->adjacent_halfedge->prev);
			to_erase_edges.push_back(face->adjacent_halfedge->next);
		}
	}

	checktwin(halfedges);

	for (myHalfedge* edge : to_erase_edges)
		halfedges.erase(remove(halfedges.begin(), halfedges.end(), edge), halfedges.end());

	for (myFace* face : to_erase_faces)
		faces.erase(remove(faces.begin(), faces.end(), face), faces.end());

	for (auto* face : newfaces)
		faces.push_back(face);
}

//return false if already triangle, true othewise.
bool myMesh::triangulate(myFace *f)
{
	/**** TODO ****/
	return f->adjacent_halfedge != f->adjacent_halfedge->next->next->next;
}


bool myMesh::checkoriginof(myHalfedge he)
{
	if (he.source == nullptr) throw runtime_error("origin of error");
	return true;
}

bool myMesh::checksourceVertex(vector<myVertex*> vertices)
{
	for (myVertex* vertice : vertices)
	{
		if (vertice->originof->source == nullptr) throw runtime_error("source vertex error");
	}
	return true;
}


bool myMesh::checknul_he(vector<myHalfedge*> halfedges)
{
	for (myHalfedge* halfedge : halfedges)
	{
		if (halfedge->source->originof->source == nullptr) throw runtime_error("check nul he error");
	}
	return true;
}

bool myMesh::checksourceinv_he(vector<myMesh*> mesh)
{

	return true;
}

bool myMesh::checknextprev(vector<myHalfedge*> halfedges)
{
	return true;
}

bool myMesh::checktwin(vector<myHalfedge*> halfedges)
{
	for (myHalfedge* halfedge : halfedges)
	{
		//if (halfedge->prev->twin != halfedge->twin->next) throw runtime_error("check twin next he error");
		//if (halfedge->next->twin != halfedge->twin->prev) throw runtime_error("check twin prev he error");
	}
	return true;
}

bool myMesh::checkadjacentface()
{
	return true;
}

bool myMesh::checkroundhe(int iteration_max)
{
	return true;
}


bool myMesh::checkadjacenthalfedges()
{
	return true;
}

bool myMesh::checknul()
{
	return true;
}

bool myMesh::allhalfedge()
{
	return true;
}

