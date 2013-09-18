/*=========================================================================

Program:   Visualization Toolkit
Module:    myOBJReader.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "stdafx.h"		// Precompiled header files
#include "myOBJReader.h"

// Custom
#include "Utility.h"

vtkStandardNewMacro(myOBJReader);

#include "CheckForMemoryLeaks.h"	// MUST be Last include

/// ---------------------------------------------------------------------------------------------
/// <summary> Instantiate object with nullptr filename.
/// </summary>
myOBJReader::myOBJReader()
{
	this->FileName = nullptr;

	this->SetNumberOfInputPorts(0);

	this->objectMeshCollection = vtkPolyDataCollection::New();
	this->groupnames.clear();
}

/// ---------------------------------------------------------------------------------------------
/// <summary> Finalizes an instance of the <see cref="myOBJReader" /> class. (deletes filename pointer)
/// </summary>
myOBJReader::~myOBJReader()
{
	if (this->FileName)
	{
		delete [] this->FileName;
		this->FileName = nullptr;
	}
}

/// ---------------------------------------------------------------------------------------------
vtkPolyDataCollection *myOBJReader::getPolyDataCollection()
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

\*---------------------------------------------------------------------------*/


/// <summary> Requests data (overridden method)
/// </summary>
/// <returns>int.</returns>
int myOBJReader::RequestData(
	vtkInformation *vtkNotUsed(request),
	vtkInformationVector **vtkNotUsed(inputVector),
	vtkInformationVector *outputVector)
{
	// get the info object
	//vtkInformation *outInfo = outputVector->GetInformationObject(0);

	if (!this->FileName)
	{ 
		cout << "Filename must be specified" << endl;
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

	// Intialize some structures to store the file contents in
	points = vtkPoints::New();
	normals = vtkFloatArray::New();
	normals->SetNumberOfComponents(3);
	polys = vtkCellArray::New();
	normal_polys = vtkCellArray::New();

	// -- Custom
	//tcoords = vtkFloatArray::New();
	//tcoords->SetNumberOfComponents(2);
	//tcoord_polys = vtkCellArray::New();

	// -- work through the file line by line, assigning into the above 7 structures as appropriate --

	{ // (make a local scope section to emphasize that the variables below are only used here)

		hasNormals = false;
		//hasTCoords = false;
		bool normals_same_as_verts = true;
		bool tcoords_same_as_verts = true;
		bool everything_ok = true; // (use of this flag avoids early return and associated memory leak)

		int num_g_s = 0; 
		const int MAX_LINE = 1024;
		char rawLine[MAX_LINE];
		float xyz[3];
		char label[MAX_LINE];
		char prevLabel[MAX_LINE];
		//int faceId = -1;
		pointIds = vtkIdList::New();

		new_polys = vtkCellArray::New();
		new_normal_polys = vtkCellArray::New();
		new_points = vtkPoints::New();
		new_normals = vtkFloatArray::New();
		new_normals->SetNumberOfComponents(3);

		//new_tcoords = vtkFloatArray::New();
		//new_tcoords->SetNumberOfComponents(2);
		//new_tcoord_polys = vtkCellArray::New();

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
			//else if (strcmp(cmd, "vt") == 0)
			//{
			//	// this is a tcoord, expect two floats, separated by whitespace:
			//	if (sscanf(pLine, "%f %f", xyz, xyz+1) == 2)
			//	{
			//		tcoords->InsertNextTuple(xyz);
			//		//hasTCoords = true;
			//	}
			//	else
			//	{
			//		vtkErrorMacro(<<"Error reading 'vt' at line " << lineNr);
			//		everything_ok = false;
			//	}
			//}
			else if (strcmp(cmd, "vn") == 0)
			{
				// this is a normal, expect three floats, separated by whitespace:
				if (sscanf(pLine, "%f %f %f", xyz, xyz+1, xyz+2) == 3)
				{
					normals->InsertNextTuple(xyz);
					//hasNormals = true;
				}
				else
				{
					vtkErrorMacro(<<"Error reading 'vn' at line " << lineNr);
					everything_ok = false;
				}
			}
			else if (strcmp(cmd, "mtllib") == 0)
			{
			}
			else if (strcmp(cmd, "usemtl") == 0)
			{
			}
			else if (strcmp(cmd, "s") == 0)
			{
			}
			else if (strcmp(cmd, "g") == 0)
			{
				num_g_s++; 
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
						mesh = vtkPolyData::New();

						new_polys->Delete();
						new_normal_polys->Delete();
						new_points->Delete();

						// -- custom
						//new_tcoord_polys->Delete();

						if (hasNormals)
							new_normals->Delete();

						// if has tcoords? checkkk
						//if (hasTCoords)
						//	new_tcoords->Delete();

						pointIds->Reset();

						CreatePolyData();

						mesh->SetPoints(new_points);
						//						int numP   = new_points->GetNumberOfPoints();
						//						int nCells = new_polys->GetNumberOfCells();

						if (new_polys->GetNumberOfCells())
						{
							mesh->SetPolys(new_polys);
						}

						if (hasNormals && normals_same_as_verts)
						{
							mesh->GetPointData()->SetNormals(new_normals);
						}
						//if (hasTCoords && tcoords_same_as_verts)
						//{
						//	mesh->GetPointData()->SetTCoords(new_tcoords);
						//}
						mesh->Squeeze();
						mesh->BuildLinks();

						// Add polydata to the collection as well as group name to groupnames vector
						objectMeshCollection->AddItem(mesh);
						groupnames.push_back(prevLabel);

						//faceId = -1;
						polys        = vtkCellArray::New();
						normal_polys = vtkCellArray::New();
						//tcoord_polys = vtkCellArray::New();
						hasNormals = false;
						//hasTCoords = false;
						strcpy(prevLabel,label);
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
				//tcoord_polys->InsertNextCell(0);
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
							//tcoord_polys->InsertCellPoint(iTCoord-1);
							//nTCoords++;
							normal_polys->InsertCellPoint(iNormal-1);
							nNormals++;

							//if (iTCoord != iVert)
							//	tcoords_same_as_verts = false;
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
							//tcoord_polys->InsertCellPoint(iTCoord-1);
							//nTCoords++;
							//if (iTCoord != iVert)
							//	tcoords_same_as_verts = false;
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
				} // end face "f" line processing  

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
				//tcoord_polys->UpdateCellCount(nTCoords);
				normal_polys->UpdateCellCount(nNormals);

				// also make a note of whether any cells have tcoords, and whether any have normals
				//if (nTCoords > 0) { hasTCoords = true; }
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
	normals->Delete();
	//tcoords->Delete();
	polys->Delete();
	normal_polys->Delete();
	//tcoord_polys->Delete();

	return 1;
}
/// ---------------------------------------------------------------------------------------------
/// <summary>
/// Creates vtkPolyData for the current mesh (using new_polys, new_tcoords, new_normals, etc.)
/// Doesn't return anything, just a setter method (no need to call)
/// </summary>
void myOBJReader::CreatePolyData()
{
	vtkDebugMacro(<<"Duplicating vertices so that tcoords and normals are correct");

	int numPoints = this->points->GetNumberOfPoints();
	//	int numNormals = this->normals->GetNumberOfTuples();

	pointIds->SetNumberOfIds(numPoints);
	//	int numIds = pointIds->GetNumberOfIds();
	for (int i = 0; i < numPoints; i++)
		pointIds->InsertId(i,-1);

	new_points = vtkPoints::New();
	new_normals = vtkFloatArray::New();
	new_normals->SetNumberOfComponents(3);

	//new_tcoords = vtkFloatArray::New();
	//new_tcoords->SetNumberOfComponents(2);

	new_polys = vtkCellArray::New();
	new_normal_polys = vtkCellArray::New();
	//new_tcoord_polys = vtkCellArray::New();

	// for each poly, copy its vertices into new_points (and point at them)
	// also copy its tcoords into new_tcoords
	// also copy its normals into new_normals
	polys->InitTraversal();

	vtkIdType dummy_warning_prevention_mechanism[1];
	vtkIdType n_pts=-1,*pts=dummy_warning_prevention_mechanism;
	vtkIdType n_tcoord_pts=-1,*tcoord_pts=dummy_warning_prevention_mechanism;
	vtkIdType n_normal_pts=-1,*normal_pts=dummy_warning_prevention_mechanism;

	//	int nc = polys->GetNumberOfCells();

	for (int i=0; i<polys->GetNumberOfCells(); ++i)
	{
		polys->GetNextCell(n_pts,pts);

		// If some vertices have tcoords and not others (likewise normals)
		// then we must do something else VTK will complain. (crash on render attempt)
		// Easiest solution is to delete polys that don't have complete tcoords (if there
		// are any tcoords in the dataset) or normals (if there are any normals in the dataset).

		// copy the corresponding points, tcoords and normals across
		for (int j=0; j<n_pts; ++j)
		{
			// copy the vertex into the new structure and update
			// the vertex index in the polys structure (pts is a pointer into it)
			if (pointIds->GetId(pts[j]) == -1)
			{
				// copy the tcoord for this point across (if there is one)
				int id = pts[j];
				pts[j] = new_points->InsertNextPoint(points->GetPoint(pts[j]));
				pointIds->InsertId(id,pts[j]);
			}
			else
			{  
				pts[j] = pointIds->GetId(pts[j]);
			}
		}
		// copy this poly (pointing at the new points) into the new polys list
		new_polys->InsertNextCell(n_pts,pts);
	}

	//	int n = new_points->GetNumberOfPoints();

	//doTextures:
	//	if (!hasTCoords)
	//		return;
	//
	//	if (hasTCoords)
	//		tcoord_polys->InitTraversal();
	//
	//	numPoints = this->tcoords->GetNumberOfTuples();
	//	pointIds->Reset();
	//	pointIds->SetNumberOfIds(numPoints);
	//
	//	for (int i = 0; i < numPoints; i++)
	//		pointIds->InsertId(i,-1);
	//
	//	for (int i=0; i< tcoord_polys->GetNumberOfCells(); ++i)
	//	{
	//		tcoord_polys->GetNextCell(n_tcoord_pts,tcoord_pts);
	//
	//		// copy the corresponding normal across
	//		for (int j=0; j<n_tcoord_pts; ++j)
	//		{
	//			if (pointIds->GetId(tcoord_pts[j]) == -1)
	//			{
	//				// copy the normal for this point across (if there is one)
	//				int id = tcoord_pts[j];
	//				tcoord_pts[j] = new_tcoords->InsertNextTuple(tcoords->GetTuple(tcoord_pts[j]));
	//				pointIds->InsertId(id,tcoord_pts[j]);	
	//			}
	//			else
	//			{  
	//				tcoord_pts[j] = pointIds->GetId(tcoord_pts[j]);
	//			}
	//		}
	//		// copy this normal poly (pointing at the new normals) into the new normal_polys list
	//		new_tcoord_polys->InsertNextCell(n_tcoord_pts,tcoord_pts);
	//	}

doNormals:
	if (!hasNormals)
		return;

	if (hasNormals)
		normal_polys->InitTraversal();

	numPoints = this->normals->GetNumberOfTuples();
	pointIds->Reset();
	pointIds->SetNumberOfIds(numPoints);

	for (int i = 0; i < numPoints; i++)
		pointIds->InsertId(i,-1);

	for (int i=0; i< normal_polys->GetNumberOfCells(); ++i)
	{
		normal_polys->GetNextCell(n_normal_pts,normal_pts);

		// copy the corresponding normal across
		for (int j=0; j<n_normal_pts; ++j)
		{
			if (pointIds->GetId(normal_pts[j]) == -1)
			{
				// copy the normal for this point across (if there is one)
				int id = normal_pts[j];
				normal_pts[j] = new_normals->InsertNextTuple(normals->GetTuple(normal_pts[j]));
				pointIds->InsertId(id,normal_pts[j]);	
			}
			else
			{  
				normal_pts[j] = pointIds->GetId(normal_pts[j]);
			}
		}
		// copy this normal poly (pointing at the new normals) into the new normal_polys list
		new_normal_polys->InsertNextCell(n_normal_pts,normal_pts);
	}
}

/// ---------------------------------------------------------------------------------------------
/// <summary> Prints self as well as filename (no need to call) 
/// </summary>
/// <param name="os">output stream</param>
/// <param name="indent">vtkIndent object</param>
void myOBJReader::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	os << indent << "File Name: "
		<< (this->FileName ? this->FileName : "(none)") << "\n";

}
/// ---------------------------------------------------------------------------------------------
std::vector<std::string> myOBJReader::getGroupNames()
{
	return groupnames;
}

