/*
 *  Untested code snippets for exporting cameras
 *
 *  @author Fredrik Dyrkell
 *  @date   2004-06-04
 */

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
     * Get the data
     */
    MPoint eye     = camera.eyePoint( space, &status );
    MVector up     = camera.upDirection( space, &status ;
    MPoint look_at = camera.centerOfInterestPoint( space, &status );
    // Dont know if this will work!
    MVector dir    = (look_at - eye).normalize( );
    double aspect  = camera.aspectRatio( &status );
    double hfov    = camera.horizontalFieldOfView( &status );
    // Have no idea if aspect is h/w or vice versa!
    double vfov    = hfov * aspect;
    bool is_ortho  = camera.isOrtho( &status );
    double left, right, bottom, top;
    status = camera.getRenderingFrustum( aspect, left, right, bottom, top );
    /* 
     * And write it like this:
     *    <camera id="" type="perspective/ortho/...">
     *      <position x="1.0" y="1.0" z="1.0" />
     *      <direction x="1.0" y="1.0" z="1.0" />
     *      <up x="1.0" y="1.0" z="1.0" />
     *      <fov horiz="1.0" vert="1.0" />
     *    </camera>
     */
    fprintf( file, "<camera id='%d' type='perspective'>\n", cameraId );
    fprintf( file, "  <position x='%f' y='%f' z='%f' />\n",  eye.x, eye.y, eye.z );
    fprintf( file, "  <direction x='%f' y='%f' z='%f' />\n", dir.x, dir.y, dir.z );
    fprintf( file, "  <up x='%f' y='%f' z='%f' />\n", up.x, up.y, up.z );
    fprintf( file, "  <fov horiz='%f' vert='%f' />\n", RADTODEG * hfov, RADTODEG * vfov );
    fprintf( file, "</camera>\n" );
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




