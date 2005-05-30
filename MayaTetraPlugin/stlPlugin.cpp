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

#include "stlPlugin.h"
#include "mayaPlugin.h"

#pragma warning(disable : 4702 4239)

stlPlugin::stlPlugin()
{
}

stlPlugin::~stlPlugin()
{
}

MPxFileTranslator::MFileKind stlPlugin::identifyFile(const MFileObject& fileObject,
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

MStatus stlPlugin::reader(	const MFileObject& fileObject,
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

MStatus stlPlugin::writer( const MFileObject &fileObject,
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
	MString basename = getBaseName(fileObject.name(), ".stl");
	
	char fullpath[1024];
	strcpy( fullpath, pathname.asChar() );
	strcat( fullpath, basename.asChar() );
	strcat( fullpath, ".stl" );
	
	exportMeshes(fullpath, (mode == MPxFileTranslator::kExportActiveAccessMode));

  strcat( fullpath, ".vol" );
	exportVol(fullpath, (mode == MPxFileTranslator::kExportActiveAccessMode));

	MAnimControl::setCurrentTime(origTime);
	
	_flushall(); // Make sure output window is updated;
	BringMayaOutputWindowToFront();
	
	return MS::kSuccess;
}

void stlPlugin::parseOptions(const MString& options)
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

bool stlPlugin::isNodeInSelection(const MDagPath & path,
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

MObject stlPlugin::findFirstPlugConnection(const MObject& object,
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

void stlPlugin::exportMeshes(const char *fullpath, bool exportSelectedOnly)
{
	printf("-------------------------------------------------------------------------------\n");

  FILE *outfile = fopen(fullpath, "wt");
  if ( outfile == NULL )
	{
		printf("stlPlugin::exportMeshes() : Could not open output file '%s'\n", fullpath);
		return;
	}

	printf("Exporting meshes to file '%s'\n", fullpath);

	MDagPath currentPath;

	MSelectionList selectionList;
	MGlobal::getActiveSelectionList(selectionList);

	int totalNumTriangles = 0;
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

		int g, triangle_count;
		std::map< int, int > lookup_map;
		int nontriangulated = 0;

		int numVertices = fnMesh.numVertices();
		int numNormals = fnMesh.numNormals();
		int numUVs = fnMesh.numUVs();

    fprintf(outfile, "solid\n");

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

			int numTris = 0;
			int numPolys = 0;
			int maxNumTrisPerPolygon = 0;
			for( ; !faceIter.isDone(); faceIter.next() )
			{
				if (!faceIter.hasValidTriangulation()) {
					continue;
				}

				triangle_count = 0;
				/*status =*/ faceIter.numTriangles(triangle_count);
				if (triangle_count == 0) {
					continue;
				}

				if (maxNumTrisPerPolygon < triangle_count)
					maxNumTrisPerPolygon = triangle_count;

				numTris += triangle_count;
				numPolys++;
			}
			totalNumTriangles += numTris;
			totalNumPolys += numPolys;

			printf("Group: %d triangles in %d polygons (max. %d tris per poly).\n",
				numTris, numPolys, maxNumTrisPerPolygon);

			for( faceIter.reset(); !faceIter.isDone(); faceIter.next() )
			{
				if (!faceIter.hasValidTriangulation()) {
					//printf("* Warning: Skipping polygon %i since it has no valid triangulation.\n", faceIter.index());
					nontriangulated++;
					continue;
				}

				triangle_count = 0;
				/*status =*/ faceIter.numTriangles(triangle_count);
				if (triangle_count == 0) {
					printf("* Warning: Polygon %i contains no triangles.\n", faceIter.index());
					continue;
				}

				//bool has_uvs = faceIter.hasUVs();

				// Mapping of mesh-relative vertex indices to polygon-relative indices
				lookup_map.clear();
				int idxCount = faceIter.polygonVertexCount();
				for (int i = 0; i < idxCount; i++) {
					lookup_map[faceIter.vertexIndex(i)] = i;
				}

        MVector faceNormal;
        faceIter.getNormal(faceNormal, MSpace::kWorld);

				// for every triangle in this polygon...
				for (int t = 0; t < triangle_count; t++)
				{
					MPointArray triangle_points;
					MIntArray triangle_vertex_list;
					faceIter.getTriangle(t, triangle_points, triangle_vertex_list, MSpace::kObject);

					if (triangle_vertex_list.length() != 3) {
						printf("* Error: Triangle %i in polygon %i has %d indices!\n",
							t, faceIter.index(), triangle_vertex_list.length());
					}

          fprintf(outfile, " facet normal %f %f %f\n",
            faceNormal.x, faceNormal.y, faceNormal.z);
          fprintf(outfile, "  outer loop\n");

					for (int v = 0; v < 3; v++)
					{
						int localIdx = lookup_map[ triangle_vertex_list[v] ];

            fprintf(outfile, "   vertex %f %f %f\n", 
              faceIter.point(localIdx).x,
              faceIter.point(localIdx).y,
              faceIter.point(localIdx).z);
          }

          fprintf(outfile, "  endloop\n");
          fprintf(outfile, " endfacet\n");
        }
			}
		}
    //fprintf(outfile, "\n");

    fprintf(outfile, "endsolid\n");

    /*
    fprintf(outfile, "# holes\n");
    fprintf(outfile, "0\n");
    fprintf(outfile, "\n");

    fprintf(outfile, "# regions\n");
    fprintf(outfile, "0\n");
    fprintf(outfile, "\n");
    */

		bool doubleSided = false;
		fnDagNode.findPlug( "doubleSided" ).getValue(doubleSided);
		if (doubleSided) printf("* Warning: mesh is double-sided!\n");
		
		//bool opposite = false;
		//fnDagNode.findPlug( "opposite" ).getValue(opposite);

    //break;
	}

	printf("\nTotal nr. of polygons in file: %d\n", totalNumPolys);
	printf("\nTotal nr. of triangles in file: %d\n", totalNumTriangles);

	fclose(outfile);
}

void stlPlugin::exportVol(const char *fullpath, bool exportSelectedOnly)
{
	printf("-------------------------------------------------------------------------------\n");

  FILE *outfile = fopen(fullpath, "wt");
  if ( outfile == NULL )
	{
		printf("stlPlugin::exportVol() : Could not open output file '%s'\n", fullpath);
		return;
	}

	printf("Exporting meshes to file '%s'\n", fullpath);

	MDagPath currentPath;

	MSelectionList selectionList;
	MGlobal::getActiveSelectionList(selectionList);

	int totalNumTriangles = 0;
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

		int g, triangle_count;
		//std::map< int, int > lookup_map;
		int nontriangulated = 0;

		int numVertices = fnMesh.numVertices();
		int numNormals = fnMesh.numNormals();
		int numUVs = fnMesh.numUVs();

    fprintf(outfile, "mesh3d\n");
    fprintf(outfile, "dimension\n");
    fprintf(outfile, "3\n\n");

    fprintf(outfile, "points\n");
    fprintf(outfile, "%d\n", numVertices);

		for (int n = 0; n < numVertices; n++) {
      fprintf(outfile, "  %f %f %f\n",
        pointArray[n].x, pointArray[n].y, pointArray[n].z);
		}
    fprintf(outfile, "\n");

    fprintf(outfile, "volumeelements\n");
    fprintf(outfile, "0\n\n");

    fprintf(outfile, "edgesegments\n");
    fprintf(outfile, "0\n\n");

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

			int numTris = 0;
			int numPolys = 0;
			int maxNumTrisPerPolygon = 0;
			for( ; !faceIter.isDone(); faceIter.next() )
			{
				if (!faceIter.hasValidTriangulation()) {
					continue;
				}

				triangle_count = 0;
				/*status =*/ faceIter.numTriangles(triangle_count);
				if (triangle_count == 0) {
					continue;
				}

				if (maxNumTrisPerPolygon < triangle_count)
					maxNumTrisPerPolygon = triangle_count;

				numTris += triangle_count;
				numPolys++;
			}
			totalNumTriangles += numTris;
			totalNumPolys += numPolys;

			printf("Group: %d triangles in %d polygons (max. %d tris per poly).\n",
				numTris, numPolys, maxNumTrisPerPolygon);

      fprintf(outfile, "surfaceelements\n");
      fprintf(outfile, "%d\n", numTris);

			for( faceIter.reset(); !faceIter.isDone(); faceIter.next() )
			{
				if (!faceIter.hasValidTriangulation()) {
					//printf("* Warning: Skipping polygon %i since it has no valid triangulation.\n", faceIter.index());
					nontriangulated++;
					continue;
				}

        // Domain is per polygon, not per triangle...
        int surface_nr = 1, boundary_condition_nr = 0;
        int domainOutside = 0, domainInside = 0;
        fnMesh.getIntBlindData(faceIter.index(), MFn::kMeshPolygonComponent, 1001, "matOut", domainOutside);
        fnMesh.getIntBlindData(faceIter.index(), MFn::kMeshPolygonComponent, 1001, "matIn", domainInside);
        //printf("CompID: %d - outside = %d, inside = %d\n", faceIter.index(), domainOutside, domainInside);

				triangle_count = 0;
				/*status =*/ faceIter.numTriangles(triangle_count);
				if (triangle_count == 0) {
					printf("* Warning: Polygon %i contains no triangles.\n", faceIter.index());
					continue;
				}

				//bool has_uvs = faceIter.hasUVs();

        /*
				int idxCount = faceIter.polygonVertexCount();
				// Mapping of mesh-relative vertex indices to polygon-relative indices
				lookup_map.clear();
				for (int i = 0; i < idxCount; i++) {
					lookup_map[faceIter.vertexIndex(i)] = i;
				}*/

        //MVector faceNormal;
        //faceIter.getNormal(faceNormal, MSpace::kWorld);

				// for every triangle in this polygon...
				for (int t = 0; t < triangle_count; t++)
				{
					MPointArray triangle_points;
					MIntArray triangle_vertex_list;
					faceIter.getTriangle(t, triangle_points, triangle_vertex_list, MSpace::kObject);

					if (triangle_vertex_list.length() != 3) {
						printf("* Error: Triangle %i in polygon %i has %d indices!\n",
							t, faceIter.index(), triangle_vertex_list.length());
					}

          fprintf(outfile, "  %d %d %d %d 3 %d %d %d\n",
            surface_nr, boundary_condition_nr,
            domainInside, domainOutside,
            triangle_vertex_list[0] + 1,
            triangle_vertex_list[1] + 1,
            triangle_vertex_list[2] + 1);

					//for (int v = 0; v < 3; v++)
					//{
					//	int localIdx = lookup_map[ triangle_vertex_list[v] ];
          //}
        }
			}
      break;
		}

    bool doubleSided = false;
		fnDagNode.findPlug( "doubleSided" ).getValue(doubleSided);
		if (doubleSided) printf("* Warning: mesh is double-sided!\n");
		
		//bool opposite = false;
		//fnDagNode.findPlug( "opposite" ).getValue(opposite);

    break;
	}

	printf("\nTotal nr. of polygons in file: %d\n", totalNumPolys);
	printf("\nTotal nr. of triangles in file: %d\n", totalNumTriangles);

	fclose(outfile);
}

// Helper functions

void stlPlugin::printIndentation(int nSpaces)
{
	for (int i = 0; i < nSpaces; i++) putchar(' ');
}

MString stlPlugin::getPathName(const MString &fullname)
{
	MString retstr = fullname;
	int lastSlashLocation = fullname.rindex('/');
	if (lastSlashLocation > 0)
	{
		retstr = fullname.substring(0, lastSlashLocation);
	}
	return retstr;
}

MString stlPlugin::getBaseName(const MString &filename, const char *extension)
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

MString stlPlugin::getExtension(const MString &filename)
{
  MString retstr = "";
  int extLocation = filename.rindex('.');
  if (extLocation > 0) {
    retstr = filename.substring(extLocation, filename.length() - 1);
  }
  return retstr;
}

MString stlPlugin::nodeName(const MObject& object )
{
	return MFnDependencyNode(object).name();
}

MString stlPlugin::nodeType(const MObject& object)
{
	return MFnDependencyNode(object).typeName();
}
