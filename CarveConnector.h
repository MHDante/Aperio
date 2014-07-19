// ***********************************************************************
// Carve Connector - Converts meshes between Carve CSG and VTK
//					 and triangulates, cleans, merges points as well
// ***********************************************************************

#ifndef CARVE_CONNECTOR_H
#define CARVE_CONNECTOR_H

// VTK Includes
#include <vtkCleanPolyData.h>
#include <memory>

using namespace std;

class CarveConnector
{
public:
	CarveConnector();
	~CarveConnector();

	// Public Methods

	//-------------------------------------------------------------------------------------------------------------
	/// <summary> Returns a cube in Carve CSG format (MeshSet)
	/// </summary>
	/// <param name="size">Size of cube</param>
	/// <param name="t">A transformation matrix </param>
	/// <returns>Cube in Carve's MeshSet format</returns>
	static unique_ptr<carve::mesh::MeshSet<3> > makeCube(float size, const carve::math::Matrix &t = carve::math::Matrix());

	//-------------------------------------------------------------------------------------------------------------
	/// <summary> Performs A - B Boolean operation and returns C
	/// </summary>
	/// <param name="a">First Element (second element subtracts from this)</param>
	/// <param name="b">Second Element</param>
	/// <returns>Resulting boolean MeshSet</returns>
	static unique_ptr<carve::mesh::MeshSet<3> > perform(unique_ptr<carve::mesh::MeshSet<3> > &a, unique_ptr<carve::mesh::MeshSet<3> > &b, carve::csg::CSG::OP op);

	//-------------------------------------------------------------------------------------------------------------
	/// <summary> Converts Carve MeshSet to vtkPolyData
	/// </summary>
	/// <param name="mesh">MeshSet in Carve's to convert</param>
	/// <returns>Resulting vtkPolyData (stored in smartpointers)</returns>
	static vtkSmartPointer<vtkPolyData> meshSetToVTKPolyData(unique_ptr<carve::mesh::MeshSet<3> > &c);

	//-------------------------------------------------------------------------------------------------------------
	/// <summary> Converts vtkPolyData to Carve MeshSet 
	/// </summary>
	/// <param name="mesh">vtkPolyData to convert </param>
	/// <returns>Resulting MeshSet in Carve format</returns>
	static unique_ptr<carve::mesh::MeshSet<3> > vtkPolyDataToMeshSet(vtkSmartPointer<vtkPolyData> &thepolydata);

	//-------------------------------------------------------------------------------------------------------------
	/// <summary> Cleans vtkPolyData (triangulate and remove duplicate vertices before giving to Carve)
	/// </summary>
	/// <param name="mesh">vtkPolyData to clean </param>
	/// <returns>Resulting clean vtkPolydata </returns>
	static vtkSmartPointer<vtkPolyData> cleanVtkPolyData(vtkSmartPointer<vtkPolyData> &thepolydata);
};
#endif
