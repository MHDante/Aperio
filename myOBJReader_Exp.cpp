// ----------------------- EXPERIMENTAL ---------------------------------------
// ----------------------- EXPERIMENTAL ---------------------------------------
// ----------------------- EXPERIMENTAL ---------------------------------------

/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkOBJReader.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "stdafx.h"		// Precompiled header files
#include "myOBJReader_Exp.h" 

vtkStandardNewMacro(myOBJReader_Exp); 

// Additional includes
#include <vtkCleanPolyData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include "CheckForMemoryLeaks.h"	// MUST be Last include 

/// ---------------------------------------------------------------------------------------------
/// <summary> Instantiate object with NULL filename.
/// </summary>
myOBJReader_Exp::myOBJReader_Exp()
{
	this->FileName = nullptr;

	this->SetNumberOfInputPorts(0);

	this->objectMeshCollection = vtkPolyDataCollection::New();
	this->groupnames.clear();
}

/// ---------------------------------------------------------------------------------------------
/// <summary> Finalizes an instance of the <see cref="myOBJReader_Exp" /> class. (deletes filename pointer)
/// </summary>
myOBJReader_Exp::~myOBJReader_Exp()
{
	if (this->FileName)
	{
		delete [] this->FileName;
		this->FileName = nullptr;
	}
}

/// ---------------------------------------------------------------------------------------------
/// <summary> Gets a poly data collection of all meshes
/// </summary>
/// <returns>vtkPolyDataCollection *.</returns>
vtkPolyDataCollection *myOBJReader_Exp::getPolyDataCollection()
{
	return objectMeshCollection;
}

/*---------------------------------------------------------------------------*\

This is only partial support for the OBJ format, which is quite complicated.
To find a full specification, search the net for "OBJ format", eg.:

http://en.wikipedia.org/wiki/Obj
http://netghost.narod.ru/gff/graphics/summary/waveobj.htm

We support the following types:

v <x> <y> <z>

vertex

vn <x> <y> <z>

vertex normal

vt <x> <y>

texture coordinate

f <v_a> <v_b> <v_c> ...

polygonal face linking vertices v_a, v_b, v_c, etc. which
are 1-based indices into the vertex list

f <v_a>/<t_a> <v_b>/<t_b> ...

polygonal face as above, but with texture coordinates for
each vertex. t_a etc. are 1-based indices into the texture
coordinates list (from the vt lines)

f <v_a>/<t_a>/<n_a> <v_b>/<t_b>/<n_b> ...

polygonal face as above, with a normal at each vertex, as a
1-based index into the normals list (from the vn lines)

f <v_a>//<n_a> <v_b>//<n_b> ...

polygonal face as above but without texture coordinates.

Per-face tcoords and normals are supported by duplicating
the vertices on each face as necessary.

l <v_a> <v_b> ...

lines linking vertices v_a, v_b, etc. which are 1-based
indices into the vertex list

p <v_a> <v_b> ...

points located at the vertices v_a, v_b, etc. which are 1-based
indices into the vertex list

\*---------------------------------------------------------------------------*/


/// <summary> Requests data (overridden method)
/// </summary>
/// <returns>int.</returns>
int myOBJReader_Exp::RequestData(
	vtkInformation *vtkNotUsed(request),
	vtkInformationVector **vtkNotUsed(inputVector),
	vtkInformationVector *outputVector)
{
	// get the info object
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// get the ouptut
	vtkPolyData *output = vtkPolyData::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (!this->FileName)
	{
		vtkErrorMacro(<< "A FileName must be specified.");
		return 0;
	}

	FILE *in = fopen(this->FileName,"r");

	if (in == nullptr)
	{
		vtkErrorMacro(<< "File " << this->FileName << " not found");
		return 0;
	}

	vtkDebugMacro(<<"Reading file");

	// intialise some structures to store the file contents in
	vtkPoints *points = vtkPoints::New();
	vtkFloatArray *tcoords = vtkFloatArray::New();
	tcoords->SetNumberOfComponents(2);
	vtkFloatArray *normals = vtkFloatArray::New();
	normals->SetNumberOfComponents(3);
	vtkCellArray *polys = vtkCellArray::New();
	vtkCellArray *tcoord_polys = vtkCellArray::New();
	vtkCellArray *normal_polys = vtkCellArray::New();

	bool hasTCoords = false;
	bool hasNormals = false;
	bool tcoords_same_as_verts = true;
	bool normals_same_as_verts = true;
	bool everything_ok = true; // (use of this flag avoids early return and associated memory leak)

	int num_g_s = 0;

	// -- work through the file line by line, assigning into the above 7 structures as appropriate --

	{ // (make a local scope section to emphasise that the variables below are only used here)

		const int MAX_LINE = 1024;
		char label[MAX_LINE];
		char prevLabel[MAX_LINE];
		char rawLine[MAX_LINE];
		float xyz[3];

		int lineNr = 0;
		while (everything_ok && fgets(rawLine, MAX_LINE, in) != nullptr)
		{
			lineNr++;
			char *pLine = rawLine;
			char *pEnd = rawLine + strlen(rawLine);

			// find the first non-whitespace character
			while (isspace(*pLine) && pLine < pEnd) { pLine++; }

			// this first non-whitespace is the command
			const char *cmd = pLine;

			// skip over non-whitespace
			while (!isspace(*pLine) && pLine < pEnd) { pLine++; }

			// terminate command
			if (pLine < pEnd)
			{
				*pLine = '\0';
				pLine++;
			}

			// in the OBJ format the first characters determine how to interpret the line:
			if (strcmp(cmd, "v") == 0)
			{
				// this is a vertex definition, expect three floats, separated by whitespace:
				if (sscanf(pLine, "%f %f %f", xyz, xyz+1, xyz+2) == 3)
				{
					points->InsertNextPoint(xyz);
				}
				else
				{
					vtkErrorMacro(<<"Error reading 'v' at line " << lineNr);
					everything_ok = false;
				}
			}
			else if (strcmp(cmd, "vt") == 0)
			{
				// this is a tcoord, expect two floats, separated by whitespace:
				if (sscanf(pLine, "%f %f", xyz, xyz+1) == 2)
				{
					tcoords->InsertNextTuple(xyz);
				}
				else
				{
					vtkErrorMacro(<<"Error reading 'vt' at line " << lineNr);
					everything_ok = false;
				}
			}
			else if (strcmp(cmd, "vn") == 0)
			{
				// this is a normal, expect three floats, separated by whitespace:
				if (sscanf(pLine, "%f %f %f", xyz, xyz+1, xyz+2) == 3)
				{
					normals->InsertNextTuple(xyz);
					hasNormals = true;
				}
				else
				{
					vtkErrorMacro(<<"Error reading 'vn' at line " << lineNr);
					everything_ok = false;
				}
			}
			else if (strcmp(cmd, "g") == 0)
			{
				num_g_s++; 
				// this is the start of a mesh object
				if (sscanf(pLine, "%s\n", label) == 1)
				{
					strcpy(label, pLine);	// to get full groupname with spaces and without newline
					if (label[strlen(label) - 1] == '\n')
						label[strlen(label) - 1] = '\0';

					if (num_g_s == 1)
					{
						strcpy(prevLabel,label);
					}
					else if (num_g_s > 1)
					{
						// start here
						if (everything_ok)   // (otherwise just release allocated memory and return)
						{
							//vtkSmartPointer<vtkCellArray> polys_n = vtkSmartPointer<vtkCellArray>::New();
							//polys_n->DeepCopy(polys);

							//vtkSmartPointer<vtkCellArray> tcoord_polys_n = vtkSmartPointer<vtkCellArray>::New();
							//tcoord_polys_n->DeepCopy(tcoord_polys);

							//vtkSmartPointer<vtkCellArray> normal_polys_n = vtkSmartPointer<vtkCellArray>::New();
							//normal_polys_n->DeepCopy(normal_polys);

							// -- now turn this lot into a useable vtkPolyData --		
							mesh = vtkSmartPointer<vtkPolyData>::New();

							//// otherwise we can duplicate the vertices as necessary (a bit slower)
							vtkDebugMacro(<<"Duplicating vertices so that tcoords and normals are correct");

							vtkSmartPointer<vtkPoints> new_points = vtkSmartPointer<vtkPoints>::New();
							vtkSmartPointer<vtkFloatArray> new_tcoords = vtkSmartPointer<vtkFloatArray>::New();
							new_tcoords->SetNumberOfComponents(2);
							vtkSmartPointer<vtkFloatArray> new_normals = vtkSmartPointer<vtkFloatArray>::New();
							new_normals->SetNumberOfComponents(3);
							vtkSmartPointer<vtkCellArray> new_polys = vtkSmartPointer<vtkCellArray>::New();

							// for each poly, copy its vertices into new_points (and point at them)
							// also copy its tcoords into new_tcoords
							// also copy its normals into new_normals
							polys->InitTraversal();
							tcoord_polys->InitTraversal();
							normal_polys->InitTraversal();

							vtkIdType dummy_warning_prevention_mechanism[1];
							vtkIdType n_pts=-1,*pts=dummy_warning_prevention_mechanism;
							vtkIdType n_tcoord_pts=-1,*tcoord_pts=dummy_warning_prevention_mechanism;
							vtkIdType n_normal_pts=-1,*normal_pts=dummy_warning_prevention_mechanism;
							for (int i=0; i<polys->GetNumberOfCells(); ++i)
							{
								polys->GetNextCell(n_pts,pts);
								tcoord_polys->GetNextCell(n_tcoord_pts,tcoord_pts);
								normal_polys->GetNextCell(n_normal_pts,normal_pts);

								// If some vertices have tcoords and not others (likewise normals)
								// then we must do something else VTK will complain. (crash on render attempt)
								// Easiest solution is to delete polys that don't have complete tcoords (if there
								// are any tcoords in the dataset) or normals (if there are any normals in the dataset).
								if (
									(n_pts != n_tcoord_pts && hasTCoords) ||
									(n_pts != n_normal_pts && hasNormals)
									)
								{
									cout << "skipped";
									// skip this poly
									vtkDebugMacro(<<"Skipping poly "<<i+1<<" (1-based index)");
								}
								else
								{
									// copy the corresponding points, tcoords and normals across
									// copy the corresponding points, tcoords and normals across
									for (int j=0; j<n_pts; ++j)
									{
										// copy the tcoord for this point across (if there is one)
										if (n_tcoord_pts>0)
										{
											new_tcoords->InsertNextTuple(tcoords->GetTuple(tcoord_pts[j])); ///sss
										}
										// copy the normal for this point across (if there is one)
										if (n_normal_pts>0)
										{
											new_normals->InsertNextTuple(normals->GetTuple(normal_pts[j])); /// sss
										}
										// copy the vertex into the new structure and update
										// the vertex index in the polys structure (pts is a pointer into it)
										pts[j] = new_points->InsertNextPoint(points->GetPoint(pts[j]));	//sss
									}
									// copy this poly (pointing at the new points) into the new polys list
									new_polys->InsertNextCell(n_pts,pts);
								}
							}

							// use the new structures for the output
							mesh->SetPoints(new_points);
							mesh->SetPolys(new_polys);
							if (hasTCoords)
							{
								mesh->GetPointData()->SetTCoords(new_tcoords);
							}
							if (hasNormals)
							{
								mesh->GetPointData()->SetNormals(new_normals);
							}

							// TODO: fixup for pointElems and lineElems too
							mesh->Squeeze();

							// Clean mesh - kind of buggy
							//vtkSmartPointer<vtkCleanPolyData> clean = vtkSmartPointer<vtkCleanPolyData>::New();
							//clean->SetInput(mesh);
							//clean->PointMergingOn();
							//clean->Update();
							//mesh = clean->GetOutput();

							//new_points->Delete();
							//new_polys->Delete();
							//new_tcoords->Delete();
							//new_normals->Delete();
						}
						//}
						// Add polydata to the collection as well as group name to groupnames vector
						objectMeshCollection->AddItem(mesh);
						groupnames.push_back(prevLabel);	


						strcpy(prevLabel,label);

						polys->Delete();
						tcoord_polys->Delete();
						normal_polys->Delete();

						polys = vtkCellArray::New();
						tcoord_polys = vtkCellArray::New();
						normal_polys = vtkCellArray::New();

						hasTCoords = false;
						hasNormals = false;
						tcoords_same_as_verts = true;
						normals_same_as_verts = true;
						everything_ok = true; // (use of this flag avoids early 
						// end check
					}
				}
				else
				{
					vtkErrorMacro(<<"Error reading 'g' at line " << lineNr);
					everything_ok = false;
				}
			}
			else if (strcmp(cmd, "f") == 0)
			{
				// this is a face definition, consisting of 1-based indices separated by whitespace and /

				polys->InsertNextCell(0); // we don't yet know how many points are to come
				tcoord_polys->InsertNextCell(0);
				normal_polys->InsertNextCell(0);

				int nVerts=0, nTCoords=0, nNormals=0; // keep a count of how many of each there are

				while (everything_ok && pLine < pEnd)
				{
					// find the first non-whitespace character
					while (isspace(*pLine) && pLine < pEnd) { pLine++; }

					if (pLine < pEnd)         // there is still data left on this line
					{
						int iVert,iTCoord,iNormal;
						if (sscanf(pLine, "%d/%d/%d", &iVert, &iTCoord, &iNormal) == 3)
						{
							polys->InsertCellPoint(iVert-1); // convert to 0-based index
							nVerts++;
							tcoord_polys->InsertCellPoint(iTCoord-1);
							nTCoords++;
							normal_polys->InsertCellPoint(iNormal-1);
							nNormals++;
							if (iTCoord != iVert)
								tcoords_same_as_verts = false;
							if (iNormal != iVert)
								normals_same_as_verts = false;
						}
						else if (sscanf(pLine, "%d//%d", &iVert, &iNormal) == 2)
						{
							polys->InsertCellPoint(iVert-1);
							nVerts++;
							normal_polys->InsertCellPoint(iNormal-1);
							nNormals++;
							if (iNormal != iVert)
								normals_same_as_verts = false;
						}
						else if (sscanf(pLine, "%d/%d", &iVert, &iTCoord) == 2)
						{
							polys->InsertCellPoint(iVert-1);
							nVerts++;
							tcoord_polys->InsertCellPoint(iTCoord-1);
							nTCoords++;
							if (iTCoord != iVert)
								tcoords_same_as_verts = false;
						}
						else if (sscanf(pLine, "%d", &iVert) == 1)
						{
							polys->InsertCellPoint(iVert-1);
							nVerts++;
						}
						else if (strcmp(pLine, "\\\n") == 0)
						{
							// handle backslash-newline continuation
							if (fgets(rawLine, MAX_LINE, in) != nullptr)
							{
								lineNr++;
								pLine = rawLine;
								pEnd = rawLine + strlen(rawLine);
								continue;
							}
							else
							{
								vtkErrorMacro(<<"Error reading continuation line at line " << lineNr);
								everything_ok = false;
							}
						}
						else
						{
							vtkErrorMacro(<<"Error reading 'f' at line " << lineNr);
							everything_ok = false;
						}
						// skip over what we just read
						// (find the first whitespace character)
						while (!isspace(*pLine) && pLine < pEnd) { pLine++; }
					}
				}

				// count of tcoords and normals must be equal to number of vertices or zero
				if ( nVerts < 3 ||
					(nTCoords > 0 && nTCoords != nVerts) ||
					(nNormals > 0 && nNormals != nVerts)
					)
				{
					vtkErrorMacro
						(
						<<"Error reading file near line " << lineNr
						<< " while processing the 'f' command"
						);
					everything_ok = false;
				}

				// now we know how many points there were in this cell
				polys->UpdateCellCount(nVerts);
				tcoord_polys->UpdateCellCount(nTCoords);
				normal_polys->UpdateCellCount(nNormals);

				// also make a note of whether any cells have tcoords, and whether any have normals
				if (nTCoords > 0) { hasTCoords = true; }
				if (nNormals > 0) { hasNormals = true; }
			}
			else
			{
				//vtkDebugMacro(<<"Ignoring line: "<<rawLine);
			}

		} // (end of while loop)

	} // (end of local scope section)

	// we have finished with the file
	fclose(in);
	points->Delete();
	tcoords->Delete();
	normals->Delete();

	return 1;
}

/// ---------------------------------------------------------------------------------------------
/// <summary>
/// Not needed.
/// </summary>
/// <param name="os">output stream.</param>
/// <param name="indent"></param>
void myOBJReader_Exp::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	os << indent << "File Name: "
		<< (this->FileName ? this->FileName : "(none)") << "\n";

}
/// ---------------------------------------------------------------------------------------------
/// <summary> Return group names as a vector of strings. 
/// </summary>
/// <returns>groupnames vector</returns>
std::vector<std::string> myOBJReader_Exp::getGroupNames()
{
	return groupnames;
}

// ************************************************************************* //
