// 
// TetGen / NetGen input-file (.smesh / .stl) Export Plugin for Maya
//

#define REQUIRE_IOSTREAM

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

#include "stlPlugin.h"
#include "tetraPlugin.h"
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
// plug-ins Manager. Change this whenever the translator is modified.
char *pluginVersion = "TETR0.01";
//======================================================================

__declspec( dllexport ) MStatus initializePlugin(MObject obj)
{
	MStatus	status, lasterror = MS::kSuccess;
	
	MFnPlugin plugin(obj, "Tetra Plugin for Maya", pluginVersion);
	
	status = plugin.registerFileTranslator(
		"stlPlugin",
		"stlPlugin.rgb",
		stlPlugin::creator,
		"stlPluginOptions", "",
		true);

	if (!status) { 
	  printf( "error: %s\n", status.errorString( ).asChar( ) );
	  lasterror = status;
	}

	status = plugin.registerFileTranslator(
		"tetraPlugin",
		"tetraPlugin.rgb",
		tetraPlugin::creator,
		"tetraPluginOptions", "",
		true);

	if (!status) { 
	  printf( "error: %s\n", status.errorString( ).asChar( ) );
	  lasterror = status;
	}

	// Set the mel scripts to be run when the plugin is loaded / unloaded
	//status = plugin.registerUI("tetraExportCreateUI", "tetraExportDeleteUI");
	//if (!status) { cerr << status; lasterror = status; }

	return MS::kSuccess;
}

//======================================================================

__declspec( dllexport ) MStatus uninitializePlugin(MObject obj)
{
	MStatus	status, lasterror = MS::kSuccess;

	MFnPlugin plugin(obj);
	status = plugin.deregisterFileTranslator("tetraPlugin");

	if (!status) { 
	  printf( "error: %s\n", status.errorString( ).asChar( ) );
	  lasterror = status;
	}

	status = plugin.deregisterFileTranslator("stlPlugin");

	if (!status) { 
	  printf( "error: %s\n", status.errorString( ).asChar( ) );
	  lasterror = status;
	}

	return lasterror;
}

//======================================================================
