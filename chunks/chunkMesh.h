#pragma once

#include "serialize.h"

class MeshGroup
{
public:
	MeshGroup();
	MeshGroup(const char* name, int nTris);
	virtual ~MeshGroup();

	void serialize(Serializer* pSerializer);
	
	void setName(const char* name);
	char *materialName;

	int numTriangles;
	int *vertexIndices;
	int *normalIndices;
	int *uvIndices;
};

class ChunkMesh
{
public:
	ChunkMesh();
	ChunkMesh(int nVertices, int nNormals, int nUVs, int nGroups);
	virtual ~ChunkMesh();

	void serialize(Serializer* pSerializer);

	//const int chunkType = CHUNKTYPE_MESH;

	int numVertices;
	float *vertices;

	int numNormals;
	float *normals;

	int numUVs;
	float *uvs;

	int numMeshGroups;
	MeshGroup **meshGroups;
};
