#ifndef CARVE_CONNECTOR_H
#define CARVE_CONNECTOR_H

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
	static unique_ptr<carve::mesh::MeshSet<3> > performDifference(unique_ptr<carve::mesh::MeshSet<3> > &a, unique_ptr<carve::mesh::MeshSet<3> > &b);

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
};

#endif
