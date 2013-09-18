// ----------------------- EXPERIMENTAL ---------------------------------------
// ----------------------- EXPERIMENTAL ---------------------------------------
// ----------------------- EXPERIMENTAL ---------------------------------------
// Loads texture coordinates and normals but duplicates them and is slower
// vtkCleanPolyData used to clean duplicate vertices but messes up texture
// and normals. (So turned off) Therefore, use only if speed not a problem

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
// <summary>Modified vtkOBJReader (experimental) allowing groups to be read AND texture coords 
// (but duplicates a lot of vertices; very slow and not useful for carving)</summary>
// ***********************************************************************

/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkOBJReader.h

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkOBJReader - read Wavefront .obj files
// .SECTION Description
// vtkOBJReader is a source object that reads Wavefront .obj
// files. The output of this source object is polygonal data.
// .SECTION See Also
// vtkOBJImporter

#ifndef __myOBJReader_Exp_h
#define __myOBJReader_Exp_h 

/// <summary>
/// Modified version of vtkOBJReader that reads in .OBJ files (added feature: reads groups, removed: textures)
/// </summary>
class VTK_EXPORT myOBJReader_Exp : public vtkPolyDataAlgorithm 
{
	/// ---------------------------------------------------------------------------------------------
public:

	/// <summary> Creates a new instance of this reader </summary>
	/// <returns>myOBJReader *.</returns>
	static myOBJReader_Exp *New();

	vtkTypeMacro(myOBJReader_Exp,vtkPolyDataAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);

	std::vector<std::string> getGroupNames();

	/// <summary> The object mesh collection (a vtkPolyDataCollection of all meshes)</summary>
	vtkPolyDataCollection *objectMeshCollection;
	vtkPolyDataCollection *getPolyDataCollection();

	// --- Custom newly added data ----
	/// <summary> Vector of strings containing all group names (order following objectMeshCollection) </summary>
	std::vector<std::string> groupnames;

	// Description:
	// Specify file name of Wavefront .obj file.
	vtkSetStringMacro(FileName);
	vtkGetStringMacro(FileName);

	/// ---------------------------------------------------------------------------------------------
protected:
	myOBJReader_Exp();
	~myOBJReader_Exp();

	int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

	char *FileName;

	/// <summary>
	/// The mesh as a vtkPolyData (contains points, polygonal faces, textures, normals)
	/// </summary>
	vtkSmartPointer<vtkPolyData> mesh;

	/// ---------------------------------------------------------------------------------------------
private:
	/// <summary>
	/// Initializes a new instance of the <see cref="myOBJReader"/> class. (Not implemented)
	/// </summary>
	/// <param name="">The object</param>
	myOBJReader_Exp(const myOBJReader_Exp&);  // Not implemented.
	/// <summary>
	/// Operator = (Not implemented)
	/// </summary>
	/// <param name="">The object</param>
	void operator=(const myOBJReader_Exp&);  // Not implemented.
};

#endif
