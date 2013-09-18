// ***********************************************************************
// Assembly         : additive
// Author           : David Tran
// Created          : 03-10-2013
//
// Last Modified By : David Tran
// Last Modified On : 03-10-2013
// ***********************************************************************
// <copyright file="myOBJReader.h" company="">
//     Copyright (c) . All rights reserved.
// </copyright>
// <summary>Modified vtkOBJReader allowing groups to be read (no texture coords) </summary>
// ***********************************************************************

/*=========================================================================

Program:   Visualization Toolkit
Module:    myOBJReader.h

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME myOBJReader - read Wavefront .obj files
// .SECTION Description
// myOBJReader is a source object that reads Wavefront .obj
// files. The output of this source object is polygonal data.
// .SECTION See Also
// vtkOBJImporter

#ifndef __myOBJReader_h
#define __myOBJReader_h

/// <summary>
/// Modified version of vtkOBJReader that reads in .OBJ files (added feature: reads groups, removed: textures)
/// </summary>
class VTK_EXPORT myOBJReader : public vtkPolyDataAlgorithm 
{
	/// ---------------------------------------------------------------------------------------------
public:

	/// <summary> Creates a new instance of this reader </summary>
	/// <returns>myOBJReader *.</returns>
	static myOBJReader *New();

	vtkTypeMacro(myOBJReader,vtkPolyDataAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);
	void CreatePolyData();
	std::vector<std::string> getGroupNames();

	vtkSetStringMacro(FileName);
	vtkGetStringMacro(FileName);

	/// <summary> The object mesh collection (a vtkPolyDataCollection of all meshes)</summary>
	vtkPolyDataCollection *objectMeshCollection;

	/// <summary> Gets a poly data collection of all meshes </summary>
	/// <returns>vtkPolyDataCollection *.</returns>
	vtkPolyDataCollection *getPolyDataCollection();

	// --- Custom newly added data ----

	/// <summary> Return group names as a vector of strings. </summary>
	/// <returns>groupnames vector</returns>
	std::vector<std::string> groupnames;

	/// ---------------------------------------------------------------------------------------------
protected:
	myOBJReader();
	~myOBJReader();

	int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

	/// <summary> .OBJ filename </summary>
	char *FileName;

	vtkPoints     *points;
	vtkIdList     *pointIds;
	vtkFloatArray *normals;
	vtkCellArray  *polys;
	vtkCellArray  *normal_polys;
	bool hasNormals;
	//bool hasTCoords;
	vtkPoints     *new_points;
	vtkFloatArray *new_normals;
	vtkCellArray  *new_polys;
	vtkCellArray  *new_normal_polys;

	// -- custom - trying to add texture coords
	//vtkFloatArray *tcoords;
	//vtkCellArray * tcoord_polys;
	//vtkFloatArray *new_tcoords;
	//vtkCellArray * new_tcoord_polys;

	/// <summary>
	/// The mesh as a vtkPolyData (contains points, polygonal faces, textures, normals)
	/// </summary>
	vtkPolyData   *mesh;

	/// ---------------------------------------------------------------------------------------------
private:
	/// <summary>
	/// Initializes a new instance of the <see cref="myOBJReader"/> class. (Not implemented)
	/// </summary>
	/// <param name="">The object</param>
	myOBJReader(const myOBJReader&);  // Not implemented.
	/// <summary>
	/// Operator = (Not implemented)
	/// </summary>
	/// <param name="">The object</param>
	void operator=(const myOBJReader&);  // Not implemented.
};

#endif
