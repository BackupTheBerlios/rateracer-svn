Rate Racer - a renderer
-----------------------
This is my hobby raytracer and global illumination renderer project,
along with exporters and loaders for different file formats.

Build Tips:
- You need Microsoft Visual C++ and C# v7.1 (or later)
- You also need Microsoft .NET Framework 1.1 (or later)
- To build MayaExporter, create environment variable MAYA_PATH pointing to
  your Maya installation directory. Then add Maya's include and lib
  directories under MSDEV - Tools - Options - Projects - VC++ Directories.
- To start the raytracer, set RateRacerGUI as your startup project.
  Set RateRacerGUI Properties -
        Configuration Properties - Debugging - Working Directory
  to the root of your checked out directory (for all configurations),
  in order to find models etc.
  
// Gweronimo@users.berlios.de

Project log
-----------
- Tested import and checkout with SubVersion via TortoiseSVN using SVN+SSH: protocol.
- Tested anonymous checkout with SubVersion via TortoiseSVN using SVN: protocol.
- Added chunks project (serialization of binary data)
- Added MayaExporter project (chunksExport plugin, export .chunks and .chunks.xml)
- FD successfully checked everything out, and commited for the first time :-)
- Tested SubVersion conflict handling.
- Added camera export 
- Added light sources (point light and spherical light)
- Added untested code for extracting texture nodes
- Added extraction of triangles from untriangulated polygons
- gweronimo: Checked in the rest of the RateRacer project.
  It's still in an awful mess after the move to a .NET GUI.
  I enabled SSE2 optimization, so it might not work (as is) on older machines.
