/*
 *  Untested code snippets for exporting light sources
 *
 *  @author Fredrik Dyrkell
 *  @date   2004-06-04
 */

void exportPointlights( ) {  
  MStatus status;
  MSpace::Space space = MSpace::kWorld;
  /*
   * Iterate over all point lights
   */
  MItDag iter( MItDag::TraversalType::kBreadthFirst, MFn::kPointLight, &status );
  int lightId = 0;
  
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
     *  NOTE: an omni light with a radius might as well be used 
     *        instead of a spherical light? Or do we want to separate 
     *        those to?
     */
    // Need to have more than a number for the id since we will have
    // the same way of numbering the other types of light sources.
    fprintf( file, "<lightsource id='OmniLight %d' type='omni'>\n", lightId );
    fprintf( file, "  <position x='%f' y='%f' z='%f' />\n", translation.x, translation.y, translation.z );
    fprintf( file, "  <samples u='1' v='1' />\n" );
    fprintf( file, "  <color r='%f' g='%f' b='%f' />\n", color.r, color.g, color.b );
    fprintf( file, "  <intensity value='%f' />\n", intensity );
    fprintf( file, "</lightsource>\n", lightId );

    iter.next();
    lightId++;
    
  } // while
}


void exportSpotlights(FILE* file, unsigned depth)
{
  MStatus status;
  MSpace::Space space = MSpace::kWorld;
  /*
   * Iterate over all spot lights
   */
  MItDag iter( MItDag::TraversalType::kBreadthFirst, MFn::kSpotLight, &status );
  int lightId = 0; 
  
  while( !iter.isDone( ) ) {
    MStatus status;
    MDagPath path;
    status = iter.getPath( path );
    MFnSpotLight spot_light( path );
    /*
     * Get transform and data
     */
    MObject transform_object = path.transform( &status );
    MFnTransform light_transform_temp( transform_object, &status );
    MDagPath transform_path;
    status = light_transform_temp.getPath( transform_path );
    MFnTransform light_transform( transform_path, &status );

    MColor color        = spot_light.color(&status);
    float intensity     = spot_light.intensity(&status);
    double inner_cone   = spot_light.coneAngle(&status);
    double outer_cone   = inner_cone + spot_light.penumbraAngle(&status);
    double start_dist   = spot_light.startDistance(MFnSpotLight::kFirst, &status);
    double end_dist     = spot_light.endDistance(MFnSpotLight::kFirst, &status);
    MVector translation = light_transform.translation(MSpace::kWorld, &status);
    MFloatVector dir    = spot_light.lightDirection(0, MSpace::kWorld, &status);
    /*
     * TODO: Read out how many samples that should 
     *       be used to sample the light source!
     */
    int uSamples = 1;
    int vSampels = 1;

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
     *  NOTE: an omni light with a radius might as well be used 
     *        instead of a spherical light? Or do we want to separate 
     *        those to?
     */
    // Need to have more than a number for the id since we will have
    // the same way of numbering the other types of light sources.
    fprintf( file, "<lightsource id='SpotLight %d' type='spot'>\n", lightId );
    fprintf( file, "  <position x='%f' y='%f' z='%f' />\n", translation.x, translation.y, translation.z );
    fprintf( file, "  <direction x='%f' y='%f' z='%f' />\n", dir.x, dir.y, dir.z );
    fprintf( file, "  <samples u='%d' v='%d' />\n", uSamples, vSamples );
    fprintf( file, "  <color r='%f' g='%f' b='%f' />\n", color.r, color.g, color.b );
    fprintf( file, "  <intensity value='%f' />\n", intensity );
    fprintf( file, "</lightsource>\n" );
    // TODO: Print spot-specifics, inner/outer cone and start & end distance!

    iter.next();
    lightId++;

  } // while
}
