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
	void messagebox(string text);

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
	//vtkSmartPointer<vtkShaderProgram2> buildShader(vtkRenderWindow *context, string vert = "", string frag = "");
	vtkSmartPointer<vtkShaderProgram2> makeShader(vtkRenderWindow *context, string vert, string frag);
	
	/// <summary> VTK method to reload new vertex (0) or fragment shader (1) files in vtkShaderProgram2 object </summary>
	/// <param name="vertex_and_frag">vertex and fragment shader filenames (nullstring is default, meaning none, so no reloads)</param>
	void updateShader(vtkShaderProgram2* shaderProgram, string vert = "", string frag = "");

	/// <summary> Add to meshes collection (returns the CustomMesh) </summary>
	/// <param name="vertex_and_frag"></param>
	weak_ptr<CustomMesh> addMesh(aperio *a, vtkSmartPointer<vtkPolyData> source, string groupname, vtkColor3f color = vtkColor3f(1, 1, 1), float opacity = 1.0, shared_ptr<CustomMesh> parentMesh = nullptr, shared_ptr<CustomMesh> oldMesh = nullptr);
	
	/// <summary> Remove mesh from collection </summary>
	void removeMesh(aperio *a, weak_ptr<CustomMesh> mesh);

	/// <summary> Set mesh opacity (along with its CustomMesh property) </summary>
	void setMeshOpacity(aperio *a, weak_ptr<CustomMesh> mesh, float opacity = 1.0);
	void setMeshOpacitySeparate(aperio *a, weak_ptr<CustomMesh> mesh, float opacity, float actorOpacity);

	/// <summary> For Debug Use (plot sphere) </summary>
	vtkSmartPointer<vtkActor> plotSphere(aperio *a, float x, float y, float z, float size = 1.0, float r = 1, float g = 1, float b = 1);

	/// <summary> For Debug Use (plot line as tube) </summary>
	vtkSmartPointer<vtkActor> plotLine(aperio *a, vtkVector3f p1, vtkVector3f p2, float radius = 0.1, float r = 1, float g = 1, float b = 1);

	/// <summary> Generates and returns a cell array containing connected lines from input points </summary>
	vtkSmartPointer<vtkCellArray> generateLinesFromPoints(vtkSmartPointer<vtkPoints> points);

	///-------------------------------------------------------------------------
	/// <summary> Compute normals for a vtkPolyData source
	/// </summary>
	vtkSmartPointer<vtkPolyData> computeNormals(vtkSmartPointer<vtkPolyData> source);


	///-------------------------------------------------------------------------
	/// <summary> Get Image Data from .jpg or .png file
	/// </summary>
	vtkSmartPointer<vtkImageData> getImageData(string filename);

	/// ----------------------------------------------------------------------------
	/// <summary> Compute bounding boxes for an assimp scene (call get_bounding_box)
	/// </summary>
	void get_bounding_box_for_node(const aiScene* scene, const aiNode* nd, aiVector3D* min, aiVector3D* max);
	void get_bounding_box(const aiScene* scene, aiVector3D* min, aiVector3D* max);
}
#endif
