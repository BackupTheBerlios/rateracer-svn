#include <string.h>

#include "chunksfile.h"

#include "chunkMesh.h"

static const char *FORMAT_IDENTIFIER = "!CHUNKS!";

ChunksFile::ChunksFile()
{
	strncpy(formatstring, FORMAT_IDENTIFIER, 9);

	numChunks = 0;
	chunkHeaders = NULL;
}

ChunksFile::ChunksFile(int nChunks)
{
	strncpy(formatstring, FORMAT_IDENTIFIER, 9);

	numChunks = nChunks;
	chunkHeaders = new ChunkHeader*[numChunks];
}

ChunksFile::~ChunksFile()
{
	if (numChunks > 0) {
		for (int n = 0; n < numChunks; n++) {
			delete chunkHeaders[n];
		}
		delete [] chunkHeaders;
	}
}

void ChunksFile::serialize(Serializer *pSerializer)
{
	pSerializer->fserialize( &formatstring, sizeof(char), 8 );

	if (pSerializer->getAction() == Serializer::SERIALIZE_READ) {
		formatstring[8] = '\0';
		//FIXME check format string!
	}

	pSerializer->fserialize( &numChunks );
	SERIALIZE_PREPARE_ARRAY(chunkHeaders, ChunkHeader*, numChunks, pSerializer);
	for (int n = 0; n < numChunks; n++)
	{
		SERIALIZE_PREPARE_PTR(chunkHeaders[n], ChunkHeader, pSerializer);
		//if (chunkHeaders[n] == NULL) ...
		chunkHeaders[n]->serialize( pSerializer );
	}
}

ChunkHeader::ChunkHeader()
{
	chunkType = 0;
	chunkName = NULL;
	chunkData = NULL;
}

ChunkHeader::ChunkHeader(int type, const char* name, void *data)
{
	chunkType = type;
	setName( name );
	chunkData = data;
}

ChunkHeader::~ChunkHeader()
{
	if (chunkName != NULL) delete [] chunkName;

	if (chunkData != NULL)
	{
		switch (chunkType)
		{
		case CHUNKTYPE_MESH :
			delete (ChunkMesh*)chunkData;
			break;

		default :
			printf("Error: ChunkHeader can't delete unknown data...\n");
			//delete chunkData;
			break;
		}
	}
}

void ChunkHeader::serialize(Serializer* pSerializer)
{
	pSerializer->fserialize( &chunkType );

	pSerializer->serializeString( &chunkName );

	int chunkSize = 0;
	long bookmark = pSerializer->setSizeBookmark();
	pSerializer->fserialize( &chunkSize );

	switch (chunkType)
	{
	case CHUNKTYPE_MESH :
		SERIALIZE_PREPARE_PTR(chunkData, ChunkMesh, pSerializer);
		((ChunkMesh*)chunkData)->serialize(pSerializer);
		break;

	default :
		pSerializer->fskip( chunkSize );
		break;
	}

	pSerializer->updateSizeBookmark( bookmark );
}

void ChunkHeader::setName(const char* name)
{
	chunkName = new char[ strlen(name) + 1 ];
	if (name != NULL) {
		strcpy(chunkName, name);
	} else {
		chunkName[0] = '\0';
	}
}
