/*
 *  Untested code snippets for exporting texture nodes
 *
 *  @author Fredrik Dyrkell
 *  @date   2004-06-04
 */


/*
 * Extract the texture file name for a specific attribute
 * if there is one.
 *
 * @return  True if the material attribute got a texture plug.
 *          Path to the filename in textureFile
 */
bool getTextureForAttribute( MObject material, MString attribute, /*[OUT]*/ MString* textureFile ) {  

  const MString TEXTURE_NAME( "fileTextureName" );
  MFnDependencyNode fnDependNode( material );
  MPlug plug = fnDependNode.findPlug( attribute, &status );
  if( status == MS::kSuccess ) {

    /*
     * If you're looping through arbitrary attributes here, you could
     * add a check to ensure that the queried plug represents a
     * (double3) attribute, and perhaps even MFnAttribute::isUsedAsColor().
     */
    // Is plug connected?
    MPlugArray cc;

    /*
     * Do not simply check for return value here; this returns TRUE if the
     * the plug is _either_ the source or destination for a connection,
     * even though we've specified only destination.
     */
    plug.connectedTo( cc, true /* asDst */, false );
    if( cc.length() > 0 ) {
      // As a destination there can be only one source.
      MObject src = cc[0];
      // Is it a file texture?
      if ( src.hasFn( MFn::kFileTexture ) ) {
        MFnDependencyNode fnFile( src );
        MPlug ftnPlug = fnFile.findPlug( TEXTURE_NAME, &status );
        if ( status == MS::kSuccess ) {
          MString fileTextureName;
          ftnPlug.getValue( fileTextureName );
          // Return the texture name
          *textureFile = fileTextureName;
          return true;
        }
      }
      // Not a file node? Add support for other nodes here.
        else {
        printf( "Warning: Unsupported texture node. Texture ignored...\n" );
      }
    }
    else {
      return false;      
      /*
      // The plug is not connected. Just read its color values.
      double r, g, b;
      plug.getChild(0).getValue( r );
      plug.getChild(1).getValue( g );
      plug.getChild(2).getValue( b );
      */
    }
  }
  return false;
}

