#include <string.h>

#include "serialize.h"

Serializer::Serializer()
{
	mAction = SERIALIZE_DONOTHING;
	mFile = NULL;
	mVerbosePrint = false;
}

Serializer::~Serializer()
{
}

bool Serializer::open(SerializeAction action, const char* filename)
{
	mAction = action;

	if (mVerbosePrint) {
		printf("Serializer: Opening file '%s'.\n", filename);
	}

	if (mAction == SERIALIZE_WRITE)
	{
		mFile = fopen(filename, "wb");
	}
	else if (mAction == SERIALIZE_READ)
	{
		mFile = fopen(filename, "rb");
	}
	else if (mAction == SERIALIZE_DONOTHING)
	{
		return true;
	}

	if ( mFile == NULL )
	{
		printf("Could not open file '%s' for serialization!\n", filename);
		return false;
	}

	return true;
}

void Serializer::close()
{
	if (mVerbosePrint) {
		printf("Serializer: Closing file.\n");
	}
	if (mFile != NULL) {
		fclose( mFile );
		mFile = NULL;
	}
}

long Serializer::setSizeBookmark()
{
	long bookmark = ftell( mFile );
	if (mVerbosePrint) {
		printf("Setting size bookmark at %d\n", bookmark);
	}
	return bookmark;
}

void Serializer::updateSizeBookmark(long bookmark)
{
	if (mAction == SERIALIZE_WRITE)
	{
		long lastpos = ftell( mFile );

		int size = lastpos - (bookmark + sizeof(int));

		fseek( mFile, bookmark, SEEK_SET );
		fwrite( &size, sizeof(int), 1, mFile );

		if (mVerbosePrint) {
			printf("Updating size bookmark at %d, size = %d bytes (currpos = %d)\n",
				bookmark, size, lastpos);
		}

		fseek( mFile, lastpos, SEEK_SET );
	}
}

void Serializer::fskip(int numBytes)
{
	if (mAction == SERIALIZE_READ) {
		long lastpos = ftell( mFile );
		fseek( mFile, numBytes, SEEK_CUR );
		if (mVerbosePrint) {
			printf("Skipping data at %d, size = %d bytes (newpos = %d)\n",
				lastpos, numBytes, ftell( mFile ));
		}
	}
}

size_t Serializer::fserialize(void *p, size_t size, size_t count)
{
	size_t retsize = 0;
	if (count > 0)
	{
		if (mAction == SERIALIZE_WRITE)
		{
			retsize = fwrite(p, size, count, mFile);
		}
		else if (mAction == SERIALIZE_READ)
		{
			retsize = fread(p, size, count, mFile);
		}
		if (mVerbosePrint) {
			printf("fserialize: %d x (%d bytes)\n", count, size);
		}
		if (retsize < count)
		{
			fprintf( stderr, "\n fserialize ERROR: " );
			if (feof(mFile))
				fprintf( stderr, "Premature end of file!" );
			else
				fprintf( stderr, strerror(NULL) );
			fprintf( stderr, "\n" );
			throw("Serialization failed!");
		}
	}
	return retsize;
}

void Serializer::serializeString(char **ps)
{
	int len;
	if (mAction == SERIALIZE_WRITE)
	{
		len = (int)strlen(*ps) + 1;
	}
	fserialize(&len);
	
	if (mAction == SERIALIZE_READ)
	{
		*ps = new char[len];
	}
	fserialize(*ps, sizeof(char), len);
}

size_t Serializer::fserialize(bool *p, size_t count)
{
	return fserialize(p, sizeof(*p), count);
}

size_t Serializer::fserialize(char *p, size_t count)
{
	return fserialize(p, sizeof(*p), count);
}

size_t Serializer::fserialize(short *p, size_t count)
{
	return fserialize(p, sizeof(*p), count);
}

size_t Serializer::fserialize(int *p, size_t count)
{
	return fserialize(p, sizeof(*p), count);
}

size_t Serializer::fserialize(float *p, size_t count)
{
	return fserialize(p, sizeof(*p), count);
}

size_t Serializer::fserialize(double *p, size_t count)
{
	return fserialize(p, sizeof(*p), count);
}

void Serializer::serializeArray(bool **p, int size)
{
	if (mAction == Serializer::SERIALIZE_READ) {
		*p = ((size) > 0) ? new bool[size] : NULL;
	}
	fserialize(*p, size);
}

void Serializer::serializeArray(char **p, int size)
{
	if (mAction == Serializer::SERIALIZE_READ) {
		*p = ((size) > 0) ? new char[size] : NULL;
	}
	fserialize(*p, size);
}

void Serializer::serializeArray(short **p, int size)
{
	if (mAction == Serializer::SERIALIZE_READ) {
		*p = ((size) > 0) ? new short[size] : NULL;
	}
	fserialize(*p, size);
}

void Serializer::serializeArray(int **p, int size)
{
	if (mAction == Serializer::SERIALIZE_READ) {
		*p = ((size) > 0) ? new int[size] : NULL;
	}
	fserialize(*p, size);
}

void Serializer::serializeArray(float **p, int size)
{
	if (mAction == Serializer::SERIALIZE_READ) {
		*p = ((size) > 0) ? new float[size] : NULL;
	}
	fserialize(*p, size);
}

void Serializer::serializeArray(double **p, int size)
{
	if (mAction == Serializer::SERIALIZE_READ) {
		*p = ((size) > 0) ? new double[size] : NULL;
	}
	fserialize(*p, size);
}
