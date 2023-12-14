#pragma once
#include "myFace.h"
#include "myHalfedge.h"
#include "myVertex.h"
#include <vector>
#include <string>

class myMesh
{
public:
	std::vector<myVertex *> vertices;
	std::vector<myHalfedge *> halfedges;
	std::vector<myFace *> faces;
	std::string name;

	void checkMesh();
	bool readFile(std::string filename);
	void computeNormals();
	void normalize();
	myPoint3D calcul_centre(myFace* face);

	void subdivisionCatmullClark();
	void simplifaction(float taille_edge);
	float calcul_longueur(myHalfedge *edge);
	myPoint3D* findmiddle(myHalfedge* edge);

	void splitFaceTRIS(myFace *, myPoint3D *);

	void splitEdge(myHalfedge *, myPoint3D *);
	void splitFaceQUADS(myFace *, myPoint3D *);

	void triangulate();
	bool triangulate(myFace *);
	void silhouette();

	bool checkoriginof(myHalfedge he);
	bool checksourceVertex(std::vector<myVertex*> vertices);


	bool checknul_he(std::vector<myHalfedge*> halfedges);
	bool checksourceinv_he(std::vector<myMesh*> mesh);
	bool checknextprev(std::vector<myHalfedge*> halfedges);
	bool checktwin(std::vector<myHalfedge*> halfedges);
	bool checkadjacentface();
	bool checkroundhe(int iteration_max);


	bool checkadjacenthalfedges();
	bool checknul();
	bool allhalfedge();


	void clear();

	myMesh(void);
	~myMesh(void);
};

