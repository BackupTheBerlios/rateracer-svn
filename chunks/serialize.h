#pragma once

#include <stdio.h>

// Helper macros //////////////////////////////////////////////////////////

#define SERIALIZE_PREPARE_PTR(ptr, type, pSerializer) \
	if (pSerializer->getAction() == Serializer::SERIALIZE_READ) { \
		ptr = new type; \
	} \

#define SERIALIZE_PREPARE_ARRAY(ptr, type, size, pSerializer) \
	if (pSerializer->getAction() == Serializer::SERIALIZE_READ) { \
		ptr = ((size) > 0) ? new type[size] : NULL; \
	} \

#define SERIALIZE_ARRAY(ptr, type, size, pSerializer) \
{ \
	SERIALIZE_PREPARE_ARRAY(ptr, type, size, pSerializer) \
	pSerializer->fserialize(ptr, sizeof(type), size); \
}

///////////////////////////////////////////////////////////////////////////

class Serializer
{
public:
	typedef enum {
		SERIALIZE_WRITE, SERIALIZE_READ, SERIALIZE_DONOTHING
	} SerializeAction;

	Serializer();
	virtual ~Serializer();

	bool open(SerializeAction action, const char* filename);
	void close();

	SerializeAction getAction() { return mAction; }

	long setSizeBookmark();
	void updateSizeBookmark(long bookmark);

	void fskip(int numBytes);

	// Generic data
	size_t fserialize(void *p, size_t size, size_t count = 1);

	// Fundamental types
	size_t fserialize(bool   *p, size_t count = 1);
	size_t fserialize(char   *p, size_t count = 1);
	size_t fserialize(short  *p, size_t count = 1);
	size_t fserialize(int    *p, size_t count = 1);
	size_t fserialize(float  *p, size_t count = 1);
	size_t fserialize(double *p, size_t count = 1);

	// Arrays of fundamental types
	void serializeArray(bool   **p, int size);
	void serializeArray(char   **p, int size);
	void serializeArray(short  **p, int size);
	void serializeArray(int    **p, int size);
	void serializeArray(float  **p, int size);
	void serializeArray(double **p, int size);

	// Null-terminated string
	void serializeString(char **ps);

	bool mVerbosePrint;

private:
	SerializeAction mAction;
	FILE *mFile;
};

///////////////////////////////////////////////////////////////////////////
