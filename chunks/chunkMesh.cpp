#include <string.h>

#include "chunkMesh.h"

ChunkMesh::ChunkMesh()
{
	numVertices = 0;
	vertices = NULL;

	numNormals = 0;
	normals = NULL;

	numUVs = 0;
	uvs = NULL;

	numMeshGroups = 0;
	meshGroups = NULL;
}

ChunkMesh::ChunkMesh(int nVertices, int nNormals, int nUVs, int nGroups)
{
	vertices = NULL;
	normals = NULL;
	uvs = NULL;
	meshGroups = NULL;

	numVertices = nVertices;
	if (numVertices > 0) vertices = new float[3*numVertices];

	numNormals = nNormals;
	if (numNormals > 0) normals = new float[3*numNormals];

	numUVs = nUVs;
	if (numUVs > 0) uvs = new float[2*numUVs];

	numMeshGroups = nGroups;
	if (numMeshGroups > 0) meshGroups = new MeshGroup*[numMeshGroups];
}

ChunkMesh::~ChunkMesh()
{
	if (numVertices > 0) delete [] vertices;
	if (numNormals  > 0) delete [] normals;
	if (numUVs      > 0) delete [] uvs;

	if (numMeshGroups > 0) {
		for (int n = 0; n < numMeshGroups; n++) {
			delete meshGroups[n];
		}
		delete [] meshGroups;
	}
}

void ChunkMesh::serialize(Serializer* pSerializer)
{
	pSerializer->fserialize( &numVertices );
	pSerializer->serializeArray(&vertices, 3*numVertices);

	pSerializer->fserialize( &numNormals );
	pSerializer->serializeArray(&normals, 3*numNormals);

	pSerializer->fserialize( &numUVs );
	pSerializer->serializeArray(&uvs, 2*numUVs);

	pSerializer->fserialize( &numMeshGroups );
	SERIALIZE_PREPARE_ARRAY(meshGroups, MeshGroup*, numMeshGroups, pSerializer);
	for (int n = 0; n < numMeshGroups; n++)
	{
		SERIALIZE_PREPARE_PTR(meshGroups[n], MeshGroup, pSerializer);
		//if (meshGroups[n] == NULL) ...
		meshGroups[n]->serialize( pSerializer );
	}
}

///////////////////////////////////////////////////////////////////////////

MeshGroup::MeshGroup()
{
	materialName = NULL;

	numTriangles = 0;
	vertexIndices = NULL;
	normalIndices = NULL;
	uvIndices = NULL;
}

MeshGroup::MeshGroup(const char* name, int nTris)
{
	vertexIndices = NULL;
	normalIndices = NULL;
	uvIndices = NULL;

	setName(name);

	numTriangles = nTris;
	if (numTriangles > 0)
	{
		vertexIndices = new int[3*numTriangles];
		normalIndices = new int[3*numTriangles];
		uvIndices     = new int[3*numTriangles];
	}
}

MeshGroup::~MeshGroup()
{
	if (materialName != NULL) delete [] materialName;

	if (numTriangles > 0) {
		delete [] vertexIndices;
		delete [] normalIndices;
		delete [] uvIndices;
	}
}

void MeshGroup::serialize(Serializer* pSerializer)
{
	pSerializer->serializeString( &materialName );

	pSerializer->fserialize( &numTriangles );

	pSerializer->serializeArray(&vertexIndices, 3*numTriangles);
	pSerializer->serializeArray(&normalIndices, 3*numTriangles);
	pSerializer->serializeArray(&uvIndices,     3*numTriangles);
}

void MeshGroup::setName(const char* name)
{
	materialName = new char[ strlen(name) + 1 ];
	if (name != NULL) {
		strcpy(materialName, name);
	} else {
		materialName[0] = '\0';
	}
}
