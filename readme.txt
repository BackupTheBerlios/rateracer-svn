RateRacer project log
---------------------

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
