/*
 * Iostream is pure evil
 */
#define REQUIRE_IOSTREAM

#include <vector>

#define NT_ENV
#define NT_PLUGIN
#include <maya/MComputation.h>
#include <maya/MSelectionList.h>
#include <maya/MAnimControl.h>
#include <maya/MDagPath.h>
#include <maya/MItDag.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFnMesh.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnComponent.h>
#include <maya/MFnCharacter.h>
#include <maya/MFnClip.h>
#include <maya/MFnMotionPath.h>
#include <maya/MFnCamera.h>
#include <maya/MFnGravityField.h>
#include <maya/MFnAttribute.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnTransform.h>
#include <maya/MMatrix.h>
#include <maya/MQuaternion.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MColor.h>
#include <maya/MFnLight.h>
#include <maya/MFnPointLight.h>

#include <string>

//#include "mathlib/gm/gm.h"
#include "chunks/chunksfile.h"
#include "chunks/chunkMesh.h"

#include "chunksExport.h"
#include "mayaPlugin.h"

#pragma warning(disable : 4702 4239)

chunksExport::chunksExport()
{
}

chunksExport::~chunksExport()
{
}

MPxFileTranslator::MFileKind chunksExport::identifyFile(const MFileObject& /*fileName*/,
																											const char *buffer,
																											short /*size*/) const
{
	MFileKind rval = kNotMyFileType;
	MString magic("!CHUNKS!");
	if (0 == strncmp(buffer, magic.asChar(), magic.length()))
	{
		rval = kIsMyFileType;
	}
	return rval;
}

MStatus chunksExport::reader(	const MFileObject& fileObject,
														const MString& /*options*/,
														MPxFileTranslator::FileAccessMode mode )
{
	// Reader implementation is only used for verification, as yet...

	char fullpath[1024];
	strcpy( fullpath, fileObject.fullName().asChar() );

	importMeshes(		fullpath, (mode == MPxFileTranslator::kImportAccessMode));
	importSceneData(fullpath, (mode == MPxFileTranslator::kImportAccessMode));

	_flushall(); // Make sure output window is updated;
	BringMayaOutputWindowToFront();
	
	return MS::kSuccess;
}

MStatus chunksExport::writer( const MFileObject &fileObject,
														const MString& /*options*/,
														MPxFileTranslator::FileAccessMode mode )
{
	//parseOptions(options);

	MTime origTime = MAnimControl::currentTime();
	
	// Lets check the TimeSlider control now
	//MTime start( MAnimControl::minTime().value(), MTime::uiUnit() );
	//MTime end(   MAnimControl::maxTime().value(), MTime::uiUnit() );
	
	/*
	// Now setup some paths to do basic texture file searching
	// for textures with relative paths
	MStringArray wSpacePaths;
	MGlobal::executeCommand( "workspace -q -rd", wSpacePaths ); // rootdir
	*/

	MString pathname = getPathName(fileObject.fullName());
	MString basename = getBaseName(fileObject.name(), ".chunks");
	
	char fullpath[1024];
	strcpy( fullpath, pathname.asChar() );
	strcat( fullpath, basename.asChar() );
	strcat( fullpath, ".chunks" );
	
	exportMeshes(		fullpath, (mode == MPxFileTranslator::kExportActiveAccessMode));
	exportSceneData(fullpath, (mode == MPxFileTranslator::kExportActiveAccessMode));

	MAnimControl::setCurrentTime(origTime);
	
	_flushall(); // Make sure output window is updated;
	BringMayaOutputWindowToFront();
	
	return MS::kSuccess;
}

void chunksExport::parseOptions(const MString& options)
{
	if (options.length() > 0)
	{
		MStringArray optionList;
		MStringArray theOption;
		
		options.split(';', optionList);
		
		//break out all the options.
		for (unsigned int i = 0; i < optionList.length(); ++i)
		{
			optionList[i].split('=', theOption);
			if (theOption.length() > 1)
			{
				//if (theOption[0] == MString("useTransformTree"))
				//	mUseTransformTree = theOption[1].asInt();
				theOption.clear();
			}
		}
	}
}

bool chunksExport::isNodeInSelection(const MDagPath & path,
																	   const MSelectionList& selList)
{
	MDagPath searchPath( path );
	while (searchPath.length() > 0)
	{
		if ( selList.hasItem(searchPath) ) return true;
		searchPath.pop();
	}   
	return false;
}

MObject chunksExport::findFirstPlugConnection(const MObject& object,
																							const char *plugname,
																							bool upstream)
{
	MPlugArray srcPlugArray;
	MFnDependencyNode depNode(object);
	
	MPlug plug = depNode.findPlug(plugname);
	plug.connectedTo( srcPlugArray, upstream, !upstream );
	
	MObject obj = MObject::kNullObj;
	if ( srcPlugArray.length() > 0 )
		obj = srcPlugArray[0].node();
	return obj;
}

void chunksExport::exportMeshes(const char *fullpath, bool exportSelectedOnly)
{
	Serializer serializer;

	//serializer.mVerbosePrint = true;

	printf("-------------------------------------------------------------------------------\n");

	if ( !serializer.open(Serializer::SERIALIZE_WRITE, fullpath) )
	{
		printf("chunksExport::exportMeshes() : Could not open output file '%s'\n", fullpath);
		return;
	}

	printf("Exporting meshes to file '%s'\n", fullpath);

	std::vector< ChunkHeader* > chunksVector;

	MDagPath currentPath;

	MSelectionList selectionList;
	MGlobal::getActiveSelectionList(selectionList);

	int totalNumTriangles = 0;

	MItDag dagIterator(MItDag::kDepthFirst, MFn::kMesh);
	//dagIterator.count()

	for( ; !dagIterator.isDone(); dagIterator.next() )
	{
		dagIterator.getPath( currentPath ); 
		//currentPath.extendToShape();
		
		if (exportSelectedOnly && !isNodeInSelection(currentPath, selectionList))
			continue;

		int nontriangulated = 0;
		//int n;

		MStatus stat;
		MFnMesh fnMesh( currentPath, &stat );
		if( stat != MS::kSuccess )
		{
			printf("*** Error - failed to get mesh function set in buildMesh().\n");
			return;
		}

		MFnDagNode fnDagNode(currentPath);

		//FIXME clone & triangulate!
		//FIXME export mesh transform (fnMesh.transformationMatrix() ?)

		/////////////////////////////////////////////////////////////////////////////

		MFloatPointArray  pointArray;
		//MColorArray       vcolorArray;
		MFloatVectorArray normalArray;
		MFloatArray       texUArray;
		MFloatArray       texVArray;
		MObjectArray			shaderSets;
		MObjectArray			faceGroups;

		fnMesh.getConnectedSetsAndMembers(
			currentPath.instanceNumber(),
			shaderSets, faceGroups, true);
		
		int numGroups = shaderSets.length();
		
		// Skip default shader connection if multiple shaders exist
		if (numGroups > 1)
		{
			for (unsigned int i = 0; i < shaderSets.length(); i++)
			{
				// Default shader has empty component list
				if (MFnComponent(faceGroups[i]).elementCount() == 0) {
					numGroups--;
					printf("\nDefault shader skipped!\n");
				}
			}
		}

		if (numGroups == 0) {
			printf("\nEmpty mesh skipped!\n");
			continue;
		}

		printf("\nMesh object: %s\n", fnMesh.name().asChar());
		printf("%d faces in %d groups\n", fnMesh.numPolygons(), numGroups);
		printf("Vertices: %d\n", fnMesh.numVertices());
		//printf("Colors:   %d\n", fnMesh.num());
		printf("Normals:  %d\n", fnMesh.numNormals());
		printf("UVs:      %d\n", fnMesh.numUVs());

		MSpace::Space coordSpace = MSpace::kWorld;

		fnMesh.getPoints( pointArray, coordSpace );
		//fnMesh.getVertexColors( vcolorArray );
		fnMesh.getNormals( normalArray, coordSpace );
		fnMesh.getUVs( texUArray, texVArray );

		int n, g, uvIdx;

		int numVertices = fnMesh.numVertices();
		int numNormals = fnMesh.numNormals();
		int numUVs = fnMesh.numUVs();

		ChunkMesh *newMesh = new ChunkMesh(
			fnMesh.numVertices(),
			fnMesh.numNormals(),
			fnMesh.numUVs(),
			numGroups );

		for (n = 0; n < newMesh->numVertices; n++) {
			newMesh->vertices[3*n+0] = pointArray[n].x;
			newMesh->vertices[3*n+1] = pointArray[n].y;
			newMesh->vertices[3*n+2] = pointArray[n].z;
		}

		// TODO: check for normals of near zero length!
		for (n = 0; n < newMesh->numNormals; n++) {
			newMesh->normals[3*n+0] = normalArray[n].x;
			newMesh->normals[3*n+1] = normalArray[n].y;
			newMesh->normals[3*n+2] = normalArray[n].z;
		}

		for (n = 0; n < newMesh->numUVs; n++) {
			newMesh->uvs[2*n+0] = texUArray[n];
			newMesh->uvs[2*n+1] = texVArray[n];
		}

		std::vector< MeshGroup* > meshgroupsVector;

		for (g = 0; g < numGroups; g++)
		{
			// Skip default shader connection if multiple shaders exist
			if (shaderSets.length() > 1 &&
					MFnComponent(faceGroups[g]).elementCount() == 0)
			{
				printf("\nDefault shader skipped!\n");
				continue;
			}

			MObject shader = findFirstPlugConnection(shaderSets[g], "surfaceShader");

			MItMeshPolygon faceIter( currentPath, faceGroups[g] );

			MeshGroup* newGroup = new MeshGroup(
				nodeName(shader).asChar(),
				faceIter.count() );

			int index = 0;
			for( ; !faceIter.isDone(); faceIter.next() )
			{
				int idxCount = faceIter.polygonVertexCount();
				
				if (idxCount > 3)
				{
					nontriangulated++;
					idxCount = 3;
				}
				
				for (int v = 0; v < idxCount; v++)
				{
					newGroup->vertexIndices[index] = faceIter.vertexIndex(v);
					newGroup->normalIndices[index] = faceIter.normalIndex(v);

					uvIdx = 0;
					faceIter.getUVIndex(v, uvIdx);
					newGroup->uvIndices[index] = uvIdx;

					index++;
				}

				totalNumTriangles++;
			}

			meshgroupsVector.push_back( newGroup );
		}
		
		for (n = 0; n < (int)meshgroupsVector.size(); n++)
		{
			newMesh->meshGroups[n] = meshgroupsVector[n];
		}
		
		if (nontriangulated > 0)
		{
			printf(" *** Warning: Mesh has %d non-triangulated face(s)! ***\n",
				nontriangulated);
		}
		
		//if (mesh->vCount == 0) printf("*** Warning - mesh has no vertices!\n");
		
		bool doubleSided = false;
		fnDagNode.findPlug( "doubleSided" ).getValue(doubleSided);
		if (doubleSided) printf("*** Warning - mesh is double-sided!\n");
		
		//bool opposite = false;
		//fnDagNode.findPlug( "opposite" ).getValue(opposite);

		chunksVector.push_back( 
			new ChunkHeader(CHUNKTYPE_MESH, fnMesh.name().asChar(), newMesh) );
	}

	printf("\nTotal nr. of triangles in file: %d\n", totalNumTriangles);

	ChunksFile chunksFile( (int)chunksVector.size() );
	for (int n = 0; n < (int)chunksVector.size(); n++)
	{
		chunksFile.chunkHeaders[n] = chunksVector[n];
	}

	chunksFile.serialize( &serializer );

	serializer.close();
}

void chunksExport::exportSceneData(const char *fullpath, bool exportAll)
{
	std::string str = fullpath;
	str += ".xml";

	//bool result = false;
	FILE *outfile = fopen(str.c_str(), "wt");
	if ( outfile == NULL )
	{
		printf("chunksExport::exportSceneData() : Could not open output file '%s'\n", str.c_str());
		return;
	}

	printf("-------------------------------------------------------------------------------\n");
	printf("Exporting scene data to file '%s'\n", str.c_str());

	//MSelectionList selectionList;
	//MGlobal::getActiveSelectionList(selectionList);

	fprintf( outfile, "<scenedata>\n" );
	exportCameras  (outfile, exportAll);
	exportLights   (outfile, exportAll);
	exportMaterials(outfile, exportAll);
	fprintf( outfile, "</scenedata>\n" );

	fclose(outfile);
}

void chunksExport::exportCameras(FILE* outfile, bool exportAll)
{
	MDagPath currentPath;

	MItDag camIterator(MItDag::kDepthFirst, MFn::kCamera);

	for( ; !camIterator.isDone(); camIterator.next() )
	{
		camIterator.getPath( currentPath ); 
		//currentPath.extendToShape();

		MFnCamera fnCamera(currentPath);

    printf("\nFound camera: %s\n", fnCamera.name().asChar() );
	}
}

void chunksExport::exportLights(FILE* outfile, bool exportAll)
{
	MDagPath currentPath;

	MItDag lampIterator(MItDag::kDepthFirst, MFn::kLight);

	for( ; !lampIterator.isDone(); lampIterator.next() )
	{
		lampIterator.getPath( currentPath ); 
		currentPath.extendToShape();

		//if (currentPath.node().hasFn(MFn::kPointLight))
		{
			//MFnPointLight fnPointLight(currentPath);
			MFnLight fnLight(currentPath);
			printf("\nFound light: %s\n", fnLight.name().asChar() );
		}

		/*
		MFnLight
			MFnNonAmbientLight 
				MFnNonExtendedLight 
					MFnSpotLight 
					MFnPointLight 
					MFnDirectionalLight 
				MFnAreaLight 
			MFnAmbientLight
		*/
	}
}

void chunksExport::exportMaterials(FILE* outfile, bool exportAll)
{
	printf("\nExporting materials...\n");

	MItDependencyNodes shaderIterator(MFn::kLambert);

	for( ; !shaderIterator.isDone(); shaderIterator.next() )
	{
		MObject &shader = shaderIterator.item();
		MString shadername = nodeName(shader);
		printf("\nMaterial '%s'\n", shadername.asChar());

		/*
		color
		transparency
		ambient color
		incandescence
		diffuseCoeff
		translucenceCoeff (depth, focus)
		glowIntensity, hideSource
		refractionLimit, refractiveIndex, refractedColor

		specular: specColor, reflectivity, reflected color, rayReflectionLimit
		  phong - cosinePower (+ roughness, hilite size, whiteness)
			blinn - eccentricity, rolloff
		  (anisotropic - angle, spreadXY, roughness, fresnelIndex)

		(bumpmapping->)
		(textures: color transparency ambient incandescence bump diffuse translucence)

		(ray refraction - absorbance, thickness, shadowAttenuation, chromatic aberration)
		(ray reflection - specularity)

		MFnLambertShader
			MFnReflectShader
				MFnPhongShader
				MFnBlinnShader
		*/

		if( shader.hasFn( MFn::kPhong ))
		{
			printf("Phong material: %s\n", shadername.asChar());

			fprintf(outfile, "<material id='%s' type='%s'>\n", shadername.asChar(), "phong");
			dumpLambertParameters(outfile, shader);
			dumpReflectParameters(outfile, shader);
			dumpPhongParameters  (outfile, shader);
			fprintf(outfile, "</material>\n");
		}
		else if( shader.hasFn( MFn::kBlinn ))
		{
			printf("Blinn material: %s\n", shadername.asChar());

			fprintf(outfile, "<material id='%s' type='%s'>\n", shadername.asChar(), "blinn");
			dumpLambertParameters(outfile, shader);
			dumpReflectParameters(outfile, shader);
			dumpBlinnParameters  (outfile, shader);
			fprintf(outfile, "</material>\n");
		}
		else if( shader.hasFn( MFn::kReflect ))
		{
			printf("Lambert material: %s\n", shadername.asChar());

			fprintf(outfile, "<material id='%s' type='%s'>\n", shadername.asChar(), "reflect");
			dumpLambertParameters(outfile, shader);
			dumpReflectParameters(outfile, shader);
			fprintf(outfile, "</material>\n");
		}
		else if( shader.hasFn( MFn::kLambert ))
		{
			printf("Lambert material: %s\n", shadername.asChar());

			fprintf(outfile, "<material id='%s' type='%s'>\n", shadername.asChar(), "lambert");
			dumpLambertParameters(outfile, shader);
			fprintf(outfile, "</material>\n");
		}
		else
		{
			printf("WARNING: Unrecognized material type on node '%s'!\n", shadername.asChar());
		}
	}
}

void chunksExport::dumpLambertParameters(FILE* outfile, const MObject& shader)
{
  MFnLambertShader lambert( shader );

  MColor ambColor = lambert.ambientColor();
  MColor diffColor = lambert.color();
  float  diffCoeff = lambert.diffuseCoeff();
	MColor emitColor = lambert.incandescence();
  MColor transpColor = lambert.transparency();
	//float translucenceCoeff = lambert.translucenceCoeff();

	float glow = lambert.glowIntensity();

	// Ray tracing parameters...
	bool useRefraction = lambert.rtRefractedColor();
	float refrIdx = lambert.refractiveIndex();
	short refrLimit = lambert.refractedRayDepthLimit();

  fprintf(outfile, "  <ambient r='%.3f' g='%.3f' b='%.3f'>\n",
		ambColor[0], ambColor[1], ambColor[2]);
  fprintf(outfile, "  </ambient>\n");

  fprintf(outfile, "  <diffuse r='%.3f' g='%.3f' b='%.3f'>\n",
		diffColor[0], diffColor[1], diffColor[2]);
  fprintf(outfile, "  </diffuse>\n");

  fprintf(outfile, "  <diffuseCoeff value='%.3f'>\n",
		diffCoeff);
  fprintf(outfile, "  </diffuseCoeff>\n");

  fprintf(outfile, "  <emission r='%.3f' g='%.3f' b='%.3f'>\n",
		emitColor[0], emitColor[1], emitColor[2]);
  fprintf(outfile, "  </emission>\n");

  fprintf(outfile, "  <transparency r='%.3f' g='%.3f' b='%.3f'>\n",
		transpColor[0], transpColor[1], transpColor[2]);
  fprintf(outfile, "  </transparency>\n");

  fprintf(outfile, "  <glow value='%.3f'>\n",
		glow);
  fprintf(outfile, "  </glow>\n");

	if (useRefraction) {
		fprintf(outfile, "  <refraction index='%.3f' limit='%d'>\n",
			refrIdx, refrLimit);
		fprintf(outfile, "  </refraction>\n");
	}

/*define DECLARE_ATTR(node, attrname) \
	MFnAttribute attrname (node.attribute(#attrname));

	DECLARE_ATTR( lambert, ambientColor )

	ambientColor

	for (int n = 0; n < (int)lambert.attributeCount(); n++) {
		MObject o = lambert.attribute(n);
		MFnAttribute attr(o);
		printf("  attribute: %s\n", attr.name().asChar());
	}*/
}

void chunksExport::dumpReflectParameters(FILE* outfile, const MObject& shader)
{
	MFnReflectShader reflect( shader );

	MColor specColor = reflect.specularColor();

	// Ray tracing parameters...
	float reflectivity = reflect.reflectivity();
	short reflLimit = reflect.reflectedRayDepthLimit();
	//MColor reflect.reflectedColor();

  fprintf(outfile, "  <specular r='%.3f' g='%.3f' b='%.3f'>\n",
		specColor[0], specColor[1], specColor[2]);
  fprintf(outfile, "  </specular>\n");

  fprintf(outfile, "  <reflection limit='%d'>\n",
		reflLimit);
  fprintf(outfile, "  </reflection>\n");

	fprintf(outfile, "  <reflectivity value='%.3f'>\n",
		reflectivity);
	fprintf(outfile, "  </reflectivity>\n");
}

void chunksExport::dumpPhongParameters(FILE* outfile, const MObject& shader)
{
	MFnPhongShader phong( shader );

	float specPower = phong.cosPower();

  fprintf(outfile, "  <specularPower value='%.3f'>\n",
		specPower);
  fprintf(outfile, "  </specularPower>\n");
}

void chunksExport::dumpBlinnParameters(FILE* outfile, const MObject& shader)
{
	MFnBlinnShader blinn( shader );

	float specPower = blinn.eccentricity();
	float specRollOff = blinn.specularRollOff();

  fprintf(outfile, "  <specularPower value='%.3f'>\n",
		specPower);
  fprintf(outfile, "  </specularPower>\n");

  fprintf(outfile, "  <specularRollOff value='%.3f'>\n",
		specRollOff);
  fprintf(outfile, "  </specularRollOff>\n");
}

// (Dummy) import methods (only for verification as yet...)

void chunksExport::importMeshes(const char *fullpath, bool /*doimport*/)
{
	Serializer serializer;

	//serializer.mVerbosePrint = true;

	printf("-------------------------------------------------------------------------------\n");

	if ( !serializer.open(Serializer::SERIALIZE_READ, fullpath) )
	{
		printf("chunksExport::importMeshes() : Could not open input file '%s'\n", fullpath);
		return;
	}

	printf("Importing meshes from file '%s'\n\n", fullpath);

	ChunksFile chunksFile;

	chunksFile.serialize( &serializer );

	printf("formatstring: %s\n", chunksFile.formatstring);
	printf("numChunks: %d\n", chunksFile.numChunks);

	for (int n = 0; n < chunksFile.numChunks; n++)
	{
		ChunkHeader *chunkHeader = (ChunkHeader *)chunksFile.chunkHeaders[n];

		switch (chunkHeader->chunkType)
		{
			case CHUNKTYPE_MESH :
			{
				ChunkMesh *pMesh = (ChunkMesh *)chunkHeader->chunkData;
				printf("\n* Mesh chunk: '%s'\n", chunkHeader->chunkName);
				printf("numVertices: %d\n", pMesh->numVertices);
				printf("numNormals: %d\n", pMesh->numNormals);
				printf("numUVs: %d\n", pMesh->numUVs);

				printf("numMeshGroups: %d\n", pMesh->numMeshGroups);
				for (int g = 0; g < pMesh->numMeshGroups; g++)
				{
					MeshGroup *pGroup = pMesh->meshGroups[g];
					printf("  Mesh group: material = '%s' numTriangles = %d\n",
						pGroup->materialName, pGroup->numTriangles);
				}
				break;
			}

			default :
			{
				printf("\n* Unknown chunk '%s', type = %d\n",
					chunkHeader->chunkName, chunkHeader->chunkType);
				break;
			}
		}
	}

	serializer.close();
}

void chunksExport::importSceneData(	const char *fullpath, bool /*doimport*/)
{
}

// Helper functions

void chunksExport::printIndentation(int nSpaces)
{
	for (int i = 0; i < nSpaces; i++) putchar(' ');
}

MString chunksExport::getPathName(const MString &fullname)
{
	MString retstr = fullname;
	int lastSlashLocation = fullname.rindex('/');
	if (lastSlashLocation > 0)
	{
		retstr = fullname.substring(0, lastSlashLocation);
	}
	return retstr;
}

MString chunksExport::getBaseName(const MString &filename, const char *extension)
{
	MString retstr;
	int extLocation = filename.rindex('.');
	if (extLocation > 0 &&
			filename.substring(extLocation, filename.length() - 1) == MString(extension))
	{
		retstr = filename.substring(0, extLocation - 1);
	}
	else
	{
		retstr = filename;
	}
	return retstr;
}

MString chunksExport::nodeName(const MObject& object )
{
	return MFnDependencyNode(object).name();
}

MString chunksExport::nodeType(const MObject& object)
{
	return MFnDependencyNode(object).typeName();
}
