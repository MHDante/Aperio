// ***********************************************************************
// Utility Functions - Contains general utility functions (conversions, etc.)
//					 also contains common VTK actions like convert sources
//					 to vtkActors, and build & return shader programs
// ***********************************************************************

#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <stdarg.h>
#include <iostream>
#include <sstream>

class aperio;	// Forward declarations
class CustomMesh;

/// <summary>
/// Utility namespace: contains general utility functions
/// </summary>
namespace Utility
{
	/// Functions ---------------------------------------------------------------------------------

	/// <summary>
	/// Adds a clock to the map of clocks, linking clockname->clock_t
	/// and prints the start time of the clock
	/// </summary>
	/// <param name="clockname">The clockname.</param>
	void start_clock(char clockname);

	/// <summary> Ends the specified clock.
	/// </summary>
	/// <param name="clockname">The clock's name</param>
	void end_clock(char clockname);

	/// <summary> Display a windows Information MessageBox </summary>
	/// <param name="text">The message</param>
	void messagebox(std::string text);

	/// <summary> VTK method to convert source/filter to vtkActor (Makes mapper and actor) </summary>
	/// <param name="source">Source/Filter to convert and r,g,b,a (colour and opacity)</param>
	/// <return name="source">Resulting Actor</param>
	vtkSmartPointer<vtkActor> sourceToActor(aperio *ap, vtkSmartPointer<vtkPolyData> source, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

	/// <summary> Method to convert tinyobj format to vtkPolyData </summary>
	/// <param name="source">Converts tinyobj to vtkPolydata </param>
	/// <return name="source">Resulting vtkPolyData</param>
	vtkSmartPointer<vtkPolyData> assimpOBJToVtkPolyData(aiMesh *mesh);

	/// <summary> VTK method to generate Texture coordinates and attach to vtkPolyData's point data </summary>
	/// <param name="source">Polydata to generate texture coordinates for</param>
	void generateTexCoords(vtkSmartPointer<vtkPolyData> source);

	/// <summary> VTK method to make vertex and fragment shader and add to vtkShaderProgram2 object </summary>
	/// <param name="vertex_and_frag">vertex and fragment shader filenames (nullstring is default, meaning none)</param>
	//vtkSmartPointer<vtkShaderProgram2> buildShader(vtkRenderWindow *context, std::string vert = "", std::string frag = "");
	vtkSmartPointer<vtkShaderProgram2> makeShader(vtkRenderWindow *context, std::string vert, std::string frag);
	
	/// <summary> VTK method to reload new vertex (0) or fragment shader (1) files in vtkShaderProgram2 object </summary>
	/// <param name="vertex_and_frag">vertex and fragment shader filenames (nullstring is default, meaning none, so no reloads)</param>
	void updateShader(vtkShaderProgram2* shaderProgram, std::string vert = "", std::string frag = "");

	vtkSmartPointer<vtkPolyData> smoothNormals(vtkSmartPointer<vtkPolyData> source);

	/// <summary> Add to meshes collection (returns the CustomMesh) </summary>
	/// <param name="vertex_and_frag"></param>
	CustomMesh& addMesh(aperio *a, vtkSmartPointer<vtkPolyData> source, int z, std::string groupname, vtkColor3f color, float opacity);

	///-------------------------------------------------------------------------
	/// <summary> Compute normals for a vtkPolyData source
	/// </summary>
	vtkSmartPointer<vtkPolyData> computeNormals(vtkSmartPointer<vtkPolyData> source);

	/// ----------------------------------------------------------------------------
	/// <summary> Compute bounding boxes for an assimp scene (call get_bounding_box)
	/// </summary>
	void get_bounding_box_for_node(const aiScene* scene, const aiNode* nd, aiVector3D* min, aiVector3D* max);
	void get_bounding_box(const aiScene* scene, aiVector3D* min, aiVector3D* max);
}
#endif
