/*
 * Iostream is pure evil
 */
#define REQUIRE_IOSTREAM

#include <vector>
#include <map>

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
#include <maya/MPointArray.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnComponent.h>
#include <maya/MFnSingleIndexedComponent.h>
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
#include <math.h>

#include "tetraPlugin.h"
#include "mayaPlugin.h"

#pragma warning(disable : 4702 4239)

tetraPlugin::tetraPlugin()
{
}

tetraPlugin::~tetraPlugin()
{
}

MPxFileTranslator::MFileKind tetraPlugin::identifyFile(const MFileObject& fileObject,
																											const char *buffer,
																											short /*size*/) const
{
	MFileKind rval = kNotMyFileType;
  //if (getExtension(fileObject.fullName().asChar()) == ".mesh")
  {
    //rval = kIsMyFileType;
  }
	return rval;
}

MStatus tetraPlugin::reader(	const MFileObject& fileObject,
														const MString& /*options*/,
														MPxFileTranslator::FileAccessMode mode )
{
	// Reader implementation is only used for verification, as yet...

	char fullpath[1024];
	strcpy( fullpath, fileObject.fullName().asChar() );

  MString extension = getExtension(fileObject.fullName().asChar());
  //if (extension == ".mesh")
  {
    //importMeshFile(fullpath);
  }
  //else
  {
    printf("File extension '%s' not recognized!\n", extension.asChar());
  }

	_flushall(); // Make sure output window is updated;
	BringMayaOutputWindowToFront();
	
	return MS::kSuccess;
}

MStatus tetraPlugin::writer( const MFileObject &fileObject,
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
	MString basename = getBaseName(fileObject.name(), ".smesh");
	
	char fullpath[1024];
	strcpy( fullpath, pathname.asChar() );
	strcat( fullpath, basename.asChar() );
	strcat( fullpath, ".smesh" );
	
	exportMeshes(		fullpath, (mode == MPxFileTranslator::kExportActiveAccessMode));

	MAnimControl::setCurrentTime(origTime);
	
	_flushall(); // Make sure output window is updated;
	BringMayaOutputWindowToFront();
	
	return MS::kSuccess;
}

void tetraPlugin::parseOptions(const MString& options)
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

bool tetraPlugin::isNodeInSelection(const MDagPath & path,
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

MObject tetraPlugin::findFirstPlugConnection(const MObject& object,
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

void tetraPlugin::exportMeshes(const char *fullpath, bool exportSelectedOnly)
{
	printf("-------------------------------------------------------------------------------\n");

  FILE *outfile = fopen(fullpath, "wt");
  if ( outfile == NULL )
	{
		printf("tetraPlugin::exportMeshes() : Could not open output file '%s'\n", fullpath);
		return;
	}

	printf("Exporting meshes to file '%s'\n", fullpath);

	MDagPath currentPath;

	MSelectionList selectionList;
	MGlobal::getActiveSelectionList(selectionList);

	int totalNumPolys = 0;

	MItDag dagIterator(MItDag::kDepthFirst, MFn::kMesh);
	for( ; !dagIterator.isDone(); dagIterator.next() )
	{
		dagIterator.getPath( currentPath ); 
		//currentPath.extendToShape();
    MFnDagNode fnDagNode( currentPath );
		
    if(fnDagNode.isIntermediateObject())
      continue;

    bool visible;
    fnDagNode.findPlug("visibility").getValue(visible);
    if (!visible) continue;

		if (exportSelectedOnly && !isNodeInSelection(currentPath, selectionList))
			continue;

		MStatus stat;
		MFnMesh fnMesh( currentPath, &stat );
		if( stat != MS::kSuccess )
		{
			printf("*** Error - failed to get mesh function set in buildMesh().\n");
			continue;
		}

		//FIXME export mesh transform (fnMesh.transformationMatrix() ?)

		/////////////////////////////////////////////////////////////////////////////

		MFloatPointArray  pointArray;
		//MColorArray       vcolorArray;
		MFloatVectorArray normalArray;
		MFloatArray       texUArray;
		MFloatArray       texVArray;
		MObjectArray			shaderSets;
		MObjectArray			faceGroups;

    // FIXME replace with getConnectedShaders() ...
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
					//printf("\nEmpty shader group skipped!\n");
				}
			}
		}

		if (numGroups == 0) {
			printf("\nEmpty mesh skipped!\n");
			continue;
		}

		printf("\nMesh object: %s\n", fnMesh.name().asChar());
		printf("Vertices: %d\n", fnMesh.numVertices());
		//printf("Colors:   %d\n", fnMesh.num());
		printf("Normals:  %d\n", fnMesh.numNormals());
		printf("UVs:      %d\n", fnMesh.numUVs());
		printf("%d groups with a total of %d polygons:\n", numGroups, fnMesh.numPolygons());

		MSpace::Space coordSpace = MSpace::kWorld;

		fnMesh.getPoints( pointArray, coordSpace );
		//fnMesh.getVertexColors( vcolorArray );
		fnMesh.getNormals( normalArray, coordSpace );
		fnMesh.getUVs( texUArray, texVArray );

		int n, g;
    //int uvIdx;
		std::map< int, int > lookup_map;

		int numVertices = fnMesh.numVertices();
		int numNormals = fnMesh.numNormals();
		int numUVs = fnMesh.numUVs();

    fprintf(outfile, "# nodes\n");
    fprintf(outfile, "%d 3 0 0\n", numVertices);
    fprintf(outfile, "\n");

		for (n = 0; n < numVertices; n++) {
      fprintf(outfile, "%d %f %f %f\n",
        n+1, pointArray[n].x, pointArray[n].y, pointArray[n].z);
			//newMesh->vertices[3*n+0] = pointArray[n].x;
			//newMesh->vertices[3*n+1] = pointArray[n].y;
			//newMesh->vertices[3*n+2] = pointArray[n].z;
		}
    fprintf(outfile, "\n");

    /*
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
    */

		for (g = 0; g < (int)shaderSets.length(); g++)
		{
      if (shaderSets.length() > 1)
      {
			  // Skip default shader connection if multiple shaders exist
			  if (MFnComponent(faceGroups[g]).elementCount() == 0)
			  {
				  printf("(Empty shader group skipped)\n");
				  continue;
			  }
      }

			MObject shader = findFirstPlugConnection(shaderSets[g], "surfaceShader");

			MItMeshPolygon faceIter( currentPath, faceGroups[g] );

			int numPolys = 0;
			for( ; !faceIter.isDone(); faceIter.next() )
			{
				numPolys++;
			}
			totalNumPolys += numPolys;

      fprintf(outfile, "# facets\n");
      fprintf(outfile, "%d 0\n", totalNumPolys);
      fprintf(outfile, "\n");

			printf("Group: %d polygons.\n", numPolys);

			//int index = 0;
			for( faceIter.reset(); !faceIter.isDone(); faceIter.next() )
			{
				int idxCount = faceIter.polygonVertexCount();
				//bool has_uvs = faceIter.hasUVs();

        fprintf(outfile, "%d", idxCount);

				for (int v = 0; v < idxCount; v++)
				{
          fprintf(outfile, " %d", faceIter.vertexIndex(v)+1);

					//newGroup->vertexIndices[index] = triangle_vertex_list[v];

					//int localIdx = lookup_map[ triangle_vertex_list[v] ];

					//newGroup->vertexIndices[index] = faceIter.vertexIndex(localIdx);
					//newGroup->normalIndices[index] = faceIter.normalIndex(localIdx);

					//uvIdx = 0;
					//faceIter.getUVIndex(localIdx, uvIdx);
					//newGroup->uvIndices[index] = uvIdx;

					//index++;
				}
        fprintf(outfile, "\n");
			}
		}
    fprintf(outfile, "\n");
		
    fprintf(outfile, "# holes\n");
    fprintf(outfile, "0\n");
    fprintf(outfile, "\n");

    fprintf(outfile, "# regions\n");
    fprintf(outfile, "0\n");
    fprintf(outfile, "\n");

		bool doubleSided = false;
		fnDagNode.findPlug( "doubleSided" ).getValue(doubleSided);
		if (doubleSided) printf("* Warning: mesh is double-sided!\n");
		
		//bool opposite = false;
		//fnDagNode.findPlug( "opposite" ).getValue(opposite);

    break;
	}

	printf("\nTotal nr. of polygons in file: %d\n", totalNumPolys);

	fclose(outfile);
}

// Helper functions

void tetraPlugin::printIndentation(int nSpaces)
{
	for (int i = 0; i < nSpaces; i++) putchar(' ');
}

MString tetraPlugin::getPathName(const MString &fullname)
{
	MString retstr = fullname;
	int lastSlashLocation = fullname.rindex('/');
	if (lastSlashLocation > 0)
	{
		retstr = fullname.substring(0, lastSlashLocation);
	}
	return retstr;
}

MString tetraPlugin::getBaseName(const MString &filename, const char *extension)
{
	MString retstr;

  MString ext = "";
  int extLocation = filename.rindex('.');
  if (extLocation > 0) {
    ext = filename.substring(extLocation, filename.length() - 1);
  }

	if (ext == MString(extension)) {
		retstr = filename.substring(0, extLocation - 1);
	}
	else {
		retstr = filename;
	}
	return retstr;
}

MString tetraPlugin::getExtension(const MString &filename)
{
  MString retstr = "";
  int extLocation = filename.rindex('.');
  if (extLocation > 0) {
    retstr = filename.substring(extLocation, filename.length() - 1);
  }
  return retstr;
}

MString tetraPlugin::nodeName(const MObject& object )
{
	return MFnDependencyNode(object).name();
}

MString tetraPlugin::nodeType(const MObject& object)
{
	return MFnDependencyNode(object).typeName();
}
