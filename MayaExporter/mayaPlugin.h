#pragma once

// 
// Plugin for Maya
//

// Allow us to keep track of the plugin version number
extern char *chunksPluginVersion;

// Plugin enter/exit points
extern __declspec( dllexport ) MStatus initializePlugin(MObject obj);
extern __declspec( dllexport ) MStatus uninitializePlugin(MObject obj);

// Helper function
extern void BringMayaOutputWindowToFront();
