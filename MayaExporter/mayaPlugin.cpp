// 
// RateRacer (.chunks & .xml) Export Plugin for Maya
//

#include <iostream>

#pragma comment(lib, "Foundation.lib")
#pragma comment(lib, "OpenMayaUI.lib")
#pragma comment(lib, "OpenMaya.lib")
#pragma comment(lib, "OpenMayaAnim.lib")
#pragma comment(lib, "OpenMayaFX.lib")
#pragma comment(lib, "Image.lib")

#pragma warning( disable : 4100 )
#define NT_ENV
#define NT_PLUGIN
#include <maya/MFnPlugin.h>
#pragma warning( default : 4100 )

#include "chunksExport.h"
#include "mayaPlugin.h"

//======================================================================

// Helper function
void BringMayaOutputWindowToFront()
{
	HWND hWnd = FindWindow(NULL, "Output Window");
	if (hWnd != NULL) {
		BringWindowToTop(hWnd);
	}
}

//======================================================================
// The following string is used to define the version number to the Maya
// plug-ins Manager.  Change this whenever the translator is modified.
char *chunksPluginVersion = "CHNK0.01";
//======================================================================

__declspec( dllexport ) MStatus initializePlugin(MObject obj)
{
	MStatus	status, lasterror = MS::kSuccess;
	
	MFnPlugin plugin(obj, "Rate Racer Export Plugin for Maya", chunksPluginVersion);
	
	status = plugin.registerFileTranslator(
		"chunksExport",
		"chunksExport.rgb",
		chunksExport::creator,
		"chunksExportOptions", "",
		true);
	if (!status) { std::cerr << status; lasterror = status; }

	// Set the mel scripts to be run when the plugin is loaded / unloaded
	//status = plugin.registerUI("chunksExportCreateUI", "chunksExportDeleteUI");
	//if (!status) { cerr << status; lasterror = status; }

	return MS::kSuccess;
}

//======================================================================

__declspec( dllexport ) MStatus uninitializePlugin(MObject obj)
{
	MStatus	status, lasterror = MS::kSuccess;

	MFnPlugin plugin(obj);
	
	status = plugin.deregisterFileTranslator("chunksExport");
	if (!status) { std::cerr << status; lasterror = status; }

	return lasterror;
}

//======================================================================
