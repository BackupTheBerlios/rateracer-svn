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

MPxFileTranslator::MFileKind chunksExport::identifyFile(const MFileObject& fileObject,
																											const char *buffer,
																											short /*size*/) const
{
	MFileKind rval = kNotMyFileType;
	MString magic("!CHUNKS!");
	if (0 == strncmp(buffer, magic.asChar(), magic.length()))
	{
		rval = kIsMyFileType;
	}
  else if (getExtension(fileObject.fullName().asChar()) == ".mesh")
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

  MString extension = getExtension(fileObject.fullName().asChar());
  if (extension == ".mesh")
  {
    importMeshFile(fullpath);
  }
  else if (extension == ".chunks")
  {
    importMeshes(		fullpath, (mode == MPxFileTranslator::kImportAccessMode));
    importSceneData(fullpath, (mode == MPxFileTranslator::kImportAccessMode));
  }
  else
  {
    printf("File extension '%s' not recognized!\n", extension.asChar());
  }

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

		int n, g, triangle_count, uvIdx;
		std::map< int, int > lookup_map;
		int nontriangulated = 0;

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

				if (maxNumTrisPerPolygon < triangle_count )
					maxNumTrisPerPolygon = triangle_count;

				numTris += triangle_count;
				numPolys++;
			}
			totalNumTriangles += numTris;

			printf("Group: %d triangles in %d polygons (max. %d tris per poly).\n",
				numTris, numPolys, maxNumTrisPerPolygon);

			MeshGroup* newGroup = new MeshGroup( nodeName(shader).asChar(), numTris );

			int index = 0;
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

					for (int v = 0; v < 3; v++)
					{
						newGroup->vertexIndices[index] = triangle_vertex_list[v];

						int localIdx = lookup_map[ triangle_vertex_list[v] ];

						//newGroup->vertexIndices[index] = faceIter.vertexIndex(localIdx);
						newGroup->normalIndices[index] = faceIter.normalIndex(localIdx);

						uvIdx = 0;
						faceIter.getUVIndex(localIdx, uvIdx);
						newGroup->uvIndices[index] = uvIdx;

						index++;
					}
				}
			}
			meshgroupsVector.push_back( newGroup );
		}
		
		for (n = 0; n < (int)meshgroupsVector.size(); n++)
		{
			newMesh->meshGroups[n] = meshgroupsVector[n];
		}
		
		bool doubleSided = false;
		fnDagNode.findPlug( "doubleSided" ).getValue(doubleSided);
		if (doubleSided) printf("* Warning: mesh is double-sided!\n");
		
		//bool opposite = false;
		//fnDagNode.findPlug( "opposite" ).getValue(opposite);

		if (nontriangulated > 0)
		{
			printf("* Warning: Mesh has %d polygon(s) where no valid triangulation exists!\n",
				nontriangulated);
		}
		
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
	printf("\nExporting cameras...\n");
  MSpace::Space space = MSpace::kWorld;
  MStatus status;
  /*
   * Iterate over all cameras
   */
  MItDag iter( MItDag::TraversalType::kBreadthFirst, MFn::kCamera, &status );
  /*
   * Camera counter
   */
  int cameraId = 0;
  while( !iter.isDone( ) ) {
    MDagPath path;
    status = iter.getPath( path );

    MFnCamera camera( path, &status );
    /*
     * Get the "ordinary" camera stuff
     */
    MPoint eye     = camera.eyePoint( space, &status );
    MVector up     = camera.upDirection( space, &status );
    MPoint look_at = camera.centerOfInterestPoint( space, &status );
    MVector dir    = look_at - eye;
    dir.normalize( );
    double hfov    = camera.horizontalFieldOfView( &status );
    double vfov    = camera.verticalFieldOfView( &status );
    MString name   = camera.name( );
    // What on earch is focal length used for??
    double focalLength = camera.focalLength( );
    /*
     * Ortho camera stuff
     */
    bool is_ortho  = camera.isOrtho( &status );
    double left, right, bottom, top;
    double aspect  = camera.aspectRatio( &status );
    status = camera.getRenderingFrustum( aspect, left, right, bottom, top );
    /*
     * Depth of field is a *must have*!
     * NOTE: Parameter "Focus region scale" is missing!
     */
    // Do we have dof or not?
    bool dof = camera.isDepthOfField( );
    double focusDist   = camera.focusDistance( );
    double fStop       = camera.fStop( );
    /* 
     * And write it like this:
     *    <camera id="" type="perspective/ortho/...">
     *      <position x="1.0" y="1.0" z="1.0" />
     *      <direction x="1.0" y="1.0" z="1.0" />
     *      <up x="1.0" y="1.0" z="1.0" />
     *      <fov horiz="1.0" vert="1.0" />
     *    </camera>
     */
    // Taken from GM, perhaps move this someplace else?
    float const RADTODEG = 57.29577951308232286465f;
    fprintf( outfile, "<camera id='%s' type='perspective'>\n", name.asChar( ) );
    fprintf( outfile, "  <position x='%.3f' y='%.3f' z='%.3f' />\n",  eye.x, eye.y, eye.z );
    fprintf( outfile, "  <direction x='%.3f' y='%.3f' z='%.3f' />\n", dir.x, dir.y, dir.z );
    fprintf( outfile, "  <up x='%.3f' y='%.3f' z='%.3f' />\n", up.x, up.y, up.z );
    fprintf( outfile, "  <fov horiz='%.3f' vert='%.3f' />\n", RADTODEG * hfov, RADTODEG * vfov );
    if( dof ) {
      fprintf( outfile, "  <dof focusdist='%.3f' fstop='%.3f' />\n", focusDist, fStop );
    }
    fprintf( outfile, "</camera>\n" );
    /*     
    TODO: maybe add support for orthogonal cameras?
    
    if( is_ortho ) {
        fprintf(file, "OrthoCamera %lf %lf %lf %lf  %lf %lf %lf  %lf %lf %lf  %lf %lf %lf\n",
          left, right, top, bottom, eye.x, eye.y, eye.z, look_at.x, look_at.y, look_at.z,
          up.x, up.y, up.z);
    }
    */
    iter.next();
    cameraId++;

  } // while
}


void chunksExport::exportPointlights( FILE* outfile ) {  

	printf("\nExporting point lights...\n");

  MStatus status;
  MSpace::Space space = MSpace::kWorld;
  /*
   * Iterate over all point lights
   */
  MItDag iter( MItDag::TraversalType::kBreadthFirst, MFn::kPointLight, &status );
  while( !iter.isDone() ) {
    MStatus status;
    MDagPath path;
    status = iter.getPath(path);
    MFnPointLight point_light(path);
    /*
     * Get transform and data
     */
    MObject transform_object = path.transform( &status );
    MFnTransform light_transform_temp( transform_object, &status );
    MDagPath transform_path;
    status = light_transform_temp.getPath( transform_path );
    MFnTransform light_transform( transform_path, &status );

    MColor color = point_light.color( &status );
    float intensity = point_light.intensity( &status );
    MVector translation = light_transform.translation( MSpace::kWorld, &status );
    MString name = point_light.name( );
    short decay = point_light.decayRate( );
    /*
     * If we use ray traced shadows we can also set the number of
     * samples and a radius, i.e. a spherical light
     */
    bool useRayTraceShadows = point_light.useRayTraceShadows( );
    short numShadowSamples  = point_light.numShadowSamples( );
    float radius            = point_light.shadowRadius( );
    // Dont know what this one's for
    short rayDepthLimit     = point_light.rayDepthLimit( );
    
    /*
     *  <lightsource id="" type="omni/spot/spherical/directional/area/...">
     *    <position x="1.0" y="1.0" z="1.0" />
     *    <direction x="1.0" y="1.0" z="1.0" />
     *    <radius value="1.0" />
     *    <samples u="1" v="1" />
     *    <color r="1.0" g="1.0" b="1.0" />
     *    <intensity value="1.0" />
     *  </lightsource>
     *
     *  TODO: Add decay parameter, in Maya you can choose <none, linear, quadratic, cubic>
     */
    if( useRayTraceShadows ) {
      fprintf( outfile, "<lightsource id='%s' type='spherical'>\n", name.asChar( ) );
    } else {
      fprintf( outfile, "<lightsource id='%s' type='omni'>\n", name.asChar( ) );
    }
    fprintf( outfile, "  <position x='%.3f' y='%.3f' z='%.3f' />\n", translation.x, translation.y, translation.z );
    fprintf( outfile, "  <color r='%.3f' g='%.3f' b='%.3f' />\n", color.r, color.g, color.b );
    fprintf( outfile, "  <intensity value='%.3f' />\n", intensity );

    if( useRayTraceShadows ) {
      fprintf( outfile, "  <samples u='%d' v='%d' />\n", (int)sqrtf( numShadowSamples ), (int)sqrtf( numShadowSamples ) );
      fprintf( outfile, "  <radius value='%.3f' />\n", radius );
    }    
    fprintf( outfile, "</lightsource>\n" );

    iter.next();
  } // while
}



/*
  Light hierarchy
  
		MFnLight
			MFnNonAmbientLight 
				MFnNonExtendedLight 
					MFnSpotLight 
					MFnPointLight 
					MFnDirectionalLight 
				MFnAreaLight 
			MFnAmbientLight
*/

void chunksExport::exportLights(FILE* outfile, bool exportAll)
{
  exportPointlights( outfile );
  // Not yet implemented
  //exportSpotlights( outfile );
  //exportArealights( outfile );
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
			printf("* Warning: Unrecognized material type on node '%s'!\n", shadername.asChar());
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

MStatus AssignToShadingGroup(const MObject  &shadingGroup,
                             const MDagPath &dagPath,
                             const MObject  &component)
{
  MStatus status;
  MFnSet fnSG( shadingGroup, &status );
  //printf("fnSG: %s\n", fnSG.name().asChar());

  if ( fnSG.restriction() != MFnSet::kRenderableOnly )
    return MS::kFailure;

  //printf("Adding member...\n");
  status = fnSG.addMember( dagPath, component );
  if ( status != MS::kSuccess )
  {
    printf("assignToShadingGroup() could not add Dag/Component to SG !\n");
  }
  return status;
}

void execCmd(MString cmd)
{
  MGlobal::executeCommand(cmd, true);
}

bool skipMeshTag(FILE *infile, int& numLines)
{
  char line[255];

  numLines = 0;
  fscanf(infile, "%d", &numLines);

  for (int n = 0; n < numLines; n++)
  {
    if (fgets(line, 255, infile) == NULL)
    {
      printf("Premature end of file!\n");
      return false;
    }
  }
  return true;
}

void chunksExport::importMeshFile(const char *fullpath)
{
  printf("-------------------------------------------------------------------------------\n");

  FILE *infile = fopen(fullpath, "rt");
  if ( infile == NULL )
  {
    printf("chunksExport::importMeshFile() : Could not open input file '%s'\n", fullpath);
    return;
  }

  printf("\nImporting mesh file '%s'\n\n", fullpath);

  char line[255];

  int numVertices = 0;
  int numTris = 0;
  int numMaterials = 0;
  int numLinesSkipped = 0;

  MFloatPointArray points;
  MIntArray faceEdgeCounts;
  MIntArray faceVerts;

  typedef std::map< int, std::vector<int> > MatIndexMap;
  MatIndexMap materialMap;

  while (!feof(infile))
  {
    if (fgets(line, 255, infile) == NULL)
    {
      break;
    }
    strlwr(line);

    if (strstr( line, "#" ) != NULL)
    {
      // Skipping comment line...
    }
    else if (strstr( line, "dimension" ) != NULL)
    {
      int dimension = 0;
      fscanf(infile, "%d", &dimension);
      if (dimension != 3)
      {
        printf("Dimension is %d, not 3!\n", dimension);
        break;
      }
    }
    else if (strstr( line, "vertices" ) != NULL)
    {
      fscanf(infile, "%d", &numVertices);

      printf("Num vertices: %d\n", numVertices);
      const float cBig = 1e20f;
      float xmin =  cBig, ymin =  cBig, zmin =  cBig;
      float xmax = -cBig, ymax = -cBig, zmax = -cBig;
      for (int n = 0; n < numVertices; n++)
      {
        float x = 0, y = 0, z = 0; int ref;
        fscanf(infile, "%e %e %e %d", &x, &y, &z, &ref);

        if (xmin > x) xmin = x; if (ymin > y) ymin = y; if (zmin > z) zmin = z;
        if (xmax < x) xmax = x; if (ymax < y) ymax = y; if (zmax < z) zmax = z;

        MFloatPoint pnt( x, y, z );
        points.append( pnt );
      }
      printf("BBox: (%f %f %f) - (%f %f %f)\n", xmin, ymin, zmin, xmax, ymax, zmax);
      printf("Size: (%f %f %f)\n", xmax - xmin, ymax - ymin, zmax - zmin);
      float maxDim = xmax - xmin;
      if (ymax - ymin > maxDim) maxDim = ymax - ymin;
      if (zmax - zmin > maxDim) maxDim = zmax - zmin;
      float minDim = xmax - xmin;
      if (ymax - ymin < maxDim) maxDim = ymax - ymin;
      if (zmax - zmin < maxDim) maxDim = zmax - zmin;
      float scale = 1.0f;
      while (maxDim > 10.0f) {
        maxDim *= 0.5f; scale *= 0.5f;
      }
      while (minDim < 0.1f) {
        minDim *= 2.0f; scale *= 2.0f;
      }
      if (scale != 1.0f) {
        printf("Changing scale to %f !\n", scale);
        for (n = 0; n < numVertices; n++) {
          points[n].x *= scale;
          points[n].y *= scale;
          points[n].z *= scale;
        }
      }
    }
    else if (strstr( line, "triangles" ) != NULL)
    {
      fscanf(infile, "%d", &numTris);

      printf("Num triangles: %d\n", numTris);
      for (int n = 0; n < numTris; n++)
      {
        int v1 = 0, v2 = 0, v3 = 0, mat = 0;
        fscanf(infile, "%d %d %d %d", &v1, &v2, &v3, &mat);
        if (v1 < 1 || v1 > numVertices ||
            v2 < 1 || v2 > numVertices ||
            v3 < 1 || v3 > numVertices)
        {
          printf("ERROR: vertex #%d index out of range! (%d %d %d)\n",
            n, v1, v2, v3);
        }
        // Convert 1-indexed -> 0-indexed
        faceVerts.append( v1 - 1 );
        faceVerts.append( v2 - 1 );
        faceVerts.append( v3 - 1 );
        faceEdgeCounts.append( 3 );

        materialMap[mat].push_back(n);
      }
    }
    else if (strstr( line, "corners" ) != NULL)
    {
      if (!skipMeshTag(infile, numLinesSkipped)) break;
      printf("Num corners: %d (skipped)\n", numLinesSkipped);
    }
    else if (strstr( line, "edges" ) != NULL)
    {
      if (!skipMeshTag(infile, numLinesSkipped)) break;
      printf("Num edges: %d (skipped)\n", numLinesSkipped);
    }
    else if (strstr( line, "ridges" ) != NULL)
    {
      if (!skipMeshTag(infile, numLinesSkipped)) break;
      printf("Num ridges: %d (skipped)\n", numLinesSkipped);
    }
    else if (strstr( line, "end" ) != NULL)
    {
      printf("Found end tag!\n");
    }
    else
    {
      printf("// %s", line);
    }
  }
  fclose(infile);


  //int edgesPerFace = 3;
  //int numFaceVerts = numTriangles * edgesPerFace;
  //int numEdges = numFaceVerts / 2;

  MStatus stat;

  MFnMesh fnMesh;
  MObject newTransformNode = fnMesh.create( numVertices, numTris, points, 
    faceEdgeCounts, faceVerts, MObject::kNullObj, &stat );
  if (stat != MS::kSuccess) {
    printf("Could not create MFnMesh\n");
    return;
  }

  // Primitive is created so tell shape it has changed
  fnMesh.updateSurface();
  //fnMesh.syncObject();
  //fnMesh.userNode()

  //MDGModifier dgModifier;
  //dgModifier.renameNode( newTransformNode, "pImportedMesh1" );
  //dgModifier.doIt();

  // Put the polygon into initial shading group
  //execCmd("sets -e -fe initialShadingGroup " + fnMesh.name());

  MDagPath polyDagPath;
  MItDag dagIterator(MItDag::kDepthFirst, MFn::kMesh);
  for( ; !dagIterator.isDone(); dagIterator.next() )
  {
    dagIterator.getPath(polyDagPath);
    MFnDagNode polyDagNode( polyDagPath );
    //printf("Dag node: %s path: %s\n",
    //  polyDagNode.name().asChar(), polyDagPath.fullPathName().asChar());
    if (polyDagNode.name() == fnMesh.name())
    {
      //printf("Found matching mesh node...\n");
      break;
    }
  }
  MFnDagNode fnDagNode( polyDagPath, &stat );
  MDagPath dagPath = fnDagNode.dagPath();

  /////////////////////////////////////////////////////////////////////////////////

  printf("Num materials: %d\n", materialMap.size());

  MatIndexMap::iterator it;
  for (it = materialMap.begin(); it != materialMap.end(); it++)
  {
    MFnPhongShader fnPhong;
    MObject newShader = fnPhong.create();
    float r = float(rand()) / float(RAND_MAX);
    float g = float(rand()) / float(RAND_MAX);
    float b = float(rand()) / float(RAND_MAX);
    MColor color(r,g,b);
    fnPhong.setColor(color);
    MString shaderName = fnPhong.name();

    printf("  Material #%d: %d triangles, assigning shader '%s'\n",
      it->first, it->second.size(), shaderName.asChar());

    execCmd("sets -renderable true -noSurfaceShader true -empty -name " + shaderName + "SG");
    execCmd("connectAttr -f " + shaderName + ".outColor " + shaderName + "SG.surfaceShader");

    MSelectionList shaderSel;
    MGlobal::getSelectionListByName( shaderName + "SG", shaderSel );
    MObject selectedSG;
    shaderSel.getDependNode( 0, selectedSG );
    //printf("Selected shadergroup node (%d items in selection)\n", shaderSel.length());
    //printf("Selected shadergroup: %s %s %s\n",
    //  selectedSG.apiTypeStr(), nodeType(selectedSG).asChar(), nodeName(selectedSG).asChar());

    MFnSingleIndexedComponent faces;
    MObject components = faces.create( MFn::kMeshPolygonComponent );
    std::vector<int>::iterator f;
    for (f = it->second.begin(); f != it->second.end(); f++) {
      faces.addElement( (*f) );
    }
    /*
    MSelectionList selectedFaces;
    selectedFaces.add(dagPath, components);
    MGlobal::setActiveSelectionList( selectedFaces );
    execCmd("ls -sl");
    */
    AssignToShadingGroup( selectedSG, dagPath, components );
  }

  execCmd("modelEditor -edit -displayAppearance smoothShaded -activeOnly false modelPanel4");

  execCmd("select -r " + fnDagNode.name());
  execCmd("FrameSelectedInAllViews");
  execCmd("fitAllPanels -selected");

  //execCmd("FrameAllInAllViews");
  //execCmd("fitAllPanels -all");
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

MString chunksExport::getExtension(const MString &filename)
{
  MString retstr = "";
  int extLocation = filename.rindex('.');
  if (extLocation > 0) {
    retstr = filename.substring(extLocation, filename.length() - 1);
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
