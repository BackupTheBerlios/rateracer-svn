#pragma once

#pragma warning( disable : 4100 )
#include <maya/MGlobal.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MFnDagNode.h>
#include <maya/MDagPathArray.h>
#include <maya/MObjectArray.h>
#pragma warning( default : 4100 )

class stlPlugin : public MPxFileTranslator
{
public:
	stlPlugin();
	virtual ~stlPlugin();
	
	static void	*creator() { return new stlPlugin(); }
	
	MStatus     reader(const MFileObject& fileObject,
										 const MString& optionsString,
										 MPxFileTranslator::FileAccessMode mode);
	
	MStatus     writer(const MFileObject& fileObject,
										 const MString& optionsString,
										 MPxFileTranslator::FileAccessMode mode);
	
	bool        canBeOpened()      const	{ return false; }
	bool        haveReadMethod()   const	{ return true; }
	bool        haveWriteMethod()  const	{ return true; }
	
	MString     defaultExtension() const  { return "stl"; }
	
	MFileKind   identifyFile(const MFileObject& fileObject,
													 const char *buffer,
													 short size) const;

private:
	void parseOptions(const MString& options);

	int  findShader(const MObject& shader);
	int  addShader(const MObject& shader);

	MObject findFirstPlugConnection(const MObject& object,
																	const char *plugname,
																	bool upstream = true);

	bool isNodeInSelection( const MDagPath& path, const MSelectionList& selList );
		
	void exportMeshes(		const char *fullpath, bool exportAll);
	void exportVol(		const char *fullpath, bool exportAll);

	void exportSceneData(	const char *fullpath, bool exportAll);

	void exportCameras(		FILE* outfile, bool exportAll);
	void exportLights(		FILE* outfile, bool exportAll);
	void exportMaterials(	FILE* outfile, bool exportAll);

  void exportPointlights( FILE* outfile );


	void dumpLambertParameters(FILE* outfile, const MObject& shader);
	void dumpReflectParameters(FILE* outfile, const MObject& shader);
	void dumpPhongParameters(  FILE* outfile, const MObject& shader);
	void dumpBlinnParameters(  FILE* outfile, const MObject& shader);

	// (Dummy) import methods (only for verification as yet...)
	void importMeshes(		const char *fullpath, bool doimport);
	void importSceneData(	const char *fullpath, bool doimport);

  void importMeshFile(const char *fullpath);

	// Helper functions
	static void printIndentation(int nSpaces);
	static MString getPathName(const MString &fullname);
	static MString getBaseName(const MString &filename, const char *extension);
  static MString getExtension(const MString &filename);
	static MString nodeName(const MObject& object );
	static MString nodeType(const MObject& object);
};
