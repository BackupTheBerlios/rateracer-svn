#pragma once

#ifdef _DEBUG
#pragma comment(lib, "../chunks/debug/chunkslib.lib")
#else
#pragma comment(lib, "../chunks/release/chunkslib.lib")
#endif

#include "serialize.h"

enum ChunkTypes
{
	CHUNKTYPE_MESH = 1
};

class ChunkHeader
{
public:
	ChunkHeader();
	ChunkHeader(int type, const char* name, void *data);
	virtual ~ChunkHeader();

	void serialize(Serializer* pSerializer);

	int chunkType;

	void setName(const char* name);
	char *chunkName;

	//int chunkSize;
	void *chunkData;
};

class ChunksFile : Serializer
{
public:
	ChunksFile();
	ChunksFile(int nChunks);
	virtual ~ChunksFile();

	void serialize(Serializer *pSerializer);

	char formatstring[9]; // null-termination added...

	int numChunks;
	ChunkHeader **chunkHeaders;
};
